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

#ifndef OSS_JS_HTTPREQUESTOBJECT_H_INCLUDED
#define OSS_JS_HTTPREQUESTOBJECT_H_INCLUDED


#include <Poco/Net/HTTPRequest.h>
#include "OSS/JS/JSPlugin.h"

class HttpRequestObject: public OSS::JS::ObjectWrap
{
public:
  typedef Poco::Net::HTTPRequest Request;
  typedef Poco::Net::NameValueCollection Cookies;
  
  JS_CONSTRUCTOR_DECLARE();

  //
  // Methods
  //
  JS_METHOD_DECLARE(setMethod);
  JS_METHOD_DECLARE(getMethod);
  JS_METHOD_DECLARE(setUri);
  JS_METHOD_DECLARE(getUri);
  JS_METHOD_DECLARE(setHost);
  JS_METHOD_DECLARE(getHost);
  JS_METHOD_DECLARE(setCookies);
  JS_METHOD_DECLARE(getCookies);
  JS_METHOD_DECLARE(hasCredentials);
  JS_METHOD_DECLARE(getCredentials);
  JS_METHOD_DECLARE(setCredentials);
  
  JS_METHOD_DECLARE(setVersion);
  JS_METHOD_DECLARE(getVersion);
  JS_METHOD_DECLARE(setContentLength);
  JS_METHOD_DECLARE(getContentLength);
  JS_METHOD_DECLARE(setTransferEncoding);
  JS_METHOD_DECLARE(getTransferEncoding);
  JS_METHOD_DECLARE(setChunkedTransferEncoding);
  JS_METHOD_DECLARE(getChunkedTransferEncoding);
  JS_METHOD_DECLARE(setContentType);
  JS_METHOD_DECLARE(getContentType);
  JS_METHOD_DECLARE(setKeepAlive);
  JS_METHOD_DECLARE(getKeepAlive);

  JS_METHOD_DECLARE(reset);
  JS_METHOD_DECLARE(dispose);
  
  Request* request();
private:
  HttpRequestObject();
  ~HttpRequestObject();
  Request* _request;
};

//
// Inlines
//
HttpRequestObject::Request* HttpRequestObject::request()
{
  return _request;
}
  
#endif /* HTTPREQUESTOBJECT_H */

