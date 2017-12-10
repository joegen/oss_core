#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/modules/ProcessObject.h"

using OSS::JS::JSObjectWrap;

JS_CLASS_INTERFACE(ProcessObject, "Process") 
{
  JS_CLASS_METHOD_DEFINE(ProcessObject, "run", run);
  JS_CLASS_METHOD_DEFINE(ProcessObject, "kill", kill);
  JS_CLASS_METHOD_DEFINE(ProcessObject, "stop", stop);
  JS_CLASS_METHOD_DEFINE(ProcessObject, "restart", restart);
  JS_CLASS_METHOD_DEFINE(ProcessObject, "unmonitor", unmonitor);
  JS_CLASS_METHOD_DEFINE(ProcessObject, "isAlive", isAlive);
  JS_CLASS_METHOD_DEFINE(ProcessObject, "getPid", getPid);
  JS_CLASS_INTERFACE_END(ProcessObject); 
}

ProcessObject::ProcessObject() :
  _pProcess(0)
{
}

ProcessObject::~ProcessObject()
{
  delete _pProcess;
}

JS_CONSTRUCTOR_IMPL(ProcessObject)
{
  js_enter_scope();
  ProcessObject* pProcess = 0;
  
  js_method_arg_assert_size_gteq(2);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  
  std::string processName = js_method_arg_as_std_string(0);
  std::string startupCommand = js_method_arg_as_std_string(1);
  std::string shutdownCommand;
  std::string pidFile;
  
  if (js_method_arg_length() >= 3)
  {
    js_method_arg_assert_string(2);
    startupCommand = js_method_arg_as_std_string(2);
  }
  
  if (js_method_arg_length() >= 4)
  {
    js_method_arg_assert_string(3);
    pidFile = js_method_arg_as_std_string(3);
  }
  
  pProcess = new ProcessObject();
  pProcess->_pProcess = new ProcessObject::Process(processName, startupCommand, shutdownCommand, pidFile);
  pProcess->_pProcess->setDeadProcAction(ProcessObject::Process::ProcessUnmonitor);
  pProcess->Wrap(js_method_arg_self());
  
  return js_method_arg_self();
}

JS_METHOD_IMPL(ProcessObject::run)
{
  js_enter_scope();
  ProcessObject* pProcess = js_method_arg_unwrap_self(ProcessObject);
  return JSBoolean(pProcess->_pProcess->executeAndMonitor());
}

JS_METHOD_IMPL(ProcessObject::kill)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  int signal = js_method_arg_as_int32(0);
  ProcessObject* pProcess = js_method_arg_unwrap_self(ProcessObject);
  return JSInt32(pProcess->_pProcess->kill(signal));
}

JS_METHOD_IMPL(ProcessObject::stop)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  int signal = js_method_arg_as_int32(0);
  ProcessObject* pProcess = js_method_arg_unwrap_self(ProcessObject);
  return JSBoolean(pProcess->_pProcess->shutDown(signal));
}

JS_METHOD_IMPL(ProcessObject::restart)
{
  js_enter_scope();
  ProcessObject* pProcess = js_method_arg_unwrap_self(ProcessObject);
  return JSBoolean(pProcess->_pProcess->restart());
}

JS_METHOD_IMPL(ProcessObject::unmonitor)
{
  js_enter_scope();
  ProcessObject* pProcess = js_method_arg_unwrap_self(ProcessObject);
  pProcess->_pProcess->unmonitor();
  return JSUndefined();
}

JS_METHOD_IMPL(ProcessObject::isAlive)
{
  js_enter_scope();
  ProcessObject* pProcess = js_method_arg_unwrap_self(ProcessObject);
  return JSBoolean(pProcess->_pProcess->isAlive());
}

JS_METHOD_IMPL(ProcessObject::getPid)
{
  js_enter_scope();
  ProcessObject* pProcess = js_method_arg_unwrap_self(ProcessObject);
  return JSInt32(pProcess->_pProcess->getPID());
}

JS_EXPORTS_INIT()
{
  js_enter_scope();
  js_export_class(ProcessObject);
  js_export_finalize(); 
}

JS_REGISTER_MODULE(JSProcess);