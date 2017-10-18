#include "OSS/JS/JSPlugin.h"
#include <iostream>

class Logger : public JSPlugin
{
public:
  Logger();
  virtual ~Logger();
  virtual std::string name() const;
  virtual v8::Handle<v8::Value> exports(const v8::Arguments& args); 
};

Logger::Logger()
{
}

Logger::~Logger()
{
}

std::string Logger::name() const
{
  return "OSS Logger";
}

v8::Handle<v8::Value> Logger::exports(const v8::Arguments& args)
{
  return v8::Undefined();
}


extern "C"
{
  bool pocoBuildManifest(Poco::ManifestBase* pManifest_)
  {
    typedef Poco::Manifest<JSPlugin> _Manifest;
    std::string requiredType(typeid(_Manifest).name());
    std::string actualType(pManifest_->className());
    if (requiredType == actualType)
    {
      Poco::Manifest<JSPlugin>* pManifest = static_cast<_Manifest*>(pManifest_);
      pManifest->insert(new Poco::MetaObject<Logger, JSPlugin>("Logger"));
      return true;
    }
    else 
    {
      return false;
    }
  }

  void pocoInitializeLibrary()
  {
    std::cout << "Plugin initialized" << std::endl;
  }

  void pocoUninitializeLibrary()
  {
    std::cout << "Plugin uninitialized" << std::endl;
  }
}


