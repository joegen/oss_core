

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


#include "OSS/SIP/SBC/SBCNotifyBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SIPVia.h"


namespace OSS {
namespace SIP {
namespace SBC {

  
using OSS::SIP::SIPMessage;

SBCNotifyBehavior::SBCNotifyBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_NOTIFY, "SBC NOTIFY Request Handler"),
  _pSubscribeBehavior(0)
{
  setName("SBC NOTIFY Request Handler");
}

SBCNotifyBehavior::~SBCNotifyBehavior()
{
}

SIPMessage::Ptr SBCNotifyBehavior::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  if (!pRequest->isMidDialog())
  {
    SIPMessage::Ptr routeResponse;
    routeResponse = SBCDefaultBehavior::onRouteTransaction(pRequest, pTransaction, localInterface, target);
    if (routeResponse)
      return routeResponse;

    //
    // Check for the existence of a branch parameter
    //
    std::string oldVia;
    SIPVia::msgGetTopVia(pRequest.get(), oldVia);
    std::string branch;
    SIPVia::getBranch(oldVia, branch);
    if (branch.empty())
    {
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Missing Via Branch Parameter");
      return serverError;
    }
    //
    // Prepare the SIP Message for outbound
    //
    pRequest->hdrListRemove("Route");
    pRequest->hdrListRemove("Record-Route");
    pRequest->hdrListRemove("Via");
    pRequest->hdrListRemove("Contact");

    //
    // Set the target tranport
    //
    std::string targetTransport;
    if (!pRequest->getProperty(OSS::PropertyMap::PROP_TargetTransport, targetTransport) || targetTransport.empty())
    {
      targetTransport = "udp";
      pRequest->setProperty(OSS::PropertyMap::PROP_TargetTransport, "udp");
    }

    //
    // Prepare the new via
    //
    OSS::string_to_upper(targetTransport);
    std::ostringstream viaBranch;
    viaBranch <<  "z9hG4bK" << OSS::string_hash(branch.c_str());
    std::string newVia = SBCContact::constructVia(_pManager, pRequest, localInterface, targetTransport, viaBranch.str());
    pRequest->hdrListPrepend("Via", newVia);
    //
    // Prepare the new contact
    //
    SBCContact::SessionInfo sessionInfo;
    SBCContact::transform(this->getManager(),
      pRequest,
      pTransaction,
      localInterface,
      sessionInfo);

    return SIPMessage::Ptr();
  }

  std::string event = pRequest->hdrGet("event");
  if (event.empty())
  {

    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Bad Event Header");
    return serverError;
  }
  OSS::string_to_lower(event);
  if (event.find("refer") != std::string::npos)
  {
    //
    // This is a NOTIFY due to a REFER.
    //
    return SBCDefaultBehavior::onRouteTransaction(pRequest, pTransaction, localInterface, target);
  }

  //
  // Remove all route headers prior to routing
  //
  pRequest->hdrListRemove("route");
  return _pSubscribeBehavior->onRouteNotify(pRequest, pTransaction, localInterface, target);
}

void SBCNotifyBehavior::onProcessResponseOutbound(
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

} } } // OSS::SIP::SBC


