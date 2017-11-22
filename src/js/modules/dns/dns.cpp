
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
#include "OSS/UTL/Thread.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/BlockingQueue.h"
#include "udnspp/dnsresolver.h"
#include "OSS/UTL/Logger.h"
#include "OSS//JS/modules/Async.h"

#include <poll.h>

using namespace udnspp;

#define POOL_SIZE 10
typedef OSS::BlockingQueue<DNSResolver*> ContextPool;
typedef std::map<int, DNSContext*> ContextMap;
static ContextPool _pool;
static ContextMap _map;
static boost::thread* pollThread = 0;
JSPersistentFunctionHandle* work_cb = 0;
pollfd pfds[POOL_SIZE];

static void initialize_pool(uint32_t size)
{
  int index = 0;
  for (uint32_t i = 0; i < size; i++)
  {
    DNSContext* context = new DNSContext();
    context->context(true);
    DNSResolver* resolver = new DNSResolver(context, true);
    _pool.enqueue(resolver);
    pfds[index].fd = context->getSocketFd();
    pfds[index].events = POLLIN;
    ++index;
    _map[context->getSocketFd()] = context;
  }
}


static void on_a_lookup(const DNSARecordV4& record, void* userData)
{
  JSPersistentFunctionHandle* cb = (JSPersistentFunctionHandle*)userData;
  const DNSAddressList& records = record.getRecords();
  JSLocalValueHandle result = JSArray(records.size());
  std::size_t index = 0;
  for (DNSAddressList::const_iterator iter = records.begin(); iter != records.end(); iter++)
  {
    result->ToObject()->Set(index, JSString(*iter));
  }
  
  JSLocalValueHandle common = JSObject();
 
  common->ToObject()->Set(JSLiteral("cname"), JSString(record.getCName().c_str()));
  common->ToObject()->Set(JSLiteral("qname"), JSString(record.getQName().c_str()));
  common->ToObject()->Set(JSLiteral("ttl"), JSUInt32(record.getTTL()));
  
  JSLocalArgumentVector args;
  args.push_back(result);
  args.push_back(common);
  
  JSValueHandle obj = (*cb)->Get(JSLiteral("resolver"));
  DNSResolver* resolver = js_unwrap_pointer_from_local_object<DNSResolver>(obj->ToObject());
  Async::unmonitor_fd(resolver->context()->getSocketFd());
  assert(resolver);
  _pool.enqueue(resolver);
  
  (*cb)->Call((*JSPlugin::_pContext)->Global(), args.size(), args.data());
  cb->Dispose();
  delete cb;
  
  (*work_cb)->Call((*JSPlugin::_pContext)->Global(), 0, 0);
}

static void on_aaaa_lookup(const DNSARecordV6& record, void* userData)
{
}

static void on_srv_lookup(const DNSSRVRecord& record, void* userData)
{
}

static void on_ptr_lookup(const DNSPTRRecord& record, void* userData)
{
}

static void on_txt_lookup(const DNSTXTRecord& record, void* userData)
{
}

static void on_naptr_lookup(const DNSNAPTRRecord& record, void* userData)
{
}

static void on_mx_lookup(const DNSMXRecord& record, void* userData)
{
}

JS_METHOD_IMPL(__lookup_a)
{
  static DNSARecordV4CB resolver_cb = boost::bind(on_a_lookup, _1, _2);
  js_enter_scope();
  js_method_arg_assert_size_eq(3);
  js_method_arg_assert_string(0);
  js_method_arg_assert_function(1);
  js_method_arg_assert_object(2);
  
  std::string query = js_method_arg_as_std_string(0);
  JSPersistentFunctionHandle* cb = new JSPersistentFunctionHandle; 
  *cb = js_method_arg_as_persistent_function(1);
  JSLocalObjectHandle obj = js_method_arg_as_object(2);
  DNSResolver* resolver = js_unwrap_pointer_from_local_object<DNSResolver>(obj);
  (*cb)->Set(JSLiteral("resolver"), obj);
  resolver->resolveA4(query, 0, resolver_cb, cb);
  while(!dns_timeouts(resolver->context()->context(), -1, 0) == -1);
  return JSUndefined();
}

JS_METHOD_IMPL(__lookup_aaaa)
{
  static DNSARecordV6CB resolver_cb = boost::bind(on_aaaa_lookup, _1, _2);
  js_enter_scope();
  js_method_arg_assert_size_eq(3);
  js_method_arg_assert_string(0);
  js_method_arg_assert_function(1);
  js_method_arg_assert_object(2);
  
  std::string query = js_method_arg_as_std_string(0);
  JSPersistentFunctionHandle* cb = new JSPersistentFunctionHandle; 
  *cb = js_method_arg_as_persistent_function(1);
  JSLocalObjectHandle obj = js_method_arg_as_object(2);
  DNSResolver* resolver = js_unwrap_pointer_from_local_object<DNSResolver>(obj);
  
  resolver->resolveA6(query, 0, resolver_cb, cb);
  while(!dns_timeouts(resolver->context()->context(), -1, 0) == -1);
  return JSUndefined();
}

JS_METHOD_IMPL(__lookup_srv)
{
  static DNSARecordV4CB resolver_cb = boost::bind(on_a_lookup, _1, _2);
  js_enter_scope();
  js_method_arg_assert_size_eq(3);
  js_method_arg_assert_string(0);
  js_method_arg_assert_function(1);
  js_method_arg_assert_object(2);
  
  std::string query = js_method_arg_as_std_string(0);
  JSPersistentFunctionHandle* cb = new JSPersistentFunctionHandle; 
  *cb = js_method_arg_as_persistent_function(1);
  JSLocalObjectHandle obj = js_method_arg_as_object(2);
  DNSResolver* resolver = js_unwrap_pointer_from_local_object<DNSResolver>(obj);
  
  resolver->resolveA4(query, 0, resolver_cb, cb);
  dns_timeouts(resolver->context()->context(), -1, 0);
  return JSUndefined();
}

JS_METHOD_IMPL(__relinquish_context)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_object(0);
  JSLocalObjectHandle obj = js_method_arg_as_object(0);
  DNSResolver* resolver = js_unwrap_pointer_from_local_object<DNSResolver>(obj);
  _pool.enqueue(resolver);
  return JSUndefined();
}

JS_METHOD_IMPL(__acquire_context)
{
  js_enter_scope();
  
  DNSResolver* resolver;
  _pool.dequeue(resolver);
  if (!resolver)
  {
    js_throw("Unable to acquire context");
  }
  
  JSObjectHandle obj = js_wrap_pointer_to_local_object(resolver);
  obj->Set(JSLiteral("fd"), JSUInt32(resolver->context()->getSocketFd()));
  
  return obj;
}

JS_METHOD_IMPL(__get_context_count)
{
  js_enter_scope();
  return JSUInt32(_pool.size());
}

JS_METHOD_IMPL(__process_io_events)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_uint32(0);
  uint32_t fd = js_method_arg_as_uint32(0);
  
  ContextMap::iterator iter = _map.find(fd);
  if (iter != _map.end())
  {
    dns_ioevent(iter->second->context(), 0);
  }
  return JSUndefined();
}

JS_METHOD_IMPL(__set_work_callback)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_function(0);
  work_cb = new JSPersistentFunctionHandle; 
  *work_cb = js_method_arg_as_persistent_function(0);
  return JSUndefined();
}

JS_EXPORTS_INIT()
{
  js_enter_scope();
  initialize_pool(POOL_SIZE);
  js_export_method("_acquire_context", __acquire_context);
  js_export_method("_relinquish_context", __relinquish_context);
  js_export_method("_get_context_count", __get_context_count);
  js_export_method("_process_io_events", __process_io_events);
  js_export_method("_set_work_callback", __set_work_callback);
  js_export_method("_lookup_a", __lookup_a);
  js_export_method("_lookup_aaaa", __lookup_aaaa);
  js_export_method("_lookup_srv", __lookup_srv);
  js_export_finalize();
}

JS_REGISTER_MODULE(JSDNS);
