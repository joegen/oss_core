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

#ifndef OSS_ASYNC_H_INCLUDED
#define OSS_ASYNC_H_INCLUDED

#include <boost/thread.hpp>
#include <boost/thread/future.hpp>

#include <OSS/JS/JSPlugin.h>
#include <OSS/UTL/BlockingQueue.h>
#include <queue>
#include <pthread.h>
#include "OSS/JSON/Json.h"


typedef OSS::BlockingQueue<std::string> AsyncStringQueue;
typedef boost::function<void(std::string message)> AsyncStringQueueCallback;
typedef boost::function<void()> WakeupTask;
typedef std::queue<WakeupTask> WakeupTaskQueue;
typedef boost::promise<std::string> StringPromise;
typedef boost::future<std::string> AsyncFuture;

class AsyncPromise : public StringPromise
{
public:
  AsyncPromise(const std::string& data) : _data(data) {}
  std::string _data;
};

struct Async
{
  typedef std::queue<AsyncPromise*> PromiseQueue;
  static WakeupTaskQueue _wakeupTaskQueue;
  static OSS::mutex_critic_sec _wakeupTaskQueueMutex;
  static JSPersistentFunctionHandle _jsonParser;
  static JSPersistentFunctionHandle _promiseHandler;
  static pthread_t _threadId;
  static PromiseQueue _promises;
  static OSS::mutex_critic_sec* _promisesMutex;
  
  static void register_string_queue(AsyncStringQueue* pQueue, AsyncStringQueueCallback cb);
  static void unregister_string_queue(int fd);
  static void async_execute(const JSPersistentFunctionHandle& handle, const JSPersistentArgumentVector& args);
  static void unmonitor_fd(int fd);
  static void clear_timer(int timerId);
  static bool json_execute_promise(const OSS::JSON::Object& request, OSS::JSON::Object& reply, uint32_t timeout = 0);
  
  static void __wakeup_pipe();
  static void __insert_wakeup_task(const WakeupTask& task);
  static bool __do_one_wakeup_task();
  static bool __execute_one_promise();
  static JSValueHandle __json_parse(const std::string& json);
  
};


#endif // endif

