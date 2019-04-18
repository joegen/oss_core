#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"
#include <rutil/Logger.hxx>
#include "OSS/JS/modules/ResipClientSubscriptionHandler.h"
#include <resip/dum/ClientSubscription.hxx>


using OSS::JS::JSObjectWrap;
using namespace resip;

/// ClientSubscriptionHandler ///
class client_susbcription_handler : public ClientSubscriptionHandler
{
public:
  client_susbcription_handler(ResipClientSubscriptionHandler* handler) :
    _handler(handler)
  {
  }
    
  virtual ~client_susbcription_handler()
  {
  }
  
  std::string toString(const SipMessage& notify)
  {
    const HeaderFieldValue& body = notify.getRawBody();
    std::string data(body.getBuffer(), body.getLength());
    return data;
  }
  
  std::string toString(const Data& data)
  {
    return std::string(data.data(), data.size());
  }
  
  //Client must call acceptUpdate or rejectUpdate for any onUpdateFoo
  virtual void onUpdatePending(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder)
  {
    h->acceptUpdate();
    std::string key = toString(h->getDocumentKey());
    std::string content = toString(notify);
    _handler->onUpdatePending(key, content, outOfOrder);
  }
  
  virtual void onUpdateActive(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder)
  {
    h->acceptUpdate();
    std::string key = toString(h->getDocumentKey());
    std::string content = toString(notify);
    _handler->onUpdateActive(key, content, outOfOrder);
  }
  
  //unknown Subscription-State value
  virtual void onUpdateExtension(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder)
  {
    h->acceptUpdate();
    std::string key = toString(h->getDocumentKey());
    std::string content = toString(notify);
    _handler->onUpdateExtension(key, content, outOfOrder);
  }

  virtual int onRequestRetry(ClientSubscriptionHandle, int retrySeconds, const SipMessage& notify)
  {
    // Request failure on Retry-After header
    return -1;
  }

  //subscription can be ended through a notify or a failure response.
  virtual void onTerminated(ClientSubscriptionHandle h, const SipMessage* msg)
  {
    std::string key = toString(h->getDocumentKey());
    _handler->onTerminated(key);
  }
  
  //not sure if this has any value - can be called for either a 200/SUBSCRIBE or a NOTIFY - whichever arrives first
  virtual void onNewSubscription(ClientSubscriptionHandle h, const SipMessage& notify)
  {
    std::string key = toString(h->getDocumentKey());
    _handler->onNewSubscription(key);
  }

  virtual void onNotifyNotReceived(ClientSubscriptionHandle h)
  {
    std::string key = toString(h->getDocumentKey());
    _handler->onNotifyNotReceived(key);
    h->end();
  }

  /// Called when a TCP or TLS flow to the server has terminated.  This can be caused by socket
  /// errors, or missing CRLF keep alives pong responses from the server.
  //  Called only if clientOutbound is enabled on the UserProfile and the first hop server 
  /// supports RFC5626 (outbound).
  /// Default implementation is to re-form the subscription using a new flow
  virtual void onFlowTerminated(ClientSubscriptionHandle)
  {
  }
  
private:
  ResipClientSubscriptionHandler* _handler;
};

JS_CLASS_INTERFACE(ResipClientSubscriptionHandler, "ClientSubscriptionHandler") 
{ 
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "onUpdatePending", handleOnUpdatePending);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "onUpdateActive", handleOnUpdateActive);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "onUpdateExtension", handleOnUpdateExtension);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "onTerminated", handleOnTerminated);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "onNewSubscription", handleOnNewSubscription);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "onNotifyNotReceived", handleOnNotifyNotReceived);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "onFlowTerminated", handleOnFlowTerminated);
  JS_CLASS_INTERFACE_END(ResipClientSubscriptionHandler); 
}

JS_CONSTRUCTOR_IMPL(ResipClientSubscriptionHandler)
{
  ResipClientSubscriptionHandler* object = new ResipClientSubscriptionHandler();
  object->_handler = new client_susbcription_handler(object);
  object->_isolate = OSS::JS::JSIsolate::getIsolate();
  object->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

ResipClientSubscriptionHandler::ResipClientSubscriptionHandler() :
  _handler(0),
  _handleOnUpdatePending(0),
  _handleOnUpdateActive(0),
  _handleOnUpdateExtension(0),
  _handleOnTerminated(0),
  _handleOnNewSubscription(0),
  _handleOnNotifyNotReceived(0),
  _handleOnFlowTerminated(0)
{
}

ResipClientSubscriptionHandler::~ResipClientSubscriptionHandler()
{
  delete _handler;
  
  if (_handleOnUpdatePending)
  {
    _handleOnUpdatePending->Dispose();
  }
  
  if (_handleOnUpdateActive)
  {
    _handleOnUpdateActive->Dispose();
  }
  
  if (_handleOnUpdateExtension)
  {
    _handleOnUpdateExtension->Dispose();
  }
  
  if (_handleOnTerminated)
  {
    _handleOnTerminated->Dispose();
  }
  
  if (_handleOnNewSubscription)
  {
    _handleOnNewSubscription->Dispose();
  }
  
  if (_handleOnNotifyNotReceived)
  {
    _handleOnNotifyNotReceived->Dispose();
  }
  
  if (_handleOnFlowTerminated)
  {
    _handleOnFlowTerminated->Dispose();
  }
  
  delete _handleOnUpdatePending;
  delete _handleOnUpdateActive;
  delete _handleOnUpdateExtension;
  delete _handleOnTerminated;
  delete _handleOnNewSubscription;
  delete _handleOnNotifyNotReceived;
  delete _handleOnFlowTerminated;
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnUpdatePending)
{
  js_method_arg_declare_self(ResipClientSubscriptionHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnUpdatePending = new JSPersistentFunctionHandle;
  *(self->_handleOnUpdatePending) = handler;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnUpdateActive)
{
  js_method_arg_declare_self(ResipClientSubscriptionHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnUpdateActive = new JSPersistentFunctionHandle;
  *(self->_handleOnUpdateActive) = handler;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnUpdateExtension)
{
  js_method_arg_declare_self(ResipClientSubscriptionHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnUpdateExtension = new JSPersistentFunctionHandle;
  *(self->_handleOnUpdateExtension) = handler;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnTerminated)
{
  js_method_arg_declare_self(ResipClientSubscriptionHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnTerminated = new JSPersistentFunctionHandle;
  *(self->_handleOnTerminated) = handler;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnNewSubscription)
{
  js_method_arg_declare_self(ResipClientSubscriptionHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnNewSubscription = new JSPersistentFunctionHandle;
  *(self->_handleOnNewSubscription) = handler;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnNotifyNotReceived)
{
  js_method_arg_declare_self(ResipClientSubscriptionHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnNotifyNotReceived = new JSPersistentFunctionHandle;
  *(self->_handleOnNotifyNotReceived) = handler;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnFlowTerminated)
{
  js_method_arg_declare_self(ResipClientSubscriptionHandler, self);
  js_method_arg_declare_persistent_function(handler, 0);
  self->_handleOnFlowTerminated = new JSPersistentFunctionHandle;
  *(self->_handleOnFlowTerminated) = handler;
  return JSUndefined();
}

void ResipClientSubscriptionHandler::onUpdatePendingIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnUpdatePending)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    result->ToObject()->Set(JSLiteral("eventBody"), JSString(data->eventBody.c_str()));
    result->ToObject()->Set(JSLiteral("isOutOfOrder"), JSBoolean(data->isOutOfOrder));

    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnUpdatePending)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientSubscriptionHandler::onUpdateActiveIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnUpdateActive)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    result->ToObject()->Set(JSLiteral("eventBody"), JSString(data->eventBody.c_str()));
    result->ToObject()->Set(JSLiteral("isOutOfOrder"), JSBoolean(data->isOutOfOrder));

    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnUpdateActive)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientSubscriptionHandler::onUpdateExtensionIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnUpdateExtension)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    result->ToObject()->Set(JSLiteral("eventBody"), JSString(data->eventBody.c_str()));
    result->ToObject()->Set(JSLiteral("isOutOfOrder"), JSBoolean(data->isOutOfOrder));

    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnUpdateExtension)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientSubscriptionHandler::onTerminatedIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnTerminated)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnTerminated)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}
  
void ResipClientSubscriptionHandler::onUpdatePending(const std::string& key, const std::string& eventBody, bool isOutOfOrder)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->eventBody = eventBody;
  user_data->isOutOfOrder = isOutOfOrder;
  _isolate->doTask(boost::bind(&ResipClientSubscriptionHandler::onUpdatePendingIsolated, this, _1), user_data);
}

void ResipClientSubscriptionHandler::onUpdateActive(const std::string& key, const std::string& eventBody, bool isOutOfOrder)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->eventBody = eventBody;
  user_data->isOutOfOrder = isOutOfOrder;
  _isolate->doTask(boost::bind(&ResipClientSubscriptionHandler::onUpdateActiveIsolated, this, _1), user_data);
}

void ResipClientSubscriptionHandler::onUpdateExtension(const std::string& key, const std::string& eventBody, bool isOutOfOrder)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->eventBody = eventBody;
  user_data->isOutOfOrder = isOutOfOrder;
  _isolate->doTask(boost::bind(&ResipClientSubscriptionHandler::onUpdateExtensionIsolated, this, _1), user_data);
}

void ResipClientSubscriptionHandler::onTerminated(const std::string& key)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->isOutOfOrder = false;
  _isolate->doTask(boost::bind(&ResipClientSubscriptionHandler::onTerminatedIsolated, this, _1), user_data);
}

void ResipClientSubscriptionHandler::onNewSubscriptionIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnNewSubscription)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnNewSubscription)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientSubscriptionHandler::onNewSubscription(const std::string& key)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->isOutOfOrder = false;
  _isolate->doTask(boost::bind(&ResipClientSubscriptionHandler::onNewSubscriptionIsolated, this, _1), user_data);
}

void ResipClientSubscriptionHandler::onNotifyNotReceivedIsolated(void* user_data)
{
  update_data* data = (update_data*)user_data;
  assert(data);
  if (_handleOnNotifyNotReceived)
  {
    JSLocalValueHandle result = JSObject();
    result->ToObject()->Set(JSLiteral("key"), JSString(data->key.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnNotifyNotReceived)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientSubscriptionHandler::onNotifyNotReceived(const std::string& key)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->isOutOfOrder = false;
  _isolate->doTask(boost::bind(&ResipClientSubscriptionHandler::onNotifyNotReceivedIsolated, this, _1), user_data);
}