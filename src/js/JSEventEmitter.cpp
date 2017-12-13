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

#include "OSS/JS/JSEventLoop.h"
#include "OSS/JS/modules/QueueObject.h"
#include "OSS/JS/modules/Async.h"


namespace OSS {
namespace JS {


JSEventEmitter::JSEventEmitter(JSEventLoop* pEventLoop) :
  _pEventLoop(pEventLoop)
{
}

JSEventEmitter::~JSEventEmitter()
{
}

void JSEventEmitter::emit(const JSEventArgument& event)
{
  OSS::mutex_critic_sec_lock lock(_eventQueueMutex);
  _eventQueue.push(event);
  _pEventLoop->taskManager().queueTask(boost::bind(&JSEventEmitter::onEmitEvent, this, _1) ,0);
}

void JSEventEmitter::onEmitEvent(void* userData)
{
  _eventQueueMutex.lock();
  if (_eventQueue.empty())
  {
    _eventQueueMutex.unlock();
    return;
  }
  std::string json = _eventQueue.front().json();
  int fd = _eventQueue.front().getQueueFd();
  _eventQueue.pop();
  _eventQueueMutex.unlock();
  
  QueueObject::Event::Ptr pEvent = QueueObject::Event::Ptr(new QueueObject::Event());
  js_assign_persistent_arg_vector(pEvent->_eventData, v8::Local<v8::Value>::New(_pEventLoop->getIsolate()->parseJSON(json)));
  _pEventLoop->queueManager().enqueue(fd, pEvent);
}


} } // OSS::JS


