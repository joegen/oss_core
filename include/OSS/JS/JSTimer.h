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

#ifndef OSS_JSTIMER_H_INCLUDED
#define OSS_JSTIMER_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_V8
#include "OSS/JS/JS.h"
#include "OSS/JS/JSFunctionCallback.h"
#include "OSS/Net/Net.h"


namespace OSS {
namespace JS {


class JSTimerManager;

class JSTimer : public JSFunctionCallback
{
public:
  ~JSTimer();
  
protected:
  JSTimer(JSTimerManager* pManager, int id, int expire, const v8::Handle<v8::Value>& callback, const v8::Handle<v8::Value>& args);
  void onTimerExpire();
  void cancel();
  int getIdentifier() const;
  int getExpireTime() const;
  int _expire;
  int _id;
  OSS::NET_TIMER_HANDLE _timer;
  JSTimerManager* _pManager;
  friend class JSTimerManager;
};


} } // OSS::JS


#endif // ENABLE_FEATURE_V8
#endif // OSS_JSTIMER_H_INCLUDED

