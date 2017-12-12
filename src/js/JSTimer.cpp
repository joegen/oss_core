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


#include "OSS/JS/JSTimerManager.h"
#include "OSS/JS/JSTimer.h"


namespace OSS {
namespace JS {

JSTimer::JSTimer(JSTimerManager* pManager, int id, int expire, const v8::Handle<v8::Value>& callback) :
  JSFunctionCallback(callback),
  _pManager(pManager)
{
  _timer = OSS::net_io_timer_create(expire, boost::bind(&JSTimer::onTimerExpire, this));
  _id = id;
}

JSTimer::JSTimer(JSTimerManager* pManager, int id, int expire, const v8::Handle<v8::Value>& callback, const v8::Handle<v8::Value>& args) :
  JSFunctionCallback(callback, args),
  _pManager(pManager)
{
  _timer = OSS::net_io_timer_create(expire, boost::bind(&JSTimer::onTimerExpire, this));
  _id = id;
}

JSTimer::~JSTimer()
{
}

void JSTimer::onTimerExpire()
{
  JSTimer::Ptr pTimer = _pManager->removeTimer(_id);
  if(pTimer)
  {
    _pManager->execute(pTimer);
  }
}

int JSTimer::getIdentifier() const
{
  return _id;
}

int JSTimer::getExpireTime() const
{
  return _expire;
}

void JSTimer::cancel()
{
   OSS::net_io_timer_cancel(_timer);
}


} } // OSS::JS
