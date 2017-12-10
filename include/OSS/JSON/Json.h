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


#ifndef OSS_JSON_H_INCLUDED
#define OSS_JSON_H_INCLUDED

#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"

namespace OSS {
namespace JSON {
  
  
typedef json::UnknownElement UnknownElement;
typedef json::Array Array;
typedef json::Boolean Boolean;
typedef json::Number Number;
typedef json::Object Object;
typedef json::String String;
typedef json::Reader Reader;
typedef json::Writer Writer;
typedef json::Exception Exception;

bool json_parse_string(const std::string& jsonString, OSS::JSON::Object& object);
bool json_parse_string(const std::string& jsonString, OSS::JSON::Object& object, OSS::JSON::Exception& e);
bool json_object_to_string(const OSS::JSON::Object& object, std::string& jsonString);
bool json_object_to_string(const OSS::JSON::Object& object, std::string& jsonString, OSS::JSON::Exception& e);

template <typename T>
bool json_to_string(const T& object, std::string& jsonString, OSS::JSON::Exception& e)
{
  try
  {
    std::stringstream ostr;
    OSS::JSON::Writer::Write(object, ostr);
    jsonString = ostr.str();
  }
  catch(OSS::JSON::Exception& e_)
  {
    e = e_;
    return false;
  }
  return true;
}

template <typename T>
bool json_to_string(const T& object, std::string& jsonString)
{
  OSS::JSON::Exception e;
  return json_to_string<T>(object, jsonString, e);
}


  
} }

#endif // OSS_JSON_H_INCLUDED


