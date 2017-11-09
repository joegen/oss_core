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

#include <unistd.h>
#include <sys/file.h>
#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/modules/FileObject.h"
#include "OSS/JS/modules/BufferObject.h"

static int LINE_MAX_LEN = 0;
//
// Define the Interface
//
JS_CLASS_INTERFACE(FileObject, "File") 
{
  JS_CLASS_METHOD_DEFINE(FileObject, "fopen", _fopen);
  JS_CLASS_METHOD_DEFINE(FileObject, "fclose", _fclose);
  JS_CLASS_METHOD_DEFINE(FileObject, "fseek", _fseek);
  JS_CLASS_METHOD_DEFINE(FileObject, "rewind", _rewind);
  JS_CLASS_METHOD_DEFINE(FileObject, "fflush", _fflush);
  JS_CLASS_METHOD_DEFINE(FileObject, "feof", _feof);
  JS_CLASS_METHOD_DEFINE(FileObject, "ferror", _ferror);
  JS_CLASS_METHOD_DEFINE(FileObject, "fread", _fread);
  JS_CLASS_METHOD_DEFINE(FileObject, "fwrite", _fwrite);
  JS_CLASS_METHOD_DEFINE(FileObject, "fgets", _fgets);
  JS_CLASS_METHOD_DEFINE(FileObject, "fileno", _fileno);
  JS_CLASS_METHOD_DEFINE(FileObject, "fmemopen", _fmemopen);
  JS_CLASS_METHOD_DEFINE(FileObject, "fputc", _fputc);
  JS_CLASS_METHOD_DEFINE(FileObject, "fputs", _fputs);
  JS_CLASS_METHOD_DEFINE(FileObject, "ftell", _ftell);
  JS_CLASS_METHOD_DEFINE(FileObject, "flock", _flock);
  
  JS_CLASS_INTERFACE_END(FileObject); 
}

FileObject::FileObject() :
  _pFile(0)
{
  sysconf(_SC_LINE_MAX);
}

FileObject::~FileObject()
{
  if (_pFile)
  {
    ::fclose(_pFile);
    _pFile = 0;
  }
}

JS_CONSTRUCTOR_IMPL(FileObject) 
{
  js_enter_scope();
  FileObject* pFile = new FileObject();
  pFile->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(FileObject::_fopen)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_string(0);
  js_method_arg_assert_string(1);
  std::string file = js_method_arg_as_std_string(0);
  std::string options = js_method_arg_as_std_string(1);
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  pFile->_pFile = ::fopen(file.c_str(), options.c_str());
  return JSBoolean(pFile->_pFile != 0);
}

JS_METHOD_IMPL(FileObject::_fmemopen)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_uint32(0);
  js_method_arg_assert_string(1);
  
  int len = js_method_arg_as_int32(0);
  std::string options = js_method_arg_as_std_string(1);
  
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(len, "Invalid Argument");
  js_assert(pFile->_pFile, "Invalid Argument");
  JSValueHandle result = BufferObject::createNew(len);
  BufferObject* pBuffer = js_unwrap_object(BufferObject, result->ToObject());
  ByteArray& buf = pBuffer->buffer();
  pFile->_pFile = ::fmemopen(buf.data(), buf.size(), options.c_str());
  return JSBoolean(pFile->_pFile != 0);
}

JS_METHOD_IMPL(FileObject::_fclose)
{
  js_enter_scope();
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  if (pFile->_pFile)
  {
    FILE* pHandle = pFile->_pFile;
    pFile->_pFile = 0;
    return JSInt32(::fclose(pHandle));
  }
  return JSInt32(EOF);
}

JS_METHOD_IMPL(FileObject::_fseek)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_int32(0);
  js_method_arg_assert_int32(1);
  int32_t offset = js_method_arg_as_int32(0);
  int32_t whence = js_method_arg_as_int32(1);
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid Argument");
  return JSInt32( ::fseek(pFile->_pFile, offset, whence) );
}

JS_METHOD_IMPL(FileObject::_rewind)
{
  js_enter_scope();
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid Argument");
  ::rewind(pFile->_pFile);
  return JSUndefined();
}

JS_METHOD_IMPL(FileObject::_fflush)
{
  js_enter_scope();
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid Argument");
  return JSInt32(::fflush(pFile->_pFile));
}

JS_METHOD_IMPL(FileObject::_feof)
{
  js_enter_scope();
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid Argument");
  return JSBoolean(::feof(pFile->_pFile) != 0);
}

JS_METHOD_IMPL(FileObject::_ferror)
{
  js_enter_scope();
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid Argument");
  return JSInt32(::ferror(pFile->_pFile));
}

JS_METHOD_IMPL(FileObject::_fread)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  int len = js_method_arg_as_int32(0);
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(len, "Invalid Argument");
  js_assert(pFile->_pFile, "Invalid Argument");
  JSValueHandle result = BufferObject::createNew(len);
  BufferObject* pBuffer = js_unwrap_object(BufferObject, result->ToObject());
  ByteArray& buf = pBuffer->buffer();
  std::size_t ret = ::fread(buf.data(), buf.size(), len, pFile->_pFile);
  if (!ret)
  {
    return JSUndefined();
  }
  return result;
}

JS_METHOD_IMPL(FileObject::_fwrite)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid Argument");
  
  if (js_method_arg_is_string(0))
  {
    std::string data = js_method_arg_as_std_string(0);
    return JSInt32(::fwrite(data.data(), 1, data.size(), pFile->_pFile));
  }
  else if (js_method_arg_is_array(0))
  {
    JSArrayHandle args1 = js_method_arg_as_array(0);
    ByteArray bytes;
    if (!js_int_array_to_byte_array(args1, bytes))
    {
      return JSInt32(-1);
    }
    return JSInt32(::fwrite(bytes.data(), 1, bytes.size(), pFile->_pFile));
  }
  else if (js_method_arg_is_buffer(0))
  {
    BufferObject* pBuffer = js_method_arg_unwrap_object(BufferObject, 0);
    return JSInt32(::fwrite(pBuffer->buffer().data(), 1, pBuffer->buffer().size(), pFile->_pFile));
  }
  
  return JSException("Invalid Argument");
}

JS_METHOD_IMPL(FileObject::_fgets)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  int len = js_method_arg_as_int32(0);
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(len, "Invalid Argument");
  js_assert(pFile->_pFile, "Invalid Argument");
  JSValueHandle result = BufferObject::createNew(len);
  BufferObject* pBuffer = js_unwrap_object(BufferObject, result->ToObject());
  ByteArray& buf = pBuffer->buffer();
  if (!::fgets((char*)buf.data(), buf.size(), pFile->_pFile))
  {
    return JSUndefined();
  }
  return result;
}

JS_METHOD_IMPL(FileObject::_fileno)
{
  js_enter_scope();
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid State");
  return  JSInt32(::fileno(pFile->_pFile));
}

JS_METHOD_IMPL(FileObject::_fputc)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid State");
  int c = js_method_arg_as_int32(0);
  return JSInt32(::fputc(c, pFile->_pFile));
}

JS_METHOD_IMPL(FileObject::_fputs)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string data = js_method_arg_as_std_string(0);
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid State");
  return JSInt32(::fputs(data.c_str(), pFile->_pFile));
}

JS_METHOD_IMPL(FileObject::_ftell)
{
  js_enter_scope();
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid State");
  return JSInt32(::ftell(pFile->_pFile));
}

JS_METHOD_IMPL(FileObject::_flock)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  int32_t operation = js_method_arg_as_int32(0);
  FileObject* pFile = js_method_arg_unwrap_self(FileObject);
  js_assert(pFile->_pFile, "Invalid State");
  return JSInt32(::flock(::fileno(pFile->_pFile), operation));
}

JS_EXPORTS_INIT()
{
  js_enter_scope();
  js_export_class(FileObject);
  js_export_global_constructor("File", FileObject::_constructor);
  
  LINE_MAX_LEN = sysconf(_SC_LINE_MAX);
  js_export_const(SEEK_SET);
  js_export_const(SEEK_CUR);
  js_export_const(SEEK_END);
  js_export_const(LINE_MAX_LEN);
  js_export_const(LOCK_SH);
  js_export_const(LOCK_EX)
  js_export_const(LOCK_UN);
  js_export_const(LOCK_NB);
  
  
  js_export_finalize();
}

JS_REGISTER_MODULE(JSFile);
