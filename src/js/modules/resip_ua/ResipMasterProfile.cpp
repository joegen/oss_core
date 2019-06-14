#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"

#include <rutil/Logger.hxx>

#include "OSS/JS/modules/ResipMasterProfile.h"


using OSS::JS::JSObjectWrap;
using namespace resip;

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
  JS_CLASS_METHOD_DEFINE(ResipMasterProfile, "setClientSubscriptionWaitFornotify)", setClientSubscriptionWaitFornotify);
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

JS_METHOD_IMPL(ResipMasterProfile::setClientSubscriptionWaitFornotify)
{
  js_method_arg_declare_self(ResipMasterProfile, self);
  js_method_arg_declare_uint32(expireMs, 0);
  self->profile()->setClientSubscriptionWaitFornotify(expireMs);
  return JSUndefined();
}