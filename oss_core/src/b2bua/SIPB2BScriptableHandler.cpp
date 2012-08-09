// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
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


#include "OSS/SIP/SIPB2BScriptableHandler.h"


namespace OSS {
namespace SIP {


SIPB2BScriptableHandler::SIPB2BScriptableHandler(SIPB2BTransactionManager* pManager,
  OSS::SIP::SIPB2BHandler::MessageType type,
  const std::string& contextName) :
    OSS::SIP::SIPB2BHandler(type),
    _outboundScript(contextName),
    _outboundResponseScript(contextName),
    _pManager(pManager)
{

}


SIPB2BScriptableHandler::~SIPB2BScriptableHandler()
{

}

void SIPB2BScriptableHandler::initialize()
{

}

void SIPB2BScriptableHandler::deinitialize()
{

}

SIPMessage::Ptr onTransactionCreated(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  return SIPMessage::Ptr();
}

SIPMessage::Ptr SIPB2BScriptableHandler::onAuthenticateTransaction(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  return SIPMessage::Ptr();
}

SIPMessage::Ptr SIPB2BScriptableHandler::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  return SIPMessage::Ptr();
}


SIPMessage::Ptr SIPB2BScriptableHandler::onRouteOutOfDialogTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  return SIPMessage::Ptr();
}


SIPMessage::Ptr SIPB2BScriptableHandler::onProcessRequestBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  return SIPMessage::Ptr();
}

void SIPB2BScriptableHandler::onProcessResponseBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{

}

void SIPB2BScriptableHandler::onProcessOutbound(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{

}

bool SIPB2BScriptableHandler::onRouteResponse(
  const OSS::SIP::SIPMessage::Ptr& pRequest,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& target)
{
  return false;
}

SIPMessage::Ptr SIPB2BScriptableHandler::onGenerateLocalResponse(
  const OSS::SIP::SIPMessage::Ptr& pRequest,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  SIPB2BTransaction::Ptr pTransaction)
{
  return SIPMessage::Ptr();
}

void SIPB2BScriptableHandler::onProcessResponseInbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{

}

void SIPB2BScriptableHandler::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{

}

void SIPB2BScriptableHandler::onTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  SIPB2BTransaction::Ptr pTransaction)
{

}

void SIPB2BScriptableHandler::onDestroyTransaction(SIPB2BTransaction::Ptr pTransaction)
{

}

SIPMessage::Ptr SIPB2BScriptableHandler::onInvokeLocalHandler(
  const OSS::SIP::SIPMessage::Ptr& pRequest,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  SIPB2BTransaction::Ptr pTransaction)
{
  return SIPMessage::Ptr();
}


bool SIPB2BScriptableHandler::loadOutboundScript(const boost::filesystem::path& scriptFile, bool validate)
{
  return false;
}

bool SIPB2BScriptableHandler::loadOutboundResponseScript(const boost::filesystem::path& scriptFile, bool validate)
{
  return false;
}

void SIPB2BScriptableHandler::recompileScripts()
{

}


bool SIPB2BScriptableHandler::onClientTransactionCreated(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  return false;
}

bool SIPB2BScriptableHandler::onRouteClientTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  return false;
}

void SIPB2BScriptableHandler::onProcessClientResponse(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{

}

void SIPB2BScriptableHandler::onClientTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  SIPB2BTransaction::Ptr pTransaction)
{

}

} }  // OSS::SIP




