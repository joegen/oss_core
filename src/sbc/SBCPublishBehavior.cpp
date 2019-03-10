

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


#include "OSS/SIP/SBC/SBCPublishBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"


namespace OSS {
namespace SIP {
namespace SBC {

using OSS::SIP::SIPMessage;

SBCPublishBehavior::SBCPublishBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_PUBLISH, "SBC PUBLISH Request Handler")
{
  setName("SBC PUBLISH Request Handler");
}

SBCPublishBehavior::~SBCPublishBehavior()
{
}

void SBCPublishBehavior::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
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

SIPMessage::Ptr SBCPublishBehavior::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{  
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


} } } // OSS::SIP::SBC


