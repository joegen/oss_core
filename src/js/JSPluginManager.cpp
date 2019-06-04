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



#include <Poco/ClassLoader.h>
#include <Poco/ThreadPool.h>
#include "OSS/UTL/Logger.h"
#include "OSS/JS/JSPluginManager.h"
#include "OSS/JS/JSIsolate.h"

Poco::ThreadPool* JSPlugin::_pThreadPool = 0;

namespace OSS {
namespace JS {

JSPluginManager::JSPluginManager(JSIsolate* pIsolate) :
  _pIsolate(pIsolate)
{
  if (!JSPlugin::_pThreadPool)
  {
    JSPlugin::_pThreadPool = new Poco::ThreadPool(10, 256);
  }
}

JSPluginManager::~JSPluginManager()
{
}

JSPlugin* JSPluginManager::loadPlugin(const std::string& path)
{
  OSS::mutex_critic_sec_lock lock(_pluginMutex);
  PluginMap::iterator iter = _plugins.find(path);
  if (iter != _plugins.end())
  {
    return iter->second;
  }
  //
  // Create a new loader and instantiate the class
  //
  JSPluginLoader* pLoader = new JSPluginLoader();
  try
  {
    pLoader->loadLibrary(path);
    JSPlugin* pPlugin = pLoader->create("oss_module");
    if (!pPlugin)
    {
      OSS_LOG_ERROR("JSPluginManager::loadPlugin is unable to load " << path);
      delete pLoader;
      return 0;
    }
    _plugins[path] = pPlugin;
    _pluginsLoaders[path] = pLoader;
    pLoader->classFor("oss_module").autoDelete(pPlugin);
    return pPlugin;
  }
  catch(const Poco::Exception& e)
  {
    OSS_LOG_ERROR("JSPluginManager::loadPlugin is unable to load " << path << " Exception: " << e.message());
  }
  return 0;
}

void JSPluginManager::releaseAllPlugins()
{
  OSS::mutex_critic_sec_lock lock(_pluginMutex);
  for (LoaderMap::iterator iter = _pluginsLoaders.begin(); iter != _pluginsLoaders.end(); iter++)
  {
    iter->second->unloadLibrary(iter->first);
    delete iter->second;
  }
  _pluginsLoaders.clear();
}

JSIsolate* JSPluginManager::getIsolate()
{
  return _pIsolate;
}


} }



