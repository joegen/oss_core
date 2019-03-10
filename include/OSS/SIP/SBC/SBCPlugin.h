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


#ifndef SBCPLUGIN_H
#define	SBCPLUGIN_H


#include <dlfcn.h>
#include <string>
#include <vector>
#include <sstream>
#include <map>
//
// Base class for the plugin factory
//
typedef std::map<std::string, std::string> PluginProperties;

template <typename T>
class SBCPlugin
{
public:
  SBCPlugin()
  {
    _pluginName = "SBCPlugin";
    _priority = 0;
  }
  virtual ~SBCPlugin(){}
  virtual T* createInstance(const PluginProperties& properties = PluginProperties()) = 0;
  const std::string& getPluginName() const{ return _pluginName; };
  const std::string& getPluginSubType() const{ return _pluginSubType; };
  int getPriority(){ return _priority; };
protected:
  std::string _pluginName;
  std::string _pluginSubType;
  int _priority;
};


//
// The encryption plugin
//
class SBCEncryptionPluginBase
{
public:
  SBCEncryptionPluginBase(){}
  virtual ~SBCEncryptionPluginBase(){}
  virtual void encryptSIP(std::vector<char>& packet) = 0;
  virtual void decryptSIP(std::vector<char>& packet) = 0;
  virtual void encryptRTP(std::vector<char>& packet) = 0;
  virtual void decryptRTP(std::vector<char>& packet) = 0;
};
typedef SBCPlugin<SBCEncryptionPluginBase> SBCEncryptionPlugin;
typedef std::vector<SBCEncryptionPlugin*> SBCEncryptionPluginVector;


//
// holder class for all plugins that will be registered by a module
//
class SBCPluginContainer
{
public:
  typedef void(*FuncLog)(const char*);

  SBCPluginContainer(){}
  ~SBCPluginContainer()
  {
  }

  void registerEncryptionPlugin(SBCEncryptionPlugin* pPlugin)
  {
    _encryptionPlugins.push_back(pPlugin);
  }

  void cleanUp()
  {
    //
    // Delete encryption plugins
    //
    for (SBCEncryptionPluginVector::iterator iter = _encryptionPlugins.begin();
      iter != _encryptionPlugins.end(); iter++)
    {
      delete *iter;
    }


  }

  SBCEncryptionPluginVector& encryption(){return _encryptionPlugins; }

public:
  static FuncLog logDebug;
  static FuncLog logInfo;
  static FuncLog logWarning;
  static FuncLog logError;
  static FuncLog logNotice;
  static FuncLog logCritical;

private:
  SBCEncryptionPluginVector _encryptionPlugins;
};

#define PLUGIN_LOG_DEBUG(data) \
{ \
  std::ostringstream strm; \
  strm << data; \
  SBCPluginContainer::logDebug(strm.str().c_str()); \
}

#define PLUGIN_LOG_INFO(data) \
{ \
  std::ostringstream strm; \
  strm << data; \
  SBCPluginContainer::logInfo(strm.str().c_str()); \
}

#define PLUGIN_LOG_NOTICE(data) \
{ \
  std::ostringstream strm; \
  strm << data; \
  SBCPluginContainer::logNotice(strm.str().c_str()); \
}

#define PLUGIN_LOG_ERROR(data) \
{ \
  std::ostringstream strm; \
  strm << data; \
  SBCPluginContainer::logError(strm.str().c_str()); \
}

#define PLUGIN_LOG_WARNING(data) \
{ \
  std::ostringstream strm; \
  strm << data; \
  SBCPluginContainer::logWarning(strm.str().c_str()); \
}

#define PLUGIN_LOG_CRITICAL(data) \
{ \
  std::ostringstream strm; \
  strm << data; \
  SBCPluginContainer::logCritical(strm.str().c_str()); \
}

#endif	/* SBCPLUGIN_H */

