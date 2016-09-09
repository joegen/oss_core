
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

#include <stdio.h>
#include <boost/thread/pthread/mutex.hpp>

#include "OSS/JS/DUK/DuktapeContext.h"
#include "OSS/JS/DUK/duk_context_helper.h"
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/CoreUtils.h"

#include "OSS/JS/DUK/InternalModules.h"


namespace OSS {
namespace JS {
namespace DUK {



OSS::mutex_critic_sec DuktapeContext::_duk_mutex;
DuktapeContext::ModuleMap DuktapeContext::_moduleMap;
DuktapeContext::ModuleDirectories DuktapeContext::_moduleDirectories;
DuktapeContext::InternalModules DuktapeContext::_internalModules;
DuktapeContext::InternalJSModules DuktapeContext::_internalJSModules;


DuktapeContext* DuktapeContext::rootInstance()
{
  static DuktapeContext* pInstance = 0;
  if (!pInstance)
  {
    pInstance = new DuktapeContext("root-instance");
  }
  return pInstance;
}

DuktapeContext::DuktapeContext(const std::string& name) :
  _name(name),
  _pContext(0)
{
  OSS::mutex_critic_sec_lock lock(DuktapeContext::_duk_mutex);
  _pContext = duk_create_heap(NULL, // alloc function
      0, // realloc function
      0, // free function
      this, // user data
      duktape_fatal_handler); // fatal error handler
  
  assert(DuktapeContext::getContext(_pContext) == this);
  
  loadInternalModules();
}

DuktapeContext::~DuktapeContext()
{
  OSS::mutex_critic_sec_lock lock(DuktapeContext::_duk_mutex);
  duk_destroy_heap(_pContext);
  _pContext = 0;
}

void DuktapeContext::loadInternalModules()
{
  duk_init_common_js(_pContext);
  //
  // Initialize internal modules
  //
  createInternalModule("system", MOD::system_mod_init);
  createInternalModule("bson", MOD::bson_mod_init);
  
  //
  // Inline JS modules
  //
  std::string object_js(
    #include "js/duk/mod/object.js.inl"
  );
  createInternalModule("utils/object", object_js);
  
  std::string assert_js(
    #include "js/duk/mod/assert.js.inl"
  );
  createInternalModule("assert", assert_js);
}

DuktapeContext* DuktapeContext::getContext(duk_context* ctx)
{
  duk_memory_functions funcs;
  assert(ctx);
  duk_get_memory_functions(ctx, &funcs);
  return (DuktapeContext*)funcs.udata;
}

bool DuktapeContext::addModuleDirectory(const std::string& path)
{
  boost::filesystem::path moduleDirectory(path.c_str());
  if (!boost::filesystem::exists(moduleDirectory))
  {
    OSS_LOG_DEBUG("DuktapeContext::addModuleDirectory - " << OSS::boost_path(moduleDirectory) << " does not exists");
    return false;
  }
  OSS_LOG_DEBUG("DuktapeContext::addModuleDirectory - " << OSS::boost_path(moduleDirectory) << " REGISTERED");
  DuktapeContext::_moduleDirectories.push_back(moduleDirectory);
  return true;
}

void DuktapeContext::createInternalModule(DuktapeContext* pContext, const std::string& moduleId, ModuleInit initFunc)
{
  DuktapeContext::_internalModules[moduleId] = initFunc;
}

void DuktapeContext::createInternalModule(DuktapeContext* pContext, const std::string& moduleId, const std::string& code)
{
  DuktapeContext::_internalJSModules[moduleId] = code;
}

DuktapeModule* DuktapeContext::getModule(DuktapeContext* pContext, const std::string& moduleId)
{
  OSS::mutex_critic_sec_lock lock(DuktapeContext::_duk_mutex);
  DuktapeContext::ModuleMap::iterator iter = DuktapeContext::_moduleMap.find(moduleId);
  if (iter != DuktapeContext::_moduleMap.end())
  {
    return iter->second;
  }
  DuktapeModule* pModule = new DuktapeModule(pContext);
  DuktapeContext::_moduleMap[moduleId] = pModule;
  return pModule;
}

void DuktapeContext::deleteModule(const std::string& moduleId)
{
  OSS::mutex_critic_sec_lock lock(DuktapeContext::_duk_mutex);
  DuktapeContext::_moduleMap.erase(moduleId);
}

bool DuktapeContext::resolvePath(const std::string& file, std::string& absolutePath)
{
  boost::filesystem::path relativePath(file.c_str());
  if (boost::filesystem::exists(relativePath))
  {
    absolutePath = OSS::boost_path(relativePath);
    return true;
  }
  else
  {
    for (DuktapeContext::ModuleDirectories::iterator iter = DuktapeContext::_moduleDirectories.begin(); 
      iter != DuktapeContext::_moduleDirectories.end(); iter++)
    {
      boost::filesystem::path path = OSS::boost_path_concatenate(*iter, file);
      if (boost::filesystem::exists(path))
      {
        absolutePath = OSS::boost_path(path);
        return true;
      }
    }
  }
  return false;
}

bool DuktapeContext::resolveModule(const std::string& parentId, const std::string& moduleId, std::string& resolvedResult)
{
  OSS_LOG_DEBUG("DuktapeContext::resolveModule - resolving " << parentId << "::" << moduleId);
  bool isSharedLib = false;
  bool isJs = false;
  bool resolved = false;
  
  if (DuktapeContext::_internalModules.find(moduleId) != DuktapeContext::_internalModules.end() ||
    DuktapeContext::_internalJSModules.find(moduleId) != DuktapeContext::_internalJSModules.end())
  {
    resolvedResult = moduleId;
    return true;
  }
  
  if (!(isSharedLib = OSS::string_ends_with(moduleId, ".so")))
  {
    isJs = OSS::string_ends_with(moduleId, ".js");
  }
  
  if (isSharedLib || isJs)
  {
    resolved = DuktapeContext::resolvePath(moduleId, resolvedResult);
  }
  else
  {
    std::string soFile = moduleId + ".so";
    if (!(resolved = DuktapeContext::resolvePath(soFile, resolvedResult)))
    {
      std::string jsFile = moduleId + ".js";
      resolved = DuktapeContext::resolvePath(jsFile, resolvedResult);
    }
  }
  
  if (resolved)
  {
    OSS_LOG_DEBUG("DuktapeContext::resolveModule - module resolved " << parentId << "::" << moduleId << " -> " << resolvedResult);
  }
  else
  {
    OSS_LOG_ERROR("DuktapeContext::resolveModule - Unable to resolve " << parentId << "::" << moduleId);
  }
  
  return resolved;
}

int DuktapeContext::loadModule(const std::string& moduleId)
{
  DuktapeModule* pModule = DuktapeContext::getModule(this, moduleId);
  if (!pModule)
  {
    return -1;
  }
  if (DuktapeContext::_internalModules.find(moduleId) != DuktapeContext::_internalModules.end())
  {
    pModule->callModuleInit(DuktapeContext::_internalModules[moduleId]);
    return 0;
  }
  else if (DuktapeContext::_internalJSModules.find(moduleId) != DuktapeContext::_internalJSModules.end())
  {
    if (!pModule->loadCode(DuktapeContext::_internalJSModules[moduleId]))
    {
      OSS_LOG_ERROR("DuktapeContext::loadModule - " << "Unable to load module " << moduleId);
      deleteModule(moduleId);
      return -1;
    }
    return 1;
  }
  else if (OSS::string_ends_with(moduleId, ".so"))
  {
    if (!pModule->loadLibrary(moduleId))
    {
      OSS_LOG_ERROR("DuktapeContext::loadModule - " << "Unable to load module " << moduleId);
      deleteModule(moduleId);
      return -1;
    }
    return 0;
  }
  else if (OSS::string_ends_with(moduleId, ".js"))
  {
    if (!pModule->loadJS(moduleId))
    {
      OSS_LOG_ERROR("DuktapeContext::loadModule - " << "Unable to load module " << moduleId);
      deleteModule(moduleId);
      return -1;
    }
    return 1;
  }
  return -1;
}

bool DuktapeContext::evalFile(const std::string& file, FILE* foutput, FILE* ferror)
{
  if (!boost::filesystem::exists(file.c_str()))
  {
    OSS_LOG_ERROR("DuktapeContext::evalFile - " << "Unable to locate file " << file);
    return false;
  }
  
  std::string body;
  std::ifstream js(file.c_str());
  js.seekg(0, std::ios::end);   
  body.reserve(js.tellg());
  js.seekg(0, std::ios::beg);
  body.assign((std::istreambuf_iterator<char>(js)), std::istreambuf_iterator<char>());
  
  duk_push_lstring(_pContext, body.c_str(), body.length());
  int r = duk_module_node_peval_file(_pContext, file.c_str(), 1); 
  duktape_dump_result(_pContext, r, foutput, ferror);
  return true;
}


} } } // OSS::JS::DUK



