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

#ifndef OSS_JSFUNCTIONCALLBACKQUEUE_H_INCLUDED
#define OSS_JSFUNCTIONCALLBACKQUEUE_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_V8
#include "OSS/JS/JS.h"
#include "OSS/UTL/BlockingQueue.h"
#include "OSS/JS/JSFunctionCallback.h"


namespace OSS {
namespace JS {

class JSEventLoop;

class JSFunctionCallbackQueue : protected OSS::BlockingQueue<JSFunctionCallback::Ptr>
{
public:
  JSFunctionCallbackQueue(JSEventLoop* pEventLoop);
  ~JSFunctionCallbackQueue();
  
  void execute(v8::Handle<v8::Value> func, v8::Handle<v8::Value>  args);
  void execute(v8::Handle<v8::Value> func, v8::Handle<v8::Value>  args, v8::Handle<v8::Value> resultHandler);
  void execute(const JSFunctionCallback::Ptr& fun);
  bool doOneWork();
  int getFd() const;
};

} } // OSSJS


#endif // ENABLE_FEATURE_V8
#endif // OSS_JSFUNCTIONCALLBACKQUEUE_H_INCLUDED

