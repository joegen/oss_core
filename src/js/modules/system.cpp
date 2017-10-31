#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include <unistd.h>
#include "OSS/JS/modules/BufferObject.h"


using OSS::JS::ObjectWrap;
typedef BufferObject::ByteArray ByteArray;


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
  if (args.Length() < 2 || !args[0]->IsInt32() || (!args[1]->IsArray() && !args[1]->IsString() && !args[1]->IsObject()))
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
  else if (args[1]->IsObject())
  {
    v8::HandleScope scope;
    if (!BufferObject::isBuffer(args[1]))
    {
      return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Argument Requires Buffer")));
    }
    BufferObject* pBuffer = ObjectWrap::Unwrap<BufferObject>(args[1]->ToObject());
    return v8::Int32::New(::write(fd, pBuffer->buffer().data(), pBuffer->buffer().size()));
  }
  return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
}

static v8::Handle<v8::Value> __read(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 2 || !args[0]->IsInt32() || !args[1]->IsInt32() )
  {
    return v8::Undefined();
  }
  
  int fd = args[0]->ToInt32()->Value();
  
  //
  // Create a new Buffer
  //
  v8::Handle<v8::Value> result = BufferObject::createNew(args[1]->ToInt32()->Value());
  BufferObject* pBuffer = ObjectWrap::Unwrap<BufferObject>(result->ToObject());

  ByteArray& buf = pBuffer->buffer();
  std::size_t ret = ::read(fd, buf.data(), buf.size());
  if (!ret)
  {
    OSS_LOG_INFO("__read returning undefined");
    return v8::Undefined();
  }
  OSS_LOG_INFO("__read returning buffer " << buf.size());
  return result;
}

static v8::Handle<v8::Value> __sleep(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsUint32())
  {
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Invalid Argument")));
  }
  ::sleep(args[0]->ToUint32()->Value());
  return v8::Undefined();
}
static v8::Handle<v8::Value> __gc(const v8::Arguments& args)
{
  v8::V8::LowMemoryNotification();
  while(!v8::V8::IdleNotification());
  return v8::Undefined();
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
  exports->Set(v8::String::New("sleep"), v8::FunctionTemplate::New(__sleep)->GetFunction());
  exports->Set(v8::String::New("gc"), v8::FunctionTemplate::New(__gc)->GetFunction());
  return exports;
}

JS_REGISTER_MODULE(System);
