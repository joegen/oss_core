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


#ifndef OSS_JSISOLATEMANAGER_H_INCLUDED
#define OSS_JSISOLATEMANAGER_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_V8

#include "v8.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/JSON/Json.h"
#include "OSS/UTL/Thread.h"
#include "OSS/JS/JSIsolate.h"


namespace OSS {
namespace JS {
  
class JSIsolateManager : boost::noncopyable
{
public:
  typedef std::map<std::string, JSIsolate::Ptr> MapByName;
  typedef std::map<pthread_t, JSIsolate::Ptr> MapByThreadId;
  
  static JSIsolateManager& instance();
  
  JSIsolate::Ptr createIsolate(pthread_t parentThreadId);
    /// Creates a new isolate.  Name must be unique
  
  
  JSIsolate::Ptr findIsolate(pthread_t threadId);
    /// Returns the isolate identified by an isolate thread
  
  JSIsolate::Ptr getIsolate();
    /// Returns the isolate for the current thread.
    /// Null if the calling thread is not an isolate thread
  
  void registerIsolate(JSIsolate::Ptr isolate);
    /// Called by the isolate run method to register the isolate and map it 
    /// with a specific thread identifier
  
  void run(const JSIsolate::Ptr& pIsolate, const boost::filesystem::path& script);
  
  bool hasIsolate(pthread_t threadId);
  
  JSIsolate::Ptr rootIsolate();
    /// Returns a pointer to the root isolate.   The root isolate
    /// is automatically created by the manahaer
private:
  JSIsolateManager();
  ~JSIsolateManager();
  
  OSS::mutex_critic_sec _mapMutex;
  MapByName _byName;
  MapByThreadId _byThreadId;
  JSIsolate::Ptr _rootIsolate;
};

//
// Inlines
//

inline JSIsolate::Ptr JSIsolateManager::getIsolate()
{
  return findIsolate(pthread_self());
}
  
} } // OSS::JS

#endif // ENABLE_FEATURE_V8
#endif // OSS_JSISOLATEMANAGER_H_INCLUDED