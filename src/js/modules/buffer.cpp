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

#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/modules/BufferObject.h"

using OSS::JS::ObjectWrap;


JSPersistentFunctionHandle BufferObject::createNewFunc;
JSPersistentFunctionHandle BufferObject::_constructor;

//
// Define the Interface
//
JS_CLASS_INTERFACE(BufferObject, "Buffer") 
{
  JS_CLASS_METHOD_DEFINE(BufferObject, "size", size);
  JS_CLASS_METHOD_DEFINE(BufferObject, "toArray", toArray);
  JS_CLASS_METHOD_DEFINE(BufferObject, "toString", toString);
  JS_CLASS_METHOD_DEFINE(BufferObject, "fromArray", fromArray);
  JS_CLASS_METHOD_DEFINE(BufferObject, "fromString", fromString);
  JS_CLASS_METHOD_DEFINE(BufferObject, "fromBuffer", fromBuffer);
  JS_CLASS_METHOD_DEFINE(BufferObject, "equals", equals);
  JS_CLASS_INDEX_ACCESSOR_DEFINE(BufferObject, getAt, setAt); 
  JS_CLASS_INTERFACE_END(BufferObject); 
}

BufferObject::BufferObject()
{
}

BufferObject::BufferObject(const BufferObject& obj) :
  _buffer(obj._buffer)
{
}

BufferObject::BufferObject(std::size_t size) :
  _buffer(size)
{
}

BufferObject::~BufferObject()
{
}

JSValueHandle BufferObject::createNew(uint32_t size)
{
  js_begin_scope();
  JSValueHandle funcArgs[1];
  funcArgs[0] = JSUInt32(size);
  return BufferObject::createNewFunc->Call((*JSPlugin::_pContext)->Global(), 1, funcArgs);
}

bool BufferObject::isBuffer(JSValueHandle value)
{
  js_begin_scope();
  return !value.IsEmpty() && value->IsObject() &&
    value->ToObject()->Has(JSLiteral("ObjectType")) &&
    value->ToObject()->Get(JSLiteral("ObjectType"))->ToString()->Equals(JSLiteral("Buffer"));
}

JS_METHOD_IMPL(BufferObject::New)
{
  BufferObject* pBuffer = 0;
  
  if (js_method_get_arg_length() == 1 && js_method_arg_is_number(0))
  {
    pBuffer = new BufferObject(js_method_arg_as_integer(0));
  }
  else if (js_method_get_arg_length() == 1 && js_method_arg_is_string(0))
  {
    std::string str = js_method_arg_as_cstr(0);
    pBuffer = new BufferObject();
    if (!js_string_to_byte_array(str, pBuffer->_buffer))
    {
      delete pBuffer;
      js_throw("Invalid String Elements");
    }
  }
  else if (js_method_get_arg_length() == 1 && js_method_arg_is_array(0))
  {
    JSArrayHandle array = js_method_arg_as_array(0);
    pBuffer = new BufferObject();
    if (!js_int_array_to_byte_array(array, pBuffer->_buffer))
    {
      delete pBuffer;
      js_throw("Invalid Array Elements");
    }
  }
  else if (js_method_get_arg_length() == 1 && BufferObject::isBuffer(js_method_arg_as_object(0)))
  {
    BufferObject* obj = js_method_arg_unwrap_object(BufferObject, 0);
    if (!obj)
    {
      js_throw("Invalid Buffer Object");
    }
    pBuffer = new BufferObject(*obj);
  }
  else if(js_method_get_arg_length() != 0)
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  else
  {
    pBuffer = new BufferObject();
  }
  
  pBuffer->Wrap(js_method_arg_self());
  
  return js_method_result(js_method_arg_self());
}

JS_METHOD_IMPL(BufferObject::size)
{
  return js_method_result(JSInteger(js_method_arg_unwrap_self(BufferObject)->_buffer.size()));
}

JS_METHOD_IMPL(BufferObject::fromArray)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_array(0);
  JSArrayHandle array = js_method_arg_as_array(0);
  BufferObject* pBuffer = js_method_arg_unwrap_self(BufferObject);
  if (!js_int_array_to_byte_array(array, pBuffer->_buffer))
  {
    js_throw("Invalid Array Elements");
  }
  return js_method_result(JSUndefined());
}

JS_METHOD_IMPL(BufferObject::fromString)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_string(0);
  std::string str = js_method_arg_as_cstr(0);
  BufferObject* pBuffer = js_method_arg_unwrap_self(BufferObject);
  if (!js_string_to_byte_array(str, pBuffer->_buffer))
  {
    js_throw("Invalid Array Elements");
  }
  return js_method_result(JSUndefined());
}

JS_METHOD_IMPL(BufferObject::fromBuffer)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_object(0);
  if (!BufferObject::isBuffer(js_method_arg_as_object(0)))
  {
    js_throw("Invalid Argument");
  }
  BufferObject* theirs = js_method_arg_unwrap_object(BufferObject, 0);
  BufferObject* ours = js_method_arg_unwrap_self(BufferObject);
  ours->_buffer = theirs->_buffer;
  return js_method_result(JSUndefined());
}

JS_METHOD_IMPL(BufferObject::toArray)
{
  BufferObject* pBuffer = js_method_arg_unwrap_self(BufferObject);
  JSArrayHandle output = JSArray(pBuffer->_buffer.size());
  js_byte_array_to_int_array(pBuffer->_buffer, output);
  return js_method_result(output);
}

JS_METHOD_IMPL(BufferObject::toString) 
{
  BufferObject* pBuffer = js_method_arg_unwrap_self(BufferObject);
  return js_method_result(JSString((const char*)pBuffer->_buffer.data(), pBuffer->_buffer.size()));
}

JS_METHOD_IMPL(BufferObject::equals) 
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_object(0);
  
  if (!BufferObject::isBuffer(js_method_arg_as_object(0)))
  {
    return JSBoolean(false);
  }
  BufferObject* theirs = js_method_arg_unwrap_object(BufferObject, 0);
  BufferObject* ours = js_method_arg_unwrap_self(BufferObject);
  return js_method_result(JSBoolean(ours->_buffer == theirs->_buffer));
}

JS_METHOD_IMPL(BufferObject::isBufferObject)
{
  return js_method_result(JSBoolean(BufferObject::isBuffer(js_method_arg(0))));
}

//
// Properties
//
JS_INDEX_GETTER_IMPL(BufferObject::getAt)
{
  BufferObject* pBuffer = js_getter_info_unwrap_self(BufferObject);
  if (index >= pBuffer->_buffer.size())
  {
    js_throw("Index Out Of Range");
  }
  return js_getter_result(JSUInt32(pBuffer->_buffer[js_getter_index()]));
}

JS_INDEX_SETTER_IMPL(BufferObject::setAt)
{
  BufferObject* pBuffer = js_setter_info_unwrap_self(BufferObject);
  
  if (js_setter_index() >= pBuffer->_buffer.size())
  {
    js_throw("Index Out Of Range");
  }
  uint32_t val = js_setter_value_as_uint32();
  if (val > 256)
  {
    js_throw("Invalid Argument");
  }
  pBuffer->_buffer[js_setter_index()] = val;
  return js_setter_result(JSUndefined());
}

JS_EXPORTS_INIT()
{
  js_export_method("isBuffer", BufferObject::isBufferObject);
  js_export_class(BufferObject);
  js_export_global_constructor("Buffer", BufferObject::_constructor);
  //
  // Export the createNew function
  //
  JSValueHandle newBufferFunc = js_get_global_method("__create_buffer_object");
  if (js_is_function(newBufferFunc))
  {
    BufferObject::createNewFunc = JSPersistentFunctionCast(newBufferFunc);
  }
  
  // Must always be called last or code won't compile
  js_export_finalize(); 
}

JS_REGISTER_MODULE(Buffer);