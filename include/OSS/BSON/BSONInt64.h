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

#ifndef OSS_BSONINT64_H_INCLUDED
#define OSS_BSONINT64_H_INCLUDED


#include "OSS/BSON/BSONValue.h"


namespace OSS {
namespace BSON {

  
class BSONInt64 : public BSONValue
{
public:
  BSONInt64();
  explicit BSONInt64(int64_t value);
  explicit BSONInt64(const BSONInt64& value);
  void swap(BSONInt64& value);
  BSONInt64& operator=(int64_t value);
  BSONInt64& operator=(const BSONInt64& value);
  int64_t& value();
  const int64_t& value() const;
};

//
// Inlines
//
inline void BSONInt64::swap(BSONInt64& value)
{
  std::swap(_value, value._value);
}

inline int64_t& BSONInt64::value()
{
  return asInt64();
}

inline const int64_t& BSONInt64::value() const
{
  return asInt64();
}

} } // OSS::BSON


#endif // OSS_BSONINT64_H_INCLUDED

