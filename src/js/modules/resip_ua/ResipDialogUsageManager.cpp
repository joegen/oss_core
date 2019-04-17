#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"

#include <resip/dum/ClientSubscription.hxx>
#include <resip/dum/ClientAuthManager.hxx>
#include <rutil/FdPoll.hxx>
#include <rutil/Logger.hxx>

#include "OSS/JS/modules/ResipSIPStack.h"
#include "OSS/JS/modules/ResipDialogUsageManager.h"
#include "OSS/JS/modules/ResipMasterProfile.h"
#include "OSS/JS/modules/ResipUserProfile.h"
#include "OSS/JS/modules/ResipClientSubscriptionHandler.h"


using OSS::JS::JSObjectWrap;
using namespace resip;


class SendClientSubscription : public resip::DumCommand
{
public:
  SendClientSubscription(const Token& event, ResipDialogUsageManager& dum, const resip::SharedPtr<resip::UserProfile>& profile, const resip::Uri& aor, 
                         UInt32 subscriptionTime, int refreshInterval) :
    _event(event),
    _dum(dum),
    _aor(aor),
    _subscriptionTime(subscriptionTime),
    _refreshInterval(refreshInterval)
  {
      _profile = profile;
  }
  
  ~SendClientSubscription()
  {
  }

  virtual void executeCommand()
  {
    SharedPtr<SipMessage> sub = _dum.dum()->makeSubscription(resip::NameAddr(_aor), _profile, _event.value(), _subscriptionTime, _refreshInterval);
    _dum.dum()->send(sub);
  }

  virtual resip::Message* clone() const
  {
    return new SendClientSubscription(_event, _dum, _profile, _aor, _subscriptionTime, _refreshInterval);
  }

  virtual std::ostream& encode(std::ostream& strm) const
  {
    strm << "Add " <<  _event.value() << " watcher " << _aor;
    return strm;
  }
  
  virtual std::ostream& encodeBrief(std::ostream& strm) const
  {
    return encode(strm);
  }

private:
  Token _event;
  ResipDialogUsageManager& _dum;
  resip::SharedPtr<resip::UserProfile> _profile;
  resip::Uri _aor;
  UInt32 _subscriptionTime; 
  int _refreshInterval;
};

JS_METHOD_IMPL(ResipDialogUsageManager::sendClientSubscription)
{
  js_method_arg_declare_self(ResipDialogUsageManager, self);  
  js_method_arg_declare_unwrapped_object(ResipUserProfile, profile, 0);
  js_method_arg_declare_string(aor, 1);
  js_method_arg_declare_string(event, 2);
  js_method_arg_declare_uint32(subscriptionTime, 3);
  js_method_arg_declare_int32(refreshInterval, 4);
  
  Token eventToken(event.c_str());
  Uri uri(aor.c_str());
  SendClientSubscription* cmd = new SendClientSubscription(eventToken, *self, profile->profile(), uri, subscriptionTime, refreshInterval);
  self->dum()->post(cmd);
  return JSUndefined();
}

JS_CLASS_INTERFACE(ResipDialogUsageManager, "DialogUsageManager") 
{
  JS_CLASS_METHOD_DEFINE(ResipDialogUsageManager, "run", run);
  JS_CLASS_METHOD_DEFINE(ResipDialogUsageManager, "shutdown", shutdown);
  JS_CLASS_METHOD_DEFINE(ResipDialogUsageManager, "setMasterProfile", setMasterProfile);
  JS_CLASS_METHOD_DEFINE(ResipDialogUsageManager, "addClientSubscriptionHandler", addClientSubscriptionHandler);
  JS_CLASS_METHOD_DEFINE(ResipDialogUsageManager, "overrideContact", overrideContact);
  JS_CLASS_METHOD_DEFINE(ResipDialogUsageManager, "sendClientSubscription", sendClientSubscription);
  JS_CLASS_INTERFACE_END(ResipDialogUsageManager); 
}

ResipDialogUsageManager::ResipDialogUsageManager() :
  _dum(0),
  _thread(0)
{
}

ResipDialogUsageManager::~ResipDialogUsageManager()
{
  _dum->shutdown(this);
  _thread->shutdown();
  _thread->join();

  delete _thread;
  delete _dum;
}

void ResipDialogUsageManager::onDumCanBeDeleted()
{
}

JS_CONSTRUCTOR_IMPL(ResipDialogUsageManager)
{
  OSS::JS::JSIsolate::getIsolate()->setForceAsync(true);
  js_method_arg_declare_object(sipstack_object, 0);
  ResipSIPStack* sipstack = js_unwrap_object(ResipSIPStack, sipstack_object->ToObject());
  assert(sipstack);
  ResipDialogUsageManager* object = new ResipDialogUsageManager();
  object->_dum = new DialogUsageManager(*sipstack->stack());
  
  std::auto_ptr<resip::ClientAuthManager> clam(new resip::ClientAuthManager());
  object->_dum->setClientAuthManager(clam);
  
  object->_thread = new DumThread(*object->_dum);
  object->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(ResipDialogUsageManager::run)
{
  js_method_arg_declare_self(ResipDialogUsageManager, self);
  self->_thread->run();
  return JSUndefined();
}

JS_METHOD_IMPL(ResipDialogUsageManager::shutdown)
{
  js_method_arg_declare_self(ResipDialogUsageManager, self);
  self->_dum->shutdown(self);
  self->_thread->shutdown();
  self->_thread->join();
  return JSUndefined();
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

JS_METHOD_IMPL(ResipDialogUsageManager::overrideContact)
{
  // This is only necessary if the user agent is running behind a NAT.
  js_method_arg_declare_self(ResipDialogUsageManager, self);
  js_method_arg_declare_string(host, 0);
  js_method_arg_declare_uint32(port, 1);
  Uri contact;
  contact.host() = Data(host.c_str());
  contact.port() = port;
  SharedPtr<DumFeature> feature(new ResipOverrideContact(contact, *(self->dum()), self->dum()->dumOutgoingTarget()));
  self->dum()->addOutgoingFeature(feature);
  return JSUndefined();
}