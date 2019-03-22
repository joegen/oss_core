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
#include "OSS/JS/JSEventLoop.h"
#include "OSS/UTL/Logger.h"

using OSS::JS::JSIsolateManager;

JS_CLASS_INTERFACE(IsolateObject, "Isolate") 
{
  JS_CLASS_METHOD_DEFINE(IsolateObject, "run", run);
  JS_CLASS_METHOD_DEFINE(IsolateObject, "runSource", runSource);
  JS_CLASS_METHOD_DEFINE(IsolateObject, "stop", stop);
  JS_CLASS_METHOD_DEFINE(IsolateObject, "join", join);
  JS_CLASS_METHOD_DEFINE(IsolateObject, "execute", execute);
  JS_CLASS_METHOD_DEFINE(IsolateObject, "notify", notify);
  JS_CLASS_INTERFACE_END(IsolateObject); 
}

IsolateObject::IsolateObject(bool isRoot)
{
  if (!isRoot)
  {
    _pIsolate = JSIsolateManager::instance().createIsolate(pthread_self());
  }
  else
  {
    _pIsolate = JSIsolateManager::instance().rootIsolate();
  }
}

IsolateObject::~IsolateObject()
{
  _pIsolate->terminate();
  _pIsolate->join();
}

JS_CONSTRUCTOR_IMPL(IsolateObject) 
{
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
  js_method_arg_declare_self(IsolateObject, pSelf);
  pSelf->_pIsolate->terminate();
  return JSUndefined();
}

JS_METHOD_IMPL(IsolateObject::join)
{
  js_method_arg_declare_self(IsolateObject, pSelf);
  pSelf->_pIsolate->join();
  return JSUndefined();
}

JS_METHOD_IMPL(setChildInterIsolateHandler)
{
  js_method_arg_declare_persistent_function(func, 0);
  js_method_arg_declare_uint32(eventEmitterFd, 1);
  OSS::JS::JSIsolateManager::instance().getIsolate()->setEventEmitterFd(eventEmitterFd);
  OSS::JS::JSIsolateManager::instance().getIsolate()->eventLoop()->interIsolate().setHandler(func);
  return JSUndefined();
}

JS_METHOD_IMPL(setRootInterIsolateHandler)
{
  js_method_arg_declare_persistent_function(func, 0);
  js_method_arg_declare_uint32(eventEmitterFd, 1);
  OSS::JS::JSIsolateManager::instance().rootIsolate()->setEventEmitterFd(eventEmitterFd);
  OSS::JS::JSIsolateManager::instance().rootIsolate()->eventLoop()->interIsolate().setHandler(func);
  return JSUndefined();
}

JS_METHOD_IMPL(IsolateObject::execute)
{
  js_enter_scope();
  js_method_arg_declare_self(IsolateObject, pSelf);
  js_method_arg_declare_string(request, 0);
  js_method_arg_declare_uint32(timeout, 1);
  std::string result;
  if (!pSelf->_pIsolate->eventLoop()->interIsolate().execute(request, result, timeout, 0))
  {
    return JSUndefined();
  }
  return JSString(result);
}

JS_METHOD_IMPL(IsolateObject::notify)
{
  js_enter_scope();
  js_method_arg_declare_self(IsolateObject, pSelf);
  js_method_arg_declare_string(request, 0);
  pSelf->_pIsolate->eventLoop()->interIsolate().notify(request, 0);
  return JSUndefined();
}

JS_METHOD_IMPL(notifyParentIsolate)
{
  js_enter_scope();
  js_method_arg_declare_string(request, 0);
  OSS::JS::JSIsolate::Ptr pIsolate = OSS::JS::JSIsolateManager::instance().getIsolate(); 
  if (pIsolate && !pIsolate->isRoot())
  {
    pIsolate->getParentIsolate()->notify(request, 0);
  }
  return JSUndefined();
}

JS_METHOD_IMPL(isRootIsolate)
{
  return JSBoolean(OSS::JS::JSIsolateManager::instance().rootIsolate()->isThreadSelf());
}

JS_EXPORTS_INIT()
{
  js_export_method("isRootIsolate", isRootIsolate);
  js_export_method("setRootInterIsolateHandler", setRootInterIsolateHandler);
  js_export_method("setChildInterIsolateHandler", setChildInterIsolateHandler);
  js_export_method("notifyParentIsolate", notifyParentIsolate);
  js_export_class(IsolateObject);
  js_export_finalize();
}

JS_REGISTER_MODULE(JSIsolateObject);