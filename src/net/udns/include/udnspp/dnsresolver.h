// Library: libudnspp
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

#ifndef UDNSPP_DNSRESOLVER_INCLUDED
#define UDNSPP_DNSRESOLVER_INCLUDED


#include <udnspp/dnscontext.h>
#include <udnspp/dnsarecord.h>
#include <udnspp/dnssrvrecord.h>
#include <udnspp/dnsnaptrrecord.h>
#include <udnspp/dnsptrrecord.h>
#include <udnspp/dnsmxrecord.h>
#include <udnspp/dnstxtrecord.h>
#include <udnspp/dnscache.h>


#ifdef HAVE_BOOSTLIBS
#define ENABLE_ASYNC_RESOLVE
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#endif

namespace udnspp {

#ifdef ENABLE_ASYNC_RESOLVE

typedef boost::function<void(const DNSARecordV4&, void*)> DNSARecordV4CB;
typedef boost::function<void(const DNSARecordV6&, void*)> DNSARecordV6CB;
typedef boost::function<void(const DNSSRVRecord&, void*)> DNSSRVRecordCB;
typedef boost::function<void(const DNSPTRRecord&, void*)> DNSPTRRecordCB;
typedef boost::function<void(const DNSTXTRecord&, void*)> DNSTXTRecordCB;
typedef boost::function<void(const DNSNAPTRRecord&, void*)> DNSNAPTRRecordCB;
typedef boost::function<void(const DNSMXRecord&, void*)> DNSMXRecordCB;

#endif

#ifdef ENABLE_LRU_CACHE
typedef DNSCache<DNSARecordV4> DNSARecordV4Cache;
typedef DNSCache<DNSARecordV6> DNSARecordV6Cache;
typedef DNSCache<DNSSRVRecord> DNSSRVRecordCache;
typedef DNSCache<DNSPTRRecord> DNSPTRRecordCache;
typedef DNSCache<DNSTXTRecord> DNSTXTRecordCache;
typedef DNSCache<DNSNAPTRRecord> DNSNAPTRRecordCache;
typedef DNSCache<DNSMXRecord> DNSMXRecordCache;
#endif


class DNSResolver
{
public:
#ifdef ENABLE_ASYNC_RESOLVE
  typedef boost::mutex mutex;
  typedef boost::lock_guard<mutex> mutex_lock;
#endif

  DNSResolver();

  DNSResolver(DNSContext* pContext);

  ~DNSResolver();

  DNSARecord resolveA4(const std::string& name, int flags) const;

  DNSARecord resolveA6(const std::string& name, int flags) const;

  DNSSRVRecord resolveSRV(const std::string& name, int flags) const;

  DNSNAPTRRecord resolveNAPTR(const std::string& name, int flags) const;

  DNSPTRRecord resolvePTR4(const std::string& ip4address) const;

  DNSPTRRecord resolvePTR6(const std::string& ip6address) const;

  DNSMXRecord resolveMX(const std::string& name, int flags) const;

  DNSTXTRecord resolveTXT(const std::string& name, int qcls, int flags) const;

#ifdef ENABLE_ASYNC_RESOLVE

  void resolveA4(const std::string& name, int flags, DNSARecordV4CB cb, void* userData) const;

  void resolveA6(const std::string& name, int flags, DNSARecordV6CB cb, void* userData) const;

  void resolveSRV(const std::string& name, int flags, DNSSRVRecordCB cb, void* userData) const;

  void resolveNAPTR(const std::string& name, int flags, DNSNAPTRRecordCB cb, void* userData) const;

  void resolvePTR4(const std::string& ip4address, DNSPTRRecordCB cb, void* userData) const;

  void resolvePTR6(const std::string& ip6address, DNSPTRRecordCB cb, void* userData) const;

  void resolveMX(const std::string& name, int flags, DNSMXRecordCB cb, void* userData) const;

  void resolveTXT(const std::string& name, int qcls, int flags, DNSTXTRecordCB cb, void* userData) const;

  void start();

  void stop();

  void processEvents();

private:
  bool _stopProcessingEvents;
  boost::thread* _pThread;
  mutable mutex _eventMutex;


#endif  // ENABLE_ASYNC_RESOLVE

private:
  DNSContext* _pContext;
  bool _canDeleteContext;

#ifdef ENABLE_LRU_CACHE
public:
  void enableLRUCache(bool enabled);
  bool isLRUCacheEnabled() const;

  DNSARecordV4Cache& v4Cache();
  DNSARecordV6Cache& v6Cache();
  DNSSRVRecordCache& srvCache();
  DNSPTRRecordCache& ptrCache();
  DNSTXTRecordCache& txtCache();
  DNSNAPTRRecordCache& naPtrCache();
  DNSMXRecordCache& mxCache();

private:
  bool _enableLRUCache;
  mutable DNSARecordV4Cache _v4Cache;
  mutable DNSARecordV6Cache _v6Cache;
  mutable DNSSRVRecordCache _srvCache;
  mutable DNSPTRRecordCache _ptrCache;
  mutable DNSTXTRecordCache _txtCache;
  mutable DNSNAPTRRecordCache _naPtrCache;
  mutable DNSMXRecordCache _mxCache;
#endif

};

//
// Inlines
//

#ifdef ENABLE_LRU_CACHE
inline  void DNSResolver::enableLRUCache(bool enabled)
{
  _enableLRUCache = enabled;
}

inline bool DNSResolver::isLRUCacheEnabled() const
{
  return _enableLRUCache;
}

inline DNSARecordV4Cache& DNSResolver::v4Cache()
{
  return _v4Cache;
}

inline DNSARecordV6Cache& DNSResolver::v6Cache()
{
  return _v6Cache;
}

inline DNSSRVRecordCache& DNSResolver::srvCache()
{
  return _srvCache;
}

inline DNSPTRRecordCache& DNSResolver::ptrCache()
{
  return _ptrCache;
}

inline DNSTXTRecordCache& DNSResolver::txtCache()
{
  return _txtCache;
}

inline DNSNAPTRRecordCache& DNSResolver::naPtrCache()
{
  return _naPtrCache;
}

inline DNSMXRecordCache& DNSResolver::mxCache()
{
  return _mxCache;
}

#endif


} // namespace udnspp


#endif  // UDNSPP_DNSRESOLVER_INCLUDED