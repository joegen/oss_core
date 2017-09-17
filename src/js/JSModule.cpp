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

#include "OSS/build.h"
#if ENABLE_FEATURE_V8

#include "v8.h"
#include "OSS/JS/JSBase.h"
#include "OSS/JS/JSModule.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace JS {


static JSModule& get_current_module_manager()
{
  return JSBase::GetCurrent()->getModuleManager();
}
  
static v8::Handle<v8::Value> js_include(const v8::Arguments& args) 
{
  v8::HandleScope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  
  for (int i = 0; i < args.Length(); i++) 
  {
    std::string fileName = string_from_js_value(args[i]);
    if (boost::filesystem::exists(fileName))
    {
      v8::Handle<v8::String>  script = read_file(fileName);
      v8::Handle<v8::Script> compiled = v8::Script::Compile(script);
      v8::Handle<v8::Value> result = compiled->Run();
      if (result.IsEmpty())
      {
        // The TryCatch above is still in effect and will have caught the error.
        report_js_exception(try_catch, true);
        return v8::Undefined();
      }
      return result;
    }
    else
    {
      OSS_LOG_ERROR("Unable to locate external script " << fileName);
    }
  }
  return v8::Undefined();
}

static std::string get_module_canonical_file_name(const std::string& fileName)
{
  try
  {
    std::string canonicalName = fileName;
    OSS::string_trim(canonicalName);
    
    JSModule::InternalModules& modules = get_current_module_manager().getInternalModules();
    JSModule::InternalModules::iterator iter = modules.find(fileName);
    if (iter != modules.end())
    {
      return fileName;
    }

    if (!OSS::string_ends_with(canonicalName, ".js"))
    {
      canonicalName += ".js";
    }

    if (OSS::string_starts_with(canonicalName, "/"))
    {
      return canonicalName;
    }

    if (OSS::string_starts_with(canonicalName, "~/"))
    {
      boost::filesystem::path currentPath(getenv("HOME"));
      currentPath = OSS::boost_path_concatenate(currentPath, canonicalName.substr(2, std::string::npos));
      return OSS::boost_path(currentPath);
    }
    
    if (OSS::string_starts_with(canonicalName, "./"))
    {
      boost::filesystem::path currentPath = boost::filesystem::current_path();
      currentPath = OSS::boost_path_concatenate(currentPath, canonicalName.substr(2, std::string::npos));
      return OSS::boost_path(currentPath);
    }

    boost::filesystem::path path(canonicalName.c_str());
    boost::filesystem::path absolutePath = boost::filesystem::absolute(path);
    if (boost::filesystem::exists(absolutePath))
    {
      return OSS::boost_path(absolutePath);
    }

    const std::string& modulesDir = get_current_module_manager().getModulesDir();
    if (!modulesDir.empty())
    {
      boost::filesystem::path modulesDir(modulesDir.c_str());
      absolutePath = OSS::boost_path_concatenate(modulesDir, canonicalName);
      if (boost::filesystem::exists(absolutePath))
      {
        return OSS::boost_path(absolutePath);
      }
    }
  }
  catch(...)
  {
  }
  return fileName;
}

static v8::Handle<v8::Value> js_get_module_cononical_file_name(const v8::Arguments& args) 
{
  if (args.Length() < 1)
  {
    return v8::Undefined();
  }
  v8::HandleScope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  std::string fileName = string_from_js_value(args[0]);
  std::string canonical = get_module_canonical_file_name(fileName);
  if (canonical.empty())
  {
    return v8::Undefined();
  }
  return v8::String::New(canonical.c_str());
}

static v8::Handle<v8::Value> js_get_module_script(const v8::Arguments& args) 
{
  if (args.Length() < 1)
  {
    return v8::Undefined();
  }
  
  v8::HandleScope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);

  std::string fileName = string_from_js_value(args[0]);
  
  JSModule::InternalModules& modules = get_current_module_manager().getInternalModules();
  JSModule::InternalModules::iterator iter = modules.find(fileName);
  if (iter != modules.end())
  {
    return v8::Handle<v8::String>(v8::String::New(iter->second.script.c_str()));
  }
  
  if (boost::filesystem::exists(fileName))
  {
    return read_file(fileName);
  }
  else
  {
    OSS_LOG_ERROR("Unable to locate module " << fileName);
  }
  return v8::Undefined();
}

static v8::Handle<v8::Value> js_compile(const v8::Arguments& args)
{
  if (args.Length() < 1)
  {
    return v8::Undefined();
  }
  v8::HandleScope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  
  v8::Handle<v8::String> script = v8::Handle<v8::String>::Cast(args[0]);
  v8::Handle<v8::Script> compiled = v8::Script::Compile(script, args[1]);
  
  v8::Handle<v8::Value> result = compiled->Run();
  if (result.IsEmpty())
  {
    // The TryCatch above is still in effect and will have caught the error.
    report_js_exception(try_catch, true);
    return v8::Undefined();
  }
  return result;
}

static v8::Handle<v8::Value> js_compile_module(const v8::Arguments& args)
{
  if (args.Length() < 1)
  {
    return v8::Undefined();
  }
  v8::HandleScope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  
  std::ostringstream strm;
  strm << "( function(module, exports) {";
  strm << string_from_js_value(args[0]);
  strm << "});";

  v8::Handle<v8::String> script(v8::String::New(strm.str().c_str())); 
  v8::Handle<v8::Script> compiled = v8::Script::New(script, args[1]);
  
  v8::Handle<v8::Value> result = compiled->Run();
  if (result.IsEmpty())
  {
    // The TryCatch above is still in effect and will have caught the error.
    report_js_exception(try_catch, true);
    return v8::Undefined();
  }
  return result;
}

JSModule::JSModule(JSBase* pBase) :
  _pBase(pBase)
{
}

JSModule::~JSModule()
{
}

bool JSModule::initialize(v8::TryCatch& try_catch, v8::Handle<v8::ObjectTemplate>& global)
{
  //
  // Register the helpers
  //
  Module modules_js;
  modules_js.name = "modules.js";
  modules_js.script = std::string(
    #include "js/OSSJS_modules.js.h"
  );
  registerModuleHelper(modules_js);
  
  //
  // Register internal modules
  //
  Module module;
  module.name = "logger";
  module.script = std::string(
    #include "js/OSSJS_logger.js.h"
  );
  registerInternalModule(module);
  
  module.name = "object";
  module.script = std::string(
    #include "js/OSSJS_object.js.h"
  );
  registerInternalModule(module);
  
  module.name = "assert";
  module.script = std::string(
    #include "js/OSSJS_assert.js.h"
  );
  registerInternalModule(module);
  
  return compileModuleHelpers(try_catch, global);
}

void JSModule::registerInternalModule(const Module& module)
{
  assert(_modules.find(module.name) == _modules.end());
  _modules[module.name] = module;
}

void JSModule::registerModuleHelper(const Module& module)
{
  _moduleHelpers.push_back(module);
}

bool JSModule::setGlobals(v8::Handle<v8::ObjectTemplate>& global)
{
  global->Set(v8::String::New("__include"), v8::FunctionTemplate::New(js_include));
  global->Set(v8::String::New("__compile"), v8::FunctionTemplate::New(js_compile));
  global->Set(v8::String::New("__compile_module"), v8::FunctionTemplate::New(js_compile_module));
  global->Set(v8::String::New("__get_module_script"), v8::FunctionTemplate::New(js_get_module_script));
  global->Set(v8::String::New("__get_module_cononical_file_name"), v8::FunctionTemplate::New(js_get_module_cononical_file_name));
  return true;
}

bool JSModule::compileModuleHelpers(v8::TryCatch& try_catch, v8::Handle<v8::ObjectTemplate>& global)
{
  for (ModuleHelpers::iterator iter = _moduleHelpers.begin(); iter != _moduleHelpers.end(); iter++)
  {
    v8::Handle<v8::String> script(v8::String::New(iter->script.c_str()));
    v8::Handle<v8::Value> name(v8::String::New(iter->name.c_str()));
    v8::Handle<v8::Script> compiled = v8::Script::Compile(script, name);

    v8::Handle<v8::Value> result = compiled->Run();
    if (result.IsEmpty())
    {
      // The TryCatch above is still in effect and will have caught the error.
      report_js_exception(try_catch, true);
      return false;
    }
  } 

  return true;
}
  


} }

#endif // ENABLE_FEATURE_V8


