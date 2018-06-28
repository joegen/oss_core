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

#ifndef OSS_JS_ESLEVENTOBJECT_H_INCLUDED
#define OSS_JS_ESLEVENTOBJECT_H_INCLUDED


#include "OSS/JS/JSPlugin.h"
#include "OSS/Net/ESLEvent.h"

class ESLEventObject: public OSS::JS::JSObjectWrap
{
public:
  JS_CONSTRUCTOR_DECLARE();
   
  JS_METHOD_DECLARE(create);
  JS_METHOD_DECLARE(data);
  JS_METHOD_DECLARE(getHeader);
  JS_METHOD_DECLARE(getBody);
  JS_METHOD_DECLARE(getEventName);
  JS_METHOD_DECLARE(setBody);
  JS_METHOD_DECLARE(addHeader);
  JS_METHOD_DECLARE(pushHeader);
  JS_METHOD_DECLARE(unshiftHeader);
  JS_METHOD_DECLARE(removeHeader);
  JS_METHOD_DECLARE(first);
  JS_METHOD_DECLARE(next);
  JS_METHOD_DECLARE(setPriority);
  JS_METHOD_DECLARE(isValid);
  
protected:
  ESLEventObject();
  virtual ~ESLEventObject();
  OSS::Net::ESLEvent::Ptr _event;  
};


#endif // OSS_JS_ESLEVENTOBJECT_H_INCLUDED

