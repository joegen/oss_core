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

#ifndef OSS_JSEPERSISTENT_H_INCLUDED
#define OSS_JSEPERSISTENT_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_V8


#include <v8.h>
#include <boost/core/addressof.hpp>


namespace OSS {
namespace JS {


template <typename T>
class JSPersistentValue
{
public:
  typedef v8::Persistent<T> Value;
  
  JSPersistentValue()
  {
  }

  JSPersistentValue(const JSPersistentValue& value)
  {
    _value = value._value;
  }

  JSPersistentValue(const Value& value)
  {
    _value = value;
  }

  ~JSPersistentValue()
  {
    dispose();
  }

  JSPersistentValue& operator=(const JSPersistentValue& value)
  {
    if (boost::addressof(value) == this)
    {
      return *this;
    }
    dispose();
    _value = value._value;
    return *this;
  }

  JSPersistentValue& operator=(const Value& value)
  {
    dispose();
    _value = value;
    return *this;
  }
  
  const Value& value() const
  {
    return _value;
  }

  Value& value()
  {
    return _value;
  }

  void dispose()
  {
    if (!_value.IsEmpty())
    {
      _value.Dispose();
    }
  }
private:
  Value _value;
};


} }

#endif // ENABLE_FEATURE_V8
#endif // JSPERSISTENTVALUE_H

