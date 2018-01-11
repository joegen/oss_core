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
#include "OSS/JS/modules/HttpParserObject.h"
#include "OSS/JS/modules/BufferObject.h"

http_parser_settings HttpParserObject::_settings;

JS_CLASS_INTERFACE(HttpParserObject, "HttpParser") 
{
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "getMethod", getMethod);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "getUrl", getUrl);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "getStatus", getStatus);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "getStatusCode", getStatusCode);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "getMajorVersion", getMajorVersion);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "getMinorVersion", getMinorVersion);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "getHeader", getHeader);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "getHeaderCount", getHeaderCount);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "getHeaderAt", getHeaderAt);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "getBody", getBody);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "isRequest", isRequest);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "isReponse", isReponse);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "isMessageComplete", isMessageComplete);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "isHeadersComplete", isHeadersComplete);
  JS_CLASS_METHOD_DEFINE(HttpParserObject, "parse", parse);
  
  JS_CLASS_INTERFACE_END(HttpParserObject); 
}

HttpParserObject::HttpParserObject() :
  _statusCode(0),
  _messageComplete(false),
  _headersComplete(false),
  _messageType(HTTP_BOTH),
  _httpMajor(0),
  _httpMinor(0)
{
  http_parser_init(&_parser, HTTP_BOTH);
  _parser.data = (void*) this;
}


HttpParserObject::~HttpParserObject()
{
}

int HttpParserObject::on_message_begin(http_parser* pParser)
{
  HttpParserObject* pObject = reinterpret_cast<HttpParserObject*>(pParser->data);
  return pObject->onMessageBegin();
}

int HttpParserObject::on_headers_complete(http_parser* pParser)
{
  HttpParserObject* pObject = reinterpret_cast<HttpParserObject*>(pParser->data);
  return pObject->onHeadersComplete();
}

int HttpParserObject::on_message_complete(http_parser* pParser)
{
  HttpParserObject* pObject = reinterpret_cast<HttpParserObject*>(pParser->data);
  return pObject->onMessageComplete();
}

int HttpParserObject::on_url(http_parser* pParser, const char* at, size_t length)
{
  HttpParserObject* pObject = reinterpret_cast<HttpParserObject*>(pParser->data);
  return pObject->onUrl(at, length);
}

int HttpParserObject::on_status(http_parser* pParser, const char* at, size_t length)
{
  HttpParserObject* pObject = reinterpret_cast<HttpParserObject*>(pParser->data);
  return pObject->onStatus(at, length);
}

int HttpParserObject::on_header_field(http_parser* pParser, const char* at, size_t length)
{
  HttpParserObject* pObject = reinterpret_cast<HttpParserObject*>(pParser->data);
  return pObject->onHeaderField(at, length);
}

int HttpParserObject::on_header_value(http_parser* pParser, const char* at, size_t length)
{
  HttpParserObject* pObject = reinterpret_cast<HttpParserObject*>(pParser->data);
  return pObject->onHeaderValue(at, length);
}

int HttpParserObject::on_body(http_parser* pParser, const char* at, size_t length)
{
  HttpParserObject* pObject = reinterpret_cast<HttpParserObject*>(pParser->data);
  return pObject->onBody(at, length);
}

int HttpParserObject::onMessageBegin()
{
  _headers.clear();
  _names.clear();
  _httpMajor = 0;
  _httpMinor = 0;
  _statusCode = 0;
  _method = "";
  _url = "";
  _body = "";
  _messageComplete = false;
  _headersComplete = false;
  _messageType = HTTP_BOTH;
  return 0;
}

int HttpParserObject::onHeadersComplete()
{
  _headersComplete = true;
  return 0;
}

int HttpParserObject::onMessageComplete()
{
  _httpMajor = _parser.http_major;
  _httpMinor = _parser.http_minor;
  _messageComplete = true;
  return 0;
}

int HttpParserObject::onUrl(const char* at, size_t length)
{
  _messageType = HTTP_REQUEST;
  _url = std::string(at, length);
  _method = http_method_str((http_method)_parser.method);
  return 0;
}

int HttpParserObject::onStatus(const char* at, size_t length)
{
  _messageType = HTTP_RESPONSE;
  _status = std::string(at, length);
  _statusCode = _parser.status_code;
  return 0;
}

int HttpParserObject::onHeaderField(const char* at, size_t length)
{
  _currentHeader = std::string(at, length);
  _names.push_back(_currentHeader);
  OSS::string_to_lower(_currentHeader);
  return 0;
}

int HttpParserObject::onHeaderValue(const char* at, size_t length)
{
  _headers[_currentHeader] = std::string(at, length);
  return 0;
}

int HttpParserObject::onBody(const char* at, size_t length)
{
  _body = std::string(at, length);
  return 0;
}

bool HttpParserObject::getHeader(const std::string& header, std::string& value)
{
  std::string key(header);
  OSS::string_to_lower(key);
  Headers::iterator iter = _headers.find(key);
  if (iter == _headers.end())
  {
    return false;
  }
  value = iter->second;
  return  true;
}

bool HttpParserObject::getHeaderAt(uint32_t index, std::string& name, std::string& value)
{
  if (index >= _names.size())
  {
    return false;
  }
  std::string header = _names[index];
  name = header;
  OSS::string_to_lower(header);
  Headers::iterator iter = _headers.find(header);
  if (iter == _headers.end())
  {
    name = "";
    return false;
  }
  value = iter->second;
  return true;
}

JS_CONSTRUCTOR_IMPL(HttpParserObject) 
{
  HttpParserObject* pFile = new HttpParserObject();
  pFile->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(HttpParserObject::getMethod)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  if (pParser->getUrl().empty())
  {
    return JSUndefined();
  }
  return JSString(pParser->getMethod().c_str());
}

JS_METHOD_IMPL(HttpParserObject::getUrl)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  if (pParser->getUrl().empty())
  {
    return JSUndefined();
  }
  return JSString(pParser->getUrl().c_str());
}

JS_METHOD_IMPL(HttpParserObject::getStatus)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  if (pParser->getStatus().empty())
  {
    return JSUndefined();
  }
  return JSString(pParser->getStatus().c_str());
}

JS_METHOD_IMPL(HttpParserObject::getStatusCode)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  if (!pParser->getStatusCode())
  {
    return JSUndefined();
  }
  return JSInt32(pParser->getStatusCode());
}

JS_METHOD_IMPL(HttpParserObject::getMajorVersion)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  return JSInt32(pParser->getMajorVersion());
}

JS_METHOD_IMPL(HttpParserObject::getMinorVersion)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  return JSInt32(pParser->getMinorVersion());
}

JS_METHOD_IMPL(HttpParserObject::getHeader)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_is_string(0);
  std::string header = js_method_arg_as_std_string(0);
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  std::string value;
  if (!pParser->getHeader(header, value))
  {
    return JSUndefined();
  }
  return JSString(value.c_str());
}

JS_METHOD_IMPL(HttpParserObject::getHeaderCount)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  return JSUInt32(pParser->getHeaderCount());
}

JS_METHOD_IMPL(HttpParserObject::getHeaderAt)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  uint32_t index = js_method_arg_as_uint32(0);
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  std::string name;
  std::string value;
  if (!pParser->getHeaderAt(index, name, value))
  {
    return JSUndefined();
  }
  JSLocalObjectHandle header = JSObject();
  header->Set(JSLiteral("header"), JSString(name.c_str()));
  header->Set(JSLiteral("value"), JSString(value.c_str()));
  return header;
}

JS_METHOD_IMPL(HttpParserObject::getBody)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  if (pParser->getBody().empty())
  {
    return JSUndefined();
  }
  return JSString(pParser->getBody().c_str());
}

JS_METHOD_IMPL(HttpParserObject::isRequest)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  return JSBoolean(pParser->isRequest());
}

JS_METHOD_IMPL(HttpParserObject::isReponse)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  return JSBoolean(pParser->isResponse());
}

JS_METHOD_IMPL(HttpParserObject::isMessageComplete)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  return JSBoolean(pParser->isMessageComplete());
}

JS_METHOD_IMPL(HttpParserObject::isHeadersComplete)
{
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  return JSBoolean(pParser->isHeadersComplete());
}

JS_METHOD_IMPL(HttpParserObject::parse)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_buffer(0);
  BufferObject* pBuffer = js_method_arg_as_buffer(0);
  HttpParserObject* pParser = js_method_arg_unwrap_self(HttpParserObject);
  return JSUInt32(http_parser_execute(&pParser->_parser, &_settings, (const char*)pBuffer->buffer().data(), pBuffer->buffer().size()));
}

JS_EXPORTS_INIT()
{
  memset(&HttpParserObject::_settings, 0, sizeof(HttpParserObject::_settings));
  HttpParserObject::_settings.on_message_begin = HttpParserObject::on_message_begin;
  HttpParserObject::_settings.on_url = HttpParserObject::on_url;
  HttpParserObject::_settings.on_status = HttpParserObject::on_status;
  HttpParserObject::_settings.on_header_field = HttpParserObject::on_header_field;
  HttpParserObject::_settings.on_header_value = HttpParserObject::on_header_value;
  HttpParserObject::_settings.on_headers_complete = HttpParserObject::on_headers_complete;
  HttpParserObject::_settings.on_body = HttpParserObject::on_body;
  HttpParserObject::_settings.on_message_complete = HttpParserObject::on_message_complete;
  
  js_export_class(HttpParserObject);
  
  js_export_const(HTTP_BOTH);
  js_export_const(HTTP_REQUEST);
  js_export_const(HTTP_RESPONSE);
  
  js_export_finalize();
}

JS_REGISTER_MODULE(JSHTTPParser);