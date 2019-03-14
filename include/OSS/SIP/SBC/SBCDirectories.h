#ifndef SBCDIRECTORIES_H_INCLUDED
#define SBCDIRECTORIES_H_INCLUDED

#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Singleton.h"

namespace OSS {
namespace SIP {
namespace SBC {

class SBCDirectories : public OSS::UTL::Singleton<SBCDirectories>
{
public:
  const std::string& getTempDirectory() const;
  void setTempDirectory(const std::string& dir);
  
  const std::string& getDbDirectory() const;
  void setDbDirectory(const std::string& dir);
  
  const std::string& getConfigDirectory() const;
  void setConfigDirectory(const std::string& dir);
  
  const std::string& getLogDirectory() const;
  void setLogDirectory(const std::string& dir);
  
  const std::string& getStateDirectory() const;
  void setStateDirectory(const std::string& dir);

private:
  std::string _tmp_dir;
  std::string _db_dir;
  std::string _conf_dir;
  std::string _log_dir;
  std::string _state_dir;
};


//
// Inlines
//

inline const std::string& SBCDirectories::getConfigDirectory() const
{
	static std::string conf_dir = OSS::system_confdir() + std::string("/karoo.conf.d/config");
	return _conf_dir.empty() ? conf_dir : _conf_dir;
}

inline void SBCDirectories::setConfigDirectory(const std::string& dir)
{
	_conf_dir = dir;
}

inline const std::string& SBCDirectories::getTempDirectory() const
{
  static std::string temp_dir = "/tmp";
  return _tmp_dir.empty() ? temp_dir : _tmp_dir;
}

inline void SBCDirectories::setTempDirectory(const std::string& dir)
{
  _tmp_dir = dir;
}

inline const std::string& SBCDirectories::getDbDirectory() const
{
  static std::string db_dir = OSS::system_confdir() + std::string("/karoo.conf.d/workspace");
  return _db_dir.empty() ? _db_dir : _db_dir;
}

inline void SBCDirectories::setDbDirectory(const std::string& dir)
{
  _db_dir = dir;
}

inline const std::string& SBCDirectories::getLogDirectory() const
{
  static std::string log_dir = "/var/log/karoo";
  return _log_dir.empty() ? log_dir : _log_dir;
}

inline void SBCDirectories::setLogDirectory(const std::string& dir)
{
  _log_dir = dir;
}

inline const std::string& SBCDirectories::getStateDirectory() const
{
  static std::string state_dir = OSS::system_confdir() + std::string("/karoo.conf.d/states");
  return _state_dir.empty() ? state_dir : _state_dir;
}

inline void SBCDirectories::setStateDirectory(const std::string& dir)
{
  _state_dir = dir;
}

} } } // OSS::SIP::SBC

#endif // SBCDIRECTORIES_H_INCLUDED

