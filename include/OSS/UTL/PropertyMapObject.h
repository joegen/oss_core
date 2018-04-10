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
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USvoidE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef OSS_PROPERTYMAPOBJECT_H_INCLUDED
#define OSS_PROPERTYMAPOBJECT_H_INCLUDED

#include "OSS/UTL/PropertyMap.h"
#include "OSS/UTL/Thread.h"
#include <map>
#include <string>

namespace OSS {

class PropertyMapObject
{
public:
  typedef std::map<std::string, std::string> InternalProperties;
  PropertyMapObject();
  PropertyMapObject(const PropertyMapObject& copy);
  PropertyMapObject& operator = (const PropertyMapObject& copy);
  void swap(PropertyMapObject& copy);
  
  void setProperty(const std::string& property, const std::string& value);
    /// Set a custom property for this object.
    /// Custom properties are meant to simply hold
    /// arbitrary data to aid in how the object
    /// is processed.
  
  void setProperty(PropertyMap::Enum property, const std::string& value);
    /// Set a custom property for this object.
    /// Custom properties are meant to simply hold
    /// arbitrary data to aid in how the object
    /// is processed.

  bool getProperty(const std::string&  property, std::string& value) const;
    /// Get a custom property of this object.
    /// Custom properties are meant to simply hold
    /// arbitrary data to aid in how the object
    /// is processed.
  
  bool getProperty(PropertyMap::Enum property, std::string& value) const;
    /// Get a custom property of this object.
    /// Custom properties are meant to simply hold
    /// arbitrary data to aid in how the object
    /// is processed.
  
  std::string getProperty(const std::string&  property) const;
  std::string getProperty(PropertyMap::Enum property) const;

  void clearProperties();
    /// Remove all custom properties
  
protected:
  InternalProperties _internalProperties;
  mutable OSS::mutex_read_write _internalPropertiesMutex;
};

//
// Inlines
//
inline bool PropertyMapObject::getProperty(PropertyMap::Enum   property, std::string& value) const
{
  return getProperty(PropertyMap::propertyString(property), value);
}

inline std::string PropertyMapObject::getProperty(PropertyMap::Enum property) const
{
  return getProperty(PropertyMap::propertyString(property));
}

inline void PropertyMapObject::setProperty(PropertyMap::Enum property, const std::string& value)
{
  setProperty(PropertyMap::propertyString(property), value);
}

} // OSS

#endif // OSS_PROPERTYMAPOBJECT_H_INCLUDED

