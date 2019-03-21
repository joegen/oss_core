

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


#include <list>
#include <boost/lexical_cast.hpp>
#include <OSS/SIP/SIPHeaderTokens.h>

#include "OSS/UTL/Logger.h"
#include "OSS/SIP/SBC/SBCInviteBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPXOR.h"
#include "OSS/SIP/SBC/SBCRegisterBehavior.h"
#include "OSS/SIP/SBC/SBCPrackBehavior.h"
#include "OSS/UTL/PropertyMap.h"


namespace OSS {
namespace SIP {
namespace SBC {


using OSS::SIP::SIPMessage;
using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;
using OSS::Net::IPAddress;


static std::string  DEFAULT_ALLOW_HEADER = "SUBSCRIBE, NOTIFY, INVITE, ACK, CANCEL, BYE, REFER, INFO, OPTIONS";

SBCInviteBehavior::SBCInviteBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_INVITE, "SBC INVITE Request Handler"),
  _2xxRetransmitCache(32),
  _pCancelBehavior(0)
{
  setName("SBC INVITE Request Handler");
  _localPrackResponseCb = boost::bind(&SBCInviteBehavior::handleLocalPrackResponse, this, _1, _2, _3, _4);
  setMaxPacketsPerSecond(pManager->getMaxInvitesPerSecond());
}

SBCInviteBehavior::~SBCInviteBehavior()
{
}

SIPMessage::Ptr SBCInviteBehavior::onTransactionCreated(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
  /// Called by runtask signalling the creation of the transaction.
  /// This precedes any other transaction callbacks and therefore is the best place
  /// to initialize anything that would be needed by the transaction processing
{
  //
  // Force RTP proxy by default
  //
  pTransaction->serverTransaction()->setResponseCallback(boost::bind(&SBCInviteBehavior::onSendResponse, this, _1, _2, _3));
  pTransaction->setProperty("require-rtp-proxy", "1");
  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  return SBCDefaultBehavior::onTransactionCreated(pRequest, pTransaction);
}

void SBCInviteBehavior::onDestroyTransaction(SIPB2BTransaction::Ptr pTransaction)
    /// Signals that trhe transaction is about to be destroyed.
    /// This function will not invalidate the shared pointers
    /// to the transaction.  It is a mere indication that the transaction
    /// thread would now destroy its internal reference to the transaction.
{
}

SIPMessage::Ptr SBCInviteBehavior::onRouteInviteWithReplaces(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target)
{
  //
  // Check request-uri if host is pointing to us
  //
  OSS::SIP::SIPRequestLine rline = pRequest->getStartLine();
  OSS::SIP::SIPURI ruri;
  rline.getURI(ruri);
  std::string host;
  unsigned short port = 0;
  ruri.getHostPort(host, port);
  if (!port)
    port = 5060;
  OSS::Net::IPAddress hostPort(host, port);
  
  
  if (!_pManager->transactionManager().stack().transport().isLocalTransport(hostPort))
  {
    return OSS::SIP::SIPMessage::Ptr();
  }
  
  //
  // Parse replaces header and check if there is a session existing for the states
  //

  SIPB2BDialogData::LegInfo legInfo;
  if (_pManager->dialogStateManager().findReplacesTarget(pRequest, legInfo))
  {
    SIPContact contactList(legInfo.remoteContact);
    ContactURI contact;
    if (contactList.getSize() > 0)
    {
      //
      // Retarget the request-uri
      //
      contactList.getAt(contact, 0);
      SIPRequestLine rline(pRequest->getStartLine());
      rline.setURI(contact.getURI().c_str());
      
      OSS_LOG_INFO(pRequest->createContextId(true) << "SBCInviteBehavior::onRouteInviteWithReplaces - rewriting " << pRequest->getStartLine() << "->" << rline.data());
      
      pRequest->setStartLine(rline.data());
    }
  }
  else
  {
    //
    // Send out a 404
    //
    SIPMessage::Ptr notFound = pRequest->createResponse(SIPMessage::CODE_404_NotFound, "Attempt to replace an non-existent dialog");
    return notFound;
  }

  
  return SIPMessage::Ptr();
}

static bool is_local_100_rel(SIPB2BTransaction::Ptr pTransaction)
{
  std::string local100RelValue;
  return pTransaction->getProperty("local-100-rel", local100RelValue) && local100RelValue == "1";
}

static bool is_local_update(SIPB2BTransaction::Ptr pTransaction)
{
  std::string localUpdate;
  return pTransaction->getProperty("local-update", localUpdate) && localUpdate == "1";
}

SIPMessage::Ptr SBCInviteBehavior::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{ 
  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  
  //
  // Create the sessionId to be used by new calls
  //
  std::ostringstream sessionId;
  sessionId << OSS::string_hash(pRequest->hdrGet("call-id").c_str()) << OSS::getRandom();
  
  //
  // If the request is mid-dialog then route it as a re-invite
  //
  if (pRequest->isMidDialog())
  {
    return onRouteReinvite(pRequest, pTransaction, localInterface, target);
  }
  else
  {
    pTransaction->setProperty("session-id", sessionId.str());
    pTransaction->serverTransaction()->setProperty("session-id", sessionId.str());
  
    if (_pManager->dialogStateManager().hasDialog(pRequest->hdrGet("call-id")))
    {
      throw SBCStateException("Dialog exists for request without to-tag");
    }
    
    //
    // Consult the alias map prior to routing
    //
    if (_aliasMap.retargetRequest(pRequest))
    {
      SIPRequestLine rline(pRequest->startLine());
      SIPURI ruri;
      rline.getURI(ruri);
      std::string domain = ruri.getHost();
      //
      // Find a domain router if it exists
      //
      SIPB2BHandler::Ptr pDomainRouter = _pManager->transactionManager().findDomainRouter(domain);
      if (pDomainRouter)
      {
        bool handled = false;
        SIPMessage::Ptr pDomainResponse =  pDomainRouter->onRouteTransaction(pRequest, pTransaction, localInterface, target, handled);
        if (handled || pDomainResponse)
        {
          return pDomainResponse;
        }
      }
    }

    if (pRequest->hdrPresent(OSS::SIP::HDR_REPLACES))
    {
      //
      // Invites with replaces uses dialog states of existing calls for routing
      // so we let the dialog state manager to determine proper targets
      // 
      SIPMessage::Ptr replacesError = onRouteInviteWithReplaces(pRequest, pTransaction, localInterface, target);
      if (replacesError)
      {
        return replacesError;
      }
    }
    
    //
    // Inform the CDRManager about this new Invite
    //
    std::size_t channelLimit = 0;
    std::size_t channelCount = getManager()->cdr().onCallSetup(pRequest, pTransaction->serverTransaction(), channelLimit);
    if (channelCount > channelLimit)
    {
       SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_403_Forbidden, "Channel Count Exceeded");
      return serverError;
    }
  
    //
    // finaly we route it
    //
    SIPMessage::Ptr ret = SBCDefaultBehavior::onRouteTransaction(pRequest, pTransaction, localInterface, target);
    if (ret)
    {
      //
      // Route transaction produced a response.  This normally means an error occured in routing
      //
      return ret;
    }
  }
  
  //
  // Send a 100 Trying.  Take note that 100 trying may be delayed if route transaction,
  // particularly DNS lookups takes longer than expected.  Delaying 100 Trying after
  // we get a route is necessary because we won't be able to route the CANCEL request
  // if they ever arrive while we are still fetching the route to the INVITE
  //
  SIPMessage::Ptr trying = pTransaction->serverRequest()->createResponse(SIPMessage::CODE_100_Trying);
  OSS::Net::IPAddress responseTarget;
  pTransaction->onRouteResponse(pTransaction->serverRequest(),
    pTransaction->serverTransport(), pTransaction->serverTransaction(), responseTarget);
  pTransaction->serverTransaction()->sendResponse(trying, responseTarget);

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

  //
  // Preserver the contact as property header.  it will be used for transformation later
  //
  pRequest->setProperty("inbound-contact", pRequest->hdrGet("contact").c_str());
  pRequest->hdrListRemove("Contact");

  //
  // Set the transaction timeout
  //

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
  viaBranch << "z9hG4bK" << OSS::string_hash(branch.c_str());
  std::string newVia = SBCContact::constructVia(_pManager, pRequest, localInterface, targetTransport, viaBranch.str());
  pRequest->hdrListPrepend("Via", newVia);

  
  SBCContact::SessionInfo sessionInfo;
  sessionInfo.sessionId = sessionId.str();
  sessionInfo.callIndex = 2;

  //
  // Prepare the new contact
  //
  SBCContact::transform(this->getManager(),
    pRequest,
    pTransaction,
    localInterface,
    sessionInfo);
  
  std::string allow = pRequest->hdrGet(SIP::HDR_ALLOW);
  //
  // Check if we will handle 100rel locally.  Add supported header if we do
  //
  if (is_local_100_rel(pTransaction))
  {
    OSS_LOG_DEBUG(pTransaction->getLogId() << "Local reliable provisional response handling ENABLED");
    std::string supported = pRequest->hdrGet(SIP::HDR_SUPPORTED);
    if (allow.empty())
    {
      allow = DEFAULT_ALLOW_HEADER;
      allow += ", PRACK";
    }
    else if (allow.find("PRACK") == std::string::npos)
    {
      if (allow.find(", ") != std::string::npos)
      {
        allow += ", PRACK";
      }
      else
      {
        allow += ",PRACK";
      }
    }
    
    if (supported.empty())
    {
      supported = "100rel";
    }
    else if (supported.find("100rel") == std::string::npos)
    {
      if (supported.find(", ") == std::string::npos)
      {
        supported += ",100rel";
      }
      else
      {
        supported += ", 100rel";
      }
    }
    
    if (!supported.empty())
    {
      OSS_LOG_DEBUG(pTransaction->getLogId() << "Rewriting Supported Header -> " << supported);
      pRequest->hdrSet(SIP::HDR_SUPPORTED, supported.c_str());
      pRequest->commitData();
    }
    
    //
    // Remove 100rel in the require header since we will
    // handle it locally just in case remote does not support it
    //
    std::string require = pRequest->hdrGet(SIP::HDR_REQUIRE);
    if (require.find("100rel") != std::string::npos)
    {
      std::vector<std::string> tokens = OSS::string_tokenize(require, ",");
      if (tokens.size() <= 1)
      {
        pRequest->hdrRemove(SIP::HDR_REQUIRE);
      }
      else
      {
        std::ostringstream strm;
        for (std::size_t i = 0; i < tokens.size(); i++)
        {
          std::string& token = tokens[i];
          OSS::string_trim(token);
          strm << token;
          if (i < tokens.size() - 1)
          {
            strm << ",";
          }
        }
        pRequest->hdrSet(SIP::HDR_REQUIRE, strm.str());
      }
    }
  }
  
  //
  // If local update is enabled, add UPDATE in the Allow header
  //
  if (is_local_update(pTransaction))
  {
    if (allow.empty())
    {
      allow = DEFAULT_ALLOW_HEADER;
      allow += ", UPDATE";
    }
    else if (allow.find("UPDATE") == std::string::npos)
    {
      if (allow.find(", ") != std::string::npos)
      {
        allow += ", UPDATE";
      }
      else
      {
        allow += ",UPDATE";
      }
    }
  }
  
  if (!allow.empty())
  {
    pRequest->hdrSet(SIP::HDR_ALLOW, allow);
  }
  
  //
  // Call-ID Correlation
  //
  setCallIdCorrelation(pRequest, pTransaction);
  
  return OSS::SIP::SIPMessage::Ptr();
}

void SBCInviteBehavior::setCallIdCorrelation(SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr& pTransaction)
{
  std::string cid_correlation;
  if (pTransaction->getProperty("cid-correlation", cid_correlation))
  {
    if (cid_correlation == "b2b")
    {
      std::string cid_suffix;
      if (!pTransaction->getProperty("cid-suffix", cid_suffix))
      {
        cid_suffix = "-leg2";
      }
      std::string callId = pRequest->hdrGet(OSS::SIP::HDR_CALL_ID);
      callId += cid_suffix;
      pRequest->hdrSet(OSS::SIP::HDR_CALL_ID, callId);
      pRequest->hdrRemove("X-CID");
    }
    else if (cid_correlation == "x-cid")
    {
      std::string callId = OSS::string_create_uuid();
      std::string xCid = pRequest->hdrGet(OSS::SIP::HDR_CALL_ID);
      pRequest->hdrSet(OSS::SIP::HDR_CALL_ID, callId);
      pRequest->hdrSet("X-CID", xCid);
      pTransaction->setProperty("X-CID", xCid);
    }
  }
  else
  {
    pRequest->hdrRemove("X-CID");
  }
}

SIPMessage::Ptr SBCInviteBehavior::onRouteReinvite(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  //
  // First call the default behavior for mid dialog requests
  //
  SIPMessage::Ptr ret = SBCDefaultBehavior::onRouteTransaction(pRequest, pTransaction, localInterface, target);
  if (ret)
    return ret;

  //
  // Send a 100 Trying.  Take note that 100 trying may be delayed if route transaction,
  // particularly DNS lookups takes longer than expected.  Delaying 100 Trying after
  // we get a route is necessary because we won't be able to route the CANCEL request
  // if they ever arrive while we are still fetching the route to the INVITE
  //
  SIPMessage::Ptr trying = pTransaction->serverRequest()->createResponse(SIPMessage::CODE_100_Trying);
  OSS::Net::IPAddress responseTarget;
  pTransaction->onRouteResponse(pTransaction->serverRequest(),
    pTransaction->serverTransport(), pTransaction->serverTransaction(), responseTarget);
  pTransaction->serverTransaction()->sendResponse(trying, responseTarget);
  
  OSS_VERIFY(localInterface.isValid() && target.isValid());

  //
  // Mark the transaction as a reinvite to flag responses
  //
  pTransaction->setProperty("reinvite", "1");
  return SIPMessage::Ptr();
}

void SBCInviteBehavior::sendPrack(SIPMessage::Ptr& pResponse, SIPB2BTransaction::Ptr pTransaction)
{
  std::string lastrseq;
  pTransaction->getProperty("100-rel-last-rseq", lastrseq);
  std::string rseq = pResponse->hdrGet(SIP::HDR_RSEQ);
  if (lastrseq == rseq)
  {
    //
    // this is a retransmission.  Ignore it and tell the SBC not to forward it
    //
    pResponse->setProperty(OSS::PropertyMap::PROP_DisallowForwardResponse, "1");
    return; 
  }
  pTransaction->setProperty("100-rel-last-rseq", rseq);
  
  //
  // Create the local prack using the INVITE as the template
  //
  SIPMessage::Ptr pLocalPrack;
  

  //
  // Reset headers we either do not need or would replace
  //
  
  pLocalPrack = SIPMessage::Ptr(new SIPMessage(pTransaction->serverRequest()->data()));
  std::string toHeader = pResponse->hdrGet(SIP::HDR_TO);
  pLocalPrack->hdrSet(SIP::HDR_TO, toHeader);
  pLocalPrack->hdrRemove(SIP::HDR_CONTENT_TYPE);
  pLocalPrack->hdrRemove(SIP::HDR_CONTENT_LENGTH);
  pLocalPrack->setBody("");

  
  std::string hContact = pTransaction->clientRequest()->hdrGet(SIP::HDR_CONTACT);
  SIP::SIPContact contactHeader(hContact);
  SIP::ContactURI curi;
  contactHeader.getAt(curi, 0);

  //
  // Reset the request-URI as if it came from leg 1
  //
  std::string sessionId;
  int callIndex = 1;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
  std::ostringstream rline;
  rline << "PRACK sip:" << curi.getHostPort() << ";sbc-session-id=" << sessionId << ";sbc-call-index=" << callIndex << " SIP/2.0";
  
  // the request-uri will be reset by dialog state
  pLocalPrack->setStartLine(rline.str());
 
  // via will be reset by dialog state
  pLocalPrack->hdrRemove(SIP::HDR_VIA); 
 
  //
  // Increment the CSeq number
  //
  std::string cseq = pLocalPrack->hdrGet(SIP::HDR_CSEQ);
  int seqNum = 0;
  SIP::SIPCSeq::getNumber(cseq, seqNum);
  SIP::SIPCSeq::setMethod(cseq, "PRACK");
  SIP::SIPCSeq::setNumber(cseq, ++seqNum);
  pLocalPrack->hdrSet(SIP::HDR_CSEQ, cseq);
  
  //
  // Create the RACK header
  //
  std::ostringstream rack;
  rack << pResponse->hdrGet(SIP::HDR_RSEQ) << " " << pResponse->hdrGet(SIP::HDR_CSEQ);
  pLocalPrack->hdrSet(SIP::HDR_RACK, rack.str().c_str());
  pLocalPrack->commitData(); //commit the new headers we modified
  
  OSS::Net::IPAddress localInterface;
  OSS::Net::IPAddress target;
  _pManager->dialogStateManager().onRouteMidDialogTransaction(pLocalPrack, pTransaction, localInterface, target);
  pLocalPrack->commitData(); // commit the new headers modified by onRouteMidDialogTransaction
  
  _pManager->transactionManager().stack().sendRequest(pLocalPrack, localInterface, target, _localPrackResponseCb, OSS::SIP::SIPTransaction::TerminateCallback());
}

void SBCInviteBehavior::handleLocalPrackResponse(
    const OSS::SIP::SIPTransaction::Error& e,
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
}

void SBCInviteBehavior::onProcessResponseInbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction)
{
  if (pResponse->is2xx())
  {
    std::string id;
    if (pTransaction->serverRequest()->getTransactionId(id))
      _pCancelBehavior->removeInvite(id);
    
    _pManager->markCallStartTime(pResponse->hdrGet(OSS::SIP::HDR_CALL_ID));
  }

  std::string isReinvite;
  if (pTransaction->getProperty("reinvite", isReinvite))
  {
    onProcessReinviteResponseInbound(pResponse, pTransaction);
    return;
  }
  
  std::string sessionId;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
  _pManager->dialogStateManager().onUpdateInitialUACState(pResponse, pTransaction, sessionId);
  
  //
  // Send PRACK if we are handling 100rel locally
  //
  if (is_local_100_rel(pTransaction) && (!pResponse->is1xx(100) && !pResponse->isFinalResponse()))
  {
    std::string require = pResponse->hdrGet(SIP::HDR_REQUIRE);
    if (require.find("100rel") != std::string::npos)
    {
      sendPrack(pResponse, pTransaction);
    }
  }
  
  //
  // Some implementations barf on 181
  //
  if (pResponse->is1xx(181))
  {
    std::string disable181Relay;
    if (pTransaction->getProperty("disable-181-relay", disable181Relay) && disable181Relay == "1")
    {
      pResponse->setProperty(OSS::PropertyMap::PROP_DisallowForwardResponse, "1");
    }
  }
  
  SBCDefaultBehavior::onProcessResponseInbound(pResponse, pTransaction);
}

void SBCInviteBehavior::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  std::string logId = pTransaction->getLogId();
  //
  // Let the script process it first 
  //
  pResponse->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  SBCDefaultBehavior::onProcessResponseOutbound(pResponse, pTransaction);


  //
  // If it is a reinvite let the other handler do the job and bail-out
  //
  std::string isReinvite;
  if (pTransaction->getProperty("reinvite", isReinvite))
  {
    onProcessReinviteResponseOutbound(pResponse, pTransaction);
    return;
  }

  std::string sessionId;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));


  //
  // Update the dialog state for leg1
  //
  _pManager->dialogStateManager().onUpdateInitialUASState(pResponse, pTransaction, sessionId);

  //
  // Remove the rseq and require header if we are to handle 100rel locally
  // if remote does not support it.  Otherwise, leave it or insert one if they are missing
  //
  bool remoteSupports100Rel = pTransaction->serverRequest()->hdrGet(SIP::HDR_SUPPORTED).find("100rel") != std::string::npos;
  if (is_local_100_rel(pTransaction) && (!pResponse->is1xx(100) && !pResponse->isFinalResponse()))
  {
    std::string require;
    require = pResponse->hdrGet(SIP::HDR_REQUIRE);
    if (!require.empty() && require.find("100rel") != std::string::npos && !remoteSupports100Rel)
    {
      std::vector<std::string> tokens = OSS::string_tokenize(require, ",");
      std::ostringstream strm;
      for (std::size_t i = 0; i < tokens.size(); i++ )
      {
        std::string token = tokens[i];
        OSS::string_trim(token);
        if (token != "100rel")
        {
          strm << token;
          if (i+1 != tokens.size())
          {
            strm << ",";
          }
        }
      }
      const std::string& newRequire = strm.str();
      if (!newRequire.empty())
      {
        pResponse->hdrSet(SIP::HDR_REQUIRE, newRequire.c_str());
      }
      else
      {
        pResponse->hdrRemove(SIP::HDR_REQUIRE);
      }
      pResponse->hdrRemove(SIP::HDR_RSEQ);
      
      if (!pResponse->body().empty())
      {
        //
        // Preserve the SDP.  We will need it later to repopulate the 200 OK
        //
        pTransaction->setProperty("local-100-rel-sdp", pResponse->body());
      }
    }
    else if ((require.empty() || require.find("100rel") == std::string::npos) && remoteSupports100Rel) 
    {
      //
      // Caller supports 100rel but termination does not
      //
      if (require.empty())
      {
        require = "100rel";
      }
      else
      {
        if (require.find(", "))
        {
          require += ", 100rel";
        }
        else
        {
          require += ",100rel";
        }
      }
      pResponse->hdrSet(SIP::HDR_REQUIRE, require);
      
      std::string localRseq;
      pTransaction->getProperty("local-rseq", localRseq);
      if (localRseq.empty())
      {
        localRseq = "0";
      }
      int rseq = OSS::string_to_number<int>(localRseq.c_str());
      std::string newRseq = OSS::string_from_number<int>(++rseq);
      pTransaction->setProperty("local-rseq", newRseq);
      pResponse->hdrSet(SIP::HDR_RSEQ, newRseq);
    }
    
    //
    // Finally, if remote supports 100 rel and it is locally enabled we queue the response for retransmission
    //
    if (remoteSupports100Rel)
    {
      _pManager->getPrackHandler()->queueReliableResponse(pTransaction, pResponse);
    }
  }
  
  //
  // Cease all 100rel retransmissions on receipt of final response
  //
  if (remoteSupports100Rel && is_local_100_rel(pTransaction) && pResponse->isFinalResponse())
  {
    _pManager->getPrackHandler()->removeReliableResponse(pResponse->hdrGet(SIP::HDR_CALL_ID));
  }
  
  //
  // If this is a 2xx, then retransmit it.
  //
  if (pResponse->is2xx() || pResponse->is1xx(183))
  {
    if (!remoteSupports100Rel && is_local_100_rel(pTransaction) && pResponse->body().empty())
    {
      std::string local100RelSdp;
      pTransaction->getProperty("local-100-rel-sdp", local100RelSdp);
      if (!local100RelSdp.empty())
      {
        pResponse->body() = local100RelSdp;
        pResponse->updateLength();
        pResponse->hdrSet(SIP::HDR_CONTENT_TYPE, "application/sdp");
      }
    }
    
    std::string transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier()).c_str();
    pResponse->setProperty(OSS::PropertyMap::PROP_TransportId, transportId);

    std::string transportScheme = pTransaction->serverTransport()->getTransportScheme();
    OSS::string_to_upper(transportScheme);
    pResponse->setProperty(OSS::PropertyMap::PROP_TargetTransport, transportScheme);

    std::ostringstream cacheId;
    cacheId << pResponse->getDialogId(false) << pResponse->hdrGet("cseq");
    boost::any cacheItem = pResponse;
    pResponse->setProperty("session-id", sessionId);
    _2xxRetransmitCache.add(cacheId.str(), cacheItem);
  }
  else if (pResponse->isErrorResponse())
  {

    //
    // Remove the rtp proxies if they were created.
    // If we got an authorization challenge, we simply
    // change the state so that we recycle the previous
    // ports already alloted for the INVITE
    //
    try
    {
      _pManager->rtpProxy().removeSession(sessionId);
    }catch(...){}
  }
}

void SBCInviteBehavior::onProcessReinviteResponseInbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction)
{
  std::string sessionId;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
  _pManager->dialogStateManager().onUpdateReinviteUACState(pResponse, pTransaction, sessionId);
  SBCDefaultBehavior::onProcessResponseInbound(pResponse, pTransaction);
}

void SBCInviteBehavior::onProcessReinviteResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{

  std::string sessionId;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
  _pManager->dialogStateManager().onUpdateReinviteUASState(pResponse, pTransaction, sessionId);
  if (pResponse->is2xx())
  {
    //
    // If this is a 2xx, add it to the retranmission cache
    //
    std::ostringstream cacheId;
    cacheId << pResponse->getDialogId(false) << pResponse->hdrGet("cseq");
    boost::any cacheItem = pResponse;
    pResponse->setProperty("session-id", sessionId);
    _2xxRetransmitCache.add(cacheId.str(), cacheItem);
  }
}

void SBCInviteBehavior::onProcessAckOr2xxRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport)
{
  std::string logId = pMsg->createContextId(true);
  
  if (!pTransport->isEndpoint())
  {
    std::string isXOREncrypted = "0";
#if ENABLE_FEATURE_XOR
    if (SIP::SIPXOR::isEnabled())
    {
      pMsg->getProperty("xor", isXOREncrypted);
    }
#endif
    std::ostringstream logMsg;
    logMsg << logId << "<<< " << pMsg->startLine()
    << " LEN: " << pTransport->getLastReadCount()
    << " SRC: " << pTransport->getRemoteAddress().toIpPortString()
    << " DST: " << pTransport->getLocalAddress().toIpPortString()
    << " EXT: " << "[" << pTransport->getExternalAddress() << "]"
    << " ENC: " << isXOREncrypted
    << " PROT: " << pTransport->getTransportScheme();
    OSS::log_information(logMsg.str());
    if (OSS::log_get_level() >= OSS::PRIO_DEBUG)
      OSS::log_debug(pMsg->createLoggerData());
    
    SIPTransportService::dumpHepPacket(
      pTransport->getTransportScheme() == "udp" ? OSS::Net::IPAddress::UDP : OSS::Net::IPAddress::TCP,
      pTransport->getRemoteAddress(),
      pTransport->getLocalAddress(),
      pMsg->data()
    );
  }

  if (pMsg->is2xx())
  {
    std::ostringstream cacheId;
    cacheId << pMsg->getDialogId(false) << pMsg->hdrGet("cseq");
    Cacheable::Ptr cacheItem = _2xxRetransmitCache.get(cacheId.str());
    if (cacheItem)
    {
      SIPMessage::Ptr p2xx = boost::any_cast<SIPMessage::Ptr>(cacheItem->data());
      if (p2xx)
      {
        std::string target;
        std::string localInterface;
        OSS_VERIFY(p2xx->getProperty(PropertyMap::PROP_ResponseTarget, target));
        OSS_VERIFY(p2xx->getProperty(PropertyMap::PROP_ResponseInterface, localInterface));

        std::string transport;
        SIPVia::msgGetTopViaTransport(p2xx.get(), transport);
        OSS::string_to_lower(transport);
        
        if (transport == "udp")
        {
          std::string isXOREncrypted = "0";
#if ENABLE_FEATURE_XOR
          if (SIP::SIPXOR::isEnabled())
          {
            p2xx->getProperty("xor", isXOREncrypted);
          }
#endif

          if (!_pManager->getUserAgentName().empty())
            p2xx->hdrSet("Server", _pManager->getUserAgentName().c_str());

          _pManager->transactionManager().stack().sendRequestDirect(p2xx,
            IPAddress::fromV4IPPort(localInterface.c_str()),
            IPAddress::fromV4IPPort(target.c_str()));
        }
      }
    }
  }
  else if (pMsg->isRequest("ACK"))
  {
    OSS_LOG_DEBUG(logId << "Processing ACK request " << pMsg->startLine());
    OSS::Net::IPAddress localAddress;
    OSS::Net::IPAddress targetAddress;
    std::string sessionId;
    std::string peerXOR = "0";
    try
    {
      _pManager->dialogStateManager().onRouteAckRequest(pMsg, pTransport, _2xxRetransmitCache,
        sessionId, peerXOR, localAddress, targetAddress);
      _pManager->transactionManager().stack().sendRequestDirect(pMsg, localAddress, targetAddress);
    }
    catch(OSS::Exception e)
    {
      std::ostringstream logMsg;
      logMsg << logId << "Unable to process ACK.  Exception: " << e.message();
      OSS::log_warning(logMsg.str());
    }
  }
}


void SBCInviteBehavior::onProcessOutbound(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
  /// This is the last chance for the application to process
  /// the outbound request before it gets sent out to the transport.
  ///
  /// This is normally the place where application would want to
  /// insert application-specific headers as well as change existing
  /// headers to the desired application-specific values for as long
  /// as it wont conflict with dialog creation states.
{
  std::string id;
  if (pTransaction->serverRequest()->getTransactionId(id))
    _pCancelBehavior->registerInvite(id, pRequest);

  SBCDefaultBehavior::onProcessOutbound(pRequest, pTransaction);
}


void SBCInviteBehavior::onTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,      
  SIPB2BTransaction::Ptr pTransaction)
  /// Signals that an error occured on the transaction
  ///
  /// The transaction will be destroyed automatically after this function call
{
  SBCDefaultBehavior::onTransactionError(e, pErrorResponse, pTransaction);

  std::string id;
  if (pTransaction->serverRequest()->getTransactionId(id))
  {
    _pCancelBehavior->removeInvite(id);
    
    std::string sessionId;
    OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
    //
    // Remove the state files if they were created
    //
    try
    {

      if (!pTransaction->isMidDialog())
      {
        //
        // Do not delete state files for mid-dialog error responses for INVITE!
        // Only a BYE will terminate the dialog
        //
        boost::filesystem::path stateFile = operator/(_stateDir, sessionId);
        ClassType::remove(stateFile);
      }
    }catch(...){}

    //
    // Remove the rtp proxies if they were created
    //
    try
    {
      _pManager->rtpProxy().removeSession(sessionId);
    }catch(...){}
  }
  
  if (pErrorResponse && pErrorResponse->is4xx(SIPMessage::CODE_408_RequestTimeout))
  {
    if (pTransaction && !pTransaction->serverRequest()->isMidDialog())
    {
      std::string regId;
      if (pTransaction->getProperty("Registration-Id", regId))
      {
        _pManager->registerHandler()->deleteUpperRegistration(regId);
      }
    }
  }
}

void SBCInviteBehavior::onSendResponse(const SIPMessage::Ptr& pResponse, const SIPTransportSession::Ptr& pTransport, const SIPTransaction::Ptr& pTransaction)
{
  //
  // Inform the CDRManager about this response
  //
  if (pResponse->isResponseFamily(100) && !pResponse->is1xx(100))
  {
    getManager()->cdr().onCallProgress(pResponse, pTransaction);
  }
  else if (!pResponse->isResponseFamily(100))
  {
    getManager()->cdr().onCallFinal(pResponse, pTransaction);
  }
}


} } } // OSS::SIP::SBC


