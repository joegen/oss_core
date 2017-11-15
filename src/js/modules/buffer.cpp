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
  JS_CLASS_METHOD_DEFINE(BufferObject, "resize", resize);
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
  js_enter_scope();
  JSValueHandle funcArgs[1];
  funcArgs[0] = JSUInt32(size);
  return BufferObject::createNewFunc->Call((*JSPlugin::_pContext)->Global(), 1, funcArgs);
}

bool BufferObject::isBuffer(JSValueHandle value)
{
  js_enter_scope();
  return !value.IsEmpty() && value->IsObject() &&
    value->ToObject()->Has(JSLiteral("ObjectType")) &&
    value->ToObject()->Get(JSLiteral("ObjectType"))->ToString()->Equals(JSLiteral("Buffer"));
}

JS_CONSTRUCTOR_IMPL(BufferObject)
{
  js_enter_scope();
  BufferObject* pBuffer = 0;
  
  if (js_method_arg_length() == 1 && js_method_arg_is_number(0))
  {
    pBuffer = new BufferObject(js_method_arg_as_integer(0));
  }
  else if (js_method_arg_length() == 1 && js_method_arg_is_string(0))
  {
    std::string str = js_method_arg_as_std_string(0);
    pBuffer = new BufferObject();
    if (!js_string_to_byte_array(str, pBuffer->_buffer, true))
    {
      delete pBuffer;
      js_throw("Invalid String Elements");
    }
  }
  else if (js_method_arg_length() == 1 && js_method_arg_is_array(0))
  {
    JSArrayHandle array = js_method_arg_as_array(0);
    pBuffer = new BufferObject();
    if (!js_int_array_to_byte_array(array, pBuffer->_buffer))
    {
      delete pBuffer;
      js_throw("Invalid Array Elements");
    }
  }
  else if (js_method_arg_length() == 1 && BufferObject::isBuffer(js_method_arg_as_object(0)))
  {
    BufferObject* obj = js_method_arg_unwrap_object(BufferObject, 0);
    if (!obj)
    {
      js_throw("Invalid Buffer Object");
    }
    pBuffer = new BufferObject(*obj);
  }
  else if(js_method_arg_length() != 0)
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  else
  {
    pBuffer = new BufferObject();
  }
  
  pBuffer->Wrap(js_method_arg_self());
  
  return js_method_arg_self();
}

JS_METHOD_IMPL(BufferObject::size)
{
  js_enter_scope();
  return JSInteger(js_method_arg_unwrap_self(BufferObject)->_buffer.size());
}

JS_METHOD_IMPL(BufferObject::fromArray)
{
  js_enter_scope();
  js_method_arg_assert_size_gteq(1);
  js_method_arg_assert_array(0);
  JSArrayHandle array = js_method_arg_as_array(0);
  bool resize = true;
  if (js_method_arg_length() == 2)
  {
    js_method_arg_assert_bool(1);
    resize = js_method_arg_as_bool(1);
  }
  BufferObject* pBuffer = js_method_arg_unwrap_self(BufferObject);
  if (!js_int_array_to_byte_array(array, pBuffer->_buffer, resize))
  {
    js_throw("Invalid Array Elements");
  }
  return JSUndefined();
}

JS_METHOD_IMPL(BufferObject::fromString)
{
  js_enter_scope();
  js_method_arg_assert_size_gteq(1);
  js_method_arg_assert_string(0);
  std::string str = js_method_arg_as_std_string(0);
  bool resize = true;
  if (js_method_arg_length() == 2)
  {
    js_method_arg_assert_bool(1);
    resize = js_method_arg_as_bool(1);
  }
  BufferObject* pBuffer = js_method_arg_unwrap_self(BufferObject);
  if (!js_string_to_byte_array(str, pBuffer->_buffer, resize))
  {
    js_throw("Invalid Array Elements");
  }
  return JSUndefined();
}

JS_METHOD_IMPL(BufferObject::fromBuffer)
{
  js_enter_scope();
  js_method_arg_assert_size_gteq(1);
  js_method_arg_assert_object(0);
  if (!BufferObject::isBuffer(js_method_arg_as_object(0)))
  {
    js_throw("Invalid Argument");
  }
  
  bool resize = true;
  if (js_method_arg_length() == 2)
  {
    js_method_arg_assert_bool(1);
    resize = js_method_arg_as_bool(1);
  }
  
  BufferObject* theirs = js_method_arg_unwrap_object(BufferObject, 0);
  BufferObject* ours = js_method_arg_unwrap_self(BufferObject);
  if (resize)
  {
    ours->_buffer = theirs->_buffer;
  }
  else
  {
    std::size_t bufLen = ours->_buffer.size();
    for (std::size_t i = 0; i < theirs->_buffer.size(); i++)
    {
      if (i < bufLen)
      {
        ours->_buffer[i] = theirs->_buffer[i];
      }
      else
      {
        break;
      }
    }
  }
  return JSUndefined();
}

JS_METHOD_IMPL(BufferObject::toArray)
{
  js_enter_scope();
  BufferObject* pBuffer = js_method_arg_unwrap_self(BufferObject);
  
  uint32_t size = 0;
  if (js_method_arg_length() == 1)
  {
    size = js_method_arg_as_uint32(0);
  }
  
  JSArrayHandle output = JSArray(size ? size : pBuffer->_buffer.size());
  js_byte_array_to_int_array(pBuffer->_buffer, output, size);
  return output;
}

JS_METHOD_IMPL(BufferObject::toString) 
{
  js_enter_scope();
  BufferObject* pBuffer = js_method_arg_unwrap_self(BufferObject);
  
  uint32_t size = pBuffer->_buffer.size();
  if (js_method_arg_length() == 1)
  {
    size = js_method_arg_as_uint32(0);
  }
  
  return JSString((const char*)pBuffer->_buffer.data(), size);
}

JS_METHOD_IMPL(BufferObject::equals) 
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_object(0);
  
  if (!BufferObject::isBuffer(js_method_arg_as_object(0)))
  {
    return JSBoolean(false);
  }
  BufferObject* theirs = js_method_arg_unwrap_object(BufferObject, 0);
  BufferObject* ours = js_method_arg_unwrap_self(BufferObject);
  return JSBoolean(ours->_buffer == theirs->_buffer);
}

JS_METHOD_IMPL(BufferObject::resize)
{
  js_enter_scope();
  BufferObject* buf = js_method_arg_unwrap_self(BufferObject);
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  uint32_t size = js_method_arg_as_uint32(0);
  buf->buffer().resize(size);
  return JSUndefined();
}

JS_METHOD_IMPL(BufferObject::isBufferObject)
{
  js_enter_scope();
  return JSBoolean(BufferObject::isBuffer(js_method_arg(0)));
}

//
// Properties
//
JS_INDEX_GETTER_IMPL(BufferObject::getAt)
{
  js_enter_scope();
  BufferObject* pBuffer = js_getter_info_unwrap_self(BufferObject);
  if (index >= pBuffer->_buffer.size())
  {
    js_throw("Index Out Of Range");
  }
  return JSUInt32(pBuffer->_buffer[js_getter_index()]);
}

JS_INDEX_SETTER_IMPL(BufferObject::setAt)
{
  js_enter_scope();
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
  return JSUndefined();
}



JS_EXPORTS_INIT()
{
  js_enter_scope();
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