
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
#include "OSS/JS/JSIsolate.h"
#include "OSS/Net/Pinger.h"

struct ping_host_data_t
{
  OSS::JS::JSIsolate::Ptr isolate;
  JSPersistentFunctionHandle* cb;
  std::string host;
  bool timeout; 
  int sequence;
  int bytes;
  int ttl;
  int roundtrip;
};

static void ping_host_callback_isolated(void* user_data)
{
  ping_host_data_t* data = static_cast<ping_host_data_t*>(user_data);
  assert(data);
  
  JSLocalValueHandle result = JSObject();
  result->ToObject()->Set(JSLiteral("host"), JSString(data->host.c_str()));
  result->ToObject()->Set(JSLiteral("timeout"), JSBoolean(data->timeout));
  result->ToObject()->Set(JSLiteral("sequence"), JSInt32(data->sequence));
  result->ToObject()->Set(JSLiteral("bytes"), JSInt32(data->bytes));
  result->ToObject()->Set(JSLiteral("ttl"), JSInt32(data->ttl));
  result->ToObject()->Set(JSLiteral("roundtrip"), JSInt32(data->roundtrip));
  
  JSLocalArgumentVector args;
  args.push_back(result);
 
  (*data->cb)->Call(js_get_global(), args.size(), args.data());
  data->cb->Dispose();
  delete data->cb;
  delete data;
}

static void ping_host_callback(const std::string& host, bool timeout, int sequence, int bytes, int ttl, int roundtrip, void* user_data)
{
  
  ping_host_data_t* data = static_cast<ping_host_data_t*>(user_data);
  assert(data);
  assert(data->cb);
  assert(data->isolate);
  
  data->host = host;
  data->timeout = timeout;
  data->sequence = sequence;
  data->bytes = bytes;
  data->ttl = ttl;
  data->roundtrip = roundtrip;
  data->isolate->doTask(boost::bind(ping_host_callback_isolated, _1), data);
}

JS_METHOD_IMPL(__ping_host)
{
  js_method_arg_assert_size_eq(4);
  js_method_arg_assert_string(0);
  js_method_arg_assert_uint32(1);
  js_method_arg_assert_uint32(2);
  js_method_arg_assert_function(3);
  std::string host = js_method_arg_as_std_string(0);
  OSS::UInt32 sequence = js_method_arg_as_uint32(1);
  OSS::UInt32 ttl = js_method_arg_as_uint32(2);
  
  ping_host_data_t* data = new ping_host_data_t();
  
  data->cb = new JSPersistentFunctionHandle;
  data->isolate = OSS::JS::JSIsolate::getIsolate();
  
  *data->cb = js_method_arg_as_persistent_function(3);
  return JSBoolean(OSS::Net::Pinger::ping_host(host, sequence, ttl, boost::bind(ping_host_callback, _1, _2, _3, _4, _5, _6, _7), (void*)data));
}

JS_EXPORTS_INIT()
{
  js_export_method("_ping_host", __ping_host);
  js_export_finalize();
}

JS_REGISTER_MODULE(JSPINGER);