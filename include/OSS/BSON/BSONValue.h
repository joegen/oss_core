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

#ifndef OSS_BASONVALUE_H_INCLUDED
#define OSS_BASONVALUE_H_INCLUDED

#include "OSS/OSS.h"
#include <string>
#include <map>
#include <vector>
#include <boost/any.hpp>


namespace OSS {
namespace BSON {
  
  
class BSONValue
{
public:
  static const int TYPE_STRING;
  static const int TYPE_INT32;
  static const int TYPE_INT64;
  static const int TYPE_BOOL;
  static const int TYPE_DOUBLE;
  static const int TYPE_DOCUMENT;
  static const int TYPE_ARRAY;
  static const int TYPE_UNDEFINED;  
  
  typedef std::vector<BSONValue> Array;
  typedef std::map<std::string, BSONValue> Document;
  
  BSONValue();
  explicit BSONValue(int type);
  explicit BSONValue(const BSONValue& value);
  explicit BSONValue(int type, const char* value);
  explicit BSONValue(int type, const std::string& value);
  explicit BSONValue(int type, bool value);
  explicit BSONValue(int type, int32_t value);
  explicit BSONValue(int type, int64_t value);
  explicit BSONValue(int type, double value);
  explicit BSONValue(int type, const Document& value);
  explicit BSONValue(int type, const Array& value);
  ~BSONValue();
  
  //
  // Setters
  //
  void setValue(const BSONValue& value);
  void setValue(const char* value);
  void setValue(const std::string& value);
  void setValue(bool value);
  void setValue(int32_t value);
  void setValue(int64_t value);
  void setValue(double value);
  void setValue(const Document& value);
  void setValue(const Array& value);
  
  //
  // Plain Object Getters
  //
  const std::string& asString() const;
  const bool& asBoolean() const;
  const int32_t& asInt32() const;
  const int64_t& asInt64() const;
  const double& asDouble() const;
  const Document& asDocument() const;
  const Array& asArray() const;
  
  std::string& asString();
  bool& asBoolean();
  int32_t& asInt32();
  int64_t& asInt64();
  double& asDouble();
  Document& asDocument();
  Array& asArray();

  //
  // Const Cast operators
  //
#if 0
  operator const std::string&() const { return boost::any_cast<const std::string&>(_value); };
  operator const bool&() const { return boost::any_cast<const bool&>(_value); };
  operator const int32_t&() const { return boost::any_cast<const int32_t&>(_value); };
  operator const int64_t&() const { return boost::any_cast<const int64_t&>(_value); };
  operator const double&() const { return boost::any_cast<const double&>(_value); };
  operator const Array&() const { return boost::any_cast<const Array&>(_value); };
  operator const Document&() const { return boost::any_cast<const Document&>(_value); };
#endif
  
  //
  // Reference Cast operators
  //
  operator std::string&() { return boost::any_cast<std::string&>(_value); };
  operator bool&() { return boost::any_cast<bool&>(_value); };
  operator int32_t&() { return boost::any_cast<int32_t&>(_value); };
  operator int64_t&() { return boost::any_cast<int64_t&>(_value); };
  operator double&() { return boost::any_cast<double&>(_value); };
  operator Array&() { return boost::any_cast<Array&>(_value); };
  operator Document&() { return boost::any_cast<Document&>(_value); };
  
  //
  // Operator [] for documents
  //
  //const BSONValue& operator[] (const std::string& key) const;
  
  //
  // Operator [] for arrays
  //
  BSONValue& operator[] (std::size_t index);
  
  
  int getType() const;
  bool isType(int type) const;
  std::size_t size() const;
private:
  int _type;
  boost::any _value;
};


//
// Inlines
//

inline int BSONValue::getType() const
{  
  return _type;
}

inline bool BSONValue::isType(int type) const
{
  return type == _type;
}
  
inline const std::string& BSONValue::asString() const
{
  return boost::any_cast<const std::string&>(_value);
}

inline const bool& BSONValue::asBoolean() const
{
  return boost::any_cast<const bool&>(_value);
}

inline const int32_t& BSONValue::asInt32() const
{
  return boost::any_cast<const int32_t&>(_value);
}

inline const int64_t& BSONValue::asInt64() const
{
  return boost::any_cast<const int64_t&>(_value);
}

inline const double& BSONValue::asDouble() const
{
  return boost::any_cast<const double&>(_value);
}

inline const BSONValue::Document& BSONValue::asDocument() const
{
  return boost::any_cast<const BSONValue::Document&>(_value);
}

inline const BSONValue::Array& BSONValue::asArray() const
{
  return boost::any_cast<const BSONValue::Array&>(_value);
}

inline std::string& BSONValue::asString()
{
  return boost::any_cast<std::string&>(_value);
}

inline bool& BSONValue::asBoolean()
{
  return boost::any_cast<bool&>(_value);
}

inline int32_t& BSONValue::asInt32()
{
  return boost::any_cast<int32_t&>(_value);
}

inline int64_t& BSONValue::asInt64()
{
  return boost::any_cast<int64_t&>(_value);
}

inline double& BSONValue::asDouble()
{
  return boost::any_cast<double&>(_value);
}

inline BSONValue::Document& BSONValue::asDocument()
{
  return boost::any_cast<BSONValue::Document&>(_value);
}

inline BSONValue::Array& BSONValue::asArray()
{
  return boost::any_cast<BSONValue::Array&>(_value);
}
  
} } // OSS::BSON






#endif // OSS_BASONVALUE_H_INCLUDED

