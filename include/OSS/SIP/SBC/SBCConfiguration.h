#ifndef OSS_SBCCONFIGURATION_H_INCLUDED
#define OSS_SBCCONFIGURATION_H_INCLUDED


#include "OSS/JSON/Json.h"
#include "OSS/SIP/SBC/SBCJSModuleManager.h"


namespace OSS {
namespace SIP {
namespace SBC {


class SBCConfiguration
{
public:
  static SBCConfiguration* instance();
  static void delete_instance();
  const OSS::JSON::Object& transport() const;
  const OSS::JSON::Object& userAgent() const;
  bool initUserAgent();
  bool initTransport();
protected:
  SBCConfiguration();
  ~SBCConfiguration();
  bool retrieveConfig(const std::string& configName, OSS::JSON::Object& config);
  
private:
  OSS::JSON::Object _transport;
  OSS::JSON::Object _userAgent;
  static SBCConfiguration* _instance;
};


//
// Inlines
//

inline const OSS::JSON::Object& SBCConfiguration::transport() const
{
	return _transport;
}

inline const OSS::JSON::Object& SBCConfiguration::userAgent() const
{
	return _userAgent;
}

} } } // OSS::SIP::SBC

#endif // OSS_SBCCONFIGURATION_H_INCLUDED
