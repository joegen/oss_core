

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


#include "OSS/SIP/SBC/SBCByeBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {
namespace SBC {


using OSS::SIP::SIPMessage;
using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;

SBCByeBehavior::SBCByeBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_BYE, "SBC BYE Request Handler")
{
  setName("SBC BYE Request Handler");
}

SBCByeBehavior::~SBCByeBehavior()
{
}

SIPMessage::Ptr SBCByeBehavior::onTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  if (!pRequest->isMidDialog())
  {
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest);
    return serverError;
  }
  return SBCDefaultBehavior::onTransactionCreated(pRequest, pTransaction);
}

void SBCByeBehavior::onProcessOutbound(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  SBCDefaultBehavior::onProcessOutbound(pRequest, pTransaction);

  std::string sessionId;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
  //
  // Remove the rtp proxies if they were created
  //
  try
  {
    _pManager->rtpProxy().removeSession(sessionId);
    _pManager->removeCallStartTime(pRequest->hdrGet(OSS::SIP::HDR_CALL_ID));
  }catch(...){}
  
  //
  // Inform the CDR Manager about the disconnect event
  //
  pTransaction->serverTransaction()->setProperty("session-id", sessionId);
  getManager()->cdr().onCallTerminated(pRequest, pTransaction->serverTransaction());

}

void SBCByeBehavior::onProcessResponseOutbound(
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
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
  std::string legIndex;
  OSS_VERIFY(pTransaction->getProperty("leg-index", legIndex));
  if (!pResponse->is1xx(100))
  {
    SBCContact::SessionInfo sessionInfo;
    sessionInfo.sessionId = sessionId;
    sessionInfo.callIndex = OSS::string_to_number<unsigned>(legIndex.c_str());
    SBCContact::transform(this->getManager(),
      pResponse,
      pTransaction,
      pTransaction->serverTransport()->getLocalAddress(),
      sessionInfo);
  }

  if (pResponse->is2xx())
  {
    //
    // Remove the state files if they were created
    //
    try
    {
      _pManager->dialogStateManager().removeDialog(pTransaction, true);
    }catch(...){}
  }
}

void SBCByeBehavior::onTransactionError(
    OSS::SIP::SIPTransaction::Error e,
    SIPMessage::Ptr pErrorResponse,
    SIPB2BTransaction::Ptr pTransaction)
{
  std::string sessionId;
  if (!pTransaction->getProperty("session-id", sessionId) || sessionId.empty())
  {
      SBCDefaultBehavior::onTransactionError(e, pErrorResponse, pTransaction);
      return;
  }

  try
  {
    boost::filesystem::path stateFile = operator/(_pManager->getDialogStateDirectory(), sessionId);
    if (e && !pErrorResponse)
    {
      //
      // This is most probably due to a timeout
      // Fix this and switch the actions based on the actual
      // error instead
      //
      std::string callId = pTransaction->serverRequest()->hdrGet("call-id");
      OSS_LOG_DEBUG(pTransaction->getLogId() << "BYE Transaction Exception: " << e->message() );
      OSS_LOG_DEBUG(pTransaction->getLogId()
        << "Destroying state file " << stateFile << " with Call-ID " << callId);
      _pManager->dialogStateManager().removeDialog(pTransaction, true);

      OSS_LOG_DEBUG(pTransaction->getLogId() << "Forcing send of timeout response.");
      SIPMessage::Ptr serverError = pTransaction->serverRequest()->createResponse(SIPMessage::CODE_408_RequestTimeout);
      OSS::Net::IPAddress responseTarget;
      pTransaction->onRouteResponse(pTransaction->serverRequest(),
          pTransaction->serverTransport(), pTransaction->serverTransaction(), responseTarget);
      pTransaction->serverTransaction()->sendResponse(serverError, responseTarget);
    }
    else if (pErrorResponse && (pErrorResponse->is4xx(401) || pErrorResponse->is4xx(407) ))
    {
      //
      // Just remove the dialog from the cached list
      // but do not remove it from the state persistence
      // so that the challenged bye could still be routed
      //
      std::string callId = pTransaction->serverRequest()->hdrGet("call-id");
      OSS_LOG_DEBUG(pTransaction->getLogId() << "BYE Transaction Challenge: " << pErrorResponse->startLine());
      OSS_LOG_DEBUG(pTransaction->getLogId()
        << "Removing Dialog " << stateFile << " with Call-ID " << callId);
      _pManager->dialogStateManager().removeDialog(pTransaction, false);
    }
    else
    {
      std::string callId = pTransaction->serverRequest()->hdrGet("call-id");
      OSS_LOG_DEBUG(pTransaction->getLogId() << "BYE Transaction Error: " << pErrorResponse->startLine());
      OSS_LOG_DEBUG(pTransaction->getLogId()
        << "Destroying state file " << stateFile << " with Call-ID " << callId);
      _pManager->dialogStateManager().removeDialog(pTransaction, true);

    }
  }catch(...){}
}

} } } // OSS::SIP::SBC


