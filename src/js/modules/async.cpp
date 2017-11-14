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
#include "OSS/JS/modules/Async.h"
#include "OSS/JS/modules/QueueObject.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Net/Net.h"
#include "OSS/UTL/Semaphore.h"


typedef v8::Persistent<v8::Function> PersistentFunction;
typedef std::vector< v8::Persistent<v8::Value> > ArgumentVector;

typedef std::map<int, QueueObject*> ActiveQueue;
static ActiveQueue _activeQueue;
static bool _isTerminating = false;
static OSS::Semaphore* _exitSem = 0;
static OSS::UInt64  _garbageCollectionFrequency = 30; /// 30 seconds default

struct MonitoredFdData
{
  pollfd pfd;
  PersistentFunction *pCallback;
};
typedef std::map<int, MonitoredFdData> MonitoredFd;
static MonitoredFd _monitoredFd;


class BlockingCall
{
public:
  typedef boost::shared_ptr<BlockingCall> Ptr;
  PersistentFunction call;
  ArgumentVector args;
  PersistentFunction result;
  BlockingCall()
  {
  }
  ~BlockingCall()
  {
    call.Dispose();
    result.Dispose();
    for (ArgumentVector::iterator iter = args.begin(); iter != args.end(); iter++)
    {
      iter->Dispose();
    }
  }
};
typedef OSS::BlockingQueue<BlockingCall::Ptr> CallQueue;
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
    queue.enqueue(shared_from_this());
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
  
  BlockingCall::Ptr pCallInfo(new BlockingCall());
  
  pCallInfo->call = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(args[0]));
  handle_to_arg_vector(args[1], pCallInfo->args);
  pCallInfo->result = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(args[2]));
  
  _callQueue.enqueue(pCallInfo);
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
  Timer::Ptr pTimer = Timer::Ptr(new Timer(++Timer::timerId, expire));
  pTimer->timerCallback = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(args[0]));
  
  if (args.Length() >= 3)
  {
    handle_to_arg_vector(args[2], pTimer->timerArgs);
  }
  Timer::timers[Timer::timerId] = pTimer;
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
  Timer::TimerMap::iterator iter = Timer::timers.find(timerId);
  if (iter != Timer::timers.end())
  {
    iter->second->cancel();
    Timer::timers.erase(iter);
  }
  return v8::Undefined();
}

v8::Persistent<v8::Function> QueueObject::_constructor;
static int _wakeupPipe[2];

static void __wakeup_pipe()
{
  std::size_t w = 0;
  w = write(_wakeupPipe[1], " ", 1);
  (void)w;
}

QueueObject::QueueObject() :
  _queue(true)
{
  _activeQueue[_queue.getFd()] = this;
  __wakeup_pipe();
}

QueueObject::~QueueObject()
{
  _eventCallback.Dispose();
  _activeQueue.erase(_queue.getFd());
  __wakeup_pipe();
}

v8::Handle<v8::Value> QueueObject::New(const v8::Arguments& args)
{
  if (args.Length() != 1 || !args[0]->IsFunction())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  
  v8::HandleScope scope; 
  QueueObject* pQueue = new QueueObject();
  pQueue->_eventCallback = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(args[0]));
  pQueue->Wrap(args.This());
  return args.This();
}

v8::Handle<v8::Value> QueueObject::enqueue(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsArray())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument.  Usage: enqueue(Array)")));
  }
  QueueObject* pQueue = ObjectWrap::Unwrap<QueueObject>(args.This());
  Event::Ptr pEvent = Event::Ptr(new QueueObject::Event(*pQueue));
  handle_to_arg_vector(args[0], pEvent->_eventData);
  pQueue->_queue.enqueue(pEvent);
  return v8::Undefined();
}

static v8::Handle<v8::Value> __monitor_descriptor(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 2 || !args[0]->IsInt32() || !args[1]->IsFunction())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }

  MonitoredFdData data;
  data.pfd.fd = args[0]->ToInt32()->Value();
  data.pCallback = new PersistentFunction();
  *data.pCallback = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(args[1]));
  data.pfd.events = POLLIN;
  
  _monitoredFd[data.pfd.fd] = data;
  __wakeup_pipe();
  return v8::Undefined();
}

static v8::Handle<v8::Value> __unmonitor_descriptor(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsInt32())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  int fd = args[0]->ToInt32()->Value();
  MonitoredFd::iterator iter = _monitoredFd.find(fd);
  if (iter != _monitoredFd.end())
  {
    iter->second.pCallback->Dispose();
    delete iter->second.pCallback;
    
    _monitoredFd.erase(iter);
    __wakeup_pipe();
  }
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
  __wakeup_pipe();
}

void Async::unregister_string_queue(int fd)
{
  _monitoredStringQueue.erase(fd);
  __wakeup_pipe();
}


static v8::Handle<v8::Value> __process_events(const v8::Arguments& args)
{
  v8::HandleScope scope;
  OSS::UInt64 lastGarbageCollectionTime = 0;
  while (!_isTerminating)
  {
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
  
    std::vector<pollfd> descriptors;
    descriptors.reserve(_activeQueue.size() + 3);
    descriptors.push_back(pfds[0]);
    descriptors.push_back(pfds[1]);
    descriptors.push_back(pfds[2]);
    
    for(ActiveQueue::iterator iter = _activeQueue.begin(); iter != _activeQueue.end(); iter++)
    {
      pollfd fd;
      fd.fd = iter->first;
      fd.events = POLLIN;
      fd.revents = 0;
      descriptors.push_back(fd);
    }
    
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
    
    int ret = ::poll(descriptors.data(), descriptors.size(), 100);
    
    //
    // Lock the isolate
    //
    v8::Locker lock;
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
        continue;
      }
    }
    else if (descriptors[1].revents & POLLIN)
    {
      BlockingCall::Ptr pCallInfo;
      _callQueue.dequeue(pCallInfo);
      if (pCallInfo)
      {
        v8::Handle<v8::Value> result = pCallInfo->call->Call((*JSPlugin::_pContext)->Global(), pCallInfo->args.size(), pCallInfo->args.data());
        ArgumentVector resultArg;
        handle_to_arg_vector(result, resultArg);
        pCallInfo->result->Call((*JSPlugin::_pContext)->Global(), resultArg.size(), resultArg.data());
      }
    }
    else if (descriptors[2].revents & POLLIN)
    {
      Timer::Ptr pTimer;
      Timer::queue.dequeue(pTimer);
      if (pTimer)
      {
        Timer::timers.erase(pTimer->identifier);
        pTimer->timerCallback->Call((*JSPlugin::_pContext)->Global(), pTimer->timerArgs.size(), pTimer->timerArgs.data());
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
          ActiveQueue::iterator iter = _activeQueue.find(pfd.fd);
          if (iter != _activeQueue.end())
          {
            QueueObject::Event::Ptr pEvent;
            iter->second->_queue.dequeue(pEvent);
            if (pEvent)
            {
              iter->second->_eventCallback->Call((*JSPlugin::_pContext)->Global(), pEvent->_eventData.size(), pEvent->_eventData.data());
            }
            found = true;
          }
          
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
              (*iter->second.pCallback)->Call((*JSPlugin::_pContext)->Global(), args.size(), args.data());
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
  _exitSem->signal();
  return v8::Undefined();
}

void QueueObject::Init(v8::Handle<v8::Object> exports)
{

  v8::HandleScope scope; 
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(QueueObject::New);
  tpl->SetClassName(v8::String::NewSymbol("Queue"));
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("ObjectType"), v8::String::NewSymbol("Queue"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("enqueue"), v8::FunctionTemplate::New(QueueObject::enqueue)->GetFunction());

  
  QueueObject::_constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
  exports->Set(v8::String::NewSymbol("Queue"), QueueObject::_constructor);
  //
  // Expose as a global object
  //
  (*JSPlugin::_pContext)->Global()->Set(v8::String::NewSymbol("Queue"), QueueObject::_constructor);
  
}

static v8::Handle<v8::Value> __stop_event_loop(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (!_isTerminating)
  {
    _isTerminating = true;
    __wakeup_pipe();
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

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  ::pipe(_wakeupPipe);
  
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  exports->Set(v8::String::New("call"), v8::FunctionTemplate::New(__call)->GetFunction());
  exports->Set(v8::String::New("processEvents"), v8::FunctionTemplate::New(__process_events)->GetFunction());
  exports->Set(v8::String::New("setTimeout"), v8::FunctionTemplate::New(__schedule_one_shot_timer)->GetFunction());
  exports->Set(v8::String::New("clearTimeout"), v8::FunctionTemplate::New(__cancel_one_shot_timer)->GetFunction());
  exports->Set(v8::String::New("monitorFd"), v8::FunctionTemplate::New(__monitor_descriptor)->GetFunction());
  exports->Set(v8::String::New("unmonitorFd"), v8::FunctionTemplate::New(__unmonitor_descriptor)->GetFunction());
  exports->Set(v8::String::New("__stop_event_loop"), v8::FunctionTemplate::New(__stop_event_loop)->GetFunction());
  exports->Set(v8::String::New("setGCFrequency"), v8::FunctionTemplate::New(__set_garbage_collection_frequency)->GetFunction());
  
  QueueObject::Init(exports);
  
  return exports;
}

JS_REGISTER_MODULE(JSAsync);

