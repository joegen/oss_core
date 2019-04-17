#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"


#include <rutil/Logger.hxx>

#include "OSS/JS/modules/ResipUserProfile.h"


using OSS::JS::JSObjectWrap;
using namespace resip;


/// UserProfile

JS_CLASS_INTERFACE(ResipUserProfile, "UserProfile") 
{  
  JS_CLASS_METHOD_DEFINE(ResipUserProfile, "setDefaultFrom", setDefaultFrom);
  JS_CLASS_METHOD_DEFINE(ResipUserProfile, "setDigestCredential", setDigestCredential);
  JS_CLASS_INTERFACE_END(ResipUserProfile); 
}

ResipUserProfile::ResipUserProfile()
{
}

ResipUserProfile::~ResipUserProfile()
{
}

JS_CONSTRUCTOR_IMPL(ResipUserProfile)
{
  ResipUserProfile* object = new ResipUserProfile();
  object->_profile = resip::SharedPtr<resip::UserProfile>(new resip::UserProfile());
  object->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(ResipUserProfile::setDefaultFrom)
{
  js_method_arg_declare_self(ResipUserProfile, self);
  js_method_arg_declare_string(value, 0);
  NameAddr aor(Data(value.c_str()));
  self->profile()->setDefaultFrom(aor);
  return JSUndefined();
}

JS_METHOD_IMPL(ResipUserProfile::setDigestCredential)
{
  js_method_arg_declare_self(ResipUserProfile, self);
  js_method_arg_declare_string(realm, 0);
  js_method_arg_declare_string(user, 1);
  js_method_arg_declare_string(password, 2);
  self->profile()->setDigestCredential(realm.c_str(), user.c_str(), password.c_str());
  return JSUndefined();
}