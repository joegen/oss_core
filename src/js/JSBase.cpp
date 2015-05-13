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



namespace OSS {
namespace JS {

  
 
#define ENABLE_V8_PREEMPTION 1
#if ENABLE_V8_PREEMPTION
#define V8LOCK v8::Locker __v8Locker__
#define V8_START_PREEMPTION(ms) v8::Locker::StartPreemption(ms)
#define V8_STOP_PREEMPTION v8::Locker::StopPreemption()
#else
#define V8LOCK
#define V8_START_PREEMPTION 
#define V8_STOP_PREEMPTION 
#endif

//
// JSWorker
//
class OSS_API JSWorker : boost::noncopyable
{
public:
  enum RequestType
  {
    TYPE_INIT,
    TYPE_REQUEST,
    TYPE_RECOMPILE,
    TYPE_INIT_NODE
  };

  static bool initialize(JSBase* pJS, const boost::filesystem::path& script,
    const std::string& functionName,
    void(*extensionGlobals)(OSS_HANDLE));
    /// Initialize the javascript context and the object template.
    /// The function indicated by funtionName must exist in the script

  static bool recompile(JSBase* pJS);
    /// recompile the current active script

  static bool processRequest(JSBase* pJS, OSS_HANDLE request);
    /// Process the request

  static void run();

  static void stop();

  static boost::tuples::tuple<
    JSBase*,
    boost::filesystem::path,
    std::string,
    void(*)(OSS_HANDLE) > _initData;
  static JSBase* _pJSBase;
  static boost::tuples::tuple<JSBase*, OSS_HANDLE> _requestData;
  static OSS::semaphore* _semEvent;
  static OSS::semaphore* _semComplete;
  static boost::thread* _thread;
  static RequestType _requestType;
  static bool _exit;
  static bool _retVal;
};

//
// Global statics
//
OSS::mutex JSBase::_mutex;
boost::tuples::tuple<
    JSBase*,
    boost::filesystem::path,
    std::string,
    void(*)(OSS_HANDLE) > JSWorker::_initData;
JSBase* JSWorker::_pJSBase = 0;
boost::tuples::tuple<JSBase*, OSS_HANDLE> JSWorker::_requestData;
OSS::semaphore* JSWorker::_semEvent = 0;
OSS::semaphore* JSWorker::_semComplete = 0;
boost::thread* JSWorker::_thread = 0;
JSWorker::RequestType JSWorker::_requestType;
bool JSWorker::_exit = false;
bool JSWorker::_retVal = false;

bool JSWorker::initialize(JSBase* pJS, const boost::filesystem::path& script,
  const std::string& functionName,
  void(*extensionGlobals)(OSS_HANDLE))
{

  V8LOCK;

#if ENABLE_V8_PREEMPTION
  //OSS_LOG_INFO("Google V8 Preemption ENABLED");
  return pJS->internalInitialize(script, functionName, extensionGlobals);
#else
  JSWorker::_pJSBase = pJS;
  if (JSWorker::_thread == 0)
  {
     JSWorker::_semEvent = new OSS::semaphore(0, 0xFFFF);
     JSWorker::_semComplete = new OSS::semaphore(0, 0xFFFF);
     JSWorker::_thread = new boost::thread(boost::bind(JSWorker::run));
  }

  bool retVal = false;
  JSBase::_mutex.lock();
  JSWorker::_requestType = TYPE_INIT;

  JSWorker::_initData = boost::tuples::tuple<JSBase*,
    boost::filesystem::path,
    std::string,
    void(*)(OSS_HANDLE) >(pJS, script, functionName, extensionGlobals);

  JSWorker::_semEvent->set();
  JSWorker::_semComplete->wait();
  retVal = JSWorker::_retVal;
  JSBase::_mutex.unlock();
  return retVal;
#endif
}

bool JSWorker::recompile(JSBase* pJS)
{
  V8LOCK;

#if ENABLE_V8_PREEMPTION
  return pJS->internalRecompile();
#else
  JSWorker::_pJSBase = pJS;
  bool retVal = false;
  JSBase::_mutex.lock();
  JSWorker::_requestType = TYPE_RECOMPILE;
  JSWorker::_semEvent->set();
  JSWorker::_semComplete->wait();
  retVal = JSWorker::_retVal;
  JSBase::_mutex.unlock();
  return retVal;
#endif
}

bool JSWorker::processRequest(JSBase* pJS, OSS_HANDLE request)
{
  V8LOCK;

#if ENABLE_V8_PREEMPTION
  return pJS->internalProcessRequest(request);
#else
  JSWorker::_pJSBase = pJS;
  bool retVal = false;

  JSBase::_mutex.lock();
  JSWorker::_requestType = TYPE_REQUEST;
  JSWorker::_requestData = boost::tuples::tuple<JSBase*, OSS_HANDLE>(pJS, request);
  JSWorker::_semEvent->set();
  JSWorker::_semComplete->wait();
  retVal = JSWorker::_retVal;
  JSBase::_mutex.unlock();
  return retVal;
#endif
}

void JSWorker::stop()
{
#if ENABLE_V8_PREEMPTION
  V8_STOP_PREEMPTION;
#else
  if (JSWorker::_thread == 0)
    return;
  
  JSBase::_mutex.lock();
  JSWorker::_exit = true;
  _semEvent->set();
  JSBase::_mutex.unlock();
  _thread->join();


  delete JSWorker::_semEvent;
  delete JSWorker::_semComplete;
  delete JSWorker::_thread;
#endif

}
void JSWorker::run()
{
  OSS::OSS_register_deinit(boost::bind(JSWorker::stop));

#if !ENABLE_V8_PREEMPTION

  for(;;)
  {
    _semEvent->wait();
    if (_exit)
      return;
    if (_requestType == TYPE_INIT)
    {
      _retVal = _pJSBase->internalInitialize(_initData.get<1>(), _initData.get<2>(), _initData.get<3>());
    }
    else if (_requestType == TYPE_REQUEST)
    {
      _retVal = _pJSBase->internalProcessRequest(_requestData.get<1>());
    }
    else if (_requestType == TYPE_RECOMPILE)
    {
      _retVal = _pJSBase->internalRecompile();
    }
    _semComplete->set();
  }
#endif
}


//
// Global Callbacks = boost::function<void(OSS_HANDLE)>()
//


static std::string toString(v8::Handle<v8::Value> str)
{
  if (!str->IsString())
    return "";
  v8::String::Utf8Value value(str);
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
    
    std::cerr << errorMsg.str() << std::endl;
    OSS_LOG_ERROR("\t[CID=00000000] JS: " << *error << std::endl << "{" << std::endl << errorMsg.str() << std::endl << "}");
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
    msg << *value0 << "JS: " << *value1;
    OSS::log_notice(msg.str());
  }
  else
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg = args[0];
    v8::String::Utf8Value value(arg);
    std::ostringstream msg;
    msg << "JS: " << *value;
    OSS::log_notice(msg.str());
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
    msg << *value0 << "JS: " << *value1;
    OSS::log_debug(msg.str());
  }
  else
  {
    v8::HandleScope scope;
    v8::Handle<v8::Value> arg = args[0];
    v8::String::Utf8Value value(arg);
    std::ostringstream msg;
    msg << "JS: " << *value;
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
static std::string read_file(const std::string& name) {
  std::string data;
  FILE* file = fopen(name.c_str(), "rb");
  if (file == NULL)
  {
    return std::string();
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
  data = std::string(chars, size);
  delete[] chars;
  return data;
}

static v8::Handle<v8::String> read_global_scripts()
{
  static std::string gAccessList(
    #include "./scripts/JS_AccessList.h"
  );

  static std::string gAuthProfile(
    #include "./scripts/JS_AuthProfile.h"
  );

  static std::string gPropertyObject(
    #include "./scripts/JS_PropertyObject.h"
  ); 

  static std::string gRouteProfile(
    #include "./scripts/JS_RouteProfile.h"
  ); 

  static std::string gSIPMessage(
    #include "./scripts/JS_SIPMessage.h"
  ); 

  static std::string gTransactionProfile(
    #include "./scripts/JS_TransactionProfile.h"
  ); 
  
  std::ostringstream data;
  data  << gAccessList 
        << gAuthProfile
        << gPropertyObject
        << gRouteProfile
        << gSIPMessage
        << gTransactionProfile;
  
  return  v8::String::New(data.str().c_str(), data.str().size());
}

static std::string read_directory(const boost::filesystem::path& directory)
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
                return std::string();
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
      OSS_LOG_WARNING("Google V8 exception: " << e.what());
    }
    catch(...)
    {
      OSS_LOG_WARNING("Unknown Google V8 exception.");
    }
  }
  
  return data;
}

static bool compile_and_run(const v8::Handle<v8::String>& javaScript)
{
  //
  // We're just about to compile the script; set up an error handler to
  // catch any exceptions the script might throw.
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);

  //
  // Compile global helpers
  // 
  try
  {
    //
    // Compile it!
    //
    
    if (javaScript.IsEmpty())
    {
      OSS_LOG_ERROR("Failed to compile script");
      // The script failed to compile; bail out.
      return false;
    }

    //OSS_LOG_INFO("Google V8 is compiling global.detail for " << _script);
    v8::Handle<v8::Script> compiledScript = v8::Script::Compile(javaScript);
    if (compiledScript.IsEmpty())
    {
      reportException(try_catch, true);
      return false;
    }

   // OSS_LOG_INFO("Google V8 is running global.detail for " << _script);
     // Run the script!
    v8::Handle<v8::Value> result = compiledScript->Run();
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
    return false;
  }
  
  return true;
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
    static_cast<v8::Persistent<v8::Function>*>(_processFunc)->Dispose();
    static_cast<v8::Persistent<v8::Context>*>(_context)->Dispose();
  }

  delete static_cast<v8::Persistent<v8::ObjectTemplate>*>(_globalTemplate);
  delete static_cast<v8::Persistent<v8::ObjectTemplate>*>(_requestTemplate);
  delete static_cast<v8::Persistent<v8::Function>*>(_processFunc);
  delete static_cast<v8::Persistent<v8::Context>*>(_context);

}

bool JSBase::initialize(const boost::filesystem::path& scriptFile, const std::string& functionName,
  void(*extensionGlobals)(OSS_HANDLE) )
{
  return JSWorker::initialize(this, scriptFile, functionName, extensionGlobals);
}

bool JSBase::initialize(const std::string& entryPoint,
    const std::string& helper,
    const std::string& main,
    void(*extensionGlobals)(OSS_HANDLE))
{
  if (entryPoint.empty() || main.empty())
  {
    OSS_LOG_ERROR("JSBase::initialize - entryPoint or main script is empty");
    return false;
  }
  
  //
  // Make sure that entryPoint is found in the script
  //
  if (main.find(entryPoint) == std::string::npos)
  {
    OSS_LOG_ERROR("JSBase::initialize - entryPoint not found in main script");
    return false;
  }
  
  _functionName = entryPoint;
  _helperScript = helper;
  _mainScript = main;
  
  // Create a handle scope to hold the temporary references.
  v8::HandleScope handle_scope;
  
  v8::Handle<v8::String> globalScript;
  v8::Handle<v8::String> helperScript;
  v8::Handle<v8::String> mainScript;
  
  globalScript = read_global_scripts();
  
  if (!helper.empty())
  {
    helperScript = v8::String::New(helper.c_str(), helper.size()); 
  }
  
  mainScript = v8::String::New(main.c_str(), main.size());
  
  return internalInitialize(entryPoint, globalScript, helperScript, mainScript, extensionGlobals);
}

bool JSBase::internalInitialize(
  const boost::filesystem::path& scriptFile, const std::string& functionName,
  void(*extensionGlobals)(OSS_HANDLE) )
{
  if (!boost::filesystem::exists(scriptFile))
  {
    OSS_LOG_ERROR("Google V8 is unable to locate file " << scriptFile);
    return false;
  }

  //
  // Compile global helpers
  //
  
  boost::filesystem::path globalsDir;
  if (!_globalScriptsDirectory.empty())
    globalsDir = boost::filesystem::path(_globalScriptsDirectory);
  else
    globalsDir = operator/(scriptFile.branch_path(), "global.detail");

  v8::Handle<v8::String> globalScript;
  v8::Handle<v8::String> helperScript;
  v8::Handle<v8::String> mainScript;
  
  try
  {
    if (!boost::filesystem::exists(globalsDir))
    {
      globalScript = read_global_scripts();
    }
    else
    {
      std::string globalData = read_directory(globalsDir);
      globalScript = v8::String::New(globalData.c_str(), globalData.size());
    }

    if (globalScript.IsEmpty())
    {
      OSS_LOG_ERROR("Failed to compile global exports for " << _script);
      // The script failed to compile; bail out.
      return false;
    }
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
  boost::filesystem::path helpersDir;
  if (!_helperScriptsDirectory.empty())
  {
    helpersDir = boost::filesystem::path(_helperScriptsDirectory);
  }
  else
  {
    helpersDir = OSS::boost_path(_script) + ".detail";
  }
  
  if (boost::filesystem::exists(helpersDir))
  {
    //
    // This script has a heper directory
    //
    try
    {
      boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
      for (boost::filesystem::directory_iterator itr(helpersDir); itr != end_itr; ++itr)
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
              
              std::string helperScriptData;
              helperScriptData = read_file(OSS::boost_path(currentFile));
              helperScript = v8::String::New(helperScriptData.c_str(), helperScriptData.size());
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

  //
  // Compile the main script
  //
  std::string mainScriptData = read_file(OSS::boost_path(_script));
  mainScript = v8::String::New(mainScriptData.c_str(), mainScriptData.size());

  return internalInitialize(functionName, globalScript, helperScript, mainScript, extensionGlobals);
}

bool JSBase::internalInitialize(
    const std::string& entryPoint,
    const v8::Handle<v8::String>& globals, 
    const v8::Handle<v8::String>& helperScript,
    const v8::Handle<v8::String>& mainScript,
    void(*extensionGlobals)(OSS_HANDLE))
{
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


  v8::Persistent<v8::Context>* context_ = new v8::Persistent<v8::Context>();
  v8::Persistent<v8::Function>* processFunc_ = new v8::Persistent<v8::Function>();
  v8::Persistent<v8::ObjectTemplate>* requestTemplate_ = new v8::Persistent<v8::ObjectTemplate>;
  v8::Persistent<v8::ObjectTemplate>* globalTemplate_ = new v8::Persistent<v8::ObjectTemplate>;
  _context = context_;
  _processFunc = processFunc_;
  _requestTemplate = requestTemplate_;
  _globalTemplate = globalTemplate_;
  _functionName = entryPoint;
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
  
  //const v8::Handle<v8::String>& mainScript,
  //  const v8::Handle<v8::String>& helperScript,
  //  const v8::Handle<v8::String>& globals,
    
  if (!compile_and_run(globals))
  {
    OSS_LOG_ERROR("JSBase::internalInitialize - Unable to compile globals");
    return false;
  }
  
  //
  // Helpers can be absent
  //
  if (!helperScript.IsEmpty() && !compile_and_run(helperScript))
  {
    OSS_LOG_ERROR("JSBase::internalInitialize - Unable to compile helper scripts");
    return false;
  }
  
  if (!compile_and_run(mainScript))
  {
    OSS_LOG_ERROR("JSBase::internalInitialize - Unable to compile main script");
    return false;
  }
  
  // The script compiled and ran correctly.  Now we fetch out the
  // Process function from the global object.
  v8::Handle<v8::String> process_name = v8::String::New(entryPoint.c_str());
  v8::Handle<v8::Value> process_val = context->Global()->Get(process_name);

  // If there is no Process function, or if it is not a function,
  // bail out
  if (!process_val->IsFunction())
  {
    OSS_LOG_ERROR("Google V8 is unable to load function " << entryPoint);
    return false;
  }

  // It is a function; cast it to a Function
  v8::Handle<v8::Function> process_fun = v8::Handle<v8::Function>::Cast(process_val);

  // Store the function in a Persistent handle, since we also want
  // that to remain after this call returns
  *(static_cast<v8::Persistent<v8::Function>*>(_processFunc)) = v8::Persistent<v8::Function>::New(process_fun);

  // all went well.  request the template creation as the final step
  v8::Handle<v8::ObjectTemplate> objectTemplate = v8::ObjectTemplate::New();
  objectTemplate->SetInternalFieldCount(1);
  *(static_cast<v8::Persistent<v8::ObjectTemplate>*>(_requestTemplate)) = v8::Persistent<v8::ObjectTemplate>::New(objectTemplate);

  _isInitialized = true;

  return  _isInitialized;
}

bool JSBase::recompile()
{
  return JSWorker::recompile(this);
}

bool JSBase::internalRecompile()
{
  if (_mainScript.empty())
  {
    //
    // Use the old prototype
    //
    return internalInitialize(_script, _functionName, _extensionGlobals);
  }
  else
  {
    return initialize(_functionName, _helperScript, _mainScript, _extensionGlobals);
  }
}


bool JSBase::processRequest(OSS_HANDLE request)
{
  return JSWorker::processRequest(this, request);
}

bool JSBase::internalProcessRequest(OSS_HANDLE request)
{
  if (!_isInitialized)
    return false;
  
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


} } // OSS::JS





