#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"
#include <rutil/Logger.hxx>
#include "OSS/JS/modules/ResipClientRegistrationHandler.h"
#include <resip/dum/ClientRegistration.hxx>


using OSS::JS::JSObjectWrap;
using namespace resip;

/// ClientRegistrationHandler ///
class client_registration_handler : public ClientRegistrationHandler
{
public:
  client_registration_handler(ResipClientRegistrationHandler* handler) :
    _handler(handler)
  {
  }
    
  virtual ~client_registration_handler()
  {
  }

  std::string toString(const Data& data)
  {
    return std::string(data.data(), data.size());
  }
  
  /// Called when registraion succeeds or each time it is sucessfully
  /// refreshed (manual refreshes only). 
  virtual void onSuccess(ClientRegistrationHandle h, const SipMessage& response)
  {
    Uri to = response.header(h_To).uri();
    std::string key = toString(to.getAorNoPort());
    _handler->onSuccess(key);
  }

  /// Called when all of my bindings have been removed
  virtual void onRemoved(ClientRegistrationHandle h, const SipMessage& response)
  {
    Uri to = response.header(h_To).uri();
    std::string key = toString(to.getAorNoPort());
    _handler->onRemoved(key);
  }

  /// call on Retry-After failure. 
  /// return values: -1 = fail, 0 = retry immediately, N = retry in N seconds
  virtual int onRequestRetry(ClientRegistrationHandle h, int retrySeconds, const SipMessage& response)
  {
    Uri to = response.header(h_To).uri();
    std::string key = toString(to.getAorNoPort());
    _handler->onRequestRetry(key);
    return -1;
  }

  /// Called if registration fails, usage will be destroyed (unless a 
  /// Registration retry interval is enabled in the Profile)
  virtual void onFailure(ClientRegistrationHandle h, const SipMessage& response)
  {
    Uri to = response.header(h_To).uri();
    std::string key = toString(to.getAorNoPort());
    _handler->onFailure(key);
  }

  /// Called before attempting to refresh a registration
  /// Return true if the refresh should go ahead or false otherwise
  /// Default implementation always returns true
  virtual bool onRefreshRequired(ClientRegistrationHandle h, const SipMessage& lastRequest)
  {
    Uri to = lastRequest.header(h_To).uri();
    std::string key = toString(to.getAorNoPort());
    _handler->onRefreshRequired(key);
    return true;
  }
  
private:
  ResipClientRegistrationHandler* _handler;
};

JS_CLASS_INTERFACE(ResipClientRegistrationHandler, "ClientRegistrationHandler") 
{ 
  JS_CLASS_METHOD_DEFINE(ResipClientRegistrationHandler, "onSuccess", handleOnSuccess);
  JS_CLASS_METHOD_DEFINE(ResipClientRegistrationHandler, "onRemoved", handleOnRemoved);
  JS_CLASS_METHOD_DEFINE(ResipClientRegistrationHandler, "onRequestRetry", handleOnRequestRetry);
  JS_CLASS_METHOD_DEFINE(ResipClientRegistrationHandler, "onFailure", handleOnFailure);
  JS_CLASS_METHOD_DEFINE(ResipClientRegistrationHandler, "onRefreshRequired", handleOnRefreshRequired);

  JS_CLASS_INTERFACE_END(ResipClientRegistrationHandler); 
}

JS_CONSTRUCTOR_IMPL(ResipClientRegistrationHandler)
{
  ResipClientRegistrationHandler* object = new ResipClientRegistrationHandler();
  object->_handler = new client_registration_handler(object);
  object->_isolate = OSS::JS::JSIsolate::getIsolate();
  object->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

ResipClientRegistrationHandler::ResipClientRegistrationHandler() :
  _handler(0),
  _handleOnSuccess(0),
  _handleOnRemoved(0),
  _handleOnRequestRetry(0),
  _handleOnFailure(0),
  _handleOnRefreshRequired(0)
{
}

ResipClientRegistrationHandler::~ResipClientRegistrationHandler()
{
  delete _handler;
  
  if (_handleOnSuccess)
  {
    _handleOnSuccess->Dispose();
  }
  if (_handleOnRemoved)
  {
    _handleOnRemoved->Dispose();
  }
  if (_handleOnRequestRetry)
  {
    _handleOnRequestRetry->Dispose();
  }
  if (_handleOnFailure)
  {
    _handleOnFailure->Dispose();
  }
  if (_handleOnRefreshRequired)
  {
    _handleOnRefreshRequired->Dispose();
  }
  
  delete _handleOnSuccess;
  delete _handleOnRemoved;
  delete _handleOnRequestRetry;
  delete _handleOnFailure;
  delete _handleOnRefreshRequired;
}

JS_METHOD_IMPL(ResipClientRegistrationHandler::handleOnSuccess)
{
  js_method_arg_declare_self(ResipClientRegistrationHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnSuccess = new JSPersistentFunctionHandle;
  *(self->_handleOnSuccess) = handler;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientRegistrationHandler::handleOnRemoved)
{
  js_method_arg_declare_self(ResipClientRegistrationHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnRemoved = new JSPersistentFunctionHandle;
  *(self->_handleOnRemoved) = handler;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientRegistrationHandler::handleOnRequestRetry)
{
  js_method_arg_declare_self(ResipClientRegistrationHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnRequestRetry = new JSPersistentFunctionHandle;
  *(self->_handleOnRequestRetry) = handler;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientRegistrationHandler::handleOnFailure)
{
  js_method_arg_declare_self(ResipClientRegistrationHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnFailure = new JSPersistentFunctionHandle;
  *(self->_handleOnFailure) = handler;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientRegistrationHandler::handleOnRefreshRequired)
{
  js_method_arg_declare_self(ResipClientRegistrationHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnRefreshRequired = new JSPersistentFunctionHandle;
  *(self->_handleOnRefreshRequired) = handler;
  return JSUndefined();
}

void ResipClientRegistrationHandler::onSuccessIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnSuccess)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnSuccess)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientRegistrationHandler::onRemovedIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnRemoved)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnRemoved)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientRegistrationHandler::onRequestRetryIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnRequestRetry)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnRequestRetry)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientRegistrationHandler::onFailureIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnFailure)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnFailure)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientRegistrationHandler::onRefreshRequiredIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnRefreshRequired)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnRefreshRequired)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientRegistrationHandler::onSuccess(const std::string& key)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  _isolate->doTask(boost::bind(&ResipClientRegistrationHandler::onSuccessIsolated, this, _1), user_data);
}

void ResipClientRegistrationHandler::onRemoved(const std::string& key)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  _isolate->doTask(boost::bind(&ResipClientRegistrationHandler::onRemovedIsolated, this, _1), user_data);
}

void ResipClientRegistrationHandler::onRequestRetry(const std::string& key)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  _isolate->doTask(boost::bind(&ResipClientRegistrationHandler::onRequestRetryIsolated, this, _1), user_data);
}

void ResipClientRegistrationHandler::onFailure(const std::string& key)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  _isolate->doTask(boost::bind(&ResipClientRegistrationHandler::onFailureIsolated, this, _1), user_data);
}

void ResipClientRegistrationHandler::onRefreshRequired(const std::string& key)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  _isolate->doTask(boost::bind(&ResipClientRegistrationHandler::onRefreshRequiredIsolated, this, _1), user_data);
}