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
#include "OSS/JS/modules/ESLEventObject.h"

using OSS::JS::JSObjectWrap;
using OSS::Net::ESLEvent;

JS_CLASS_INTERFACE(ESLEventObject, "ESLEvent") 
{
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "create", create);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "data", data);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "getHeader", getHeader);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "getBody", getBody);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "getEventName", getEventName);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "setBody", setBody);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "addHeader", addHeader);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "pushHeader", pushHeader);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "unshiftHeader", unshiftHeader);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "removeHeader", removeHeader);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "first", first);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "next", next);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "setPriority", setPriority);
  JS_CLASS_METHOD_DEFINE(ESLEventObject, "isValid", isValid);
  JS_CLASS_INTERFACE_END(ESLEventObject);
}

ESLEventObject::ESLEventObject()
{
  _event = ESLEvent::Ptr(new ESLEvent());
}

ESLEventObject::~ESLEventObject()
{
}

JS_CONSTRUCTOR_IMPL(ESLEventObject)
{
  ESLEventObject* obj = new ESLEventObject();
  obj->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(ESLEventObject::create)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  
  std::string type = js_method_arg_as_std_string(0);
  std::string subClass = js_method_arg_as_std_string(0);
  
  return JSBoolean(pObject->_event->create(type, subClass));
}

JS_METHOD_IMPL(ESLEventObject::data)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSUndefined();
  }
  return JSString(pObject->_event->data());
}

JS_METHOD_IMPL(ESLEventObject::getHeader)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSUndefined();
  }
  
  js_method_arg_assert_size_gteq(1);
  js_method_arg_assert_string(0);
  
  std::string name = js_method_arg_as_std_string(0);
  uint32_t index = 0;
  if (js_method_arg_length() > 1)
  {
    js_method_arg_assert_uint32(1);
    index = js_method_arg_as_uint32(1);
  }
  std::string value;
  if (pObject->_event->getHeader(name, value, index))
  {
    return JSString(value);
  }
  return JSUndefined();
}

JS_METHOD_IMPL(ESLEventObject::getBody)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSUndefined();
  }
  
  std::string body;
  if (pObject->_event->getBody(body))
  {
    return JSString(body);
  }
  return JSUndefined();
}

JS_METHOD_IMPL(ESLEventObject::getEventName)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSUndefined();
  }
  std::string name;
  if (pObject->_event->getEventName(name))
  {
    return JSString(name);
  }
  return JSUndefined();
}

JS_METHOD_IMPL(ESLEventObject::setBody)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSBoolean(false);
  }
  js_method_arg_assert_size_gteq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  return JSBoolean(pObject->_event->setBody(value));
}

JS_METHOD_IMPL(ESLEventObject::addHeader)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSBoolean(false);
  }
  js_method_arg_assert_size_gteq(2);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  
  std::string name = js_method_arg_as_std_string(0);
  std::string value = js_method_arg_as_std_string(1);
  return JSBoolean(pObject->_event->addHeader(name, value));
}

JS_METHOD_IMPL(ESLEventObject::pushHeader)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSBoolean(false);
  }
  js_method_arg_assert_size_gteq(2);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  
  std::string name = js_method_arg_as_std_string(0);
  std::string value = js_method_arg_as_std_string(1);
  return JSBoolean(pObject->_event->pushHeader(name, value));
}

JS_METHOD_IMPL(ESLEventObject::unshiftHeader)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSBoolean(false);
  }
  js_method_arg_assert_size_gteq(2);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  
  std::string name = js_method_arg_as_std_string(0);
  std::string value = js_method_arg_as_std_string(1);
  return JSBoolean(pObject->_event->unshiftHeader(name, value));
}

JS_METHOD_IMPL(ESLEventObject::removeHeader)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSBoolean(false);
  }
  js_method_arg_assert_size_gteq(1);
  js_method_arg_assert_string(0);
  std::string name = js_method_arg_as_std_string(0);
  return JSBoolean(pObject->_event->removeHeader(name));
}

JS_METHOD_IMPL(ESLEventObject::first)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSUndefined();
  }
  const char* iter = pObject->_event->first();
  if (iter)
  {
    return JSString(iter);
  }
  return JSUndefined();
}

JS_METHOD_IMPL(ESLEventObject::next)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSUndefined();
  }
  const char* iter = pObject->_event->next();
  if (iter)
  {
    return JSString(iter);
  }
  return JSUndefined();
}

JS_METHOD_IMPL(ESLEventObject::setPriority)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSBoolean(false);
  }
  js_method_arg_assert_size_gteq(1);
  js_method_arg_assert_int32(0);
  int32_t priority = js_method_arg_as_int32(0);
  if (priority > ESLEvent::EVENT_PRIORITY_HIGH)
  {
    return JSBoolean(false);
  }
  
  return JSBoolean(pObject->_event->setPriority((ESLEvent::Priority)priority));
}

JS_METHOD_IMPL(ESLEventObject::isValid)
{
  ESLEventObject* pObject = js_method_arg_unwrap_self(ESLEventObject);
  if (!pObject->_event)
  {
    return JSBoolean(false);
  }
  return JSBoolean(pObject->_event->isValid());
}

JS_EXPORTS_INIT()
{
  js_export_class(ESLEventObject);
  js_export_int32("EVENT_PRIORITY_NORMAL", ESLEvent::EVENT_PRIORITY_NORMAL);
  js_export_int32("EVENT_PRIORITY_LOW", ESLEvent::EVENT_PRIORITY_LOW);
  js_export_int32("EVENT_PRIORITY_HIGH", ESLEvent::EVENT_PRIORITY_HIGH);
  js_export_finalize(); 
}

JS_REGISTER_MODULE(JSESLEvent);