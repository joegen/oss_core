#ifndef RESIPAPPDIALOGSETFACTORY_H_INCLUDED
#define RESIPAPPDIALOGSETFACTORY_H_INCLUDED

#include <v8.h>
#include <vector>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSEventArgument.h"

#include "resip/dum/AppDialog.hxx"
#include "resip/dum/AppDialogSet.hxx"
#include "resip/dum/AppDialogSetFactory.hxx"
#include "resip/dum/DialogUsageManager.hxx"
#include "resip/stack/SipMessage.hxx"

class ResipAppDialogSetFactoryImpl : public resip::AppDialogSetFactory
{
public:
  typedef std::map<std::string, std::string> Variables;
  ResipAppDialogSetFactoryImpl();
  virtual ~ResipAppDialogSetFactoryImpl();
  virtual resip::AppDialogSet* createAppDialogSet(resip::DialogUsageManager& dum, const resip::SipMessage& msg);
   // For a UAS the testAppDialogSet will be created by DUM using this function.  If you want to set 
   // Application Data, then one approach is to wait for onNewSession(ServerInviteSessionHandle ...) 
   // to be called, then use the ServerInviteSessionHandle to get at the AppDialogSet or AppDialog,
   // then cast to your derived class and set the desired application data.
  void setVariable(const std::string& name, const std::string& value);
  std::string getVariable(const std::string& name) const;
private:
  Variables _variables;
};

class ResipAppDialogSetFactory : public OSS::JS::JSObjectWrap
{
public:
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(setVariable);
  JS_METHOD_DECLARE(getVariable);
private:
  ResipAppDialogSetFactory();
  virtual ~ResipAppDialogSetFactory();
  std::auto_ptr<ResipAppDialogSetFactoryImpl> _self;
};

#endif //.RESIPAPPDIALOGSETFACTORY_H_INCLUDED

