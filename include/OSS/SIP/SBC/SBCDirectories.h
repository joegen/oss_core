#ifndef SBCDIRECTORIES_H_INCLUDED
#define SBCDIRECTORIES_H_INCLUDED

#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace SIP {
namespace SBC {

class SBCDirectories 
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
  
  void prepareDirectories();
  
  const std::string& getRegistrationStateDirectory() const;
  const std::string& getDialogStateDirectory() const;

  static SBCDirectories* instance();
  static void deleteInstance();
private:
  std::string _tmp_dir;
  std::string _db_dir;
  std::string _conf_dir;
  std::string _log_dir;
  std::string _state_dir;
  std::string _reg_state_dir;
  std::string _dialog_state_dir;
  static SBCDirectories* _instance;
};

//
// Inlines
//

inline const std::string& SBCDirectories::getRegistrationStateDirectory() const
{
  return _reg_state_dir;
}

inline const std::string& SBCDirectories::getDialogStateDirectory() const
{
  return _dialog_state_dir;
}

} } } // OSS::SIP::SBC

#endif // SBCDIRECTORIES_H_INCLUDED

