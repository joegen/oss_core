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

#ifndef HTTPRESPONSEOBJECT
#define HTTPRESPONSEOBJECT


#include <Poco/Net/HTTPResponse.h>
#include "OSS/JS/JSPlugin.h"

class HttpResponseObject: public OSS::JS::ObjectWrap
{
public:
  typedef Poco::Net::HTTPResponse Response;
  typedef Poco::Net::NameValueCollection Cookies;
  typedef Response::HTTPStatus Status;
  
  JS_CONSTRUCTOR_DECLARE();

  //
  // Methods
  //
  JS_METHOD_DECLARE(setStatus);
  JS_METHOD_DECLARE(getStatus);
  JS_METHOD_DECLARE(setReason);
  JS_METHOD_DECLARE(getReason);
  
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
  
  Response* response();
  
private:
  HttpResponseObject();
  ~HttpResponseObject();
  Response* _response;
};
 
//
// Inlines
//
inline HttpResponseObject::Response* HttpResponseObject::response()
{
  return _response;
}

#endif /* HTTPREQUESTOBJECT_H */

