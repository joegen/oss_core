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


#include "OSS/BSON/bson.h"
#include "OSS/BSON/BSONValue.h"


namespace OSS {
namespace BSON {
  
const int BSONValue::TYPE_STRING = BSON_TYPE_UTF8;
const int BSONValue::TYPE_INT32 = BSON_TYPE_INT32;
const int BSONValue::TYPE_INT64 = BSON_TYPE_INT64;
const int BSONValue::TYPE_BOOL = BSON_TYPE_BOOL;
const int BSONValue::TYPE_DOUBLE = BSON_TYPE_DOUBLE;
const int BSONValue::TYPE_DOCUMENT = BSON_TYPE_DOCUMENT;
const int BSONValue::TYPE_ARRAY = BSON_TYPE_ARRAY;
const int BSONValue::TYPE_UNDEFINED = BSON_TYPE_UNDEFINED;
 
 
BSONValue::BSONValue()
{
  _type = TYPE_UNDEFINED;
}

BSONValue::BSONValue(int type)
{
  _type = type;
  if (_type == TYPE_ARRAY)
  {
    _value = Array();
  }
  else if (_type == TYPE_DOCUMENT)
  {
    _value = Document();
  }
  else if (_type == TYPE_STRING)
  {
    _value = std::string();
  }
  else if (_type == TYPE_BOOL)
  {
    _value = false;
  }
  else if (_type == TYPE_DOUBLE)
  {
    _value = double(0.00);
  }
  else if (_type == TYPE_INT32)
  {
    _value = int32_t(0);
  }
  else if (_type == TYPE_INT64)
  {
    _value = int64_t(0);
  }
  else
  {
    _type = TYPE_UNDEFINED;
  }
}

BSONValue::BSONValue(const BSONValue& value)
{
  _type = value._type;
  _value = value._value;
}

BSONValue::BSONValue(int type, boost::any value)
{
  _type = TYPE_STRING;
  _value = value;
}

BSONValue::BSONValue(int type, const char* value)
{
  _type = TYPE_STRING;
  _value = std::string(value);
}

BSONValue::BSONValue(int type, bool value)
{
  _type = TYPE_BOOL;
  _value = value;
}

BSONValue::BSONValue(int type, int32_t value)
{
  _type = TYPE_INT32;
  _value = value;
}

BSONValue::BSONValue(int type, int64_t value)
{
  _type = TYPE_INT64;
  _value = value;
}

BSONValue::BSONValue(int type, double value)
{
  _type = TYPE_DOUBLE;
  _value = value;
}

BSONValue::BSONValue(const Document& value)
{
  _type = TYPE_DOCUMENT;
  _value = value;
}

BSONValue::BSONValue(const Array& value)
{
  _type = TYPE_ARRAY;
  _value = value;
}

BSONValue::~BSONValue()
{
}

void BSONValue::setValue(const Document& value)
{
  _type = TYPE_DOCUMENT;
  _value = value;
}

void BSONValue::setValue(const Array& value)
{
  _type = TYPE_ARRAY;
  _value = value;
}

void BSONValue::setValue(const BSONValue& value)
{
  _type = value._type;
  _value = value._value;
}

void BSONValue::setValue(const std::string& value)
{
  _type = TYPE_STRING;
  _value = value;
}

void BSONValue::setValue(const char* value)
{
  _type = TYPE_STRING;
  _value = std::string(value);
}

void BSONValue::setValue(bool value)
{
  _type = TYPE_BOOL;
  _value = value;
}

void BSONValue::setValue(int32_t value)
{
  _type = TYPE_INT32;
  _value = value;
}

void BSONValue::setValue(int64_t value)
{
  _type = TYPE_INT64;
  _value = value;
}

void BSONValue::setValue(double value)
{
  _type = TYPE_DOUBLE;
  _value = value;
}

#if 0

const BSONValue& BSONValue::operator[] (const std::string& key) const
{
  static BSONValue undefined;
  if (_type == TYPE_DOCUMENT)
  {
    const Document& document = boost::any_cast<const Document&>(const_cast<BSONValue*>(this)->_value);
    Document::const_iterator iter = document.find(key);
    if (iter != document.end())
    {
      return iter->second;
    }
  }
  return undefined;
}

#endif

BSONValue& BSONValue::operator[] (std::size_t index)
{
  static BSONValue undefined;
  
  if (_type == TYPE_ARRAY)
  {
    Array& array = boost::any_cast<Array&>(_value);
    
    if (index < array.size())
    {
      return array[index];
    }
    else
    {
      array.push_back(undefined);
      while(index >= array.size())
      {
        array.push_back(undefined);
      }
      return array[index];
    }
  }
  return undefined;
}

std::size_t BSONValue::size() const
{
  if (_type == TYPE_ARRAY)
  {
     return boost::any_cast<const Array&>(const_cast<BSONValue*>(this)->_value).size();
  }
  else if (_type == TYPE_DOCUMENT)
  {
     return boost::any_cast<const Document&>(const_cast<BSONValue*>(this)->_value).size();
  }
  return 0;
}

} } // OSS::BSON




