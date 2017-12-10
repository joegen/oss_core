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

#ifndef OSS_CONFIGOBJECT_H_INCLUDED
#define OSS_CONFIGOBJECT_H_INCLUDED


#include <libconfig.h>
#include "OSS/JS/JSPlugin.h"


class ConfigObject : public OSS::JS::JSObjectWrap
{
public:
  typedef std::map<uint32_t, config_setting_t*> Settings;
  
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(readFile);
  JS_METHOD_DECLARE(writeFile);
  JS_METHOD_DECLARE(toString);
  JS_METHOD_DECLARE(rootSetting);
  JS_METHOD_DECLARE(lookupString);
  JS_METHOD_DECLARE(lookupInteger);
  JS_METHOD_DECLARE(lookupFloat);
  JS_METHOD_DECLARE(lookupBoolean);
  JS_METHOD_DECLARE(lookupSetting);
  JS_METHOD_DECLARE(settingLength);
  JS_METHOD_DECLARE(settingType);
  JS_METHOD_DECLARE(settingLookupElement);
  JS_METHOD_DECLARE(settingLookupString);
  JS_METHOD_DECLARE(settingLookupInteger);
  JS_METHOD_DECLARE(settingLookupFloat);
  JS_METHOD_DECLARE(settingLookupBoolean);
  JS_METHOD_DECLARE(settingLookupSetting);
  
  JS_METHOD_DECLARE(settingAddSetting);
  JS_METHOD_DECLARE(settingSetAsString);
  JS_METHOD_DECLARE(settingSetAsInteger);
  JS_METHOD_DECLARE(settingSetAsFloat);
  JS_METHOD_DECLARE(settingSetAsBoolean);
  
  static bool isConfig(JSValueHandle value);
  config_setting_t* findSetting(uint32_t id);
  uint32_t findSetting(config_setting_t* pSetting);
  uint32_t getRootSetting();
  
  config_t& getConfig();
private:
  ConfigObject();
  virtual ~ConfigObject();
  config_t _cfg;
  Settings _settings;
  uint32_t _id;
  uint32_t _rootSettingId;
};

//
// Inlines
//
inline config_t& ConfigObject::getConfig()
{
  return _cfg;
}


#endif // OSS_CONFIGOBJECT_H_INCLUDED

