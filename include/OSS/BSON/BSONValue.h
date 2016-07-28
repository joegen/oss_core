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
#include "OSS/BSON/BSONObject.h"
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
  BSONValue(const BSONValue& value);
  ~BSONValue();
  
  //
  // Setters
  //
  void setValue(const BSONValue& value);
  void swap(BSONValue& value);
  BSONValue& operator=(const BSONValue& value);

  
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
  operator const std::string&() const { return boost::any_cast<const std::string&>(_value); };
  operator const bool&() const { return boost::any_cast<const bool&>(_value); };
  operator const int32_t&() const { return boost::any_cast<const int32_t&>(_value); };
  operator const int64_t&() const { return boost::any_cast<const int64_t&>(_value); };
  operator const double&() const { return boost::any_cast<const double&>(_value); };
  operator const Array&() const { return boost::any_cast<const Array&>(_value); };
  operator const Document&() const { return boost::any_cast<const Document&>(_value); };
  
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
  const BSONValue& operator[] (const char* key) const;
  BSONValue& operator[] (const char* key);
  const BSONValue& operator[] (const std::string& key) const;
  BSONValue& operator[] (const std::string& key);
  
  //
  // Operator [] for arrays
  //
  const BSONValue& operator[] (int index) const;
  BSONValue& operator[] (int index);
  const BSONValue& operator[] (std::size_t index) const;
  BSONValue& operator[] (std::size_t index);
  
  
  int getType() const;
  bool isType(int type) const;
  std::size_t size() const;
  
  //
  // Serializing to JSON
  //
  void toJSON(std::ostream& strm) const;
  std::string toJSON() const;
  
  //
  // Serializing to BSON
  //
  void toBSON(BSONObject& bson) const;
  BSONObject toBSON() const;
  
  //
  // serialize from BSON
  //
  void fromBSON(const BSONObject& bson);
  
protected:
  void serializeDocument(const Document& document, std::ostream& strm) const;
  void serializeArray(const Array& array, std::ostream& strm) const;
  void serializeDocument(const std::string& key, const Document& document, BSONObject& bson) const;
  void serializeArray(const std::string& key, const Array& array, BSONObject& bson) const;
  
  int _type;
  mutable boost::any _value;
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

