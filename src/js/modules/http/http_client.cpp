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
#include "OSS/JS/modules/HttpClientObject.h"
#include "OSS/JS/modules/HttpRequestObject.h"
#include "OSS/JS/modules/HttpResponseObject.h"
#include "OSS/JS/modules/BufferObject.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/modules/QueueObject.h"

using OSS::JS::ObjectWrap;

//
// Define the Interface
//
JS_CLASS_INTERFACE(HttpClientObject, "HttpClient") 
{
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "setHost", setHost);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "getHost", getHost);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "setPort", setPort);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "getPort", getPort);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "setProxyHost", setProxyHost);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "setProxyPort", setProxyPort);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "getProxyHost", getProxyHost);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "getProxyPort", getProxyPort);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "setKeepAliveTimeout", setKeepAliveTimeout);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "getKeepAliveTimeout", getKeepAliveTimeout);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "sendRequest", sendRequest);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "receiveResponse", receiveResponse);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "connected", connected);
  
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "_read", read);
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "_write", write);
  
  JS_CLASS_METHOD_DEFINE(HttpClientObject, "setEventFd", setEventFd);
  
  JS_CLASS_INTERFACE_END(HttpClientObject);
}

HttpClientObject::HttpClientObject() :
  _input(0),
  _output(0),
  _eventFd(-1)
{
}

HttpClientObject::~HttpClientObject()
{
}

JS_CONSTRUCTOR_IMPL(HttpClientObject)
{
  js_enter_scope();
  HttpClientObject* pClient = new HttpClientObject();
  pClient->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(HttpClientObject::setHost)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pClient->_session.setHost(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpClientObject::getHost)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  return JSString(pClient->_session.getHost());
}

JS_METHOD_IMPL(HttpClientObject::setPort)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  uint32_t value = js_method_arg_as_uint32(0);
  pClient->_session.setPort(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpClientObject::getPort)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  return JSUInt32(pClient->_session.getPort());
}

JS_METHOD_IMPL(HttpClientObject::setProxyHost)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pClient->_session.setProxyHost(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpClientObject::setProxyPort)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  uint32_t value = js_method_arg_as_uint32(0);
  pClient->_session.setProxyPort(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpClientObject::getProxyHost)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  return JSString(pClient->_session.getProxyHost());
}

JS_METHOD_IMPL(HttpClientObject::getProxyPort)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  return JSUInt32(pClient->_session.getProxyPort());
}

JS_METHOD_IMPL(HttpClientObject::setKeepAliveTimeout)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  uint32_t seconds = js_method_arg_as_uint32(0);
  Poco::Timespan timespan;
  timespan.assign(seconds, 0);
  pClient->_session.setKeepAliveTimeout(timespan);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpClientObject::getKeepAliveTimeout)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  const Poco::Timespan& timespan = pClient->_session.getKeepAliveTimeout();
  return JSUInt32(timespan.milliseconds() * 1000);
}

JS_METHOD_IMPL(HttpClientObject::sendRequest)
{
  js_enter_scope();
  js_method_arg_declare_self(HttpClientObject, self);
  js_method_arg_declare_external_object(HttpRequestObject, pRequest, 0);
  
  self->_output = &(self->_session.sendRequest(*pRequest->request()));
  return JSBoolean(self->_output && !self->_output->bad() && !self->_output->fail());
}


class ResponseReceiver : public Poco::Runnable
{
public:
  HttpClientObject* _client;
  HttpResponseObject* _response;
  int _fd;
  ResponseReceiver(HttpClientObject* client, HttpResponseObject* response, int emitterFd) :
    _client(client),
    _response(response),
    _fd(emitterFd)
  {
  }
  
  void run()
  {
    try
    {
      _client->setOutputStream(0);
      _client->setInputStream(&(_client->getSession().receiveResponse((*_response->response()))));
      OSS::JSON::Array json;
      json[0] = OSS::JSON::String("response_ready");
      QueueObject::json_enqueue_object(_client->getEventFd(), json);
    }
    catch(const HttpClientObject::MessageException& e)
    {
      OSS_LOG_ERROR(e.message().c_str());
      OSS::JSON::Array json;
      json[0] = OSS::JSON::String("error");
      json[1] = OSS::JSON::String(e.message().c_str());
      QueueObject::json_enqueue_object(_client->getEventFd(), json);
    }
    catch(const std::exception& e)
    {
      OSS::JSON::Array json;
      json[0] = OSS::JSON::String("error");
      json[1] = OSS::JSON::String(e.what());
      QueueObject::json_enqueue_object(_client->getEventFd(), json);
    }
    
    delete this;
  }
};

JS_METHOD_IMPL(HttpClientObject::receiveResponse)
{
  js_enter_scope();
  js_method_arg_declare_self(HttpClientObject, self);
  js_method_arg_declare_external_object(HttpResponseObject, pResponse, 0);
  
  if (self->getEventFd() == -1)
  {
    js_throw("Event Emitter FD not set");
  }
  
  ResponseReceiver* runnable = new ResponseReceiver(self, pResponse, self->getEventFd());
  try
  {
    JSPlugin::_pThreadPool->start(*runnable);
  }
  catch(const NoThreadAvailableException& e)
  {
    delete runnable;
    js_throw(e.message().c_str());
  }
  catch(...)
  {
    delete runnable;
    js_throw("Unknown Exception");
  }
  return JSUndefined();
}

JS_METHOD_IMPL(HttpClientObject::connected)
{
  js_enter_scope();
  HttpClientObject* pClient = js_method_arg_unwrap_self(HttpClientObject);
  return JSBoolean(pClient->_session.connected());
}

class ResponseReader : public Poco::Runnable
{
public:
  HttpClientObject* _client;
  BufferObject* _buf;
  uint32_t _size;
  ResponseReader(HttpClientObject* client, BufferObject* buf, uint32_t size) :
    _client(client),
    _buf(buf),
    _size(size)
  {
  }
  
  void run()
  {
    try
    {
      std::istream& strm = *_client->getInputStream();
      strm.read((char*)_buf->buffer().data(), _size);
      OSS::JSON::Array json;
      json[0] = OSS::JSON::String("read_ready");
      json[1] = OSS::JSON::Number(strm.gcount());
      QueueObject::json_enqueue_object(_client->getEventFd(), json);
    }
    catch(const HttpClientObject::MessageException& e)
    {
      OSS_LOG_ERROR(e.message().c_str());
      OSS::JSON::Array json;
      json[0] = OSS::JSON::String("error");
      json[1] = OSS::JSON::String(e.message().c_str());
      QueueObject::json_enqueue_object(_client->getEventFd(), json);
    }
    catch(const std::exception& e)
    {
      OSS::JSON::Array json;
      json[0] = OSS::JSON::String("error");
      json[1] = OSS::JSON::String(e.what());
      QueueObject::json_enqueue_object(_client->getEventFd(), json);
    }
    
    delete this;
  }
};

JS_METHOD_IMPL(HttpClientObject::read)
{
  js_enter_scope();
  js_method_arg_declare_self(HttpClientObject, self);
  js_method_arg_declare_external_object(BufferObject, buf, 0);
  js_method_arg_declare_uint32(size, 1);
  
  if (self->_input)
  {
    ResponseReader* runnable = new ResponseReader(self, buf, size);
    try
    {
      JSPlugin::_pThreadPool->start(*runnable);
    }
    catch(const NoThreadAvailableException& e)
    {
      delete runnable;
      js_throw(e.message().c_str());
    }
    catch(...)
    {
      delete runnable;
      js_throw("Unknown Exception");
    }
  }
  else
  {
    OSS::JSON::Array json;
    json[0] = OSS::JSON::String("error");
    json[1] = OSS::JSON::String("Input Stream Not Set");
    QueueObject::json_enqueue_object(self->getEventFd(), json);
  }
  return JSUndefined();
}

JS_METHOD_IMPL(HttpClientObject::write)
{
  js_enter_scope();
  js_method_arg_declare_self(HttpClientObject, self);
  js_method_arg_declare_external_object(BufferObject, buf, 0);
  js_method_arg_declare_uint32(size, 1);
  
  if (self->_output)
  {
    std::ostream& strm = *self->_output;
    return JSBoolean(!strm.write((char*)buf->buffer().data(), size).fail());
  }
  
  return JSBoolean(false);
}

JS_METHOD_IMPL(HttpClientObject::setEventFd)
{
  js_enter_scope();
  js_method_arg_declare_self(HttpClientObject, self);
  js_method_arg_declare_int32(fd, 0);
  self->_eventFd = fd;
  return JSUndefined();
}

JS_EXPORTS_INIT()
{
  js_enter_scope();
  js_export_class(HttpClientObject);
  js_export_finalize(); 
}

JS_REGISTER_MODULE(JSHttpClient);
