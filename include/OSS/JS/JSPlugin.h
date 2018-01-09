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

#ifndef OSS_JSPLUGIN_H_INCLUDED
#define OSS_JSPLUGIN_H_INCLUDED

#include <v8.h>
#include <vector>
#include <Poco/ClassLoader.h>
#include <Poco/Manifest.h>
#include <Poco/ThreadPool.h>
#include <Poco/Runnable.h>
#include "OSS/JS/JSObjectWrap.h"
#include "OSS/JS/JSIsolateManager.h"
#include <iostream>

class JSPlugin
{
public:
  typedef Poco::Runnable Runnable;
  JSPlugin();
  virtual ~JSPlugin();
  virtual std::string name() const = 0;
  virtual bool initExportFunc(std::string& funcName) = 0;

  static Poco::ThreadPool* _pThreadPool;
};

inline JSPlugin::JSPlugin()
{
}

inline JSPlugin::~JSPlugin()
{
}

typedef Poco::ClassLoader<JSPlugin> JSPluginLoader;
typedef Poco::Manifest<JSPlugin> JSPluginManifest;

#define JS_REGISTER_MODULE(Class) \
class Class : public JSPlugin \
{ \
public: \
  Class(); \
  virtual std::string name() const; \
  virtual bool initExportFunc(std::string& funcName); \
}; \
Class::Class() {} \
std::string Class::name() const { return #Class; } \
bool Class::initExportFunc(std::string& funcName) \
{ \
  funcName = "__" #Class "_init_exports"; \
  js_get_global()->Set(v8::String::New(funcName.c_str()), v8::FunctionTemplate::New(init_exports)->GetFunction()); \
  return true; \
} \
extern "C" { \
  bool pocoBuildManifest(Poco::ManifestBase* pManifest_) { \
    typedef Poco::Manifest<JSPlugin> _Manifest; \
    std::string requiredType(typeid(_Manifest).name()); \
    std::string actualType(pManifest_->className()); \
    if (requiredType == actualType) { \
      Poco::Manifest<JSPlugin>* pManifest = static_cast<_Manifest*>(pManifest_); \
      pManifest->insert(new Poco::MetaObject<Class, JSPlugin>("oss_module")); \
      return true; \
    } \
    else { \
      return false; \
    } \
  } \
}


#include "OSS/JS/JS.h"

#endif // OSS_JSPLUGIN_H_INCLUDED

