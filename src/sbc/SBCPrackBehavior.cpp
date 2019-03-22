

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


#include "OSS/SIP/SBC/SBCPrackBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"


namespace OSS {
namespace SIP {
namespace SBC {

using OSS::SIP::SIPMessage;

static const int T1_100_REL = 500; // timer initial value
static const int MAX_100_REL_RETRAN = 10; // Maximum number of time we retransmit the response

SBCPrackBehavior::SBCPrackBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_PRACK, "SBC PRACK Request Handler"),
  _p100RelThread(0),
  _isTerminating(false)
{
  setName("SBC PRACK Request Handler");
  start100RelThread();
}

SBCPrackBehavior::~SBCPrackBehavior()
{
  stop100RelThread();
}

SIPMessage::Ptr SBCPrackBehavior::onTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  if (!pRequest->isMidDialog())
  {
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest);
    return serverError;
  }
  return SBCDefaultBehavior::onTransactionCreated(pRequest, pTransaction);
}


void SBCPrackBehavior::onProcessResponseOutbound(
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

//
// Local 100 Rel support
//

    
SBCPrackBehavior::ReliableResponse::ReliableResponse()
{
  lastDuration = T1_100_REL;
  timeRemaining = T1_100_REL;
  retransmitCount = 0;
}

SBCPrackBehavior::ReliableResponse::ReliableResponse(const ReliableResponse& r)
{
  lastDuration = r.lastDuration;
  timeRemaining = r.timeRemaining;
  retransmitCount = r.retransmitCount;
  response = r.response;
  target = r.target;
  localAddress = r.localAddress;
  callId = r.callId;
  rseq = r.rseq;
}

SBCPrackBehavior::ReliableResponse& SBCPrackBehavior::ReliableResponse::operator=(const ReliableResponse& r)
{
  lastDuration = r.lastDuration;
  timeRemaining = r.timeRemaining;
  retransmitCount = r.retransmitCount;
  response = r.response;
  target = r.target;
  localAddress = r.localAddress;
  callId = r.callId;
  rseq = r.rseq;
  return *this;
}


void SBCPrackBehavior::start100RelThread()
{
  if (!_p100RelThread)
  {
    _p100RelThread = new boost::thread(boost::bind(&SBCPrackBehavior::run100RelThread, this));
  }
}

void SBCPrackBehavior::stop100RelThread()
{
  _isTerminating = true;
  if (_p100RelThread)
  {
    _p100RelThread->join();
    delete _p100RelThread;
    _p100RelThread = 0;
  }
}

void SBCPrackBehavior::run100RelThread()
{
  while (!_isTerminating)
  {
    retransmit100Rel();
    if (!_isTerminating)
    {
      OSS::thread_sleep(T1_100_REL); // Sleep for T1 duration
    }
  }
}

void SBCPrackBehavior::retransmit100Rel()
{
  OSS::mutex_critic_sec_lock lock(_100RelMutex);
  for (ReliableResponseQueue::iterator iter = _100RelQueue.begin(); iter != _100RelQueue.end() && !_isTerminating;)
  {
    if (!send100Rel(*iter))
    {
      OSS_LOG_DEBUG(iter->response->createContextId(true) << "Removing reliable response from retransmission queue rseq=" << iter->rseq);
      iter = _100RelQueue.erase(iter);
    }
    else
    {
      iter++;
    }
  }
}

bool SBCPrackBehavior::send100Rel(ReliableResponse& response)
{
  response.timeRemaining -= T1_100_REL; // decrement the remaining time before we send it
  if (response.timeRemaining <= 0)
  {
    ++response.retransmitCount;
    response.lastDuration = response.lastDuration * 2; // double the duration
    response.timeRemaining = response.lastDuration;
    _pManager->transactionManager().stack().sendRequestDirect(response.response, response.localAddress, response.target);
  }
  return response.retransmitCount < MAX_100_REL_RETRAN;
}

bool SBCPrackBehavior::queueReliableResponse(const SIPB2BTransaction::Ptr pTransaction, const SIPMessage::Ptr& pResponse)
{
  OSS::mutex_critic_sec_lock lock(_100RelMutex);
  ReliableResponse rel;
  std::string target;
  std::string localInterface;
  OSS_VERIFY(pResponse->getProperty(PropertyMap::PROP_ResponseTarget, target));
  OSS_VERIFY(pResponse->getProperty(PropertyMap::PROP_ResponseInterface, localInterface));
  
  rel.response = pResponse;
  rel.localAddress = OSS::Net::IPAddress::fromV4IPPort(localInterface.c_str());
  rel.target = OSS::Net::IPAddress::fromV4IPPort(target.c_str());
  rel.callId = pResponse->hdrGet(SIP::HDR_CALL_ID);
  rel.rseq = pResponse->hdrGet(SIP::HDR_RSEQ);
  
  if (rel.callId.empty() || rel.rseq.empty() || !rel.localAddress.isValid() || !rel.target.isValid())
  {
    return false;
  }
  _100RelQueue.push_back(rel);
  
  OSS_LOG_DEBUG(pTransaction->getLogId() << "Queued reliable response rseq=" << rel.rseq);
  return true;
}

bool SBCPrackBehavior::removeReliableResponse(const std::string& callId, const std::string& rseq)
{
  OSS::mutex_critic_sec_lock lock(_100RelMutex);
  ReliableResponseQueue::iterator iter = _100RelQueue.begin();
  bool found = false;
  while(iter != _100RelQueue.end())
  {
    if (iter->callId == callId && (iter->rseq == rseq || rseq.empty()))
    {
      found = true;
      OSS_LOG_DEBUG(iter->response->createContextId(true) << "Removing reliable response from retransmission queue rseq=" << iter->rseq);
      
      iter = _100RelQueue.erase(iter);
      if (!rseq.empty())
      {
        break;
      }
    }
    else
    {
      iter++;
    }
  }
  return found;
}

SIPMessage::Ptr SBCPrackBehavior::onRouteTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target)
{
  //
  // Call the default behavior first so that states (like session-id and local-100-rel) are properly populated
  //
  SIPMessage::Ptr pResponse = SBCDefaultBehavior::onRouteTransaction(pRequest, pTransaction, localInterface, target);
  
  std::string local100Rel;
  if (!pTransaction->getProperty("local-100-rel", local100Rel) || local100Rel != "1")
  {
    return pResponse;
  }
  
  //
  // The fact that we are getting a prack, it means leg-1 advertizes 100rel in the supported header
  //
  
  if (!pRequest->body().empty())
  {
    //
    // We do not support SDP in PRACK for now
    //
    return pTransaction->serverRequest()->createResponse(SIPMessage::CODE_406_NotAcceptable, "SDP in PRACK Unsupported");
  }
  
  std::string callId = pRequest->hdrGet(SIP::HDR_CALL_ID);
  if (callId.empty())
  {
    return pTransaction->serverRequest()->createResponse(SIPMessage::CODE_400_BadRequest, "Missing Call-ID Header");
  }
  
  std::string rack = pRequest->hdrGet(SIP::HDR_RACK);
  if (rack.empty())
  {
    return pTransaction->serverRequest()->createResponse(SIPMessage::CODE_400_BadRequest, "Missing RAck Header");
  }
  
  std::vector<std::string> tokens = OSS::string_tokenize(rack, " ");
  if (tokens.size() != 3)
  {
    return pTransaction->serverRequest()->createResponse(SIPMessage::CODE_400_BadRequest, "Malformed RAck Header");
  }
  
  std::string rseq = tokens[0];
  OSS::string_trim(rseq);
  
  removeReliableResponse(callId, rseq);
  
  return pTransaction->serverRequest()->createResponse(SIPMessage::CODE_200_Ok);
}

} } } // OSS::SIP::SBC


