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
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Exception.h>
#include <Poco/Net/NetException.h>
#include "OSS/JS/JSPlugin.h"

class HttpClientObject: public OSS::JS::JSObjectWrap
{
public:
  typedef Poco::Net::HTTPClientSession Session;
  typedef Poco::Net::HTTPSClientSession SecureSession;
  typedef Poco::Net::MessageException MessageException;
  typedef Poco::NoThreadAvailableException NoThreadAvailableException;
  
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
  JS_METHOD_DECLARE(setKeepAliveTimeout);
  JS_METHOD_DECLARE(getKeepAliveTimeout);
  JS_METHOD_DECLARE(sendRequest);
  JS_METHOD_DECLARE(receiveResponse);
  JS_METHOD_DECLARE(connected);
  JS_METHOD_DECLARE(secure);
  JS_METHOD_DECLARE(dispose);
  
  JS_METHOD_DECLARE(read);
  JS_METHOD_DECLARE(write);
  
  JS_METHOD_DECLARE(setEventFd);
  int getEventFd() const;
  std::istream* getInputStream();
  std::ostream* getOutputStream();
  void setInputStream(std::istream* strm);
  void setOutputStream(std::ostream* strm);

protected:
  HttpClientObject(bool isSecure);
  virtual ~HttpClientObject();
  Session* _session;
  std::istream* _input;
  std::ostream* _output;
  int _eventFd;
  bool _isSecure;
  
  friend class ResponseReceiver;
  friend class RequestSender;
};

//
// Inlines
//
inline int HttpClientObject::getEventFd() const
{
  return _eventFd;
}

inline std::istream* HttpClientObject::getInputStream()
{
  return _input;
}

inline void HttpClientObject::setInputStream(std::istream* strm)
{
  _input = strm;
}

inline void HttpClientObject::setOutputStream(std::ostream* strm)
{
  _output = strm;
}

inline std::ostream* HttpClientObject::getOutputStream()
{
  return _output;
}

#endif //OSS_JS_HTTPCLIENTOBJECT_H_INCLUDED

