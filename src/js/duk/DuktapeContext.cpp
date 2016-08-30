
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


#include <boost/thread/pthread/mutex.hpp>

#include "OSS/JS/DUK/DuktapeContext.h"
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace JS {
namespace DUK {



OSS::mutex_critic_sec DuktapeContext::_duk_mutex;
DuktapeContext::ContextMap DuktapeContext::_contextMap;
DuktapeContext::ModuleMap DuktapeContext::_moduleMap;


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
    pContext->resolveModule(parentId, moduleId, resolvedId);
  } 
  return 0;
}

static duk_ret_t duktape_load_module(duk_context* ctx)
{
  std::string resolvedId = duk_require_string(ctx, 0);
  DuktapeContext* pContext = DuktapeContext::getContext(ctx);
  if (pContext)
  {
    pContext->loadModule(resolvedId);
  }
  return 0;
}

DuktapeContext::DuktapeContext(const std::string& name) :
  _name(name),
  _pContext(0)
{
  OSS::mutex_critic_sec_lock lock(DuktapeContext::_duk_mutex);
  _pContext = duk_create_heap(NULL, // alloc function
      NULL, // realloc function
      NULL, // free function
      this, // user data
      duktape_fatal_handler); // fatal error handler
  DuktapeContext::_contextMap[(intptr_t)_pContext] = this;
  initCommonJS();
}

DuktapeContext::~DuktapeContext()
{
  OSS::mutex_critic_sec_lock lock(DuktapeContext::_duk_mutex);
  DuktapeContext::_contextMap.erase((intptr_t)_pContext);
  duk_destroy_heap(_pContext);
  _pContext = 0;
}

void DuktapeContext::initCommonJS()
{
  OSS::mutex_critic_sec_lock lock(DuktapeContext::_duk_mutex);
  duk_push_object(_pContext);
  duk_push_c_function(_pContext, duktape_resolve_module, DUK_VARARGS);
  duk_put_prop_string(_pContext, -2, "resolve");
  duk_push_c_function(_pContext, duktape_load_module, DUK_VARARGS);
  duk_put_prop_string(_pContext, -2, "load");
}

DuktapeContext* DuktapeContext::getContext(duk_context* ctx)
{
  OSS::mutex_critic_sec_lock lock(DuktapeContext::_duk_mutex);
  DuktapeContext::ContextMap::iterator iter = DuktapeContext::_contextMap.find((intptr_t)ctx);
  if (iter != DuktapeContext::_contextMap.end())
  {
    return iter->second;
  }
  return 0;
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


bool DuktapeContext::resolveModule(const std::string& parentId, const std::string& moduleId, std::string& resolvedResults)
{
  return false;
}

bool DuktapeContext::loadModule(const std::string& moduleId)
{
  DuktapeModule* pModule = DuktapeContext::getModule(this, moduleId);
  if (!pModule)
  {
    return false;
  }

  if (OSS::string_ends_with(moduleId, ".so"))
  {
    return pModule->loadLibrary(moduleId);
  }
  else if (OSS::string_ends_with(moduleId, ".js"))
  {
    return pModule->loadJS(moduleId);
  }
  return false;
}


} } } // OSS::JS::DUK



