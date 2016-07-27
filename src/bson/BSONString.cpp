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


#include "OSS/BSON/BSONString.h"


namespace OSS {
namespace BSON {

  

BSONString::BSONString()
{
  _type = TYPE_STRING;
  _value = std::string();
}

BSONString::BSONString(const char* value)
{
  _type = TYPE_STRING;
  _value = std::string(value);
}

BSONString::BSONString(const std::string& value)
{
  _type = TYPE_STRING;
  _value = value;
}

BSONString::BSONString(const BSONString& value)
{
  _type = TYPE_STRING;
  _value = value._value;
}

BSONString& BSONString::operator=(const char* value)
{
  _value = std::string(value);
  return *this;
}

BSONString& BSONString::operator=(const std::string& value)
{
  _value = value;
  return *this;
}

BSONString& BSONString::operator=(const BSONString& value)
{
  BSONString clonable(value);
  swap(clonable);
  return *this;
}



} } // OSS::BSON


