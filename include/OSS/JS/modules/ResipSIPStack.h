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

#ifndef OSS_RESIPSIPSTACK_H_INCLUDED
#define OSS_RESIPSIPSTACK_H_INCLUDED


#include <v8.h>
#include <vector>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSEventArgument.h"

#include <resip/stack/SipStack.hxx>
#include <resip/stack/StackThread.hxx>
#include <rutil/SharedPtr.hxx>

class ResipSIPStack : public OSS::JS::JSObjectWrap
{
public:
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(run);
  JS_METHOD_DECLARE(shutdown);
  JS_METHOD_DECLARE(addTransport);
  resip::SipStack* stack();
private:
  ResipSIPStack();
  virtual ~ResipSIPStack();
  resip::SipStack* _stack;
};

//
// Inlines
//

inline resip::SipStack* ResipSIPStack::stack()
{
  return _stack;
}

#endif // OSS_RESIPSIPSTACK_H_INCLUDED