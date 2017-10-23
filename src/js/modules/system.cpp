#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include <unistd.h>

typedef std::vector<unsigned char> ByteArray;

static bool int_array_to_byte_array(v8::Handle<v8::Array>& input, ByteArray& output)
{
  v8::HandleScope scope;
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

static bool byte_array_to_int_array(ByteArray& input, v8::Handle<v8::Array>& output)
{
  v8::HandleScope scope;
  uint32_t i = 0;
  for (ByteArray::iterator iter = input.begin(); iter != input.end(); iter++)
  {
    output->Set(i++, v8::Int32::New(*iter));
  }
  return output->Length() > 0;
}

static v8::Handle<v8::Value>  __close(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsInt32())
  {
    return v8::Int32::New(-1);
  }
  int fd = args[0]->ToInt32()->Value();
  return v8::Int32::New(::close(fd));
}

static v8::Handle<v8::Value>  __exit(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsInt32())
  {
    return v8::Int32::New(-1);
  }
  ::exit(args[0]->ToInt32()->Value());
}

static v8::Handle<v8::Value>  ___exit(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsInt32())
  {
    return v8::Int32::New(-1);
  }
  ::_exit(args[0]->ToInt32()->Value());
}

static v8::Handle<v8::Value> __write(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 2 || !args[0]->IsInt32() || (!args[1]->IsArray() && !args[1]->IsString()))
  {
    return v8::Int32::New(-1);
  }
  
  int fd = args[0]->ToInt32()->Value();
  
  if (args[1]->IsString())
  {
    v8::String::Utf8Value arg1(args[1]);
    char* data = *arg1;
    std::size_t len = strlen(data) + 1;
    return v8::Int32::New(::write(fd, data, len));
  }
  else if (args[1]->IsArray())
  {
    v8::Handle<v8::Array> args1 = v8::Handle<v8::Array>::Cast(args[1]);
    ByteArray bytes;
    if (!int_array_to_byte_array(args1, bytes))
    {
      return v8::Int32::New(-1);
    }
    return v8::Int32::New(::write(fd, bytes.data(), bytes.size()));
  }
  return v8::Int32::New(-1);
}

static v8::Handle<v8::Value> __read(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 2 || !args[0]->IsInt32() || !args[1]->IsInt32() )
  {
    return v8::Undefined();
  }
  
  int fd = args[0]->ToInt32()->Value();
  ByteArray buf(args[1]->ToInt32()->Value());
  std::size_t ret = ::read(fd, buf.data(), buf.size());
  if (!ret)
  {
    return v8::Undefined();
  }
  v8::Handle<v8::Array> output = v8::Array::New(ret);
  if (!byte_array_to_int_array(buf, output))
  {
    return v8::Undefined();
  }
  return output;
}

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  exports->Set(v8::String::New("read"), v8::FunctionTemplate::New(__read)->GetFunction());
  exports->Set(v8::String::New("write"), v8::FunctionTemplate::New(__write)->GetFunction());
  exports->Set(v8::String::New("close"), v8::FunctionTemplate::New(__close)->GetFunction());
  exports->Set(v8::String::New("exit"), v8::FunctionTemplate::New(__exit)->GetFunction()); 
  exports->Set(v8::String::New("_exit"), v8::FunctionTemplate::New(___exit)->GetFunction());
  
  return exports;
}

JS_REGISTER_MODULE(System);
