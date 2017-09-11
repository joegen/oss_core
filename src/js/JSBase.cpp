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


#include "OSS/JS/JSBase.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Application.h"
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/Thread.h"


#define ENABLE_GLOBAL_SCRIPTS_DIR 0


namespace OSS {
namespace JS {


std::vector<std::string> JSBase::_globalScripts; 
JSBase::InternalModules JSBase::_modules;
JSBase::ModuleHelpers JSBase::_moduleHelpers;
std::string JSBase::_modulesDir;
  
static std::string toString(v8::Handle<v8::Value> str)
{
  if (!str->IsString())
    return "";
  v8::String::Utf8Value value(str);
  return *value;
}

std::string jsvalToString(const jsval& str)
{
  if (!str->IsString())
    return "";
  jsstringutf8 value(str);
  return *value;
}

const char* toCString(const v8::String::Utf8Value& value)
{
  return *value ? *value : "<str conversion failed>";
}

static void reportException(v8::TryCatch &try_catch, bool show_line)
{
  using namespace v8;
  
  Handle<Message> message = try_catch.Message();

  v8::String::Utf8Value error(try_catch.Exception());
  if (error.length() > 0)
  {

    std::ostringstream errorMsg;

    /*
     02:50:22.863: [CID=00000000] JS: File undefined:12
    [CID=00000000] JS: Source Line   var notheetoo = nothere.subst(0, 10);
    [CID=00000000] JS:
                              ^
    [CID=00000000] JS:
    {TypeError: Cannot call method 'subst' of undefined
        at RouteProfile.isTellMeRoutable (unknown source)
        at RouteProfile.isRoutable (unknown source)
        at handle_request (unknown source)
    }
    */

    if (show_line && !message.IsEmpty())
    {
      // Print (filename):(line number): (message).
      String::Utf8Value filename(message->GetScriptResourceName());
      const char* filename_string = toCString(filename);
      int linenum = message->GetLineNumber();
      //fprintf(stderr, "%s:%i\n", filename_string, linenum);

      errorMsg << filename_string << ":" << linenum << std::endl;
      // Print line of source code.
      String::Utf8Value sourceline(message->GetSourceLine());
      const char* sourceline_string = toCString(sourceline);

      // HACK HACK HACK
      //
      // FIXME
      //
      // Because of how CommonJS modules work, all scripts are wrapped with a
      // "function (function (exports, __filename, ...) {"
      // to provide script local variables.
      //
      // When reporting errors on the first line of a script, this wrapper
      // function is leaked to the user. This HACK is to remove it. The length
      // of the wrapper is 62. That wrapper is defined in src/node.js
      //
      // If that wrapper is ever changed, then this number also has to be
      // updated. Or - someone could clean this up so that the two peices
      // don't need to be changed.
      //
      // Even better would be to get support into V8 for wrappers that
      // shouldn't be reported to users.
      int offset = linenum == 1 ? 62 : 0;

      //fprintf(stderr, "%s\n", sourceline_string + offset);
      errorMsg << sourceline_string + offset << std::endl;
      // Print wavy underline (GetUnderline is deprecated).
      int start = message->GetStartColumn();
      for (int i = offset; i < start; i++)
      {
        errorMsg << " ";
      }
      int end = message->GetEndColumn();
      for (int i = start; i < end; i++)
      {
        errorMsg << "^";
      }
      errorMsg << std::endl;
    }

    String::Utf8Value trace(try_catch.StackTrace());

    if (trace.length() > 0)
    {
      errorMsg << *trace;
    }
    OSS_LOG_ERROR("[CID=00000000]\tJS: " << *error << std::endl << "{" << std::endl << errorMsg.str() << std::endl << "}");
  }
  else
  {
    OSS_LOG_ERROR("[CID=00000000]\tJS: Unknown Exception");
  }
}



static v8::Handle<v8::Value> log_info_callback(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  if (args.Length() == 2)
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg0 = args[0];
    v8::String::Utf8Value value0(arg0);

    v8::Handle<v8::Value> arg1 = args[1];
    v8::String::Utf8Value value1(arg1);
    std::ostringstream msg;
    msg << *value0 << " " << *value1;
    OSS::log_information(msg.str());
  }
  else
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg = args[0];
    v8::String::Utf8Value value(arg);
    std::ostringstream msg;
    msg << *value;
    OSS::log_information(msg.str());
  }
  
  return v8::Undefined();
}

static v8::Handle<v8::Value> log_debug_callback(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  if (args.Length() == 2)
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg0 = args[0];
    v8::String::Utf8Value value0(arg0);

    v8::Handle<v8::Value> arg1 = args[1];
    v8::String::Utf8Value value1(arg1);
    std::ostringstream msg;
    msg << *value0 << " " << *value1;
    OSS::log_debug(msg.str());
  }
  else
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg = args[0];
    v8::String::Utf8Value value(arg);
    std::ostringstream msg;
    msg << *value;
    OSS::log_debug(msg.str());
  }

  return v8::Undefined();
}

static v8::Handle<v8::Value> log_error_callback(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  if (args.Length() == 2)
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg0 = args[0];
    v8::String::Utf8Value value0(arg0);

    v8::Handle<v8::Value> arg1 = args[1];
    v8::String::Utf8Value value1(arg1);
    std::ostringstream msg;
    msg << *value0 << "JavaScript Error: " << *value1;
    OSS::log_error(msg.str());
  }
  else
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg = args[0];
    v8::String::Utf8Value value(arg);
    std::ostringstream msg;
    msg << "JavaScript Error: " << *value;
    OSS::log_error(msg.str());
  }
  return v8::Undefined();
}

// Reads a file into a v8 string.
static v8::Handle<v8::String> read_file(const std::string& name) {
  FILE* file = fopen(name.c_str(), "rb");
  if (file == NULL) return v8::Handle<v8::String>();

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);

  char* chars = new char[size + 1];
  chars[size] = '\0';
  for (int i = 0; i < size;) {
    int read = fread(&chars[i], 1, size - i, file);
    i += read;
  }
  fclose(file);
  v8::Handle<v8::String> result = v8::String::New(chars, size);
  delete[] chars;
  return result;
}

static v8::Handle<v8::String> read_global_scripts()
{
  std::ostringstream data;
  
  for (std::vector<std::string>::iterator iter = JSBase::_globalScripts.begin(); iter != JSBase::_globalScripts.end(); iter++)
  {
    data << *iter << std::endl;
  }
  
  return  v8::String::New(data.str().c_str(), data.str().size());
}

static v8::Handle<v8::String> read_directory(const boost::filesystem::path& directory)
{
  std::string data;

  if (boost::filesystem::exists(directory))
  {
    try
    {
      boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
      for (boost::filesystem::directory_iterator itr(directory); itr != end_itr; ++itr)
      {
        if (boost::filesystem::is_directory(itr->status()))
        {
          continue;
        }
        else
        {
          boost::filesystem::path currentFile = itr->path();
          std::string fileName = OSS::boost_file_name(currentFile);
          if (boost::filesystem::is_regular(currentFile))
          {
            if (OSS::string_ends_with(fileName, ".js"))
            {
              //OSS_LOG_INFO("Google V8 is loading " << currentFile);
              FILE* file = fopen(OSS::boost_path(currentFile).c_str(), "rb");
              if (file == NULL)
              {
                OSS_LOG_ERROR("Google V8 failed to open file " << currentFile);
                return v8::Handle<v8::String>();
              }

              fseek(file, 0, SEEK_END);
              int size = ftell(file);
              rewind(file);

              char* chars = new char[size + 1];
              chars[size] = '\0';
              for (int i = 0; i < size;) {
                int read = fread(&chars[i], 1, size - i, file);
                i += read;
              }
              fclose(file);
              data += chars;
              //OSS_LOG_INFO("Google V8 " << currentFile << " LOADED");
              delete[] chars;
            }

          }
        }
      }
    }
    catch(std::exception& e)
    {
      OSS_LOG_WARNING("Googe V8 exception: " << e.what());
    }
    catch(...)
    {
      OSS_LOG_WARNING("Unknown Googe V8 exception.");
    }

  }
  return  v8::String::New(data.c_str(), data.size());
}

static jsval js_include(const jsargs& args) 
{
  jsscope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  
  for (int i = 0; i < args.Length(); i++) 
  {
    std::string fileName = jsvalToString(args[i]);
    if (boost::filesystem::exists(fileName))
    {
      v8::Handle<v8::String>  script = read_file(fileName);
      v8::Handle<v8::Script> compiled = v8::Script::Compile(script);
      v8::Handle<v8::Value> result = compiled->Run();
      if (result.IsEmpty())
      {
        // The TryCatch above is still in effect and will have caught the error.
        reportException(try_catch, true);
        return jsvoid();
      }
      return result;
    }
    else
    {
      OSS_LOG_ERROR("Unable to locate external script " << fileName);
    }
  }
  return jsvoid();
}

static std::string get_module_canonical_file_name(const std::string& fileName)
{
  try
  {
    std::string canonicalName = fileName;
    OSS::string_trim(canonicalName);

    JSBase::InternalModules::iterator iter = JSBase::_modules.find(fileName);
    if (iter != JSBase::_modules.end())
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

    if (!JSBase::_modulesDir.empty())
    {
      boost::filesystem::path modulesDir(JSBase::_modulesDir.c_str());
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

static jsval js_get_module_cononical_file_name(const jsargs& args) 
{
  if (args.Length() < 1)
  {
    return jsvoid();
  }
  jsscope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  std::string fileName = jsvalToString(args[0]);
  std::string canonical = get_module_canonical_file_name(fileName);
  if (canonical.empty())
  {
    return jsvoid();
  }
  return jsstring::New(canonical.c_str());
}

static jsval js_get_module_script(const jsargs& args) 
{
  if (args.Length() < 1)
  {
    return jsvoid();
  }
  
  jsscope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);

  std::string fileName = jsvalToString(args[0]);
  
  JSBase::InternalModules::iterator iter = JSBase::_modules.find(fileName);
  if (iter != JSBase::_modules.end())
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
  return jsvoid();
}

static jsval js_compile(const jsargs& args)
{
  if (args.Length() < 1)
  {
    return jsvoid();
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
    reportException(try_catch, true);
    return jsvoid();
  }
  return result;
}

static jsval js_compile_module(const jsargs& args)
{
  if (args.Length() < 1)
  {
    return jsvoid();
  }
  v8::HandleScope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  
  std::ostringstream strm;
  strm << "( function(module, exports) {";
  strm << jsvalToString(args[0]);
  strm << "});";

  v8::Handle<v8::String> script(v8::String::New(strm.str().c_str())); 
  v8::Handle<v8::Script> compiled = v8::Script::New(script, args[1]);
  
  v8::Handle<v8::Value> result = compiled->Run();
  if (result.IsEmpty())
  {
    // The TryCatch above is still in effect and will have caught the error.
    reportException(try_catch, true);
    return jsvoid();
  }
  return result;
}


static std::string V8ErrorReport;
static bool _hasSetErrorCB = false;
static void V8ErrorMessageCallback(v8::Handle<v8::Message> message,
v8::Handle<v8::Value> data)
{
  v8::HandleScope handle_scope;
  std::string error =
          + " Javascript error on line " + OSS::string_from_number(message->GetLineNumber())
          + " : " + toString(message->GetSourceLine());
  OSS::log_error(error);
}

JSBase::JSBase(const std::string& contextName) :
  _contextName(contextName),
  _context(0),
  _processFunc(0),
  _requestTemplate(0),
  _globalTemplate(0),
  _isInitialized(false),
  _extensionGlobals(0)
{
}

JSBase::~JSBase()
{
  if (_globalTemplate)
  {
    static_cast<v8::Persistent<v8::ObjectTemplate>*>(_globalTemplate)->Dispose();
    static_cast<v8::Persistent<v8::ObjectTemplate>*>(_requestTemplate)->Dispose();
    if (_processFunc)
    {
      static_cast<v8::Persistent<v8::Function>*>(_processFunc)->Dispose();
    }
    static_cast<v8::Persistent<v8::Context>*>(_context)->Dispose();
  }

  delete static_cast<v8::Persistent<v8::ObjectTemplate>*>(_globalTemplate);
  delete static_cast<v8::Persistent<v8::ObjectTemplate>*>(_requestTemplate);
  delete static_cast<v8::Persistent<v8::Function>*>(_processFunc);
  delete static_cast<v8::Persistent<v8::Context>*>(_context);

}

bool JSBase::initModules()
{
  //
  // Register the helpers
  //
  Module modules_js;
  modules_js.name = "modules.js";
  modules_js.script = std::string(
    #include "js/OSSJS_modules.js.h"
  );
  JSBase::registerModuleHelper(modules_js);
  
  //
  // Register internal modules
  //
  Module logger_js;
  logger_js.name = "logger";
  logger_js.script = std::string(
    #include "js/OSSJS_logger.js.h"
  );
  JSBase::registerInternalModule(logger_js);
  return true;
}

void JSBase::addGlobalScript(const std::string& script)
{
  _globalScripts.push_back(script);
}

bool JSBase::initialize(const boost::filesystem::path& scriptFile, const std::string& functionName,
  void(*extensionGlobals)(OSS_HANDLE), const std::string& preloaded )
{
  _preloaded = preloaded;
  return internalInitialize(scriptFile, functionName, extensionGlobals, _preloaded);
}

bool JSBase::internalInitialize(
  const boost::filesystem::path& scriptFile, const std::string& functionName,
  void(*extensionGlobals)(OSS_HANDLE), const std::string& preloaded)
{
  v8::Locker __v8Locker__;
  
  if (preloaded.empty() && !boost::filesystem::exists(scriptFile))
  {
    OSS_LOG_ERROR("Unable to locate file " << scriptFile);
    return false;
  }

  //OSS_LOG_INFO("Google V8 JSBase::internalInitialize INVOKED");

  // Create a handle scope to hold the temporary references.
  v8::HandleScope handle_scope;
  
  v8::Persistent<v8::ObjectTemplate>* oldGlobalTemplate = static_cast<v8::Persistent<v8::ObjectTemplate>*>(_globalTemplate);
  v8::Persistent<v8::ObjectTemplate>* oldRequestTemplate = static_cast<v8::Persistent<v8::ObjectTemplate>*>(_requestTemplate);
  v8::Persistent<v8::Function>* oldProcessFunc = static_cast<v8::Persistent<v8::Function>*>(_processFunc);
  v8::Persistent<v8::Context>* oldContext = static_cast<v8::Persistent<v8::Context>*>(_context);

  if (oldContext)
  {
    (*oldContext)->DetachGlobal();
    oldContext->Dispose();
    delete static_cast<v8::Persistent<v8::Context>*>(oldContext);
    _context = 0;
  }
  
  if (oldGlobalTemplate)
  {
    oldGlobalTemplate->Dispose();
    delete static_cast<v8::Persistent<v8::ObjectTemplate>*>(oldGlobalTemplate);
    _globalTemplate = 0;
  }

  if (oldRequestTemplate)
  {
    oldRequestTemplate->Dispose();
    delete static_cast<v8::Persistent<v8::ObjectTemplate>*>(oldRequestTemplate);
    _requestTemplate = 0;
  }

  if (oldProcessFunc)
  {
    oldProcessFunc->Dispose();
    delete static_cast<v8::Persistent<v8::Function>*>(oldProcessFunc);
    _processFunc = 0;
  }


  v8::Persistent<v8::Function>* processFunc_ = 0;
  if (!functionName.empty())
  {
    processFunc_ = new v8::Persistent<v8::Function>();
  }
  
  v8::Persistent<v8::Context>* context_ = new v8::Persistent<v8::Context>();
  v8::Persistent<v8::ObjectTemplate>* requestTemplate_ = new v8::Persistent<v8::ObjectTemplate>;
  v8::Persistent<v8::ObjectTemplate>* globalTemplate_ = new v8::Persistent<v8::ObjectTemplate>;
  _context = context_;
  _processFunc = processFunc_;
  _requestTemplate = requestTemplate_;
  _globalTemplate = globalTemplate_;
  _functionName = functionName;
  _script = scriptFile;
  _extensionGlobals = extensionGlobals;

  if (!_hasSetErrorCB)
  {
    if (!_hasSetErrorCB)
      v8::V8::AddMessageListener(V8ErrorMessageCallback);
  }
  
  

  //OSS_LOG_INFO("Google V8 is loading context for " << _script);
  // Create a template for the global object where we set the
  // built-in global functions.
  //v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
  v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
  *(static_cast<v8::Persistent<v8::ObjectTemplate>*>(_globalTemplate)) = v8::Persistent<v8::ObjectTemplate>::New(global);
  
  global->Set(v8::String::New("__include"), v8::FunctionTemplate::New(js_include));
  global->Set(v8::String::New("__compile"), v8::FunctionTemplate::New(js_compile));
  global->Set(v8::String::New("__compile_module"), v8::FunctionTemplate::New(js_compile_module));
  global->Set(v8::String::New("__get_module_script"), v8::FunctionTemplate::New(js_get_module_script));
  global->Set(v8::String::New("__get_module_cononical_file_name"), v8::FunctionTemplate::New(js_get_module_cononical_file_name));
  
  global->Set(v8::String::New("log_info"), v8::FunctionTemplate::New(log_info_callback));
  global->Set(v8::String::New("log_debug"), v8::FunctionTemplate::New(log_debug_callback));
  global->Set(v8::String::New("log_error"), v8::FunctionTemplate::New(log_error_callback));
  
  //
  // Initialize subclass global functions
  //
  //OSS_LOG_INFO("Google V8 is initializing global exports for " << _script);
  initGlobalFuncs(_globalTemplate);

  //
  // Initialize extension funcs
  //
  if (_extensionGlobals)
    _extensionGlobals(_globalTemplate);

  // Each processor gets its own context so different processors
  // don't affect each other (ignore the first three lines).
  v8::Handle<v8::Context> context = v8::Context::New(0, global);

  // Store the context in the processor object in a persistent handle,
  // since we want the reference to remain after we return from this
  // method.
  *(static_cast<v8::Persistent<v8::Context>*>(_context)) = v8::Persistent<v8::Context>::New(context);

  // Enter the new context so all the following operations take place
  // within it.
  v8::Context::Scope context_scope(context);

  //OSS_LOG_INFO("Google V8 context for " << _script << " CREATED");

  //
  // We're just about to compile the script; set up an error handler to
  // catch any exceptions the script might throw.
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);

  //
  // Compile global helpers
  //
  
  boost::filesystem::path helpers;
  if (!_globalScriptsDirectory.empty())
    helpers = boost::filesystem::path(_globalScriptsDirectory);
  else
    helpers = operator/(scriptFile.branch_path(), "global.detail");

  
  try
  {
    //
    // Compile it!
    //
    v8::Handle<v8::String> helperScript;

    if (!ENABLE_GLOBAL_SCRIPTS_DIR || !boost::filesystem::exists(helpers))
      helperScript = read_global_scripts();
    else
      helperScript = read_directory(helpers);


    if (helperScript.IsEmpty())
    {
      OSS_LOG_ERROR("Failed to compile global exports for " << _script);
      // The script failed to compile; bail out.
      return false;
    }

    //OSS_LOG_INFO("Google V8 is compiling global.detail for " << _script);
    v8::Handle<v8::Script> compiledHelper = v8::Script::Compile(helperScript);
    if (compiledHelper.IsEmpty())
    {
      reportException(try_catch, true);
      return false;
    }

   // OSS_LOG_INFO("Google V8 is running global.detail for " << _script);
     // Run the script!
    v8::Handle<v8::Value> result = compiledHelper->Run();
    if (result.IsEmpty())
    {
      // The TryCatch above is still in effect and will have caught the error.
      reportException(try_catch, true);
      return false;
    }
   // OSS_LOG_INFO("Google V8 global.detail for " << _script << " EXECUTED");
  }
  catch(OSS::Exception e)
  {
    std::ostringstream logMsg;
    logMsg << "Filesystem error while compiling script global helpers - " << e.message();
    OSS::log_warning(logMsg.str());
  }

  //
  // Compile the helpers
  //
  if (!_helperScriptsDirectory.empty())
    helpers = boost::filesystem::path(_helperScriptsDirectory);
  else
    helpers = OSS::boost_path(_script) + ".detail";
  if (boost::filesystem::exists(helpers))
  {
    //
    // This script has a heper directory
    //
    try
    {
      boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
      for (boost::filesystem::directory_iterator itr(helpers); itr != end_itr; ++itr)
      {
        if (boost::filesystem::is_directory(itr->status()))
        {
          continue;
        }
        else
        {
          std::string fileName = OSS::boost_file_name(itr->path());
          boost::filesystem::path currentFile = itr->path();
          
          if (boost::filesystem::is_regular(currentFile))
          {
            if (OSS::string_ends_with(fileName, ".js"))
            {
              //
              // Compile it!
              //
              //OSS_LOG_INFO("Google V8 is compiling helper script " << currentFile);
              v8::Handle<v8::String> helperScript;
              helperScript = read_file(OSS::boost_path(currentFile));
              if (helperScript.IsEmpty())
              {
                reportException(try_catch, true);
                return false;
              }

              v8::Handle<v8::Script> compiledHelper = v8::Script::Compile(helperScript);

              if (compiledHelper.IsEmpty())
              {
                reportException(try_catch, true);
                return false;
              }

               // Run the script!
              v8::Handle<v8::Value> result = compiledHelper->Run();
              if (result.IsEmpty())
              {
                // The TryCatch above is still in effect and will have caught the error.
                reportException(try_catch, true);
                return false;
              }
            }
          }
        }
      }
    }
    catch(OSS::Exception e)
    {
      std::ostringstream logMsg;
      logMsg << "Filesystem error while compiling script helpers - " << e.message();
      OSS::log_warning(logMsg.str());
    }
  }
  
  JSBase::compileModuleHelpers();

  //
  // Compile the main script script
  //
  //OSS_LOG_INFO("Google V8 is compiling main script " << _script);
  v8::Handle<v8::String> script;
  
  if (preloaded.empty())
  {
    script = read_file(OSS::boost_path(_script));
  }
  else
  {
    script = v8::String::New(preloaded.data(), preloaded.size());
  }

  v8::Handle<v8::Script> compiled_script = v8::Script::Compile(script);

  if (compiled_script.IsEmpty())
  {
    reportException(try_catch, true);
    return false;
  }

  //OSS_LOG_INFO("Google V8 is running main script " << _script);
  // Run the script!
  v8::Handle<v8::Value> result = compiled_script->Run();
  if (result.IsEmpty())
  {
    // The TryCatch above is still in effect and will have caught the error.
    reportException(try_catch, true);
    return false;
  }



  // The script compiled and ran correctly.  Now we fetch out the
  // Process function from the global object.
  if (!functionName.empty())
  {
    v8::Handle<v8::String> process_name = v8::String::New(functionName.c_str());
    v8::Handle<v8::Value> process_val = context->Global()->Get(process_name);

    // If there is no Process function, or if it is not a function,
    // bail out
    if (!process_val->IsFunction())
    {
      OSS_LOG_ERROR("Google V8 is unable to load function " << functionName);
      return false;
    }

    // It is a function; cast it to a Function
    v8::Handle<v8::Function> process_fun = v8::Handle<v8::Function>::Cast(process_val);

    // Store the function in a Persistent handle, since we also want
    // that to remain after this call returns
    *(static_cast<v8::Persistent<v8::Function>*>(_processFunc)) = v8::Persistent<v8::Function>::New(process_fun);
  }

  // all went well.  request the template creation as the final step
  v8::Handle<v8::ObjectTemplate> objectTemplate = v8::ObjectTemplate::New();
  objectTemplate->SetInternalFieldCount(1);
  *(static_cast<v8::Persistent<v8::ObjectTemplate>*>(_requestTemplate)) = v8::Persistent<v8::ObjectTemplate>::New(objectTemplate);

  _isInitialized = true;

  return  _isInitialized;
}

bool JSBase::recompile()
{
  return internalRecompile();
}

bool JSBase::internalRecompile()
{
  
  if (!internalInitialize(_script, _functionName, _extensionGlobals, _preloaded))
  {
    return false;
  }
  return true;
}


bool JSBase::processRequest(OSS_HANDLE request)
{
  return internalProcessRequest(request);
}

bool JSBase::internalProcessRequest(OSS_HANDLE request)
{
  if (!_isInitialized || !_processFunc)
    return false;
  
  v8::Locker __v8Locker__;
  
  v8::HandleScope handle_scope;

  
  // Enter this processor's context so all the remaining operations
  // take place there
  v8::Context::Scope context_scope(*(static_cast<v8::Persistent<v8::Context>*>(_context)));

  // Fetch the template for creating JavaScript request wrappers.
  // It only has to be created once, which we do on demand.
  v8::Handle<v8::ObjectTemplate> templ = *(static_cast<v8::Persistent<v8::ObjectTemplate>*>(_requestTemplate));

    // Set up an exception handler before calling the Process function
  v8::TryCatch try_catch;
  
  // Create an empty http request wrapper.
  v8::Handle<v8::Object> request_obj = templ->NewInstance();

  if (request_obj.IsEmpty())
  {
    reportException(try_catch, true);
    return false;
  }

  // Wrap the raw C++ pointer in an External so it can be referenced
  // from within JavaScript.
  v8::Handle<v8::External> request_ptr = v8::External::New(request);

  // Store the request pointer in the JavaScript wrapper.
  request_obj->SetInternalField(0, request_ptr);


  // Invoke the process function, giving the global object as 'this'
  // and one argument, the request.
  const int argc = 1;
  v8::Handle<v8::Value> argv[argc] = { request_obj };
  v8::Handle<v8::Value> result = (*(static_cast<v8::Persistent<v8::Function>*>(_processFunc)))->Call((*(static_cast<v8::Persistent<v8::Context>*>(_context)))->Global(), argc, argv);
  if (result.IsEmpty())
  {
    reportException(try_catch, true);
    return false;
  }

  return true;
}

bool JSBase::callFunction(const std::string& funcName)
{
  if (!_isInitialized || !_context)
    return false;
  
  v8::Locker __v8Locker__;
  
  v8::HandleScope handle_scope;

  v8::Persistent<v8::Context>& context = *(static_cast<v8::Persistent<v8::Context>*>(_context));
  // Enter this processor's context so all the remaining operations
  // take place there
  v8::Context::Scope context_scope(context);
   
  // The script compiled and ran correctly.  Now we fetch out the
  // Process function from the global object.
  v8::Handle<v8::String> func_name = v8::String::New(funcName.c_str());
  v8::Handle<v8::Value> func_val = context->Global()->Get(func_name);

  // If there is no Process function, or if it is not a function,
  // bail out
  if (!func_val->IsFunction())
  {
    OSS_LOG_ERROR("JSBase::callFunction - Google V8 is unable to load function " << funcName);
    return false;
  }

  // It is a function; cast it to a Function
  v8::Handle<v8::Function> process_fun = v8::Handle<v8::Function>::Cast(func_val);
  
  // call it without any arguments
  
  process_fun->Call(context->Global(), 0, 0);
  
  return true;

}

void JSBase::registerInternalModule(const Module& module)
{
  assert(_modules.find(module.name) == _modules.end());
  _modules[module.name] = module;
}

void JSBase::registerModuleHelper(const Module& module)
{
  _moduleHelpers.push_back(module);
}

bool JSBase::compileModuleHelpers()
{
  v8::HandleScope scope;
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  for (ModuleHelpers::iterator iter = _moduleHelpers.begin(); iter != _moduleHelpers.end(); iter++)
  {
    v8::Handle<v8::String> script(v8::String::New(iter->script.c_str()));
    v8::Handle<v8::Value> name(v8::String::New(iter->name.c_str()));
    v8::Handle<v8::Script> compiled = v8::Script::Compile(script, name);

    v8::Handle<v8::Value> result = compiled->Run();
    if (result.IsEmpty())
    {
      // The TryCatch above is still in effect and will have caught the error.
      reportException(try_catch, true);
      return false;
    }
  }
  return true;
}


} } // OSS::JS






