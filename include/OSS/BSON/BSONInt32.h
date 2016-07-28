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

#ifndef OSS_BSONINT32_H_INCLUDED
#define OSS_BSONINT32_H_INCLUDED


#include "OSS/BSON/BSONValue.h"


namespace OSS {
namespace BSON {

  
class BSONInt32 : public BSONValue
{
public:
  BSONInt32();
  BSONInt32(const BSONInt32& value);
  explicit BSONInt32(int32_t value);
  void swap(BSONInt32& value);
  BSONInt32& operator=(int32_t value);
  BSONInt32& operator=(const BSONInt32& value);
  int32_t& value();
  const int32_t& value() const;
};

//
// Inlines
//
inline void BSONInt32::swap(BSONInt32& value)
{
  std::swap(_value, value._value);
}

inline int32_t& BSONInt32::value()
{
  return asInt32();
}

inline const int32_t& BSONInt32::value() const
{
  return asInt32();
}

} } // OSS::BSON


#endif // OSS_BSONINT32_H_INCLUDED

