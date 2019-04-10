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

#ifndef OSS_RESIPCLIENTSUBSCRIPTIONHANDLER_H_INCLUDED
#define OSS_RESIPCLIENTSUBSCRIPTIONHANDLER_H_INCLUDED


#include <v8.h>
#include <vector>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSEventArgument.h"

#include <resip/dum/SubscriptionHandler.hxx>
#include <rutil/SharedPtr.hxx>

class ResipClientSubscriptionHandler : public OSS::JS::JSObjectWrap
{
public:
  struct update_data
  {
    std::string key;
    std::string eventBody; 
    bool isOutOfOrder;
  };
  
  JS_CONSTRUCTOR_DECLARE();

  /// Client must call acceptUpdate or rejectUpdate for any onUpdateFoo
  /// virtual void onUpdatePending(ClientSubscriptionHandle, const SipMessage& notify, bool outOfOrder)=0;
  JS_METHOD_DECLARE(handleOnUpdatePending);

  /// virtual void onUpdateActive(ClientSubscriptionHandle, const SipMessage& notify, bool outOfOrder)=0;
  JS_METHOD_DECLARE(handleOnUpdateActive);

  /// unknown Subscription-State value
  /// virtual void onUpdateExtension(ClientSubscriptionHandle, const SipMessage& notify, bool outOfOrder)=0;
  JS_METHOD_DECLARE(handleOnUpdateExtension);

  /// subscription can be ended through a notify or a failure response.
  /// virtual void onTerminated(ClientSubscriptionHandle, const SipMessage* msg)=0;
  JS_METHOD_DECLARE(handleOnTerminated);

  /// not sure if this has any value - can be called for either a 200/SUBSCRIBE or a NOTIFY - whichever arrives first
  /// virtual void onNewSubscription(ClientSubscriptionHandle, const SipMessage& notify)=0;
  JS_METHOD_DECLARE(handleOnNewSubscription);

  /// virtual void onNotifyNotReceived(ClientSubscriptionHandle);
  JS_METHOD_DECLARE(handleOnNotifyNotReceived);

  /// Called when a TCP or TLS flow to the server has terminated.  This can be caused by socket
  /// errors, or missing CRLF keep alives pong responses from the server.
  /// Called only if clientOutbound is enabled on the UserProfile and the first hop server 
  /// supports RFC5626 (outbound).
  /// Default implementation is to re-form the subscription using a new flow
  /// virtual void onFlowTerminated(ClientSubscriptionHandle);
  JS_METHOD_DECLARE(handleOnFlowTerminated);

  resip::ClientSubscriptionHandler* handler();
  
  void onUpdatePending(const std::string& key, const std::string& eventBody, bool isOutOfOrder);
  void onUpdateActive(const std::string& key, const std::string& eventBody, bool isOutOfOrder);
  void onUpdateExtension(const std::string& key, const std::string& eventBody, bool isOutOfOrder);

protected:
  void onUpdatePendingIsolated(void* user_data);
  void onUpdateActiveIsolated(void* user_data);
  void onUpdateExtensionIsolated(void* user_data);
  
private:
  ResipClientSubscriptionHandler();
  virtual ~ResipClientSubscriptionHandler();
  resip::ClientSubscriptionHandler* _handler;
  JSPersistentFunctionHandle* _handleOnUpdatePending;
  JSPersistentFunctionHandle* _handleOnUpdateActive;
  JSPersistentFunctionHandle* _handleOnUpdateExtension;
  JSPersistentFunctionHandle* _handleOnTerminated;
  JSPersistentFunctionHandle* _handleOnNewSubscription;
  JSPersistentFunctionHandle* _handleOnNotifyNotReceived;
  JSPersistentFunctionHandle* _handleOnFlowTerminated;
  OSS::JS::JSIsolate::Ptr _isolate;
};

//
// Inlines
//

inline resip::ClientSubscriptionHandler* ResipClientSubscriptionHandler::handler()
{
  return _handler;
}

#endif // OSS_RESIPCLIENTSUBSCRIPTIONHANDLER_H_INCLUDED