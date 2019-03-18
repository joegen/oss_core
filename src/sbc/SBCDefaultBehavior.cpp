
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


#include "OSS/Net/DNS.h"
#include "OSS/SIP/SBC/SBCDefaultBehavior.h"
#include "OSS/SIP/SBC/SBCRegisterBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/UTL/Logger.h"
#include "OSS/ABNF/ABNFSIPIPV4Address.h"
#include "OSS/ABNF/ABNFSIPIPV6Address.h"

namespace OSS {
namespace SIP {
namespace SBC {

using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;

#define DEFAULT_MAX_FORWARDS "30"

SBCDefaultBehavior::SBCDefaultBehavior(
  SBCManager* pManager, 
  OSS::SIP::B2BUA::SIPB2BHandler::MessageType type,
  const std::string& contextName) :
  OSS::SIP::B2BUA::SIPB2BHandler(type),
  _pManager(pManager),
  _pauseKeepAlive(true),
  _packetsPerSecond(0),
  _lastPacketRateMarker(0),
  _maxPacketsPerSecond(0)
{
}

SBCDefaultBehavior::~SBCDefaultBehavior()
{
}

void SBCDefaultBehavior::initialize()
{

}

void SBCDefaultBehavior::deinitialize()
{
}

SIPMessage::Ptr SBCDefaultBehavior::onTransactionCreated(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
  /// Called by runtask signalling the creation of the transaction.
  /// This precedes any other transaction callbacks and therefore is the best place
  /// to initialize anything that would be needed by the transaction processing
{
  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  
  pRequest->setProperty(PropertyMap::PROP_SourceTransport, pTransaction->serverTransport()->getTransportScheme());
  pRequest->setProperty(PropertyMap::PROP_SourceAddress, pTransaction->serverTransport()->getRemoteAddress().toString());
  pRequest->setProperty(PropertyMap::PROP_SourcePort, OSS::string_from_number<unsigned short>(pTransaction->serverTransport()->getRemoteAddress().getPort()));
  
  if (_maxPacketsPerSecond && logPacketRate(pRequest, pTransaction) > _maxPacketsPerSecond)
  {
    OSS::SIP::SIPMessage::Ptr response = pRequest->createResponse(500, "Maximum Packets Per Second Violation");
    return response;
  }
  
  _pManager->modules().notifyTransactionEvent("onTransactionCreated", pTransaction);
  
  OSS::JSON::Object result;
  _pManager->modules().processRequestEvent("inboundRequest", pRequest, result);

  if (pRequest->isMidDialog() && !pRequest->isRequest("SUBSCRIBE"))
    return _pManager->dialogStateManager().onMidDialogTransactionCreated(pRequest, pTransaction);
  
  return OSS::SIP::SIPMessage::Ptr();
}

void SBCDefaultBehavior::onDestroyTransaction(SIPB2BTransaction::Ptr pTransaction)
    /// Signals that trhe transaction is about to be destroyed.
    /// This function will not invalidate the shared pointers
    /// to the transaction.  It is a mere indication that the transaction
    /// thread would now destroy its internal reference to the transaction.
{
  _pManager->modules().notifyTransactionEvent("onTransactionDestroyed", pTransaction);
}

SIPMessage::Ptr SBCDefaultBehavior::onAuthenticateTransaction(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
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
  if ((pRequest->isMidDialog() && !pRequest->isRequest(OSS::SIP::REQ_REGISTER)) || pRequest->isRequest(OSS::SIP::REQ_CANCEL))
  {
    return OSS::SIP::SIPMessage::Ptr();
  }
  
  //
  // The script allowed us to proceed. check with the manager if it concurs
  //
  return _pManager->authenticator().onAuthenticateTransaction(pRequest, pTransaction);
}

SIPMessage::Ptr SBCDefaultBehavior::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  if (pRequest->isMidDialog() && !pRequest->isRequest("SUBSCRIBE"))
  {
    pRequest->setProperty("mid-dialog", "1");
    SIPMessage::Ptr ret = _pManager->dialogStateManager().onRouteMidDialogTransaction(pRequest, pTransaction, localInterface, target);
    if (!ret)
      _pManager->onPostRouteMidDialogTransaction(pRequest, pTransaction);
    return ret;
  }
  else if (SBCContact::isRegisterRoute(pRequest))
  {      
    SIPMessage::Ptr ret = onRouteUpperReg(pRequest, pTransaction, localInterface, target);
    
    //
    // Check if this is a locally initiated registration
    //
    if (!ret && pTransaction->hasProperty(PropertyMap::propertyString(PropertyMap::PROP_LocalReg)))
    {
      return onRouteOutOfDialogTransaction(pRequest, pTransaction, localInterface, target);
    }
    else
    {
      return ret;
    }
  }
  else
  {
    //
    // Everything else, consult the java script routing api
    //
    return onRouteOutOfDialogTransaction(pRequest, pTransaction, localInterface, target);
  }
}

SIPMessage::Ptr SBCDefaultBehavior::onRouteOutOfDialogTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  //
  // Check if this request is banned
  //
  SIPMessage::Ptr bannedResponse = _pManager->autoBanRules().processBanRules(pRequest, pTransaction);
  if (bannedResponse)
  {
    return bannedResponse;
  }
  
  std::string logId = pRequest->createContextId(true);
  //
  // Lets see first if this is intended for a locally registered user
  //
  if (!target.isValid())
  {
    SIPMessage::Ptr pRegResponse = _pManager->registrar().onRouteTransaction(pRequest, pTransaction, localInterface, target);
    if (pRegResponse)
    {
      return pRegResponse;
    }
  }
  //
  // If there is no target set by the registrar, then we call the route script
  //
  if (!target.isValid() && !_pManager->onRetargetTransaction(pRequest, pTransaction))
  {
    //
    // Check the local registrar if retarget pointed to a locally registered UA
    //
    std::string retargeted;
    if (pRequest->getProperty("retargeted", retargeted))
    {
      SIPMessage::Ptr pRegResponse = _pManager->registrar().onRouteTransaction(pRequest, pTransaction, localInterface, target);
      if (pRegResponse)
      {
        return pRegResponse;
      }
    }
    
    std::string maxForwards = pRequest->hdrGet("max-forwards");
    if (maxForwards.empty())
    {
      maxForwards = DEFAULT_MAX_FORWARDS;
    }
    int maxF = OSS::string_to_number<int>(maxForwards.c_str());
    if (--maxF == 0)
    {
      return pRequest->createResponse(SIPMessage::CODE_483_TooManyHops);
    }
    pRequest->hdrRemove("max-forwards");
    pRequest->hdrSet("Max-Forwards", OSS::string_from_number(maxF).c_str());
    
    //
    // finally let the script route it
    //
    OSS::JSON::Object result;
    if (!_pManager->modules().processRequestEvent("routeRequest", pRequest, result))
    {
      return pRequest->createResponse(SIPMessage::CODE_500_InternalServerError);
    }
 
    //
    // Check if script would like to forward the call towards a registered user
    //
    std::string localRegRetarget;
    if (pRequest->getProperty("local-reg-retarget", localRegRetarget) && localRegRetarget == "1")
    {
      SIPMessage::Ptr pRegResponse = _pManager->registrar().onRouteTransaction(pRequest, pTransaction, localInterface, target);
      if (pRegResponse)
      {
        return pRegResponse;
      }
      
      if (!target.isValid())
      {
        return pRequest->createResponse(SIPMessage::CODE_404_NotFound);
      }
    }
    
    OSS_LOG_DEBUG(logId << "SBCDefaultBehavior::onRouteOutOfDialogTransaction - Flexi-Route handled.");
  }

  //
  // Check if the script wanted us to resolve the new request-uri through the registrar once more
  //
  std::string targetIdentity;
  if (pRequest->getProperty(OSS::PropertyMap::PROP_TargetIdentity, targetIdentity) && !targetIdentity.empty())
  {
    SIPMessage::Ptr pRegResponse = _pManager->registrar().onRouteTransaction(pRequest, pTransaction, localInterface, target);
    if (pRegResponse)
    {
      return pRegResponse;
    }
  } 
  
  //
  // Allow the SBC to have a glimpse of what the script did
  //
  _pManager->onPostRouteTransaction(pRequest, pTransaction);

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
    else if (routeAction == "banned")
    { 
      std::string scode;
      SIPMessage::StatusCodes statusCode = SIPMessage::CODE_403_Forbidden;
      if (pRequest->getProperty("reject-code", scode) && !scode.empty())
      {
        statusCode = (SIPMessage::StatusCodes)OSS::string_to_number<int>(scode.c_str());
      }

      std::string reason = "Congratulations.  Your address is now banned.";
      SIPMessage::Ptr serverError;
      pRequest->getProperty("reject-reason", reason);
      serverError = pRequest->createResponse(statusCode, reason.c_str());
      SIPTransportSession::rateLimit().banAddress(pTransaction->serverTransport()->getRemoteAddress().address());
      
      return serverError;
    }
  }

  //
  // Check if the script provided an outbound target
  //
  

  static OSS::ABNF::ABNFEvaluate<OSS::ABNF::ABNFSIPIPV4Address> isIPV4;
  static OSS::ABNF::ABNFEvaluate<OSS::ABNF::ABNFSIPIPV6Address> isIPV6;

  std::string host;
  if (pRequest->getProperty("target-address", host) && !host.empty())
  {
    //
    // Automatically whitelist the target if the target was explicitly set by the script
    //
    
    OSS::Net::IPAddress trustedPeer(host);
    
    if (trustedPeer.isValid())
    {
      OSS_LOG_INFO(logId << "SBCDefaultBehavior::onRouteTransaction - Marking " << host << " as trusted node.");
      SIPTransportSession::rateLimit().whiteListAddress(trustedPeer.address());
    }
  }
  
  bool isWhiteListed = pTransaction->serverTransport() && 
        SIPTransportSession::rateLimit().isWhiteListed(pTransaction->serverTransport()->getRemoteAddress().address());
    
  if (isWhiteListed)
  {
    OSS_LOG_INFO(logId << "SBCDefaultBehavior::onRouteOutOfDialogTransaction - Processing request from TRUSTED peer [" << pRequest->startLine() << "]");
  }
  else
  {
    OSS_LOG_WARNING(logId << "SBCDefaultBehavior::onRouteOutOfDialogTransaction - Processing request from UNTRUSTED peer [" << pRequest->startLine() << "]");
  }
  
  
  bool isIPAddress = !host.empty() && (isIPV4(host.c_str()) || isIPV6(host.c_str()));
  
  if (isIPAddress)
  {
    OSS_LOG_DEBUG(logId << "SBCDefaultBehavior::onRouteOutOfDialogTransaction - Remote target is finalized -> " << host);
      
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
    OSS_LOG_DEBUG(logId << "SBCDefaultBehavior::onRouteOutOfDialogTransaction - Remote target is still resolvable.  Performaing DNS lookup");
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
  // Check if the target-address will loop back to us.  We do not spiral requests.
  // This is normally indicative of a malicious INVITE scanning us or simply
  // a case of bad route script.  There are also cases where an iPBX (like Aastra)
  // do not send sbc-reg-id in the request-uri resulting to the IP pointing back
  // to Karoo.   We hack this by routing by aor
  //
  if (pRequest->isRequest("INVITE") || pRequest->isRequest("REGISTER") || pRequest->isRequest("SUBSCRIBE") || pRequest->isRequest("PUBLISH"))
  {
    if (_pManager->isLocalTransport("udp", target) ||
        _pManager->isLocalTransport("tcp", target) ||
        _pManager->isLocalTransport("tls", target))
    {       
      bool routedByAor = false;
      if (isWhiteListed && pRequest->isRequest("INVITE"))
      {
        //
        // HACK! Best effort AOR routing.  
        //
        routedByAor = _pManager->onRouteByAOR(pRequest.get(), pTransaction.get(), true, localInterface, target);
      }
        
      if (!routedByAor && pTransaction->serverTransport() &&
          !_pManager->isLocalInterface("udp", pTransaction->serverTransport()->getRemoteAddress()) &&
          !_pManager->isLocalInterface("tcp", pTransaction->serverTransport()->getRemoteAddress()) &&
          !_pManager->isLocalInterface("tls", pTransaction->serverTransport()->getRemoteAddress()))
      {
        
        if (!isWhiteListed)
        {
          if (!pTransaction->hasProperty(OSS::PropertyMap::PROP_IsLocallyAuthenticated) && pRequest->isRequest("INVITE"))
          {
            SIPTransportSession::rateLimit().banAddress(pTransaction->serverTransport()->getRemoteAddress().address());
            return pRequest->createResponse(404, "Congratulations.  Your address is now banned.");
          }
          else
          {
            return pRequest->createResponse(404, "No Upstream Route Configured");
          }
        }
        else
        {
          return pRequest->createResponse(404, "No Upstream Route Configured");
        }
      }
    }
  }
  //
  // Check if the script provided a local-interface
  //
  std::string transport;
  pRequest->getProperty(OSS::PropertyMap::PROP_TargetTransport, transport);
  if (transport.empty())
  {
    transport = "udp";
  }
  OSS::string_to_lower(transport);
  
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
    const SIPListener* pListener = _pManager->transactionManager().stack().transport().getTransportForDestination(transport, host);
    if (pListener)
    {
      //
      // Check if a static subnet route is configured for a specific listener
      //
      localInterface = pListener->getAddress();
      localInterface.setPort(OSS::string_to_number<unsigned short>(pListener->getPort().c_str()));
      localInterface.externalAddress() = pListener->getExternalAddress();
    }
    else
    {
      //
      // local-interface is not set by the script.  use the default interface
      //
      localInterface = _pManager->transactionManager().stack().transport().defaultListenerAddress();
    }
  }
  

  SIPURI ruri;
  if (!pRequest->getRequestUri(ruri))
  {
    return pRequest->createResponse(400, "Bad Request - Unable to Parse R-URI");
  }
  //
  // Check if the request-uri has a transport parameter.  
  // Adjust it accordingly;
  //
  std::string uriTransport;
  if (ruri.getParam("transport", uriTransport) && !uriTransport.empty())
  {
    ruri.setParam("transport", transport.c_str());
    pRequest->setRequestUri(ruri);
  }
  
  return OSS::SIP::SIPMessage::Ptr();
}

SIPMessage::Ptr SBCDefaultBehavior::onRouteUpperReg(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  if (_pManager->registerHandler())
    return _pManager->registerHandler()->onRouteUpperReg(pRequest, pTransaction, localInterface, target);

  return OSS::SIP::SIPMessage::Ptr();
}

SIPMessage::Ptr SBCDefaultBehavior::onRouteLocalReg(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    std::vector<OSS::Net::IPAddress>& localInterfaces,
    std::vector<OSS::Net::IPAddress>& targets)
{
    return SIPMessage::Ptr();
}

bool SBCDefaultBehavior::onRouteResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& target)
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

  OSS::Net::IPAddress viaHost(sentBy);
  if (!viaHost.isValid())
    viaHost = OSS::Net::IPAddress::fromV4IPPort(sentBy.c_str());
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
      target.setPort(5060);
    }
  }
  return true;
}

SIPMessage::Ptr SBCDefaultBehavior::onGenerateLocalResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    SIPB2BTransaction::Ptr pTransaction)
{
  SIPMessage::Ptr ok = pRequest->createResponse(SIPMessage::CODE_200_Ok);
  return ok;
}

SIPMessage::Ptr SBCDefaultBehavior::onProcessRequestBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
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

void SBCDefaultBehavior::onProcessResponseBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
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

void SBCDefaultBehavior::onProcessOutbound(
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
   pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
   if (!_pManager->getUserAgentName().empty())
     pRequest->hdrSet("User-Agent", _pManager->getUserAgentName().c_str());
}

void SBCDefaultBehavior::onProcessResponseInbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{

}

void SBCDefaultBehavior::onProcessResponseOutbound(
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
  pResponse->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  if (!_pManager->getUserAgentName().empty())
     pResponse->hdrSet("Server", _pManager->getUserAgentName().c_str());
}

void SBCDefaultBehavior::onTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  SIPB2BTransaction::Ptr pTransaction)
  /// Signals that an error occured on the transaction
  ///
  /// The transaction will be destroyed automatically after this function call
{
  if (e && !pErrorResponse)
  {
    SIPMessage::Ptr serverError = pTransaction->serverRequest()->createResponse(SIPMessage::CODE_408_RequestTimeout);
    OSS::Net::IPAddress responseTarget;
    pTransaction->onRouteResponse(pTransaction->serverRequest(),
        pTransaction->serverTransport(), pTransaction->serverTransaction(), responseTarget);
    pTransaction->serverTransaction()->sendResponse(serverError, responseTarget);
  }
}

void SBCDefaultBehavior::sendUDPKeepAlive(const OSS::Net::IPAddress& localInterface,
  const OSS::Net::IPAddress& target)
{
  if (!_pauseKeepAlive)
  {
    _pManager->transactionManager().stack().transport().sendUDPKeepAlive(localInterface, target);
  }
}

SIPMessage::Ptr SBCDefaultBehavior::onInvokeLocalHandler(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    SIPB2BTransaction::Ptr pTransaction)
{
  return SIPMessage::Ptr();
}

bool SBCDefaultBehavior::onClientTransactionCreated(const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  return false;
}

bool SBCDefaultBehavior::onRouteClientTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  return false;
}

void SBCDefaultBehavior::onProcessClientResponse(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
}

void SBCDefaultBehavior::onClientTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  SIPB2BTransaction::Ptr pTransaction)
  /// Signals that an error occured on the transaction
  ///
  /// The transaction will be destroyed automatically after this function call
{
}

unsigned long SBCDefaultBehavior::logPacketRate(const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  OSS::mutex_critic_sec_lock lock(_packetRateMutex);
  
  OSS::UInt64 now = OSS::getTime();
  if (!_lastPacketRateMarker)
  {
    _lastPacketRateMarker = now;
  }
  if (now > (_lastPacketRateMarker + 1000))
  {
    //
    // Reset the counter
    //
    _lastPacketRateMarker = now;
    _packetsPerSecond = 0;
  }
  return ++_packetsPerSecond;
}

unsigned long SBCDefaultBehavior::getPacketsPerSecond() const
{
  OSS::mutex_critic_sec_lock lock(_packetRateMutex);
  return _packetsPerSecond;
}

} } } // OSS::SIP::SBC


