#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include <unistd.h>
#include <getopt.h>

static int gArgc = 0;
static char** gArgv = 0;

static v8::Handle<v8::Value> __getopt(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() == 0 || !args[0]->IsString())
  {
    return v8::Exception::TypeError(v8::String::NewSymbol("Invalid argument! Usage: getopt(String)"));
  }
  v8::String::Utf8Value optstring(args[0]);
  return v8::Uint32::New(::getopt(gArgc, gArgv, *optstring));
}

static void __init_argv(v8::Persistent<v8::Object>& exports)
{
  v8::HandleScope scope;
  OSS::OSS_argv(&gArgc, &gArgv);
  assert(gArgc > 0);
  exports->Set(v8::String::NewSymbol("argc"), v8::Uint32::New(gArgc));
  v8::Handle<v8::Array> argv = v8::Array::New(gArgc);
  for (int i = 0; i < gArgc; i++)
  {
    argv->Set(i, v8::String::New(gArgv[i]));
  }
  exports->Set(v8::String::NewSymbol("argv"), argv);
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
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  __init_argv(exports);
  exports->Set(v8::String::New("getopt"), v8::FunctionTemplate::New(__getopt)->GetFunction());
  exports->SetAccessor(v8::String::New("opterr"), __opterr_get, __opterr_set);
  exports->SetAccessor(v8::String::New("optind"), __optind_get, __optind_set);
  exports->SetAccessor(v8::String::New("optarg"), __optarg_get, 0);
  exports->SetAccessor(v8::String::New("optopt"), __optopt_get, 0);
  return exports;
}

JS_REGISTER_MODULE(GetOpt);