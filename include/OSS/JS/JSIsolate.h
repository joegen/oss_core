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


#ifndef OSS_JSISOLATE_H_INCLUDED
#define OSS_JSISOLATE_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_V8

#include "v8.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSModule.h"


namespace OSS {
namespace JS {
  
//
// Singleton implementation for the Common JS Isolate
//
class JSIsolate : boost::noncopyable
{
public:
 
  static JSIsolate& instance();
    // Returns the instance of the root isolate


  int run(const boost::filesystem::path& script);
  bool call(const std::string& method, const OSS::JSON::Object& arguments, OSS::JSON::Object& reply, uint32_t timeout = 0, void* userData = 0);
  bool call(const OSS::JSON::Object& request, OSS::JSON::Object& reply, uint32_t timeout = 0, void* userData = 0);
  bool notify(const std::string& method, const OSS::JSON::Object& arguments, void* userData = 0);
  bool notify(const OSS::JSON::Object& request, void* userData = 0);
  void terminate();
  void setExitValue(int value);
  int getExitValue() const;
  JSModule& getModuleManager();
  bool isThreadSelf();
      // returns true if the current thread is the isolate thread
private:
  JSIsolate();
  ~JSIsolate();

  v8::Isolate* _pIsolate;
  JSModule _moduleManager;
  int _exitValue;
  pthread_t _threadId;
};
  
//
// Inlines
//

inline void JSIsolate::setExitValue(int value)
{
  _exitValue = value;
}

inline int JSIsolate::getExitValue() const
{
  return _exitValue;
}

inline JSModule& JSIsolate::getModuleManager()
{
  return _moduleManager;
}
  
} } 

#endif  //ENABLE_FEATURE_V8
#endif // OSS_JSISOLATE_H_INCLUDED

