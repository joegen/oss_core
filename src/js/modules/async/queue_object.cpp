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

#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/modules/Async.h"
#include "OSS/JS/modules/QueueObject.h"
#include "OSS/JS/JSEventLoop.h"
#include "OSS/JS/JSIsolate.h"
#include "OSS/JS/JSIsolateManager.h"


using OSS::JS::JSObjectWrap;


OSS::mutex_critic_sec* QueueObject::_jsonQueueMutex = new OSS::mutex_critic_sec();
QueueObject::JsonQueue QueueObject::_jsonQueue;

JS_CLASS_INTERFACE(QueueObject, "Queue") 
{
  JS_CLASS_METHOD_DEFINE(QueueObject, "enqueue", enqueue);
  JS_CLASS_METHOD_DEFINE(QueueObject, "getFd", getFd);
  JS_CLASS_INTERFACE_END(QueueObject); 
}

QueueObject::QueueObject() :
  _queue(true)
{
  getIsolate()->eventLoop()->queueManager().addQueue(this);
  Async::__wakeup_pipe();
}

QueueObject::~QueueObject()
{
  _eventCallback.Dispose();
  getIsolate()->eventLoop()->queueManager().removeQueue(this);
  Async::__wakeup_pipe();
}

JS_CONSTRUCTOR_IMPL(QueueObject)
{
  js_enter_scope();
  js_method_arg_declare_persistent_function(handler, 0);
  QueueObject* pQueue = new QueueObject();
  pQueue->_eventCallback = handler;
  pQueue->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(QueueObject::enqueue)
{
  js_enter_scope();
  js_method_arg_declare_array(items, 0);
  js_method_arg_declare_self(QueueObject, pQueue);
  Event::Ptr pEvent = Event::Ptr(new QueueObject::Event());
  js_assign_persistent_arg_vector(pEvent->_eventData, _args_[0]);
  pQueue->_queue.enqueue(pEvent);
  return JSUndefined();
}

JS_METHOD_IMPL(QueueObject::getFd)
{
  js_enter_scope();
  js_method_arg_declare_self(QueueObject, self);
  return JSInt32(self->_queue.getFd());
}


  
void QueueObject::json_enqueue(OSS::JS::JSIsolate* pIsolate, int fd, const std::string& json)
{
  _jsonQueueMutex->lock();
  JsonEvent event;
  event.fd = fd;
  event.json = json;
  _jsonQueue.push(event);
  _jsonQueueMutex->unlock();
  Async::__insert_wakeup_task(pIsolate, boost::bind(&QueueObject::on_json_dequeue));
}

void QueueObject::json_enqueue_object(OSS::JS::JSIsolate* pIsolate, int fd, const OSS::JSON::Array& object)
{
  std::string json;
  if (OSS::JSON::json_to_string<OSS::JSON::Array>(object, json))
  {
    QueueObject::json_enqueue(pIsolate, fd, json);
  }
}

//
// This method is called from the event loop thread
//
void QueueObject::on_json_dequeue()
{
  JsonEvent event;
  _jsonQueueMutex->lock();
  if (_jsonQueue.empty())
  {
     _jsonQueueMutex->unlock();
     return;
  }
  event = _jsonQueue.front();
  _jsonQueue.pop();
  _jsonQueueMutex->unlock();
  
  Event::Ptr pEvent = Event::Ptr(new QueueObject::Event());
  js_assign_persistent_arg_vector(pEvent->_eventData, v8::Local<v8::Value>::New(Async::__json_parse(event.json)));
  OSS::JS::JSIsolateManager::instance().getIsolate()->eventLoop()->queueManager().enqueue(event.fd, pEvent);

}