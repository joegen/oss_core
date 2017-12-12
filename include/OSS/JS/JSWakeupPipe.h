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

#ifndef OSS_JSWAKEUPPIPE_H_INCLUDED
#define OSS_JSWAKEUPPIPE_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_V8

#include "v8.h"


#include <unistd.h>
#include <boost/noncopyable.hpp>


namespace OSS {
namespace JS {

class JSWakeupPipe : boost::noncopyable
{
public:
  JSWakeupPipe();
  ~JSWakeupPipe();
  void wakeup();
  void clearOne();
  int getFd();
  
protected:
  int _wakeupPipe[2];
};

//
// Inlines
//

inline JSWakeupPipe::JSWakeupPipe()
{
  pipe(_wakeupPipe);
}

inline JSWakeupPipe::~JSWakeupPipe()
{
  close(_wakeupPipe[0]);
  close(_wakeupPipe[1]);
}

inline int JSWakeupPipe::getFd()
{
  return _wakeupPipe[0];
}

inline void JSWakeupPipe::wakeup()
{
  std::size_t w = 0;
  w = write(_wakeupPipe[1], " ", 1);
  (void)w;
}

inline void JSWakeupPipe::clearOne()
{
  std::size_t r = 0;
  char buf[1];
  r = read(_wakeupPipe[0], buf, 1);
  (void)r;
}

} } // OSS::JS


#endif // ENABLE_FEATURE_V8
#endif // OSS_JSWAKEUPPIPE_H_INCLUDED

