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

#ifndef OSS_JSPLUGINMANAGER_H_INCLUDED
#define OSS_JSPLUGINMANAGER_H_INCLUDED

#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/Thread.h"

namespace OSS {
namespace JS {

  
class JSIsolate;

class JSPluginManager
{
public:
  typedef std::map<std::string, JSPluginLoader*> LoaderMap;
  typedef std::map<std::string, JSPlugin*> PluginMap;
  JSPlugin* loadPlugin(const std::string& path);
  void releaseAllPlugins();
  JSIsolate* getIsolate();
  
private:
  OSS::mutex_critic_sec _pluginMutex;
  LoaderMap _pluginsLoaders;
  PluginMap _plugins;
  JSIsolate* _pIsolate;
protected:
  JSPluginManager(JSIsolate* pIsolate);
  ~JSPluginManager();
  
  friend class JSIsolate;
};


//
// Inlines
//


} }

#endif // OSS_JSPLUGINMANAGER_H_INCLUDED

