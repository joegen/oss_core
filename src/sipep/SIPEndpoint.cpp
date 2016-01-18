
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
#include "OSS/SIP/SIPVia.h"


namespace OSS {
namespace SIP {
namespace EP {
  

#define TCP_PORT_BASE 20000
#define TCP_PORT_MAX  30000
#define WS_PORT_BASE 40000
#define WS_PORT_MAX 50000 
#define RTP_PROXY_THREAD_COUNT 10
#define EXTERNAL_IP_HOST_URL "stun.ezuce.com"
#define DEFAULT_SIP_PORT 5060
#define DEFAULT_SIP_TLS_PORT 5061
#define DEFAULT_SIP_WS_PORT 5062
  
SIPEndpoint::SIPEndpoint()
{
  _stack.setRequestHandler(boost::bind(&SIPEndpoint::handleRequest, this, _1, _2, _3));
  _stack.setAckFor2xxTransactionHandler(boost::bind(&SIPEndpoint::handleAckFor2xxTransaction, this, _1, _2));
}

SIPEndpoint::~SIPEndpoint()
{
  stopEndpoint();
}


bool SIPEndpoint::addTransport(const OSS::Net::IPAddress& address)
{
  switch (address.getProtocol())
  {
    case OSS::Net::IPAddress::UnknownTransport:
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

bool SIPEndpoint::runEndpoint()
{
  try 
  {
    _stack.transport().setTCPPortRange(TCP_PORT_BASE, TCP_PORT_MAX);
    _stack.transport().setWSPortRange(WS_PORT_BASE, WS_PORT_MAX);
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
  _stack.sendRequest(pRequest, localAddress, remoteAddress,
    boost::bind(&SIPEndpoint::handleEndpointResponse, this, _1,_2, _3, _4),
    boost::bind(&SIPEndpoint::onTransactionTerminated, this, _1));
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

void SIPEndpoint::handleAckFor2xxTransaction(
  const OSS::SIP::SIPMessage::Ptr& pMsg,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport)
{
  EndpointEventPtr pEvent(new EndpointEvent());
  pEvent->eventType = SIPEndpoint::Ackfor2xx;
  pEvent->sipRequest = pMsg;
  pEvent->transportSession = pTransport;
  _endpointEventQueue.enqueue(pEvent);
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



} } } // OSS::SIP::EP