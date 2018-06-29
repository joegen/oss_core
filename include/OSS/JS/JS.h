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


#ifndef OSS_JS_INCLUDED
#define OSS_JS_INCLUDED


#define OSS_JS_MAJOR_VERSION 1
#define OSS_JS_MINOR_VERSION 0
#define OSS_JS_BUILD_NUMBER 0



#include "OSS/OSS.h"
#include <vector>

#include "OSS/JS/JSObjectWrap.h"

#define CONST_EXPORT(Name) exports->Set(v8::String::New(#Name), v8::Integer::New(Name), v8::ReadOnly);
#define TYPE_ERROR_EXCEPTION(Msg) v8::Exception::TypeError(v8::String::NewSymbol(Msg));


//
// Class and method implementation macros
//
#define JS_CONSTRUCTOR_DECLARE() \
  static JSPersistentFunctionHandle _constructor; \
  static void Init(JSObjectHandle exports); \
  static v8::Handle<v8::Value> New(const v8::Arguments& _args_);

typedef v8::Handle<v8::Value> (*JS_METHOD_FUNC)(const v8::Arguments&);

#define JS_METHOD_DECLARE(Method) static v8::Handle<v8::Value> Method(const v8::Arguments& _args_)
#define JS_INDEX_GETTER_DECLARE(Method) static v8::Handle<v8::Value> Method(uint32_t index, const v8::AccessorInfo& _args_);
#define JS_INDEX_SETTER_DECLARE(Method) static v8::Handle<v8::Value> Method(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& _args_);

#define js_export_method(Name, Func) exports->Set(v8::String::NewSymbol(Name), v8::FunctionTemplate::New(Func)->GetFunction())
#define js_export_global_constructor(Name, Func) js_get_global()->Set(v8::String::NewSymbol(Name), Func)
#define js_export_const CONST_EXPORT
#define js_export_string(Name, Value) exports->Set(v8::String::NewSymbol(Name), v8::String::NewSymbol(Value), v8::ReadOnly)
#define js_export_string_symbol(Name) exports->Set(v8::String::NewSymbol(Name), v8::String::NewSymbol(Name), v8::ReadOnly)
#define js_export_int32(Name, Value) exports->Set(v8::String::NewSymbol(Name), v8::Int32::New(Value), v8::ReadOnly)

#define JS_CLASS_INTERFACE(Class, Name) \
  JSPersistentFunctionHandle Class::_constructor; \
  void Class::Init(v8::Handle<v8::Object> exports) {\
  js_enter_scope(); \
  std::string className = Name; \
  v8::Local<v8::FunctionTemplate> tpl = OSS::JS::JSObjectWrap::ExportConstructorTemplate<Class>(Name, exports);

#define JS_CLASS_METHOD_DEFINE(Class, Name, Func) OSS::JS::JSObjectWrap::ExportMethod<Class>(tpl, Name, Func)

#define JS_CLASS_INDEX_ACCESSOR_DEFINE(Class, Getter, Setter) OSS::JS::JSObjectWrap::ExportIndexHandler<Class>(tpl, Getter, Setter);
#define JS_CLASS_INTERFACE_END(Class) } OSS::JS::JSObjectWrap::FinalizeConstructorTemplate<Class>(className.c_str(), tpl, exports);
#define js_export_class(Class) Class::Init(exports)

#define JS_METHOD_IMPL(Method) v8::Handle<v8::Value>  Method(const v8::Arguments& _args_)\

#define JS_CONSTRUCTOR_IMPL(Class) JS_METHOD_IMPL(Class::New)

#define JS_INDEX_GETTER_IMPL(Method) JSValueHandle Method(uint32_t index, const v8::AccessorInfo& _args_)

#define JS_INDEX_SETTER_IMPL(Method)  JSValueHandle Method(uint32_t index,v8::Local<v8::Value> value, const v8::AccessorInfo& _args_)

#define JS_EXPORTS_INIT() static v8::Handle<v8::Value> init_exports(const v8::Arguments& _args_) { \
  v8::Local<v8::Object> exports = v8::Local<v8::Object>::New(v8::Object::New());

#define js_export_finalize() } return exports;

//
// Value Definitions
//
#define JSUndefined() v8::Undefined()
#define JSValueHandle v8::Handle<v8::Value>
#define JSLocalValueHandle v8::Local<v8::Value>
#define JSStringHandle v8::Handle<v8::String>
#define JSLocalStringHandle v8::Local<v8::String>
#define JSArrayHandle v8::Handle<v8::Array>
#define JSLocalArrayHandle v8::Local<v8::Array>
#define JSObjectHandle v8::Handle<v8::Object>
#define JSObjectTemplateHandle v8::Handle<v8::ObjectTemplate>
#define JSLocalObjectHandle v8::Local<v8::Object>
#define JSLocalObjectTemplateHandle v8::Local<v8::ObjectTemplate>
#define JSLocalObjectTemplateHandle v8::Local<v8::ObjectTemplate>
#define JSPersistentObjectHandle v8::Persistent<v8::Object>
#define JSLiteral(Text) v8::String::NewSymbol(Text)
#define JSExternalHandle v8::Handle<v8::External> 
#define JSPersistentFunctionHandle v8::Persistent<v8::Function>
#define JSFunctionHandle v8::Handle<v8::Function>
#define JSLocalFunctionHandle v8::Local<v8::Function>
#define JSPersistentFunctionCast(Handle) JSPersistentFunctionHandle::New(JSFunctionHandle::Cast(Handle))
inline JSStringHandle JSString(const char* str) { return v8::String::New(str); }
inline JSStringHandle JSString(const std::string& str) { return v8::String::New(str.data(), str.size()); }
inline JSStringHandle JSString(const char* str, std::size_t len) { return v8::String::New(str, len); }

#define JSArguments const v8::Arguments
#define JSArgumentVector std::vector< v8::Handle<v8::Value> >
#define JSLocalArgumentVector std::vector< v8::Local<v8::Value> >
#define JSPersistentArgumentVector std::vector< v8::Persistent<v8::Value> >
#define JSBoolean(Exp) v8::Boolean::New(Exp)
#define JSArray(Size) v8::Array::New(Size)
#define JSInt32(Value) v8::Int32::New(Value)
#define JSUInt32(Value) v8::Uint32::New(Value)
#define JSInteger(Value) v8::Integer::New(Value)
#define JSObject() v8::Object::New()
#define JSObjectTemplate() v8::ObjectTemplate::New()
#define JSStringNew(Str) v8::String::New(Str)
#define JSExternal(Ptr) v8::External::New(Ptr)
#define JSException(What) v8::ThrowException(v8::Exception::Error(JSLiteral(What)))

#define JSFalse JSBoolean(false)
#define JSTrue JSBoolean(false)



//
// Helper functions
//
#define js_throw(What) return JSException(What)
#define js_assert(Expression, What) if (!(Expression)) { js_throw(What); }
#define js_is_function(Handle) Handle->IsFunction()
#define js_get_global() OSS::JS::JSIsolateManager::instance().getIsolate()->getGlobal()
#define js_get_global_method(Name) js_get_global()->Get(JSLiteral(Name))
#define js_enter_scope() v8::HandleScope _scope_
#define js_unwrap_object(Class, Object) OSS::JS::JSObjectWrap::Unwrap<Class>(Object)


//
// Argument parsing
//
#define js_method_arg_self() _args_.This()
#define js_method_arg_length() _args_.Length()
#define js_method_arg(Index) _args_[Index]

#define js_method_arg_is_object(Index) _args_[Index]->IsObject()
#define js_method_arg_is_string(Index) _args_[Index]->IsString()
#define js_method_arg_is_array(Index) _args_[Index]->IsArray()
#define js_method_arg_is_number(Index) _args_[Index]->IsNumber()
#define js_method_arg_is_int32(Index) _args_[Index]->IsInt32()
#define js_method_arg_is_uint32(Index) _args_[Index]->IsUint32()
#define js_method_arg_is_bool(Index) _args_[Index]->IsBoolean()
#define js_method_arg_is_date(Index) _args_[Index]->IsDate()
#define js_method_arg_is_buffer(Index) BufferObject::isBuffer(_args_[Index])
#define js_method_arg_is_function(Index) _args_[Index]->IsFunction()

#define js_method_arg_assert_size_eq(Value) if (_args_.Length() != Value) js_throw("Invalid Argument Count")
#define js_method_arg_assert_size_gt(Value) if (_args_.Length() <= Value) js_throw("Invalid Argument Count")
#define js_method_arg_assert_size_lt(Value) if (_args_.Length() >= Value) js_throw("Invalid Argument Count")
#define js_method_arg_assert_size_gteq(Value) if (_args_.Length() < Value) js_throw("Invalid Argument Count")
#define js_method_arg_assert_size_lteq(Value) if (_args_.Length() > Value) js_throw("Invalid Argument Count")
#define js_method_arg_assert_object(Index) if (!js_method_arg_is_object(Index)) js_throw("Invalid Argument Type")
#define js_method_arg_assert_string(Index) if (!js_method_arg_is_string(Index)) js_throw("Invalid Argument Type")
#define js_method_arg_assert_array(Index) if (!js_method_arg_is_array(Index)) js_throw("Invalid Argument Type")
#define js_method_arg_assert_number(Index) if (!js_method_arg_is_number(Index)) js_throw("Invalid Argument Type")
#define js_method_arg_assert_int32(Index) if (!js_method_arg_is_int32(Index)) js_throw("Invalid Argument Type")
#define js_method_arg_assert_uint32(Index) if (!js_method_arg_is_uint32(Index)) js_throw("Invalid Argument Type")
#define js_method_arg_assert_bool(Index) if (!js_method_arg_is_bool(Index)) js_throw("Invalid Argument Type")
#define js_method_arg_assert_date(Index) if (!js_method_arg_is_date(Index)) js_throw("Invalid Argument Type")
#define js_method_arg_assert_buffer(Index) if (!js_method_arg_is_buffer(Index)) js_throw("Invalid Argument Type")
#define js_method_arg_assert_function(Index) if (!js_method_arg_is_function(Index)) js_throw("Invalid Argument Type")

#define js_method_arg_as_object(Index) _args_[Index]->ToObject()
#define js_method_arg_as_string(Index) v8::String::Utf8Value(_args_[Index])
#define js_method_arg_as_std_string(Index) std::string((const char*) *js_method_arg_as_string(Index))
#define js_method_arg_as_array(Index) JSArrayHandle::Cast(_args_[Index])
#define js_method_arg_as_integer(Index) _args_[Index]->IntegerValue()
#define js_method_arg_as_number(Index) _args_[Index]->NumberValue()
#define js_method_arg_as_int32(Index) _args_[Index]->Int32Value()
#define js_method_arg_as_uint32(Index) _args_[Index]->Uint32Value()
#define js_method_arg_as_bool(Index) _args_[Index]->BooleanValue()
#define js_method_arg_as_buffer(Index) js_method_arg_unwrap_object(BufferObject, Index)
#define js_method_arg_as_persistent_function(Index) v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(_args_[Index]))

#define js_method_arg_has_property(Index, Name) _args_[Index]->ToObject()->Has(v8::String::NewSymbol(Name))
#define js_method_arg_get_property(Index, Name) _args_[Index]->ToObject()->Get(v8::String::NewSymbol(Name))

#define js_method_arg_unwrap_object(Class, Index) js_unwrap_object(Class, js_method_arg_as_object(Index))
#define js_method_arg_unwrap_self(Class) js_unwrap_object(Class, js_method_arg_self())

#define js_setter_info_unwrap_self js_method_arg_unwrap_self
#define js_getter_info_unwrap_self js_method_arg_unwrap_self
#define js_setter_value_as_uint32() value->ToUint32()->Value()
#define js_setter_index() index
#define js_getter_index js_setter_index

#define js_handle_as_string(Handle) v8::String::Utf8Value(Handle)
#define js_handle_as_std_string(Handle) std::string((const char*) *js_handle_as_string(Handle))

#define js_method_arg_type(Type, Var, Index, Func, Msg) \
  Type Var; \
  if (Index >= _args_.Length()) \
    js_throw("Invalid Argument Count"); \
  if (!Func(Var, _args_[Index])) \
    js_throw(Msg);

#define js_method_arg_declare_bool(Var, Index) js_method_arg_type(bool, Var, Index, js_assign_bool, "Invalid Type.  Expecting Bool")
#define js_method_arg_declare_uint32(Var, Index) js_method_arg_type(uint32_t, Var, Index, js_assign_uint32, "Invalid Type.  Expecting UInt32")
#define js_method_arg_declare_int32(Var, Index) js_method_arg_type(int32_t, Var, Index, js_assign_int32, "Invalid Type.  Expecting Int32")
#define js_method_arg_declare_number(Var, Index) js_method_arg_type(double, Var, Index, js_assign_number, "Invalid Type.  Expecting Nymber")
#define js_method_arg_declare_string(Var, Index) js_method_arg_type(std::string, Var, Index, js_assign_string, "Invalid Type.  Expecting String")
#define js_method_arg_declare_array(Var, Index) js_method_arg_type(JSArrayHandle, Var, Index, js_assign_array, "Invalid Type.  Expecting Array")
#define js_method_arg_declare_object(Var, Index) js_method_arg_type(JSObjectHandle, Var, Index, js_assign_object, "Invalid Type.  Expecting Object")
#define js_method_arg_declare_external_object(Class, Var, Index) js_method_arg_type(Class*, Var, Index, js_assign_external_object<Class>, "Invalid Type.  Expecting External Object")
#define js_method_arg_declare_function(Var, Index) js_method_arg_type(JSLocalFunctionHandle, Var, Index, js_assign_function, "Invalid Type.  Expecting Function") 
#define js_method_arg_declare_persistent_function(Var, Index) js_method_arg_type(JSPersistentFunctionHandle, Var, Index, js_assign_persistent_function, "Invalid Type.  Expecting Function") 
#define js_method_arg_declare_self(Class, Var) Class* Var = js_method_arg_unwrap_self(Class)

#define js_method_declare_isolate(Var) OSS::JS::JSIsolate::Ptr Var = OSS::JS::JSIsolateManager::instance().getIsolate(); \
  if (!Var) { js_throw("Unable to retrieve isolate"); }


#define js_function_call(Func, Data, Size) Func->Call(js_get_global(), Size, Data)

template <typename T>
bool js_assign_external_object(T*& value, const JSValueHandle& handle)
{
  if (!handle->IsObject())
  {
    return false;
  }
  value = js_unwrap_object(T, handle->ToObject());
  return true;
}

inline bool js_assign_persistent_function(JSPersistentFunctionHandle& value, const JSValueHandle& handle) 
{ 
  if (!handle->IsFunction())
  {
    return false;
  } 
  value =  v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(handle));
  return true; 
}

inline bool js_assign_function(JSLocalFunctionHandle& value, const JSValueHandle& handle) 
{ 
  if (!handle->IsFunction())
  {
    return false;
  } 
  value =  v8::Local<v8::Function>::New(v8::Handle<v8::Function>::Cast(handle));
  return true; 
}

inline bool js_assign_uint32(uint32_t& value, const JSValueHandle& handle) 
{ 
  if (!handle->IsUint32())
  {
    return false;
  } 
  value =  handle->ToUint32()->Value();
  return true; 
}

inline bool js_assign_int32(int32_t& value, const JSValueHandle& handle) 
{ 
  if (!handle->IsInt32())
  {
    return false;
  } 
  value =  handle->ToInt32()->Value();
  return true; 
}

inline bool js_assign_number(double& value, const JSValueHandle& handle) 
{ 
  if (!handle->IsNumber())
  {
    return false;
  } 
  value =  handle->ToNumber()->Value();
  return true; 
}

inline bool js_assign_array(JSArrayHandle& value, const JSValueHandle& handle) 
{ 
  if (!handle->IsArray())
  {
    return false;
  } 
  value =  JSArrayHandle::Cast(handle);
  return true; 
}

inline bool js_assign_string(std::string& value, const JSValueHandle& handle) 
{ 
  if (!handle->IsString())
  {
    return false;
  } 
  value = (const char*) (*v8::String::Utf8Value(handle)); 
  return true; 
}


inline bool js_assign_bool(bool& value, const JSValueHandle& handle) 
{ 
  if (!handle->IsBoolean())
  {
    return false;
  } 
  value = handle->ToBoolean()->Value(); 
  return true; 
}

inline bool js_assign_object(JSObjectHandle& value, const JSValueHandle& handle) 
{ 
  if (!handle->IsObject())
  {
    return false;
  } 
  value = handle->ToObject();
  return true; 
}

inline void js_assign_persistent_arg_vector(JSPersistentArgumentVector& output, const JSValueHandle& handle)
{
  if (handle->IsArray())
  {
    v8::Handle<v8::Array> arrayArg = v8::Handle<v8::Array>::Cast(handle);
    for (std::size_t i = 0; i <arrayArg->Length(); i++)
    {
      output.push_back(v8::Persistent<v8::Value>::New(arrayArg->Get(i)));
    }
  }
  else
  {
    output.push_back(v8::Persistent<v8::Value>::New(handle));
  }
}


inline JSObjectHandle js_wrap_pointer_to_local_object(void* ptr)
{
  JSLocalObjectTemplateHandle objectTemplate = JSObjectTemplate();
  objectTemplate->SetInternalFieldCount(1);
  
  JSLocalObjectTemplateHandle classTemplate = JSLocalObjectTemplateHandle::New(objectTemplate);
  JSObjectHandle JSObjectWrapper = classTemplate->NewInstance();
  JSExternalHandle objectPointer = JSExternal(ptr);
  JSObjectWrapper->SetInternalField(0, objectPointer);
  return JSObjectWrapper;
}

template <typename T>
T* js_unwrap_pointer_from_local_object(JSObjectHandle obj, uint32_t index = 0)
{
  JSExternalHandle ptr = JSExternalHandle::Cast(obj->GetInternalField(index));
  return static_cast<T*>(ptr->Value());
}


#endif // OSS_JS_INCLUDED





