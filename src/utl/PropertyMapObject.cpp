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

#include "OSS/UTL/PropertyMapObject.h"


namespace OSS {


PropertyMapObject::PropertyMapObject()
{
}

PropertyMapObject::PropertyMapObject(const PropertyMapObject& copy)
{
  OSS::mutex_read_lock lock(copy._internalPropertiesMutex);
  _internalProperties = copy._internalProperties;
}

PropertyMapObject& PropertyMapObject::operator = (const PropertyMapObject& copy)
{
  PropertyMapObject swappable(copy);
  swap(swappable);
  return *this;
}

void PropertyMapObject::swap(PropertyMapObject& copy)
{
  OSS::mutex_write_lock lock_theirs(copy._internalPropertiesMutex);
  OSS::mutex_write_lock lock_ours(_internalPropertiesMutex);
  std::swap(_internalProperties, copy._internalProperties);
}

void PropertyMapObject::setProperty(const std::string& property, const std::string& value)
{
  if (property.empty())
    return;
  
  OSS::mutex_write_lock lock(_internalPropertiesMutex);
  _internalProperties[property] = value;
}

bool PropertyMapObject::getProperty(const std::string&  property, std::string& value) const
{
  if (property.empty())
    return false;
  
  OSS::mutex_read_lock lock_theirs(_internalPropertiesMutex);
  InternalProperties::const_iterator iter = _internalProperties.find(property);
  if (iter != _internalProperties.end())
  {
    value = iter->second;
    return true;
  }
  return false;
}

std::string PropertyMapObject::getProperty(const std::string&  property) const
{
  std::string value;
  getProperty(property, value);
  return value;
}

void PropertyMapObject::clearProperties()
{
  OSS::mutex_write_lock lock(_internalPropertiesMutex);
  _internalProperties.clear();
}

} // OSS


