#include "OSS/SIP/SBC/SBCDirectories.h"

namespace OSS {
namespace SIP {
namespace SBC {

SBCDirectories* SBCDirectories::_instance = 0;

SBCDirectories* SBCDirectories::instance()
{
  if (!SBCDirectories::_instance)
  {
    SBCDirectories::_instance = new SBCDirectories();
  }
  return SBCDirectories::_instance;
}
void SBCDirectories::deleteInstance()
{
  delete SBCDirectories::_instance; 
  SBCDirectories::_instance = 0;
}

const std::string& SBCDirectories::getConfigDirectory() const
{
	static std::string conf_dir = OSS::system_confdir() + std::string("/karoo.conf.d/config");
	return _conf_dir.empty() ? conf_dir : _conf_dir;
}

void SBCDirectories::setConfigDirectory(const std::string& dir)
{
	_conf_dir = dir;
}

const std::string& SBCDirectories::getTempDirectory() const
{
  static std::string temp_dir = "/tmp";
  return _tmp_dir.empty() ? temp_dir : _tmp_dir;
}

void SBCDirectories::setTempDirectory(const std::string& dir)
{
  _tmp_dir = dir;
}

const std::string& SBCDirectories::getDbDirectory() const
{
  static std::string db_dir = OSS::system_confdir() + std::string("/karoo.conf.d/workspace");
  return _db_dir.empty() ? db_dir : _db_dir;
}

void SBCDirectories::setDbDirectory(const std::string& dir)
{
  _db_dir = dir;
}

const std::string& SBCDirectories::getLogDirectory() const
{
  static std::string log_dir = "/var/log/karoo";
  return _log_dir.empty() ? log_dir : _log_dir;
}

void SBCDirectories::setLogDirectory(const std::string& dir)
{
  _log_dir = dir;
}

const std::string& SBCDirectories::getStateDirectory() const
{
  static std::string state_dir = OSS::system_confdir() + std::string("/karoo.conf.d/states");
  return _state_dir.empty() ? state_dir : _state_dir;
}

void SBCDirectories::setStateDirectory(const std::string& dir)
{
  _state_dir = dir;
}

} } }
