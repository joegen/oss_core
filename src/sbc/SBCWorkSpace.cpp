#include <OSS/JSON/Json.h>
#include <OSS/UTL/Thread.h>

#include "OSS/SIP/SBC/SBCWorkSpace.h"
#include "OSS/SIP/SBC/SBCDirectories.h"


namespace OSS {
namespace SIP {
namespace SBC {


SBCWorkSpace::SBCWorkSpace(const std::string& name) :
  _name(name)
{
}

SBCWorkSpace::~SBCWorkSpace()
{
    _db.close();
}

bool SBCWorkSpace::set(const std::string& key, const json::Object& value, int expires)
{
  std::string json;
  return (OSS::JSON::json_object_to_string(value, json) && set(key, json, expires)); 
}

bool SBCWorkSpace::set(const std::string& key, const std::string& value, int expires)
{
    OSS::mutex_critic_sec_lock lock(_dbMutex);
    return _db.set(key, value);
}

bool SBCWorkSpace::get(const std::string& key, json::Object& value) const
{
  std::string json;
  return (get(key, json) && OSS::JSON::json_parse_string(json, value));
}

bool SBCWorkSpace::get(const std::string& key, std::string& value) const
{
    OSS::mutex_critic_sec_lock lock(_dbMutex);
    return _db.get(key, value);
}

bool SBCWorkSpace::del(const std::string& key)
{
    OSS::mutex_critic_sec_lock lock(_dbMutex);
    return _db.erase(key);
}

bool SBCWorkSpace::getKeys(const std::string& pattern, std::vector<std::string>& keys)
{
    OSS::mutex_critic_sec_lock lock(_dbMutex);
    return _db.getKeys(pattern, keys);
}

bool SBCWorkSpace::open(const std::string& localDbFile)
{
  OSS::mutex_critic_sec_lock lock(_dbMutex);
  return _db.open(localDbFile);
}

bool SBCWorkSpace::open()
{
  assert(!_name.empty());
  std::ostringstream path;
  path << SBCDirectories::instance()->getDbDirectory() << "/" << _name;
  return open(path.str());
}

void SBCWorkSpace::close()
{
    OSS::mutex_critic_sec_lock lock(_dbMutex);
    _db.close();
}

} } }