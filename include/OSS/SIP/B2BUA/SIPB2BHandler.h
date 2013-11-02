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

#ifndef _SIPB2BHANDLER_INCLUDED_H
#define	_SIPB2BHANDLER_INCLUDED_H


#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "OSS/SIP/B2BUA/B2BUA.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"

namespace OSS {
namespace SIP {
namespace B2BUA {

class SIPB2BHandler :
  public boost::enable_shared_from_this<SIPB2BHandler>,
  private boost::noncopyable
{
public:
  typedef boost::shared_ptr<SIPB2BHandler> Ptr;
  
  enum MessageType
  {
    TYPE_INVITE = 0,
    TYPE_REGISTER,
    TYPE_BYE,
    TYPE_CANCEL,
    TYPE_EXEC,
    TYPE_INFO,
    TYPE_MESSAGE,
    TYPE_OPTIONS,
    TYPE_PRACK,
    TYPE_PUBLISH,
    TYPE_SUBSCRIBE,
    TYPE_NOTIFY,
    TYPE_REFER,
    TYPE_UPDATE,
    TYPE_SDP,
    TYPE_ANY,
    TYPE_INVALID
  };

  SIPB2BHandler(MessageType type);
    /// Create a new B2BHandler

  virtual ~SIPB2BHandler();
    /// Destroyes the B2BHandler

  virtual SIPMessage::Ptr onTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
  { return SIPMessage::Ptr(); };
    /// Called by runtask signalling the creation of the transaction.
    /// This prcedes any other transaction callbacks and therefore is the best place
    /// to initialize anything that would be needed by the transaction processing

  virtual bool onClientTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
  { return true; };
    /// Called by runtask signalling the creation of the transaction.
    /// This prcedes any other transaction callbacks and therefore is the best place
    /// to initialize anything that would be needed by the transaction processing

  virtual SIPMessage::Ptr onAuthenticateTransaction(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
  { return SIPMessage::Ptr(); };
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
    OSS::IPAddress& localInterface,
    OSS::IPAddress& target)
  { return SIPMessage::Ptr(); };
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

  virtual bool onRouteClientTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::IPAddress& localInterface,
    OSS::IPAddress& target)
  { return false; };
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

  virtual void onProcessClientResponse(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction)
  {};
    /// Process the newly received response

  virtual SIPMessage::Ptr onProcessRequestBody(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction)
  { return SIPMessage::Ptr(); };
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
    SIPB2BTransaction::Ptr pTransaction)
  {};
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
    SIPB2BTransaction::Ptr pTransaction)
  {};
    /// This is the last chance for the application to process
    /// the outbound request before it gets sent out to the transport.
    ///
    /// This is normally the place where application would want to
    /// insert application-specific headers as well as change existing
    /// headers to the desired application-specific values for as long
    /// as it wont conflict with dialog creation states.

  virtual bool onRouteResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::IPAddress& target)
  { return false; };
   /// This is normally the place where the application can specify the
   /// target for a reponse.

  virtual SIPMessage::Ptr onGenerateLocalResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    SIPB2BTransaction::Ptr pTransaction)
  { return SIPMessage::Ptr(); };
    /// This will be called by the B2BUA if the transaction property "generate-local-response"
    /// is set to 1 by the route handler.  This is used by the applicaiton layer to allow
    /// transactions to directly respond to the request without waiting for the remote
    /// UA to send its own response

  virtual SIPMessage::Ptr onInvokeLocalHandler(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    SIPB2BTransaction::Ptr pTransaction)
  { return SIPMessage::Ptr(); };
    /// This will be called by the B2BUA if the transaction property "invoke-local-handler"
    /// is set to 1 by the route handler.  This is used by the applicaiton layer to allow
    /// transactions to process a request locally.  Example is local registration.

  virtual void onProcessResponseInbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction)
  {};
    /// Process the newly received response


  virtual void onProcessResponseOutbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction)
  {};
    /// This is the last chance for the application to process
    /// the outbound response before it gets sent out to the transport.
    ///
    /// This is normally the place where application would want to
    /// insert application-specific headers as well as change existing
    /// headers to the desired application-specific values for as long
    /// as it wont conflict with dialog creation states.

  virtual void onProcessAckFor2xxRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport){};
    /// Call back for ACK and 200 OK request for INVITE handler

  virtual void onTransactionError(
    OSS::SIP::SIPTransaction::Error e,
    SIPMessage::Ptr pErrorResponse,
    SIPB2BTransaction::Ptr pTransaction)
  {};
    /// Signals that an error occured on the transaction
    ///
    /// The transaction will be destroyed automatically after this function call

  virtual void onClientTransactionError(
    OSS::SIP::SIPTransaction::Error e,
    SIPMessage::Ptr pErrorResponse,
    SIPB2BTransaction::Ptr pTransaction)
  {};
    /// Signals that an error occured on the transaction
    ///
    /// The transaction will be destroyed automatically after this function call

  virtual void onDestroyTransaction(SIPB2BTransaction::Ptr pTransaction)
  {};
    /// Signals that trhe transaction is about to be destroyed.
    /// This function will not invalidate the shared pointers
    /// to the transaction.  It is a mere indication that the transaction
    /// thread would now destroy its internal reference to the transaction.


  virtual void initialize()
  {};
    /// Called by the B2BUA when the handler is first registered

  virtual void deinitialize()
  {};
    /// Called by the B2BUA before a handler is destroyed

  MessageType getType() const;
    /// Return the type of message this handler would process

private:
  MessageType _type;
};

//
// Inlines
//

inline SIPB2BHandler::MessageType SIPB2BHandler::getType() const
{
  return _type;
}

inline SIPB2BHandler::SIPB2BHandler(MessageType type) : _type(type)
{
}

inline SIPB2BHandler::~SIPB2BHandler()
{
}

} } } // OSS::SIP::B2BUA


#endif	// SIPB2BHANDLER_INCLUDED_H


