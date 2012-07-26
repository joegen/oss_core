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

#include <boost/tuple/tuple.hpp>
#include "OSS/SIP/SIPB2BTransactionManager.h"
#include "OSS/SIP/SIPB2BTransaction.h"
#include "OSS/SIP/SIPB2BClientTransaction.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/Logger.h"
#include "OSS/Core.h"

namespace OSS {
namespace SIP {


SIPB2BTransactionManager::SIPB2BTransactionManager(int minThreadcount, int maxThreadCount) :
  _threadPool(minThreadcount, maxThreadCount),
  _stack(),
  _useSourceAddressForResponses(false)
{
  _stack.setRequestHandler(boost::bind(&SIPB2BTransactionManager::handleRequest, this, _1, _2, _3));
  _stack.setUnknownInviteTransactionHandler(boost::bind(&SIPB2BTransactionManager::handleUnknowInviteTransaction, this, _1, _2));
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
  SIPB2BTransaction* b2bTransaction = onCreateB2BTransaction(pMsg, pTransport, pTransaction);
  if (!b2bTransaction)
    return;
  
#if 0
  if (!_threadPool.schedule(boost::bind(&SIPB2BTransaction::runTask, b2bTransaction)))
  {
    OSS::log_error(pMsg->createContextId(true) + "No available thread to handle SIPB2BTransactionManager::handleRequest");
    SIPMessage::Ptr serverError = pMsg->createResponse(500, "Thread Resource Depleted");
    pTransaction->sendResponse(serverError, pTransport->getRemoteAddress());
    delete b2bTransaction;
  }
#else
  b2bTransaction->runTask();
#endif
}

void SIPB2BTransactionManager::handleUnknowInviteTransaction(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport)
{
  MessageHandlers::iterator iter = _handlers.find(SIPB2BHandler::TYPE_INVITE);
  if (iter != _handlers.end() && iter->second)
  {
    if (!_threadPool.schedule(boost::bind(&SIPB2BHandler::onProcessUnknownInviteRequest, iter->second, pMsg, pTransport)))
    {
      OSS::log_error(pMsg->createContextId(true) + "No available thread to handle SIPB2BTransactionManager::handleUnknowInviteTransaction");
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

SIPMessage::Ptr SIPB2BTransactionManager::onTransactionCreated(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onTransactionCreated(pRequest, pTransaction);
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

  MessageHandlers::iterator iter = _handlers.find(getMessageType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onAuthenticateTransaction(pRequest, pTransaction);
  }
  return pRequest->createResponse(405, "No Corresponding Handler");
}

SIPMessage::Ptr SIPB2BTransactionManager::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    SIPMessage::Ptr result = iter->second->onRouteTransaction(pRequest, pTransaction, localInterface, target);
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
    MessageHandlers::iterator iter = _handlers.find(getMessageType(pRequest));
    if (iter != _handlers.end() && iter->second)
    {
      return iter->second->onRouteResponse(pRequest, pTransport, pTransaction, target);
    }
    return false;
  }
}

SIPMessage::Ptr SIPB2BTransactionManager::onGenerateLocalResponse(
  const OSS::SIP::SIPMessage::Ptr& pRequest,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onGenerateLocalResponse(pRequest, pTransport, pTransaction);
  }
  return pRequest->createResponse(405, "No Corresponding Handler");
}

SIPMessage::Ptr SIPB2BTransactionManager::onInvokeLocalHandler(
  const OSS::SIP::SIPMessage::Ptr& pRequest,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onInvokeLocalHandler(pRequest, pTransport, pTransaction);
  }
  return pRequest->createResponse(405, "No Corresponding Handler");
}

SIPMessage::Ptr SIPB2BTransactionManager::onProcessRequestBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getBodyType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onProcessRequestBody(pRequest, pTransaction);
  }

  iter = _handlers.find(getMessageType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onProcessRequestBody(pRequest, pTransaction);
  }

  return pRequest->createResponse(405, "No Corresponding Handler");
}

void SIPB2BTransactionManager::onProcessResponseBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getBodyType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onProcessResponseBody(pRequest, pTransaction);
  }

  iter = _handlers.find(getMessageType(pTransaction->clientRequest()));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onProcessResponseBody(pRequest, pTransaction);
  }
}

void SIPB2BTransactionManager::onProcessOutbound(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onProcessOutbound(pRequest, pTransaction);
  }
}

void SIPB2BTransactionManager::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pTransaction->serverRequest()));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onProcessResponseOutbound(pResponse, pTransaction);
  }
}

void SIPB2BTransactionManager::onProcessResponseInbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pTransaction->serverRequest()));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onProcessResponseInbound(pResponse, pTransaction);
  }
}


void SIPB2BTransactionManager::onTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pTransaction->serverRequest()));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onTransactionError(e, pErrorResponse, pTransaction);
  }
}

void SIPB2BTransactionManager::onDestroyTransaction(SIPB2BTransaction::Ptr pTransaction)
{
}

void SIPB2BTransactionManager::sendClientRequest(
  const OSS::SIP::SIPMessage::Ptr& pMsg)
{
  if (!_threadPool.schedule(boost::bind(&SIPB2BTransaction::runTask, onCreateB2BClientTransaction(pMsg))))
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
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onClientTransactionCreated(pRequest, pTransaction);
  }
  return false;
}

bool SIPB2BTransactionManager::onRouteClientTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pRequest));
  if (iter != _handlers.end() && iter->second)
  {
    return iter->second->onRouteClientTransaction(pRequest, pTransaction, localInterface, target);
  }
  return false;
}

void SIPB2BTransactionManager::onProcessClientResponse(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pResponse));
  if (iter != _handlers.end() && iter->second)
  {
    iter->second->onProcessClientResponse(pResponse, pTransaction);
  }
}


void SIPB2BTransactionManager::onClientTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  MessageHandlers::iterator iter = _handlers.find(getMessageType(pTransaction->serverRequest()));
  if (iter != _handlers.end() && iter->second)
  {
    iter->second->onClientTransactionError(e, pErrorResponse, pTransaction);
  }
}



} } // OSS::SIP

