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

#ifndef OSS_BSONSTRING_H_INCLUDED
#define OSS_BSONSTRING_H_INCLUDED


#include "OSS/BSON/BSONValue.h"


namespace OSS {
namespace BSON {

  
class BSONString : public BSONValue
{
public:
  BSONString();
  explicit BSONString(const char* value);
  explicit BSONString(const std::string& value);
  explicit BSONString(const BSONString& value);
  void swap(BSONString& value);
  BSONString& operator=(const char* value);
  BSONString& operator=(const std::string& value);
  BSONString& operator=(const BSONString& value);
  std::string& value();
  const std::string& value() const;
};

//
// Inlines
//

inline void BSONString::swap(BSONString& value)
{
  std::swap(_value, value._value);
}

inline std::string& BSONString::value()
{
  return asString();
}

inline const std::string& BSONString::value() const
{
  return asString();
}

} } // OSS::BSON


#endif // OSS_BSONSTRING_H_INCLUDED

