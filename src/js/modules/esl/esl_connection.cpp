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
#include "OSS/JS/modules/ESLConnectionObject.h"
#include "OSS/JS/modules/ESLEventObject.h"

using OSS::JS::JSObjectWrap;
using OSS::Net::ESLEvent;
using OSS::Net::ESLConnection;

JS_CLASS_INTERFACE(ESLConnectionObject, "ESLConnection") 
{
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "connect", connect);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "getInfo", getInfo);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "send", send);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "sendRecv", sendRecv);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "api", api);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "bgapi", bgapi);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "sendEvent", sendEvent);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "sendMsg", sendMsg);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "recvEvent", recvEvent);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "recvEventTimed", recvEventTimed);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "filter", filter);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "events", events);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "execute", execute);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "disconnect", disconnect);
  JS_CLASS_METHOD_DEFINE(ESLConnectionObject, "connected", connected); 
  JS_CLASS_INTERFACE_END(ESLConnectionObject);
}

ESLConnectionObject::ESLConnectionObject()
{
  _connection = ESLConnection::Ptr(new ESLConnection());
}

ESLConnectionObject::~ESLConnectionObject()
{
}

JS_CONSTRUCTOR_IMPL(ESLConnectionObject)
{
  ESLConnectionObject* obj = new ESLConnectionObject();
  obj->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(ESLConnectionObject::connect)
{
  
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(3);
  js_method_arg_assert_string(0);
  js_method_arg_assert_uint32(1);
  js_method_arg_assert_string(2);

  std::string host = js_method_arg_as_std_string(0);
  unsigned short port =js_method_arg_as_uint32(1);
  std::string password = js_method_arg_as_std_string(2);
  
  OSS_LOG_INFO("Connecting to " << host << ":" << port);
  
  return JSBoolean(pObject->_connection->connect(host, port, password));
}

JS_METHOD_IMPL(ESLConnectionObject::getInfo)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  ESLEvent::Ptr pResponse = pObject->_connection->getInfo();
  if (!pResponse)
  {
    return JSUndefined();
  }
  return JSString(pResponse->data());
}

JS_METHOD_IMPL(ESLConnectionObject::send)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(1);
  std::string cmd = js_method_arg_as_std_string(0);
  return JSBoolean(pObject->_connection->send(cmd));
}

JS_METHOD_IMPL(ESLConnectionObject::sendRecv)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(1);
  std::string cmd = js_method_arg_as_std_string(0);
  ESLEvent::Ptr pResponse = pObject->_connection->sendRecv(cmd);
  if (!pResponse)
  {
    return JSUndefined();
  }
  return JSString(pResponse->data());
}

JS_METHOD_IMPL(ESLConnectionObject::api)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(2);
  std::string cmd = js_method_arg_as_std_string(0);
  std::string arg = js_method_arg_as_std_string(1);
  ESLEvent::Ptr pResponse = pObject->_connection->api(cmd, arg);
  if (!pResponse)
  {
    return JSUndefined();
  }
  return JSString(pResponse->data());
}

JS_METHOD_IMPL(ESLConnectionObject::bgapi)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(3);
  std::string cmd = js_method_arg_as_std_string(0);
  std::string arg = js_method_arg_as_std_string(1);
  std::string uuid = js_method_arg_as_std_string(2);
  ESLEvent::Ptr pResponse = pObject->_connection->bgapi(cmd, arg, uuid);
  if (!pResponse)
  {
    return JSUndefined();
  }
  return JSString(pResponse->data());
}

JS_METHOD_IMPL(ESLConnectionObject::sendEvent)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(1);
  ESLEventObject* pEvent = js_method_arg_as_esl_event(0);
  if (!pEvent)
  {
    return JSUndefined();
  }
  ESLEvent::Ptr pResponse = pObject->_connection->sendEvent(pEvent->_event);
  if (!pResponse)
  {
    return JSUndefined();
  }
  return JSString(pResponse->data());
}

JS_METHOD_IMPL(ESLConnectionObject::sendMsg)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(2);
  ESLEventObject* pEvent = js_method_arg_as_esl_event(0);
  std::string uuid = js_method_arg_as_std_string(1);
  if (!pEvent)
  {
    return JSFalse;
  }
  return JSBoolean(pObject->_connection->sendMsg(pEvent->_event, uuid));
}

JS_METHOD_IMPL(ESLConnectionObject::recvEvent)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  ESLEvent::Ptr pResponse = pObject->_connection->recvEvent();
  if (!pResponse)
  {
    return JSUndefined();
  }
  return JSString(pResponse->data());
}

JS_METHOD_IMPL(ESLConnectionObject::recvEventTimed)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(1);
  int timeout = js_method_arg_as_int32(0);
  ESLEvent::Ptr pResponse = pObject->_connection->recvEventTimed(timeout);
  if (!pResponse)
  {
    return JSUndefined();
  }
  return JSString(pResponse->data());
}

JS_METHOD_IMPL(ESLConnectionObject::filter)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(2);
  std::string header = js_method_arg_as_std_string(0);
  std::string value = js_method_arg_as_std_string(1);
  ESLEvent::Ptr pResponse = pObject->_connection->filter(header, value);
  if (!pResponse)
  {
    return JSUndefined();
  }
  return JSString(pResponse->data());
}

JS_METHOD_IMPL(ESLConnectionObject::events)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(1);
  std::string value = js_method_arg_as_std_string(0);
  return JSBoolean(pObject->_connection->events("json", value));
}

JS_METHOD_IMPL(ESLConnectionObject::execute)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  js_method_arg_assert_size_gteq(3);
  std::string app = js_method_arg_as_std_string(0);
  std::string arg = js_method_arg_as_std_string(1);
  std::string uuid = js_method_arg_as_std_string(2);
  ESLEvent::Ptr pResponse = pObject->_connection->execute(app, arg, uuid);
  if (!pResponse)
  {
    return JSUndefined();
  }
  return JSString(pResponse->data());
}

JS_METHOD_IMPL(ESLConnectionObject::disconnect)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  return JSBoolean(pObject->_connection->disconnect());
}

JS_METHOD_IMPL(ESLConnectionObject::connected)
{
  ESLConnectionObject* pObject = js_method_arg_unwrap_self(ESLConnectionObject);
  return JSBoolean(pObject->_connection->connected());
}

JS_EXPORTS_INIT()
{
  js_export_class(ESLConnectionObject);
  js_export_finalize(); 
}

JS_REGISTER_MODULE(JSESLConnection);
