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


#include <poll.h>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSUtil.h"
#include "OSS/JS/JSIsolate.h"
#include "OSS/JS/JSIsolateManager.h"
#include "OSS/JS/JSEventLoop.h"
#include "OSS/JS/modules/Async.h"
#include "OSS/JS/modules/QueueObject.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Net/Net.h"
#include "OSS/UTL/Semaphore.h"


typedef v8::Persistent<v8::Function> PersistentFunction;
typedef std::vector< v8::Persistent<v8::Value> > ArgumentVector;

static bool _enableAsync = false;
static OSS::Semaphore* _exitSem = 0;
static OSS::UInt64  _garbageCollectionFrequency = 30; /// 30 seconds default
pthread_t Async::_threadId = 0;
v8::Persistent<v8::ObjectTemplate> Async::_externalPointerTemplate;

static v8::Handle<v8::Value> __call(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() != 3 || !args[0]->IsFunction() || !args[1]->IsArray() || !args[2]->IsFunction())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  
  js_method_declare_isolate(pIsolate);
  pIsolate->eventLoop()->functionCallback().execute(args[0], args[1], args[2]);
  _enableAsync = true;
  return v8::Undefined();
}

static v8::Handle<v8::Value> __schedule_one_shot_timer(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 2 || !args[0]->IsFunction() || !args[1]->IsInt32())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  int expire = args[1]->ToInt32()->Value();
  
  js_method_declare_isolate(pIsolate);
  int timerId = 0;
  if (args.Length() >= 3)
  {
    timerId = pIsolate->eventLoop()->timerManager().scheduleTimer(expire, args[0], args[2]);
  }
  else
  {
    timerId = pIsolate->eventLoop()->timerManager().scheduleTimer(expire, args[0]);
  }
  _enableAsync = true;
  
  return v8::Int32::New(timerId);
}

static v8::Handle<v8::Value> __cancel_one_shot_timer(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsInt32())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  int32_t timerId = args[0]->ToInt32()->Value();
  Async::clear_timer(timerId);
  return v8::Undefined();
}

void Async::clear_timer(int timerId)
{
  OSS::JS::JSIsolate::Ptr pIsolate = OSS::JS::JSIsolateManager::instance().getIsolate();
  pIsolate->eventLoop()->timerManager().cancelTimer(timerId);
}

void Async::__wakeup_pipe(OSS::JS::JSIsolate* pIsolate)
{
  if (!pIsolate)
  {
    OSS::JS::JSIsolateManager::instance().getIsolate()->eventLoop()->wakeup();
  }
  else
  {
    pIsolate->eventLoop()->wakeup();
  }
}

static v8::Handle<v8::Value> __monitor_descriptor(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 2 || !args[0]->IsInt32() || !args[1]->IsFunction())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }

  _enableAsync = true;
  
  js_method_declare_isolate(pIsolate);
  OSS::JS::JSEventLoop* pEventLoop = pIsolate->eventLoop();
  pEventLoop->fdManager().addFileDescriptor(args[0]->ToInt32()->Value(), args[1], POLLIN);
  Async::__wakeup_pipe();
  return v8::Undefined();
}
void Async::unmonitor_fd(const OSS::JS::JSIsolate::Ptr& pIsolate, int fd)
{
  if (pIsolate->eventLoop()->fdManager().removeFileDescriptor(fd))
  {
    Async::__wakeup_pipe();
  }
}

static v8::Handle<v8::Value> __unmonitor_descriptor(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsInt32())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  js_method_declare_isolate(pIsolate);
  int fd = args[0]->ToInt32()->Value();
  Async::unmonitor_fd(pIsolate, fd);
  return v8::Undefined();
}

JS_METHOD_IMPL(__set_promise_callback)
{
  js_method_arg_declare_persistent_function(func, 0);
  OSS::JS::JSIsolate::getIsolate()->eventLoop()->interIsolate().setHandler(func);
  _enableAsync = true;
  return JSUndefined();
}

bool Async::json_execute_promise(OSS::JS::JSIsolate* pIsolate, const OSS::JSON::Object& request, OSS::JSON::Object& reply, uint32_t timeout, void* promiseData)
{
  return pIsolate->eventLoop()->interIsolate().execute(request, reply, timeout, promiseData);
}


static v8::Handle<v8::Value> __stop_event_loop(const v8::Arguments& args)
{
  v8::HandleScope scope;
  OSS::JS::JSIsolate::getIsolate()->eventLoop()->terminate();
  Async::__wakeup_pipe();
  return v8::Undefined();
}

static v8::Handle<v8::Value> __set_garbage_collection_frequency(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() == 0 || !args[0]->IsInt32())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  _garbageCollectionFrequency = args[0]->ToInt32()->Value();
  return v8::Undefined();
}

static v8::Handle<v8::Value> __process_events(const v8::Arguments& args)
{
  Async::_threadId = pthread_self();
  
  OSS::JS::JSIsolate::Ptr pIsolate = OSS::JS::JSIsolate::getIsolate();
  OSS::JS::JSEventLoop* pEventLoop = pIsolate->eventLoop();
  OSS::JS::JSEventQueueManager& queueManager = pEventLoop->queueManager();
  OSS::JS::JSInterIsolateCallManager& interIsolate = pEventLoop->interIsolate();
  
  if (pIsolate->getForceAsync() || queueManager.getSize() > 0 || interIsolate.isEnabled())
  {
    _enableAsync = true;
  }

  if (_enableAsync)
  {
    pEventLoop->processEvents();
  }
  
  if (_exitSem)
  {
    _exitSem->signal();
  }
  return v8::Undefined();
}

JS_EXPORTS_INIT()
{
  js_export_method("call", __call);
  js_export_method("processEvents", __process_events);
  js_export_method("setTimeout", __schedule_one_shot_timer);
  js_export_method("clearTimeout", __cancel_one_shot_timer);
  js_export_method("monitorFd", __monitor_descriptor);
  js_export_method("unmonitorFd", __unmonitor_descriptor);
  js_export_method("setGCFrequency", __set_garbage_collection_frequency);
  
  js_export_method("__stop_event_loop", __stop_event_loop);
  js_export_method("__set_promise_callback", __set_promise_callback);
  
  js_export_class(QueueObject);
  
  //
  // Create the template we use to wrap C++ pointers
  //
  v8::Handle<v8::ObjectTemplate> externalObjectTemplate = v8::ObjectTemplate::New();
  externalObjectTemplate->SetInternalFieldCount(1);
  Async::_externalPointerTemplate = v8::Persistent<v8::ObjectTemplate>::New(externalObjectTemplate);
  
  js_export_finalize();
}

JS_REGISTER_MODULE(JSAsync);

