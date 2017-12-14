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

#ifndef OSS_JSINTERISOLATE_CALL_H_INCLUDED
#define OSS_JSINTERISOLATE_CALL_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_V8


#include "OSS/JS/JS.h"
#include "OSS/JSON/Json.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/shared_ptr.hpp>


namespace OSS {
namespace JS {

class JSInterIsolateCall : protected boost::promise<std::string>
{
public:
  typedef boost::shared_ptr<JSInterIsolateCall> Ptr;
  typedef OSS::JSON::Object Request;
  typedef OSS::JSON::Object Result;
  typedef boost::future<std::string> Future;
  ~JSInterIsolateCall();
  
  void setRequest(const Request& request);
  const Request& getRequest() const;
  bool setResult(const std::string& json);
  void setResult(const Result& result);
  const Result& getResult() const;
  void setUserData(void* userData);
  void* getUserData() const;
  void setTimeout(uint32_t timeout);
  uint32_t getTimeout() const;
  bool waitForResult();
  std::string json() const;

protected:
  JSInterIsolateCall();
  JSInterIsolateCall(const Request& request, uint32_t timeout, void* userData);
  void setValue(const std::string& value);
  Request _request;
  Result _result;
  void* _userData;
  uint32_t _timeout;
  Future _future;
  friend class JSInterIsolateCallManager;
};

//
// Inlines
//


inline JSInterIsolateCall::JSInterIsolateCall() :
  _userData(0),
  _timeout(0)
{
  _future = get_future();
}

inline JSInterIsolateCall::JSInterIsolateCall(const Request& request, uint32_t timeout, void* userData) :
  _request(request),
  _userData(userData),
  _timeout(timeout)
{
   _future = get_future();
}

inline JSInterIsolateCall::~JSInterIsolateCall()
{
}

inline void JSInterIsolateCall::setRequest(const Request& request)
{
  _request = request;
}

inline const JSInterIsolateCall::Request& JSInterIsolateCall::getRequest() const
{
  return _request;
}

inline bool JSInterIsolateCall::setResult(const std::string& json)
{
  return OSS::JSON::json_parse_string(json, _result);
}

inline void JSInterIsolateCall::setResult(const Result& result)
{
  _result = result;
}

inline const JSInterIsolateCall::Result& JSInterIsolateCall::getResult() const
{
  return _result;
}

inline void JSInterIsolateCall::setUserData(void* userData)
{
  _userData = userData;
}

inline void* JSInterIsolateCall::getUserData() const
{
  return _userData;
}

inline void JSInterIsolateCall::setTimeout(uint32_t timeout)
{
  _timeout = timeout;
}

inline uint32_t JSInterIsolateCall::getTimeout() const
{
  return _timeout;
}

inline bool JSInterIsolateCall::waitForResult()
{
  if (_timeout)
  {
    boost::system_time const future_timeout = boost::get_system_time() + boost::posix_time::milliseconds(_timeout);
    if (!_future.timed_wait_until(future_timeout))
    {
      return false;
    }
  }
  return setResult(_future.get());
}

inline std::string JSInterIsolateCall::json() const
{
  std::string json;
  OSS::JSON::json_object_to_string(_request, json);
  return json;
}

inline void JSInterIsolateCall::setValue(const std::string& value)
{
  set_value(value);
}


 
} } // OSS::JS

#endif //  ENABLE_FEATURE_V8
#endif // OSS_JSEVENTARGUMENT_H_INCLUDED

