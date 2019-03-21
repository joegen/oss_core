// OSS Software Solutions Application Programmer Interface
// 
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
#ifndef SIP_SBCDFBEHAV_INCLUDED
#define SIP_SBCDFBEHAV_INCLUDED

#include "OSS/SIP/SBC/SBC.h"
#include "OSS/SIP/SBC/SBCException.h"
#include "OSS/SIP/B2BUA/SIPB2BHandler.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/UTL/Cache.h"
#include "OSS/SIP/SBC/SBCContact.h"
#include "OSS/SIP/SBC/SBCDirectories.h"

namespace OSS {
namespace SIP {
namespace SBC {

class SBCManager;

class OSS_API SBCDefaultBehavior : public OSS::SIP::B2BUA::SIPB2BHandler
{
public:
  SBCDefaultBehavior(SBCManager* pManager, 
    OSS::SIP::B2BUA::SIPB2BHandler::MessageType type = OSS::SIP::B2BUA::SIPB2BHandler::TYPE_ANY,
    const std::string& contextName = "SBC Default Request Handler");
    /// Creates a new SBC default behavior
    /// This is the base class of all SBC behaviors

  virtual ~SBCDefaultBehavior();
    /// Destroys the default behavior

  virtual void initialize();
    /// Called by the B2BUA when the handler is first registered

  virtual void deinitialize();
    /// Called by the B2BUA before a handler is destroyed

  virtual SIPMessage::Ptr onTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);
    /// Called by runtask signalling the creation of the transaction.
    /// This precedes any other transaction callbacks and therefore is the best place
    /// to initialize anything that would be needed by the transaction processing

  virtual SIPMessage::Ptr onAuthenticateTransaction(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);
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
    SIPB2BTransaction::Ptr pTransaction,
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
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);


  SIPMessage::Ptr onRouteUpperReg(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);

  SIPMessage::Ptr onRouteLocalReg(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    std::vector<OSS::Net::IPAddress>& localInterfaces,
    std::vector<OSS::Net::IPAddress>& targets);

  virtual SIPMessage::Ptr onProcessRequestBody(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction);
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
    SIPB2BTransaction::Ptr pTransaction);
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
    SIPB2BTransaction::Ptr pTransaction);
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
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& target);
   /// This is normally the place where the application can specify the
   /// target for a reponse.

  virtual SIPMessage::Ptr onGenerateLocalResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    SIPB2BTransaction::Ptr pTransaction);
    /// This will be called by the B2BUA if the transaction property "generate-local-response"
    /// is set to 1 by the route handler.  This is used by the applicaiton layer to allow
    /// transactions to directly respond to the request without waiting for the remote
    /// UA to send its own response

  virtual void onProcessResponseInbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);
    /// Process the newly received response

  virtual void onProcessResponseOutbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);
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
    SIPB2BTransaction::Ptr pTransaction);
    /// Signals that an error occured on the transaction
    ///
    /// The transaction will be destroyed automatically after this function call

  virtual void onDestroyTransaction(SIPB2BTransaction::Ptr pTransaction);
    /// Signals that trhe transaction is about to be destroyed.
    /// This function will not invalidate the shared pointers
    /// to the transaction.  It is a mere indication that the transaction
    /// thread would now destroy its internal reference to the transaction.

  virtual SIPMessage::Ptr onInvokeLocalHandler(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    SIPB2BTransaction::Ptr pTransaction);
    /// This will be called by the B2BUA if the transaction property "invoke-local-handler"
    /// is set to 1 by the route handler.  This is used by the applicaiton layer to allow
    /// transactions to process a request locally.  Example is local registration.

  SBCManager* getManager() const;
    /// Return a pointer to the SBCManager

  void sendUDPKeepAlive(const OSS::Net::IPAddress& localInterface,
    const OSS::Net::IPAddress& target);
    /// Send CRLF/CRLF keep-alive
  
  void pauseKeepAlive(bool pause);
    /// Pause sending of keep-alive
  
  void startKeepAlive();
    /// Start sending of keep-alive

  virtual bool onClientTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);
    /// Called by runtask signalling the creation of the transaction.
    /// This precedes any other transaction callbacks and therefore is the best place
    /// to initialize anything that would be needed by the transaction processing

  virtual bool onRouteClientTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
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
    SIPB2BTransaction::Ptr pTransaction);
    /// Process the newly received response

  virtual void onClientTransactionError(
    OSS::SIP::SIPTransaction::Error e,
    SIPMessage::Ptr pErrorResponse,
    SIPB2BTransaction::Ptr pTransaction);
    /// Signals that an error occured on the transaction
    ///
    /// The transaction will be destroyed automatically after this function call
  
  unsigned long logPacketRate(const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);
  unsigned long getPacketsPerSecond() const;
  void setMaxPacketsPerSecond(unsigned long packetsPerSecond);
  unsigned long getMaxPacketsPerSecond() const;
  
protected:
  SBCManager* _pManager;
  bool _pauseKeepAlive;
  
  mutable OSS::mutex_critic_sec _packetRateMutex;
  unsigned long _packetsPerSecond;
  OSS::UInt64 _lastPacketRateMarker;
  unsigned long _maxPacketsPerSecond;
  boost::filesystem::path _stateDir;
};

//
// Inlines
//


inline SBCManager* SBCDefaultBehavior::getManager() const
{
  return _pManager;
}

inline void SBCDefaultBehavior::pauseKeepAlive(bool pause)
{
  _pauseKeepAlive = pause;
}

inline void SBCDefaultBehavior::startKeepAlive()
{
  pauseKeepAlive(false);
}

inline void SBCDefaultBehavior::setMaxPacketsPerSecond(unsigned long packetsPerSecond)
{
  _maxPacketsPerSecond = packetsPerSecond;
}

inline unsigned long SBCDefaultBehavior::getMaxPacketsPerSecond() const
{
  return _maxPacketsPerSecond;
}

} } } // OSS::SIP::SBC

#endif // SIP_SBCDFBEHAV_INCLUDED
