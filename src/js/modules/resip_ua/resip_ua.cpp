#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"

#include "OSS/JS/modules/ResipSIPStack.h"
#include "OSS/JS/modules/ResipDialogUsageManager.h"
#include "OSS/JS/modules/ResipMasterProfile.h"
#include "OSS/JS/modules/ResipClientRegistrationHandler.h"
#include "OSS/JS/modules/ResipClientSubscriptionHandler.h"
#include "resip/dum/ClientSubscription.hxx"

using OSS::JS::JSObjectWrap;
using namespace resip;

/// GLOBAL EXPORTS ///

#define EXPORT_GLOBALS() \
  js_export_class(ResipSIPStack); \
  js_export_class(ResipDialogUsageManager); \
  js_export_class(ResipMasterProfile); \
  js_export_class(ResipClientRegistrationHandler); \
  js_export_class(ResipClientSubscriptionHandler); \
  CONST_EXPORT(UNKNOWN_TRANSPORT); \
  CONST_EXPORT(TLS); \
  CONST_EXPORT(TCP); \
  CONST_EXPORT(UDP); \
  CONST_EXPORT(SCTP); \
  CONST_EXPORT(DCCP); \
  CONST_EXPORT(DTLS); \
  CONST_EXPORT(WS); \
  CONST_EXPORT(WSS); \
  CONST_EXPORT(MAX_TRANSPORT); \
  CONST_EXPORT(UNKNOWN); \
  CONST_EXPORT(ACK); \
  CONST_EXPORT(BYE); \
  CONST_EXPORT(CANCEL); \
  CONST_EXPORT(INVITE); \
  CONST_EXPORT(NOTIFY); \
  CONST_EXPORT(OPTIONS); \
  CONST_EXPORT(REFER); \
  CONST_EXPORT(REGISTER); \
  CONST_EXPORT(SUBSCRIBE); \
  CONST_EXPORT(RESPONSE); \
  CONST_EXPORT(MESSAGE); \
  CONST_EXPORT(INFO); \
  CONST_EXPORT(PRACK); \
  CONST_EXPORT(PUBLISH); \
  CONST_EXPORT(SERVICE); \
  CONST_EXPORT(UPDATE);

/// SipStack ///

JS_CLASS_INTERFACE(ResipSIPStack, "SipStack") 
{
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "run", run);
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "shutdown", shutdown);
  JS_CLASS_METHOD_DEFINE(ResipSIPStack, "addTransport", addTransport);
  JS_CLASS_INTERFACE_END(ResipSIPStack); 
}

ResipSIPStack::ResipSIPStack() :
  _stack(0)
{
}

ResipSIPStack::~ResipSIPStack()
{
  delete _stack;
}

JS_CONSTRUCTOR_IMPL(ResipSIPStack)
{
  js_method_arg_declare_persistent_function(handler, 0);
  ResipSIPStack* object = new ResipSIPStack();
  object->_stack = new SipStack();
  object->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(ResipSIPStack::run)
{
  js_method_arg_declare_self(ResipSIPStack, stack);
  stack->_stack->run();
  return JSUndefined();
}

JS_METHOD_IMPL(ResipSIPStack::shutdown)
{
  js_method_arg_declare_self(ResipSIPStack, stack);
  stack->_stack->shutdown();
  return JSUndefined();
}

JS_METHOD_IMPL(ResipSIPStack::addTransport)
{
  js_method_arg_declare_self(ResipSIPStack, self);
  js_method_arg_declare_int32(type, 0);
  js_method_arg_declare_uint32(port, 1);
  assert(self);
  TransportType transport_type = (TransportType)type;
  self->stack()->addTransport(transport_type, port);
  return JSUndefined();
}

/// DialogUsageManager ///

JS_CLASS_INTERFACE(ResipDialogUsageManager, "DialogUsageManager") 
{
  JS_CLASS_METHOD_DEFINE(ResipDialogUsageManager, "setMasterProfile", setMasterProfile);
  JS_CLASS_METHOD_DEFINE(ResipDialogUsageManager, "addClientSubscriptionHandler", addClientSubscriptionHandler);
  JS_CLASS_METHOD_DEFINE(ResipDialogUsageManager, "setClientRegistrationHandler", setClientRegistrationHandler);
  JS_CLASS_INTERFACE_END(ResipDialogUsageManager); 
}

ResipDialogUsageManager::ResipDialogUsageManager() :
  _dum(0)
{
}
ResipDialogUsageManager::~ResipDialogUsageManager()
{
  delete _dum;
}

JS_CONSTRUCTOR_IMPL(ResipDialogUsageManager)
{
  js_method_arg_declare_object(sipstack_object, 0);
  ResipSIPStack* sipstack = js_unwrap_object(ResipSIPStack, sipstack_object->ToObject());
  assert(sipstack);
  ResipDialogUsageManager* object = new ResipDialogUsageManager();
  object->_dum = new DialogUsageManager(*sipstack->stack());
  object->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(ResipDialogUsageManager::setMasterProfile)
{
  js_method_arg_declare_self(ResipDialogUsageManager, self);  
  js_method_arg_declare_unwrapped_object(ResipMasterProfile, profile, 0);
  self->dum()->setMasterProfile(profile->profile());
  return JSUndefined();
}

JS_METHOD_IMPL(ResipDialogUsageManager::addClientSubscriptionHandler)
{
  js_method_arg_declare_self(ResipDialogUsageManager, self);
  js_method_arg_declare_string(event, 0);
  js_method_arg_declare_unwrapped_object(ResipClientSubscriptionHandler, handler, 1);
  self->dum()->addClientSubscriptionHandler(event.c_str(), handler->handler());
  return JSUndefined();
}

JS_METHOD_IMPL(ResipDialogUsageManager::setClientRegistrationHandler)
{
  js_method_arg_declare_self(ResipDialogUsageManager, self);  
  js_method_arg_declare_unwrapped_object(ResipClientRegistrationHandler, handler, 0);
  self->dum()->setClientRegistrationHandler(handler->handler());
  return JSUndefined();
}

//
// MasterProfile
//

JS_CLASS_INTERFACE(ResipMasterProfile, "MasterProfile") 
{  
  JS_CLASS_METHOD_DEFINE(ResipMasterProfile, "addSupportedMethod", addSupportedMethod);
  JS_CLASS_METHOD_DEFINE(ResipMasterProfile, "addAllowedEvent", addAllowedEvent);
  JS_CLASS_METHOD_DEFINE(ResipMasterProfile, "validateAcceptEnabled", validateAcceptEnabled);
  JS_CLASS_METHOD_DEFINE(ResipMasterProfile, "validateContentEnabled", validateContentEnabled);
  JS_CLASS_METHOD_DEFINE(ResipMasterProfile, "setUserAgent", setUserAgent);
  JS_CLASS_METHOD_DEFINE(ResipMasterProfile, "setOutboundProxy", setOutboundProxy);
  JS_CLASS_METHOD_DEFINE(ResipMasterProfile, "setDefaultFrom", setDefaultFrom);
  JS_CLASS_METHOD_DEFINE(ResipMasterProfile, "setDigestCredential", setDigestCredential);
  JS_CLASS_INTERFACE_END(ResipMasterProfile); 
}

ResipMasterProfile::ResipMasterProfile()
{
}

ResipMasterProfile::~ResipMasterProfile()
{
}

JS_CONSTRUCTOR_IMPL(ResipMasterProfile)
{
  ResipMasterProfile* object = new ResipMasterProfile();
  object->_profile = resip::SharedPtr<resip::MasterProfile>(new resip::MasterProfile());
  object->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(ResipMasterProfile::addSupportedMethod)
{
  js_method_arg_declare_self(ResipMasterProfile, self);
  js_method_arg_declare_int32(value, 0);
  MethodTypes method_type = (MethodTypes)value;
  self->profile()->addSupportedMethod(method_type);
  return JSUndefined();
}

JS_METHOD_IMPL(ResipMasterProfile::addAllowedEvent)
{
  js_method_arg_declare_self(ResipMasterProfile, self);
  js_method_arg_declare_string(value, 0);
  Token token(value.c_str());
  self->profile()->addAllowedEvent(token);
  return JSUndefined();
}

JS_METHOD_IMPL(ResipMasterProfile::validateAcceptEnabled)
{
  js_method_arg_declare_self(ResipMasterProfile, self);
  js_method_arg_declare_bool(value, 0);
  self->profile()->validateAcceptEnabled() = value;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipMasterProfile::validateContentEnabled)
{
  js_method_arg_declare_self(ResipMasterProfile, self);
  js_method_arg_declare_bool(value, 0);
  self->profile()->validateContentEnabled() = value;
  return JSUndefined();
}

JS_METHOD_IMPL(ResipMasterProfile::setUserAgent)
{
  js_method_arg_declare_self(ResipMasterProfile, self);
  js_method_arg_declare_string(value, 0);
  self->profile()->setUserAgent(value.c_str());
  return JSUndefined();
}

JS_METHOD_IMPL(ResipMasterProfile::setOutboundProxy)
{
  js_method_arg_declare_self(ResipMasterProfile, self);
  js_method_arg_declare_string(value, 0);
  Uri uri(Data(value.c_str()));
  self->profile()->setOutboundProxy(uri);
  return JSUndefined();
}

JS_METHOD_IMPL(ResipMasterProfile::setDefaultFrom)
{
  js_method_arg_declare_self(ResipMasterProfile, self);
  js_method_arg_declare_string(value, 0);
  NameAddr aor(Data(value.c_str()));
  self->profile()->setDefaultFrom(aor);
  return JSUndefined();
}

JS_METHOD_IMPL(ResipMasterProfile::setDigestCredential)
{
  js_method_arg_declare_self(ResipMasterProfile, self);
  js_method_arg_declare_string(realm, 0);
  js_method_arg_declare_string(user, 1);
  js_method_arg_declare_string(password, 2);
  self->profile()->setDigestCredential(realm.c_str(), user.c_str(), password.c_str());
  return JSUndefined();
}

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
  virtual void onTerminated(ClientSubscriptionHandle, const SipMessage* msg)
  {
  }
  
  //not sure if this has any value - can be called for either a 200/SUBSCRIBE or a NOTIFY - whichever arrives first
  virtual void onNewSubscription(ClientSubscriptionHandle, const SipMessage& notify)
  {
  }
  
  virtual void onNotifyNotReceived(ClientSubscriptionHandle)
  {
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
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "handleOnUpdatePending", handleOnUpdatePending);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "handleOnUpdateActive", handleOnUpdateActive);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "handleOnUpdateExtension", handleOnUpdateExtension);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "handleOnTerminated", handleOnTerminated);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "handleOnNewSubscription", handleOnNewSubscription);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "handleOnNotifyNotReceived", handleOnNotifyNotReceived);
  JS_CLASS_METHOD_DEFINE(ResipClientSubscriptionHandler, "handleOnFlowTerminated", handleOnFlowTerminated);
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
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnUpdateActive)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnUpdateExtension)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnTerminated)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnNewSubscription)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnNotifyNotReceived)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientSubscriptionHandler::handleOnFlowTerminated)
{
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


#if 1
#include "rutil/Logger.hxx"
#include "resip/dum/MasterProfile.hxx"
#include "resip/dum/DumFeature.hxx"
#include "resip/dum/OutgoingEvent.hxx"

#include "reg_event_ua.h"

using namespace resip;

static SharedPtr<MasterProfile> master_profile(new MasterProfile());
static RegEventClient* reg_event_client = 0;
static JSPersistentFunctionHandle* reg_event_client_callback = 0;
static OSS::JS::JSIsolate::Ptr active_isolate;

JS_METHOD_IMPL(__profile_set_callback)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_function(0);
  if (!reg_event_client_callback)
  {
    reg_event_client_callback = new JSPersistentFunctionHandle;
    *reg_event_client_callback = js_method_arg_as_persistent_function(0);
  }
  return JSBoolean(true);
}

JS_METHOD_IMPL(__profile_set_from)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string from = js_method_arg_as_std_string(0);
  NameAddr aor(Data(from.c_str()));
  master_profile->setDefaultFrom(aor);
  return JSBoolean(true);
}

JS_METHOD_IMPL(__profile_set_outbound_proxy)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string proxy = js_method_arg_as_std_string(0);
  Uri uri(Data(proxy.c_str()));
  master_profile->setOutboundProxy(uri);
  return JSBoolean(true);
}

JS_METHOD_IMPL(__profile_set_default_subscription_time)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  OSS::UInt32 expire = js_method_arg_as_uint32(0);
  master_profile->setDefaultSubscriptionTime(expire);
  return JSBoolean(true);
}

JS_METHOD_IMPL(__profile_set_digest_credential)
{
  js_method_arg_assert_size_eq(3);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  js_method_arg_assert_string(2);

  Data userName(js_method_arg_as_std_string(0));
  Data password(js_method_arg_as_std_string(1));
  Data realm(js_method_arg_as_std_string(2));
  master_profile->setDigestCredential(realm, userName, password);
  return JSBoolean(true);
}

struct reg_event_data
{
  std::string aor;
  std::string reg;
};

static void handle_reg_event_isolated(void* user_data)
{
  reg_event_data* data = (reg_event_data*)user_data;
  assert(data);
  JSLocalValueHandle result = JSObject();
  result->ToObject()->Set(JSLiteral("aor"), JSString(data->aor.c_str()));
  result->ToObject()->Set(JSLiteral("reg"), JSString(data->reg.c_str()));
  JSLocalArgumentVector args;
  args.push_back(result);
  (*reg_event_client_callback)->Call(js_get_global(), args.size(), args.data());
}

static void handle_reg_event(const Data& aor, const Data& reg)
{
  reg_event_data* data = new reg_event_data();
  data->aor = aor.c_str();
  data->reg = reg.c_str();
  active_isolate->doTask(boost::bind(handle_reg_event_isolated, _1), data);
}

JS_METHOD_IMPL(__watch_reg_events)
{
  if (!reg_event_client)
  {
    active_isolate = OSS::JS::JSIsolate::getIsolate();
    Log::initialize(Log::Cout, Log::Debug, "oss_core");
    master_profile->addSupportedMethod(NOTIFY);
    master_profile->addAllowedEvent(Token("mwi"));
    master_profile->validateAcceptEnabled() = false;
    master_profile->validateContentEnabled() = false;
    master_profile->setUserAgent("RFC3680-testUA");
    reg_event_client = new RegEventClient(master_profile);
    reg_event_client->_cb = boost::bind(handle_reg_event, _1, _2);
    reg_event_client->run();
  }
  
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  Data arg(js_method_arg_as_std_string(0));
  Uri aor(arg);
  reg_event_client->watchAor(aor);
  
  return JSBoolean(true);
}

#else
JS_METHOD_IMPL(__profile_set_callback)
{
  return JSBoolean(true);
}

JS_METHOD_IMPL(__profile_set_from)
{
  return JSBoolean(true);
}

JS_METHOD_IMPL(__profile_set_digest_credential)
{
  return JSBoolean(true);
}

JS_METHOD_IMPL(__watch_reg_events)
{
  return JSBoolean(true);
}
#endif
JS_EXPORTS_INIT()
{
  js_export_method("_watch_reg_events", __watch_reg_events);
  js_export_method("_profile_set_from", __profile_set_from);
  js_export_method("_profile_set_default_subscription_time", __profile_set_default_subscription_time);
  
  js_export_method("_profile_set_outbound_proxy", __profile_set_outbound_proxy);
  js_export_method("_profile_set_digest_credential", __profile_set_digest_credential);
  js_export_method("_profile_set_callback", __profile_set_callback);
  
  EXPORT_GLOBALS();
  
  js_export_finalize();
}

JS_REGISTER_MODULE(JSRESIPUA);