
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
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace JS {
namespace DUK {



OSS::mutex_critic_sec DuktapeContext::_duk_mutex;
DuktapeContext::ModuleMap DuktapeContext::_moduleMap;
DuktapeContext::ModuleDirectories DuktapeContext::_moduleDirectories;
DuktapeContext::InternalModules DuktapeContext::_internalModules;


static void duktape_fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg) 
{ 
  OSS_LOG_FATAL("Duktape Fatal Error!  "  << "Code: " << code << " Msg:" << (const char *) (msg ? msg : "null"));
  abort();
}

static duk_ret_t duktape_resolve_module(duk_context* ctx) 
{
  std::string moduleId = duk_get_string(ctx, 0);
  std::string parentId = duk_get_string(ctx, 1);
  std::string resolvedId;
  
  DuktapeContext* pContext = DuktapeContext::getContext(ctx);
  if (pContext)
  {
    if (pContext->resolveModule(parentId, moduleId, resolvedId))
    {
      duk_push_string(ctx, resolvedId.c_str());
      return 1;
    }
  }
  duk_error(ctx, DUK_ERR_ERROR, "Module not found");
  return -42;
}

static duk_ret_t duktape_load_module(duk_context* ctx)
{
  std::string resolvedId = duk_require_string(ctx, 0);
  DuktapeContext* pContext = DuktapeContext::getContext(ctx);
  if (pContext)
  {
    return pContext->loadModule(resolvedId);
  }
  return -1;
}

static int duktape_get_error_stack(duk_context *ctx) 
{
    if (!duk_is_object(ctx, -1)) {
        return 1;
    }

    if (!duk_has_prop_string(ctx, -1, "stack")) {
        return 1;
    }

    if (!duk_is_error(ctx, -1)) {
        /* Not an Error instance, don't read "stack" */
        return 1;
    }

    duk_get_prop_string(ctx, -1, "stack");  /* caller coerces */
    duk_remove(ctx, -2);
    return 1;
}


static void duktape_dump_result(duk_context* ctx, int r, FILE* foutput, FILE* ferror) 
{
    if (r != DUK_EXEC_SUCCESS) 
    {
      if (ferror) 
      {
        duk_safe_call(ctx, duktape_get_error_stack, 1 /*nargs*/, 1 /*nrets*/);
        fprintf(ferror, "%s\n", duk_safe_to_string(ctx, -1));
        fflush(ferror);
      }
    } 
    else 
    {
      if (foutput) 
      {
        /* TODO: make this optional with a parameter? */
        /* beautify output */
        duk_eval_string(ctx, "(function (v) {\n"
                             "    try {\n"
                             "        return Duktape.enc('jx', v, null, 4);\n"
                             "    } catch (e) {\n"
                             "        return ''+v;\n"
                             "    }\n"
                             "})");
        duk_insert(ctx, -2);
        duk_call(ctx, 1);

        fprintf(foutput, "= %s\n", duk_safe_to_string(ctx, -1));
        fflush(foutput);
      }
    }
    duk_pop(ctx);
}

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
  initCommonJS();
}

DuktapeContext::~DuktapeContext()
{
  OSS::mutex_critic_sec_lock lock(DuktapeContext::_duk_mutex);
  duk_destroy_heap(_pContext);
  _pContext = 0;
}

void DuktapeContext::initCommonJS()
{
  duk_push_object(_pContext);
  duk_push_c_function(_pContext, duktape_resolve_module, DUK_VARARGS);
  duk_put_prop_string(_pContext, -2, "resolve");
  duk_push_c_function(_pContext, duktape_load_module, DUK_VARARGS);
  duk_put_prop_string(_pContext, -2, "load");
  duk_module_node_init(_pContext);
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
  
  if (DuktapeContext::_internalModules.find(moduleId) != DuktapeContext::_internalModules.end())
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



