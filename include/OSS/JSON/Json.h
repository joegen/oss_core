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
bool jsong_object_to_string(const OSS::JSON::Object& object, std::string& jsonString);
bool jsong_object_to_string(const OSS::JSON::Object& object, std::string& jsonString, OSS::JSON::Exception& e);
  
} }

#endif // OSS_JSON_H_INCLUDED


