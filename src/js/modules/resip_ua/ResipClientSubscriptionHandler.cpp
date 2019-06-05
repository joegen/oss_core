#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"
#include "OSS/UTL/Thread.h"
#include <rutil/Logger.hxx>
#include "OSS/JS/modules/ResipClientSubscriptionHandler.h"
#include <resip/dum/ClientSubscription.hxx>


using OSS::JS::JSObjectWrap;
using namespace resip;

/// ClientSubscriptionHandler ///
class client_subscription_handler : public ClientSubscriptionHandler
{
public:
  struct SubscriptionHandle
  {
    ClientSubscriptionHandle h;
    std::string nextRefreshContentType;
    std::string nextRefreshContent;
  };
  typedef std::map<std::string, SubscriptionHandle> Subscriptions;
  static Subscriptions _subscriptions;
  static OSS::mutex_critic_sec _mutex;
  
  client_subscription_handler(ResipClientSubscriptionHandler* handler);
    
  virtual ~client_subscription_handler();
  
  static std::string toString(const SipMessage& notify);
  
  static std::string toString(const Data& data);
  
  //Client must call acceptUpdate or rejectUpdate for any onUpdateFoo
  virtual void onUpdatePending(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder);
  
  virtual void onUpdateActive(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder);
  
  //unknown Subscription-State value
  virtual void onUpdateExtension(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder);

  virtual int onRequestRetry(ClientSubscriptionHandle, int retrySeconds, const SipMessage& notify);

  //subscription can be ended through a notify or a failure response.
  virtual void onTerminated(ClientSubscriptionHandle h, const SipMessage* msg);
  
  //not sure if this has any value - can be called for either a 200/SUBSCRIBE or a NOTIFY - whichever arrives first
  virtual void onNewSubscription(ClientSubscriptionHandle h, const SipMessage& notify);

  virtual void onNotifyNotReceived(ClientSubscriptionHandle h);

  /// Called when a TCP or TLS flow to the server has terminated.  This can be caused by socket
  /// errors, or missing CRLF keep alives pong responses from the server.
  //  Called only if clientOutbound is enabled on the UserProfile and the first hop server 
  /// supports RFC5626 (outbound).
  /// Default implementation is to re-form the subscription using a new flow
  virtual void onFlowTerminated(ClientSubscriptionHandle);
  
  /// called to allow app to adorn a message.
  virtual void onReadyToSend(ClientSubscriptionHandle, SipMessage& msg);
  
  static void addSubscription(ClientSubscriptionHandle handle);
  static void removeSubscription(ClientSubscriptionHandle handle);
  static bool requestRefresh(const std::string& id, const std::string& contentType, const std::string& content, OSS::UInt32 interval = 0);
  static bool getSubscribePayload(const std::string& id, std::string& contentType, std::string& content);
  static bool flushSubscribePayload(const std::string& id);
private:
  ResipClientSubscriptionHandler* _handler;
};

client_subscription_handler::Subscriptions client_subscription_handler::_subscriptions;
OSS::mutex_critic_sec client_subscription_handler::_mutex;

client_subscription_handler::client_subscription_handler(ResipClientSubscriptionHandler* handler) :
  _handler(handler)
{
}

client_subscription_handler::~client_subscription_handler()
{
}

void client_subscription_handler::addSubscription(ClientSubscriptionHandle h)
{
  OSS::mutex_critic_sec_lock lock(client_subscription_handler::_mutex);
  std::string id = toString(h->getId());
  SubscriptionHandle handle;
  handle.h = h;
  client_subscription_handler::_subscriptions[id] = handle;
}
void client_subscription_handler::removeSubscription(ClientSubscriptionHandle h)
{
  OSS::mutex_critic_sec_lock lock(client_subscription_handler::_mutex);
  std::string id = toString(h->getId());
  client_subscription_handler::_subscriptions.erase(id);
}

bool client_subscription_handler::requestRefresh(const std::string& id, const std::string& contentType, const std::string& content, OSS::UInt32 interval)
{
  OSS::mutex_critic_sec_lock lock(client_subscription_handler::_mutex);
  Subscriptions::iterator iter =  client_subscription_handler::_subscriptions.find(id);
  if (iter != client_subscription_handler::_subscriptions.end())
  {
    iter->second.nextRefreshContentType = contentType;
    iter->second.nextRefreshContent = content;
    ClientSubscriptionHandle h = iter->second.h;
    h->requestRefresh(interval);
    return true;
  }
  return false;
}

bool client_subscription_handler::getSubscribePayload(const std::string& id, std::string& contentType, std::string& content)
{
  OSS::mutex_critic_sec_lock lock(client_subscription_handler::_mutex);
  Subscriptions::iterator iter =  client_subscription_handler::_subscriptions.find(id);
  if (iter != client_subscription_handler::_subscriptions.end())
  {
    contentType = iter->second.nextRefreshContentType;
    content = iter->second.nextRefreshContent;
    return true;
  }
  return false;
}

bool client_subscription_handler::flushSubscribePayload(const std::string& id)
{
  OSS::mutex_critic_sec_lock lock(client_subscription_handler::_mutex);
  Subscriptions::iterator iter =  client_subscription_handler::_subscriptions.find(id);
  if (iter != client_subscription_handler::_subscriptions.end())
  {
    iter->second.nextRefreshContentType = "";
    iter->second.nextRefreshContent = "";
    return true;
  }
  return false;
}

bool ResipClientSubscriptionHandler::requestRefresh(const std::string& id, const std::string& contentType, std::string& content, OSS::UInt32 interval)
{
  return client_subscription_handler::requestRefresh(id, contentType, content, interval);
}

std::string client_subscription_handler::toString(const SipMessage& notify)
{
  const HeaderFieldValue& body = notify.getRawBody();
  std::string data(body.getBuffer(), body.getLength());
  return data;
}

std::string client_subscription_handler::toString(const Data& data)
{
  return std::string(data.data(), data.size());
}

void client_subscription_handler::onReadyToSend(ClientSubscriptionHandle h, SipMessage& msg)
{
  std::string id = toString(h->getId());
  std::string contentType;
  std::string content;
  if (client_subscription_handler::getSubscribePayload(id, contentType, content) && !contentType.empty() && !content.empty())
  {
    std::vector<std::string> tokens = OSS::string_tokenize(contentType, "/");
    if (tokens.size() == 2) 
    {
      msg.setBody(content.c_str(), content.length());
      msg.header(h_ContentType).type() = tokens[0].c_str();
      msg.header(h_ContentType).subType() = tokens[1].c_str();
      msg.header(h_ContentLength).value() = content.length();
    }
  }
}

//Client must call acceptUpdate or rejectUpdate for any onUpdateFoo
void client_subscription_handler::onUpdatePending(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder)
{
  h->acceptUpdate();
  std::string key = toString(h->getDocumentKey());
  std::string id = toString(h->getId());
  std::string content = toString(notify);
  client_subscription_handler::flushSubscribePayload(id);
  _handler->onUpdatePending(key, id, content, outOfOrder);
}

void client_subscription_handler::onUpdateActive(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder)
{
  h->acceptUpdate();
  std::string key = toString(h->getDocumentKey());
  std::string id = toString(h->getId());
  std::string content = toString(notify);
  client_subscription_handler::flushSubscribePayload(id);
  _handler->onUpdateActive(key, id, content, outOfOrder);
}

//unknown Subscription-State value
void client_subscription_handler::onUpdateExtension(ClientSubscriptionHandle h, const SipMessage& notify, bool outOfOrder)
{
  h->acceptUpdate();
  std::string key = toString(h->getDocumentKey());
  std::string id = toString(h->getId());
  std::string content = toString(notify);
  _handler->onUpdateExtension(key, id, content, outOfOrder);
}

int client_subscription_handler::onRequestRetry(ClientSubscriptionHandle, int retrySeconds, const SipMessage& notify)
{
  // Request failure on Retry-After header
  return -1;
}

//subscription can be ended through a notify or a failure response.
void client_subscription_handler::onTerminated(ClientSubscriptionHandle h, const SipMessage* msg)
{
  std::string key = toString(h->getDocumentKey());
  std::string id = toString(h->getId());
  _handler->onTerminated(key, id);
}

//not sure if this has any value - can be called for either a 200/SUBSCRIBE or a NOTIFY - whichever arrives first
void client_subscription_handler::onNewSubscription(ClientSubscriptionHandle h, const SipMessage& notify)
{
  std::string key = toString(h->getDocumentKey());
  std::string id = toString(h->getId());
  _handler->onNewSubscription(key, id);
}

void client_subscription_handler::onNotifyNotReceived(ClientSubscriptionHandle h)
{
  std::string key = toString(h->getDocumentKey());
  std::string id = toString(h->getId());
  _handler->onNotifyNotReceived(key, id);
  h->end();
}

/// Called when a TCP or TLS flow to the server has terminated.  This can be caused by socket
/// errors, or missing CRLF keep alives pong responses from the server.
//  Called only if clientOutbound is enabled on the UserProfile and the first hop server 
/// supports RFC5626 (outbound).
/// Default implementation is to re-form the subscription using a new flow
void client_subscription_handler::onFlowTerminated(ClientSubscriptionHandle)
{
}


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
  object->_handler = new client_subscription_handler(object);
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
    result->ToObject()->Set(JSLiteral("id"), JSString(data->id.c_str()));
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
    result->ToObject()->Set(JSLiteral("id"), JSString(data->id.c_str()));
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
    result->ToObject()->Set(JSLiteral("id"), JSString(data->id.c_str()));
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
    result->ToObject()->Set(JSLiteral("id"), JSString(data->id.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnTerminated)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}
  
void ResipClientSubscriptionHandler::onUpdatePending(const std::string& key, const std::string& id, const std::string& eventBody, bool isOutOfOrder)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->id = id;
  user_data->eventBody = eventBody;
  user_data->isOutOfOrder = isOutOfOrder;
  _isolate->doTask(boost::bind(&ResipClientSubscriptionHandler::onUpdatePendingIsolated, this, _1), user_data);
}

void ResipClientSubscriptionHandler::onUpdateActive(const std::string& key, const std::string& id, const std::string& eventBody, bool isOutOfOrder)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->id = id;
  user_data->eventBody = eventBody;
  user_data->isOutOfOrder = isOutOfOrder;
  _isolate->doTask(boost::bind(&ResipClientSubscriptionHandler::onUpdateActiveIsolated, this, _1), user_data);
}

void ResipClientSubscriptionHandler::onUpdateExtension(const std::string& key, const std::string& id, const std::string& eventBody, bool isOutOfOrder)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->id = id;
  user_data->eventBody = eventBody;
  user_data->isOutOfOrder = isOutOfOrder;
  _isolate->doTask(boost::bind(&ResipClientSubscriptionHandler::onUpdateExtensionIsolated, this, _1), user_data);
}

void ResipClientSubscriptionHandler::onTerminated(const std::string& key, const std::string& id)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->id = id;
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
    result->ToObject()->Set(JSLiteral("id"), JSString(data->id.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnNewSubscription)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientSubscriptionHandler::onNewSubscription(const std::string& key, const std::string& id)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->id = id;
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
    result->ToObject()->Set(JSLiteral("id"), JSString(data->id.c_str()));
    JSLocalArgumentVector args;
    args.push_back(result);
    (*_handleOnNotifyNotReceived)->Call(js_get_global(), args.size(), args.data());
  }
  delete data;
}

void ResipClientSubscriptionHandler::onNotifyNotReceived(const std::string& key, const std::string& id)
{
  update_data* user_data = new update_data();
  user_data->key = key;
  user_data->id = id;
  user_data->isOutOfOrder = false;
  _isolate->doTask(boost::bind(&ResipClientSubscriptionHandler::onNotifyNotReceivedIsolated, this, _1), user_data);
}