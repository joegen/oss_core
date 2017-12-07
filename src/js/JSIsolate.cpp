
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
#include "OSS/JS/modules/Async.h"
#include "OSS/JS/JSUtil.h"
#include "OSS/UTL/Logger.h"


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

JSIsolate& JSIsolate::instance()
{
  static JSIsolate isolate("_root_");
  return isolate;
}

JSIsolate::JSIsolate(const std::string& name) :
  _pIsolate(0),
  _threadId(0),
  _name(name)
{
}

JSIsolate::~JSIsolate()
{
}

int JSIsolate::run(const boost::filesystem::path& script)
{
  v8::V8::AddMessageListener(V8ErrorMessageCallback);
  _pIsolate = v8::Isolate::New();
  v8::Isolate::Scope global_scope(_pIsolate);
  
  v8::HandleScope handle_scope;
  v8::Persistent<v8::Context> _context;
  v8::Persistent<v8::ObjectTemplate> _globalTemplate;
  
   v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
  _globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(global);
  
  //
  // Initialize global and assign it to the context
  //
  _moduleManager.setGlobals(global);
  _moduleManager.setMainScript(script);
  
  v8::Handle<v8::Context> context = v8::Context::New(0, global);
  _context = v8::Persistent<v8::Context>::New(context);
  v8::Context::Scope context_scope(context);
  
  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  
    
  JSPluginManager::instance().setContext(&_context);
  JSPluginManager::instance().setGlobal(&_globalTemplate);

  if (!_moduleManager.initialize(try_catch, global))
  {
    report_js_exception(try_catch, true);
    _exitValue = -1;
    _context.Dispose();
    _globalTemplate.Dispose();
    return _exitValue;
  }
  
  v8::Handle<v8::String> scriptSource = read_file_skip_shebang(OSS::boost_path(script), true);
  v8::Handle<v8::Script> compiledScript = v8::Script::Compile(scriptSource, v8::String::New(OSS::boost_path(script).c_str()));
  
  if (compiledScript.IsEmpty())
  {
    report_js_exception(try_catch, true);
    _exitValue = -1;
    _context.Dispose();
    _globalTemplate.Dispose();
    return _exitValue;
  }
  //
  // Set the thread id and update the manager
  //
  _threadId = pthread_self();
  JSIsolateManager::instance().registerIsolate(*this);
  
  v8::Handle<v8::Value> result = compiledScript->Run();
  if (result.IsEmpty())
  {
    report_js_exception(try_catch, true);
    _exitValue = -1;
  }

  _context.Dispose();
  _globalTemplate.Dispose();
  
  return _exitValue;
}

bool JSIsolate::call(const std::string& method, const OSS::JSON::Object& arguments, OSS::JSON::Object& reply, uint32_t timeout, void* userData)
{
  return false;
}

bool JSIsolate::call(const OSS::JSON::Object& request, OSS::JSON::Object& reply, uint32_t timeout, void* userData)
{
  return false;
}

bool JSIsolate::notify(const std::string& method, const OSS::JSON::Object& arguments, void* userData)
{
  return false;
}

bool JSIsolate::notify(const OSS::JSON::Object& request, void* userData)
{
  return false;
}

void JSIsolate::terminate()
{
}

bool JSIsolate::isThreadSelf()
{
  return _threadId == pthread_self();
}

JSIsolate::Ptr JSIsolate::getIsolate()
{
  return JSIsolate::Ptr(0);
}

} } 



