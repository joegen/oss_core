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

#ifndef OSS_BSONBOOL_H_INCLUDED
#define OSS_BSONBOOL_H_INCLUDED


#include "OSS/BSON/BSONValue.h"


namespace OSS {
namespace BSON {

  
class BSONBool : public BSONValue
{
public:
  BSONBool();
  BSONBool(const BSONBool& value);
  explicit BSONBool(bool value);
  void swap(BSONBool& value);
  BSONBool& operator=(bool value);
  BSONBool& operator=(const BSONBool& value);
  bool& value();
  const bool& value() const;
};

//
// Inlines
//
inline void BSONBool::swap(BSONBool& value)
{
  std::swap(_value, value._value);
}

inline bool& BSONBool::value()
{
  return asBoolean();
}

inline const bool& BSONBool::value() const
{
  return asBoolean();
}

} } // OSS::BSON


#endif // OSS_BSONBOOL_H_INCLUDED

