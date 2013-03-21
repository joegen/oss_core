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


#include "OSS/Persistent/DataType.h"
#include <libconfig.h++>

namespace OSS {
namespace Persistent {

DataType::DataType() : 
  _persistentvalue(0),
  _isVolatile(true),
  _volatileType(TypeNone)
{
}

DataType::~DataType()
{
}

DataType::DataType(const DataType& dataType)
{
  _persistentvalue = dataType._persistentvalue;
  _isVolatile = dataType._isVolatile;
  _volatileValue = dataType._volatileValue;
  _volatileType = dataType._volatileType;
  _persistentClass = dataType._persistentClass;
}

void DataType::swap(DataType& dataType)
{
  std::swap(_persistentvalue, dataType._persistentvalue);
  std::swap(_isVolatile, dataType._isVolatile);
  std::swap(_volatileType, dataType._volatileType);
  _volatileValue.swap(dataType._volatileValue);
  _persistentClass.swap(dataType._persistentClass);
}

DataType& DataType::operator=(const DataType& dataType)
{
  DataType clonable(dataType);
  swap(clonable);
  return *this;
}

DataType::operator bool() const
{
  try
  {
    if (_isVolatile)
    {
      return  boost::any_cast<bool>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator  bool();
    }
  }
  catch(...)
  {
    throw PersistenceException("Data Type is not bool castable");
  }
}

DataType::operator int() const
{
  try
  {
    if (_isVolatile)
    {
      return  boost::any_cast<int>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator  int();
    }
  }
  catch(...)
  {
    throw PersistenceException("Data Type is not int castable");
  }
}

DataType::operator unsigned int() const
{
  try
  {
    if (_isVolatile)
    {
      return  boost::any_cast<unsigned int>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator unsigned int();
    }
  }
  catch(...)
  {
    throw PersistenceException("Data Type is not unsigned int castable");
  }
}

DataType::operator long() const
{
  try
  {
    if (_isVolatile)
    {
      return  boost::any_cast<long>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator long();
    }
  }
  catch(...)
  {
    throw PersistenceException("Data Type is not long castable");
  }
}

DataType::operator unsigned long() const
{
  try
  {
    if (_isVolatile)
    {
      return  boost::any_cast<unsigned long>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator unsigned long();
    }
  }
  catch(...)
  {
    throw PersistenceException("Data Type is not unsigned long castable");
  }
}

DataType::operator long long() const
{
  try
  {
    if (_isVolatile)
    {
      return  boost::any_cast<long long>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator long long();
    }
  }
  catch(...)
  {
    throw PersistenceException("Data Type is not long long castable");
  }
}

DataType::operator unsigned long long() const
{
  try
  {
    if (_isVolatile)
    {
      return  boost::any_cast<unsigned long long>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator unsigned long long();
    }
  }
  catch(...)
  {
    throw PersistenceException("Data Type is not unsigned long long castable");
  }
}

DataType::operator double() const
{
  try
  {
    if (_isVolatile)
    {
      return  boost::any_cast<double>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator double();
    }
  }
  catch(...)
  {
    throw PersistenceException("Data Type is not double castable");
  }
}

DataType::operator float() const
{
  try
  {
    if (_isVolatile)
    {
      return  boost::any_cast<float>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator float();
    }
  }
  catch(...)
  {
    throw PersistenceException("Data Type is not float castable");
  }
}

DataType::operator const char*() const
{
  try
  {
    if (_isVolatile)
    {
        return  boost::any_cast<const char*>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator const char*();
    }
  }
  catch(...)
  {
    if (_isVolatile)
    {
      try
      {
        return  boost::any_cast<const std::string&>(_volatileValue).c_str();
      }
      catch(...)
      {
        throw PersistenceException("Data Type is not const char* castable");
      }
    }
    else
    {
      throw PersistenceException("Data Type is not const char* castable");
    }
  }
}

DataType::operator std::string() const
{
  try
  {
    if (_isVolatile)
    {
      return  boost::any_cast<std::string>(_volatileValue);
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      return static_cast<libconfig::Setting*>(_persistentvalue)->operator std::string();
    }
  }
  catch(...)
  {
    if (_isVolatile)
    {
      try
      {
        return  std::string(boost::any_cast<const char*>(_volatileValue));
      }
      catch(...)
      {
        throw PersistenceException("Data Type is not std::string castable");
      }
    }
    else
    {
      throw PersistenceException("Data Type is not std::string castable");
    }
  }
}

DataType& DataType::operator=(bool value)
{
  try
  {
    if (_isVolatile)
    {
      _volatileType = TypeBoolean;
      _volatileValue = value;
      return *this;
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      static_cast<libconfig::Setting*>(_persistentvalue)->operator=(value);
      return *this;
    }
  }
  catch(...)
  {
    throw PersistenceException("Copy operation failed for type bool");
  }
}

DataType& DataType::operator=(int value)
{
  try
  {
    if (_isVolatile)
    {
      _volatileType = TypeInt;
      _volatileValue = value;
      return *this;
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      static_cast<libconfig::Setting*>(_persistentvalue)->operator=(value);
      return *this;
    }
  }
  catch(...)
  {
    throw PersistenceException("Copy operation failed for type int");
  }
}

DataType& DataType::operator=(long value)
{
  try
  {
    if (_isVolatile)
    {
      _volatileType = TypeInt;
      _volatileValue = value;
      return *this;
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      static_cast<libconfig::Setting*>(_persistentvalue)->operator=(value);
      return *this;
    }
  }
  catch(...)
  {
    throw PersistenceException("Copy operation failed for type long");
  }
}

DataType& DataType::operator=(const long long &value)
{
  try
  {
    if (_isVolatile)
    {
      _volatileType = TypeInt64;
      _volatileValue = value;
      return *this;
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      static_cast<libconfig::Setting*>(_persistentvalue)->operator=(value);
      return *this;
    }
  }
  catch(...)
  {
    throw PersistenceException("Copy operation failed for type long long");
  }
}

DataType& DataType::operator=(const double &value)
{
  try
  {
    if (_isVolatile)
    {
      _volatileType = TypeFloat;
      _volatileValue = value;
      return *this;
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      static_cast<libconfig::Setting*>(_persistentvalue)->operator=(value);
      return *this;
    }
  }
  catch(...)
  {
    throw PersistenceException("Copy operation failed for type double");
  }
}

DataType& DataType::operator=(float value)
{
  try
  {
    if (_isVolatile)
    {
      _volatileType = TypeFloat;
      _volatileValue = value;
      return *this;
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      static_cast<libconfig::Setting*>(_persistentvalue)->operator=(value);
      return *this;
    }
  }
  catch(...)
  {
    throw PersistenceException("Copy operation failed for type float");
  }
}

DataType& DataType::operator=(const char *value)
{
  try
  {
    if (_isVolatile)
    {
      _volatileType = TypeString;
      _volatileValue = value;
      return *this;
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      static_cast<libconfig::Setting*>(_persistentvalue)->operator=(value);
      return *this;
    }
  }
  catch(...)
  {
    throw PersistenceException("Copy operation failed for type const char*");
  }
}

DataType& DataType::operator=(const std::string &value)
{
  try
  {
    if (_isVolatile)
    {
      _volatileType = TypeString;
      _volatileValue = value;
      return *this;
    }
    else
    {
      OSS_VERIFY_NULL(_persistentvalue);
      static_cast<libconfig::Setting*>(_persistentvalue)->operator=(value);
      return *this;
    }
  }
  catch(...)
  {
    throw PersistenceException("Copy operation failed for type std::string");
  }
}

DataType DataType::operator[](const char * key) const
{
  try
  {
    OSS_VERIFY_NULL(_persistentvalue);
    DataType dataType;
    dataType._isVolatile = false;
    libconfig::Setting& setting = static_cast<libconfig::Setting*>(_persistentvalue)->operator[](key);
    dataType._persistentvalue = &setting;
    dataType._persistentClass = _persistentClass;
    return dataType;
  }
  catch(...)
  {
    throw PersistenceException("DataType::operator[](const char * key) failed");
  }
}

DataType DataType::operator[](int index) const
{
  try
  {
    OSS_VERIFY_NULL(_persistentvalue);
    DataType dataType;
    dataType._isVolatile = false;
    libconfig::Setting& setting = static_cast<libconfig::Setting*>(_persistentvalue)->operator[](index);
    dataType._persistentvalue = &setting;
    dataType._persistentClass = _persistentClass;
    return dataType;
  }
  catch(...)
  {
    throw PersistenceException("DataType::operator[](int index) failed");
  }
}

bool DataType::get(const char *name, bool& value) const
{
  if (_isVolatile)
    return false;
  OSS_VERIFY_NULL(_persistentvalue);
  return static_cast<libconfig::Setting*>(_persistentvalue)->lookupValue(name, value);
}

bool DataType::get(const char *name, int& value) const
{
  if (_isVolatile)
    return false;
  OSS_VERIFY_NULL(_persistentvalue);
  return static_cast<libconfig::Setting*>(_persistentvalue)->lookupValue(name, value);
}

bool DataType::get(const char *name, unsigned int& value) const
{
  if (_isVolatile)
    return false;
  OSS_VERIFY_NULL(_persistentvalue);
  return static_cast<libconfig::Setting*>(_persistentvalue)->lookupValue(name, value);
}

bool DataType::get(const char *name, long long& value) const
{
  if (_isVolatile)
    return false;
  OSS_VERIFY_NULL(_persistentvalue);
  return static_cast<libconfig::Setting*>(_persistentvalue)->lookupValue(name, value);
}

bool DataType::get(const char *name, unsigned long long& value) const
{
  if (_isVolatile)
    return false;
  OSS_VERIFY_NULL(_persistentvalue);
  return static_cast<libconfig::Setting*>(_persistentvalue)->lookupValue(name, value);
}

bool DataType::get(const char *name, double& value) const
{
  if (_isVolatile)
    return false;
  OSS_VERIFY_NULL(_persistentvalue);
  return static_cast<libconfig::Setting*>(_persistentvalue)->lookupValue(name, value);
}

bool DataType::get(const char *name, float& value) const
{
  if (_isVolatile)
    return false;
  OSS_VERIFY_NULL(_persistentvalue);
  return static_cast<libconfig::Setting*>(_persistentvalue)->lookupValue(name, value);
}

bool DataType::get(const char *name, const char*& value) const
{
  if (_isVolatile)
    return false;
  OSS_VERIFY_NULL(_persistentvalue);
  return static_cast<libconfig::Setting*>(_persistentvalue)->lookupValue(name, value);
}

bool DataType::get(const char *name, std::string& value) const
{
  if (_isVolatile)
    return false;
  OSS_VERIFY_NULL(_persistentvalue);
  return static_cast<libconfig::Setting*>(_persistentvalue)->lookupValue(name, value);
}

int DataType::getElementCount() const
{
  if (_isVolatile)
    return 0;
  OSS_VERIFY_NULL(_persistentvalue);
  return const_cast<const libconfig::Setting&>(*static_cast<libconfig::Setting*>(_persistentvalue)).getLength();
}

DataType DataType::addGroupElement(const char* name, Type type)
{
  if (_isVolatile)
    throw PersistenceException("Attempt to add an element to a volatile object.");
  OSS_VERIFY_NULL(_persistentvalue);
  try
  {
    libconfig::Setting& setting = static_cast<libconfig::Setting*>(_persistentvalue)->add(name, (libconfig::Setting::Type)(int)type);
    DataType dataType;
    dataType._isVolatile = false;
    dataType._persistentvalue = &setting;
    return dataType;
  }
  catch(...)
  {
    throw PersistenceException("DataType::addGroupElement(const char* name, Type type) FAILED");
  }
}

DataType DataType::addArrayOrListElement(Type type)
{
  if (_isVolatile)
    throw PersistenceException("Attempt to add an element to a volatile object.");
  OSS_VERIFY_NULL(_persistentvalue);
  try
  {
    libconfig::Setting& setting =  static_cast<libconfig::Setting*>(_persistentvalue)->add((libconfig::Setting::Type)(int)type);
    DataType dataType;
    dataType._isVolatile = false;
    dataType._persistentvalue = &setting;
    return dataType;
  }
  catch(...)
  {
    throw PersistenceException("DataType::addArrayOrListElement(Type type) FAILED");
  }
}

bool DataType::exists(const char* name) const
{
   if (_isVolatile)
     return false;
   return const_cast<const libconfig::Setting&>(*static_cast<libconfig::Setting*>(_persistentvalue)).exists(name);
}


} }  // OSS::Persistent
