#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include <unistd.h>
#include "OSS/JS/modules/BufferObject.h"


JS_METHOD_IMPL(__close)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  int fd = js_method_arg_as_int32(0);
  return js_method_result(JSInt32(::close(fd)));
}

JS_METHOD_IMPL(__exit)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  ::exit(js_method_arg_as_int32(0));
  return js_method_result(JSUndefined());
}

JS_METHOD_IMPL(___exit)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  ::_exit(js_method_arg_as_int32(0));
  return js_method_result(JSUndefined());
}

JS_METHOD_IMPL(__write)
{
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_int32(0);
  int fd = js_method_arg_as_int32(0);
  if (js_method_arg_is_string(1))
  {
    std::string data = js_method_arg_as_std_string(1);
    return JSInt32(::write(fd, data.data(), data.size()));
  }
  else if (js_method_arg_is_array(1))
  {
    JSArrayHandle args1 = js_method_arg_as_array(1);
    ByteArray bytes;
    if (!js_int_array_to_byte_array(args1, bytes))
    {
      return JSInt32(-1);
    }
    return JSInt32(::write(fd, bytes.data(), bytes.size()));
  }
  else if (js_method_arg_is_buffer(1))
  {
    BufferObject* pBuffer = js_method_arg_unwrap_object(BufferObject, 1);
    return JSInt32(::write(fd, pBuffer->buffer().data(), pBuffer->buffer().size()));
  }
  
  return js_method_result(JSException("Invalid Argument"));
}

JS_METHOD_IMPL(__read)
{
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_int32(0);
  js_method_arg_assert_int32(1);

  
  int fd = js_method_arg_as_int32(0);
  int len = js_method_arg_as_int32(1);
  
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
  return js_method_result(result);
}

JS_METHOD_IMPL(__sleep)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  ::sleep(js_method_arg_as_uint32(0));
  return js_method_result(JSUndefined());
}
JS_METHOD_IMPL(__gc)
{
  v8::V8::LowMemoryNotification();
  while(!v8::V8::IdleNotification());
  return js_method_result(JSUndefined());
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
  
  js_export_finalize(); 
}

JS_REGISTER_MODULE(System);
