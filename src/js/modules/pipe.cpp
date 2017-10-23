#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include <unistd.h>
#include <fcntl.h>

static v8::Handle<v8::Value> __pipe(const v8::Arguments& args)
{
  int flags = 0;
  if (args.Length() >= 1 && args[0]->IsInt32())
  {
    flags = args[0]->ToInt32()->Value();
  }
  int ret = 0;
  int pipefd[2];
  if (!flags)
  {
    ret = ::pipe(pipefd);
  }
  else
  {
    ret = ::pipe2(pipefd, flags);
  }
  
  if (ret == 0)
  {
    v8::Handle<v8::Array> result = v8::Array::New(2);
    result->Set(0, v8::Int32::New(pipefd[0]));
    result->Set(1, v8::Int32::New(pipefd[1]));
    return result;
  }
  
  return v8::Undefined();
}

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  exports->Set(v8::String::New("pipe"), v8::FunctionTemplate::New(__pipe)->GetFunction()); 
  return exports;
}

JS_REGISTER_MODULE(Pipe);
