#include "OSS/JS/modules/ResipAppDialogSetFactory.h"
#include "OSS/JS/modules/ResipDialogUsageManager.h"

using namespace resip;

class ResipAppDialog : public AppDialog
{
public:
   ResipAppDialog(HandleManager& ham, Data &SampleAppData) : AppDialog(ham), mSampleAppData(SampleAppData)
   {  
      std::cout << mSampleAppData << ": ResipAppDialog: created." << std::endl;  
   }
   virtual ~ResipAppDialog() 
   { 
      std::cout << mSampleAppData << ": ResipAppDialog: destroyed." << std::endl; 
   }
   Data mSampleAppData;
};

class ResipAppDialogSet : public AppDialogSet
{
public:
   ResipAppDialogSet(DialogUsageManager& dum, Data SampleAppData) : AppDialogSet(dum), mSampleAppData(SampleAppData)
   {  
      std::cout << mSampleAppData << ": ResipAppDialogSet: created." << std::endl;  
   }
   virtual ~ResipAppDialogSet() 
   {  
      std::cout << mSampleAppData << ": ResipAppDialogSet: destroyed." << std::endl;  
   }
   virtual AppDialog* createAppDialog(const SipMessage& msg) 
   {  
      return new ResipAppDialog(mDum, mSampleAppData);  
   }
   virtual SharedPtr<UserProfile> selectUASUserProfile(const SipMessage& msg) 
   { 
      std::cout << mSampleAppData << ": ResipAppDialogSet: UAS UserProfile requested for msg: " << msg.brief() << std::endl;  
      return mDum.getMasterUserProfile(); 
   }
   Data mSampleAppData;
};

ResipAppDialogSetFactoryImpl::ResipAppDialogSetFactoryImpl()
{
}

ResipAppDialogSetFactoryImpl::~ResipAppDialogSetFactoryImpl()
{
}

void ResipAppDialogSetFactoryImpl::setVariable(const std::string& name, const std::string& value)
{
  _variables[name] = value;
}

std::string ResipAppDialogSetFactoryImpl::getVariable(const std::string& name) const
{
  Variables::const_iterator iter = _variables.find(name);
  if ( iter != _variables.end())
  {
    return iter->second;
  }
  return std::string();
}
  
AppDialogSet* ResipAppDialogSetFactoryImpl::createAppDialogSet(DialogUsageManager& dum, const SipMessage& msg) 
{  
  return new ResipAppDialogSet(dum, Data("UAS") + Data("(") + getMethodName(msg.header(h_RequestLine).getMethod()) + Data(")"));  
}

JS_CLASS_INTERFACE(ResipAppDialogSetFactory, "AppDialogSetFactory") 
{  
  JS_CLASS_METHOD_DEFINE(ResipAppDialogSetFactory, "setVariable", setVariable);
  JS_CLASS_METHOD_DEFINE(ResipAppDialogSetFactory, "getVariable", getVariable);
  JS_CLASS_INTERFACE_END(ResipAppDialogSetFactory); 
}

ResipAppDialogSetFactory::ResipAppDialogSetFactory()
{
}

ResipAppDialogSetFactory::~ResipAppDialogSetFactory()
{
}

JS_CONSTRUCTOR_IMPL(ResipAppDialogSetFactory)
{
  ResipAppDialogSetFactory* object = new ResipAppDialogSetFactory();
  object->_self = std::auto_ptr<ResipAppDialogSetFactoryImpl>(new ResipAppDialogSetFactoryImpl());
  object->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(ResipAppDialogSetFactory::setVariable)
{
  js_method_arg_declare_self(ResipAppDialogSetFactory, self);
  js_method_arg_declare_string(name, 0);
  js_method_arg_declare_string(value, 1);
  self->_self->setVariable(name, value);
  return JSUndefined();
}

JS_METHOD_IMPL(ResipAppDialogSetFactory::getVariable)
{
  js_method_arg_declare_self(ResipAppDialogSetFactory, self);
  js_method_arg_declare_string(name, 0);
  std::string value = self->_self->getVariable(name);
  return JSString(value.c_str());
}
