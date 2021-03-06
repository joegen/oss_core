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

#ifndef OSS_RESIPCLIENTREGISTRATIONHANDLER_H_INCLUDED
#define OSS_RESIPCLIENTREGISTRATIONHANDLER_H_INCLUDED


#include <v8.h>
#include <vector>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSEventArgument.h"

#include <resip/dum/RegistrationHandler.hxx>
#include <rutil/SharedPtr.hxx>

class ResipClientRegistrationHandler : public OSS::JS::JSObjectWrap
{
public:
  struct update_data
  {
    std::string key;
  };

  JS_CONSTRUCTOR_DECLARE();

  /// Called when registraion succeeds or each time it is sucessfully
  /// refreshed (manual refreshes only). 
  /// virtual void onSuccess(ClientRegistrationHandle, const SipMessage& response)=0;
  JS_METHOD_DECLARE(handleOnSuccess);

  ///  Called when all of my bindings have been removed
  /// virtual void onRemoved(ClientRegistrationHandle, const SipMessage& response) = 0;
  JS_METHOD_DECLARE(handleOnRemoved);

  /// call on Retry-After failure. 
  /// return values: -1 = fail, 0 = retry immediately, N = retry in N seconds
  /// virtual int onRequestRetry(ClientRegistrationHandle, int retrySeconds, const SipMessage& response)=0;
  JS_METHOD_DECLARE(handleOnRequestRetry);

  /// Called if registration fails, usage will be destroyed (unless a 
  /// Registration retry interval is enabled in the Profile)
  /// virtual void onFailure(ClientRegistrationHandle, const SipMessage& response)=0;
  JS_METHOD_DECLARE(handleOnFailure);

  /// Called before attempting to refresh a registration
  /// Return true if the refresh should go ahead or false otherwise
  /// Default implementation always returns true
  /// virtual bool onRefreshRequired(ClientRegistrationHandle, const SipMessage& lastRequest);
  JS_METHOD_DECLARE(handleOnRefreshRequired);
  
  resip::ClientRegistrationHandler* handler();

  void onSuccess(const std::string& key);
  void onRemoved(const std::string& key);
  void onRequestRetry(const std::string& key);
  void onFailure(const std::string& key);
  void onRefreshRequired(const std::string& key);

protected:
  void onSuccessIsolated(void* user_data);
  void onRemovedIsolated(void* user_data);
  void onRequestRetryIsolated(void* user_data);
  void onFailureIsolated(void* user_data);
  void onRefreshRequiredIsolated(void* user_data);

private:
  ResipClientRegistrationHandler();
  virtual ~ResipClientRegistrationHandler();
  resip::ClientRegistrationHandler* _handler;

  JSPersistentFunctionHandle* _handleOnSuccess;
  JSPersistentFunctionHandle* _handleOnRemoved;
  JSPersistentFunctionHandle* _handleOnRequestRetry;
  JSPersistentFunctionHandle* _handleOnFailure;
  JSPersistentFunctionHandle* _handleOnRefreshRequired;

  OSS::JS::JSIsolate::Ptr _isolate;
};

//
// Inlines
//

inline resip::ClientRegistrationHandler* ResipClientRegistrationHandler::handler()
{
  return _handler;
}

#endif // OSS_RESIPCLIENTREGISTRATIONHANDLER_H_INCLUDED