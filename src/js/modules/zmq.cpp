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

#include <list>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/modules/ZMQSocketObject.h"
#include "OSS/JS/modules/BufferObject.h"
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/Semaphore.h"

using OSS::JS::ObjectWrap;
v8::Persistent<v8::Function> ZMQSocketObject::_constructor;
boost::thread* ZMQSocketObject::_pPollThread;
typedef std::list<ZMQSocketObject*> Sockets;
static Sockets _socketList;
static OSS::mutex_critic_sec _socketListMutex;
static ZMQSocketObject* _notifyReceiver = 0;
static ZMQSocketObject* _notifySender = 0;
typedef OSS::ZMQ::ZMQSocket::PollItems PollItems;
static bool _wakeupPipeEnabled = false;
static zmq::context_t* _context = 0;
static OSS::Semaphore* _notifySem;
static bool _isTerminating = false;


static void __wakeup_pipe()
{
  if (_wakeupPipeEnabled)
  {
    std::string buf(" ");
    _notifySender->getSocket()->sendRequest(buf);
  }
}

void ZMQSocketObject::notifyReadable()
{
  std::size_t w = 0;
  w = write(_pipe[1], " ", 1);
  (void)w;
  _notifySem->wait();
}

void ZMQSocketObject::clearReadable()
{
  std::size_t r = 0;
  char buf[1];
  r = read(_pipe[0], buf, 1);
  (void)r;
  _notifySem->signal();
}

void ZMQSocketObject::pollForReadEvents()
{
   _wakeupPipeEnabled = true;
  while (!_isTerminating)
  {
    PollItems items;
    zmq_pollitem_t notifier;
    _notifyReceiver->getSocket()->initPollItem(notifier);
    items.push_back(notifier);
    {
      OSS::mutex_critic_sec_lock lock(_socketListMutex);
      for (Sockets::iterator iter = _socketList.begin(); iter != _socketList.end(); iter++)
      {
        zmq_pollitem_t item;
        (*iter)->getSocket()->initPollItem(item);
        items.push_back(item); 
      }
    }

    int rc = OSS::ZMQ::ZMQSocket::poll(items, -1);

    if (_isTerminating)
    {
      break;
    }
    else if (rc < 0) 
    {
      if (zmq_errno() == EINTR) 
      {
        continue;
      } 
      else 
      {
        break;
      }
    }

    for (PollItems::iterator iter = items.begin(); iter != items.end(); iter++)
    {
      zmq_pollitem_t& pfd = *iter;
      if (pfd.revents & ZMQ_POLLIN)
      {
        if (pfd.socket == _notifyReceiver->getSocket()->socket()->get())
        {
          std::string buf;
          assert(_notifyReceiver->getSocket()->receiveReply(buf));
        }
        else
        {
          bool found = false;
          {
            OSS::mutex_critic_sec_lock lock(_socketListMutex);
            for (Sockets::iterator iter = _socketList.begin(); iter != _socketList.end(); iter++)
            {
              if (pfd.socket == (*iter)->getSocket()->socket()->get())
              {
                (*iter)->notifyReadable();
                found = true;
                break;
              }
            }
          }

          if (found)
          {
            break;
          }
        }
        break;
      }
    }
  }
}

ZMQSocketObject::ZMQSocketObject(Socket::SocketType type) :
  _pSocket(0)
{
  _pSocket = new Socket(type, _context);
  ::pipe(_pipe);
}

ZMQSocketObject::~ZMQSocketObject()
{
  delete _pSocket;
  OSS::mutex_critic_sec_lock lock(_socketListMutex);
  _socketList.remove(this);
  __wakeup_pipe();
}


v8::Handle<v8::Value> ZMQSocketObject::New(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsInt32())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument.  Must provide socket type.")));
  }
  ZMQSocketObject* pSocket = new ZMQSocketObject((Socket::SocketType)args[0]->ToInt32()->Value());
  pSocket->Wrap(args.This());
  
  return args.This();
}

static bool __get_string_arg(const v8::Arguments& args, std::string& value)
{
  v8::HandleScope scope;
  if (args.Length() == 0 || !args[0]->IsString())
  {
    return false;
  }
  value = *v8::String::Utf8Value(args[0]);
  return true;
}

static bool __get_buffer_arg(const v8::Arguments& args, std::string& value)
{
  v8::HandleScope scope;
  if (args.Length() == 0 || !BufferObject::isBuffer(args[0]))
  {
    return false;
  }
  BufferObject* pBuffer = ObjectWrap::Unwrap<BufferObject>(args[0]->ToObject());
  std::copy(pBuffer->buffer().begin(), pBuffer->buffer().end(), std::back_inserter(value));
  return true;
}

v8::Handle<v8::Value> ZMQSocketObject::connect(const v8::Arguments& args)
{
  v8::HandleScope scope;
  ZMQSocketObject* pObject = ObjectWrap::Unwrap<ZMQSocketObject>(args.This());
  std::string arg;
  if (!__get_string_arg(args, arg))
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
   
  bool ret = pObject->_pSocket->connect(arg);
  if (ret)
  {
    OSS::mutex_critic_sec_lock lock(_socketListMutex);
    _socketList.push_back(pObject);
    __wakeup_pipe();
  }
  return v8::Boolean::New(ret);
}

v8::Handle<v8::Value> ZMQSocketObject::bind(const v8::Arguments& args)
{
  v8::HandleScope scope;
  ZMQSocketObject* pObject = ObjectWrap::Unwrap<ZMQSocketObject>(args.This());
  std::string arg;
  if (!__get_string_arg(args, arg))
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  bool ret = pObject->_pSocket->bind(arg);
  if (ret)
  {
    OSS::mutex_critic_sec_lock lock(_socketListMutex);
    _socketList.push_back(pObject);
    __wakeup_pipe();
  }
  return v8::Boolean::New(ret);
}

v8::Handle<v8::Value> ZMQSocketObject::subscribe(const v8::Arguments& args)
{
  v8::HandleScope scope;
  ZMQSocketObject* pObject = ObjectWrap::Unwrap<ZMQSocketObject>(args.This());
  std::string arg;
  if (!__get_string_arg(args, arg) && !__get_buffer_arg(args, arg))
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  return v8::Boolean::New(pObject->_pSocket->subscribe(arg));
}

v8::Handle<v8::Value> ZMQSocketObject::publish(const v8::Arguments& args)
{
  v8::HandleScope scope;
  ZMQSocketObject* pObject = ObjectWrap::Unwrap<ZMQSocketObject>(args.This());
  std::string arg;
  if (!__get_string_arg(args, arg) && !__get_buffer_arg(args, arg))
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  return v8::Boolean::New(pObject->_pSocket->publish(arg));
}

v8::Handle<v8::Value> ZMQSocketObject::send(const v8::Arguments& args)
{
  v8::HandleScope scope;
  ZMQSocketObject* pObject = ObjectWrap::Unwrap<ZMQSocketObject>(args.This());
  std::string arg;
  if (!__get_string_arg(args, arg) && !__get_buffer_arg(args, arg))
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  return v8::Boolean::New(pObject->_pSocket->sendRequest(arg));
}

v8::Handle<v8::Value> ZMQSocketObject::receive(const v8::Arguments& args)
{
  v8::HandleScope scope;
  ZMQSocketObject* pObject = ObjectWrap::Unwrap<ZMQSocketObject>(args.This());
  std::string msg;
  v8::Handle<v8::Value> result = v8::Undefined();
  if (pObject->_pSocket->receiveReply(msg, 0))
  {
    BufferObject* pBuffer = 0;
    if (args.Length() == 1 && BufferObject::isBuffer(args[0]))
    {
      result = args[0];
      pBuffer = ObjectWrap::Unwrap<BufferObject>(args[0]->ToObject());
      if (msg.size() > pBuffer->buffer().size())
      {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("Size of read buffer is too small")));
      }
      std::copy(msg.begin(), msg.end(), pBuffer->buffer().begin());
    }
    else
    {
      return v8::ThrowException(v8::Exception::Error(v8::String::New("Read buffer not provided")));
    }
  }
  result->ToObject()->Set(v8::String::NewSymbol("payloadSize"), v8::Uint32::New(msg.size()));
  pObject->clearReadable();
  return result;
}

v8::Handle<v8::Value> ZMQSocketObject::close(const v8::Arguments& args)
{
  v8::HandleScope scope;
  ZMQSocketObject* pObject = ObjectWrap::Unwrap<ZMQSocketObject>(args.This());
  pObject->_pSocket->close();
  _socketList.remove(pObject);
  __wakeup_pipe();
  return v8::Undefined();
}

v8::Handle<v8::Value> ZMQSocketObject::getFd(const v8::Arguments& args)
{
  v8::HandleScope scope;
  ZMQSocketObject* pObject = ObjectWrap::Unwrap<ZMQSocketObject>(args.This());
  return v8::Int32::New(pObject->_pipe[0]);
}
  
void ZMQSocketObject::Init(v8::Handle<v8::Object> exports)
{
  v8::HandleScope scope;
  exports->Set(v8::String::New("REQ"), v8::Integer::New(Socket::REQ), v8::ReadOnly);
  exports->Set(v8::String::New("REP"), v8::Integer::New(Socket::REP), v8::ReadOnly);
  exports->Set(v8::String::New("PUSH"), v8::Integer::New(Socket::PUSH), v8::ReadOnly);
  exports->Set(v8::String::New("PULL"), v8::Integer::New(Socket::PULL), v8::ReadOnly);
  exports->Set(v8::String::New("PUB"), v8::Integer::New(Socket::PUB), v8::ReadOnly);
  exports->Set(v8::String::New("SUB"), v8::Integer::New(Socket::SUB), v8::ReadOnly);

  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(ZMQSocketObject::New);
  tpl->SetClassName(v8::String::NewSymbol("ZMQSocket"));
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("ObjectType"), v8::String::NewSymbol("ZMQSocket"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("connect"), v8::FunctionTemplate::New(connect)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("bind"), v8::FunctionTemplate::New(bind)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("connect"), v8::FunctionTemplate::New(connect)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("publish"), v8::FunctionTemplate::New(publish)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("subscribe"), v8::FunctionTemplate::New(subscribe)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("send"), v8::FunctionTemplate::New(send)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("receive"), v8::FunctionTemplate::New(receive)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("_close"), v8::FunctionTemplate::New(close)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("getFd"), v8::FunctionTemplate::New(getFd)->GetFunction());
  
  ZMQSocketObject::_constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
  exports->Set(v8::String::NewSymbol("ZMQSocket"), ZMQSocketObject::_constructor);
  
  _context = new zmq::context_t(1);
  _notifySem = new OSS::Semaphore();
  _notifyReceiver = new ZMQSocketObject(OSS::ZMQ::ZMQSocket::PULL);
  _notifySender = new ZMQSocketObject(OSS::ZMQ::ZMQSocket::PUSH);
  assert(_notifyReceiver->getSocket()->bind("inproc://zmq_notifier"));
  assert(_notifySender->getSocket()->connect("inproc://zmq_notifier"));
  ZMQSocketObject::_pPollThread = new boost::thread(boost::bind(pollForReadEvents));
}

static v8::Handle<v8::Value> cleanup_exports(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (_isTerminating)
  {
    //
    // Don't let this function be called twice or we will double free stuff
    //
    return v8::Undefined();
  }

  _isTerminating = true;
  __wakeup_pipe();
  
  
  //
  // Disable the wakeup pipe from here forward
  //
  _wakeupPipeEnabled = false;
  ZMQSocketObject::_pPollThread->join();
  delete ZMQSocketObject::_pPollThread;
  delete _notifyReceiver;
  delete _notifySender;
  delete _notifySem;
  //
  // Intentionally leak the context for it might block if there are
  // pending I/O 
  //
  // delete _context ;

  return v8::Undefined();
}

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  
  ZMQSocketObject::Init(exports);
  exports->Set(v8::String::NewSymbol("__cleanup_exports"), v8::FunctionTemplate::New(cleanup_exports)->GetFunction());
  
  return exports;
}

JS_REGISTER_MODULE(JSZMQSocket);


