#include "OSS/JSON/Json.h"

namespace OSS {
namespace JSON {
  
  
bool json_parse_string(const std::string& jsonString, OSS::JSON::Object& object)
{
  OSS::JSON::Exception e;
  return json_parse_string(jsonString, object, e);
}

bool json_parse_string(const std::string& jsonString, OSS::JSON::Object& object, OSS::JSON::Exception& e)
{
  try
  {
    std::stringstream ostr;
    ostr << jsonString;
    OSS::JSON::Reader::Read(object, ostr);
  }
  catch(OSS::JSON::Exception& e_)
  {
    e = e_;
    return false;
  }
  return true;
}

bool json_object_to_string(const OSS::JSON::Object& object, std::string& jsonString)
{
  OSS::JSON::Exception e;
  return json_object_to_string(object, jsonString, e);
}

bool json_object_to_string(const OSS::JSON::Object& object, std::string& jsonString, OSS::JSON::Exception& e)
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

} } // OSS::JSON