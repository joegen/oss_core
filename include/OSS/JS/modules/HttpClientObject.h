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

#ifndef OSS_JS_HTTPCLIENTOBJECT_H_INCLUDED
#define OSS_JS_HTTPCLIENTOBJECT_H_INCLUDED

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/NetException.h>
#include "OSS/JS/JSPlugin.h"

class HttpClientObject: public OSS::JS::ObjectWrap
{
public:
  typedef Poco::Net::HTTPClientSession Session;
  typedef Poco::Net::MessageException MessageException;
  
  JS_CONSTRUCTOR_DECLARE();

  //
  // Methods
  //
  JS_METHOD_DECLARE(setHost);
  JS_METHOD_DECLARE(getHost);
  JS_METHOD_DECLARE(setPort);
  JS_METHOD_DECLARE(getPort);
  JS_METHOD_DECLARE(setProxyHost);
  JS_METHOD_DECLARE(setProxyPort);
  JS_METHOD_DECLARE(getProxyHost);
  JS_METHOD_DECLARE(getProxyPort);
  JS_METHOD_DECLARE(setProxyUsername);
  JS_METHOD_DECLARE(getProxyUsername);
  JS_METHOD_DECLARE(setProxyPassword);
  JS_METHOD_DECLARE(getProxyPassword);
  JS_METHOD_DECLARE(setKeepAliveTimeout);
  JS_METHOD_DECLARE(getKeepAliveTimeout);
  JS_METHOD_DECLARE(sendRequest);
  JS_METHOD_DECLARE(receiveResponse);
  JS_METHOD_DECLARE(peekResponse);
  JS_METHOD_DECLARE(reset);
  JS_METHOD_DECLARE(secure);
  JS_METHOD_DECLARE(bypassProxy);
  JS_METHOD_DECLARE(connected);
  JS_METHOD_DECLARE(getSocketFd);
  
  JS_METHOD_DECLARE(read);
  JS_METHOD_DECLARE(write);
  
private:
  HttpClientObject();
  virtual ~HttpClientObject();
  Session _session;
  std::istream* _input;
  std::ostream* _output;
};

#endif //OSS_JS_HTTPCLIENTOBJECT_H_INCLUDED

