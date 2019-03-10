

// OSS Software Solutions Application Programmer Interface
// Package: SBC
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/SIP/SBC/SBCOptionsBehavior.h"
#include "OSS/SIP/SBC/SBCRegisterBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"


namespace OSS {
namespace SIP {
namespace SBC {

using OSS::SIP::SIPMessage;

SBCOptionsBehavior::SBCOptionsBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_OPTIONS, "SBC OPTIONS Request Handler")
{
  setName("SBC OPTIONS Request Handler");
}

SBCOptionsBehavior::~SBCOptionsBehavior()
{
}

SIPMessage::Ptr SBCOptionsBehavior::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  SIPFrom to(pRequest->hdrGet("to"));
  std::string aor = to.getAor();
  if (_pManager->isOptionsRoutingEnabled() && _pManager->isTrunkRegistration(aor))
  {
    SIPMessage::Ptr ok = pRequest->createResponse(SIPMessage::CODE_200_Ok);
    return ok;
  }
  else if (!_pManager->isOptionsRoutingEnabled())
  {
    SIPMessage::Ptr bannedResponse = _pManager->autoBanRules().processBanRules(pRequest, pTransaction);
    if (bannedResponse)
    {
      return bannedResponse;
    }
    //
    // If this is a registered endpoint and is no longer found in the DB
    // send a 404
    //
    if (SBCContact::isRegisterRoute(pRequest))
    {      
      SIPMessage::Ptr ret = onRouteUpperReg(pRequest, pTransaction, localInterface, target);
      if (ret)
      {
        return ret;
      }
    }
    
    SIPMessage::Ptr ok = pRequest->createResponse(SIPMessage::CODE_200_Ok);
    return ok;
  }
  
  SIPMessage::Ptr ret = SBCDefaultBehavior::onRouteTransaction(pRequest, pTransaction, localInterface, target);
  if (ret)
  {
    return ret;
  }
  
  pRequest->hdrListRemove("Route");  
  pRequest->hdrListRemove("Record-Route");
  pRequest->hdrListRemove("Via");

  //
  // Prepare the new via
  //
  std::string targetTransport;
  if (!pRequest->getProperty(OSS::PropertyMap::PROP_TargetTransport, targetTransport) || targetTransport.empty())
    targetTransport = "udp";
  OSS::string_to_upper(targetTransport);
  std::string viaBranch = "z9hG4bK";
  viaBranch += OSS::string_create_uuid();
  std::string newVia = SBCContact::constructVia(_pManager, pRequest, localInterface, targetTransport, viaBranch);
  pRequest->hdrListPrepend("Via", newVia);

  return ret;
}

void SBCOptionsBehavior::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
  /// This is the last chance for the application to process
  /// the outbound response before it gets sent out to the transport.
  ///
  /// This is normally the place where application would want to
  /// insert application-specific headers as well as change existing
  /// headers to the desired application-specific values for as long
  /// as it wont conflict with dialog creation states.
{
  SBCDefaultBehavior::onProcessResponseOutbound(pResponse, pTransaction);

  std::string sessionId;
  std::string legIndex;

  pTransaction->getProperty("session-id", sessionId);
  pTransaction->getProperty("leg-index", legIndex);

  if (!pResponse->is1xx(100))
  {
    SBCContact::SessionInfo sessionInfo;
    sessionInfo.sessionId = sessionId;

    if (!legIndex.empty())
      sessionInfo.callIndex = OSS::string_to_number<unsigned>(legIndex.c_str());

    SBCContact::transform(this->getManager(),
      pResponse,
      pTransaction,
      pTransaction->serverTransport()->getLocalAddress(),
      sessionInfo);
  }
}

void SBCOptionsBehavior::onTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,      
  SIPB2BTransaction::Ptr pTransaction)
  /// Signals that an error occured on the transaction
  ///
  /// The transaction will be destroyed automatically after this function call
{
  SBCDefaultBehavior::onTransactionError(e, pErrorResponse, pTransaction);
  if (pErrorResponse && pErrorResponse->is4xx(SIPMessage::CODE_408_RequestTimeout))
  {
    std::string regId;
    if (pTransaction && pTransaction->getProperty("Registration-Id", regId))
    {
      _pManager->registerHandler()->deleteUpperRegistration(regId);
    }
  }
}

} } } // OSS::SIP::SBC


