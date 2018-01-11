#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include <unistd.h>
#include <getopt.h>

static int gArgc = 0;
static char** gArgv = 0;

JS_METHOD_IMPL(__getopt)
{
  js_method_arg_declare_string(optstring, 0);
  return v8::Uint32::New(::getopt(gArgc, gArgv, optstring.c_str()));
}

static v8::Handle<v8::Value> __optind_get(v8::Local<v8::String> property, const v8::AccessorInfo& info) 
{
  return v8::Integer::New(optind);
}

void __optind_set(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
  optind = value->Int32Value();
}

static v8::Handle<v8::Value> __opterr_get(v8::Local<v8::String> property, const v8::AccessorInfo& info) 
{
  return v8::Integer::New(opterr);
}

void __opterr_set(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
  opterr = value->Int32Value();
}

static v8::Handle<v8::Value> __optarg_get(v8::Local<v8::String> property, const v8::AccessorInfo& info) 
{
  return v8::String::New(optarg);
}

static v8::Handle<v8::Value> __optopt_get(v8::Local<v8::String> property, const v8::AccessorInfo& info) 
{
  return v8::Uint32::New(optopt);
}

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::Local<v8::Object> exports = v8::Local<v8::Object>::New(v8::Object::New());
  
  OSS::OSS_argv(&gArgc, &gArgv);
  optind = 2;
  exports->Set(v8::String::New("argc"), v8::Uint32::New(gArgc));
  v8::Local<v8::Array> argv = v8::Array::New(gArgc);
  for (int i = 0; i < gArgc; i++)
  {
    argv->Set(i, v8::String::New(gArgv[i]));
  }
  exports->Set(v8::String::New("argv"), argv);
  exports->Set(v8::String::New("getopt"), v8::FunctionTemplate::New(__getopt)->GetFunction());
  exports->SetAccessor(v8::String::New("opterr"), __opterr_get, __opterr_set);
  exports->SetAccessor(v8::String::New("optind"), __optind_get, __optind_set);
  exports->SetAccessor(v8::String::New("optarg"), __optarg_get, 0);
  exports->SetAccessor(v8::String::New("optopt"), __optopt_get, 0);
  OSS_LOG_INFO("Got HERE");
  return exports;
}

JS_REGISTER_MODULE(GetOpt);