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
