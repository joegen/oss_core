#include "OSS/SIP/SBC/SBCConfiguration.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SBC/SBCContact.h"

namespace OSS {
namespace SIP {
namespace SBC {


SBCConfiguration* SBCConfiguration::_instance = 0;

SBCConfiguration* SBCConfiguration::instance()
{
  if (!SBCConfiguration::_instance)
  {
    SBCConfiguration::_instance = new SBCConfiguration();
  }
  return SBCConfiguration::_instance;
}

void SBCConfiguration::delete_instance()
{
  delete SBCConfiguration::_instance;
  SBCConfiguration::_instance = 0;
}

SBCConfiguration::SBCConfiguration()
{
}

SBCConfiguration::~SBCConfiguration()
{
}

bool SBCConfiguration::initUserAgent()
{
  if (!retrieveConfig("userAgentConfig", _userAgent))
  {
    return false;
  }
  if (_userAgent.Exists("user_agent_name"))
  {
    OSS::JSON::String val = _userAgent["user_agent_name"];
    SBCManager::instance()->_userAgentName = val.Value();
  }

  if (_userAgent.Exists("enable_options_routing"))
  {
    OSS::JSON::Boolean val = _userAgent["enable_options_routing"];
    SBCManager::instance()->_enableOptionsRouting = val.Value();
  }

  if (_userAgent.Exists("disable_options_keep_alive"))
  {
    OSS::JSON::Boolean val = _userAgent["disable_options_keep_alive"];
    SBCManager::instance()->_enableOptionsKeepAlive = !val.Value();
  }

  if (_userAgent.Exists("require_rtp_for_registrations"))
  {
    OSS::JSON::Boolean val = _userAgent["require_rtp_for_registrations"];
    SBCManager::instance()->_requireRtpForRegistrations = val.Value();
  }
  
  if (_userAgent.Exists("max_registers_per_second"))
  {
    OSS::JSON::Number val = _userAgent["max_registers_per_second"];
    SBCManager::instance()->_maxRegistersPerSecond = val.Value();
  }
  
  if (_userAgent.Exists("max_invites_per_second"))
  {
    OSS::JSON::Number val = _userAgent["max_invites_per_second"];
    SBCManager::instance()->_maxInvitesPerSecond = val.Value();
  }
  
  if (_userAgent.Exists("max_subscribes_per_second"))
  {
    OSS::JSON::Number val = _userAgent["max_subscribes_per_second"];
    SBCManager::instance()->_maxSubscribesPerSecond = val.Value();
  }
  
  if (_userAgent.Exists("register_state_in_contact_params"))
  {
    OSS::JSON::Boolean val = _userAgent["register_state_in_contact_params"];
    SBCContact::_registerStateInParams = val.Value();
  }
  
  if (_userAgent.Exists("dialog_state_in_contact_params"))
  {
    OSS::JSON::Boolean val = _userAgent["dialog_state_in_contact_params"];
    SBCContact::_dialogStateInParams = val.Value();
  }
  return true;
}

bool SBCConfiguration::initTransport()
{
  if (!retrieveConfig("transportConfig", _transport))
  {
    return false;
  }
  SBCManager::instance()->transactionManager().stack().initTransportFromJSON(_transport);
  return true;
}

bool SBCConfiguration::retrieveConfig(const std::string& configName, OSS::JSON::Object& config)
{
  SBCJSModuleManager::CustomEventArgs args;
  SBCJSModuleManager::CustomEventArgs result;
  if (!SBCJSModuleManager::instance()->processCustomEvent(configName, args, result) || result.empty())
  {
    return false;
  }
  SBCJSModuleManager::CustomEventArgs::iterator iter = result.find("JSONConfig");
  if (iter == result.end())
  {
    return false;
  }
  return OSS::JSON::json_parse_string(iter->second, config);
}


} } } // OSS::SIP::SBC