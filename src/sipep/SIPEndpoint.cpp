
// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "GNU Lesser General Public License (LGPL)".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/SIP/EP/SIPEndpoint.h"
#include "OSS/SIP/EP/SIPEndpointRetransmitter.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/UTL/Logger.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPCSeq.h"


namespace OSS {
namespace SIP {
namespace EP {
  

#define TCP_PORT_BASE 20000
#define TCP_PORT_MAX  30000
#define WS_PORT_BASE 40000
#define WS_PORT_MAX 50000 
#define RTP_PROXY_THREAD_COUNT 10
#define DEFAULT_SIP_PORT 5060
#define DEFAULT_SIP_TLS_PORT 5061
#define DEFAULT_SIP_WS_PORT 5062
  
typedef SIPEndpointRetransmitter Retransmitter;
  
SIPEndpoint::SIPEndpoint() :
  _endpointEventQueue(true/*enable pipe */)
{
  _stack.setRequestHandler(boost::bind(&SIPEndpoint::handleRequest, this, _1, _2, _3));
  _stack.setThrottleRequestHandler(boost::bind(&SIPEndpoint::handleThrottleRequest, this, _1, _2, _3));
  _stack.setAckOr2xxTransactionHandler(boost::bind(&SIPEndpoint::handleAckOr2xxTransaction, this, _1, _2));
}

SIPEndpoint::~SIPEndpoint()
{
  stopEndpoint();
}


bool SIPEndpoint::addTransport(const OSS::Net::IPAddress& address)
{
  if (!address.isValid())
  {
    return false;
  }
  
  switch (address.getProtocol())
  {
    case OSS::Net::IPAddress::UnknownTransport:
      _stack.tcpListeners().push_back(address);
    case OSS::Net::IPAddress::UDP:
      _stack.udpListeners().push_back(address);
      break;
    case OSS::Net::IPAddress::TCP:
      _stack.tcpListeners().push_back(address);
      break;
    case OSS::Net::IPAddress::TLS:
      _stack.tlsListeners().push_back(address);
      break;
    case OSS::Net::IPAddress::WS:
      _stack.wsListeners().push_back(address);
      break;
    case OSS::Net::IPAddress::WSS:
      return false; /// We do not support WSS just yet.
      break;
  }
  return true;
}

bool SIPEndpoint::addTransport(OSS::Net::IPAddress::Protocol proto, unsigned short port)
{
  bool addedOne = false;
  std::vector<OSS::Net::IPAddress> localIps = OSS::Net::IPAddress::getLocalAddresses();
  for(std::vector<OSS::Net::IPAddress>::iterator iter = localIps.begin(); iter != localIps.end(); iter++)
  {
    //
    // We only support V4 for now
    //
    if (iter->isValid() && iter->address().is_v4())
    {
      bool added = false;
      iter->setPort(port);
      iter->setProtocol(proto);
      added = addTransport(*iter);
      addedOne = addedOne || added;
    }
  }
  return addedOne;
}

bool SIPEndpoint::runEndpoint()
{
  try 
  {
    _stack.transport().setTCPPortRange(TCP_PORT_BASE, TCP_PORT_MAX);
#if ENABLE_FEATURE_WEBSOCKETS
    _stack.transport().setWSPortRange(WS_PORT_BASE, WS_PORT_MAX);
#endif
    _stack.transportInit();
    _stack.run();
  }
  catch(...)
  {
    return false;
  }
  
  return true;
}

void SIPEndpoint::stopEndpoint()
{
  _stack.stop();
  EndpointEventPtr pEvent(new EndpointEvent());
  pEvent->eventType = SIPEndpoint::EndpointTerminated;
  _endpointEventQueue.enqueue(pEvent);
}

void SIPEndpoint::sendEndpointRequest(
  const SIPMessage::Ptr& pRequest,
  const OSS::Net::IPAddress& localAddress,
  const OSS::Net::IPAddress& remoteAddress)
{
  pRequest->commitData();
  
  if (!pRequest->isRequest(OSS::SIP::REQ_ACK)) 
  {
    _stack.sendRequest(pRequest, localAddress, remoteAddress,
      boost::bind(&SIPEndpoint::handleEndpointResponse, this, _1,_2, _3, _4),
      boost::bind(&SIPEndpoint::onTransactionTerminated, this, _1));
  }
  else
  {
    _stack.sendRequestDirect(pRequest, localAddress, remoteAddress);
  }
}

static std::string create_retransmission_id(const SIPMessage::Ptr& pRequest, bool asSender)
{
  std::ostringstream id;
  OSS::SIP::SIPCSeq cseq(pRequest->hdrGet(OSS::SIP::HDR_CSEQ));
  id << pRequest->getDialogId(asSender) << "-" << cseq.getNumber();
  return id.str();
}

void SIPEndpoint::sendEndpointResponse(
    const SIPMessage::Ptr& pResponse,
    const SIPTransaction::Ptr& pTransaction,
    const OSS::Net::IPAddress& remoteAddress)
{
  if (!pTransaction || !remoteAddress.isValid())
  {
    OSS_LOG_ERROR("SIPEndpoint::sendEndpointResponse - Invalid transaction target");
    return;
  }
  
  bool is2xx = pResponse->isResponseFamily(OSS::SIP::SIPMessage::CODE_200_Ok);
  bool isInvite = pResponse->isResponseTo(OSS::SIP::REQ_INVITE);
  bool isReliableTransport = pTransaction->transport()->isReliableTransport();
  Retransmitter::Ptr pRetran;
  if (!isReliableTransport && is2xx && isInvite)
  {    
    OSS::mutex_critic_sec_lock lock(_2xxRetransmitCacheMutex);
    pRetran = Retransmitter::Ptr(new Retransmitter(this, pResponse, pTransaction->transport(), remoteAddress)); 
    //
    // If this is a 2xx, add it to the re-tranmission cache
    //
    std::string cacheId = create_retransmission_id(pResponse, true);
    _2xxRetransmitCache[cacheId] = pRetran;
    OSS_LOG_DEBUG(pTransaction->getLogId() << "Added 2xx cache-id: " << cacheId << " to retransmission cache.");
  }

  if (pTransaction && pResponse)
  {
    pTransaction->sendResponse(pResponse, remoteAddress);
  }
  
  //
  // Start retransmitting
  //
  if (pRetran)
  {
    pRetran->start(500);
  }
}

void SIPEndpoint::handleRequest(
  const OSS::SIP::SIPMessage::Ptr& pMsg, 
  const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
  const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  EndpointEventPtr pEvent(new EndpointEvent());
  pEvent->eventType = SIPEndpoint::IncomingRequest;
  pEvent->sipRequest = pMsg;
  pEvent->transportSession = pTransport;
  pEvent->transaction = pTransaction;
  _endpointEventQueue.enqueue(pEvent);
}

unsigned long SIPEndpoint::handleThrottleRequest(
  const OSS::SIP::SIPMessage::Ptr& pMsg, 
  const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
  const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  return 0;
}

void SIPEndpoint::handleAckOr2xxTransaction(
  const OSS::SIP::SIPMessage::Ptr& pMsg,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport)
{
  bool isAck = pMsg->isRequest(OSS::SIP::REQ_ACK);
  
  std::string logId = pMsg->createContextId(true);
  std::string msgType = "2xx";
  if (isAck)
  {
    msgType = "ACK";
  }
  OSS_LOG_DEBUG(logId << "Processing " << msgType << " request " << pMsg->startLine());
  {
    std::string isXOREncrypted = "0";
    pMsg->getProperty(OSS::PropertyMap::PROP_XOR, isXOREncrypted);

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
  }
  
  EndpointEventPtr pEvent(new EndpointEvent());
  pEvent->eventType = isAck ? IncomingAckFor2xx : Incoming2xxRetran;
  pEvent->sipRequest = pMsg;
  pEvent->transportSession = pTransport;
  _endpointEventQueue.enqueue(pEvent);
  
  Retransmitter::Ptr pRetran;
  if (isAck)
  {
    OSS::mutex_critic_sec_lock lock(_2xxRetransmitCacheMutex);
    std::string cacheId = create_retransmission_id(pMsg, false);
    RetransmitCache::iterator iter = _2xxRetransmitCache.find(cacheId);
    if (iter != _2xxRetransmitCache.end())
    {
      OSS_LOG_DEBUG(logId << "Found 2xx retransmission object for ACK.  ID:  " << cacheId);
      pRetran = iter->second;
      _2xxRetransmitCache.erase(iter);
      pRetran->stop();
    }
    else
    {
      OSS_LOG_WARNING(logId << "2xx retransmission object for ACK.  ID:  " << cacheId << " does not exists");
    }
  }
}
 
void SIPEndpoint::handleEndpointResponse(const SIPTransaction::Error& e, const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport, const SIPTransaction::Ptr& pTransaction)
{
  
  EndpointEventPtr pEvent(new EndpointEvent());
  if (e) 
  {
    pEvent->eventType = SIPEndpoint::TransactionError;
    pEvent->transactionError = e;
  }
  else
  {
    pEvent->eventType = SIPEndpoint::IncomingResponse;
  }
  pEvent->sipRequest = pMsg;
  pEvent->transportSession = pTransport;
  pEvent->transaction = pTransaction;
  _endpointEventQueue.enqueue(pEvent);
}

void SIPEndpoint::onTransactionTerminated(const SIPTransaction::Ptr& pTransaction)
{
  EndpointEventPtr pEvent(new EndpointEvent());
  pEvent->eventType = SIPEndpoint::TransactionTermination;
  _endpointEventQueue.enqueue(pEvent);
}

void SIPEndpoint::receiveEndpointEvent(EndpointEventPtr& ev)
{
  _endpointEventQueue.dequeue(ev);
}

void SIPEndpoint::onHandleAckTimeout(const SIPMessage::Ptr& pRequest)
{
  EndpointEventPtr pEvent(new EndpointEvent());
  pEvent->eventType = SIPEndpoint::AckTimeout;
  pEvent->sipRequest = pRequest;
  _endpointEventQueue.enqueue(pEvent);
  
  //
  // Destroy the retransmission
  //
  OSS::mutex_critic_sec_lock lock(_2xxRetransmitCacheMutex);
  Retransmitter::Ptr pRetran;
  std::string cacheId = create_retransmission_id(pRequest, true);
  RetransmitCache::iterator iter = _2xxRetransmitCache.find(cacheId);
  if (iter != _2xxRetransmitCache.end())
  {
    pRetran = iter->second;
    _2xxRetransmitCache.erase(iter);
    pRetran->stop();
  }
}

OSS::SIP::SIPTransaction::Ptr SIPEndpoint::createClientTransaction(const OSS::SIP::SIPMessage::Ptr& pMsg)
{
  return _stack.createClientTransaction(pMsg);
}

} } } // OSS::SIP::EP