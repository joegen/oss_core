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


#ifndef OSS_PERSISTENT_DATATYPE_H_INCLUDED
#define OSS_PERSISTENT_DATATYPE_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_CONFIG
#if OSS_HAVE_CONFIGPP

#include <typeinfo>
#include <boost/any.hpp>

#include "OSS/OSS.h"
#include "OSS/UTL/Exception.h"
#include "OSS/Persistent/Reference.h"


namespace OSS {
namespace Persistent {

OSS_CREATE_INLINE_EXCEPTION(PersistenceException, OSS::IOException, "Persistence Exception");

class OSS_API DataType
{
public:
  enum Type
  {
    TypeNone = 0,
    // scalar types
    TypeInt,
    TypeInt64,
    TypeFloat,
    TypeString,
    TypeBoolean,
    // aggregate types
    TypeGroup,
    TypeArray,
    TypeList
  };

  DataType();
    /// Instantiate a new data type

  DataType(const DataType& dataType);
    /// Instantiate a new data type from another

  ~DataType();
    /// Destroy the data type

  void swap(DataType& dataType);
    /// Exchange the values of two data types

  operator bool() const;
    /// Cast the data type to bool.
    /// This will throw a PersistenceException if the datatype is not castable.

  operator int() const;
    /// Cast the data type to int.
    /// This will throw a PersistenceException if the datatype is not castable.

  operator unsigned int() const;
    /// Cast the data type to unsigned int.
    /// This will throw a PersistenceException if the datatype is not castable.

  operator long() const;
    /// Cast the data type to long.
    /// This will throw a PersistenceException if the datatype is not castable.
  operator unsigned long() const;
    /// Cast the data type to unsigned long.
    /// This will throw a PersistenceException if the datatype is not castable.

  operator long long() const;
    /// Cast the data type to long long.
    /// This will throw a PersistenceException if the datatype is not castable.

  operator unsigned long long() const;
    /// Cast the data type to unsigned long long.
    /// This will throw a PersistenceException if the datatype is not castable.

  operator double() const;
    /// Cast the data type to double.
    /// This will throw a PersistenceException if the datatype is not castable.

  operator float() const;
    /// Cast the data type to float.
    /// This will throw a PersistenceException if the datatype is not castable.

  operator const char*() const;
    /// Cast the data type to const char*.
    /// This will throw a PersistenceException if the datatype is not castable.

  operator std::string() const;
    /// Cast the data type to std::string.
    /// This will throw a PersistenceException if the datatype is not castable.

  DataType& operator=(bool value);
    /// Copy from a boolean type
    /// This will throw a PersistenceException if the datatype is not castable.

  DataType& operator=(int value);
    /// Copy from an int type
    /// This will throw a PersistenceException if the datatype is not castable.

  DataType& operator=(long value);
    /// Copy from a long type
    /// This will throw a PersistenceException if the datatype is not castable.

  DataType& operator=(const long long& value);
    /// Copy from a long long type
    /// This will throw a PersistenceException if the datatype is not castable.

  DataType& operator=(const double& value);
    /// Copy from a double type
    /// This will throw a PersistenceException if the datatype is not castable.

  DataType& operator=(float value);
    /// Copy from a value type
    /// This will throw a PersistenceException if the datatype is not castable.

  DataType& operator=(const char* value);
    /// Copy from a const char* type
    /// This will throw a PersistenceException if the datatype is not castable.

  DataType& operator=(const std::string& value);
    /// Copy from a std::string type
    /// This will throw a PersistenceException if the datatype is not castable.

  DataType operator[](const char* key) const;
    /// Get a member object using its name
    /// This will throw and exception if the member does not exist

  DataType operator[](const std::string& key) const;
    /// Get a member object using its name
    /// This will throw and exception if the member does not exist

  DataType operator[](int index) const;
    /// Get a member object using its index
    /// This will throw and exception if the member does not exist

  bool get(const char* name, bool& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.


  bool get(const char* name, int& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const char* name, unsigned int& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const char* name, long long& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const char* name, unsigned long long& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const char* name, double& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const char* name, float& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const char* name, const char*& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const char* name, std::string& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const std::string& name, bool& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const std::string& name, int& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const std::string& name, unsigned int& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const std::string& name, long long& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const std::string& name, unsigned long long& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const std::string& name, double& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const std::string& name, float& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const std::string& name, const char*& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  bool get(const std::string& name, std::string& value) const;
    /// These are convenience methods for looking up the value of a child setting with the
    /// given name. If the setting is found and is of an appropriate type, the value is stored
    /// in value and the method returns true. Otherwise, value is left unmodified and the
    /// method returns false. These methods do not throw exceptions.

  int getElementCount() const;
    /// This method returns the number of elements in a group, or the number of elements in
    /// a list or array. For other types, it returns 0.

  DataType addGroupElement(const std::string& name, Type type);
    /// This method adds a new child element with the given name and type to the
    /// element, which must be a group. They return a reference to the new element. If
    /// the data type already has a child element with the given name, or if the name is
    /// invalid, a PersistenceException is thrown. If the setting is not a group, a
    /// PersistenceException is thrown.
    /// Once a setting has been created, neither its name nor type can be changed.


  DataType addGroupElement(const char* name, Type type);
    /// This method adds a new child element with the given name and type to the
    /// element, which must be a group. They return a reference to the new element. If
    /// the data type already has a child element with the given name, or if the name is
    /// invalid, a PersistenceException is thrown. If the setting is not a group, a
    /// PersistenceException is thrown.
    /// Once a setting has been created, neither its name nor type can be changed.

  DataType addArrayOrListElement(Type type);
    /// This method adds a new element to the data type, which must be of type TypeArray or
    /// TypeList. If the data type is an array which currently has zero elements, the type pa-
    /// rameter (which must be TypeInt, TypeInt64, TypeFloat, TypeBool, or TypeString)
    /// determines the type for the array; otherwise it must match the type of the existing
    /// elements in the array.
    /// The method returns the new data type on success. If type is a scalar type, the new
    /// data type will have a default value of 0, 0.0, false, or NULL, as appropriate.
    /// The method throws a SettingTypeException if the setting is not an array or list, or
    /// if type is invalid.

  bool exists(const std::string& name) const;
    /// This test if the setting has a child element with the given name. It will
    /// return true if the element exists, and false otherwise. These method does not throw
    /// exceptions.

  bool exists(const char* name) const;
    /// This test if the setting has a child element with the given name. It will
    /// return true if the element exists, and false otherwise. These method does not throw
    /// exceptions.



private:
  DataType& operator=(const DataType& dataType);
    /// Copy another data type

  OSS::OSS_HANDLE _persistentvalue;
  bool _isVolatile;
  boost::any _volatileValue;
  Type _volatileType;

  Reference::Ptr _persistentClass;

  friend class ClassType;
};


//
// Inlines
//

inline DataType DataType::operator[](const std::string & key) const
{
  return(operator[](key.c_str()));
}

inline bool DataType::get(const std::string& name, bool& value) const
{
  return get(name.c_str(), value);
}

inline bool DataType::get(const std::string& name, int& value) const
{
  return get(name.c_str(), value);
}

inline bool DataType::get(const std::string& name, unsigned int& value) const
{
  return get(name.c_str(), value);
}

inline bool DataType::get(const std::string& name, long long& value) const
{
  return get(name.c_str(), value);
}

inline bool DataType::get(const std::string& name, unsigned long long& value) const
{
  return get(name.c_str(), value);
}

inline bool DataType::get(const std::string& name, double& value) const
{
  return get(name.c_str(), value);
}

inline bool DataType::get(const std::string& name, float& value) const
{
  return get(name.c_str(), value);
}

inline bool DataType::get(const std::string& name, const char*& value) const
{
  return get(name.c_str(), value);
}

inline bool DataType::get(const std::string& name, std::string& value) const
{
  return get(name.c_str(), value);
}

inline DataType DataType::addGroupElement(const std::string& name, Type type)
{
  return(addGroupElement(name.c_str(), type));
}

inline bool DataType::exists(const std::string& name) const
{
  return exists(name.c_str());
}

} }  // OSS::Persistent


#endif // OSS_HAVE_CONFIGPP
#endif // ENABLE_FEATURE_CONFIG
#endif // OSS_PERSISTENT_DATATYPE_H_INCLUDED

