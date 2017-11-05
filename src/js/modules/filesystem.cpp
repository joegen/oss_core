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
#include <boost/filesystem.hpp>

static v8::Handle<v8::Value> __exists(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if (args.Length() == 0 || !args[0]->IsString())
  {
    return v8::Exception::TypeError(v8::String::NewSymbol("Invalid Argument"));
  }
  v8::String::Utf8Value pathStr(args[0]);
  boost::filesystem::path file(*pathStr);
  return v8::Boolean::New(boost::filesystem::exists(file));
}

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  exports->Set(v8::String::New("exists"), v8::FunctionTemplate::New(__exists)->GetFunction());
  return exports;
}

JS_REGISTER_MODULE(Fork);
