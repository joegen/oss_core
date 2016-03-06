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
#ifndef SIPB2BSCRIPTABLEHANDLER_H_INCLUDED
#define SIPB2BSCRIPTABLEHANDLER_H_INCLUDED

#include <map>

#include "OSS/UTL/Cache.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/JS/JSSIPMessage.h"
#include "OSS/SIP/B2BUA/SIPB2BHandler.h"
#include "OSS/SIP/B2BUA/SIPB2BContact.h"
#include "OSS/SIP/B2BUA/SIPB2BDialogData.h"
#include "OSS/RTP/RTPProxyManager.h"

namespace OSS {
namespace SIP {
namespace B2BUA {

class SIPB2BTransactionManager;
class SIPB2BDialogStateManager;

class OSS_API SIPB2BScriptableHandler : public OSS::SIP::B2BUA::SIPB2BHandler
{
public:

  enum MessageType
  {
    TYPE_INBOUND,
    TYPE_AUTH,
    TYPE_ROUTE,
    TYPE_ROUTE_FAILOVER,
    TYPE_OUTBOUND_REQUEST,
    TYPE_OUTBOUND_RESPONSE
  };

  SIPB2BScriptableHandler(
    SIPB2BTransactionManager* pManager,
    SIPB2BDialogStateManager* pDialogState,
    const std::string& contextName = "B2BUA Scriptable Handler");
    /// Creates a new B2BUA default behavior
    /// This is the base class of all B2BUA behaviors

  virtual ~SIPB2BScriptableHandler();
    /// Destroys the default behavior

  virtual void initialize();
    /// Called by the B2BUA when the handler is first registered

  virtual void deinitialize();
    /// Called by the B2BUA before a handler is destroyed

  virtual SIPMessage::Ptr onTransactionCreated(
    const SIPMessage::Ptr& pRequest, OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// Called by runtask signalling the creation of the transaction.
    /// This precedes any other transaction callbacks and therefore is the best place
    /// to initialize anything that would be needed by the transaction processing

  virtual SIPMessage::Ptr onAuthenticateTransaction(
    const SIPMessage::Ptr& pRequest, OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// Authenticate the new Transaction request,
    ///
    /// This method is called from the B2B Transaction runTask().
    /// The intention of the virtual function is to create a
    /// proxy-auth response.  If a null Ptr is returned by this
    /// callback, runTask will proceed in processing the request
    /// without authenticating it.
    ///
    ///
    /// Take note that authentication state is not maintained
    /// by the transaction.  It is the responsibility of the application
    /// to maintain the authentication state.

  virtual SIPMessage::Ptr onRouteTransaction(
    SIPMessage::Ptr& pRequest,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
    /// Route the new request.
    ///
    /// This method expects that the application will format the request-uri
    /// towards the intended target, insert the correct via and contact where responses,
    /// will be received as well as insert a route-set if
    /// upstream proxies are needed.
    ///
    /// If the return value is an error response, the transaction
    /// will send it automatically to the sender.  Eg 404, if no route exists.
    /// If the request is routable, the return value must be a null-Ptr.
    ///
    /// Both Local Interface and Target address that the request would use
    /// must be set by the application layer


  SIPMessage::Ptr onRouteOutOfDialogTransaction(
    SIPMessage::Ptr& pRequest,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);


  SIPMessage::Ptr onRouteUpperReg(
    SIPMessage::Ptr& pRequest,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);

  virtual SIPMessage::Ptr onProcessRequestBody(
    SIPMessage::Ptr& pRequest,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
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

  virtual void onProcessResponseBody(
    SIPMessage::Ptr& pRequest,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// This method allows the application to
    /// process the body of the response
    /// before it gets sent out.
    ///
    /// This method allows the upper layer to modify the body
    /// based on specific application requirements.  For
    /// example, a media proxy may modify SDP address and port
    /// so that RTP passes through the application.
    ///

  virtual void onProcessOutbound(
    SIPMessage::Ptr& pRequest,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// This is the last chance for the application to process
    /// the outbound request before it gets sent out to the transport.
    ///
    /// This is normally the place where application would want to
    /// insert application-specific headers as well as change existing
    /// headers to the desired application-specific values for as long
    /// as it wont conflict with dialog creation states.

  bool onRouteResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& target);
   /// This is normally the place where the application can specify the
   /// target for a reponse.

  virtual SIPMessage::Ptr onGenerateLocalResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// This will be called by the B2BUA if the transaction property "generate-local-response"
    /// is set to 1 by the route handler.  This is used by the applicaiton layer to allow
    /// transactions to directly respond to the request without waiting for the remote
    /// UA to send its own response

  virtual void onProcessResponseInbound(
    SIPMessage::Ptr& pResponse,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// Process the newly received response

  virtual void onProcessResponseOutbound(
    SIPMessage::Ptr& pResponse,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// This is the last chance for the application to process
    /// the outbound response before it gets sent out to the transport.
    ///
    /// This is normally the place where application would want to
    /// insert application-specific headers as well as change existing
    /// headers to the desired application-specific values for as long
    /// as it wont conflict with dialog creation states.

  virtual void onTransactionError(
    OSS::SIP::SIPTransaction::Error e,
    SIPMessage::Ptr pErrorResponse,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// Signals that an error occured on the transaction
    ///
    /// The transaction will be destroyed automatically after this function call

  virtual void onDestroyTransaction(OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// Signals that trhe transaction is about to be destroyed.
    /// This function will not invalidate the shared pointers
    /// to the transaction.  It is a mere indication that the transaction
    /// thread would now destroy its internal reference to the transaction.

  virtual SIPMessage::Ptr onInvokeLocalHandler(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// This will be called by the B2BUA if the transaction property "invoke-local-handler"
    /// is set to 1 by the route handler.  This is used by the applicaiton layer to allow
    /// transactions to process a request locally.  Example is local registration.

  bool loadInboundScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals = "", const std::string& helpers = "");
    /// Load the inbound script file for this transaction.
    ///
    /// This method will throw a JSSIPMessageException if an error occurs
    /// in loading or compiling the script

  bool loadAuthScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals = "", const std::string& helpers = "");
    /// Load the authentication script file for this transaction.
    ///
    /// This method will throw a JSSIPMessageException if an error occurs
    /// in loading or compiling the script

  bool loadRouteScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals = "", const std::string& helpers = "");
    /// Load the routing script file for this transaction.
    ///
    /// This method will throw a JSSIPMessageException if an error occurs
    /// in loading or compiling the script

  bool loadRouteFailoverScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals = "", const std::string& helpers = "");
    /// Load the routing failover script file for this transaction.
    ///
    /// This method will throw a JSSIPMessageException if an error occurs
    /// in loading or compiling the script

  bool loadOutboundScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals = "", const std::string& helpers = "");
    /// Load the routing script file for this transaction.
    ///
    /// This method will throw a JSSIPMessageException if an error occurs
    /// in loading or compiling the script

  bool loadOutboundResponseScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals = "", const std::string& helpers = "");
    /// Load the routing script file for this transaction.
    ///
    /// This method will throw a JSSIPMessageException if an error occurs
    /// in loading or compiling the script

  bool loadScript(OSS::JS::JSSIPMessage& script, const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals = "", const std::string& helpers = "");
    /// Generic script loader
  void recompileScripts();
    /// Recompile loaded scripts.  This is normally due to a SIGHUP being received by the B2BUA

  SIPB2BTransactionManager* getManager() const;
    /// Return a pointer to the SIPB2BTransactionManager


  virtual bool onClientTransactionCreated(
    const SIPMessage::Ptr& pRequest, OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// Called by runtask signalling the creation of the transaction.
    /// This precedes any other transaction callbacks and therefore is the best place
    /// to initialize anything that would be needed by the transaction processing

  virtual bool onRouteClientTransaction(
    SIPMessage::Ptr& pRequest,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
    /// Route the new request.
    ///
    /// This method expects that the application will format the request-uri
    /// towards the intended target, insert the correct via and contact where responses,
    /// will be received as well as insert a route-set if
    /// upstream proxies are needed.
    ///
    /// If the return value is an error response, the transaction
    /// will send it automatically to the sender.  Eg 404, if no route exists.
    /// If the request is routable, the return value must be a null-Ptr.
    ///
    /// Both Local Interface and Target address that the request would use
    /// must be set by the application layer

  void onProcessClientResponse(
    SIPMessage::Ptr& pResponse,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// Process the newly received response

  virtual void onClientTransactionError(
    OSS::SIP::SIPTransaction::Error e,
    SIPMessage::Ptr pErrorResponse,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    /// Signals that an error occured on the transaction
    ///
    /// The transaction will be destroyed automatically after this function call

  virtual bool onProcessRequest(
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,
    MessageType type, 
    const OSS::SIP::SIPMessage::Ptr& request);
    /// Send the SIPRequest to the application.  This means that the scripting
    /// engine is not initialized for this message type. Otherwise, the javascript engine
    /// will handle the request

  //
  // INVITE handlers
  //
  virtual void onProcessAckOr2xxRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport);
    /// Callback for ACK and 200 OK retransmission for INVITE

  //
  // REGISTER handlers
  //
  void sendOptionsKeepAlive(RegData& regData);
  void handleOptionsResponse(
    const OSS::SIP::SIPTransaction::Error& e,
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransacion);
    /// This callback handles responses to the OPTIONS keep-alive

  bool getRegistrationId(const ContactURI& curi, std::string& regId) const;
  bool getRegistrationId(const SIPURI& binding, std::string& regId) const;

  OSS::RTP::RTPProxyManager& rtpProxy();

  bool getExternalAddress(
    const OSS::Net::IPAddress& internalIp,
    std::string& externalIp) const;
    /// Return the external interface for a given internal listener
protected:
  void runOptionsThread();
    /// This method runs the OPTIONS keep-alive loop

  void runOptionsResponseThread();
    /// This method runs the OPTIONS keep-alive response loop

protected:
  OSS::JS::JSSIPMessage _inboundScript;
  OSS::JS::JSSIPMessage _authScript;
  OSS::JS::JSSIPMessage _routeScript;
  OSS::JS::JSSIPMessage _routeFailoverScript;
  OSS::JS::JSSIPMessage _outboundScript;
  OSS::JS::JSSIPMessage _outboundResponseScript;
  SIPB2BTransactionManager* _pTransactionManager;
  SIPB2BDialogStateManager* _pDialogState;

  //
  // INVITE related variables
  //
  OSS::CacheManager _2xxRetransmitCache;
  OSS::mutex_read_write _rwInvitePoolMutex;
  std::map<std::string, SIPMessage::Ptr> _invitePool;
  //
  // REGISTER related variables
  //
  boost::thread* _pOptionsThread;
  OSS::semaphore _optionsThreadExit;
  OSS::BlockingQueue<std::string> _optionsResponseQueue;
  boost::thread* _pOptionsResponseThread;
  OSS::semaphore _optionsResponseThreadExit;
  OSS::SIP::SIPTransaction::Callback _keepAliveResponseCb;
  OSS::mutex_read_write _rwKeepAliveListMutex;
  typedef std::map<OSS::Net::IPAddress, OSS::Net::IPAddress> KeepAliveList;
  KeepAliveList _keepAliveList;
  OSS::thread_pool _threadPool;
  //
  // RTP Proxy
  //
  OSS::RTP::RTPProxyManager _rtpProxy;
};

//
// Inlines
//


inline SIPB2BTransactionManager* SIPB2BScriptableHandler::getManager() const
{
  return _pTransactionManager;
}

inline bool SIPB2BScriptableHandler::loadInboundScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals, const std::string& helpers)
{
  return loadScript(_inboundScript, scriptFile, extensionGlobals, globals, helpers);
}

inline bool SIPB2BScriptableHandler::loadAuthScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals, const std::string& helpers)
{
  return loadScript(_authScript, scriptFile, extensionGlobals, globals, helpers);
}

inline bool SIPB2BScriptableHandler::loadRouteScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals, const std::string& helpers)
{
  return loadScript(_routeScript, scriptFile, extensionGlobals, globals, helpers);
}

inline bool SIPB2BScriptableHandler::loadRouteFailoverScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals, const std::string& helpers)
{
  return loadScript(_routeFailoverScript, scriptFile, extensionGlobals, globals, helpers);
}

inline bool SIPB2BScriptableHandler::loadOutboundScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals, const std::string& helpers)
{
  return loadScript(_outboundScript, scriptFile, extensionGlobals, globals, helpers);
}

inline bool SIPB2BScriptableHandler::loadOutboundResponseScript(const boost::filesystem::path& scriptFile, void(*extensionGlobals)(OSS_HANDLE), const std::string& globals, const std::string& helpers)
{
  return loadScript(_outboundResponseScript, scriptFile, extensionGlobals, globals, helpers);
}

inline OSS::RTP::RTPProxyManager& SIPB2BScriptableHandler::rtpProxy()
{
  return _rtpProxy;
}

inline bool SIPB2BScriptableHandler::getExternalAddress(
    const OSS::Net::IPAddress& internalIp,
    std::string& externalIp) const
{
  return _pTransactionManager->stack().transport().getExternalAddress(internalIp, externalIp);
}

} } } // OSS::SIP::B2BUA

#endif // SIPB2BSCRIPTABLEHANDLER_H_INCLUDED
