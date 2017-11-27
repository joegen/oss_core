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

#ifndef OSS_QUEUEOBJECT_H_INCLUDED
#define OSS_QUEUEOBJECT_H_INCLUDED


#include <v8.h>
#include <vector>
#include <OSS/JS/JSPlugin.h>
#include <OSS/UTL/BlockingQueue.h>
#include "OSS/JSON/Json.h"

class QueueObject : public OSS::JS::ObjectWrap
{
public:
  struct JsonEvent
  {
    int fd;
    std::string json;
  };
  typedef std::vector< v8::Persistent<v8::Value> > EventData;
  typedef v8::Persistent<v8::Function> EventCallback;
  typedef std::map<intptr_t, QueueObject*> ActiveQueues;
  typedef std::queue<JsonEvent> JsonQueue;
  
  class Event : public boost::enable_shared_from_this<Event>
  {
  public:
    typedef boost::shared_ptr<Event> Ptr;
    QueueObject::EventData _eventData;
    Event(QueueObject& queue) :
      _queue(queue)
    {
    }
    ~Event()
    {
      for (EventData::iterator iter = _eventData.begin(); iter != _eventData.end(); iter++)
      {
        iter->Dispose();
      }
    }
    QueueObject& _queue;
  };
  
  typedef OSS::BlockingQueue<Event::Ptr> EventQueue;
  
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(enqueue);
  JS_METHOD_DECLARE(getFd);
  
  static void json_enqueue_object(int fd, OSS::JSON::Object& object);
  static void json_enqueue(int fd, const std::string& json);
  static void on_json_dequeue();
  
  typedef std::map<int, QueueObject*> ActiveQueue;
  static OSS::mutex_critic_sec* _activeQueueMutex;
  static ActiveQueue _activeQueue;
  static OSS::mutex_critic_sec* _jsonQueueMutex;
  static JsonQueue _jsonQueue;
  
  EventQueue _queue;
  EventCallback _eventCallback;
  
private:
  QueueObject();
  virtual ~QueueObject();
  
};

#endif // OSS_QUEUEOBJECT_H_INCLUDED

