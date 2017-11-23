
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
#define MAX_TIMEOUT_SEC 1
typedef OSS::BlockingQueue<DNSResolver*> ContextPool;
typedef std::map<int, DNSContext*> ContextMap;
static ContextPool _pool;
static ContextMap _map;
JSPersistentFunctionHandle* _work_cb = 0;
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

static void relinquish_context(DNSResolver* resolver)
{
  assert(resolver);
  Async::unmonitor_fd(resolver->context()->getSocketFd());
  _pool.enqueue(resolver);
}

template <typename T>
void call_result_callback(const T& record, const JSLocalValueHandle& result, void* userData)
{
  JSPersistentFunctionHandle* cb = (JSPersistentFunctionHandle*)userData;
  
  JSLocalValueHandle common = JSObject();
  common->ToObject()->Set(JSLiteral("cname"), JSString(record.getCName().c_str()));
  common->ToObject()->Set(JSLiteral("qname"), JSString(record.getQName().c_str()));
  common->ToObject()->Set(JSLiteral("ttl"), JSUInt32(record.getTTL()));
  
  JSLocalArgumentVector args;
  args.push_back(result);
  args.push_back(common);
  
  JSValueHandle obj = (*cb)->Get(JSLiteral("resolver"));
  DNSResolver* resolver = js_unwrap_pointer_from_local_object<DNSResolver>(obj->ToObject());
  relinquish_context(resolver);
  
  if (obj->ToObject()->HasOwnProperty(JSLiteral("timerId")))
  {
    JSValueHandle timerId = obj->ToObject()->Get(JSLiteral("timerId"));
    Async::clear_timer(timerId->ToInt32()->Value());
  }
  
  (*cb)->Call((*JSPlugin::_pContext)->Global(), args.size(), args.data());
  cb->Dispose();
  delete cb;
  
  (*_work_cb)->Call((*JSPlugin::_pContext)->Global(), 0, 0);
}

static void on_a_lookup(const DNSARecordV4& record, void* userData)
{
  const DNSAddressList& records = record.getRecords();
  JSLocalValueHandle result = JSArray(records.size());
  std::size_t index = 0;
  for (DNSAddressList::const_iterator iter = records.begin(); iter != records.end(); iter++)
  {
    result->ToObject()->Set(index++, JSString(*iter));
  }
  call_result_callback<DNSARecordV4>(record, result, userData);
}

static void on_aaaa_lookup(const DNSARecordV6& record, void* userData)
{
  const DNSAddressList& records = record.getRecords();
  JSLocalValueHandle result = JSArray(records.size());
  std::size_t index = 0;
  for (DNSAddressList::const_iterator iter = records.begin(); iter != records.end(); iter++)
  {
    result->ToObject()->Set(index++, JSString(*iter));
  }
  call_result_callback<DNSARecordV6>(record, result, userData);
}

static void on_srv_lookup(const DNSSRVRecord& record, void* userData)
{
  const DNSSRVRecordList& records = record.getRecords();
  JSLocalValueHandle result = JSArray(records.size());
  std::size_t index = 0;
  for (DNSSRVRecordList::const_iterator iter = records.begin(); iter != records.end(); iter++)
  {
    JSObjectHandle srv = JSObject();
    srv->Set(JSLiteral("priority"), JSInt32(iter->priority));
    srv->Set(JSLiteral("weight"), JSInt32(iter->weight));
    srv->Set(JSLiteral("port"), JSInt32(iter->port));
    srv->Set(JSLiteral("name"), JSString(iter->name));
    result->ToObject()->Set(index++, srv);
  }
  call_result_callback<DNSSRVRecord>(record, result, userData);
}

static void on_ptr_lookup(const DNSPTRRecord& record, void* userData)
{
  const DNSPTRRecordList& records = record.getRecords();
  JSLocalValueHandle result = JSArray(records.size());
  std::size_t index = 0;
  for (DNSPTRRecordList::const_iterator iter = records.begin(); iter != records.end(); iter++)
  {
    result->ToObject()->Set(index++, JSString(*iter));
  }
  call_result_callback<DNSPTRRecord>(record, result, userData);
}

static void on_txt_lookup(const DNSTXTRecord& record, void* userData)
{
  const DNSTXTRecordList& records = record.getRecords();
  JSLocalValueHandle result = JSArray(records.size());
  std::size_t index = 0;
  for (DNSTXTRecordList::const_iterator iter = records.begin(); iter != records.end(); iter++)
  {
    result->ToObject()->Set(index++, JSString(*iter));
  }
  call_result_callback<DNSTXTRecord>(record, result, userData);
}

static void on_naptr_lookup(const DNSNAPTRRecord& record, void* userData)
{
  const DNSNAPTRRecordList& records = record.getRecords();
  JSLocalValueHandle result = JSArray(records.size());
  std::size_t index = 0;
  for (DNSNAPTRRecordList::const_iterator iter = records.begin(); iter != records.end(); iter++)
  {
    JSObjectHandle srv = JSObject();
    srv->Set(JSLiteral("order"), JSInt32(iter->order));
    srv->Set(JSLiteral("preference"), JSInt32(iter->preference));
    srv->Set(JSLiteral("flags"), JSString(iter->flags));
    srv->Set(JSLiteral("service"), JSString(iter->service));
    srv->Set(JSLiteral("regexp"), JSString(iter->regexp));
    srv->Set(JSLiteral("replacement"), JSString(iter->flags));
    result->ToObject()->Set(index++, srv);
  }
  call_result_callback<DNSNAPTRRecord>(record, result, userData);
}

static void on_mx_lookup(const DNSMXRecord& record, void* userData)
{
  const DNSMXRecordList& records = record.getRecords();
  JSLocalValueHandle result = JSArray(records.size());
  std::size_t index = 0;
  for (DNSMXRecordList::const_iterator iter = records.begin(); iter != records.end(); iter++)
  {
    JSObjectHandle srv = JSObject();
    srv->Set(JSLiteral("priority"), JSInt32(iter->priority));
    srv->Set(JSLiteral("name"), JSString(iter->name));
    result->ToObject()->Set(index++, srv);
  }
  call_result_callback<DNSMXRecord>(record, result, userData);
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
  return JSInt32(dns_timeouts(resolver->context()->context(),MAX_TIMEOUT_SEC, 0));
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
  (*cb)->Set(JSLiteral("resolver"), obj);
  
  resolver->resolveA6(query, 0, resolver_cb, cb);
  return JSInt32(dns_timeouts(resolver->context()->context(),MAX_TIMEOUT_SEC, 0));
}

JS_METHOD_IMPL(__lookup_srv)
{
   static DNSSRVRecordCB resolver_cb = boost::bind(on_srv_lookup, _1, _2);
  
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
  
  resolver->resolveSRV(query, 0, resolver_cb, cb);
  return JSInt32(dns_timeouts(resolver->context()->context(),MAX_TIMEOUT_SEC, 0));
}

JS_METHOD_IMPL(__lookup_ptr4)
{
  static DNSPTRRecordCB resolver_cb = boost::bind(on_ptr_lookup, _1, _2);
  
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
  
  resolver->resolvePTR4(query, resolver_cb, cb);
  return JSInt32(dns_timeouts(resolver->context()->context(),MAX_TIMEOUT_SEC, 0));
}

JS_METHOD_IMPL(__lookup_ptr6)
{
  static DNSPTRRecordCB resolver_cb = boost::bind(on_ptr_lookup, _1, _2);
  
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
  
  resolver->resolvePTR6(query, resolver_cb, cb);
  return JSInt32(dns_timeouts(resolver->context()->context(),MAX_TIMEOUT_SEC, 0));
}

JS_METHOD_IMPL(__lookup_naptr)
{
  static DNSNAPTRRecordCB resolver_cb = boost::bind(on_naptr_lookup, _1, _2);
  
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
  
  resolver->resolveNAPTR(query, 0, resolver_cb, cb);
  return JSInt32(dns_timeouts(resolver->context()->context(),MAX_TIMEOUT_SEC, 0));
}

JS_METHOD_IMPL(__lookup_txt)
{
  static DNSTXTRecordCB resolver_cb = boost::bind(on_txt_lookup, _1, _2);
  
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
  
  resolver->resolveTXT(query, 0, DNS_C_ANY, resolver_cb, cb);
  return JSInt32(dns_timeouts(resolver->context()->context(),MAX_TIMEOUT_SEC, 0));
}

JS_METHOD_IMPL(__lookup_mx)
{
  static DNSMXRecordCB resolver_cb = boost::bind(on_mx_lookup, _1, _2);
  
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
  
  resolver->resolveMX(query, 0, resolver_cb, cb);
  return JSInt32(dns_timeouts(resolver->context()->context(),MAX_TIMEOUT_SEC, 0));
}

JS_METHOD_IMPL(__relinquish_context)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_object(0);
  JSLocalObjectHandle obj = js_method_arg_as_object(0);
  DNSResolver* resolver = js_unwrap_pointer_from_local_object<DNSResolver>(obj);
  relinquish_context(resolver);
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
  _work_cb = new JSPersistentFunctionHandle; 
  *_work_cb = js_method_arg_as_persistent_function(0);
  return JSUndefined();
}

JS_METHOD_IMPL(__get_next_context_timeout)
{
  js_enter_scope();
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_object(0);
  JSLocalObjectHandle obj = js_method_arg_as_object(0);
  DNSResolver* resolver = js_unwrap_pointer_from_local_object<DNSResolver>(obj);
  if (!resolver)
  {
    return JSInt32(-1);
  }
  return JSInt32(dns_timeouts(resolver->context()->context(), MAX_TIMEOUT_SEC, 0));
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
  js_export_method("_get_next_context_timeout", __get_next_context_timeout);
  js_export_method("_lookup_a", __lookup_a);
  js_export_method("_lookup_aaaa", __lookup_aaaa);
  js_export_method("_lookup_srv", __lookup_srv);
  js_export_method("_lookup_txt", __lookup_txt);
  js_export_method("_lookup_mx", __lookup_mx);
  js_export_method("_lookup_ptr", __lookup_ptr4);
  js_export_method("_lookup_ptr4", __lookup_ptr4);
  js_export_method("_lookup_ptr6", __lookup_ptr6);
  js_export_method("_lookup_naptr", __lookup_naptr);
  js_export_finalize();
}

JS_REGISTER_MODULE(JSDNS);
