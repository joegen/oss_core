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
  createRootIsolate();
}  

JSIsolateManager::~JSIsolateManager()
{
}

JSIsolate::Ptr JSIsolateManager::createIsolate(const std::string& name)
{
  assert(!hasIsolate(name));
  
  OSS::mutex_critic_sec_lock lock(_mapMutex);
  JSIsolate::Ptr pIsolate = JSIsolate::Ptr(new JSIsolate(name));
  _byName[name] = pIsolate;
  return JSIsolate::Ptr();
}

void JSIsolateManager::destroyIsolate(const std::string& name)
{
  JSIsolate::Ptr pIsolate = findIsolate(name);
  if (pIsolate)
  {
    pIsolate->terminate();
    OSS::mutex_critic_sec_lock lock(_mapMutex);
    _byName.erase(name);
    _byThreadId.erase(pIsolate->getThreadId());
  }
}

JSIsolate::Ptr JSIsolateManager::findIsolate(const std::string& name)
{
  OSS::mutex_critic_sec_lock lock(_mapMutex);
  MapByName::iterator iter = _byName.find(name);
  if (iter != _byName.end())
  {
    return iter->second;
  }
  return JSIsolate::Ptr();
}

JSIsolate::Ptr JSIsolateManager::findIsolate(pthread_t threadId)
{
  OSS::mutex_critic_sec_lock lock(_mapMutex);
  MapByThreadId::iterator iter = _byThreadId.find(threadId);
  if (iter != _byThreadId.end())
  {
    return iter->second;
  }
  return JSIsolate::Ptr();
}

bool JSIsolateManager::hasIsolate(const std::string& name)
{
  OSS::mutex_critic_sec_lock lock(_mapMutex);
  MapByName::iterator iter = _byName.find(name);
  return (iter != _byName.end());
}

bool JSIsolateManager::hasIsolate(pthread_t threadId)
{
  OSS::mutex_critic_sec_lock lock(_mapMutex);
  MapByThreadId::iterator iter = _byThreadId.find(threadId);
  return (iter != _byThreadId.end());
}

void JSIsolateManager::createRootIsolate()
{
  JSIsolate::Ptr root = createIsolate("_root_");
  OSS::mutex_critic_sec_lock lock(_mapMutex);
}

void JSIsolateManager::registerIsolate(JSIsolate& isolate)
{
  JSIsolate::Ptr pIsolate = findIsolate(isolate.getName());
  if (!pIsolate)
  {
    return;
  }
  OSS::mutex_critic_sec_lock lock(_mapMutex);
  assert(pIsolate->getThreadId());
  _byThreadId[pIsolate->getThreadId()] = pIsolate;
}

JSIsolate::Ptr JSIsolateManager::rootIsolate()
{
  return findIsolate("_root_");
}

  
} } // OSS::JS


