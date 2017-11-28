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
#include "OSS/JS/modules/HttpRequestObject.h"

using OSS::JS::ObjectWrap;

//
// Define the Interface
//
JS_CLASS_INTERFACE(HttpRequestObject, "HttpRequest") 
{
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setMethod", setMethod);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getMethod", getMethod);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setUri", setUri);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getUri", getUri);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setHost", setHost);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getHost", getHost);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setCookies", setCookies);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getCookies", getCookies);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "hasCredentials", hasCredentials);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getCredentials", getCredentials);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setCredentials", setCredentials);
  
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setVersion", setVersion);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getVersion", getVersion);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setContentLength", setContentLength);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getContentLength", getContentLength);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setTransferEncoding", setTransferEncoding);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getTransferEncoding", getTransferEncoding);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setChunkedTransferEncoding", setChunkedTransferEncoding);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getChunkedTransferEncoding", getChunkedTransferEncoding);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setContentType", setContentType);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getContentType", getContentType);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "setKeepAlive", setKeepAlive);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "getKeepAlive", getKeepAlive);
  
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "reset", reset);
  JS_CLASS_METHOD_DEFINE(HttpRequestObject, "dispose", dispose);
  
  JS_CLASS_INTERFACE_END(HttpRequestObject);
}

HttpRequestObject::HttpRequestObject() :
  _request(0)
{
  _request = new Request();
}

HttpRequestObject::~HttpRequestObject()
{
  delete _request;
}

JS_CONSTRUCTOR_IMPL(HttpRequestObject)
{
  js_enter_scope();
  HttpRequestObject* pObject = new HttpRequestObject();
  pObject->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(HttpRequestObject::setMethod)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pObject->_request->setMethod(value);
  return JSUndefined();
}
JS_METHOD_IMPL(HttpRequestObject::getMethod)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  return JSString(pObject->_request->getMethod());
}

JS_METHOD_IMPL(HttpRequestObject::setUri)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pObject->_request->setURI(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::getUri)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  return JSString(pObject->_request->getURI());
}

JS_METHOD_IMPL(HttpRequestObject::setHost)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pObject->_request->setHost(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::getHost)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  return JSString(pObject->_request->getHost());
}

JS_METHOD_IMPL(HttpRequestObject::setCookies)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_object(0);
  
  JSLocalObjectHandle obj = js_method_arg_as_object(0);
  JSLocalArrayHandle properties = obj->GetPropertyNames();
  Cookies cookies;
  for(uint32_t i = 0; i < properties->Length(); i++)
  {
    JSLocalValueHandle name = properties->Get(i);
    JSLocalValueHandle value = obj->Get(name);
    cookies.add(js_handle_as_std_string(name), js_handle_as_std_string(value));
  }
  pObject->_request->setCookies(cookies);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::getCookies)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  Cookies cookies;
  pObject->_request->getCookies(cookies);
  if (cookies.empty())
  {
    return JSUndefined();
  }
  JSLocalObjectHandle obj = JSObject();
  for (Cookies::ConstIterator iter = cookies.begin(); iter != cookies.end(); iter++)
  {
    obj->Set(JSLiteral(iter->first.c_str()), JSString(iter->second));
  }
  return obj;
}

JS_METHOD_IMPL(HttpRequestObject::hasCredentials)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  return JSBoolean(pObject->_request->hasCredentials());
}

JS_METHOD_IMPL(HttpRequestObject::getCredentials)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  if (!pObject->_request->hasCredentials())
  {
    return JSUndefined();
  }
  JSLocalObjectHandle obj = JSObject();
  
  std::string scheme;
  std::string authInfo;
  pObject->_request->getCredentials(scheme, authInfo);
  
  obj->Set(JSLiteral("scheme"), JSString(scheme));
  obj->Set(JSLiteral("authInfo"), JSString(authInfo));
  
  return obj;
}

JS_METHOD_IMPL(HttpRequestObject::setCredentials)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  
  std::string scheme = js_method_arg_as_std_string(0);
  std::string authInfo = js_method_arg_as_std_string(1);
  pObject->_request->setCredentials(scheme, authInfo);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::setVersion)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pObject->_request->setVersion(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::getVersion)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  return JSString(pObject->_request->getVersion());
}

JS_METHOD_IMPL(HttpRequestObject::setContentLength)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_declare_uint32(contentLength, 0);
  pObject->_request->setContentLength(contentLength);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::getContentLength)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  return JSUInt32(pObject->_request->getContentLength());
}

JS_METHOD_IMPL(HttpRequestObject::setTransferEncoding)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pObject->_request->setTransferEncoding(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::getTransferEncoding)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  return JSString(pObject->_request->getTransferEncoding());
}

JS_METHOD_IMPL(HttpRequestObject::setChunkedTransferEncoding)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_declare_bool(flag, 0);
  pObject->_request->setChunkedTransferEncoding(flag);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::getChunkedTransferEncoding)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  return JSBoolean(pObject->_request->getChunkedTransferEncoding());
}

JS_METHOD_IMPL(HttpRequestObject::setContentType)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pObject->_request->setContentType(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::getContentType)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  return JSString(pObject->_request->getContentType());
}

JS_METHOD_IMPL(HttpRequestObject::setKeepAlive)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  js_method_arg_declare_bool(flag, 0);
  pObject->_request->setKeepAlive(flag);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::getKeepAlive)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  return JSBoolean(pObject->_request->getKeepAlive());
}

JS_METHOD_IMPL(HttpRequestObject::reset)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  delete pObject->_request;
  pObject->_request = new HttpRequestObject::Request();
  return JSUndefined();
}

JS_METHOD_IMPL(HttpRequestObject::dispose)
{
  js_enter_scope();
  HttpRequestObject* pObject = js_method_arg_unwrap_self(HttpRequestObject);
  js_assert(pObject && pObject->_request, "HTTP Request has been disposed");
  delete pObject->_request;
  pObject->_request = 0;
  return JSUndefined();
}


JS_EXPORTS_INIT()
{
  js_enter_scope();
  js_export_class(HttpRequestObject);
  
  js_export_string("HTTP_GET", HttpRequestObject::Request::HTTP_GET.c_str());
	js_export_string("HTTP_HEAD", HttpRequestObject::Request::HTTP_HEAD.c_str());
	js_export_string("HTTP_PUT", HttpRequestObject::Request::HTTP_PUT.c_str());
	js_export_string("HTTP_POST", HttpRequestObject::Request::HTTP_POST.c_str());
	js_export_string("HTTP_OPTIONS", HttpRequestObject::Request::HTTP_OPTIONS.c_str());
	js_export_string("HTTP_DELETE", HttpRequestObject::Request::HTTP_DELETE.c_str());
	js_export_string("HTTP_TRACE", HttpRequestObject::Request::HTTP_TRACE.c_str());
	js_export_string("HTTP_CONNECT", HttpRequestObject::Request::HTTP_CONNECT.c_str());
  js_export_string("HTTP_1_0", HttpRequestObject::Request::HTTP_1_0.c_str());
  js_export_string("HTTP_1_1", HttpRequestObject::Request::HTTP_1_1.c_str());
  
  js_export_finalize(); 
}

JS_REGISTER_MODULE(JSHttpRequest);