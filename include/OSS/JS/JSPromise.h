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

#ifndef OSS_JSPROMISE_H_INCLUDED
#define OSS_JSPROMISE_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_V8

#include "v8.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/JSON/Json.h"

#include <boost/thread.hpp>
#include <boost/thread/future.hpp>

namespace OSS {
namespace JS {

  
class JSPromise : boost::promise<std::string>
{
public:
  typedef boost::future<std::string> Future;
  JSPromise();
  JSPromise(const std::string& data, void* userData);
  ~JSPromise();
  void setData(const std::string& data);
  const std::string& getData() const;
  void setUserData(void* userData);
  void* getUserData() const;
protected:
  std::string _data;
  void* _userData;
};
  
//
// Inlines
//
inline inline JSPromise::JSPromise() :
  _userData(0)
{
}

inline JSPromise::JSPromise(const std::string& data, void* userData) :
  _data(data),
  _userData(userData)
{
}

inline JSPromise::~JSPromise()
{
}

inline void JSPromise::setData(const std::string& data)
{
  _data = data;
}

inline const std::string& JSPromise::getData() const
{
  return _data;
}

inline void JSPromise::setUserData(void* userData)
{
  _userData = userData;
}

inline void* JSPromise::getUserData() const
{
  return _userData;
}


} }

#endif // ENABLE_FEATURE_V8
#endif // OSS_JSPROMISE_H_INCLUDED

