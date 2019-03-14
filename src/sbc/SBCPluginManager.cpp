
// OSS Software Solutions Application Programmer Interface
// Package: SBC Plugin Framework
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSSAPI API.
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#include "OSS/SIP/SBC/SBCPluginManager.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/SIP/SIPXOR.h"
#include "OSS/UTL/Logger.h"
#include "OSS/SIP/SBC/SBCRegisterBehavior.h"



void plugin_log_debug(const char* data)
{
  OSS::log_debug(data);
}

void plugin_log_info(const char* data)
{
  OSS::log_information(data);
}

void plugin_log_notice(const char* data)
{
  OSS::log_notice(data);
}

void plugin_log_warning(const char* data)
{
  OSS::log_warning(data);
}

void plugin_log_error(const char* data)
{
  OSS::log_error(data);
}

void plugin_log_critical(const char* data)
{
  OSS::log_critical(data);
}

SBCPluginContainer::FuncLog SBCPluginContainer::logCritical = plugin_log_critical;
SBCPluginContainer::FuncLog SBCPluginContainer::logDebug = plugin_log_debug;
SBCPluginContainer::FuncLog SBCPluginContainer::logError = plugin_log_error;
SBCPluginContainer::FuncLog SBCPluginContainer::logInfo = plugin_log_info;
SBCPluginContainer::FuncLog SBCPluginContainer::logNotice = plugin_log_notice;
SBCPluginContainer::FuncLog SBCPluginContainer::logWarning = plugin_log_warning;


namespace OSS {
namespace SIP {
namespace SBC {
  
SBCPluginLoader::SBCPluginLoader()
{
  _handle = 0;
}

SBCPluginLoader::~SBCPluginLoader()
{
  
  _plugins.cleanUp();
  unloadPlugin();
}

bool SBCPluginLoader::loadPlugin(const std::string& path)
{
  if (_handle)
  {
    OSS_LOG_ERROR(path << " is already loaded!");
    return false;
  }

  _handle = ::dlopen(path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
  _path = path;
  if (_handle)
  {
    FuncInitializePlugin initializePlugin = (FuncInitializePlugin)findPluginSymbol("initializePlugin");
    if (initializePlugin != 0)
    {
      OSS_LOG_INFO("Calling initialize for " << path);
      return initializePlugin(&_plugins);
    }
    else
    {
      unloadPlugin();
      OSS_LOG_ERROR(path << " doesn't have an initialize method exported.");
      return false;
    }
  }
  else
  {
  	OSS_LOG_ERROR("Unable to load shared library using dlopen - " << path);
    return false;
  }
  return false;
}

void SBCPluginLoader::unloadPlugin()
{
  if (_handle)
  {
    ::dlclose(_handle);
    _handle = 0;
  }
}

bool SBCPluginLoader::isPluginLoaded() const
{
  return _handle != 0;
}

void* SBCPluginLoader::findPluginSymbol(const std::string& name)
{
  void* result = 0;
  if (_handle)
  {
    result = ::dlsym(_handle, name.c_str());
  }
  return result;
}

const std::string& SBCPluginLoader::getPath() const
{
  return _path;
}

SBCPluginContainer& SBCPluginLoader::plugins()
{
  return _plugins;
}

//
// The plugin manager
//
SBCPluginManager::SBCPluginManager(SBCManager* pManager) :
  _pManager(pManager)
{
  _pEncryptor = 0;
}

SBCPluginManager::~SBCPluginManager()
{
  if (_pEncryptor)
  {
    delete _pEncryptor;
    _pEncryptor = 0;
  }
}

bool SBCPluginManager::loadPlugins(const boost::filesystem::path& pathConfigFile, const boost::filesystem::path& sipConfigFile)
{
  loadEncryptionPlugin(pathConfigFile, sipConfigFile);
  return true;
}
  
bool SBCPluginManager::loadEncryptionPlugin(const boost::filesystem::path& pathConfigFile, const boost::filesystem::path& sipConfigFile)
{
  using namespace OSS::Persistent;

  ClassType pathCfg;
  if (!pathCfg.load(pathConfigFile))
    return false;
  std::string plugin;
  if (!pathCfg["paths"].exists("pea-encryption-plugin"))
    return false;

  plugin = (const char*)pathCfg["paths"]["pea-encryption-plugin"];
  if (!boost::filesystem::exists(plugin))
      return false;


  OSS_LOG_INFO("Loading Encryption plugins ... ")
  if (!_encryption.loadPlugin(plugin))
  {
    OSS_LOG_ERROR("Unable to load PAE Plugin.");
    return false;
  }

  if (_encryption.plugins().encryption().size() == 0)
  {
    OSS_LOG_ERROR("Unable to load PAE Plugin. Share file registered zero encryptors");
    return false;
  }
  
  std::vector<char> buff;
  _pEncryptor = _encryption.plugins().encryption()[0]->createInstance();
#if ENABLE_FEATURE_XOR
  if (_pEncryptor)
  {
    OSS_LOG_INFO( "PEA Encryption Plugin " <<
      _encryption.plugins().encryption()[0]->getPluginName() << "/" <<
      _encryption.plugins().encryption()[0]->getPluginSubType() <<
      " loaded from file " << plugin);

    SIPXOR::rtpEncryptExternal = boost::bind(&SBCEncryptionPluginBase::encryptRTP, _pEncryptor, _1);
    SIPXOR::rtpDecryptExternal = boost::bind(&SBCEncryptionPluginBase::decryptRTP, _pEncryptor, _1);
    SIPXOR::sipEncryptExternal = boost::bind(&SBCEncryptionPluginBase::encryptSIP, _pEncryptor, _1);
    SIPXOR::sipDecryptExternal = boost::bind(&SBCEncryptionPluginBase::decryptSIP, _pEncryptor, _1);
  }
  else
  {
    OSS_LOG_ERROR("Unable to create PEA Encryption Plugin instance.");
  }
#endif

  return _pEncryptor != 0;
}

bool SBCPluginManager::loadRoutePlugin(const boost::filesystem::path& pathConfigFile, const boost::filesystem::path& sipConfigFile)
{
  return false;
}

bool SBCPluginManager::loadCallDetailPlugin(const boost::filesystem::path& pathConfigFile, const boost::filesystem::path& sipConfigFile)
{
  return false;
}


} } } // OSS::SIP::SBC








