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

#ifndef OSS_HTTPPARSEROBJECT_H_INCLUDED
#define OSS_HTTPPARSEROBJECT_H_INCLUDED


#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/modules/http_parser/http_parser.h"

class HttpParserObject : public OSS::JS::ObjectWrap
{
public:
  typedef std::map<std::string, std::string> Headers;
  typedef std::vector<std::string> HeaderNames;
  
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(getMajorVersion);
  JS_METHOD_DECLARE(getMinorVersion);
  JS_METHOD_DECLARE(getMethod);
  JS_METHOD_DECLARE(getUrl);
  JS_METHOD_DECLARE(getStatusCode);
  JS_METHOD_DECLARE(getStatus);
  JS_METHOD_DECLARE(getHeader);
  JS_METHOD_DECLARE(getHeaderCount);
  JS_METHOD_DECLARE(getHeaderAt);
  JS_METHOD_DECLARE(getBody);
  JS_METHOD_DECLARE(isRequest);
  JS_METHOD_DECLARE(isReponse);
  JS_METHOD_DECLARE(isMessageComplete);
  JS_METHOD_DECLARE(isHeadersComplete);
  JS_METHOD_DECLARE(parse);

  static http_parser_settings _settings;
  static int on_message_begin(http_parser* pParser);
  static int on_headers_complete(http_parser* pParser);
  static int on_message_complete(http_parser* pParser);
  static int on_url(http_parser* pParser, const char* at, size_t length);
  static int on_status(http_parser* pParser, const char* at, size_t length);
  static int on_header_field(http_parser* pParser, const char* at, size_t length);
  static int on_header_value(http_parser* pParser, const char* at, size_t length);
  static int on_body(http_parser* pParser, const char* at, size_t length);
  
  bool isMessageComplete() const;
  bool isHeadersComplete() const;
  int getMessageType() const;
  const std::string& getMethod() const;
  const std::string& getUrl() const;
  const std::string& getStatus() const;
  int getStatusCode() const;
  bool getHeader(const std::string& header, std::string& value);
  uint32_t getHeaderCount() const;
  bool getHeaderAt(uint32_t index, std::string& name, std::string& value);
  const std::string& getBody();
  bool isRequest() const;
  bool isResponse() const;
  int getMajorVersion() const;
  int getMinorVersion() const;
  
private:
  HttpParserObject();
  virtual ~HttpParserObject();
  
  int onMessageBegin();
  int onHeadersComplete();
  int onMessageComplete();
  int onUrl(const char* at, size_t length);
  int onStatus(const char* at, size_t length);
  int onHeaderField(const char* at, size_t length);
  int onHeaderValue(const char* at, size_t length);
  int onBody(const char* at, size_t length); 
  
  http_parser _parser;
  Headers _headers;
  HeaderNames _names;
  int _statusCode;
  std::string _method;
  std::string _url;
  std::string _status;
  std::string _body;
  std::string _currentHeader;
  bool _messageComplete;
  bool _headersComplete;
  http_parser_type _messageType;
  int _httpMajor;
  int _httpMinor;
};


//
// Inlines
//

inline bool HttpParserObject::isMessageComplete() const
{
  return _messageComplete;
}

inline bool HttpParserObject::isHeadersComplete() const
{
  return _headersComplete;
}

inline int HttpParserObject::getMessageType() const
{
  return _messageType;
}

inline const std::string& HttpParserObject::getUrl() const
{
  return _url;
}

inline const std::string& HttpParserObject::getStatus() const
{
  return _status;
}

inline const std::string& HttpParserObject::getBody()
{
  return _body;
}

inline bool HttpParserObject::isRequest() const
{
  return _messageType == HTTP_REQUEST;
}

inline bool HttpParserObject::isResponse() const
{
  return _messageType == HTTP_RESPONSE;
}

inline const std::string& HttpParserObject::getMethod() const
{
  return _method;
}

inline int HttpParserObject::getStatusCode() const
{
  return _statusCode;
}

inline int HttpParserObject::getMajorVersion() const
{
  return _httpMajor;
}
inline int HttpParserObject::getMinorVersion() const
{
  return _httpMinor;
}

inline uint32_t HttpParserObject::getHeaderCount() const
{
  return _headers.size();
}

#endif // OSS_HTTPPARSEROBJECT_H_INCLUDED

