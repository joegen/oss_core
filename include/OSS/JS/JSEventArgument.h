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

#ifndef OSS_JSEVENTARGUMENT_H_INCLUDED
#define OSS_JSEVENTARGUMENT_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_V8


#include <v8.h>
#include <boost/core/addressof.hpp>
#include <OSS/JS/JSPlugin.h>
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSPersistentValue.h"


namespace OSS {
namespace JS {

class JSEventArgument : protected OSS::JSON::Array
{
public:
  typedef OSS::JSON::UnknownElement Argument;
  typedef OSS::JSON::String String;
  typedef OSS::JSON::Number Number;
  typedef OSS::JSON::Boolean Boolean;
  typedef OSS::JSON::Array Array;
  typedef OSS::JSON::Object Object;
  
  JSEventArgument();
  
  JSEventArgument(JSEventArgument& event);
  
  ~JSEventArgument();
  
  JSEventArgument& operator = (const JSEventArgument& event);
  
  void setQueueFd(int queueFd);
  
  int getQueueFd() const;
  
  void setEventName(const std::string& eventName);
  
  std::string& getEventName() const;
  
  void addArgument(const Argument& argument);
  void addString(const std::string& argument);
  void addUInt32(uint32_t argument);
  void addInt32(int32_t argument);
  void addNumber(double argument);
  void addBoolean(bool argument);
  void addObject(const Object& argument);
  void addArray(const Array& argument);
  
  template <typename T>
  bool getArgument(T& argument, std::size_t index) 
  {
    if (Size() -1 < index)
    {
      return false;
    }
    argument = (*this)[index];
    return true;
  }
  
  bool getString(std::string& argument, std::size_t index);
  bool getUInt32(uint32_t& argument, std::size_t index);
  bool getInt32(int32_t& argument, std::size_t index);
  bool getNumber(double& argument, std::size_t index);
  bool getBoolean(bool& argument, std::size_t index);
  bool getObject(Object& argument, std::size_t index);
  bool getArray(Array& argument, std::size_t index);
  
  std::string json();
private:
  int _queueFd;
};

//
// Inlines
//

inline JSEventArgument::JSEventArgument() :
  _queueFd(0)
{
}

inline JSEventArgument::JSEventArgument(JSEventArgument& event) :
  OSS::JSON::Array(event),
  _queueFd(event._queueFd)
{
}

inline JSEventArgument::~JSEventArgument()
{
}

inline JSEventArgument& JSEventArgument::operator = (const JSEventArgument& event)
{
  if (boost::addressof(event) == this)
  {
    return *this;
  }
  Array::operator=(event);
  _queueFd = event._queueFd;
  return *this;
}

inline void JSEventArgument::setQueueFd(int queueFd)
{
  _queueFd = queueFd; 
}
  
inline int JSEventArgument::getQueueFd() const
{
  return _queueFd;
}

inline void JSEventArgument::setEventName(const std::string& eventName)
{
  (*this)[0] = String(eventName);
}
  
inline std::string& JSEventArgument::getEventName() const
{
  String& eventName = (String&)(*this)[0];
  return eventName.Value();
}

inline void JSEventArgument::addArgument(const Argument& argument)
{
  this->Insert(argument);
}

inline void JSEventArgument::addString(const std::string& argument)
{
  addArgument(String(argument));
}

inline void JSEventArgument::addUInt32(uint32_t argument)
{
  addArgument(Number(argument));
}

inline void JSEventArgument::addInt32(int32_t argument)
{
  addArgument(Number(argument));
}

inline void JSEventArgument::addNumber(double argument)
{
  addArgument(Number(argument));
}

inline void JSEventArgument::addBoolean(bool argument)
{
  addArgument(Boolean(argument));
}

inline void JSEventArgument::addObject(const Object& argument)
{
  addArgument(argument);
}

inline void JSEventArgument::addArray(const Array& argument)
{
  addArgument(argument);
}

inline bool JSEventArgument::getString(std::string& argument, std::size_t index) 
{
  String element;
  if (!getArgument<String>(element, index))
  {
    return false;
  }
  argument = element.Value();
  return true;
}

inline bool JSEventArgument::getUInt32(uint32_t& argument, std::size_t index) 
{
  Number element;
  if (!getArgument<Number>(element, index))
  {
    return false;
  }
  if (element.Value() < 0)
  {
    return false;
  }
  argument = (uint32_t)element.Value();
  return true;
}

inline bool JSEventArgument::getInt32(int32_t& argument, std::size_t index) 
{
  Number element;
  if (!getArgument<Number>(element, index))
  {
    return false;
  }
  argument = (int32_t)element.Value();
  return true;
}

inline bool JSEventArgument::getNumber(double& argument, std::size_t index) 
{
  Number element;
  if (!getArgument<Number>(element, index))
  {
    return false;
  }
  argument = element.Value();
  return true;
}

inline bool JSEventArgument::getBoolean(bool& argument, std::size_t index) 
{
  Boolean element;
  if (!getArgument<Boolean>(element, index))
  {
    return false;
  }
  argument = element.Value();
  return true;
}

inline bool JSEventArgument::getObject(Object& argument, std::size_t index) 
{
  Object element;
  if (!getArgument<Object>(element, index))
  {
    return false;
  }
  argument = element;
  return true;
}

inline bool JSEventArgument::getArray(Array& argument, std::size_t index) 
{
  Array element;
  if (!getArgument<Array>(element, index))
  {
    return false;
  }
  argument = element;
  return true;
}

inline std::string JSEventArgument::json()
{
  std::string result;
  OSS::JSON::json_to_string<Array>(*this, result);
  return result;
}

  
} } // OSS::JS

#endif //  ENABLE_FEATURE_V8
#endif // OSS_JSEVENTARGUMENT_H_INCLUDED

