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
  void* _userData;
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
  static OSS::mutex* _promisesMutex;
  static v8::Persistent<v8::ObjectTemplate> _externalPointerTemplate;
  
  static void register_string_queue(AsyncStringQueue* pQueue, AsyncStringQueueCallback cb);
    // This is a utility function that allows the C++ side to receive event strings from ossjs plugins
  
  static void unregister_string_queue(int fd);
    // Removes the C++ string queue
  
  static void unmonitor_fd(int fd);
    // Forcibly unmonitor a particular file handle.  Use with extreme caution.
  
  static void clear_timer(int timerId);
    // Cancel a running timer
  
  static bool json_execute_promise(const OSS::JSON::Object& request, OSS::JSON::Object& reply, uint32_t timeout = 0, void* promiseData = 0);
    // This method is called from the C++ side to execute an RPC style function call towards ossjs.
    // The request object must have a (1) method property containing the name of the callback function
    // (2) and an argument property containing any structure.  The promiseData parameter may 
    // contain a pointer to a structure which can be accessed by C++ plugins.
    // This function is perfectly thread safe. 
    // CAVEAT: timeout will be ignored if promiseData is not null.  This makes sure that the js side
    // will not end up with an invalid pointer if it was not done processing the data yet

  static void async_execute(const JSPersistentFunctionHandle& handle, const JSPersistentArgumentVector& args);
    // Execute a persistent function object within the event loop.  This is another
    // way to call ossjs functions from the C++ side.  The C++ should own a persistent handle to both
    // the function and the arguments.  
    // There is no data returned by the JS side.
    // This function will not block 
  
  //
  // Methods intended to be called within the event loop and are not safe 
  // to be called directly from another thread.  The se function would be prefix with __
  //
  
  static bool __execute_one_promise();
    // Execute a promise queued by json_execute_promise 
  
  static void __wakeup_pipe();
    // Wakes up the event loop and reset the poll fd set
  
  static void __insert_wakeup_task(const WakeupTask& task);
    // Wakes up the event loop and perform a specific task
  
  static bool __do_one_wakeup_task();
    // Called by the event loop to execute a task queued by __insert_wakeup_task
  
  static JSValueHandle __json_parse(const std::string& json);
    // Helper function to convert a JSON string to a v8 handle
  
  
  
};


#endif // endif

