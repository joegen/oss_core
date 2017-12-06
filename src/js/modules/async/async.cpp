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
#include "OSS/JS/modules/Async.h"
#include "OSS/JS/modules/QueueObject.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Net/Net.h"
#include "OSS/UTL/Semaphore.h"


typedef v8::Persistent<v8::Function> PersistentFunction;
typedef std::vector< v8::Persistent<v8::Value> > ArgumentVector;

static bool _isTerminating = false;
static bool _enableAsync = false;
static OSS::Semaphore* _exitSem = 0;
static OSS::UInt64  _garbageCollectionFrequency = 30; /// 30 seconds default
pthread_t Async::_threadId = 0;
Async::PromiseQueue Async::_promises;
OSS::mutex* Async::_promisesMutex = new OSS::mutex();
v8::Persistent<v8::ObjectTemplate> Async::_externalPointerTemplate;

struct MonitoredFdData
{
  pollfd pfd;
  PersistentFunction *pCallback;
};
typedef std::map<int, MonitoredFdData> MonitoredFd;
static MonitoredFd _monitoredFd;


class FunctionCallInfo
{
public:
  typedef boost::shared_ptr<FunctionCallInfo> Ptr;
  PersistentFunction call;
  ArgumentVector args;
  PersistentFunction result;
  bool isDirectCall;
  FunctionCallInfo()
  {
    isDirectCall = false;
  }
  ~FunctionCallInfo()
  {
    if (!isDirectCall)
    {
      call.Dispose();
    }
    result.Dispose();
    for (ArgumentVector::iterator iter = args.begin(); iter != args.end(); iter++)
    {
      iter->Dispose();
    }
  }
};
typedef OSS::BlockingQueue<FunctionCallInfo::Ptr> CallQueue;
static CallQueue _callQueue(true);

class Timer : public boost::enable_shared_from_this<Timer>
{
public:
  typedef boost::shared_ptr<Timer> Ptr;
  typedef OSS::BlockingQueue<Timer::Ptr> TimerQueue;
  typedef std::map<int, Timer::Ptr> TimerMap;
  OSS::NET_TIMER_HANDLE timer;
  ArgumentVector timerArgs;
  PersistentFunction timerCallback;
  int identifier;
  Timer(int id, int expire) 
  {
    timer = OSS::net_io_timer_create(expire, boost::bind(&Timer::onTimer, this));
    identifier = id;
  }
  ~Timer()
  {
    timerCallback.Dispose();
  }
  
  void onTimer()
  {
    if (!_isTerminating)
    {
      queue.enqueue(shared_from_this());
    }
  }
  
  void cancel()
  {
     OSS::net_io_timer_cancel(timer);
  }
  static TimerQueue queue;
  static TimerMap timers;
  static int timerId;
};
Timer::TimerQueue Timer::queue(true);
Timer::TimerMap Timer::timers;
int Timer::timerId = 0;


static void handle_to_arg_vector(v8::Handle<v8::Value> input, ArgumentVector& output)
{
  if (input->IsArray())
  {
    v8::Handle<v8::Array> arrayArg = v8::Handle<v8::Array>::Cast(input);
    for (std::size_t i = 0; i <arrayArg->Length(); i++)
    {
      output.push_back(v8::Persistent<v8::Value>::New(arrayArg->Get(i)));
    }
  }
  else
  {
    output.push_back(v8::Persistent<v8::Value>::New(input));
  }
}

static v8::Handle<v8::Value> __call(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() != 3 || !args[0]->IsFunction() || !args[1]->IsArray() || !args[2]->IsFunction())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  
  FunctionCallInfo::Ptr pCallInfo(new FunctionCallInfo());
  
  pCallInfo->call = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(args[0]));
  handle_to_arg_vector(args[1], pCallInfo->args);
  pCallInfo->result = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(args[2]));
  
  _callQueue.enqueue(pCallInfo);
  
  _enableAsync = true;
  return v8::Undefined();
}

void Async::async_execute(const JSPersistentFunctionHandle& func, const JSPersistentArgumentVector& args)
{
  FunctionCallInfo::Ptr pCallInfo(new FunctionCallInfo());
  pCallInfo->isDirectCall = true;
  pCallInfo->call = func;
  pCallInfo->args = args;
  _enableAsync = true;
  _callQueue.enqueue(pCallInfo);
}

static v8::Handle<v8::Value> __schedule_one_shot_timer(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 2 || !args[0]->IsFunction() || !args[1]->IsInt32())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  int expire = args[1]->ToInt32()->Value();
  Timer::Ptr pTimer = Timer::Ptr(new Timer(++Timer::timerId, expire));
  pTimer->timerCallback = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(args[0]));
  
  if (args.Length() >= 3)
  {
    handle_to_arg_vector(args[2], pTimer->timerArgs);
  }
  Timer::timers[Timer::timerId] = pTimer;
  _enableAsync = true;
  return v8::Int32::New(Timer::timerId);
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
  Timer::TimerMap::iterator iter = Timer::timers.find(timerId);
  if (iter != Timer::timers.end())
  {
    iter->second->cancel();
    Timer::timers.erase(iter);
  }
}

static int _wakeupPipe[2];
WakeupTaskQueue Async::_wakeupTaskQueue;
OSS::mutex_critic_sec Async::_wakeupTaskQueueMutex;
void Async::__wakeup_pipe()
{
  std::size_t w = 0;
  w = write(_wakeupPipe[1], " ", 1);
  (void)w;
}

void Async::__insert_wakeup_task(const WakeupTask& task)
{
  OSS::mutex_critic_sec_lock lock(_wakeupTaskQueueMutex);
  Async::_wakeupTaskQueue.push(task);
  _enableAsync = true;
  Async::__wakeup_pipe();
}

bool Async::__do_one_wakeup_task()
{
  OSS::mutex_critic_sec_lock lock(Async::_wakeupTaskQueueMutex);
  if (Async::_wakeupTaskQueue.empty())
  {
    return false;
  }
  Async::_wakeupTaskQueue.front()();
  Async::_wakeupTaskQueue.pop();
  return true;
}


static v8::Handle<v8::Value> __monitor_descriptor(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 2 || !args[0]->IsInt32() || !args[1]->IsFunction())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }

  _enableAsync = true;
  
  MonitoredFdData data;
  data.pfd.fd = args[0]->ToInt32()->Value();
  data.pCallback = new PersistentFunction();
  *data.pCallback = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(args[1]));
  data.pfd.events = POLLIN;
  
  _monitoredFd[data.pfd.fd] = data;
  Async::__wakeup_pipe();
  return v8::Undefined();
}
void Async::unmonitor_fd(int fd)
{
  MonitoredFd::iterator iter = _monitoredFd.find(fd);
  if (iter != _monitoredFd.end())
  {
    iter->second.pCallback->Dispose();
    delete iter->second.pCallback;
    
    _monitoredFd.erase(iter);
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
  int fd = args[0]->ToInt32()->Value();
  Async::unmonitor_fd(fd);
  return v8::Undefined();
}

struct MonitoredStringQueueData
{
  AsyncStringQueue* queue;
  AsyncStringQueueCallback cb;
};

typedef std::map<int, MonitoredStringQueueData> MonitoredStringQueue;
static MonitoredStringQueue _monitoredStringQueue;

void Async::register_string_queue(AsyncStringQueue* pQueue, AsyncStringQueueCallback cb)
{
  MonitoredStringQueueData data;
  data.queue = pQueue;
  data.cb = cb;
  _monitoredStringQueue[pQueue->getFd()] = data;
  _enableAsync = true;
  Async::__wakeup_pipe();
}

void Async::unregister_string_queue(int fd)
{
  _monitoredStringQueue.erase(fd);
  Async::__wakeup_pipe();
}

JSPersistentFunctionHandle Async::_promiseHandler;

JS_METHOD_IMPL(__set_promise_callback)
{
  js_enter_scope();
  js_method_arg_declare_persistent_function(func, 0);
  Async::_promiseHandler = func;
  _enableAsync = true;
  return JSUndefined();
}

bool Async::__execute_one_promise()
{
  OSS::mutex_lock lock(*Async::_promisesMutex);
  if (Async::_promises.empty())
  {
    return false;
  }
  
  AsyncPromise* promise = Async::_promises.front();
  Async::_promises.pop();
  
  JSValueHandle request = Async::__json_parse(promise->_data);
  //v8::Handle<v8::Object> request_obj;
  //OSS::JS::wrap_external_object(v8::Context::GetCurrent(), &Async::_externalPointerTemplate, request_obj, promise->_userData);

  JSArgumentVector jsonArg;
  jsonArg.push_back(request);
  JSValueHandle result =  Async::_promiseHandler->Call(js_get_global(), jsonArg.size(), jsonArg.data());
  std::string theFuture;
  if (!result.IsEmpty() && result->IsString())
  {
    theFuture = js_handle_as_std_string(result);
  }
  promise->set_value(theFuture);
  return true;
}

bool Async::json_execute_promise(const OSS::JSON::Object& request, OSS::JSON::Object& reply, uint32_t timeout, void* promiseData)
{
  if (Async::_promiseHandler.IsEmpty())
  {
    return false;
  }
  std::string requestJson;
  if (!OSS::JSON::json_object_to_string(request, requestJson))
  {
    OSS_LOG_ERROR("Unable to execute promise. Parser error");
    return false;
  }
  
  AsyncPromise promise = (requestJson);
  promise._userData = promiseData;
  AsyncFuture future = promise.get_future();

  Async::_promisesMutex->lock();
  Async::_promises.push(&promise);
  Async::_promisesMutex->unlock();

  
  if (!OSS::JS::JSIsolate::instance().isThreadSelf())
  {
    Async::__wakeup_pipe();
  }
  else
  {
    Async::__execute_one_promise();
  }

  if (timeout != 0)
  {
    boost::system_time const future_timeout = boost::get_system_time() + boost::posix_time::milliseconds(timeout);
    if (!future.timed_wait_until(future_timeout))
    {
      return false;
    }
  }
  
  std::string replyJson = future.get();
  if (!OSS::JSON::json_parse_string(replyJson, reply))
  {
    return false;
  }
  return true;
}


static v8::Handle<v8::Value> __stop_event_loop(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (!_isTerminating)
  {
    _isTerminating = true;
    Async::__wakeup_pipe();
    //
    // Perform the rest of the cleanup here
    //
  }
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

JSPersistentFunctionHandle Async::_jsonParser;
JSValueHandle Async::__json_parse(const std::string& json)
{
  if (Async::_jsonParser.IsEmpty())
  {
    js_throw("JSON parser function is not set");
  }
  JSValueHandle val = JSString(json);
  JSArgumentVector jsonArg;
  jsonArg.push_back(val);
  return Async::_jsonParser->Call(js_get_global(), jsonArg.size(), jsonArg.data());
}

JS_METHOD_IMPL(__set_json_parser)
{
  js_enter_scope();
  js_method_arg_declare_persistent_function(func, 0);
  Async::_jsonParser = func;
  return JSUndefined();
}

JS_METHOD_IMPL(emit_json_string)
{
  js_enter_scope();
  js_method_arg_declare_uint32(fd, 0);
  js_method_arg_declare_string(json, 1);
  QueueObject::json_enqueue(fd, json);
  return JSUndefined();
}

static v8::Handle<v8::Value> __process_events(const v8::Arguments& args)
{
  v8::HandleScope scope;
  OSS::UInt64 lastGarbageCollectionTime = 0;
  Async::_threadId = pthread_self();
  
  if (QueueObject::_activeQueue.size() > 0)
  {
    _enableAsync = true;
  }

  std::vector<pollfd> descriptors;
  bool reconstructFdSet = true;
  
  //
  // Static Descriptors
  //
  pollfd pfds[3];
  pfds[0].fd = _wakeupPipe[0];
  pfds[0].events = POLLIN;
  pfds[0].revents = 0;
  pfds[1].fd = _callQueue.getFd();
  pfds[1].events = POLLIN;
  pfds[1].revents = 0;
  pfds[2].fd = Timer::queue.getFd();
  pfds[2].events = POLLIN;
  pfds[2].revents = 0;
  while (_enableAsync && !_isTerminating)
  {
    if (reconstructFdSet)
    {
      descriptors.clear();
      descriptors.reserve(QueueObject::_activeQueue.size() + 3);
      descriptors.push_back(pfds[0]);
      descriptors.push_back(pfds[1]);
      descriptors.push_back(pfds[2]);
      
      QueueObject::_activeQueueMutex->lock();
      for(QueueObject::QueueObject::ActiveQueue::iterator iter = QueueObject::_activeQueue.begin(); iter != QueueObject::_activeQueue.end(); iter++)
      {
        pollfd fd;
        fd.fd = iter->first;
        fd.events = POLLIN;
        fd.revents = 0;
        descriptors.push_back(fd);
      }
      QueueObject::_activeQueueMutex->unlock();

      for (MonitoredStringQueue::iterator iter = _monitoredStringQueue.begin(); iter != _monitoredStringQueue.end(); iter++)
      {
        pollfd fd;
        fd.fd = iter->first;
        fd.events = POLLIN;
        fd.revents = 0;
        descriptors.push_back(fd);
      }

      for(MonitoredFd::iterator iter = _monitoredFd.begin(); iter != _monitoredFd.end(); iter++)
      {
        pollfd fd;
        fd.fd = iter->second.pfd.fd;
        fd.events = iter->second.pfd.events;
        fd.revents = 0;
        descriptors.push_back(fd);
      }
    }
    int ret = ::poll(descriptors.data(), descriptors.size(), 100);
    v8::HandleScope scope;
    if (ret == -1)
    {
      break;
    }
    else if (ret == 0)
    {
      //
      // Use the timeout to let V8 perform internal garbage collection tasks
      //
      OSS::UInt64 now = OSS::getTime();
      if (now - lastGarbageCollectionTime > _garbageCollectionFrequency * 1000)
      {
        v8::V8::LowMemoryNotification();
        lastGarbageCollectionTime = now;
      }
      while(!v8::V8::IdleNotification());
      continue;
    }
    
    //
    // Perform garbage collection every 30 seconds
    //
    OSS::UInt64 now = OSS::getTime();
    if (now - lastGarbageCollectionTime > _garbageCollectionFrequency * 1000)
    {
      v8::V8::LowMemoryNotification();
      lastGarbageCollectionTime = now;
    }
    
    //
    // Check if C++ just wants to execute a task in the event loop
    //
    if (Async::__do_one_wakeup_task() || Async::__execute_one_promise())
    {
      //
      // Do not reconstruct fdset if we did some tasks
      //
      reconstructFdSet = false;
      continue;
    }
    
    //
    // From this point onwards, we only reconstruct the FD set if we are waken up via the wakeup pipe
    //
    reconstructFdSet = false;
    if (descriptors[0].revents & POLLIN)
    {
      
      std::size_t r = 0;
      char buf[1];
      r = read(_wakeupPipe[0], buf, 1);
      (void)r;
      if (_isTerminating)
      {
        break;
      }
      else
      {
        reconstructFdSet = true;
        continue;
      }
    }
    else if (descriptors[1].revents & POLLIN)
    {
      FunctionCallInfo::Ptr pCallInfo;
      _callQueue.dequeue(pCallInfo);
      if (pCallInfo)
      {
        if (pCallInfo->isDirectCall)
        {
          pCallInfo->call->Call(js_get_global(), pCallInfo->args.size(), pCallInfo->args.data());
        }
        else
        {
          v8::Handle<v8::Value> result = pCallInfo->call->Call(js_get_global(), pCallInfo->args.size(), pCallInfo->args.data());
          ArgumentVector resultArg;
          handle_to_arg_vector(result, resultArg);
          pCallInfo->result->Call(js_get_global(), resultArg.size(), resultArg.data());
        }
      }
    }
    else if (descriptors[2].revents & POLLIN)
    {
      Timer::Ptr pTimer;
      Timer::queue.dequeue(pTimer);
      if (pTimer)
      {
        Timer::timers.erase(pTimer->identifier);
        pTimer->timerCallback->Call(js_get_global(), pTimer->timerArgs.size(), pTimer->timerArgs.data());
      }
    }
    else
    {
      bool found = false;
      std::size_t fdsize = descriptors.size();

      for (std::size_t i = 3; i < fdsize; i++)
      {
        pollfd& pfd = descriptors[i];
        int revents = pfd.revents;
        if (revents & POLLIN)
        {
          QueueObject::_activeQueueMutex->lock();
          QueueObject::ActiveQueue::iterator iter = QueueObject::_activeQueue.find(pfd.fd);
          if (iter != QueueObject::_activeQueue.end())
          {
            QueueObject::Event::Ptr pEvent;
            iter->second->_queue.dequeue(pEvent);
            if (pEvent)
            {
              iter->second->_eventCallback->Call(js_get_global(), pEvent->_eventData.size(), pEvent->_eventData.data());
            }
            found = true;
          }
          QueueObject::_activeQueueMutex->unlock();
          
          if (!found)
          {
            MonitoredStringQueue::iterator iter = _monitoredStringQueue.find(pfd.fd);
            if (iter != _monitoredStringQueue.end())
            {
              std::string message;
              iter->second.queue->dequeue(message);
              if (!message.empty())
              {
                iter->second.cb(message);
              }
              found = true;
            }
          }
          
          if (!found)
          {
            int fd = pfd.fd;
            MonitoredFd::iterator iter = _monitoredFd.find(fd);
            if (iter != _monitoredFd.end())
            {
              found = true;
              std::vector< v8::Local<v8::Value> > args(2);
              args[0] = v8::Int32::New(fd);
              args[1] = v8::Int32::New(revents);
              (*iter->second.pCallback)->Call(js_get_global(), args.size(), args.data());
            }
          }
        }
        
        if (found)
        {
          break;
        }
      }
    }
  }
  if (_exitSem)
  {
    _exitSem->signal();
  }
  return v8::Undefined();
}

JS_EXPORTS_INIT()
{
  ::pipe(_wakeupPipe);

  js_export_method("call", __call);
  js_export_method("processEvents", __process_events);
  js_export_method("setTimeout", __schedule_one_shot_timer);
  js_export_method("clearTimeout", __cancel_one_shot_timer);
  js_export_method("monitorFd", __monitor_descriptor);
  js_export_method("unmonitorFd", __unmonitor_descriptor);
  js_export_method("setGCFrequency", __set_garbage_collection_frequency);
  js_export_method("emit_json_string", emit_json_string);
  
  js_export_method("__stop_event_loop", __stop_event_loop);
  js_export_method("__set_json_parser", __set_json_parser);
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

