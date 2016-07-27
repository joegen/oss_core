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


#include "OSS/BSON/BSONInt64.h"


namespace OSS {
namespace BSON {

  

BSONInt64::BSONInt64()
{
  _type = TYPE_INT64;
  _value = int64_t(0);
}

BSONInt64::BSONInt64(int64_t value)
{
  _type = TYPE_INT64;
  _value = value;
}

BSONInt64::BSONInt64(const BSONInt64& value)
{
  _type = TYPE_INT64;
  _value = value._value;
}


BSONInt64& BSONInt64::operator=(int64_t value)
{
  _value = value;
  return *this;
}

BSONInt64& BSONInt64::operator=(const BSONInt64& value)
{
  BSONInt64 clonable(value);
  swap(clonable);
  return *this;
}


} } // OSS::BSON


