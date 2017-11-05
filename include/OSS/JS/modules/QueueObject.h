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

class QueueObject : public OSS::JS::ObjectWrap
{
public:
  typedef std::vector< v8::Persistent<v8::Value> > EventData;
  typedef v8::Persistent<v8::Function> EventCallback;
  typedef std::map<intptr_t, QueueObject*> ActiveQueues;
  
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

  QueueObject();
  virtual ~QueueObject();
  
  static v8::Persistent<v8::Function> _constructor;
  static void Init(v8::Handle<v8::Object> exports);
  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> enqueue(const v8::Arguments& args);
  
  EventQueue _queue;
  EventCallback _eventCallback;
};


#endif // OSS_QUEUEOBJECT_H_INCLUDED

