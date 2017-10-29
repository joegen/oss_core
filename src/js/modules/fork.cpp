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
