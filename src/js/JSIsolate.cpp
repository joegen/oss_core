
#include <v8.h>

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


#include "OSS/JS/JSIsolate.h"
#include "OSS/JS/JSIsolateManager.h"
#include "OSS/JS/JSEventLoop.h"
#include "OSS/JS/JSModule.h"
#include "OSS/JS/modules/Async.h"
#include "OSS/JS/JSUtil.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/JSPluginManager.h"


namespace OSS {
namespace JS {


static void V8ErrorMessageCallback(v8::Handle<v8::Message> message, v8::Handle<v8::Value> data)
{
  v8::HandleScope handle_scope;
  
  if (message->GetSourceLine()->IsString())
  {
    std::string error =
            + "Javascript error on line : "
            + string_from_js_string(message->GetSourceLine());
    OSS::log_error(error);
    OSS::log_error(get_stack_trace(message, 1024));
  }
}

JSIsolate::JSIsolate(pthread_t parentThreadId) :
  _pIsolate(0),
  _pModuleManager(0),
  _exitValue(0),
  _threadId(0),
  _parentThreadId(parentThreadId),
  _pEventLoop(0),
  _isRoot(false),
  _pThread(0)
{
  _pPluginManager = new JSPluginManager(this);
  _pModuleManager = new JSModule(this);
  _pEventLoop = new JSEventLoop(this);
}

JSIsolate::~JSIsolate()
{
  terminate();
  join();
  delete _pEventLoop;
  _pEventLoop = 0;
  delete _pModuleManager;
  _pModuleManager = 0;
  delete _pPluginManager;
  _pPluginManager = 0;
}

void JSIsolate::internal_run()
{
  if (isRoot())
  {
    v8::V8::AddMessageListener(V8ErrorMessageCallback);
  }
  
  _pIsolate = v8::Isolate::New();
  v8::Isolate::Scope global_scope(_pIsolate);
  _threadId  = pthread_self();
  JSIsolateManager::instance().registerIsolate(shared_from_this());
  v8::HandleScope handle_scope;

  v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
  _globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(global);
  
  //
  // Set the thread id and update the manager
  //
  _threadId = pthread_self();

  //
  // Initialize global and assign it to the context
  //
  JSIsolateManager::instance().modulesMutex().lock();
  _pModuleManager->initGlobalExports(global);
  JSIsolateManager::instance().modulesMutex().unlock();

  if (isRoot())
  {
    _pModuleManager->setMainScript(_script);
  }

  v8::Handle<v8::Context> context = v8::Context::New(0, global);
  _context = v8::Persistent<v8::Context>::New(context);
  v8::Context::Scope context_scope(context);
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  
  JSIsolateManager::instance().modulesMutex().lock();
  if (!_pModuleManager->initialize(try_catch, global))
  {
    OSS_LOG_ERROR("Unable to initialize module manager");
    report_js_exception(try_catch, true);
    return;
  }
  JSIsolateManager::instance().modulesMutex().unlock();
  
  v8::Handle<v8::Script> compiledScript ;
  if (_source.empty())
  {
    v8::Handle<v8::String> scriptSource = read_file_skip_shebang(OSS::boost_path(_script), true);
    compiledScript = v8::Script::Compile(scriptSource, v8::String::New(OSS::boost_path(_script).c_str()));
  }
  else
  {
    std::ostringstream strm;
    strm << "try { " << _source << " } catch(e) {console.printStackTrace(e); _exit(-1); } ;async.processEvents();";
    v8::Handle<v8::String> scriptSource = JSString(strm.str());
    compiledScript = v8::Script::Compile(scriptSource);
  }
  
  if (compiledScript.IsEmpty())
  {
    OSS_LOG_ERROR("Unable to compile script");
    report_js_exception(try_catch, true);
    _exitValue = -1;
    return;
  }
  
  v8::Handle<v8::Value> result = compiledScript->Run();
  if (result.IsEmpty())
  {
    OSS_LOG_ERROR("Unable to run script");
    report_js_exception(try_catch, true);
    _exitValue = -1;
    return;
  }

  _exitValue = 0;
}

void JSIsolate::run(const boost::filesystem::path& script)
{
  _script = script;
  _source = std::string();
  if (isRoot())
  {
    internal_run();
  }
  else
  {
    _pThread = new boost::thread(boost::bind(&JSIsolate::internal_run, this));
  }
}

void JSIsolate::runSource(const std::string& source)
{
  _source = source;
  _script = boost::filesystem::path();
  if (isRoot())
  {
    internal_run();
  }
  else
  {
    _pThread = new boost::thread(boost::bind(&JSIsolate::internal_run, this));
  }
}

bool JSIsolate::call(const std::string& method, const OSS::JSON::Object& arguments, OSS::JSON::Object& reply, uint32_t timeout, void* userData)
{
  return false;
}

bool JSIsolate::call(const OSS::JSON::Object& request, OSS::JSON::Object& reply, uint32_t timeout, void* userData)
{
  return false;
}

void JSIsolate::notify(const std::string& eventName, const OSS::JSON::Array& args, int queueFd)
{
  JSEventArgument event(eventName, args, queueFd);
  _pEventLoop->eventEmitter().emit(event);
}

void JSIsolate::terminate()
{
  _pEventLoop->terminate();
}

bool JSIsolate::isThreadSelf()
{
  return _threadId == pthread_self();
}

JSIsolate::Ptr JSIsolate::getIsolate()
{
  return JSIsolateManager::instance().findIsolate(pthread_self());
}

JSEventLoop* JSIsolate::eventLoop()
{
  return _pEventLoop;
}

JSModule* JSIsolate::getModuleManager()
{
  return _pModuleManager;
}

JSPluginManager* JSIsolate::getPluginManager()
{
  return _pPluginManager;
}

JSObjectHandle JSIsolate::getGlobal()
{
  return _context.value()->Global();
}

JSValueHandle JSIsolate::parseJSON(const std::string& json)
{
  js_enter_scope();
  v8::Local<v8::Object> JSON = getGlobal()->Get(JSLiteral("JSON"))->ToObject();
  v8::Handle<v8::Value> parseFunc = JSON->Get(JSLiteral("parse"));
  v8::Handle<v8::Function> parse = v8::Handle<v8::Function>::Cast(parseFunc);

  JSValueHandle val = JSString(json);
  JSArgumentVector args;
  args.push_back(val);
  
  return parse->Call(getGlobal(), args.size(), args.data());
}

void JSIsolate::join()
{
  if (_pThread)
  {
    _pThread->join();
    delete _pThread;
    _pThread = 0;
  }
}

} } 



