#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/CoreUtils.h"


static v8::Handle<v8::Value> __log(const v8::Arguments& args)
{
  if (args.Length() < 2 || !args[0]->IsInt32() || !args[1]->IsString())
  {
    return v8::Undefined();
  }
  v8::HandleScope scope;
  v8::String::Utf8Value arg1(args[1]);
  OSS::log(*arg1, (OSS::LogPriority)args[0]->ToInt32()->Value());
  return v8::Undefined();
}

static v8::Handle<v8::Value> __log_init(const v8::Arguments& args)
{
  if (args.Length() < 1 && !args[0]->IsString())
  {
    return v8::Undefined();
  }
  
  std::string path;
  OSS::LogPriority logLevel = OSS::PRIO_INFORMATION;
  std::string format = "%h-%M-%S.%i: %t";
  bool compress = true;
  int purgeCount = 7;
  
  v8::HandleScope scope;
  
  path = *v8::String::Utf8Value(args[0]);
  if (args.Length() >= 2 && args[1]->IsInt32())
  {
    logLevel = (OSS::LogPriority)args[1]->Int32Value();
  }
  
  if (args.Length() >= 3 && args[2]->IsString())
  {
    format = *v8::String::Utf8Value(args[2]);
  }
  
  if (args.Length() >= 4 && args[3]->IsBoolean())
  {
    compress = args[3]->BooleanValue();
  }
  
  if (args.Length() >= 5 && args[4]->IsInt32())
  {
    purgeCount = args[4]->Int32Value();
  }
  
  OSS::logger_init(path, logLevel, format, compress ? "true" : "false", OSS::string_from_number<int>(purgeCount));

  return v8::Undefined();
}

static v8::Handle<v8::Value> __log_level_get(v8::Local<v8::String> property, const v8::AccessorInfo& info) 
{
  return v8::Integer::New(OSS::log_get_level());
}

void __log_level_set(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
  OSS::log_reset_level((OSS::LogPriority)value->Int32Value());
}

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::HandleScope scope; 
  v8::Local<v8::Object> exports = v8::Object::New();
  
  //
  // Methods
  //
  exports->Set(v8::String::New("log"), v8::FunctionTemplate::New(__log)->GetFunction());
  exports->Set(v8::String::New("init"), v8::FunctionTemplate::New(__log_init)->GetFunction());
  
  //
  // Mutable Properties
  //
  exports->SetAccessor(v8::String::New("level"), __log_level_get, __log_level_set);

  //
  // Constants
  //
  exports->Set(v8::String::New("NOTICE"), v8::Int32::New(OSS::PRIO_NOTICE), v8::ReadOnly);
  exports->Set(v8::String::New("INFO"), v8::Int32::New(OSS::PRIO_INFORMATION), v8::ReadOnly);
  exports->Set(v8::String::New("DEBUG"), v8::Int32::New(OSS::PRIO_DEBUG), v8::ReadOnly);
  exports->Set(v8::String::New("TRACE"), v8::Int32::New(OSS::PRIO_TRACE), v8::ReadOnly);
  exports->Set(v8::String::New("WARNING"), v8::Int32::New(OSS::PRIO_WARNING), v8::ReadOnly);
  exports->Set(v8::String::New("ERROR"), v8::Int32::New(OSS::PRIO_ERROR), v8::ReadOnly);
  exports->Set(v8::String::New("CRITICAL"), v8::Int32::New(OSS::PRIO_CRITICAL), v8::ReadOnly);
  exports->Set(v8::String::New("FATAL"), v8::Int32::New(OSS::PRIO_FATAL), v8::ReadOnly);
  
  return exports;
}

JS_REGISTER_MODULE(Logger, "logger");


