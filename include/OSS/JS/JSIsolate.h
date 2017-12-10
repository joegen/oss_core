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
#include "OSS/JS/JSPersistentValue.h"


namespace OSS {
namespace JS {
  
class JSEventLoop;
class JSModule;
  
class JSIsolate
{
public:
  typedef boost::shared_ptr<JSIsolate> Ptr;
  typedef JSPersistentValue<v8::Context> Context;
  typedef JSPersistentValue<v8::ObjectTemplate> ObjectTemplate;
  typedef JSPersistentValue<v8::ObjectTemplate> Global;
  typedef JSPersistentValue<v8::ObjectTemplate> GlobalTemplate;
  
  ~JSIsolate();
    /// Destructor.  This is intentially made public so isolates can be used
    /// with shared_ptr
  
  
  bool call(const std::string& method, const OSS::JSON::Object& arguments, OSS::JSON::Object& reply, uint32_t timeout = 0, void* userData = 0);
  bool call(const OSS::JSON::Object& request, OSS::JSON::Object& reply, uint32_t timeout = 0, void* userData = 0);
  bool notify(const std::string& method, const OSS::JSON::Object& arguments, void* userData = 0);
  bool notify(const OSS::JSON::Object& request, void* userData = 0);
  void terminate();
  void setExitValue(int value);
  int getExitValue() const;
  JSModule* getModuleManager();
  
  bool isThreadSelf();
    /// returns true if the current thread is the isolate thread
  
  static JSIsolate::Ptr getIsolate();
    /// Returs the isolate for the current active isolate thread.
    /// If the current thread is not an isolate thread, an empty pointer
    /// will be returned
  
  const std::string& getName() const;
    /// Returns the given name for this isolate
  
  pthread_t getThreadId() const;
    /// Returns the thread identifier for this isolate
  
  bool isRoot() const;
    /// Returns true if this is the root isolate
  
  JSEventLoop* eventLoop();
    /// Returns a reference to the event loop
protected:
  JSIsolate(pthread_t parentThreadId);
    /// Creates a new isolate.  You MUST not create isolate directly.
  
  void setRoot();
  
  int run(const boost::filesystem::path& script);
    /// Run the script using this isolate
  
  Context _context;
  ObjectTemplate _objectTemplate;
  GlobalTemplate _globalTemplate;
  
  v8::Isolate* _pIsolate;
  JSModule* _pModuleManager;
  int _exitValue;
  pthread_t _threadId;
  pthread_t _parentThreadId;
  std::string _name;
  JSEventLoop* _pEventLoop;
  bool _isRoot;
  friend class JSIsolateManager;
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

inline const std::string& JSIsolate::getName() const
{
  return _name;
}

inline pthread_t JSIsolate::getThreadId() const
{
  return _threadId;
}

inline bool  JSIsolate::isRoot() const
{
  return _isRoot;
}

 inline void JSIsolate::setRoot() 
 {
   _isRoot = true;
 }
 


  
} } 

#endif  //ENABLE_FEATURE_V8
#endif // OSS_JSISOLATE_H_INCLUDED
