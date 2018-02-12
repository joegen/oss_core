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
#include "OSS/UTL/CoreUtils.h"
#if ENABLE_FEATURE_V8

#include "OSS/JS/JS.h"
#include "OSS/JS/JSModule.h"
#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSUtil.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/JSIsolate.h"
#include "OSS/JS/JSIsolateManager.h"


namespace OSS {
namespace JS {


boost::filesystem::path JSModule::_mainScript;
  
static JSModule*get_current_module_manager()
{
  return JSIsolateManager::instance().getIsolate()->getModuleManager();
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

static bool module_path_exists(const std::string& canonicalName, std::string& absolutePath)
{
  if (OSS::string_starts_with(canonicalName, "/"))
  {
    if (boost::filesystem::exists(boost::filesystem::path(canonicalName.c_str())))
    {
      absolutePath = canonicalName;
      return true;
    }
    return false;
  }

  if (OSS::string_starts_with(canonicalName, "~/"))
  {
    boost::filesystem::path currentPath(getenv("HOME"));
    currentPath = OSS::boost_path_concatenate(currentPath, canonicalName.substr(2, std::string::npos));
    if (boost::filesystem::exists(currentPath))
    {
      absolutePath = OSS::boost_path(currentPath);
      return true;
    }
    return false;
  }

  if (OSS::string_starts_with(canonicalName, "./"))
  {
    boost::filesystem::path currentPath = boost::filesystem::current_path();
    currentPath = OSS::boost_path_concatenate(currentPath, canonicalName.substr(2, std::string::npos));
    if (boost::filesystem::exists(currentPath))
    {
      absolutePath = OSS::boost_path(currentPath);
      return true;
    }
    return false;
  }

  boost::filesystem::path path(canonicalName.c_str());
  boost::filesystem::path absPath = boost::filesystem::absolute(path);
  if (boost::filesystem::exists(absPath))
  {
    absolutePath = OSS::boost_path(absPath);
    return true;
  }

  //
  // check it against the directory of the main script
  //
  boost::filesystem::path parent_path = JSModule::_mainScript.parent_path();
  absPath = OSS::boost_path_concatenate(parent_path, canonicalName);
  if (boost::filesystem::exists(absPath))
  {
    absolutePath = OSS::boost_path(absPath);
    return true;
  }
  
  //
  // check it against the oss_modules directory of the main script
  //
  parent_path = JSModule::_mainScript.parent_path();
  boost::filesystem::path module_path = OSS::boost_path_concatenate(parent_path, "oss_modules");
  absPath = OSS::boost_path_concatenate(module_path, canonicalName);
  if (boost::filesystem::exists(absPath))
  {
    absolutePath = OSS::boost_path(absPath);
    return true;
  }

  //
  // Check it against current path
  //
  absPath = OSS::boost_path_concatenate(boost::filesystem::current_path(), canonicalName);
  if (boost::filesystem::exists(absPath))
  {
    absolutePath = OSS::boost_path(absPath);
    return true;
  }

  
  //
  // Check the global module directory
  //
  const JSModule::ModulesDir& modulesDir = get_current_module_manager()->getModulesDir();
  for (JSModule::ModulesDir::const_iterator iter = modulesDir.begin(); iter != modulesDir.end(); iter++)
  {
    boost::filesystem::path modDir(iter->c_str());
    absPath = OSS::boost_path_concatenate(modDir, canonicalName);
    if (boost::filesystem::exists(absPath))
    {
      absolutePath = OSS::boost_path(absPath);
      return true;
    }
  }

  return false;
}

JS_METHOD_IMPL(__add_module_directory) 
{
  v8::HandleScope scope;
  js_method_arg_declare_string(path, 0);
  get_current_module_manager()->setModulesDir(path);
  return JSUndefined();
}

static std::string get_plugin_canonical_file_name(const std::string& fileName)
{
  try
  {
    std::string canonicalName = fileName;
    OSS::string_trim(canonicalName);
    
    if (!OSS::string_ends_with(canonicalName, ".jso"))
    {
      canonicalName += ".jso";
    }

    std::string absolutePath;
    if (module_path_exists(canonicalName, absolutePath))
    {
      return absolutePath;
    }
  }
  catch(...)
  {
  }
  return fileName;
}

static std::string get_directory_module_canonical_file_name(const std::string& fileName)
{
  try
  {
    std::string canonicalName = fileName;
    OSS::string_trim(canonicalName);
    
    if (!OSS::string_ends_with(canonicalName, "/index.js"))
    {
      canonicalName += "/index.js";
    }

    std::string absolutePath;
    if (module_path_exists(canonicalName, absolutePath))
    {
      return absolutePath;
    }
  }
  catch(...)
  {
  }
  return fileName;
}

static std::string get_module_canonical_file_name(const std::string& fileName)
{
  try
  {
    std::string canonicalName = fileName;
    OSS::string_trim(canonicalName);
    
    JSModule::InternalModules& modules = get_current_module_manager()->getInternalModules();
    JSModule::InternalModules::iterator iter = modules.find(fileName);
    if (iter != modules.end())
    {
      return fileName;
    }
    
    if (OSS::string_ends_with(canonicalName, ".jso"))
    {
      return get_plugin_canonical_file_name(fileName);
    }

    if (!OSS::string_ends_with(canonicalName, ".js"))
    {
      canonicalName += ".js";
    }

    std::string absolutePath;
    if (module_path_exists(canonicalName, absolutePath))
    {
      return absolutePath;
    }
  }
  catch(...)
  {
  }
  
  boost::filesystem::path directoryModule(get_directory_module_canonical_file_name(fileName));
  if (boost::filesystem::exists(directoryModule))
  {
    return OSS::boost_path(directoryModule);
  }
  
  return get_plugin_canonical_file_name(fileName);
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

static v8::Handle<v8::Value> js_load_plugin(const v8::Arguments& args) 
{
  if (args.Length() < 1)
  {
    return v8::Undefined();
  }
  v8::HandleScope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  std::string fileName = string_from_js_value(args[0]);
  js_method_declare_isolate(pIsolate);
  JSPlugin* pPlugin = pIsolate->getPluginManager()->loadPlugin(fileName);
  if (!pPlugin)
  {
    return v8::Undefined();
  }
  
  std::string exportFunc;
  if (pPlugin->initExportFunc(exportFunc))
  {
    v8::Handle<v8::String> func_name = v8::String::New(exportFunc.c_str());
    return js_get_global()->Get(func_name);
  }
  
  return v8::Undefined();
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
  
  JSModule::InternalModules& modules = get_current_module_manager()->getInternalModules();
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
  strm << "\"use-strict\";";
  strm << "try {";
  strm << string_from_js_value(args[0]);
  strm << "} catch(e) { e.printStackTrace(); }";
  strm << "});";

  v8::Handle<v8::String> script(v8::String::New(strm.str().c_str())); 
  v8::Handle<v8::Script> compiled = v8::Script::New(script, args[1]);
  
  std::string fileName = *v8::String::Utf8Value(args[1]);
  boost::filesystem::path path(fileName.c_str());
  boost::filesystem::path parent_path = path.parent_path();
  boost::filesystem::path current_path = boost::filesystem::current_path();
  v8::Handle<v8::Value> result = compiled->Run();

  if (result.IsEmpty())
  {
    // The TryCatch above is still in effect and will have caught the error.
    report_js_exception(try_catch, true);
    return v8::Undefined();
  }
  return result;
}

JS_METHOD_IMPL(js_lock_isolate)
{
  JSIsolateManager::instance().modulesMutex().lock();
  return JSUndefined();
}

JS_METHOD_IMPL(js_unlock_isolate)
{
  JSIsolateManager::instance().modulesMutex().unlock();
  return JSUndefined();
}

JSModule::JSModule(JSIsolate* pIsolate) :
  _pIsolate(pIsolate)
{
  _modulesDir.push_back(OSS::system_libdir() + "/oss_modules");
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

JS_METHOD_IMPL(__chdir)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string dir = js_method_arg_as_std_string(0);
  return JSInt32(chdir(dir.c_str()));
}

JS_METHOD_IMPL(__current_path)
{
  js_enter_scope();
  boost::filesystem::path path = boost::filesystem::current_path();
  return JSString(OSS::boost_path(path).c_str());
}

JS_METHOD_IMPL(__parent_path)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string pathStr = js_method_arg_as_std_string(0);
  boost::filesystem::path path(pathStr.c_str());
  boost::filesystem::path parent = path.parent_path();
  return JSString(OSS::boost_path(parent).c_str());
}

bool JSModule::initGlobalExports(v8::Handle<v8::ObjectTemplate>& global)
{
  global->Set(v8::String::New("__include"), v8::FunctionTemplate::New(js_include));
  global->Set(v8::String::New("__compile"), v8::FunctionTemplate::New(js_compile));
  global->Set(v8::String::New("__compile_module"), v8::FunctionTemplate::New(js_compile_module));
  global->Set(v8::String::New("__get_module_script"), v8::FunctionTemplate::New(js_get_module_script));
  global->Set(v8::String::New("__get_module_cononical_file_name"), v8::FunctionTemplate::New(js_get_module_cononical_file_name));
  global->Set(v8::String::New("__load_plugin"), v8::FunctionTemplate::New(js_load_plugin));
  global->Set(v8::String::New("__current_path"), v8::FunctionTemplate::New(__current_path));
  global->Set(v8::String::New("__parent_path"), v8::FunctionTemplate::New(__parent_path));
  global->Set(v8::String::New("__chdir"), v8::FunctionTemplate::New(__chdir));
  global->Set(v8::String::New("__lock_isolate"), v8::FunctionTemplate::New(js_lock_isolate));
  global->Set(v8::String::New("__unlock_isolate"), v8::FunctionTemplate::New(js_unlock_isolate));
  global->Set(v8::String::New("__add_module_directory"), v8::FunctionTemplate::New(__add_module_directory));
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
      OSS_LOG_ERROR("JSModule::compileModuleHelpers is unable to compile " << iter->name);
      // The TryCatch above is still in effect and will have caught the error.
      report_js_exception(try_catch, true);
      return false;
    }
  } 
  return true;
}

JSIsolate* JSModule::getIsolate()
{
  return _pIsolate;
}




} }

#endif // ENABLE_FEATURE_V8


