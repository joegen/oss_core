

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


#include "OSS/SIP/SBC/SBCUpdateBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SDP/SDPSession.h"


namespace OSS {
namespace SIP {
namespace SBC {

using OSS::SIP::SIPMessage;

SBCUpdateBehavior::SBCUpdateBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_UPDATE, "SBC UPDATE Request Handler")
{
  setName("SBC UPDATE Request Handler");
}

SBCUpdateBehavior::~SBCUpdateBehavior()
{
}

SIPMessage::Ptr SBCUpdateBehavior::onRouteTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target)
{
  //
  // Call the default behavior first so that states (like session-id and local-update) are properly populated
  //
  SIPMessage::Ptr pResponse = SBCDefaultBehavior::onRouteTransaction(pRequest, pTransaction, localInterface, target);
  
  std::string localUpdate;
  if (!pTransaction->getProperty("local-update", localUpdate) || localUpdate != "1")
  {
    return pResponse;
  }
  
  //
  // update needs to be handled locally
  //
  std::string sessionId;
  std::string sdp1;
  std::string sdp2; 
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
  
  OSS_LOG_INFO(pTransaction->getLogId() << "Processing UPDATE locally for session " << sessionId);
  
  const std::string& sdp0 = pRequest->body();
  if (sdp0.empty())
  {
    //
    // Update has no SDP.  It is merely a target refresh
    //
    return pTransaction->serverRequest()->createResponse(SIPMessage::CODE_200_Ok);
  }
  
  //
  // Fetch the current local SDP from the RTP proxy
  //
#if 0
  _pManager->rtpProxy().getSDP(sessionId, sdp1, sdp2);
#else
  OSS::Persistent::ClassType dialog;
  if (_pManager->dialogStateManager().findDialog(pTransaction, pTransaction->serverRequest(), dialog))
  {
    OSS::Persistent::DataType root = dialog.self();
    
    if (root.exists("leg-1") && root.exists("leg-2"))
    {
      OSS::Persistent::DataType leg1 = root["leg-1"];
      OSS::Persistent::DataType leg2 = root["leg-2"];
      
      if (leg1.exists("local-sdp") && leg2.exists("local-sdp"))
      {
        sdp1 = (const char*)leg1["local-sdp"];
        sdp2 = (const char*)leg2["local-sdp"];
      }
    }
  }
  else
  {
    OSS_LOG_ERROR(pTransaction->getLogId() << "Unable to find dialog for local UPDATE for session " << sessionId);
  }
#endif
  
  if (sdp1.empty() || sdp2.empty())
  {
    return pTransaction->serverRequest()->createResponse(SIPMessage::CODE_500_InternalServerError, "Unable To Generate Media Description");
  }
  
  std::string noop;
  std::string sid0;
  OSS::SDP::SDPSession sdp0_(sdp0.c_str());
  sdp0_.getOrigin(noop, sid0, noop, noop, noop, noop);
  
  std::string sid1;
  OSS::SDP::SDPSession sdp1_(sdp1.c_str());
  sdp1_.getOrigin(noop, sid1, noop, noop, noop, noop);
  
  std::string sid2;
  OSS::SDP::SDPSession sdp2_(sdp2.c_str());
  sdp2_.getOrigin(noop, sid2, noop, noop, noop, noop);
  
  if (sid0 == sid1)
  {
    SIPMessage::Ptr pAnswer = pTransaction->serverRequest()->createResponse(SIPMessage::CODE_200_Ok);
    pAnswer->hdrSet(SIP::HDR_CONTENT_TYPE, "application/sdp");
    pAnswer->hdrSet(SIP::HDR_CONTENT_LENGTH, OSS::string_from_number<std::size_t>(sdp2.size()));
    pAnswer->setBody(sdp2);
    pAnswer->commitData();
    return pAnswer;
  }
  
  if (sid0 == sid2)
  {
    SIPMessage::Ptr pAnswer = pTransaction->serverRequest()->createResponse(SIPMessage::CODE_200_Ok);
    pAnswer->hdrSet(SIP::HDR_CONTENT_TYPE, "application/sdp");
    pAnswer->hdrSet(SIP::HDR_CONTENT_LENGTH, OSS::string_from_number<std::size_t>(sdp1.size()));
    pAnswer->setBody(sdp1);
    pAnswer->commitData();
    return pAnswer;
  }
  
  return pTransaction->serverRequest()->createResponse(SIPMessage::CODE_488_NotAcceptableHere, "Invalid Session-ID in SDP");
}

void SBCUpdateBehavior::onProcessResponseOutbound(
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

} } } // OSS::SIP::SBC


