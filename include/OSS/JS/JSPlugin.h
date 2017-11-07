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

#ifndef OSS_JSPLUGIN_H_INCLUDED
#define OSS_JSPLUGIN_H_INCLUDED

#include <v8.h>
#include <vector>
#include <Poco/ClassLoader.h>
#include <Poco/Manifest.h>
#include "OSS/JS/ObjectWrap.h"

class JSPlugin
{
public:
  JSPlugin();
  virtual ~JSPlugin();
  virtual std::string name() const = 0;
  virtual bool initExportFunc(std::string& funcName) = 0;
  void setContext(v8::Persistent<v8::Context>* pContext);
  v8::Persistent<v8::Context>* getContext();
  void setGlobal(v8::Persistent<v8::ObjectTemplate>* pGlobal);
  v8::Persistent<v8::ObjectTemplate>* getGlobal();

  static v8::Persistent<v8::Context>* _pContext;
  static v8::Persistent<v8::ObjectTemplate>* _pGlobal;
};

inline JSPlugin::JSPlugin()
{
}

inline JSPlugin::~JSPlugin()
{
}

typedef Poco::ClassLoader<JSPlugin> JSPluginLoader;
typedef Poco::Manifest<JSPlugin> JSPluginManifest;

#define JS_REGISTER_MODULE(Class) \
class Class : public JSPlugin \
{ \
public: \
  Class(); \
  virtual std::string name() const; \
  virtual bool initExportFunc(std::string& funcName); \
}; \
Class::Class() {} \
std::string Class::name() const { return #Class; } \
bool Class::initExportFunc(std::string& funcName) \
{ \
  funcName = "__" #Class "_init_exports"; \
  v8::Context::GetCurrent()->Global()->Set(v8::String::New(funcName.c_str()), v8::FunctionTemplate::New(init_exports)->GetFunction()); \
  return true; \
} \
extern "C" { \
  bool pocoBuildManifest(Poco::ManifestBase* pManifest_) { \
    typedef Poco::Manifest<JSPlugin> _Manifest; \
    std::string requiredType(typeid(_Manifest).name()); \
    std::string actualType(pManifest_->className()); \
    if (requiredType == actualType) { \
      Poco::Manifest<JSPlugin>* pManifest = static_cast<_Manifest*>(pManifest_); \
      pManifest->insert(new Poco::MetaObject<Class, JSPlugin>("oss_module")); \
      return true; \
    } \
    else { \
      return false; \
    } \
  } \
}
typedef std::vector<unsigned char> ByteArray;
inline bool js_byte_array_to_int_array(ByteArray& input, v8::Handle<v8::Array>& output)
{
  uint32_t i = 0;
  for (ByteArray::iterator iter = input.begin(); iter != input.end(); iter++)
  {
    output->Set(i++, v8::Int32::New(*iter));
  }
  return output->Length() > 0;
}

inline  bool js_int_array_to_byte_array(v8::Handle<v8::Array>& input, ByteArray& output)
{
  output.clear();
  output.reserve(input->Length());
  for(uint32_t i = 0; i < input->Length(); i++)
  {
    uint32_t val = input->Get(i)->ToInt32()->Value();
    if (val >= 256)
    {
      return false;
    }
    output.push_back(val);
  }
  return !output.empty();
}

inline bool js_string_to_byte_array(std::string& input, ByteArray& output)
{
  output.clear();
  output.reserve(input.size());
  for(uint32_t i = 0; i < input.size(); i++)
  {
    output.push_back((uint32_t)input.at(i));
  }
  return !output.empty();
}

#define CONST_EXPORT(Name) exports->Set(v8::String::New(#Name), v8::Integer::New(Name), v8::ReadOnly);
#define TYPE_ERROR_EXCEPTION(Msg) v8::Exception::TypeError(v8::String::NewSymbol(Msg));


//
// Class and method implementation macros
//
#define JS_CONSTRUCTOR_DECLARE() \
  static JSPersistentFunctionHandle _constructor; \
  static void Init(JSObjectHandle exports); \
  static v8::Handle<v8::Value> New(const v8::Arguments& _args_);

#define JS_METHOD_DECLARE(Method) static v8::Handle<v8::Value> Method(const v8::Arguments& _args_)
#define JS_INDEX_GETTER_DECLARE(Method) static v8::Handle<v8::Value> Method(uint32_t index, const v8::AccessorInfo& _args_);
#define JS_INDEX_SETTER_DECLARE(Method) static v8::Handle<v8::Value> Method(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& _args_);

#define js_export_method(Name, Func) exports->Set(v8::String::NewSymbol(Name), v8::FunctionTemplate::New(Func)->GetFunction())
#define js_export_global_constructor(Name, Func) (*JSPlugin::_pContext)->Global()->Set(v8::String::NewSymbol(Name), Func)
#define js_export_const CONST_EXPORT
#define js_export_string(Name, Value) exports->Set(v8::String::NewSymbol(Name), v8::String::NewSymbol(Value), v8::ReadOnly);

#define JS_CLASS_INTERFACE(Class, Name) \
  JSPersistentFunctionHandle Class::_constructor; \
  void Class::Init(v8::Handle<v8::Object> exports) {\
  js_enter_scope(); \
  std::string className = Name; \
  v8::Local<v8::FunctionTemplate> tpl = OSS::JS::ObjectWrap::ExportConstructorTemplate<Class>(Name, exports);

#define JS_CLASS_METHOD_DEFINE(Class, Name, Func) OSS::JS::ObjectWrap::ExportMethod<Class>(tpl, Name, Func)

#define JS_CLASS_INDEX_ACCESSOR_DEFINE(Class, Getter, Setter) OSS::JS::ObjectWrap::ExportIndexHandler<Class>(tpl, Getter, Setter);
#define JS_CLASS_INTERFACE_END(Class) } OSS::JS::ObjectWrap::FinalizeConstructorTemplate<Class>(className.c_str(), tpl, exports);
#define js_export_class(Class) Class::Init(exports)

#define JS_METHOD_IMPL(Method) v8::Handle<v8::Value>  Method(const v8::Arguments& _args_)\

#define JS_CONSTRUCTOR_IMPL(Class) JS_METHOD_IMPL(Class::New)

#define JS_INDEX_GETTER_IMPL(Method) JSValueHandle Method(uint32_t index, const v8::AccessorInfo& _args_)

#define JS_INDEX_SETTER_IMPL(Method)  JSValueHandle Method(uint32_t index,v8::Local<v8::Value> value, const v8::AccessorInfo& _args_)

#define JS_EXPORTS_INIT() static v8::Handle<v8::Value> init_exports(const v8::Arguments& _args_) { js_enter_scope(); \
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());

#define js_export_finalize() } return exports;

//
// Value Definitions
//
#define JSUndefined() v8::Undefined()
#define JSValueHandle v8::Handle<v8::Value>
#define JSLocalValueHandle v8::Local<v8::Value>
#define JSStringHandle v8::Handle<v8::String>
#define JSArrayHandle v8::Handle<v8::Array>
#define JSObjectHandle v8::Handle<v8::Object>
#define JSLocalObjectHandle v8::Local<v8::Object>
#define JSPersistentObjectHandle v8::Persistent<v8::Object>
#define JSLiteral(Text) v8::String::NewSymbol(Text)
#define JSArguments const v8::Arguments
inline JSStringHandle JSString(const char* str) { return v8::String::New(str); }
inline JSStringHandle JSString(const char* str, std::size_t len) { return v8::String::New(str, len); }
#define JSBoolean(Exp) v8::Boolean::New(Exp)
#define JSArray(Size) v8::Array::New(Size)
#define JSInt32(Value) v8::Int32::New(Value)
#define JSUInt32(Value) v8::Uint32::New(Value)
#define JSInteger(Value) v8::Integer::New(Value)
#define JSObject() v8::Object::New();
#define JSException(What) v8::ThrowException(v8::Exception::Error(JSLiteral(What)))

//
// Function Value
//
#define JSPersistentFunctionHandle v8::Persistent<v8::Function>
#define JSFunctionHandle v8::Handle<v8::Function>
#define JSLocalFunctionHandle v8::Local<v8::Function>
#define JSPersistentFunctionCast(Handle) JSPersistentFunctionHandle::New(JSFunctionHandle::Cast(Handle))

//
// Helper functions
//
#define js_throw(What) return JSException(What)
#define js_assert(Expression, What) if (!Expression) { js_throw(What); }
#define js_is_function(Handle) Handle->IsFunction()
#define js_get_global() (*JSPlugin::_pContext)->Global()
#define js_get_global_method(Name) js_get_global()->Get(JSLiteral(Name))
#define js_enter_scope() v8::HandleScope _scope_
#define js_unwrap_object(Class, Object) OSS::JS::ObjectWrap::Unwrap<Class>(Object)


//
// Argument parsing
//
#define js_method_arg_self() _args_.This()
#define js_method_get_arg_length() _args_.Length()
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


#define js_method_arg_has_property(Index, Name) _args_[Index]->ToObject()->Has(v8::String::NewSymbol(Name))
#define js_method_arg_get_property(Index, Name) _args_[Index]->ToObject()->Get(v8::String::NewSymbol(Name))

#define js_method_arg_unwrap_object(Class, Index) js_unwrap_object(Class, js_method_arg_as_object(Index))
#define js_method_arg_unwrap_self(Class) js_unwrap_object(Class, js_method_arg_self())

#define js_setter_info_unwrap_self js_method_arg_unwrap_self
#define js_getter_info_unwrap_self js_method_arg_unwrap_self
#define js_setter_value_as_uint32() value->ToUint32()->Value()
#define js_setter_index() index
#define js_getter_index js_setter_index


#endif // OSS_JSPLUGIN_H_INCLUDED

