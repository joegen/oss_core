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
#include "OSS/JS/modules/HttpResponseObject.h"

using OSS::JS::JSObjectWrap;

//
// Define the Interface
//
JS_CLASS_INTERFACE(HttpResponseObject, "HttpResponse") 
{
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "setStatus", setStatus);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "getStatus", getStatus);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "setReason", setReason);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "getReason", getReason);
  
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "setVersion", setVersion);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "getVersion", getVersion);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "setContentLength", setContentLength);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "getContentLength", getContentLength);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "setTransferEncoding", setTransferEncoding);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "getTransferEncoding", getTransferEncoding);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "setChunkedTransferEncoding", setChunkedTransferEncoding);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "getChunkedTransferEncoding", getChunkedTransferEncoding);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "setContentType", setContentType);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "getContentType", getContentType);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "setKeepAlive", setKeepAlive);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "getKeepAlive", getKeepAlive);
  
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "reset", reset);
  JS_CLASS_METHOD_DEFINE(HttpResponseObject, "dispose", dispose);
  
  JS_CLASS_INTERFACE_END(HttpResponseObject);
}

HttpResponseObject::HttpResponseObject() :
  _response(0)
{
  _response = new Response();
}

HttpResponseObject::~HttpResponseObject()
{
  delete _response;
}

JS_CONSTRUCTOR_IMPL(HttpResponseObject)
{
  HttpResponseObject* pObject = new HttpResponseObject();
  pObject->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(HttpResponseObject::setStatus)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  js_method_arg_declare_uint32(status, 0);
  pObject->_response->setStatus((Status)status);
  return JSUndefined();
}
  
JS_METHOD_IMPL(HttpResponseObject::getStatus)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  return JSInt32(pObject->_response->getStatus());
}

JS_METHOD_IMPL(HttpResponseObject::setReason)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  js_method_arg_declare_string(reason, 0);
  pObject->_response->setReason(reason);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpResponseObject::getReason)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  return JSString(pObject->_response->getReason());
}

JS_METHOD_IMPL(HttpResponseObject::setVersion)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pObject->_response->setVersion(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpResponseObject::getVersion)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  return JSString(pObject->_response->getVersion());
}

JS_METHOD_IMPL(HttpResponseObject::setContentLength)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  js_method_arg_declare_uint32(contentLength, 0);
  pObject->_response->setContentLength(contentLength);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpResponseObject::getContentLength)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  return JSUInt32(pObject->_response->getContentLength());
}

JS_METHOD_IMPL(HttpResponseObject::setTransferEncoding)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pObject->_response->setTransferEncoding(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpResponseObject::getTransferEncoding)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  return JSString(pObject->_response->getTransferEncoding());
}

JS_METHOD_IMPL(HttpResponseObject::setChunkedTransferEncoding)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  js_method_arg_declare_bool(flag, 0);
  pObject->_response->setChunkedTransferEncoding(flag);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpResponseObject::getChunkedTransferEncoding)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  return JSBoolean(pObject->_response->getChunkedTransferEncoding());
}

JS_METHOD_IMPL(HttpResponseObject::setContentType)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string value = js_method_arg_as_std_string(0);
  pObject->_response->setContentType(value);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpResponseObject::getContentType)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  return JSString(pObject->_response->getContentType());
}

JS_METHOD_IMPL(HttpResponseObject::setKeepAlive)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  js_method_arg_declare_bool(flag, 0);
  pObject->_response->setKeepAlive(flag);
  return JSUndefined();
}

JS_METHOD_IMPL(HttpResponseObject::getKeepAlive)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  return JSBoolean(pObject->_response->getKeepAlive());
}

JS_METHOD_IMPL(getReasonForStatus)
{
  js_method_arg_declare_uint32(status, 0);
  return JSString(HttpResponseObject::Response::getReasonForStatus((HttpResponseObject::Status)status));
}

JS_METHOD_IMPL(HttpResponseObject::reset)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  delete pObject->_response;
  pObject->_response = new Response();
  return JSUndefined();
}

JS_METHOD_IMPL(HttpResponseObject::dispose)
{
  HttpResponseObject* pObject = js_method_arg_unwrap_self(HttpResponseObject);
  js_assert(pObject && pObject->_response, "HTTP Response has been disposed");
  delete pObject->_response;
  pObject->_response = 0;
  return JSUndefined();
}

JS_EXPORTS_INIT()
{
  js_export_class(HttpResponseObject);
  js_export_method("getReasonForStatus", getReasonForStatus);
  
  js_export_int32("HTTP_CONTINUE", 100);
  js_export_int32("HTTP_SWITCHING_PROTOCOLS", 101);
  js_export_int32("HTTP_PROCESSING", 102);
  js_export_int32("HTTP_OK", 200);
  js_export_int32("HTTP_CREATED", 201);
  js_export_int32("HTTP_ACCEPTED", 202);
  js_export_int32("HTTP_NONAUTHORITATIVE", 203);
  js_export_int32("HTTP_NO_CONTENT", 204);
  js_export_int32("HTTP_RESET_CONTENT", 205);
  js_export_int32("HTTP_PARTIAL_CONTENT", 206);
  js_export_int32("HTTP_MULTI_STATUS", 207);
  js_export_int32("HTTP_ALREADY_REPORTED", 208);
  js_export_int32("HTTP_IM_USED ", 226);
  js_export_int32("HTTP_MULTIPLE_CHOICES", 300);
  js_export_int32("HTTP_MOVED_PERMANENTLY", 301);
  js_export_int32("HTTP_FOUND", 302);
  js_export_int32("HTTP_SEE_OTHER", 303);
  js_export_int32("HTTP_NOT_MODIFIED", 304);
  js_export_int32("HTTP_USE_PROXY", 305);
  js_export_int32("HTTP_USEPROXY", 305); /// @deprecated
  js_export_int32("HTTP_TEMPORARY_REDIRECT", 307);
  js_export_int32("HTTP_PERMANENT_REDIRECT", 308);
  js_export_int32("HTTP_BAD_REQUEST", 400);
  js_export_int32("HTTP_UNAUTHORIZED", 401);
  js_export_int32("HTTP_PAYMENT_REQUIRED", 402);
  js_export_int32("HTTP_FORBIDDEN", 403);
  js_export_int32("HTTP_NOT_FOUND", 404);
  js_export_int32("HTTP_METHOD_NOT_ALLOWED", 405);
  js_export_int32("HTTP_NOT_ACCEPTABLE", 406);
  js_export_int32("HTTP_PROXY_AUTHENTICATION_REQUIRED", 407);
  js_export_int32("HTTP_REQUEST_TIMEOUT", 408);
  js_export_int32("HTTP_CONFLICT", 409);
  js_export_int32("HTTP_GONE", 410);
  js_export_int32("HTTP_LENGTH_REQUIRED", 411);
  js_export_int32("HTTP_PRECONDITION_FAILED", 412);
  js_export_int32("HTTP_REQUEST_ENTITY_TOO_LARGE", 413);
  js_export_int32("HTTP_REQUESTENTITYTOOLARGE", 413); /// @deprecated
  js_export_int32("HTTP_REQUEST_URI_TOO_LONG", 414);
  js_export_int32("HTTP_REQUESTURITOOLONG", 414); /// @deprecated
  js_export_int32("HTTP_UNSUPPORTED_MEDIA_TYPE", 415);
  js_export_int32("HTTP_UNSUPPORTEDMEDIATYPE", 415); /// @deprecated
  js_export_int32("HTTP_REQUESTED_RANGE_NOT_SATISFIABLE", 416);
  js_export_int32("HTTP_EXPECTATION_FAILED", 417);
  js_export_int32("HTTP_IM_A_TEAPOT", 418);
  js_export_int32("HTTP_ENCHANCE_YOUR_CALM", 420);
  js_export_int32("HTTP_MISDIRECTED_REQUEST", 421);
  js_export_int32("HTTP_UNPROCESSABLE_ENTITY", 422);
  js_export_int32("HTTP_LOCKED", 423);
  js_export_int32("HTTP_FAILED_DEPENDENCY", 424);
  js_export_int32("HTTP_UPGRADE_REQUIRED", 426);
  js_export_int32("HTTP_PRECONDITION_REQUIRED", 428);
  js_export_int32("HTTP_TOO_MANY_REQUESTS", 429);
  js_export_int32("HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE", 431);
  js_export_int32("HTTP_UNAVAILABLE_FOR_LEGAL_REASONS", 451);
  js_export_int32("HTTP_INTERNAL_SERVER_ERROR", 500);
  js_export_int32("HTTP_NOT_IMPLEMENTED", 501);
  js_export_int32("HTTP_BAD_GATEWAY", 502);
  js_export_int32("HTTP_SERVICE_UNAVAILABLE", 503);
  js_export_int32("HTTP_GATEWAY_TIMEOUT", 504);
  js_export_int32("HTTP_VERSION_NOT_SUPPORTED", 505);
  js_export_int32("HTTP_VARIANT_ALSO_NEGOTIATES", 506);
  js_export_int32("HTTP_INSUFFICIENT_STORAGE", 507);
  js_export_int32("HTTP_LOOP_DETECTED", 508);
  js_export_int32("HTTP_NOT_EXTENDED", 510);
  js_export_int32("HTTP_NETWORK_AUTHENTICATION_REQUIRED", 511);
  
  js_export_finalize(); 
}

JS_REGISTER_MODULE(JSHttpResponse);