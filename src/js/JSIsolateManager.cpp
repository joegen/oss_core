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


#include <OSS/UTL/Thread.h>

#include "v8.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/JSON/Json.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/JSIsolateManager.h"


namespace OSS {
namespace JS {
  
//
// Static globals
//

JSIsolateManager& JSIsolateManager::instance()
{
  static JSIsolateManager manager;
  return manager;
}

JSIsolateManager::JSIsolateManager()
{
  //
  // Set the external heap to 20mb before attempting to garbage collect
  //
  v8::V8::AdjustAmountOfExternalAllocatedMemory(1024 * 1024 * 20);
  
  //
  // Create the root isolate
  //
  _rootIsolate = JSIsolate::Ptr(new JSIsolate(0));
  _rootIsolate->setRoot();
}  

JSIsolateManager::~JSIsolateManager()
{
}

JSIsolate::Ptr JSIsolateManager::createIsolate(pthread_t parentThreadId)
{
  assert(parentThreadId);
  JSIsolate::Ptr pIsolate = JSIsolate::Ptr(new JSIsolate(parentThreadId));
  return pIsolate;
}

JSIsolate::Ptr JSIsolateManager::findIsolate(pthread_t threadId)
{
  OSS::mutex_critic_sec_lock lock(_mapMutex);
  if (_rootIsolate->getThreadId() == threadId)
  {
    return _rootIsolate;
  }
  
  MapByThreadId::iterator iter = _byThreadId.find(threadId);
  if (iter != _byThreadId.end())
  {
    return iter->second;
  }
  return JSIsolate::Ptr();
}

bool JSIsolateManager::hasIsolate(pthread_t threadId)
{
  OSS::mutex_critic_sec_lock lock(_mapMutex);
  MapByThreadId::iterator iter = _byThreadId.find(threadId);
  return (iter != _byThreadId.end());
}

void JSIsolateManager::registerIsolate(JSIsolate::Ptr pIsolate)
{
  OSS::mutex_critic_sec_lock lock(_mapMutex);
  assert(pIsolate->getThreadId());
  _byThreadId[pIsolate->getThreadId()] = pIsolate;
}

JSIsolate::Ptr JSIsolateManager::rootIsolate()
{
  return _rootIsolate;
}

void JSIsolateManager::run(const JSIsolate::Ptr& pIsolate, const boost::filesystem::path& script)
{
  pIsolate->_threadId  = pthread_self();
  registerIsolate(pIsolate);
  pIsolate->run(script);
}

  
} } // OSS::JS


