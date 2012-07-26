// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSSAPI API.
//
// Copyright (c) OSS Software Solutions
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


#include "OSS/SIP/SIPB2BTransaction.h"
#include "OSS/SIP/SIPB2BTransactionManager.h"
#include "OSS/Logger.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/ABNF/ABNFSIPIPV4Address.h"
#include "OSS/ABNF/ABNFSIPIPV6Address.h"
#include "OSS/SIP/SIPVia.h"


namespace OSS {
namespace SIP {


SIPB2BTransaction::SIPB2BTransaction(SIPB2BTransactionManager* pManager) :
  _pManager(pManager),
  _pInternalPtr(0),
  _hasSentLocalResponse(false),
  _isMidDialog(false),
  _failoverCount(0)
{
}

SIPB2BTransaction::~SIPB2BTransaction()
{
  std::string trnId;
  if (_pServerRequest)
    _pServerRequest->getTransactionId(trnId);
  {
    std::ostringstream logMsg;
    logMsg << _logId << "B2B Transaction DESTROYED - " << trnId;
    OSS::log_information(logMsg.str());
  }
}

bool SIPB2BTransaction::onRouteResponse(
  const OSS::SIP::SIPMessage::Ptr& pRequest, 
  const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
  const OSS::SIP::SIPTransaction::Ptr& pTransaction,
  OSS::IPAddress& target)
{
  bool validTarget = false;
  {//localize
    OSS::mutex_critic_sec_lock lock(_responseTargetMutex);
    validTarget = _responseTarget.isValid();
    if (validTarget)
    {
      target = _responseTarget;
      return true;
    }
  }//localize

  _pManager->onRouteResponse(pRequest, pTransport, shared_from_this(), target);
  _responseTarget = target;

  OSS::mutex_critic_sec_lock lock(_responseTargetMutex);
  return target.isValid();
}

void SIPB2BTransaction::releaseInternalRef()
{
  _pManager->onDestroyTransaction(shared_from_this());
  delete _pInternalPtr;
  _pInternalPtr = 0;
}


void SIPB2BTransaction::runTask()
{
  _pInternalPtr = new Ptr(this);
  try
  {
    //
    // This method runs in its own thread and will not block any operation
    // in the subsystem.  It is therefore safe to call blocking functions
    // in this method.
    //

    if (!_pServerRequest || !_pServerTransport || !_pServerTransaction)
    {
      //
      // Not calling releaseInternalRef because transacton creation ahs not been signaled yet
      //
      delete _pInternalPtr;
      _pInternalPtr = 0;
      throw OSS::SIP::SIPException("Transaction info is missing while calling SIPB2BTransaction::runTask()");
    }

    _logId =  _pServerTransaction->getLogId();

    std::string trnId;
    _pServerRequest->getTransactionId(trnId);
    {
      std::ostringstream logMsg;
      logMsg << _logId << "B2B Transaction CREATED - " << trnId;
      OSS::log_information(logMsg.str());
    }

    _isMidDialog = _pServerRequest->isMidDialog();
    //
    // Signal transaction creation
    //
    SIPMessage::Ptr pTrnCreateResponse = _pManager->onTransactionCreated(_pServerRequest, shared_from_this());
    if (pTrnCreateResponse)
    {
      OSS::IPAddress target;
      if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
      {
        if (target.isValid())
          _pServerTransaction->sendResponse(pTrnCreateResponse, target);
      }
      releaseInternalRef();
      return;
    }

    //
    // Authenticate the request
    //
    SIPMessage::Ptr pAuthenticator;
    pAuthenticator = _pManager->onAuthenticateTransaction(_pServerRequest, shared_from_this());
    if (pAuthenticator)
    {
      OSS::IPAddress target;
      if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
      {
        if (target.isValid())
          _pServerTransaction->sendResponse(pAuthenticator, target);
      }
      releaseInternalRef();
      return;
    }

    //
    // Clone the server request.
    // From now on, we will feed the clone to the server callbacks.
    //
    SIPMessage* outbound = new SIPMessage();
    *outbound = *(_pServerRequest.get());
    _pClientRequest = SIPMessage::Ptr(outbound);

    //
    // Route the outbound request.
    // Send a response (probably a 404) if the request is non-routable
    //
    OSS::IPAddress outboundTarget;
    SIPMessage::Ptr pRouteResponse;

    try
    {
      pRouteResponse = _pManager->onRouteTransaction(_pClientRequest, shared_from_this(), _localInterface, outboundTarget);
    }
    catch(OSS::Exception e)
    {
      OSS::log_warning(_logId + e.message());
      releaseInternalRef();
      return;
    }


    if (pRouteResponse)
    {
      if (pRouteResponse->isResponse())
      {
        OSS::IPAddress target;
        if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
        {
          if (target.isValid())
            _pServerTransaction->sendResponse(pRouteResponse, target);
        }
      }
      releaseInternalRef();
      return;
    }

    //
    // Check if the route handler specified that a response would be handled locally
    //
    std::string invokeLocalHandler = "0";
    if (getProperty("invoke-local-handler", invokeLocalHandler ) && invokeLocalHandler == "1")
    {
      SIPMessage::Ptr localResponse = _pManager->onInvokeLocalHandler(_pServerRequest, _pServerTransport, shared_from_this());
      if (!localResponse)
        localResponse = _pServerRequest->createResponse(500, "No local handler specified");
      OSS::IPAddress target;
      if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
      {
        if (target.isValid())
          _pServerTransaction->sendResponse(localResponse, target);
      }
      releaseInternalRef();
      return;
    }

    if (_localInterface.isValid() && !_pManager->stack().transport().isLocalTransport(_localInterface))
    {
      OSS::log_critical(_logId + "Invalid Local-Interface returned by onRouteTransaction - " + _localInterface.toIpPortString() );
      SIPMessage::Ptr serverError = _pServerRequest->createResponse(500, "Unable to determine local interface");
      OSS::IPAddress target;
      if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
      {
        if (target.isValid())
          _pServerTransaction->sendResponse(serverError, target);
      }
      releaseInternalRef();
      return;
    }

    if (!outboundTarget.isValid())
    {
      OSS::log_critical(_logId + "Invalid Outbound-Target returned by onRouteTransaction");
      SIPMessage::Ptr serverError = _pServerRequest->createResponse(500);
      OSS::IPAddress target;
      if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
      {
        if (target.isValid())
          _pServerTransaction->sendResponse(serverError, target);
      }
      releaseInternalRef();
      return;
    }

    //
    // Set the target transport of the URI if specified
    //
    std::string targetTransport;
    if (!_pClientRequest->getProperty("target-transport", targetTransport))
    {
       _pClientRequest->setProperty("target-transport", "udp");
       targetTransport = "udp";
    }

#if 0
    //
    // This conflicts with freeswitch uri authentication.  disable it for now
    //
    if (!targetTransport.empty())
    {
      OSS::string_to_lower(targetTransport);
      SIPRequestLine rline = _pClientRequest->startLine();
      SIPURI ruri;
      if (rline.getURI(ruri))
      {
        ruri.setParam("transport", targetTransport.c_str());
        rline.setURI(ruri.data().c_str());
        _pClientRequest->startLine() = rline.data();
      }
    }
#endif

    //
    // Check if the route handler specified that a response would be generated locally
    //
    std::string genLocalResponse = "0";
    if (getProperty("generate-local-response", genLocalResponse ) && genLocalResponse == "1")
    {
      SIPMessage::Ptr localResponse = _pManager->onGenerateLocalResponse(_pServerRequest, _pServerTransport, shared_from_this());
      if (localResponse)
      {
        OSS::IPAddress target;
        if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
        {
          if (target.isValid())
          {
            _pManager->onProcessResponseOutbound(localResponse, shared_from_this());
            _hasSentLocalResponse = true;
            _pServerTransaction->sendResponse(localResponse, target);
          }
        }
      }
    }


    //
    // Save the address properties
    //
    _pClientRequest->setProperty("target-address", outboundTarget.toIpPortString());
    _pClientRequest->setProperty("local-address", _localInterface.toIpPortString());

    //
    // Handle the message body
    //
    if (!_pClientRequest->body().empty())
    {
      std::string serverRequestXor = "0";
      _pServerRequest->getProperty("xor", serverRequestXor);
      std::string clientRequestXor = "0";
      _pClientRequest->getProperty("xor", clientRequestXor);
      _pClientRequest->setProperty("peer-xor", serverRequestXor);
      _pServerRequest->setProperty("peer-xor", clientRequestXor);

      SIPMessage::Ptr pBodyResponse;
      pBodyResponse = _pManager->onProcessRequestBody(_pClientRequest, shared_from_this());
      if (pBodyResponse)
      {
        if (pBodyResponse->isResponse())
        {
          OSS::IPAddress target;
          if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
          {
            if (target.isValid())
              _pServerTransaction->sendResponse(pBodyResponse, target);
          }
        }
        releaseInternalRef();
        return;
      }
    }
    //
    // Last chance for the application to process the outbound request
    //
    _pManager->onProcessOutbound(_pClientRequest, shared_from_this());

    //
    // Commit the changes
    //
    _pClientRequest->commitData();

    //
    // Send the request
    //

    OSS::SIP::SIPTransaction::Callback responseCallback
      = boost::bind(&SIPB2BTransaction::handleResponse, this, _1, _2, _3, _4);
    _pManager->stack().sendRequest(
      _pClientRequest,
      _localInterface,
      outboundTarget,
      responseCallback);

    //
    // Take note that at this point, this transaction is in limbo
    // since it is not maintained in any list. The responses
    // including transaction errors is the only callback that will
    // assure that this transaction is garbage collected
    //
  }
  catch(OSS::Exception e)
  {

    SIPMessage::Ptr serverError = _pServerRequest->createResponse(500, e.message());
    OSS::IPAddress target;
    if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
    {
      if (target.isValid())
        _pServerTransaction->sendResponse(serverError, target);
    }

    std::ostringstream errorMsg;
    errorMsg << _logId << "Fatal Exception while calling SIPB2BTransaction::runTask() - "
            << e.message();
    OSS::log_error(errorMsg.str());
    releaseInternalRef();
    return;
  }
}

bool SIPB2BTransaction::isFailoverCandidate(
    const OSS::SIP::SIPTransaction::Error& e,
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  if (_pClientRequest->isMidDialog())
    return false;

  if (_udpSrvTargets.size() <= 1 || _tcpSrvTargets.size() <= 1 || _tlsSrvTargets.size() <= 1)
    return false;

  std::string transport;
  _pClientRequest->getProperty("target-transport", transport);

  if (transport == "udp" && _failoverCount + 1 >= _udpSrvTargets.size())
    return false;
  else if (transport == "tcp" && _failoverCount + 1 >= _tcpSrvTargets.size())
    return false;
  else if (transport == "tls" && _failoverCount + 1 >= _tlsSrvTargets.size())
    return false;
  
  if (e)
  {
    if (!_pManager->threadPool().schedule(boost::bind(&SIPB2BTransaction::runFailoverTask, shared_from_this())))
    {
      OSS::log_error(_logId + "No available thread to handle SIPB2BTransaction::runFailoverTask");
      return false;
    }
    return true;
  }

  if (pMsg && pMsg->is5xx())
  {
    if (!_pManager->threadPool().schedule(boost::bind(&SIPB2BTransaction::runFailoverTask, shared_from_this())))
    {
      OSS::log_error(_logId + "No available thread to handle SIPB2BTransaction::runFailoverTask");
      return false;
    }
    return true;
  }

  return false;
}

void SIPB2BTransaction::runFailoverTask()
{
  std::string transport;
  _pClientRequest->getProperty("target-transport", transport);
  _failoverCount++;
  OSS::IPAddress target;

  if (transport == "udp")
  {
    if (_failoverCount >= _udpSrvTargets.size())
    {
      SIPMessage::Ptr serverError = _pServerRequest->createResponse(480, "All Routes Exhausted");
      OSS::IPAddress target;
      if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
      {
        if (target.isValid())
          _pServerTransaction->sendResponse(serverError, target);
      }

      std::ostringstream errorMsg;
      errorMsg << _logId << "Fatal Exception while calling SIPB2BTransaction::runFailoverTask() - Failover count exceeds UDP records available";
      OSS::log_error(errorMsg.str());
      releaseInternalRef();
      return;
    }

    unsigned short targetPort = _udpSrvTargets.begin()->get<2>();
    if (targetPort == 0)
      targetPort = 5060;
    target = _udpSrvTargets.begin()->get<1>();
    target.setPort(targetPort);
  }
  else if (transport == "tcp")
  {
    if (_failoverCount >= _tcpSrvTargets.size())
    {
      SIPMessage::Ptr serverError = _pServerRequest->createResponse(480, "All Routes Exhausted");
      OSS::IPAddress target;
      if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
      {
        if (target.isValid())
          _pServerTransaction->sendResponse(serverError, target);
      }

      std::ostringstream errorMsg;
      errorMsg << _logId << "Fatal Exception while calling SIPB2BTransaction::runFailoverTask() - Failover count exceeds TCP records available";
      OSS::log_error(errorMsg.str());
      releaseInternalRef();
      return;
    }

    unsigned short targetPort = _tcpSrvTargets.begin()->get<2>();
    if (targetPort == 0)
      targetPort = 5060;
    target = _tcpSrvTargets.begin()->get<1>();
    target.setPort(targetPort);
  }
  else if (transport == "tls")
  {
    if (_failoverCount >= _tlsSrvTargets.size())
    {
      SIPMessage::Ptr serverError = _pServerRequest->createResponse(480, "All Routes Exhausted");
      OSS::IPAddress target;
      if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
      {
        if (target.isValid())
          _pServerTransaction->sendResponse(serverError, target);
      }

      std::ostringstream errorMsg;
      errorMsg << _logId << "Fatal Exception while calling SIPB2BTransaction::runFailoverTask() - Failover count exceeds TLS records available";
      OSS::log_error(errorMsg.str());
      releaseInternalRef();
      return;
    }

    unsigned short targetPort = _tlsSrvTargets.begin()->get<2>();
    if (targetPort == 0)
      targetPort = 5061;
    target = _tlsSrvTargets.begin()->get<1>();
    target.setPort(targetPort);
  }

  if (!target.isValid())
  {
    //
    // Send a 500 server response and release the transaction
    //
    SIPMessage::Ptr serverError = _pServerRequest->createResponse(500, "DNS/SRV Error");
    OSS::IPAddress target;
    if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
    {
      if (target.isValid())
        _pServerTransaction->sendResponse(serverError, target);
    }

    std::ostringstream errorMsg;
    errorMsg << _logId << "Fatal Exception while calling SIPB2BTransaction::runFailoverTask() - Address not valid";
    OSS::log_error(errorMsg.str());
    releaseInternalRef();
    return;
  }

  _pClientRequest->setProperty("target-address", target.toIpPortString());

  //
  // change the via branch parameter
  //


  //
  // Prepare the new via
  //
  std::string oldVia;
  SIPVia::msgGetTopVia(_pClientRequest.get(), oldVia);
  SIPVia newVia(oldVia);
  std::string branch;
  SIPVia::getBranch(oldVia, branch);
  _pClientRequest->hdrListRemove("Via");
  std::ostringstream viaBranch;
  viaBranch << "z9hG4bK" << OSS::string_hash(branch.c_str());
  newVia.setParam("branch", viaBranch.str().c_str());
  _pClientRequest->hdrListPrepend("Via", newVia.data());

  //
  // Commit the changes
  //
  _pClientRequest->commitData();

  //
  // Send the request
  //
  OSS::SIP::SIPTransaction::Callback responseCallback
    = boost::bind(&SIPB2BTransaction::handleResponse, this, _1, _2, _3, _4);
  _pManager->stack().sendRequest(
    _pClientRequest,
    _localInterface,
    target,
    responseCallback);
}

void SIPB2BTransaction::handleResponse(
  const OSS::SIP::SIPTransaction::Error& e, 
  const OSS::SIP::SIPMessage::Ptr& pMsg, 
  const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
  const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  if (e)
    _pTransactionError = e;

  if (!_pClientTransaction)
    _pClientTransaction = pTransaction;

  if (!_pClientTransport)
    _pClientTransport = pTransport;

  //
  // Checks if this reponse needs to failvoer to another destination
  //
  if (isFailoverCandidate(e, pMsg, pTransport, pTransaction))
    return;
  
  //
  // Push to the response queue
  //
  _responseQueueMutex.lock();
  _responseQueue.push(pMsg);
  _responseQueueMutex.unlock();



  if (!_pManager->threadPool().schedule(boost::bind(&SIPB2BTransaction::runResponseTask, shared_from_this())))
  {
    OSS::log_error(_logId + "No available thread to handle SIPB2BTransaction::handleResponse");
  }
}

void SIPB2BTransaction::runResponseTask()
{
  try
  {
    OSS::mutex_lock reponseLock(_resposeMutex);
    if (_pTransactionError)
    {
      OSS::IPAddress localInterface;
      OSS::IPAddress target;
      std::string failoverAllowed;
      //
      // Signal the error and delete this transaction if a failover is not possible
      //
      _pManager->onTransactionError(_pTransactionError, SIPMessage::Ptr(), shared_from_this());
      releaseInternalRef();
      return;
    }

    SIPMessage::Ptr response;
    {//localize
      _responseQueueMutex.lock();
      response = _responseQueue.front();
      _responseQueue.pop();
      _responseQueueMutex.unlock();
    }//localize

    if (!response)
      throw OSS::SIP::SIPException("Response is NULL while calling SIPB2BTransaction::runResponseTask()");

    _pManager->onProcessResponseInbound(response, shared_from_this());

    if (_hasSentLocalResponse)
      return;

    if (response->isErrorResponse())
    {
      _pManager->onTransactionError(_pTransactionError, response, shared_from_this());

      OSS::IPAddress localInterface;
      OSS::IPAddress target;
      if (!_pServerRequest)
        throw OSS::SIP::SIPException("Server Request is NULL while calling SIPB2BTransaction::runResponseTask()");

      SIPMessage::Ptr pErrorResponse = _pServerRequest->reformatResponse(response);
      if (pErrorResponse)
      {
        std::string serverRequestPeerXor = "0";
        std::string clientRequestPeerXor = "0";
        _pClientRequest->getProperty("peer-xor", clientRequestPeerXor);
        _pServerRequest->getProperty("peer-xor", serverRequestPeerXor);
        response->setProperty("peer-xor", clientRequestPeerXor);
        pErrorResponse->setProperty("peer-xor", serverRequestPeerXor);

        OSS::IPAddress target;
        if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
        {
          if (target.isValid())
          {
            _pManager->onProcessResponseOutbound(pErrorResponse, shared_from_this());
             pErrorResponse->commitData();
            _pServerTransaction->sendResponse(pErrorResponse, target);
          }
        }
      }
      releaseInternalRef();
      return;
    }
    else if (response->is1xx())
    {
      if (!_pServerRequest)
          throw OSS::SIP::SIPException("Server Request is NULL while calling SIPB2BTransaction::runResponseTask()");

      SIPMessage::Ptr pProvisionalResponse = _pServerRequest->reformatResponse(response);
      if (pProvisionalResponse)
      {
        OSS::IPAddress target;
        if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
        {
          std::string serverRequestPeerXor = "0";
          std::string clientRequestPeerXor = "0";
          _pClientRequest->getProperty("peer-xor", clientRequestPeerXor);
          _pServerRequest->getProperty("peer-xor", serverRequestPeerXor);
          response->setProperty("peer-xor", clientRequestPeerXor);
          pProvisionalResponse->setProperty("peer-xor", serverRequestPeerXor);

          if (!pProvisionalResponse->body().empty())
            _pManager->onProcessResponseBody(pProvisionalResponse, shared_from_this());

          if (target.isValid())
          {
            _pManager->onProcessResponseOutbound(pProvisionalResponse, shared_from_this());
             pProvisionalResponse->commitData();
            _pServerTransaction->sendResponse(pProvisionalResponse, target);
          }
        }
      }
    }
    else if (response->is2xx())
    {
      if (!_pServerRequest)
          throw OSS::SIP::SIPException("Server Request is NULL while calling SIPB2BTransaction::runResponseTask()");

      SIPMessage::Ptr pFinalResponse = _pServerRequest->reformatResponse(response);

      if (pFinalResponse)
      {
        OSS::IPAddress target;
        if (onRouteResponse(_pServerRequest, _pServerTransport,_pServerTransaction, target))
        {
          std::string serverRequestPeerXor = "0";
          std::string clientRequestPeerXor = "0";
          _pClientRequest->getProperty("peer-xor", clientRequestPeerXor);
          _pServerRequest->getProperty("peer-xor", serverRequestPeerXor);
          response->setProperty("peer-xor", clientRequestPeerXor);
          pFinalResponse->setProperty("peer-xor", serverRequestPeerXor);

          if (!pFinalResponse->body().empty())
            _pManager->onProcessResponseBody(pFinalResponse, shared_from_this());

          pFinalResponse->setProperty("response-target", target.toIpPortString().c_str());
          pFinalResponse->setProperty("response-interface",
            _pServerTransport->getLocalAddress().toIpPortString().c_str());

          if (target.isValid())
          {
            _pManager->onProcessResponseOutbound(pFinalResponse, shared_from_this());
            pFinalResponse->commitData();
            _pServerTransaction->sendResponse(pFinalResponse, target);
          }
        }
      }

      releaseInternalRef();
      return;
    }
  }
  catch(OSS::Exception e)
  {
    std::ostringstream errorMsg;
    errorMsg << _logId << "Fatal Exception while calling SIPB2BTransaction::runResponseTask() - "
            << e.message();
    OSS::log_error(errorMsg.str());
    releaseInternalRef();
    return;
  }
}

void SIPB2BTransaction::setProperty(const std::string& property, const std::string& value)
{
  WriteLock lock(_rwlock);
  _properties[property] = value;
}

bool SIPB2BTransaction::getProperty(const std::string&  property, std::string& value) const
{
  ReadLock lock(_rwlock);
  CustomProperties::const_iterator iter = _properties.find(property);
  if (iter != _properties.end())
  {
    value = iter->second;
    return true;
  }
  return false;
}

bool SIPB2BTransaction::hasProperty(const std::string&  property) const
{
  ReadLock lock(_rwlock);
  return  _properties.find(property) != _properties.end();
}

bool SIPB2BTransaction::resolveSessionTarget(SIPMessage::Ptr& pClientRequest, OSS::IPAddress& target)
{
  std::string host;
  pClientRequest->getProperty("target-address", host);

  //
  // outbound-target is not set by the script.  try figuring it out ourselves
  // via the request-uri
  //
  SIPRequestLine requestLine = pClientRequest->getStartLine();
  SIPURI requestURI;
  if (!requestLine.getURI(requestURI))
    return false;

  std::string scheme = requestURI.getScheme();
  std::string transport = requestURI.getParam("transport");

  if (transport.empty())
  {
    //
    // Take note that the via is not set by the upper layer yet so we can't
    // use it as the basis of the transport here
    //
    if (!pClientRequest->getProperty("target-transport", transport) || transport.empty())
    {
      transport = "udp";
    }
  }

  if (scheme == "sips")
  {
    //
    // We do not support TLS yet
    //
    return false;
  }

  unsigned short port = 0;

  if (host.empty())
    requestURI.getHostPort(host, port);

  static OSS::ABNF::ABNFEvaluate<OSS::ABNF::ABNFSIPIPV4Address> isIPV4;
  static OSS::ABNF::ABNFEvaluate<OSS::ABNF::ABNFSIPIPV6Address> isIPV6;

  if (!port && !isIPV4(host.c_str()) && !isIPV6(host.c_str()))
  {
    //
    // Try DNS/SRV
    //
    std::string srvHost = "_sip._udp.";
    srvHost += host;
    if ((transport.empty() || transport == "udp") && scheme != "sips")
      _udpSrvTargets =  OSS::dns_lookup_srv(srvHost);
    
    srvHost = "_sip._tcp.";
    srvHost += host;
    if ((transport.empty() || transport == "tcp") && scheme != "sips")
      _tcpSrvTargets = OSS::dns_lookup_srv(srvHost);

    if (!_udpSrvTargets.empty())
    {
      //
      // Sort then get the first
      //
      unsigned short targetPort = _udpSrvTargets.begin()->get<2>();
      if (targetPort == 0)
        targetPort = 5060;
      target = _udpSrvTargets.begin()->get<1>();
      target.setPort(targetPort);

      pClientRequest->setProperty("target-transport", "udp");
    }

    if (!_tcpSrvTargets.empty())
    {
      //
      // sort then get the first transport
      //
      unsigned short targetPort = _tcpSrvTargets.begin()->get<2>();
      if (targetPort == 0)
        targetPort = 5060;
      target = _tcpSrvTargets.begin()->get<1>();
      target.setPort(targetPort);
      pClientRequest->setProperty("target-transport", "tcp");
    }

    if (_udpSrvTargets.empty() && _tcpSrvTargets.empty())
    {
      OSS::dns_host_record_list hosts = OSS::dns_lookup_host(host);
      if (hosts.empty())
      {
        return false;
      }
      if (port == 0)
        port = 5060;
      target = *(hosts.begin());
      target.setPort(port);
      pClientRequest->setProperty("target-transport", "udp");
    }
  }
  else
  {
    OSS::dns_host_record_list hosts = OSS::dns_lookup_host(host);
    if (hosts.empty())
    {
      return false;
    }
    if (port == 0)
      port = 5060;
    target = *(hosts.begin());
    target.setPort(port);
    pClientRequest->setProperty("target-transport", "udp");
  }

  return true;
}

} } // OSS::SIP

