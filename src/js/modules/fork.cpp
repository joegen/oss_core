#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static v8::Handle<v8::Value> __fork(const v8::Arguments& args)
{
  pid_t pid = ::fork();
  return v8::Int32::New(pid);
}

static v8::Handle<v8::Value> __wait(const v8::Arguments& args)
{
  int status = 0;
  pid_t pid = ::wait(&status);
  
  v8::Handle<v8::Object> result = v8::Object::New();
  result->Set(v8::String::New("pid"), v8::Int32::New(pid));
  result->Set(v8::String::New("status"), v8::Int32::New(status));
  return result;
}

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  exports->Set(v8::String::New("fork"), v8::FunctionTemplate::New(__fork)->GetFunction());
  exports->Set(v8::String::New("wait"), v8::FunctionTemplate::New(__wait)->GetFunction()); 
  return exports;
}

JS_REGISTER_MODULE(Fork);
