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

#ifndef SIP_SBCINVITEBEHAV_INCLUDED
#define SIP_SBCINVITEBEHAV_INCLUDED


#include "OSS/UTL/Cache.h"
#include "OSS/SIP/SBC/SBC.h"
#include "OSS/SIP/SBC/SBCDefaultBehavior.h"
#include "OSS/SIP/SBC/SBCCancelBehavior.h"
#include "OSS/SIP/SBC/SBCAliasMap.h"


namespace OSS {
namespace SIP {
namespace SBC {


class OSS_API SBCInviteBehavior : public SBCDefaultBehavior
{
public:
  typedef std::vector<boost::filesystem::path> DialogStateFileList;

  SBCInviteBehavior(SBCManager* pManager);
    /// Creates a new SBC register behavior
    /// This is the base class of all SBC behaviors

  virtual ~SBCInviteBehavior();
    /// Destroys the register behavior

  virtual SIPMessage::Ptr onTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);
    /// Called by runtask signalling the creation of the transaction.
    /// This precedes any other transaction callbacks and therefore is the best place
    /// to initialize anything that would be needed by the transaction processing

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

  virtual void onProcessAckOr2xxRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport);
    /// Call back for ACK and 200 OK request for INVITE handler

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

  void onSendResponse(const SIPMessage::Ptr& pReponse, const SIPTransportSession::Ptr& pTransport, const SIPTransaction::Ptr& pTransaction);
    /// Signals when a final response is sent to the caller
  
  void sendPrack(SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);
  
protected:
  SIPMessage::Ptr onRouteReinvite(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
  
  SIPMessage::Ptr onRouteInviteWithReplaces(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);

  void onProcessReinviteResponseInbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);

  void onProcessReinviteResponseOutbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);

  SBCAliasMap& aliasMap();
  
  void handleLocalPrackResponse(
    const OSS::SIP::SIPTransaction::Error& e,
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);
  
  void setCallIdCorrelation(SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr& pTransaction);

protected:
  OSS::SIP::SIPTransaction::Callback _localPrackResponseCb;
  OSS::CacheManager _2xxRetransmitCache;
  SBCCancelBehavior* _pCancelBehavior;
  SBCAliasMap _aliasMap;
  friend class  SBCManager;
};

//
// Inlines
//


inline SBCAliasMap& SBCInviteBehavior::aliasMap()
{
  return _aliasMap;
}

} } } // OSS::SIP::SBC

#endif // SIP_SBCINVITEBEHAV_INCLUDED

