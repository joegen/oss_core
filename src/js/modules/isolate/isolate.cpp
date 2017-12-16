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

#include "OSS/JS/modules/IsolateObject.h"
#include "OSS/JS/JSIsolateManager.h"
#include "OSS/UTL/Logger.h"

using OSS::JS::JSIsolateManager;

JS_CLASS_INTERFACE(IsolateObject, "Isolate") 
{
  JS_CLASS_METHOD_DEFINE(IsolateObject, "run", run);
  JS_CLASS_METHOD_DEFINE(IsolateObject, "runSource", runSource);
  JS_CLASS_METHOD_DEFINE(IsolateObject, "stop", stop);
  JS_CLASS_METHOD_DEFINE(IsolateObject, "join", join);
  JS_CLASS_METHOD_DEFINE(IsolateObject, "call", call);
  JS_CLASS_METHOD_DEFINE(IsolateObject, "notify", notify);
  JS_CLASS_INTERFACE_END(IsolateObject); 
}

IsolateObject::IsolateObject(pthread_t parentThreadId)
{
  _pIsolate = JSIsolateManager::instance().createIsolate(parentThreadId);
}

IsolateObject::~IsolateObject()
{
  _pIsolate->terminate();
  _pIsolate->join();
}

JS_CONSTRUCTOR_IMPL(IsolateObject) 
{
  js_enter_scope();
  IsolateObject* pObject = new IsolateObject(pthread_self());
  pObject->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(IsolateObject::run)
{
  js_enter_scope();
  js_method_arg_declare_self(IsolateObject, pSelf);
  js_method_arg_declare_string(path, 0);
  boost::filesystem::path script(path.c_str());
  JSIsolateManager::instance().run(pSelf->_pIsolate, script);
  return JSUndefined();
}

JS_METHOD_IMPL(IsolateObject::runSource)
{
  js_enter_scope();
  js_method_arg_declare_self(IsolateObject, pSelf);
  js_method_arg_declare_string(source, 0);
  JSIsolateManager::instance().runSource(pSelf->_pIsolate, source);
  return JSUndefined();
}

JS_METHOD_IMPL(IsolateObject::stop)
{
  js_enter_scope();
  js_method_arg_declare_self(IsolateObject, pSelf);
  pSelf->_pIsolate->terminate();
  return JSUndefined();
}

JS_METHOD_IMPL(IsolateObject::join)
{
  js_enter_scope();
  js_method_arg_declare_self(IsolateObject, pSelf);
  pSelf->_pIsolate->join();
  return JSUndefined();
}

JS_METHOD_IMPL(IsolateObject::call)
{
  return JSUndefined();
}

JS_METHOD_IMPL(IsolateObject::notify)
{
  return JSUndefined();
}

JS_EXPORTS_INIT()
{
  js_export_class(IsolateObject);
  js_export_finalize();
}

JS_REGISTER_MODULE(JSIsolateObject);