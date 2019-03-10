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

#ifndef SIP_SBCSUBSCRIBEBEHAV_INCLUDED
#define SIP_SBCSUBSCRIBEBEHAV_INCLUDED


#include "OSS/SIP/SBC/SBC.h"
#include "OSS/SIP/SBC/SBCDefaultBehavior.h"
#include "OSS/UTL/Cache.h"

namespace OSS {
namespace SIP {
namespace SBC {


class OSS_API SBCSubscribeBehavior : public SBCDefaultBehavior
{
public:
  SBCSubscribeBehavior(SBCManager* pManager);
    /// Creates a new SBC register behavior
    /// This is the base class of all SBC behaviors

  virtual ~SBCSubscribeBehavior();
    /// Destroys the register behavior

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

  SIPMessage::Ptr onUpdateSubscription(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
  /// This will be called when a mid-dialog subscribe is received.

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

  void onProcessResponseInbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);
  
  SIPMessage::Ptr onRouteNotify(
    SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
    /// route notify requests.  If the subscription
    /// exists, this function will will localInterface and target
    /// parameters and would return a NULL Ptr.

  SIPMessage::Ptr findSubscription(
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction);
    // Return an existing subscription request
    // that has a dialog previously established

  void expireSubscription(
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction);
    // Remove an existing subscription dialog

protected:
  void registerSubscribe(
    const SIPMessage::Ptr& pRequest,
    const SIPMessage::Ptr& pResponse,
    const SIPB2BTransaction::Ptr& pTransaction);
    /// Register a subscription request to be used for routing subsequent notifies

  SIPMessage::Ptr onInternalRouteNotify(
    SIPMessage::Ptr& pNotify,
    const SIPMessage::Ptr& pSubscribe,
    const SIPB2BTransaction::Ptr& pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
    /// route notify requests.  If the subscription
    /// exists, this function will will localInterface and target
    /// parameters and would return a NULL Ptr.

  CacheManager _subscriptions_2;
    /// Cache will expire in 2 hours

  CacheManager _subscriptions_4;
    /// Cache will expire in 4 hours

  CacheManager _subscriptions_8;
    /// Cache will expire in 8 hours

  CacheManager _subscriptions_16;
    /// Cache will expire in 16 hours

  CacheManager _subscriptions_24;
    /// Cache will expire in 2 hours
};

//
// Inlines
//


} } } // OSS::SIP::SBC

#endif // SIP_SBCSUBSCRIBEBEHAV_INCLUDED

