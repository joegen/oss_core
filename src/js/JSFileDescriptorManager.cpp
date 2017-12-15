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


#include "OSS/JS/JS.h"
#include "OSS/JS/JSIsolateManager.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/JSEventLoop.h"
#include "OSS/JS/JSIsolate.h"



namespace OSS {
namespace JS {

JSFileDescriptorManager::JSFileDescriptorManager(JSEventLoop* pEventLoop) :
  JSEventLoopComponent(pEventLoop)
{
}

JSFileDescriptorManager::~JSFileDescriptorManager()
{
}

void JSFileDescriptorManager::addFileDescriptor(int fd, v8::Handle<v8::Value> ioHandler, int events)
{
  OSS::mutex_critic_sec_lock lock(_descriptorsMutex);
  _descriptors[fd] = JSFileDescriptor::Ptr(new JSFileDescriptor(ioHandler, fd, events));
}

bool JSFileDescriptorManager::removeFileDescriptor(int fd)
{
  OSS::mutex_critic_sec_lock lock(_descriptorsMutex);
  if (_descriptors.find(fd) != _descriptors.end())
  {
    _descriptors.erase(fd);
    return true;
  }
  return false;
}

JSFileDescriptor::Ptr JSFileDescriptorManager::findDescriptor(int fd)
{
  OSS::mutex_critic_sec_lock lock(_descriptorsMutex);
  DescriptorMap::iterator iter = _descriptors.find(fd);
  if (iter != _descriptors.end())
  {
    return iter->second;
  }
  return JSFileDescriptor::Ptr();
}

void JSFileDescriptorManager::appendDescriptors(Descriptors& descriptors)
{
  OSS::mutex_critic_sec_lock lock(_descriptorsMutex);
  for (DescriptorMap::const_iterator iter = _descriptors.begin(); iter != _descriptors.end(); iter++)
  {
    descriptors.push_back(iter->second->pollFd());
  }
}

bool JSFileDescriptorManager::signalIO(pollfd pfd)
{
  _descriptorsMutex.lock();
  DescriptorMap::iterator iter = _descriptors.find(pfd.fd);
  if (iter != _descriptors.end())
  {
    JSFileDescriptor::Ptr pFD = iter->second;
    // unlock the mutex prior to calling signalIO for it may call our other 
    // functions that would attempt to lock the mutex resulting to a deadlock
    _descriptorsMutex.unlock(); 
    pFD->signalIO(pfd);
    return true;
  }
  _descriptorsMutex.unlock();
  return false;
}

JS_METHOD_IMPL(JSFileDescriptorManager::monitor_descriptor)
{
  js_enter_scope();
  js_method_declare_isolate(pIsolate);
  js_method_arg_declare_uint32(fd, 0);
  js_method_arg_assert_function(1);
  pIsolate->eventLoop()->fdManager().addFileDescriptor(fd, js_method_arg(1), POLLIN);
  pIsolate->eventLoop()->wakeup();
  return JSUndefined();
}

JS_METHOD_IMPL(JSFileDescriptorManager::unmonitor_descriptor)
{
  js_enter_scope();
  js_method_declare_isolate(pIsolate);
  js_method_arg_declare_uint32(fd, 0);
  js_method_arg_assert_function(1);
  pIsolate->eventLoop()->fdManager().removeFileDescriptor(fd);
  pIsolate->eventLoop()->wakeup();
  return JSUndefined();
}


} } // OSS::JS


