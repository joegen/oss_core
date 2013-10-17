// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
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
#include "OSS/SIP/B2BUA/SIPB2BDialogStateManager.h"
#include "OSS/SIP/B2BUA/SIPB2BScriptableHandler.h"


namespace OSS {
namespace SIP {
namespace B2BUA {

  
using namespace OSS::RTP;

SIPB2BScriptableHandler::SIPB2BScriptableHandler(
  SIPB2BTransactionManager* pTransactionManager,
  SIPB2BDialogStateManager* pDialogState,
  const std::string& contextName) :
  OSS::SIP::B2BUA::SIPB2BHandler(OSS::SIP::B2BUA::SIPB2BHandler::TYPE_ANY),
  _inboundScript(contextName),
  _authScript(contextName),
  _routeScript(contextName),
  _routeFailoverScript(contextName),
  _outboundScript(contextName),
  _outboundResponseScript(contextName),
  _pTransactionManager(pTransactionManager),
  _pDialogState(pDialogState),
  _2xxRetransmitCache(32),
  _pOptionsThread(0),
  _optionsThreadExit(0, 0xFFFF),
  _pOptionsResponseThread(0),
  _optionsResponseThreadExit(0, 0xFFFF),
  _threadPool(1, 10)
{
  _keepAliveResponseCb = boost::bind(&SIPB2BScriptableHandler::handleOptionsResponse, this, _1, _2, _3, _4);
  //
  // Initialize the options keep-alive thread
  //
  OSS_ASSERT(_pOptionsResponseThread == 0);
  OSS_ASSERT(_pOptionsThread == 0);
  _pOptionsThread = new boost::thread(boost::bind(&SIPB2BScriptableHandler::runOptionsThread, this));
  _pOptionsResponseThread = new boost::thread(boost::bind(&SIPB2BScriptableHandler::runOptionsResponseThread, this));
}

SIPB2BScriptableHandler::~SIPB2BScriptableHandler()
{
  //
  // Exit the option keep-alive loop
  //
  _optionsThreadExit.set();
  _pOptionsThread->join();
  _optionsResponseThreadExit.set();
  _optionsResponseQueue.enqueue("exit");
  _pOptionsResponseThread->join();
  delete _pOptionsThread;
  delete _pOptionsResponseThread;
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
  //
  // Force RTP proxy by default
  //
  if (pRequest->isRequest("INVITE"))
  {
    pTransaction->setProperty("require-rtp-proxy", "1");
  }
  else if (pRequest->isRequest("BYE"))
  {
    if (!pRequest->isMidDialog())
    {
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest);
      return serverError;
    }
  }

  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());

  if (_inboundScript.isInitialized())
  {
    if (!_inboundScript.processRequest(pRequest))
    {
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_500_InternalServerError);
      return serverError;
    }
  }
  else if (!onProcessRequest(TYPE_INBOUND, pRequest))
  {
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_500_InternalServerError);
    return serverError;
  }

  if (pRequest->isMidDialog() && !pRequest->isRequest("SUBSCRIBE"))
    return _pTransactionManager->postMidDialogTransactionCreated(pRequest, pTransaction);

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
  else 
  {
    if (!onProcessRequest(TYPE_AUTH, pRequest))
    {
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_500_InternalServerError);
      return serverError;
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

  if (pRequest->isRequest("CANCEL"))
  {
    std::string inviteId;
    SIPMessage::Ptr pInvite;
    pRequest->getTransactionId(inviteId, "invite");
    {
      OSS::mutex_read_lock _rwlock(_rwInvitePoolMutex);
      std::map<std::string, SIPMessage::Ptr>::iterator inviteIter = _invitePool.find(inviteId);
      if (inviteIter == _invitePool.end())
      {
        SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_481_TransactionDoesNotExist );
        return serverError;
      }
      pInvite = inviteIter->second;
    }

    pRequest.reset();
    pRequest = SIPMessage::Ptr(new SIPMessage(*(pInvite.get())));

    SIPRequestLine startLine = pRequest->startLine();
    startLine.setMethod("CANCEL");
    pRequest->setStartLine(startLine.data());

    SIPCSeq cseq = pRequest->hdrGet("cseq");
    cseq.setMethod("CANCEL");
    pRequest->hdrRemove("cseq");
    pRequest->hdrSet("CSeq", cseq.data().c_str());

    pRequest->setBody("");
    pRequest->hdrRemove("content-length");
    pRequest->hdrSet("Content-Length", "0");
    //
    // Remove headers that do not have semantics in CANCEL
    //
    pRequest->hdrRemove("content-type");
    pRequest->hdrRemove("min-se");
    pRequest->hdrRemove("allow");
    pRequest->hdrRemove("supported");
    pRequest->hdrRemove("session-expires");
    pRequest->hdrRemove("proxy-authorization");
    pRequest->hdrRemove("authorization");

    std::string isXorValue;
    if (pInvite->getProperty("xor", isXorValue) && isXorValue == "1")
      pRequest->setProperty("xor", "1");

    std::string localAddress;
    OSS_VERIFY(pInvite->getProperty("local-address", localAddress));
    pRequest->setProperty("local-address", localAddress);

    std::string targetAddress;
    OSS_VERIFY(pInvite->getProperty("target-address", targetAddress));
    pRequest->setProperty("target-address", targetAddress);

    std::string targetTransport;
    OSS_VERIFY(pInvite->getProperty("target-transport", targetTransport));
    pRequest->setProperty("target-transport", targetTransport);

    localInterface = IPAddress::fromV4IPPort(localAddress.c_str());
    OSS_VERIFY(localInterface.isValid());

    target = IPAddress::fromV4IPPort(targetAddress.c_str());
    OSS_VERIFY(target.isValid());

    return OSS::SIP::SIPMessage::Ptr();
  }
  else if (pRequest->isRequest("OPTIONS"))
  {
    //
    // We handle options locally
    //
    SIPMessage::Ptr ok = pRequest->createResponse(SIPMessage::CODE_200_Ok);
    return ok;
  }

  bool isInvite = pRequest->isRequest("INVITE");
  if (pRequest->isMidDialog() && !pRequest->isRequest("SUBSCRIBE"))
  {
    SIPMessage::Ptr ret = _pDialogState->onRouteMidDialogTransaction(pRequest, pTransaction, localInterface, target);
    if (isInvite)
      pTransaction->setProperty("reinvite", "1");

    //
    // if ret is error and this is a NOTIFY, try to send it unsolicited
    //
    SIPMessage::Ptr notifyRet;
    if (ret && pRequest->isRequest("NOTIFY") && SIPB2BContact::isRegisterRoute(pRequest))
    {
      notifyRet = onRouteUpperReg(pRequest, pTransaction, localInterface, target);
      if (notifyRet)
        return ret;
    }
    else
    {
      return ret;
    }
  }
  else if (SIPB2BContact::isRegisterRoute(pRequest))
  {
    SIPMessage::Ptr ret = onRouteUpperReg(pRequest, pTransaction, localInterface, target);
    if (ret)
      return ret;
  }
  else
  {
    SIPMessage::Ptr ret = onRouteOutOfDialogTransaction(pRequest, pTransaction, localInterface, target);
    if (ret)
    {
      //
      // Route transaction produced a response.  This normally means an error occured in routing
      //
      return ret;
    }
  }


  if (pRequest->isRequest("REGISTER"))
  {
    std::string invokeLocalHandler = "0";
    if (pTransaction->getProperty("invoke-local-handler", invokeLocalHandler ) && invokeLocalHandler == "1")
    {
      return SIPMessage::Ptr();
    }

    pRequest->hdrListRemove("Route");
    pRequest->hdrListRemove("Record-Route");
    pRequest->hdrListRemove("Via");

    std::string targetTransport;
    if (!pRequest->getProperty("target-transport", targetTransport) || targetTransport.empty())
      targetTransport = "udp";

    //
    // Prepare the new via
    //
    OSS::string_to_upper(targetTransport);
    std::string viaBranch = "z9hG4bK";
    viaBranch += OSS::string_create_uuid();
    std::string newVia = SIPB2BContact::constructVia(_pTransactionManager, pRequest, localInterface, targetTransport, viaBranch);
    pRequest->hdrListPrepend("Via", newVia);

    if (!SIPB2BContact::transformRegister(_pTransactionManager, pRequest, pTransaction, localInterface))
    {
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest);
      return serverError;
    }
  }
  else
  {
    //
    // This is a new out o dialog transaction
    //
    if (_pDialogState->hasDialog(pRequest->hdrGet("call-id")))
    {
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Dialog already exist!");
      return serverError;
    }


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
    // Set the target tranport
    //
    std::string targetTransport;
    if (!pRequest->getProperty("target-transport", targetTransport) || targetTransport.empty())
    {
      targetTransport = "udp";
      pRequest->setProperty("target-transport", "udp");
    }
    //
    // Prepare the new via
    //
    OSS::string_to_upper(targetTransport);
    std::ostringstream viaBranch;
    viaBranch << "z9hG4bK" << OSS::string_hash(branch.c_str());
    std::string newVia = SIPB2BContact::constructVia(_pTransactionManager, pRequest, localInterface, targetTransport, viaBranch.str());
    pRequest->hdrListPrepend("Via", newVia);
    //
    // Prepare the new contact
    //
    std::ostringstream sessionId;
    sessionId << OSS::string_hash(pRequest->hdrGet("call-id").c_str()) << OSS::getRandom();
    pTransaction->setProperty("session-id", sessionId.str());

    SIPB2BContact::SessionInfo sessionInfo;
    sessionInfo.sessionId = sessionId.str();
    sessionInfo.callIndex = 2;

    SIPB2BContact::transform(_pTransactionManager,
      pRequest,
      pTransaction,
      localInterface,
      sessionInfo);
  }
  return OSS::SIP::SIPMessage::Ptr();
}

SIPMessage::Ptr SIPB2BScriptableHandler::onRouteOutOfDialogTransaction(
  SIPMessage::Ptr& pRequest,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  if (!_pTransactionManager->postRetargetTransaction(pRequest, pTransaction))
  {
    if (_routeScript.isInitialized())
    {
      if (!_routeScript.processRequest(pRequest))
      {
        SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_500_InternalServerError);
        return serverError;
      }
    }
    else if (!onProcessRequest(TYPE_ROUTE, pRequest))
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
    localInterface = _pTransactionManager->stack().transport().defaultListenerAddress();
  }

  return OSS::SIP::SIPMessage::Ptr();
}

SIPMessage::Ptr SIPB2BScriptableHandler::onRouteUpperReg(
  SIPMessage::Ptr& pRequest,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localInterface,
  OSS::IPAddress& target)
{
  SIPRequestLine rline = pRequest->startLine();
  SIPURI ruri;
  if (!rline.getURI(ruri))
  {
   SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Unable to parse request-uri");
   return serverError;
  }

  std::string regId;
  if (!getRegistrationId(ruri, regId))
  {
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Unable to parse registration ID from request-URI");
   return serverError;
  }

  try
  {
    RegData registration;
    if (!_pDialogState->findOneRegistration(regId, registration))
    {
      OSS_LOG_WARNING("No registration found for " << ruri.data());
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_404_NotFound);
      return serverError;
    }

    SIPTo to(registration.aor);
    localInterface = IPAddress::fromV4IPPort(registration.localInterface.c_str());
    target = IPAddress::fromV4IPPort(registration.packetSource.c_str());

    std::string contact;
    contact = registration.contact;
    SIPFrom contactHeader = contact;
    IPAddress potentialTarget = IPAddress::fromV4IPPort(contactHeader.getHostPort().c_str());
    if (potentialTarget.isValid() && !potentialTarget.isPrivate())
      target = potentialTarget;

    //
    // Check if the contact is non-UDP.  We will try to recycle a persistent connection for TCP
    //
    std::string transportScheme = registration.targetTransport;
    OSS::string_to_upper(transportScheme);


    pRequest->setProperty("target-transport", transportScheme.c_str());

    std::string transportId = registration.transportId;
    if (!transportId.empty())
        pRequest->setProperty("transport-id", transportId.c_str());

    //
    // Check if this call is to be encrypted
    //
    if (registration.enc)
      pRequest->setProperty("xor", "1");


    std::ostringstream requestLine;
    requestLine << pRequest->getMethod() << " " << contact << " SIP/2.0";
    pRequest->setStartLine(requestLine.str().c_str());
    pRequest->hdrSet("to", to.getURI().c_str());
    //
    // Proxy media for upper reg if an rtp proxy is implemented
    //
    pTransaction->setProperty("require-rtp-proxy", "1");
  }
  catch(OSS::Exception e)
  {
    OSS_LOG_ERROR("Unable to process " << regId << " Error=" << e.what());
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_404_NotFound);
    return serverError;
  }
  return OSS::SIP::SIPMessage::Ptr();
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
  pTransaction->getProperty("respond-to-packet-source", respondtoPacketSource);
  if (!disableNatCompensation && respondtoPacketSource != "0")
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
  //
  // If target is still invalid, we simply use the sent-by
  //
  target = viaHost;
  if (viaHost.getPort() == 0)
  {
    viaHost.setPort(5060);
  }
  return target.isValid();
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
  std::string contentType =  pRequest->hdrGet("content-type");
  OSS::string_to_lower(contentType);
  if (contentType != "application/sdp")
    return OSS::SIP::SIPMessage::Ptr();

  std::string noRTPProxy;
  if (pTransaction->getProperty("no-rtp-proxy", noRTPProxy) && noRTPProxy == "1")
    return OSS::SIP::SIPMessage::Ptr();

  //
  // do not handle SDP for SIP over websockets.  It is using ICE
  // to traverse NAT.  Media anchor will mess that up.
  //
  if (pTransaction->serverTransport() && pTransaction->serverTransport()->getTransportScheme() == "ws")
    return OSS::SIP::SIPMessage::Ptr();
  
  std::string sessionId;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));

  SIPMessage* pServerRequest = pTransaction->serverRequest().get();
  std::string bottomVia;
  if (!SIPVia::msgGetBottomVia(pServerRequest, bottomVia))
  {
    SIPMessage::Ptr serverError = pServerRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Bad bottom via header");
    return serverError;
  }

 std::string sentBy;
 if (!SIPVia::getSentBy(bottomVia, sentBy))
  {
    SIPMessage::Ptr serverError = pServerRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Bad bottom via header");
    return serverError;
  }

  pTransaction->setProperty("sdp-answer-route", sentBy);

  OSS::IPAddress addrSentBy = OSS::IPAddress::fromV4IPPort(sentBy.c_str());
  OSS::IPAddress addrPacketSource = pTransaction->serverTransport()->getRemoteAddress();

  //
  // Set the local interface to be used to send to A Leg.
  // Also set the external address if one is configured.
  //
  OSS::IPAddress addrLocalInterface = pTransaction->serverTransport()->getLocalAddress();
  addrLocalInterface.externalAddress() = pTransaction->serverTransport()->getExternalAddress();

  std::string targetAddress;
  std::string localAddress;
  OSS_VERIFY(pRequest->getProperty("target-address", targetAddress) &&
    pRequest->getProperty("local-address", localAddress));

  OSS::IPAddress addrRoute = OSS::IPAddress::fromV4IPPort(targetAddress.c_str());
  OSS::IPAddress addrRouteLocalInterface = OSS::IPAddress::fromV4IPPort(localAddress.c_str());

  //
  // Assign the exteral address for B-Leg if one is configured
  //
  std::string routeExternalAddress;
  if (getExternalAddress(addrRouteLocalInterface, routeExternalAddress))
  {
    addrRouteLocalInterface.externalAddress() = routeExternalAddress;
  }

  std::string rtpProxyProp;
  bool requireRTPProxy = pTransaction->getProperty("require-rtp-proxy", rtpProxyProp) && rtpProxyProp == "1";
  std::string sdp = pRequest->getBody();
  try
  {
    RTPProxy::Attributes rtpAttributes;

    rtpAttributes.verbose = false;
    std::string propXOR = "0";
    rtpAttributes.forcePEAEncryption = pRequest->getProperty("peer-xor", propXOR) && propXOR == "1";
    rtpAttributes.forceCreate = requireRTPProxy;
    rtpAttributes.callId = pRequest->hdrGet("call-id");
    rtpAttributes.from = pRequest->hdrGet("from");
    rtpAttributes.to = pRequest->hdrGet("to");

    std::string sResizerSamples;
    if (!pServerRequest->getProperty("rtp-resizer-samples", sResizerSamples))
      pRequest->getProperty("rtp-resizer-samples", sResizerSamples);

    if (!sResizerSamples.empty())
    {
      try
      {
        std::vector<std::string> tokens = OSS::string_tokenize(sResizerSamples, "/");
        if (tokens.size() == 2)
        {
          rtpAttributes.resizerSamplesLeg1 = boost::lexical_cast<int>(tokens[0]);
          rtpAttributes.resizerSamplesLeg2 = boost::lexical_cast<int>(tokens[1]);
          OSS_LOG_INFO("RTP: Activating RTP repacketization to " << rtpAttributes.resizerSamplesLeg1 << "/" << rtpAttributes.resizerSamplesLeg2 << " resolution.")
        }
        else
        {
          rtpAttributes.resizerSamplesLeg1 = boost::lexical_cast<int>(sResizerSamples);
          rtpAttributes.resizerSamplesLeg2 = boost::lexical_cast<int>(sResizerSamples);
          OSS_LOG_INFO("RTP: Activating RTP repacketization to " << rtpAttributes.resizerSamplesLeg1 << "/" << rtpAttributes.resizerSamplesLeg2 << " resolution.")
        }
      }
      catch(...)
      {
      }
    }
    else
    {
      OSS_LOG_INFO("RTP: RTP repacketization is DISABLED.");
    }


    std::string maximumChannel;
    if (pRequest->getProperty("max-channel", maximumChannel) && !maximumChannel.empty())
      rtpAttributes.countSessions = true;


    rtpProxy().handleSDP(pTransaction->getLogId(), sessionId, addrSentBy, addrPacketSource, addrLocalInterface,
      addrRoute, addrRouteLocalInterface, RTPProxySession::INVITE, sdp, rtpAttributes);


  }
  catch(RTPProxyTooManySession e)
  {
    SIPMessage::Ptr serverError = pServerRequest->createResponse(SIPMessage::CODE_503_ServiceUnavailable, e.message().c_str());
    return serverError;
  }
  catch(OSS::Exception e)
  {
    SIPMessage::Ptr serverError = pServerRequest->createResponse(SIPMessage::CODE_400_BadRequest, e.message().c_str());
    return serverError;
  }


  pRequest->setBody(sdp);
  std::string clen = OSS::string_from_number<size_t>(sdp.size());
  pRequest->hdrSet("Content-Length", clen.c_str());
  return OSS::SIP::SIPMessage::Ptr();
}

void SIPB2BScriptableHandler::onProcessResponseBody(
  SIPMessage::Ptr& pResponse,
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
  //
  // Ignore OPTIONS response
  //
  if (pResponse->isResponseTo("OPTIONS"))
    return;

  std::string contentType = pResponse->hdrGet("content-type");
  OSS::string_to_lower(contentType);
  if (contentType != "application/sdp")
    return;

  std::string noRTPProxy;
  if (pTransaction->getProperty("no-rtp-proxy", noRTPProxy) && noRTPProxy == "1")
    return;

  //
  // do not handle SDP for SIP over websockets.  It is using ICE
  // to traverse NAT.  Media anchor will mess that up.
  //
  if (pTransaction->serverTransport() && pTransaction->serverTransport()->getTransportScheme() == "ws")
    return;

  //
  // Note:  We should stop caching responses since a foked INVITE can have
  // multiple SDP responses
  //
  std::string responseSDP;
  if (pTransaction->getProperty("response-sdp", responseSDP))
  {
    pResponse->setBody(responseSDP);
    std::string clen = OSS::string_from_number<size_t>(responseSDP.size());
    pResponse->hdrSet("Content-Length", clen.c_str());
    return;
  }

  std::string hContact = pResponse->hdrGet("contact");
  std::string sentBy;

  if (!hContact.empty())
  {
    ContactURI contactURI;
    if (!SIPContact::getAt(hContact, contactURI, 0))
      return;
    sentBy = contactURI.getHostPort();
  }

  std::string sessionId;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));

  std::string sdpAnswerRoute;
  if (!pTransaction->getProperty("sdp-answer-route", sdpAnswerRoute))
  {
    SIPMessage* pServerRequest = pTransaction->serverRequest().get();
    std::string bottomVia;
    if (!SIPVia::msgGetBottomVia(pServerRequest, bottomVia))
    {
      return;
    }
    if (!SIPVia::getSentBy(bottomVia, sdpAnswerRoute))
    {
      return;
    }
    pTransaction->setProperty("sdp-answer-route", sdpAnswerRoute);
  }

  OSS::IPAddress addrSentBy;
  if (!sentBy.empty())
    addrSentBy = OSS::IPAddress::fromV4IPPort(sentBy.c_str());
  else
    addrSentBy = pTransaction->clientTransport()->getRemoteAddress();

  OSS::IPAddress addrPacketSource = pTransaction->clientTransport()->getRemoteAddress();
  OSS::IPAddress addrLocalInterface = pTransaction->clientTransport()->getLocalAddress();
  addrLocalInterface.externalAddress() = pTransaction->clientTransport()->getExternalAddress();

  OSS::IPAddress addrRoute = OSS::IPAddress::fromV4IPPort(sdpAnswerRoute.c_str());
  OSS::IPAddress addrRouteLocalInterface = pTransaction->serverTransport()->getLocalAddress();
  addrRouteLocalInterface.externalAddress() = pTransaction->serverTransport()->getExternalAddress();

  std::string sdp = pResponse->getBody();
  std::string rtpProxyProp;
  bool requireRTPProxy = pTransaction->getProperty("require-rtp-proxy", rtpProxyProp) && rtpProxyProp == "1";

  std::string enableVerboseRTP;
  bool verboseRTP = pTransaction->getProperty("enable-verbose-rtp", enableVerboseRTP) && enableVerboseRTP == "1";
  try
  {
    RTPProxy::Attributes rtpAttributes;
    rtpAttributes.verbose = verboseRTP;
    std::string propXOR = "0";
    rtpAttributes.forcePEAEncryption = pResponse->getProperty("peer-xor", propXOR) && propXOR == "1";
    rtpAttributes.forceCreate = requireRTPProxy;
    rtpAttributes.callId = pResponse->hdrGet("call-id");
    rtpAttributes.from = pResponse->hdrGet("from");
    rtpAttributes.to = pResponse->hdrGet("to");

    rtpProxy().handleSDP(pTransaction->getLogId(), sessionId, addrSentBy, addrPacketSource, addrLocalInterface,
        addrRoute, addrRouteLocalInterface, RTPProxySession::INVITE_RESPONSE, sdp, rtpAttributes);
  }
  catch(OSS::Exception e)
  {
    std::ostringstream logMsg;
    logMsg << pTransaction->getLogId() << "Unable to process SDP in response.  Exception: " << e.message();
    OSS::log_warning(logMsg.str());
    return;
  }

  pResponse->setBody(sdp);
  std::string clen = OSS::string_from_number<size_t>(sdp.size());
  pTransaction->setProperty("response-sdp", sdp.c_str());
  pResponse->hdrSet("Content-Length", clen.c_str());
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

  if (pRequest->isRequest("INVITE"))
  {
    std::string id;
    if (pTransaction->serverRequest()->getTransactionId(id))
    {
      OSS::mutex_write_lock _rwlock(_rwInvitePoolMutex);
      _invitePool[id] = pRequest;
    }
    //
    // Send a 100 Trying.  Take note that 100 trying may be delayed if route transaction,
    // particularly DNS lookups takes longer than expected.  Delaying 100 Trying after
    // we get a route is necessary because we won't be able to route the CANCEL request
    // if they ever arrive while we are still fetching the route to the INVITE
    //
    SIPMessage::Ptr trying = pTransaction->serverRequest()->createResponse(SIPMessage::CODE_100_Trying);
    OSS::IPAddress responseTarget;
    pTransaction->onRouteResponse(pTransaction->serverRequest(),
      pTransaction->serverTransport(), pTransaction->serverTransaction(), responseTarget);
    pTransaction->serverTransaction()->sendResponse(trying, responseTarget);
  }
  else if (pRequest->isRequest("BYE"))
  {
    std::string sessionId;
    OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
    //
    // Remove the rtp proxies if they were created
    //
    try
    {
      rtpProxy().removeSession(sessionId);
    }catch(...){}
  }

  if (_outboundScript.isInitialized())
    _outboundScript.processRequest(pRequest);
  else
    onProcessRequest(TYPE_OUTBOUND_REQUEST, pRequest);

   if (!_pTransactionManager->getUserAgentName().empty())
     pRequest->hdrSet("User-Agent", _pTransactionManager->getUserAgentName().c_str());


}

bool SIPB2BScriptableHandler::getRegistrationId(const ContactURI& curi, std::string& regId) const
{
   SIPURI binding = curi.getURI();
   return getRegistrationId(binding, regId);
}

bool SIPB2BScriptableHandler::getRegistrationId(const SIPURI& binding, std::string& regId) const
{
  std::string user = binding.getUser();
  regId = "";
  if (user.find(SIPB2BContact::_regPrefix) != std::string::npos)
  {
    regId = user;
  }
  else
  {
    if (binding.hasParam(SIPB2BContact::_regPrefix))
    {
      regId = SIPB2BContact::_regPrefix;
      regId += "-";
      regId += user;
      regId += "-";
      regId += binding.getParam(SIPB2BContact::_regPrefix);
    }
  }
  return !regId.empty();
}

void SIPB2BScriptableHandler::onProcessResponseInbound(
  SIPMessage::Ptr& pResponse,
  OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction)
{
  bool isDialogForming = false;
  bool isInvite = pResponse->isResponseTo("INVITE");
  if (isInvite || pResponse->isResponseTo("SUBSCRIBE"))
  {
    isDialogForming = true;
  }
  else if (pResponse->isResponseTo("REFER"))
  {
    //
    // Out-of-dialog REFER.  This is an implied subscription so treat it as a SUBSCRIBE
    //
    isDialogForming = !pTransaction->serverRequest()->isMidDialog();
  }

  if (isDialogForming)
  {
    if (isInvite && pResponse->is2xx())
    {
      std::string id;
      if (pTransaction->serverRequest()->getTransactionId(id))
      {
        OSS::mutex_write_lock _rwlock(_rwInvitePoolMutex);
        _invitePool.erase(id);
      }
    }

    std::string isReinvite;
    if (pTransaction->getProperty("reinvite", isReinvite))
    {
      std::string sessionId;
      OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
      _pDialogState->onUpdateMidCallUACState(pResponse, pTransaction, sessionId);
      return;
    }

    std::string sessionId;
    OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
    _pDialogState->onUpdateInitialUACState(pResponse, pTransaction, sessionId);
  }
  else if (pResponse->isResponseTo("REGISTER"))
  {
    if (!pResponse->is1xx() && pResponse->hdrGetSize("contact") == 0)
    {
      std::string ct;
      ct = pTransaction->clientRequest()->hdrGet("contact");
      if (ct != "*")
      {
        if (pResponse->is2xx())
        {
          OSS_LOG_WARNING("Missing contact header in REGISTER response.");
        }
        SIPContact contact;
        contact = ct;
        ContactURI curi;
        contact.getAt(curi, 0);
        SIPURI binding = curi.getURI();
        std::string regId;
        if (getRegistrationId(binding, regId))
        {
          try
          {
            if (pResponse->is2xx())
            {
              OSS_LOG_INFO("Deleting REGISTER state " << regId);
            }
            _pDialogState->removeRegistration(regId);
            //
            // Remove from the keep-alive list
            //
            OSS::mutex_write_lock writeLock(_rwKeepAliveListMutex);
            _keepAliveList.erase(pTransaction->serverTransport()->getRemoteAddress());
          }catch(...){}
        }
      }
    }
  }
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

  if (_outboundResponseScript.isInitialized())
    _outboundResponseScript.processRequest(pResponse);
  else
    onProcessRequest(TYPE_OUTBOUND_RESPONSE, pResponse);

  std::string logId = pTransaction->getLogId();
  //
  // Let the script process it first
  //
  pResponse->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  
  //
  // Give the scripting layer a chance to process the outbound response
  //
  if (_outboundResponseScript.isInitialized())
    _outboundResponseScript.processRequest(pResponse);

  if (!_pTransactionManager->getUserAgentName().empty())
     pResponse->hdrSet("Server", _pTransactionManager->getUserAgentName().c_str());

  bool isInvite = pResponse->isResponseTo("INVITE");
  std::string isReinvite;
  if (isInvite && pTransaction->getProperty("reinvite", isReinvite))
  {
    //
    // If it is a reinvite let the other handler do the job and bail-out
    //
    std::string sessionId;
    OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
    _pDialogState->onUpdateMidCallUASState(pResponse, pTransaction, sessionId);
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
      OSS_LOG_DEBUG(pTransaction->getLogId() << " Added 2xx dialog-id: " << cacheId.str() << " to retransmission cache.");
    }
    return;
  }
  else if (isInvite || pResponse->isResponseTo("SUBSCRIBE") || pResponse->isResponseTo("REFER"))
  {
    std::string sessionId;
    OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
    //
    // Update the dialog state for leg1
    //
    _pDialogState->onUpdateInitialUASState(pResponse, pTransaction, sessionId);
    //
    // If this is a 2xx, then retransmit it.
    //
    if (isInvite && pResponse->is2xx())
    {
      std::string transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier()).c_str();
      pResponse->setProperty("transport-id", transportId);

      std::string transportScheme = pTransaction->serverTransport()->getTransportScheme();
      OSS::string_to_upper(transportScheme);
      pResponse->setProperty("target-transport", transportScheme);

      std::ostringstream cacheId;
      cacheId << pResponse->getDialogId(false) << pResponse->hdrGet("cseq");
      boost::any cacheItem = pResponse;
      pResponse->setProperty("session-id", sessionId);
      _2xxRetransmitCache.add(cacheId.str(), cacheItem);
      OSS_LOG_DEBUG(pTransaction->getLogId() << " Added 2xx dialog-id: " << cacheId.str() << " to retransmission cache.");
    }
  }
  else if (pResponse->isResponseTo("BYE"))
  {
    std::string sessionId;
    OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
    std::string legIndex;
    OSS_VERIFY(pTransaction->getProperty("leg-index", legIndex));
    if (!pResponse->is1xx(100))
    {
      SIPB2BContact::SessionInfo sessionInfo;
      sessionInfo.sessionId = sessionId;
      sessionInfo.callIndex = OSS::string_to_number<unsigned>(legIndex.c_str());
      SIPB2BContact::transform(_pTransactionManager,
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
        _pDialogState->removeDialog(pResponse->hdrGet("call-id"), sessionId);
      }catch(...){}
    }
  }
  else if (pResponse->isResponseTo("REGISTER"))
  {
    //
    // Extract the REGISTER expiration
    //
    std::string expires = pResponse->hdrGet("expires");
    std::string hContactList = pResponse->hdrGet("contact");
    ContactURI curi;
    bool hasContact = SIPContact::getAt(hContactList, curi, 0);

    if (expires.empty())
    {
      if (hasContact)
      {
        expires = curi.getHeaderParam("expires");
        if (expires.empty())
          expires = "3600";
      }

    }
    else
    {
      pResponse->hdrRemove("expires");
    }
    //
    // Rewrite the contact uri
    //
    ContactURI oldCuri;
    bool hasOldContact = false;
    SIPMessage::Ptr pRequest = pTransaction->serverRequest();
    std::string to;
    to = pRequest->hdrGet("to");
    if (!hContactList.empty() && hasContact)
    {
      std::string oldContactList = pRequest->hdrGet("contact");
      hasOldContact = SIPContact::getAt(oldContactList, oldCuri, 0);
    }

    bool is2xx = pResponse->is2xx();
    bool isTrunkReg = pTransaction->hasProperty("is-trunk-reg");

    if(!isTrunkReg && is2xx && hasContact && !hasOldContact)
    {
      //
      // Check if this was a QUERY (REGISTER with no contact)
      //
      std::vector<std::string> bindings;
      if (SIPContact::msgGetContacts(pResponse.get(), bindings) > 0)
      {
        pResponse->hdrListRemove("contact");
        for(std::vector<std::string>::iterator iter = bindings.begin(); iter != bindings.end(); iter++)
        {
          ContactURI curi = *iter;
          std::string regId;
          getRegistrationId(curi, regId);
          RegData regData;
          if (_pDialogState->findOneRegistration(regId, regData))
          {
            OSS_LOG_INFO("Listing contacts for a REGISTER query - " << regData.contact);
            pResponse->hdrListAppend("contact", regData.contact.c_str());
          }
        }
      }
    }
    else if (!isTrunkReg && is2xx && hasContact && hasOldContact)
    {
      //
      // Save the state file
      //
      OSS_LOG_INFO("Received updated contact for REGISTER from upper-reg");
      std::vector<std::string> bindings;
      if (SIPContact::msgGetContacts(pResponse.get(), bindings) == 0)
      {
        OSS_LOG_ERROR("Unable to parse contact from response.");
        return;
      }
      std::string binding = bindings[0];
      if (binding.find(SIPB2BContact::_regPrefix) == std::string::npos)
      {
        OSS_LOG_ERROR("Missing registration ID in binding " << binding);
        return;
      }

      ContactURI curi = bindings[0];
      std::string regId;
      if (!getRegistrationId(curi, regId))
      {
        OSS_LOG_ERROR("Missing registration ID in binding " << binding);
        return;
      }

      pResponse->hdrListRemove("contact");
      oldCuri.setHeaderParam("expires", expires.c_str());
      pResponse->hdrListAppend("Contact", oldCuri.data().c_str());

      try
      {
        OSS::IPAddress packetSource = pTransaction->serverTransport()->getRemoteAddress();
        OSS::IPAddress localInterface = pTransaction->serverTransport()->getLocalAddress();

        RegData registration;
        //
        // Preserve the ID
        //
        registration.key = regId;
        //
        // Preserve the call-id
        //
        registration.callId = pResponse->hdrGet("call-id");
        //
        // Preserve the contact
        //
        registration.contact = oldCuri.getURI().c_str();
        //
        // Preserve the packet source address
        //
        registration.packetSource =  packetSource.toIpPortString().c_str();
        //
        // Preserve the local interface
        //
        registration.localInterface = localInterface.toIpPortString().c_str();
        //
        // Preserve the transport ID for connection reuse
        //
        registration.transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier()).c_str();
        //
        // Preserver the trasnport scheme for connection reuse
        //
        registration.targetTransport = pTransaction->serverTransport()->getTransportScheme().c_str();
        //
        // Preserve the to header
        //
        std::string toURI;
        SIPTo::getURI(to, toURI);
        registration.aor = toURI.c_str();
        //
        // Set XOR Property if XOR is enabled
        //
        if (pTransaction->serverTransaction()->isXOREncrypted())
          registration.enc = true;

        //
        // Set expires
        //
        if (!expires.empty())
          registration.expires = OSS::string_to_number<int>(expires.c_str());
        else
          registration.expires = 3600;

        OSS::mutex_write_lock writeLock(_rwKeepAliveListMutex);
        _keepAliveList[packetSource] = localInterface;

        _pDialogState->addRegistration(registration);
      }
      catch(...)
      {
        /// We wont be able to recover on this error.
        /// transactions towards this registration will be unroutable
        std::ostringstream logMsg;
        logMsg << "Warning: " << "Unable to save persistent information for registration " << to;
        OSS::log_warning(logMsg.str());
      }
    }
    else if (isTrunkReg && is2xx && hasContact && hasOldContact)
    {
      //
      // Simply remove the contact returned by the trunk and insert the old one
      //
       pResponse->hdrListRemove("contact");
       oldCuri.setHeaderParam("expires", expires.c_str());
       pResponse->hdrListAppend("Contact", oldCuri.data().c_str());
    }
  }
  else if (pResponse->isResponseTo("INVITE") && pResponse->isErrorResponse())
  {
    std::string sessionId;
    pTransaction->getProperty("session-id", sessionId);
    //
    // Remove the rtp proxies if they were created.
    //
    if (pTransaction->clientTransaction() && pTransaction->clientTransaction()->allBranchesCompleted())
    {
      try
      {
        rtpProxy().removeSession(sessionId);
      }catch(...){}
    }
  }
  else // Any response that isn't covered by the if else block
  {
    std::string sessionId;
    std::string legIndex;

    pTransaction->getProperty("session-id", sessionId);
    pTransaction->getProperty("leg-index", legIndex);

    if (!pResponse->is1xx(100))
    {
      SIPB2BContact::SessionInfo sessionInfo;
      sessionInfo.sessionId = sessionId;

      if (!legIndex.empty())
        sessionInfo.callIndex = OSS::string_to_number<unsigned>(legIndex.c_str());

      SIPB2BContact::transform(_pTransactionManager,
        pResponse,
        pTransaction,
        pTransaction->serverTransport()->getLocalAddress(),
        sessionInfo);
    }
  }



}

void SIPB2BScriptableHandler::onProcessAckFor2xxRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport)
{
  std::string logId = pMsg->createContextId(true);
  OSS_LOG_DEBUG(logId << "Processing ACK for 2xx request " << pMsg->startLine());
  {
    std::string isXOREncrypted = "0";
    pMsg->getProperty("xor", isXOREncrypted);

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
        OSS_VERIFY(p2xx->getProperty("response-target", target));
        OSS_VERIFY(p2xx->getProperty("response-interface", localInterface));

        std::string isXOREncrypted = "0";
        p2xx->getProperty("xor", isXOREncrypted);


        std::ostringstream logMsg;
        logMsg << logId << ">>> " << p2xx->startLine()
        << " LEN: " << p2xx->data().size()
        << " SRC: " << localInterface
        << " DST: " << target
        << " ENC: " << isXOREncrypted;
        OSS::log_information(logMsg.str());
        if (OSS::log_get_level() >= OSS::PRIO_DEBUG)
          OSS::log_debug(p2xx->createLoggerData());

        if (!_pTransactionManager->getUserAgentName().empty())
          p2xx->hdrSet("Server", _pTransactionManager->getUserAgentName().c_str());

        _pTransactionManager->stack().sendRequestDirect(p2xx,
          IPAddress::fromV4IPPort(localInterface.c_str()),
          IPAddress::fromV4IPPort(target.c_str()));
      }
    }
  }
  else if (pMsg->isRequest("ACK"))
  {
    OSS_LOG_DEBUG(logId << "Processing ACK request " << pMsg->startLine());
    OSS::IPAddress localAddress;
    OSS::IPAddress targetAddress;
    std::string sessionId;
    std::string peerXOR = "0";
    try
    {
      _pDialogState->onRouteAckRequest(pMsg, pTransport, _2xxRetransmitCache,
        sessionId, peerXOR, localAddress, targetAddress);
      _pTransactionManager->stack().sendRequestDirect(pMsg, localAddress, targetAddress);
    }
    catch(OSS::Exception e)
    {
      std::ostringstream logMsg;
      logMsg << logId << "Exception: " << e.message();
      OSS::log_warning(logMsg.str());
    }
  }
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

    if (pTransaction->serverRequest()->isRequest("BYE"))
    {
      std::string sessionId;
      OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
      std::string callId = pTransaction->serverRequest()->hdrGet("call-id");
      OSS_LOG_DEBUG(pTransaction->getLogId() << "BYE Transaction Exception: " << e->message() );
      OSS_LOG_DEBUG(pTransaction->getLogId()
        << "Destroying dialog " << sessionId << " with Call-ID " << callId);
      _pDialogState->removeDialog(callId, sessionId);
    }
  }

  if (pTransaction->serverRequest()->isRequest("INVITE"))
  {
    std::string id;
    if (pTransaction->serverRequest()->getTransactionId(id))
    {
      std::string sessionId;
      OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
      OSS::mutex_write_lock _rwlock(_rwInvitePoolMutex);
      _invitePool.erase(id);
      _pDialogState->removeDialog(pTransaction->serverRequest()->hdrGet("call-id"), sessionId);
    }

    std::string sessionId;
    OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
    //
    // Remove the rtp proxies if they were created
    //
    if (pTransaction->clientTransaction() && pTransaction->clientTransaction()->allBranchesCompleted())
    {
      try
      {
        rtpProxy().removeSession(sessionId);
      }catch(...){}
    }
  }
}

bool SIPB2BScriptableHandler::loadScript(OSS::JS::JSSIPMessage& script, const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals, const std::string& helpers)
{
  script.setGlobalScriptsDirectory(globals);
  script.setHelperScriptsDirectory(helpers);
  
  bool ok = true;
  if (script.isInitialized())
    ok = script.recompile();
  else
    ok = script.initialize(scriptFile, "handle_request", extensionGlobals);
  return ok;
}
    /// Generic script loader

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


void SIPB2BScriptableHandler::handleOptionsResponse(
    const OSS::SIP::SIPTransaction::Error& e,
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  //
  // Only report errors to the queue
  //
  if ((pMsg && pMsg->is4xx(480)) || e)
  {
    SIPMessage::Ptr pRequest = pTransaction->getInitialRequest();
    SIPFrom from = pRequest->hdrGet("from");
    std::string user = from.getUser();
    if (user != "exit")
      _optionsResponseQueue.enqueue(user);
  }
}

void SIPB2BScriptableHandler::runOptionsThread()
{

  int currentIteration = 0;
  unsigned int segmentSize = 0;
  unsigned int nextSegment = 0;


  while(!_optionsThreadExit.tryWait(5000))
  {
    
    //
    // Send CRLF keep alive for every iteration
    //
    {
      OSS::mutex_read_lock readLock(_rwKeepAliveListMutex);
      for (KeepAliveList::iterator iter = _keepAliveList.begin(); iter != _keepAliveList.end(); iter++)
        _pTransactionManager->stack().transport().sendUDPKeepAlive(iter->second, iter->first);
    }
    currentIteration++;

    RegList regList;
    _pDialogState->getAllRegistrationRecords(regList);
    if (currentIteration == 1)
    {
      segmentSize = regList.size() / 12;
      if (segmentSize == 0)
        segmentSize = 12;
      nextSegment = 0;
      size_t maxIter = regList.size();
      for (size_t i = nextSegment; i < nextSegment + segmentSize && i < maxIter; i++)
        sendOptionsKeepAlive(regList[i]);
      nextSegment += segmentSize;
    }
    else
    {
      size_t maxIter = regList.size();
      for (size_t i = nextSegment; i < nextSegment + segmentSize && i < maxIter; i++)
        sendOptionsKeepAlive(regList[i]);
      nextSegment += segmentSize;
      if (currentIteration == 12)
        currentIteration = 0;
    }
  }
}

void SIPB2BScriptableHandler::runOptionsResponseThread()
{
  while(!_optionsResponseThreadExit.tryWait(0))
  {
    std::string response;
    _optionsResponseQueue.dequeue(response);
    if (response == "exit")
      break;
    else
    {
      try
      {
        RegData regData;
        if (!_pDialogState->findOneRegistration(response, regData))
          continue;

        std::ostringstream logMsg;
        logMsg << "Registration Expires: " << response;
        OSS::log_information(logMsg.str());
        //
        // Remove from the keep-alive list
        //
        OSS::mutex_write_lock writeLock(_rwKeepAliveListMutex);
        _keepAliveList.erase(OSS::IPAddress::fromV4IPPort(regData.packetSource.c_str()));
        _pDialogState->removeRegistration(regData.key);
      }
      catch(OSS::Exception e)
      {
        #if 0
        std::ostringstream logMsg;
        logMsg << "SIPB2BScriptableHandler::runOptionsResponseThread Failure - "
          << e.message();
        OSS::log_warning(logMsg.str());
        #endif
      }
    }
  }
}

void SIPB2BScriptableHandler::sendOptionsKeepAlive(RegData& regData)
{

  try
  {
    static int cseqNo = 1;

    std::string callId = OSS::string_create_uuid();
    size_t hash = OSS::string_hash(callId.c_str());

    SIPTo to(regData.aor);
    std::string transportScheme = regData.targetTransport;
    OSS::string_to_upper(transportScheme);

    IPAddress src = IPAddress::fromV4IPPort(regData.localInterface.c_str());
    IPAddress target = IPAddress::fromV4IPPort(regData.packetSource.c_str());

    _pTransactionManager->getInternalAddress(src, src);

    std::ostringstream options;
    options << "OPTIONS " << regData.contact << " SIP/2.0" << OSS::SIP::CRLF;
    options << "To: " << to.data() << OSS::SIP::CRLF;
    options << "From: sip:" << regData.key << "@" << to.getHostPort() << ";tag=" << hash << OSS::SIP::CRLF;

    if (src.externalAddress().empty())
      options << "Via: " << "SIP/2.0/" << transportScheme << " " << regData.localInterface << ";branch=z9hG4bK" << hash << ";rport" << OSS::SIP::CRLF;
    else
      options << "Via: " << "SIP/2.0/" << transportScheme << " " << src.externalAddress() << ":" << src.getPort() << ";branch=z9hG4bK" << hash << ";rport"  << OSS::SIP::CRLF;

    options << "Call-ID: " << callId << OSS::SIP::CRLF;
    options << "CSeq: " << cseqNo++ << " OPTIONS" << OSS::SIP::CRLF;

    if (src.externalAddress().empty())
      options << "Contact: " << "<sip:" << regData.key << "@" << regData.localInterface << ">" << OSS::SIP::CRLF;
    else
      options << "Contact: " << "<sip:" << regData.key << "@" << src.externalAddress() << ":" << src.getPort() << ">" << OSS::SIP::CRLF;

    options << "Content-Length: 0" << OSS::SIP::CRLF << OSS::SIP::CRLF;


    SIPMessage::Ptr msg(new SIPMessage(options.str()));
    if (regData.enc)
      msg->setProperty("xor", "1");

    msg->setProperty("target-transport", transportScheme.c_str());

    if (!regData.transportId.empty())
      msg->setProperty("transport-id", regData.transportId.c_str());

    _pTransactionManager->stack().sendRequest(msg, src, target, _keepAliveResponseCb);
  }
  catch(std::exception& e)
  {
    OSS_LOG_ERROR("SIPB2BScriptableHandler::sendOptionsKeepAlive ERROR: " << e.what());
  }
  catch(...)
  {
    OSS_LOG_ERROR("SIPB2BScriptableHandler::sendOptionsKeepAlive ERROR: Unknown exception.");
  }
}

bool SIPB2BScriptableHandler::onProcessRequest(MessageType type, const OSS::SIP::SIPMessage::Ptr& pRequest)
{
  //
  // We return true by default.  This means we will allow relay to happen.  It's up to the applicaiton
  // to override this method and process the request.
  //
  pRequest->setProperty("route-action", "accept");
  return true;
}

} } } // OSS::SIP::B2BUA


