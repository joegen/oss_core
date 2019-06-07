#include <unistd.h>
#include <pthread.h>

#include "OSS/OSS.h"
#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/modules/BufferObject.h"
#include "OSS/Net/Net.h"




JS_METHOD_IMPL(__close)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  int fd = js_method_arg_as_int32(0);
  return JSInt32(::close(fd));
}

JS_METHOD_IMPL(__exit)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  ::exit(js_method_arg_as_int32(0));
  return JSUndefined();
}

JS_METHOD_IMPL(___exit)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  ::_exit(js_method_arg_as_int32(0));
  return JSUndefined();
}

JS_METHOD_IMPL(__write)
{
  js_method_arg_assert_size_gteq(2);
  js_method_arg_assert_int32(0);
  int32_t fd = js_method_arg_as_int32(0);
  uint32_t size = 0;
  if (js_method_arg_length() == 3)
  {
    js_method_arg_assert_uint32(2);
    size = js_method_arg_as_uint32(2);
  }
  
  if (js_method_arg_is_string(1))
  {
    std::string data = js_method_arg_as_std_string(1);
    return JSInt32(::write(fd, data.data(), size ? size : data.size()));
  }
  else if (js_method_arg_is_array(1))
  {
    JSArrayHandle args1 = js_method_arg_as_array(1);
    ByteArray bytes;
    if (!js_int_array_to_byte_array(args1, bytes))
    {
      return JSInt32(-1);
    }
    return JSInt32(::write(fd, bytes.data(), size ? size : bytes.size()));
  }
  else if (js_method_arg_is_buffer(1))
  {
    BufferObject* pBuffer = js_method_arg_unwrap_object(BufferObject, 1);
    return JSInt32(::write(fd, pBuffer->buffer().data(), size ? size : pBuffer->buffer().size()));
  }
  
  return JSException("Invalid Argument");
}

JS_METHOD_IMPL(__cout)
{
  js_method_arg_declare_string(msg, 0);
  std::cout << msg;
  return JSUndefined();
}

JS_METHOD_IMPL(__oendl)
{
  std::cout << std::endl;
  return JSUndefined();
}

JS_METHOD_IMPL(__cerr)
{
  js_method_arg_declare_string(msg, 0);
  std::cerr << msg;
  return JSUndefined();
}

JS_METHOD_IMPL(__eendl)
{
  std::cerr << std::endl;
  return JSUndefined();
}

JS_METHOD_IMPL(__read)
{
  js_method_arg_assert_size_gteq(2);
  js_method_arg_assert_int32(0);
  js_method_arg_assert_uint32(1);

  
  int32_t fd = js_method_arg_as_int32(0);
  uint32_t len = js_method_arg_as_uint32(1);
  
  if (js_method_arg_length() == 2)
  {
    //
    // Create a new Buffer
    //
    JSValueHandle result = BufferObject::createNew(len);
    BufferObject* pBuffer = js_unwrap_object(BufferObject, result->ToObject());

    ByteArray& buf = pBuffer->buffer();
    std::size_t ret = ::read(fd, buf.data(), buf.size());
    if (!ret)
    {
      return JSUndefined();
    }
    return result;
  }
  else if (js_method_arg_length() == 3)
  {
    //
    // Use the provided buffer
    //
    js_method_arg_assert_buffer(2);
    BufferObject* pBuffer = js_method_arg_unwrap_object(BufferObject, 2);
    if (len > pBuffer->buffer().size())
    {
      js_throw("Length paramater exceeds buffer size");
    }
    ByteArray& buf = pBuffer->buffer();
    std::size_t ret = ::read(fd, buf.data(), len);
    return JSInt32(ret);
  }
  return JSUndefined();
}

JS_METHOD_IMPL(__sleep)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  ::sleep(js_method_arg_as_uint32(0));
  return JSUndefined();
}

JS_METHOD_IMPL(__gc)
{
  v8::V8::LowMemoryNotification();
  while(!v8::V8::IdleNotification());
  return JSUndefined();
}

JS_METHOD_IMPL(__setsid)
{
  return JSInt32(setsid());
}

JS_METHOD_IMPL(__getdtablesize)
{
  return JSInt32(getdtablesize());
}

JS_METHOD_IMPL(__getpid)
{
  return JSInt32(getpid());
}

JS_METHOD_IMPL(__getppid)
{
  return JSInt32(getppid());
}

#if !defined(__APPLE__)
JS_METHOD_IMPL(__thread_self)
{
  return JSUInt32(pthread_self());
}
#endif

JS_METHOD_IMPL(__write_pid_file)
{
  std::string pidFile;
  bool exclusive = false;
  
  int argc = js_method_arg_length();
  if (argc < 1)
  {
    return JSFalse;
  } 
  else if (argc == 1)
  {
    pidFile = js_method_arg_as_std_string(0);
  } 
  else if (argc == 2)
  {
    pidFile = js_method_arg_as_std_string(0);
    exclusive = js_method_arg_as_bool(1);
  }
  
  int handle = open(pidFile.c_str(), O_RDWR|O_CREAT, 0600);
  if (handle == -1)
  {
    return JSFalse;
  }
  
  if (exclusive && lockf(handle,F_TLOCK,0) == -1)
  {
    return JSFalse;
  }
  
  pid_t pid = getpid();
  
  char pidStr[10];
  sprintf(pidStr,"%d\n", pid);
  if (write(handle, pidStr, strlen(pidStr)) == -1)
  {
    return JSFalse;
  }
  
  if (!exclusive) {
    close(handle);
  }
  
  return JSTrue;
}

JS_METHOD_IMPL(__enable_crash_handler)
{
  OSS::OSS_enable_crash_handler();
  return JSTrue;
}

JS_EXPORTS_INIT()
{
  js_export_method("read", __read);
  js_export_method("write", __write);
  js_export_method("close", __close);
  js_export_method("exit", __exit);
  js_export_method("_exit", ___exit);
  js_export_method("sleep", __sleep);
  js_export_method("gc", __gc);
  js_export_method("setsid", __setsid);
  js_export_method("getdtablesize", __getdtablesize);
  js_export_method("getpid", __getpid);
  js_export_method("getppid", __getppid);
  js_export_method("cout", __cout);
  js_export_method("endl", __oendl);
  js_export_method("cerr", __cerr);
  js_export_method("eendl", __eendl);
  js_export_method("enable_crash_handler", __enable_crash_handler);
#if !defined(__APPLE__)
  js_export_method("thread_self", __thread_self);
#endif
  js_export_method("write_pid_file", __write_pid_file);
  

  //
  // Export system directory variables
  //
  js_export_string("PREFIX", OSS::system_prefix().c_str());
  js_export_string("EXEC_PREFIX", OSS::system_exec_prefix().c_str());
  js_export_string("BINDIR", OSS::system_bindir().c_str());
  js_export_string("SBINDIR", OSS::system_sbindir().c_str());
  js_export_string("LIBEXECDIR", OSS::system_libexecdir().c_str());
  js_export_string("DATADIR", OSS::system_datadir().c_str());
  js_export_string("CONFDIR", OSS::system_confdir().c_str());
  js_export_string("LOCALSTATEDIR", OSS::system_localstatedir().c_str());
  js_export_string("INCLUDEDIR", OSS::system_includedir().c_str());
  js_export_string("LIBDIR", OSS::system_libdir().c_str());
  
  //
  // Export default interface info
  //
  std::string iface, local_ipv4, local_ipv6;
  OSS::net_get_default_interface_name(iface);
  OSS::net_get_default_interface_address(local_ipv4, AF_INET);
  OSS::net_get_default_interface_address(local_ipv6, AF_INET6);
  js_export_string("NET_INTERFACE", iface.c_str());
  js_export_string("NET_IPV4", local_ipv4.c_str());
  js_export_string("NET_IPV6", local_ipv6.c_str());
  
  js_export_finalize(); 
}

JS_REGISTER_MODULE(System);
