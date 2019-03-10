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

#ifndef SBCPLUGINMANAGER_H
#define	SBCPLUGINMANAGER_H


#include <dlfcn.h>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

#include "OSS/OSS.h"
#include "SBCPlugin.h"


namespace OSS {
namespace SIP {
namespace SBC {

class SBCManager;

class OSS_API SBCPluginLoader
{
public:
  SBCPluginLoader();

  ~SBCPluginLoader();

  bool loadPlugin(const std::string& path);

  void unloadPlugin();

  bool isPluginLoaded() const;

  void* findPluginSymbol(const std::string& name);

  const std::string& getPath() const;

  SBCPluginContainer& plugins();

protected:
  SBCPluginContainer _plugins;

private:
  typedef bool (*FuncInitializePlugin)(SBCPluginContainer*);
  std::string _path;
	void* _handle;
};

class OSS_API SBCPluginManager
{
public:
  SBCPluginManager(SBCManager* pManager);

  ~SBCPluginManager();

  bool loadPlugins(const boost::filesystem::path& pathConfigFile, const boost::filesystem::path& sipConfigFile);

protected:
  bool loadEncryptionPlugin(const boost::filesystem::path& pathConfigFile, const boost::filesystem::path& sipConfigFile);

  bool loadRoutePlugin(const boost::filesystem::path& pathConfigFile, const boost::filesystem::path& sipConfigFile);

  bool loadCallDetailPlugin(const boost::filesystem::path& pathConfigFile, const boost::filesystem::path& sipConfigFile);

private:
  SBCManager* _pManager;
  SBCPluginLoader _encryption;
  SBCPluginLoader _registry;
  SBCEncryptionPluginBase* _pEncryptor;
};

} } } // OSS::SIP::SBC

#endif	// SBCPLUGINMANAGER_H

