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

#ifndef OSS_RESIPDIALOGUSAGEMANAGER_H_INCLUDED
#define OSS_RESIPDIALOGUSAGEMANAGER_H_INCLUDED


#include <v8.h>
#include <vector>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSEventArgument.h"

#include <resip/dum/SubscriptionHandler.hxx>
#include <resip/dum/RegistrationHandler.hxx>
#include <resip/dum/Handles.hxx>
#include <resip/dum/DialogUsageManager.hxx>
#include <resip/dum/DumThread.hxx>
#include <resip/dum/DumFeature.hxx>
#include <resip/dum/OutgoingEvent.hxx>
#include <resip/dum/DumShutdownHandler.hxx>
#include <rutil/SharedPtr.hxx>

// This feature is used to hardcode the host and port of the Contact header and
// will apply to any outgoing SIP requests. 
class ResipOverrideContact : public resip::DumFeature 
{
public:
  ResipOverrideContact(const resip::Uri& contact, 
                  resip::DialogUsageManager& dum, 
                  resip::TargetCommand::Target& target) : 
     DumFeature(dum, target),
     _contact(contact)
  {
  }

  virtual ~ResipOverrideContact() {};

  virtual ProcessingResult process(resip::Message* msg)
  {
     resip::OutgoingEvent* og = dynamic_cast<resip::OutgoingEvent*>(msg);
     if (og)
     {
        resip::SharedPtr<resip::SipMessage> sip = og->message();
        if (sip->isRequest() && 
            sip->exists(resip::h_Contacts) && 
            sip->header(resip::h_Contacts).size() == 1)
        {
           sip->header(resip::h_Contacts).front().uri().host() = _contact.host();
           sip->header(resip::h_Contacts).front().uri().port() = _contact.port();
        }
     }
     return resip::DumFeature::FeatureDone;
  }
private:
  resip::Uri _contact;
};

class ResipDumShutdownHandler : public resip::DumShutdownHandler
{
public:
};

class ResipDialogUsageManager : public OSS::JS::JSObjectWrap, resip::DumShutdownHandler
{
public:
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(run);
  JS_METHOD_DECLARE(shutdown);
  JS_METHOD_DECLARE(setMasterProfile);
  JS_METHOD_DECLARE(addClientSubscriptionHandler);
  JS_METHOD_DECLARE(overrideContact);
  JS_METHOD_DECLARE(sendClientSubscription);

  resip::DialogUsageManager* dum();
  virtual void onDumCanBeDeleted();
private:
  ResipDialogUsageManager();
  virtual ~ResipDialogUsageManager();
  resip::DialogUsageManager* _dum;
  resip::DumThread* _thread;
};

//
// Inlines
//

inline resip::DialogUsageManager* ResipDialogUsageManager::dum()
{
  return _dum;
}
#endif // OSS_RESIPDIALOGUSAGEMANAGER_H_INCLUDED