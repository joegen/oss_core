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

#ifndef OSS_JSEVENTLOOP_H_INCLUDED
#define OSS_JSEVENTLOOP_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_V8

#include "v8.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSModule.h"
#include <OSS/UTL/BlockingQueue.h>

#include "OSS/JS/JSFileDescriptorManager.h"
#include "OSS/JS/JSEventQueueManager.h"
#include "OSS/JS/JSEventEmitter.h"
#include "OSS/JS/JSTaskManager.h"
#include "OSS/JS/JSFunctionCallbackQueue.h"
#include "OSS/JS/JSTimerManager.h"
#include "OSS/JS/JSWakeupPipe.h"
#include "OSS/JS/JSInterIsolateCallManager.h"


#include <queue>


namespace OSS {
namespace JS {

class JSIsolate;
  
class JSEventLoop : public JSWakeupPipe
{
public:
  typedef OSS::BlockingQueue<std::string> StringQueue;
  typedef boost::function<void(std::string message)> StringQueueCallback;
  typedef boost::promise<std::string> Promise;
  typedef boost::future<std::string> Future;
  typedef std::vector<pollfd> Descriptors;


  JSEventLoop(JSIsolate* pIsolate);
  ~JSEventLoop();
  
  void processEvents();
  void terminate();
  void join();
  
  JSIsolate* getIsolate();
  JSFileDescriptorManager& fdManager();
  JSEventQueueManager& queueManager();
  JSEventEmitter& eventEmitter();
  JSTaskManager& taskManager();
  JSFunctionCallbackQueue& functionCallback();
  JSTimerManager& timerManager();
  JSInterIsolateCallManager& interIsolate();
  
  bool isTerminated() const;
protected:
  bool _isTerminated;
  JSIsolate* _pIsolate;
  JSPersistentFunctionHandle _jsonParser;
  JSPersistentFunctionHandle _promiseHandler;
  JSPersistentObjectHandle _externalPointerTemplate;
  JSFileDescriptorManager _fdManager;
  JSEventQueueManager _queueManager;
  JSEventEmitter _eventEmitter;
  JSTaskManager _taskManager;
  JSFunctionCallbackQueue _functionCallback;
  JSTimerManager _timerManager;
  JSInterIsolateCallManager _interIsolate;
  OSS::UInt64  _garbageCollectionFrequency; 
};

//
// Inlines
//

inline JSFileDescriptorManager& JSEventLoop::fdManager()
{
  return _fdManager;
}

inline JSEventQueueManager& JSEventLoop::queueManager()
{
  return _queueManager;
}

inline JSEventEmitter& JSEventLoop::eventEmitter()
{
  return _eventEmitter;
}

inline JSTaskManager& JSEventLoop::taskManager()
{
  return _taskManager;
}

inline JSFunctionCallbackQueue& JSEventLoop::functionCallback()
{
  return _functionCallback;
}

inline JSTimerManager& JSEventLoop::timerManager()
{
  return _timerManager;
}

inline JSInterIsolateCallManager& JSEventLoop::interIsolate()
{
  return _interIsolate;
}

inline bool JSEventLoop::isTerminated() const
{
  return _isTerminated;
}


} } 

#endif  //ENABLE_FEATURE_V8
#endif // OSS_JSEVENTLOOP_H_INCLUDED

