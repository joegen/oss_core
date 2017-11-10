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
OSS::mutex_critic_sec JSBase::_currentBaseMutex;
std::map<int32_t, JSBase*> JSBase::_currentBaseMap;
int32_t JSBase::_baseId = 0;
  
static v8::Handle<v8::String> read_global_scripts()
{
  std::ostringstream data;
  
  for (std::vector<std::string>::iterator iter = JSBase::_globalScripts.begin(); iter != JSBase::_globalScripts.end(); iter++)
  {
    data << *iter << std::endl;
  }
  
  return  v8::String::New(data.str().c_str(), data.str().size());
}

static std::string V8ErrorReport;
static bool _hasSetErrorCB = false;
static void V8ErrorMessageCallback(v8::Handle<v8::Message> message,
v8::Handle<v8::Value> data)
{
  v8::HandleScope handle_scope;
  
  if (message->GetSourceLine()->IsString())
  {
    std::string error =
            + " Javascript error on line " + OSS::string_from_number(message->GetLineNumber())
            + " : " + string_from_js_string(message->GetSourceLine());
    OSS::log_error(error);
  }
}

JSBase::JSBase(const std::string& contextName) :
  _contextName(contextName),
  _context(0),
  _processFunc(0),
  _requestTemplate(0),
  _globalTemplate(0),
  _isInitialized(false),
  _extensionGlobals(0),
  _moduleManager(this),
  _enableCommonJS(false)
{
  _id = JSBase::addBase(this);
}

JSBase::~JSBase()
{
  JSBase::removeBase(_id);
  if (_globalTemplate)
  {
    _globalTemplate->Dispose();
    _requestTemplate->Dispose();
    if (_processFunc)
    {
      _processFunc->Dispose();
    }
    _context->Dispose();
  }

  delete _globalTemplate;
  delete _requestTemplate;
  delete _processFunc;
  delete _context;

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
  
  //
  // Set the external heap to 20mb before attempting to grabage collect
  //
  v8::V8::AdjustAmountOfExternalAllocatedMemory(1024 * 1024 * 20);
  
  if (_context)
  {
    (*_context)->DetachGlobal();
    _context->Dispose();
    delete _context;
    _context = 0;
  }
  
  if (_globalTemplate)
  {
    _globalTemplate->Dispose();
    delete _globalTemplate;
    _globalTemplate = 0;
  }

  if (_requestTemplate)
  {
    _requestTemplate->Dispose();
    delete _requestTemplate;
    _requestTemplate = 0;
  }

  if (_processFunc)
  {
    _processFunc->Dispose();
    delete _processFunc;
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
  *_globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(global);
  
  global->Set(v8::String::New("__js_base_id"), v8::Integer::New(_id));
  global->Set(v8::String::New("log_info"), v8::FunctionTemplate::New(log_info_callback));
  global->Set(v8::String::New("log_debug"), v8::FunctionTemplate::New(log_debug_callback));
  global->Set(v8::String::New("log_error"), v8::FunctionTemplate::New(log_error_callback));
  
  if (_enableCommonJS)
  {
    _moduleManager.setGlobals(global);
  }

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
  *_context = v8::Persistent<v8::Context>::New(context);

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
      report_js_exception(try_catch, true);
      return false;
    }

   // OSS_LOG_INFO("Google V8 is running global.detail for " << _script);
     // Run the script!
    v8::Handle<v8::Value> result = compiledHelper->Run();
    if (result.IsEmpty())
    {
      // The TryCatch above is still in effect and will have caught the error.
      report_js_exception(try_catch, true);
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
                report_js_exception(try_catch, true);
                return false;
              }

              v8::Handle<v8::Script> compiledHelper = v8::Script::Compile(helperScript);

              if (compiledHelper.IsEmpty())
              {
                report_js_exception(try_catch, true);
                return false;
              }

               // Run the script!
              v8::Handle<v8::Value> result = compiledHelper->Run();
              if (result.IsEmpty())
              {
                // The TryCatch above is still in effect and will have caught the error.
                report_js_exception(try_catch, true);
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

  if (_enableCommonJS)
  {
    JSPluginManager::instance().setContext(_context);
    JSPluginManager::instance().setGlobal(_globalTemplate);

    if (!_moduleManager.initialize(try_catch, global))
    {
      // Exception is reported inside initialize
      return false;
    }
  }
  
  //
  // Compile the main script script
  //
  //OSS_LOG_INFO("Google V8 is compiling main script " << _script);
  v8::Handle<v8::String> script;
  v8::Handle<v8::Script> compiled_script;
  if (preloaded.empty())
  {
    script = read_file_skip_shebang(OSS::boost_path(_script));
    compiled_script = v8::Script::Compile(script, v8::String::New(_script.c_str()));
  }
  else
  {
    script = v8::String::New(preloaded.data(), preloaded.size());
    compiled_script = v8::Script::Compile(script);
  }
  

  if (compiled_script.IsEmpty())
  {
    report_js_exception(try_catch, true);
    return false;
  }

  //OSS_LOG_INFO("Google V8 is running main script " << _script);
  // Run the script!
  v8::Handle<v8::Value> result = compiled_script->Run();
  if (result.IsEmpty())
  {
    // The TryCatch above is still in effect and will have caught the error.
    report_js_exception(try_catch, true);
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
    *_processFunc = v8::Persistent<v8::Function>::New(process_fun);
  }

  // all went well.  request the template creation as the final step
  v8::Handle<v8::ObjectTemplate> objectTemplate = v8::ObjectTemplate::New();
  objectTemplate->SetInternalFieldCount(1);
  *_requestTemplate = v8::Persistent<v8::ObjectTemplate>::New(objectTemplate);

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
  v8::Context::Scope context_scope(*_context);

  // Set up an exception handler before calling the Process function
  v8::TryCatch try_catch;
  
  // Wrap the request as an internal field
  v8::Handle<v8::Object> request_obj;
  wrap_external_object(try_catch, _context, _requestTemplate, request_obj, request);


  // Invoke the process function, giving the global object as 'this'
  // and one argument, the request.
  const int argc = 1;
  v8::Handle<v8::Value> argv[argc] = { request_obj };
  v8::Handle<v8::Value> result = ((*_processFunc)->Call((*_context)->Global(), argc, argv));
  if (result.IsEmpty())
  {
    report_js_exception(try_catch, true);
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

  v8::Persistent<v8::Context>& context = *_context;
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

#if 0
std::vector<std::string> JSBase::_globalScripts;
OSS::mutex_critic_sec JSBase::_currentBaseMutex;
std::map<int, JSBase*> JSBase::_currentBaseMap;
int JSBase::_baseId = 0;
#endif

int JSBase::addBase(JSBase* base)
{
  OSS::mutex_critic_sec_lock lock(JSBase::_currentBaseMutex);
  JSBase::_baseId++;
  JSBase::_currentBaseMap[JSBase::_baseId] = base;
  return JSBase::_baseId;
}
void JSBase::removeBase(int id)
{
  OSS::mutex_critic_sec_lock lock(JSBase::_currentBaseMutex);
  JSBase::_currentBaseMap.erase(id);
}
  
JSBase* JSBase::GetCurrent()
{
  OSS::mutex_critic_sec_lock lock(JSBase::_currentBaseMutex);
  v8::Local<v8::Value> val = v8::Context::GetCurrent()->Global()->Get(v8::String::New("__js_base_id"));
  int32_t id = val->Int32Value();
  std::map<int, JSBase*>::iterator iter = JSBase::_currentBaseMap.find(id);
  if (iter == JSBase::_currentBaseMap.end())
  {
    return 0;
  }
  return iter->second;
}


} } // OSS::JS






