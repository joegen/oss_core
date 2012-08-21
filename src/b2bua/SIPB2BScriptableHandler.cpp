
// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: B2BUA
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


#include "OSS/DNS.h"
#include "OSS/Logger.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/ABNF/ABNFSIPIPV4Address.h"
#include "OSS/ABNF/ABNFSIPIPV6Address.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/B2BUA/SIPB2BScriptableHandler.h"


namespace OSS {
namespace SIP {
namespace B2BUA {


SIPB2BScriptableHandler::SIPB2BScriptableHandler(
  SIPB2BTransactionManager* pManager,
  const std::string& contextName) :
  OSS::SIP::B2BUA::SIPB2BHandler(OSS::SIP::B2BUA::SIPB2BHandler::TYPE_ANY),
  _inboundScript(contextName),
  _authScript(contextName),
  _routeScript(contextName),
  _routeFailoverScript(contextName),
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

SIPMessage::Ptr SIPB2BScriptableHandler::onTransactionCreated(
  const SIPMessage::Ptr& pRequest, OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
  /// Called by runtask signalling the creation of the transaction.
  /// This precedes any other transaction callbacks and therefore is the best place
  /// to initialize anything that would be needed by the transaction processing
{
  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());

  if (_inboundScript.isInitialized())
  {
    if (!_inboundScript.processRequest(pRequest))
    {
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_500_InternalServerError);
      return serverError;
    }
  }

  if (pRequest->isMidDialog() && !pRequest->isRequest("SUBSCRIBE"))
    return _pManager->postMidDialogTransactionCreated(pRequest, pTransaction);

  return OSS::SIP::SIPMessage::Ptr();
}

void SIPB2BScriptableHandler::onDestroyTransaction(OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
    /// Signals that trhe transaction is about to be destroyed.
    /// This function will not invalidate the shared pointers
    /// to the transaction.  It is a mere indication that the transaction
    /// thread would now destroy its internal reference to the transaction.
{

}

SIPMessage::Ptr SIPB2BScriptableHandler::onAuthenticateTransaction(
  const SIPMessage::Ptr& pRequest, OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
  /// Authenticate the new Transaction request,
  ///
  /// This method is called from the B2B Transaction runTask().
  /// The intention of the function is to create a
  /// proxy-auth response.  If a null Ptr is returned by this
  /// callback, runTask will proceed in processing the request
  /// without authenticating it.
  ///
  ///
  /// Take note that authentication state is not maintained
  /// by the transaction.  It is the responsibility of the application
  /// to maintain the authentication state.
{
  if (_authScript.isInitialized())
  {
    std::string authMethod;
    pTransaction->getProperty("auth-method", authMethod);

    if (authMethod != "none")
    {
      if (!_authScript.processRequest(pRequest))
      {
        SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_500_InternalServerError);
        return serverError;
      }
    }
    else
    {
      pRequest->setProperty("auth-action", "accept");
      return OSS::SIP::SIPMessage::Ptr();
    }

    std::string authAction;
    if (!pRequest->getProperty("auth-action", authAction))
    {
      return SIPMessage::Ptr();
    }
    else if (authAction == "reject")
    {
      SIPMessage::Ptr reject = pRequest->createResponse(SIPMessage::CODE_403_Forbidden);
      std::string authResponse;
      if (pRequest->getProperty("auth-response", authResponse) && !authResponse.empty())
      {
        reject->setStartLine(authResponse);
        reject->commitData();
        return reject;
      }
      else
      {
        reject->commitData();
        return reject;
      }
    }
  }
  return OSS::SIP::SIPMessage::Ptr();
}

SIPMessage::Ptr SIPB2BScriptableHandler::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  if (pRequest->isMidDialog() && !pRequest->isRequest("SUBSCRIBE"))
  {
    return _pManager->postRouteMidDialogTransaction(pRequest, pTransaction, localInterface, target);
  }
  else if (SIPB2BContact::isRegisterRoute(pRequest))
  {
    return onRouteUpperReg(pRequest, pTransaction, localInterface, target);
  }
  else
  {
    return onRouteOutOfDialogTransaction(pRequest, pTransaction, localInterface, target);
  }
}

SIPMessage::Ptr SIPB2BScriptableHandler::onRouteOutOfDialogTransaction(
  SIPMessage::Ptr& pRequest,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  if (!_pManager->postRetargetTransaction(pRequest, pTransaction))
  {
    if (!_routeScript.isInitialized() || !_routeScript.processRequest(pRequest))
    {
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_500_InternalServerError);
      return serverError;
    }
  }

  //
  // Check if the route-script explicitly tells us to reject the call
  //
  std::string routeAction;
  if (pRequest->getProperty("route-action", routeAction))
  {
    if (routeAction == "reject")
    {
      std::string scode;
      SIPMessage::StatusCodes statusCode = SIPMessage::CODE_404_NotFound;
      if (pRequest->getProperty("reject-code", scode) && !scode.empty())
        statusCode = (SIPMessage::StatusCodes)OSS::string_to_number<int>(scode.c_str());

      std::string reason;
      SIPMessage::Ptr serverError;
      if (pRequest->getProperty("reject-reason", reason) && !reason.empty())
        serverError = pRequest->createResponse(statusCode, reason.c_str());
      else
        serverError = pRequest->createResponse(statusCode);
      return serverError;
    }
  }

  //
  // Check if the script provided an outbound target
  //


  static OSS::ABNF::ABNFEvaluate<OSS::ABNF::ABNFSIPIPV4Address> isIPV4;
  static OSS::ABNF::ABNFEvaluate<OSS::ABNF::ABNFSIPIPV6Address> isIPV6;

  std::string host;
  pRequest->getProperty("target-address", host);
  bool isIPAddress = !host.empty() && (isIPV4(host.c_str()) || isIPV6(host.c_str()));

  if (isIPAddress)
  {
    target = host;
    std::string port;
    if (pRequest->getProperty("target-port", port))
    {
      target.setPort(OSS::string_to_number<unsigned short>(port.c_str()));
    }
    else
    {
      target.setPort(5060);
    }
    //
    // Reset the properties so that outbound.js will have access to both the host and the target address
    //
    pRequest->setProperty("target-host", host);
    pRequest->setProperty("target-address", host);
  }
  else
  {
    try
    {
      if (!pTransaction->resolveSessionTarget(pRequest, target))
      {
        return pRequest->createResponse(404, "Unable to resolve ultimate target via DNS lookup.");
      }
    }
    catch(...)
    {
      return pRequest->createResponse(404, "Unable to resolve ultimate target via DNS lookup.");
    }
    //
    // Reset the properties so that outbound.js will have access to both the host and the target address
    //
    pRequest->setProperty("target-host", host);
    host = target.toString();
    pRequest->setProperty("target-address", host);

  }

  //
  // Automatically whitelist the target if the target was explicitly set by the script
  //
  if (host.empty())
    SIPTransportSession::rateLimit().whiteListAddress(target.address());

  //
  // Check if the script provided a local-interface
  //
  std::string localIface;
  if (pRequest->getProperty("interface-address", localIface) && !localIface.empty() && localIface != "auto")
  {
    localInterface = localIface;
    std::string port;
    if (pRequest->getProperty("interface-port", port))
    {
      localInterface.setPort(OSS::string_to_number<unsigned short>(port.c_str()));
    }
  }
  else
  {
    //
    // local-interface is not set by the script.  use the default interface
    //
    localInterface = _pManager->stack().transport().defaultListenerAddress();
  }

  return OSS::SIP::SIPMessage::Ptr();
}

SIPMessage::Ptr SIPB2BScriptableHandler::onRouteUpperReg(
  SIPMessage::Ptr& pRequest,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  return _pManager->postRouteUpperReg(pRequest, pTransaction, localInterface, target);
}

bool SIPB2BScriptableHandler::onRouteResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
    OSS::IPAddress& target)
   /// This is normally the place where the application can specify the
   /// target for a reponse.
{
  std::string strNatCompensation;
  bool disableNatCompensation = pTransaction->getProperty("disable-nat-compensation", strNatCompensation) && strNatCompensation == "1";


  std::string respondtoPacketSource;
  if (!disableNatCompensation && pTransaction->getProperty("respond-to-packet-source", respondtoPacketSource) && respondtoPacketSource == "1")
  {
    target = pTransport->getRemoteAddress();
    return true;
  }
  //
  // Check if the via is private
  //
  std::string hVia = pRequest->hdrGet("via");
  if (hVia.empty())
    return false;

  std::string topVia;
  if (!SIPVia::getTopVia(hVia, topVia))
    return false;

  std::string sentBy;
  if (!SIPVia::getSentBy(topVia, sentBy))
    return false;

  OSS::IPAddress viaHost(sentBy);
  if (!viaHost.isValid())
    viaHost = OSS::IPAddress::fromV4IPPort(sentBy.c_str());
  if (!viaHost.isValid())
  {
    target = pTransport->getRemoteAddress();
  }

  if (viaHost.isPrivate() && !disableNatCompensation)
  {
    target = pTransport->getRemoteAddress();
  }
  else
  {
    target = viaHost;
    if (viaHost.getPort() == 0)
    {
      viaHost.setPort(5060);
    }
  }
  return true;
}

SIPMessage::Ptr SIPB2BScriptableHandler::onGenerateLocalResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
{
  SIPMessage::Ptr ok = pRequest->createResponse(SIPMessage::CODE_200_Ok);
  return ok;
}

SIPMessage::Ptr SIPB2BScriptableHandler::onProcessRequestBody(
  SIPMessage::Ptr& pRequest,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
  /// This method allows the application to
  /// process the body of the request
  /// before it gets sent out.
  ///
  /// This method allows the upper layer to modify the body
  /// based on specific application requirements.  For
  /// example, a media proxy may modify SDP address and port
  /// so that RTP passes through the application.
  ///
  /// If the return value is an error response, the transaction
  /// will send it automatically to the sender.
  ///
  /// If the body is supported, the return value must be a null-Ptr.
{
  return OSS::SIP::SIPMessage::Ptr();
}

void SIPB2BScriptableHandler::onProcessResponseBody(
  SIPMessage::Ptr& pRequest,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
  /// This method allows the application to
  /// process the body of the response
  /// before it gets sent out.
  ///
  /// This method allows the upper layer to modify the body
  /// based on specific application requirements.  For
  /// example, a media proxy may modify SDP address and port
  /// so that RTP passes through the application.
  ///
{
}

void SIPB2BScriptableHandler::onProcessOutbound(
  SIPMessage::Ptr& pRequest,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
  /// This is the last chance for the application to process
  /// the outbound request before it gets sent out to the transport.
  ///
  /// This is normally the place where application would want to
  /// insert application-specific headers as well as change existing
  /// headers to the desired application-specific values for as long
  /// as it wont conflict with dialog creation states.
{
   pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  if (_outboundScript.isInitialized())
    _outboundScript.processRequest(pRequest);
   if (!_pManager->getUserAgentName().empty())
     pRequest->hdrSet("User-Agent", _pManager->getUserAgentName().c_str());
}

void SIPB2BScriptableHandler::onProcessResponseInbound(
  SIPMessage::Ptr& pResponse,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
{

}

void SIPB2BScriptableHandler::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
  /// This is the last chance for the application to process
  /// the outbound response before it gets sent out to the transport.
  ///
  /// This is normally the place where application would want to
  /// insert application-specific headers as well as change existing
  /// headers to the desired application-specific values for as long
  /// as it wont conflict with dialog creation states.
{
  pResponse->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  if (_outboundResponseScript.isInitialized())
    _outboundResponseScript.processRequest(pResponse);
  if (!_pManager->getUserAgentName().empty())
     pResponse->hdrSet("Server", _pManager->getUserAgentName().c_str());
}

void SIPB2BScriptableHandler::onTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
  /// Signals that an error occured on the transaction
  ///
  /// The transaction will be destroyed automatically after this function call
{
  if (e && !pErrorResponse)
  {
    SIPMessage::Ptr serverError = pTransaction->serverRequest()->createResponse(SIPMessage::CODE_408_RequestTimeout);
    OSS::IPAddress responseTarget;
    pTransaction->onRouteResponse(pTransaction->serverRequest(),
        pTransaction->serverTransport(), pTransaction->serverTransaction(), responseTarget);
    pTransaction->serverTransaction()->sendResponse(serverError, responseTarget);
  }
}

bool SIPB2BScriptableHandler::loadInboundScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE))
{
  bool ok = true;
  if (_inboundScript.isInitialized())
  {
    if (!_inboundScript.recompile())
    {
      ok = false;
    }
  }else
  {
    if (!_inboundScript.initialize(scriptFile, "handle_request", extensionGlobals))
    {
      ok = false;
    }
  }
  return ok;
}

bool SIPB2BScriptableHandler::loadAuthScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE))
{
  bool ok = true;
  if (_authScript.isInitialized())
    ok = _authScript.recompile();
  else
    ok = _authScript.initialize(scriptFile, "handle_request", extensionGlobals);

  return ok;

}

bool SIPB2BScriptableHandler::loadRouteScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE))
{
  bool ok = false;
  if (_routeScript.isInitialized())
    ok = _routeScript.recompile();
  else
    ok = _routeScript.initialize(scriptFile, "handle_request", extensionGlobals);
  return ok;
}

bool SIPB2BScriptableHandler::loadRouteFailoverScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE))
{
  bool ok = true;
  if (_routeFailoverScript.isInitialized())
    ok = _routeFailoverScript.recompile();
  else
    ok = _routeFailoverScript.initialize(scriptFile, "handle_request", extensionGlobals);
  return ok;
}

bool SIPB2BScriptableHandler::loadOutboundScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE))
{
  bool ok = true;

  if (_outboundScript.isInitialized())
    ok = _outboundScript.recompile();
  else
    ok = _outboundScript.initialize(scriptFile, "handle_request", extensionGlobals);
  return ok;
}

bool SIPB2BScriptableHandler::loadOutboundResponseScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE))
{
  bool ok = true;
  if (_outboundResponseScript.isInitialized())
    ok = _outboundResponseScript.recompile();
  else
    ok = _outboundResponseScript.initialize(scriptFile, "handle_request", extensionGlobals);
  return ok;
}

void SIPB2BScriptableHandler::recompileScripts()
{
  if (_inboundScript.isInitialized())
  {
    std::ostringstream logMsg;
    logMsg << "Recompiling script " << _inboundScript.getScriptFilePath();
    OSS::log_information(logMsg.str());
    _inboundScript.recompile();
  }
  if (_authScript.isInitialized())
  {
    std::ostringstream logMsg;
    logMsg << "Recompiling script " << _authScript.getScriptFilePath();
    OSS::log_information(logMsg.str());
    _authScript.recompile();
  }
  if (_routeScript.isInitialized())
  {
    std::ostringstream logMsg;
    logMsg << "Recompiling script " << _routeScript.getScriptFilePath();
    OSS::log_information(logMsg.str());
    _routeScript.recompile();
  }
  if (_routeFailoverScript.isInitialized())
  {
    std::ostringstream logMsg;
    logMsg << "Recompiling script " << _routeFailoverScript.getScriptFilePath();
    OSS::log_information(logMsg.str());
    _routeFailoverScript.recompile();
  }
  if (_outboundScript.isInitialized())
  {
    std::ostringstream logMsg;
    logMsg << "Recompiling script " << _outboundScript.getScriptFilePath();
    OSS::log_information(logMsg.str());
    _outboundScript.recompile();
  }
  if (_outboundResponseScript.isInitialized())
  {
    std::ostringstream logMsg;
    logMsg << "Recompiling script " << _outboundResponseScript.getScriptFilePath();
    OSS::log_information(logMsg.str());
    _outboundResponseScript.recompile();
  }
}

SIPMessage::Ptr SIPB2BScriptableHandler::onInvokeLocalHandler(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
{
  return SIPMessage::Ptr();
}

bool SIPB2BScriptableHandler::onClientTransactionCreated(const SIPMessage::Ptr& pRequest, OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
{
  return false;
}

bool SIPB2BScriptableHandler::onRouteClientTransaction(
  SIPMessage::Ptr& pRequest,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  return false;
}

void SIPB2BScriptableHandler::onProcessClientResponse(
  SIPMessage::Ptr& pResponse,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
{
}

void SIPB2BScriptableHandler::onClientTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
  /// Signals that an error occured on the transaction
  ///
  /// The transaction will be destroyed automatically after this function call
{
}

} } } // OSS::SIP::B2BUA


