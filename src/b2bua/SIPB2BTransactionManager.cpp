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

#include <boost/tuple/tuple.hpp>
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"
#include "OSS/SIP/B2BUA/SIPB2BClientTransaction.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/Logger.h"
#include "OSS/Core.h"


namespace OSS {
namespace SIP {
namespace B2BUA {


SIPB2BTransactionManager::SIPB2BTransactionManager(int minThreadcount, int maxThreadCount) :
  _threadPool(minThreadcount, maxThreadCount),
  _stack(),
  _useSourceAddressForResponses(false),
  _pDefaultHandler(0)
{
  _stack.setRequestHandler(boost::bind(&SIPB2BTransactionManager::handleRequest, this, _1, _2, _3));
  _stack.setAckFor2xxTransactionHandler(boost::bind(&SIPB2BTransactionManager::handleAckFor2xxTransaction, this, _1, _2));
}

SIPB2BTransactionManager::~SIPB2BTransactionManager()
{
}

void SIPB2BTransactionManager::initialize(const boost::filesystem::path& cfgDirectory)
{
  OSS_VERIFY(!_sipConfigFile.empty());
  _transportConfigurationFile = operator/(cfgDirectory, _sipConfigFile);
  stack().initTransportFromConfig(_transportConfigurationFile);
}

void SIPB2BTransactionManager::deinitialize()
{
  //
  // Deinitialize all registed handlers
  //
  for( MessageHandlers::iterator iter = _handlers.begin();
    iter != _handlers.end(); iter++)
  {
    if (iter->second)
    {
      iter->second->deinitialize();
    }
  }
}

void SIPB2BTransactionManager::handleRequest(
  const OSS::SIP::SIPMessage::Ptr& pMsg, 
  const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
  const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  SIPB2BUserAgentHandler::Action action = _userAgentHandler(pMsg, pTransport, pTransaction);
  if (action == SIPB2BUserAgentHandler::Deny)
  {
    //
    // send a forbidden
    //
    OSS::log_error(pMsg->createContextId(true) + "SIPB2BTransactionManager::handleRequest - User Agent handler returned DENY");
    SIPMessage::Ptr serverError = pMsg->createResponse(403);
    pTransaction->sendResponse(serverError, pTransport->getRemoteAddress());
    return;
  }
  else if (action == SIPB2BUserAgentHandler::Handled)
  {
    //
    // Simply return.  A handler took ownership of the transaction
    //
    return;
  }

  //
  // No user agent handler too the transaction.
  //

  SIPB2BTransaction* b2bTransaction = onCreateB2BTransaction(pMsg, pTransport, pTransaction);
  if (!b2bTransaction)
    return;
  
#if SEND_ERROR_ON_B2BUA_THREAD_DEPLETION
  if (_threadPool.schedule(boost::bind(&SIPB2BTransaction::runTask, b2bTransaction)) == -1)
  {
    OSS::log_error(pMsg->createContextId(true) + "No available thread to handle SIPB2BTransactionManager::handleRequest");
    SIPMessage::Ptr serverError = pMsg->createResponse(500, "Thread Resource Depleted");
    pTransaction->sendResponse(serverError, pTransport->getRemoteAddress());
    delete b2bTransaction;
  }
#else
  //
  // The idea here is that if the threadpool runs out of threads, then we will directly
  // call runTask using the current thread which would effectively block the transport.
  // This is a good thing because blocking the transport yields our threadpool
  // allowing it to recover.
  //
  if (_threadPool.schedule(boost::bind(&SIPB2BTransaction::runTask, b2bTransaction)) == -1)
    b2bTransaction->runTask();
#endif
}

void SIPB2BTransactionManager::handleAckFor2xxTransaction(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport)
{
  SIPB2BHandler::Ptr pHandler = findHandler(SIPB2BHandler::TYPE_INVITE);
  if (pHandler)
  {
    if (_threadPool.schedule(boost::bind(&SIPB2BHandler::onProcessAckFor2xxRequest, pHandler, pMsg, pTransport)) == -1)
    {
      OSS::log_error(pMsg->createContextId(true) + "No available thread to handle SIPB2BTransactionManager::handleAckFor2xxTransaction");
    }
  }
  else if (_pDefaultHandler)
  {
    if (_threadPool.schedule(boost::bind(&SIPB2BHandler::onProcessAckFor2xxRequest, _pDefaultHandler, pMsg, pTransport)) == -1)
    {
      OSS::log_error(pMsg->createContextId(true) + "No available thread to handle SIPB2BTransactionManager::handleAckFor2xxTransaction");
    }
  }
}

SIPB2BTransaction* SIPB2BTransactionManager::onCreateB2BTransaction(
  const OSS::SIP::SIPMessage::Ptr& pMsg, 
  const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
  const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  SIPB2BTransaction* trn = new SIPB2BTransaction(this);
  trn->_pServerRequest = pMsg;
  trn->_pServerTransport = pTransport;
  trn->_pServerTransaction = pTransaction;
  return trn;
}

void SIPB2BTransactionManager::registerHandler(SIPB2BHandler::Ptr handler)
{
  OSS_ASSERT(handler);
  handler->initialize();
  _handlers[handler->getType()] = handler;
}

static SIPB2BHandler::MessageType getMessageType(const SIPMessage::Ptr& pRequest)
{
  std::string cseq = pRequest->hdrGet("cseq");
  if (cseq.empty())
    return SIPB2BHandler::TYPE_INVALID;
  OSS::string_to_upper(cseq);

  if (OSS::string_ends_with(cseq, "INVITE"))
    return SIPB2BHandler::TYPE_INVITE;
  else if (OSS::string_ends_with(cseq, "REGISTER"))
    return SIPB2BHandler::TYPE_REGISTER;
  else if (OSS::string_ends_with(cseq, "BYE"))
    return SIPB2BHandler::TYPE_BYE;
  else if (OSS::string_ends_with(cseq, "CANCEL"))
    return SIPB2BHandler::TYPE_CANCEL;
  else if (OSS::string_ends_with(cseq, "EXEC"))
    return SIPB2BHandler::TYPE_EXEC;
  else if (OSS::string_ends_with(cseq, "INFO"))
    return SIPB2BHandler::TYPE_INFO;
  else if (OSS::string_ends_with(cseq, "OPTIONS"))
    return SIPB2BHandler::TYPE_OPTIONS;
  else if (OSS::string_ends_with(cseq, "PRACK"))
    return SIPB2BHandler::TYPE_PRACK;
  else if (OSS::string_ends_with(cseq, "PUBLISH"))
    return SIPB2BHandler::TYPE_PUBLISH;
  else if (OSS::string_ends_with(cseq, "SUBSCRIBE"))
    return SIPB2BHandler::TYPE_SUBSCRIBE;
  else if (OSS::string_ends_with(cseq, "MESSAGE"))
    return SIPB2BHandler::TYPE_MESSAGE;
  else if (OSS::string_ends_with(cseq, "NOTIFY"))
    return SIPB2BHandler::TYPE_NOTIFY;
  else if (OSS::string_ends_with(cseq, "REFER"))
    return SIPB2BHandler::TYPE_REFER;
  else if (OSS::string_ends_with(cseq, "UPDATE"))
    return SIPB2BHandler::TYPE_UPDATE;
  else if (pRequest->isRequest())
    return SIPB2BHandler::TYPE_ANY;

  return SIPB2BHandler::TYPE_INVALID;
}

static SIPB2BHandler::MessageType getBodyType(const SIPMessage::Ptr& pRequest)
{
  std::string cseq = pRequest->hdrGet("cseq");
  if (cseq.empty())
    return SIPB2BHandler::TYPE_INVALID;
  OSS::string_to_upper(cseq);

  if (OSS::string_ends_with(cseq, "INVITE") ||
      OSS::string_ends_with(cseq, "UPDATE") ||
      OSS::string_ends_with(cseq, "ACK") ||
      OSS::string_ends_with(cseq, "PRACK"))
  {
    if (pRequest->getBody().empty())
      return SIPB2BHandler::TYPE_INVALID;

    std::string contentType = pRequest->hdrGet("content-type");
    OSS::string_to_lower(contentType);
    if (contentType != "application/sdp")
      return SIPB2BHandler::TYPE_INVALID;

    return SIPB2BHandler::TYPE_SDP;
  }

  return SIPB2BHandler::TYPE_INVALID;
}

SIPB2BHandler::Ptr SIPB2BTransactionManager::findHandler(const OSS::SIP::SIPMessage::Ptr& pMsg) const
{
  return findHandler(getMessageType(pMsg));
}

SIPB2BHandler::Ptr SIPB2BTransactionManager::findHandler(SIPB2BHandler::MessageType type) const
{
  MessageHandlers::const_iterator iter = _handlers.find(type);
  if (iter != _handlers.end() && iter->second)
    return iter->second;
  return SIPB2BHandler::Ptr();
}
    /// Returns the iterator for the request handler if one is registered

SIPMessage::Ptr SIPB2BTransactionManager::onTransactionCreated(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pRequest);
  if (pHandler)
  {
    return pHandler->onTransactionCreated(pRequest, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onTransactionCreated(pRequest, pTransaction);
  }
  return pRequest->createResponse(405, "No Corresponding Handler");
}

SIPMessage::Ptr SIPB2BTransactionManager::onAuthenticateTransaction(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  std::string maxForwards = pRequest->hdrGet("max-forwards");
  if (maxForwards.empty())
  {
    maxForwards = "70";
  }
  int maxF = OSS::string_to_number<int>(maxForwards.c_str());
  if (maxF == 0)
  {
    return pRequest->createResponse(SIPMessage::CODE_483_TooManyHops);
  }
  --maxF;
  pRequest->hdrRemove("max-forwards");
  pRequest->hdrSet("Max-Forwards", OSS::string_from_number(maxF).c_str());

  SIPB2BHandler::Ptr pHandler = findHandler(pRequest);
  if (pHandler)
  {
    return pHandler->onAuthenticateTransaction(pRequest, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onAuthenticateTransaction(pRequest, pTransaction);
  }
  return pRequest->createResponse(405, "No Corresponding Handler");
}

SIPMessage::Ptr SIPB2BTransactionManager::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pRequest);
  if (pHandler)
  {
    SIPMessage::Ptr result = pHandler->onRouteTransaction(pRequest, pTransaction, localInterface, target);
    //
    // _postRouteCallback is currently set by
    // the SBCStaticRouter class that allows static router
    // to bypass the results of the javascript layer.
    //
    if (_postRouteCallback)
      return _postRouteCallback(pRequest, result, pTransaction, localInterface, target);
    return result;
  }
  else if (_pDefaultHandler)
  {
    SIPMessage::Ptr result = _pDefaultHandler->onRouteTransaction(pRequest, pTransaction, localInterface, target);
    //
    // _postRouteCallback is currently set by
    // the SBCStaticRouter class that allows static router
    // to bypass the results of the javascript layer.
    //
    if (_postRouteCallback)
      return _postRouteCallback(pRequest, result, pTransaction, localInterface, target);
    return result;
  }
  return pRequest->createResponse(405, "No Corresponding Handler");
}

bool SIPB2BTransactionManager::onRouteResponse(
  const OSS::SIP::SIPMessage::Ptr& pRequest,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& target)
{
  /// This method requires the application layer to determine
  /// the target address of the response.
  if (_useSourceAddressForResponses || pTransport->isReliableTransport())
  {
    target = pTransport->getRemoteAddress();
    return true;
  }
  else
  {
    SIPB2BHandler::Ptr pHandler = findHandler(pRequest);
    if (pHandler)
    {
      return pHandler->onRouteResponse(pRequest, pTransport, pTransaction, target);
    }
    else if (_pDefaultHandler)
    {
      return _pDefaultHandler->onRouteResponse(pRequest, pTransport, pTransaction, target);
    }
    return false;
  }
}

SIPMessage::Ptr SIPB2BTransactionManager::onGenerateLocalResponse(
  const OSS::SIP::SIPMessage::Ptr& pRequest,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pRequest);
  if (pHandler)
  {
    return pHandler->onGenerateLocalResponse(pRequest, pTransport, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onGenerateLocalResponse(pRequest, pTransport, pTransaction);
  }
  return pRequest->createResponse(405, "No Corresponding Handler");
}

SIPMessage::Ptr SIPB2BTransactionManager::onInvokeLocalHandler(
  const OSS::SIP::SIPMessage::Ptr& pRequest,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pRequest);
  if (pHandler)
  {
    return pHandler->onInvokeLocalHandler(pRequest, pTransport, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onInvokeLocalHandler(pRequest, pTransport, pTransaction);
  }
  return pRequest->createResponse(405, "No Corresponding Handler");
}

SIPMessage::Ptr SIPB2BTransactionManager::onProcessRequestBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(getBodyType(pRequest));
  if (pHandler)
  {
    return pHandler->onProcessRequestBody(pRequest, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onProcessRequestBody(pRequest, pTransaction);
  }
  return pRequest->createResponse(405, "No Corresponding Handler");
}

void SIPB2BTransactionManager::onProcessResponseBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(getBodyType(pRequest));
  if (pHandler)
  {
    return pHandler->onProcessResponseBody(pRequest, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onProcessResponseBody(pRequest, pTransaction);
  }
}

void SIPB2BTransactionManager::onProcessOutbound(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pRequest);
  if (pHandler)
  {
    return pHandler->onProcessOutbound(pRequest, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onProcessOutbound(pRequest, pTransaction);
  }
}

void SIPB2BTransactionManager::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pResponse);
  if (pHandler)
  {
    return pHandler->onProcessResponseOutbound(pResponse, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onProcessResponseOutbound(pResponse, pTransaction);
  }
}

void SIPB2BTransactionManager::onProcessResponseInbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pResponse);
  if (pHandler)
  {
    return pHandler->onProcessResponseInbound(pResponse, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onProcessResponseInbound(pResponse, pTransaction);
  }
}


void SIPB2BTransactionManager::onTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pTransaction->serverRequest());
  if (pHandler)
  {
    return pHandler->onTransactionError(e, pErrorResponse, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onTransactionError(e, pErrorResponse, pTransaction);
  }
}

void SIPB2BTransactionManager::onDestroyTransaction(SIPB2BTransaction::Ptr pTransaction)
{
}

void SIPB2BTransactionManager::sendClientRequest(
  const OSS::SIP::SIPMessage::Ptr& pMsg)
{
  if (_threadPool.schedule(boost::bind(&SIPB2BTransaction::runTask, onCreateB2BClientTransaction(pMsg))) == -1)
  {
    OSS::log_error(pMsg->createContextId(true) + "No available thread to handle SIPB2BTransactionManager::sendClientRequest");
  }
}

SIPB2BTransaction* SIPB2BTransactionManager::onCreateB2BClientTransaction(
  const OSS::SIP::SIPMessage::Ptr& pMsg)
{
  SIPB2BClientTransaction* trn = new SIPB2BClientTransaction(this);
  trn->_pClientRequest = pMsg;
  return trn;
}

bool SIPB2BTransactionManager::onClientTransactionCreated(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pRequest);
  if (pHandler)
  {
    return pHandler->onClientTransactionCreated(pRequest, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onClientTransactionCreated(pRequest, pTransaction);
  }
  return false;
}

bool SIPB2BTransactionManager::onRouteClientTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pRequest);
  if (pHandler)
  {
    return pHandler->onRouteClientTransaction(pRequest, pTransaction, localInterface, target);
  }
  else if (_pDefaultHandler)
  {
    return _pDefaultHandler->onRouteClientTransaction(pRequest, pTransaction, localInterface, target);
  }
  return false;
}

void SIPB2BTransactionManager::onProcessClientResponse(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pResponse);
  if (pHandler)
  {
    pHandler->onProcessClientResponse(pResponse, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    _pDefaultHandler->onProcessClientResponse(pResponse, pTransaction);
  }
}


void SIPB2BTransactionManager::onClientTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  SIPB2BHandler::Ptr pHandler = findHandler(pErrorResponse);
  if (pHandler)
  {
    pHandler->onClientTransactionError(e, pErrorResponse, pTransaction);
  }
  else if (_pDefaultHandler)
  {
    _pDefaultHandler->onClientTransactionError(e, pErrorResponse, pTransaction);
  }
}

SIPMessage::Ptr SIPB2BTransactionManager::postMidDialogTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  return SIPMessage::Ptr();
}

bool SIPB2BTransactionManager::postRetargetTransaction(
    SIPMessage::Ptr& pRequest,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
{
  //
  // This is the chance of the transaction manager to hijack to processing of routing transactions.
  // Returning true here will mean the scripting engine will not be called
  //
  return false;
}

void SIPB2BTransactionManager::addUserAgentHandler(SIPB2BUserAgentHandler* pHandler)
{
  pHandler->setUserAgent(this);
  _userAgentHandler.addHandler(pHandler);
}
bool SIPB2BTransactionManager::registerPlugin(const std::string& name, const std::string& path)
{
  try
  {
    _pluginLoader.loadLibrary(path);
    SIPB2BUserAgentHandler* _pHandler = _pluginLoader.create(name);
    if (_pHandler)
    {
      addUserAgentHandler(_pHandler);
    }
  }
  catch(std::exception& e)
  {
    OSS_LOG_ERROR("SIPB2BTransactionManager::registerPlugin - Unable to load plugin " << path << " Error: " << e.what());
    return false;
  }

  return true;
}


} } } // OSS::SIP::B2BUA

