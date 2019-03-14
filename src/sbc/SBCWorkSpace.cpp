#include <OSS/JSON/Json.h>
#include <OSS/UTL/Thread.h>

#include "OSS/SIP/SBC/SBCWorkSpace.h"
#include "OSS/SIP/SBC/SBCDirectories.h"


namespace OSS {
namespace SIP {
namespace SBC {


SBCWorkSpace::SBCWorkSpace(const std::string& name) :
  _useLocalDb(true),
  _name(name)
{
}

SBCWorkSpace::~SBCWorkSpace()
{
  if (_useLocalDb)
  {
    _local.close();
  } else 
  {
    _remote.disconnect();
  }
}

bool SBCWorkSpace::set(const std::string& key, const json::Object& value, int expires)
{
  std::string json;
  return (OSS::JSON::json_object_to_string(value, json) && set(key, json, expires)); 
}

bool SBCWorkSpace::set(const std::string& key, const std::string& value, int expires)
{
  if (!_useLocalDb) 
  {
    return _remote.set(key, value, expires);
  } else {
    OSS::mutex_critic_sec_lock lock(_localDbMutex);
    return _local.set(key, value);
  }
}

bool SBCWorkSpace::get(const std::string& key, json::Object& value) const
{
  std::string json;
  return (get(key, json) && OSS::JSON::json_parse_string(json, value));
}

bool SBCWorkSpace::get(const std::string& key, std::string& value) const
{
  if (!_useLocalDb) 
  {
    return _remote.get(key, value);
  } else {
    OSS::mutex_critic_sec_lock lock(_localDbMutex);
    return _local.get(key, value);
  }
}

bool SBCWorkSpace::del(const std::string& key)
{
  if (!_useLocalDb) 
  {
    return _remote.del(key);
  } else {
    OSS::mutex_critic_sec_lock lock(_localDbMutex);
    return _local.erase(key);
  }
}

bool SBCWorkSpace::getKeys(const std::string& pattern, std::vector<std::string>& keys)
{
  if (!_useLocalDb) 
  {
    return _remote.getKeys(pattern, keys);
  } else {
    OSS::mutex_critic_sec_lock lock(_localDbMutex);
    return _local.getKeys(pattern, keys);
  }
}

bool SBCWorkSpace::connect(const std::string& tcpHost, int tcpPort, const std::string& password, int db, bool allowReconnect )
{
  // Disabling remotedb for now
  assert(false);
  _useLocalDb = false;
  _local.close();
  return _remote.connect(tcpHost, tcpPort, password, db, allowReconnect);
}

bool SBCWorkSpace::open(const std::string& localDbFile)
{
  _useLocalDb = true;
  _remote.disconnect();
  OSS::mutex_critic_sec_lock lock(_localDbMutex);
  return _local.open(localDbFile);
}

bool SBCWorkSpace::open()
{
  assert(!_name.empty());
  std::ostringstream path;
  path << SBCDirectories::instance()->getDbDirectory() << "/" << _name;
  return open(path.str());
}

void SBCWorkSpace::disconnect()
{
  if (!_useLocalDb) 
  {
     _remote.disconnect();
  } else {
    OSS::mutex_critic_sec_lock lock(_localDbMutex);
    _local.close();
  }
}

} } }