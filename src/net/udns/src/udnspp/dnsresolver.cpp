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


#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <udnspp/dnsresolver.h>

#ifdef WINDOWS
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif


namespace udnspp {

static const std::size_t MAX_CACHE_SIZE = 100;

DNSResolver::DNSResolver() :
  _canDeleteContext(true)
#ifdef ENABLE_LRU_CACHE
  , _enableLRUCache(true)
  , _v4Cache(MAX_CACHE_SIZE)
  , _v6Cache(MAX_CACHE_SIZE)
  , _srvCache(MAX_CACHE_SIZE)
  , _ptrCache(MAX_CACHE_SIZE)
  , _txtCache(MAX_CACHE_SIZE)
  , _naPtrCache(MAX_CACHE_SIZE)
  , _mxCache(MAX_CACHE_SIZE)
#endif
{
#ifdef  ENABLE_ASYNC_RESOLVE
  _stopProcessingEvents = false;
  _pThread = 0;
#endif
  _canDeleteContext = true;
  _pContext = new DNSContext(*DNSContext::defaultContext());
}

DNSResolver::DNSResolver(DNSContext* pContext) :
  _canDeleteContext(false)
  #ifdef ENABLE_LRU_CACHE
  , _enableLRUCache(true)
  , _v4Cache(MAX_CACHE_SIZE)
  , _v6Cache(MAX_CACHE_SIZE)
  , _srvCache(MAX_CACHE_SIZE)
  , _ptrCache(MAX_CACHE_SIZE)
  , _txtCache(MAX_CACHE_SIZE)
  , _naPtrCache(MAX_CACHE_SIZE)
  , _mxCache(MAX_CACHE_SIZE)
#endif
{
#ifdef  ENABLE_ASYNC_RESOLVE
  _stopProcessingEvents = false;
  _pThread = 0;
#endif
  _canDeleteContext = false;
  _pContext = pContext;
}

DNSResolver::~DNSResolver()
{
#ifdef  ENABLE_ASYNC_RESOLVE
  stop();
#endif
  if (_canDeleteContext)
    delete _pContext;
    
}

DNSARecord DNSResolver::resolveA4(const std::string& name, int flags) const
{
#ifdef ENABLE_LRU_CACHE
  DNSARecordV4 cached;
  if (_enableLRUCache && _v4Cache.find(name, cached))
    return cached;
#endif
  DNSContext ctx(*_pContext);
  dns_rr_a4* pRr = dns_resolve_a4(ctx.context(), name.c_str(), flags);
  if (pRr)
  {
    DNSARecordV4 rr(pRr);
    free(pRr);

#ifdef ENABLE_LRU_CACHE
    if (_enableLRUCache)
      _v4Cache.insert(rr);
#endif

    return rr;
  }
  return DNSARecord();
}

DNSARecord DNSResolver::resolveA6(const std::string& name, int flags) const
{
#ifdef ENABLE_LRU_CACHE
  DNSARecordV6 cached;
  if (_enableLRUCache && _v6Cache.find(name, cached))
    return cached;
#endif

  DNSContext ctx(*_pContext);
  dns_rr_a6* pRr = dns_resolve_a6(ctx.context(), name.c_str(), flags);
  if (pRr)
  {
    DNSARecordV6 rr(pRr);

#ifdef ENABLE_LRU_CACHE
    if (_enableLRUCache)
      _v6Cache.insert(rr);
#endif

    free(pRr);
    return rr;
  }
  return DNSARecord();
}

DNSPTRRecord DNSResolver::resolvePTR4(const std::string& address) const
{
#ifdef ENABLE_LRU_CACHE
  DNSPTRRecord cached;
  if (_enableLRUCache && _ptrCache.find(address, cached))
    return cached;
#endif
  DNSContext ctx(*_pContext);
  in_addr ip4;
  dns_pton(AF_INET, address.c_str(), &ip4);
  dns_rr_ptr* pRr = dns_resolve_a4ptr(ctx.context(), &ip4);
  if (pRr)
  {
    DNSPTRRecord rr(pRr);
#ifdef ENABLE_LRU_CACHE
    if (_enableLRUCache)
      _ptrCache.insert(rr);
#endif
    free(pRr);
    return rr;
  }
  return DNSPTRRecord();
}

DNSPTRRecord DNSResolver::resolvePTR6(const std::string& address) const
{
#ifdef ENABLE_LRU_CACHE
  DNSPTRRecord cached;
  if (_enableLRUCache && _ptrCache.find(address, cached))
    return cached;
#endif

  DNSContext ctx(*_pContext);
  in6_addr ip6;
  dns_pton(AF_INET6, address.c_str(), &ip6);
  dns_rr_ptr* pRr = dns_resolve_a6ptr(ctx.context(), &ip6);
  if (pRr)
  {
    DNSPTRRecord rr(pRr);
#ifdef ENABLE_LRU_CACHE
    if (_enableLRUCache)
      _ptrCache.insert(rr);
#endif
    free(pRr);
    return rr;
  }
  return DNSPTRRecord();
}

DNSMXRecord DNSResolver::resolveMX(const std::string& name, int flags) const
{
#ifdef ENABLE_LRU_CACHE
  DNSMXRecord cached;
  if (_enableLRUCache &&_mxCache.find(name, cached))
    return cached;
#endif

  DNSContext ctx(*_pContext);
  dns_rr_mx* pRr = dns_resolve_mx(ctx.context(), name.c_str(), flags);
  if (pRr)
  {
    DNSMXRecord rr(pRr);
#ifdef ENABLE_LRU_CACHE
    if (_enableLRUCache)
      _mxCache.insert(rr);
#endif
    free(pRr);
    return rr;
  }
  return DNSMXRecord();
}

DNSNAPTRRecord DNSResolver::resolveNAPTR(const std::string& name, int flags) const
{
#ifdef ENABLE_LRU_CACHE
  DNSNAPTRRecord cached;
  if (_enableLRUCache && _naPtrCache.find(name, cached))
    return cached;
#endif

  DNSContext ctx(*_pContext);
  dns_rr_naptr* pRr = dns_resolve_naptr(ctx.context(), name.c_str(), flags);
  if (pRr)
  {
    DNSNAPTRRecord rr(pRr);
#ifdef ENABLE_LRU_CACHE
    if (_enableLRUCache)
      _naPtrCache.insert(rr);
#endif
    free(pRr);
    return rr;
  }
  return DNSNAPTRRecord();
}

DNSTXTRecord DNSResolver::resolveTXT(const std::string& name, int qcls, int flags) const
{
#ifdef ENABLE_LRU_CACHE
  DNSTXTRecord cached;
  if (_enableLRUCache && _txtCache.find(name, cached))
    return cached;
#endif

  DNSContext ctx(*_pContext);
  dns_rr_txt* pRr = dns_resolve_txt(ctx.context(), name.c_str(), qcls, flags);
  if (pRr)
  {
    DNSTXTRecord rr(pRr);
#ifdef ENABLE_LRU_CACHE
    if (_enableLRUCache)
      _txtCache.insert(rr);
#endif
    free(pRr);
    return rr;
  }
  return DNSTXTRecord();
}


DNSSRVRecord DNSResolver::resolveSRV(const std::string& name, int flags) const
{
#ifdef ENABLE_LRU_CACHE
  DNSSRVRecord cached;
  if (_enableLRUCache && _srvCache.find(name, cached))
    return cached;
#endif

  DNSContext ctx(*_pContext);
  std::string srv;
  std::string proto;

  char* tok = 0;
  tok = std::strtok((char*)name.c_str(), ".");
  if (!tok)
    return 0;
  srv = tok;

  tok = std::strtok(0, ".");
  if (!tok)
    return 0;
  proto = tok;

  dns_rr_srv* pRr = dns_resolve_srv(ctx.context(),
    name.c_str() + srv.length() + proto.length() + 2, // domain less the srv and proto string plus two dots
    srv.c_str() + 1, // srv less the underscore
    proto.c_str() + 1,  // proto less the underscore
    flags);

  if (pRr)
  {
    DNSSRVRecord rr(pRr);
#ifdef ENABLE_LRU_CACHE
    if (_enableLRUCache)
      _srvCache.insert(rr);
#endif
    free(pRr);
    return rr;
  }

  return DNSSRVRecord();
}


#ifdef ENABLE_ASYNC_RESOLVE

struct ResolveA4CB
{
  void* user_data;
  DNSARecordV4CB cb;
  DNSResolver* resolver;
};

struct ResolveA6CB
{
  void* user_data;
  DNSARecordV6CB cb;
  DNSResolver* resolver;
};

struct ResolveMXCB
{
  void* user_data;
  DNSMXRecordCB cb;
  DNSResolver* resolver;
};

struct ResolveTXTCB
{
  void* user_data;
  DNSTXTRecordCB cb;
  DNSResolver* resolver;
};

struct ResolvePTRCB
{
  void* user_data;
  DNSPTRRecordCB cb;
  DNSResolver* resolver;
};

struct ResolveNAPTRCB
{
  void* user_data;
  DNSNAPTRRecordCB cb;
  DNSResolver* resolver;
};

struct ResolveSRVCB
{
  void* user_data;
  DNSSRVRecordCB cb;
  DNSResolver* resolver;
};

static void dns_query_a4_cb(struct dns_ctx* ctx, struct dns_rr_a4* result, void* userData)
{
  ResolveA4CB* pCb = static_cast<ResolveA4CB*>(userData);
  if (result && pCb)
  {
    DNSARecordV4 rr(result);
#ifdef ENABLE_LRU_CACHE
    DNSResolver* pResolver = pCb->resolver;
    if (pResolver->isLRUCacheEnabled())
    {
      pResolver->v4Cache().insert(rr);
    }
#endif
    pCb->cb(rr, pCb->user_data);
    delete pCb;
  }
}

void DNSResolver::resolveA4(const std::string& name, int flags, DNSARecordV4CB cb, void* userData) const
{
#ifdef ENABLE_LRU_CACHE
  DNSARecordV4 cached;
  if (_enableLRUCache && _v4Cache.find(name, cached))
  {
    cb(cached, userData);
    return;
  }
#endif

  mutex_lock lock(_eventMutex);
  ResolveA4CB* pCB = new ResolveA4CB();
  pCB->cb = cb;
  pCB->user_data = userData;
  pCB->resolver = const_cast<DNSResolver*>(this);
  dns_submit_a4(_pContext->context(), name.c_str(), flags, dns_query_a4_cb, (void*)pCB);
}

static void dns_query_a6_cb(struct dns_ctx* ctx, struct dns_rr_a6* result, void* userData)
{
  ResolveA6CB* pCb = static_cast<ResolveA6CB*>(userData);
  if (pCb)
  {
    DNSARecordV6 rr(result);
#ifdef ENABLE_LRU_CACHE
    DNSResolver* pResolver = pCb->resolver;
    if (pResolver->isLRUCacheEnabled())
    {
      pResolver->v6Cache().insert(rr);
    }
#endif
    pCb->cb(rr, pCb->user_data);
    delete pCb;
  }
}

void DNSResolver::resolveA6(const std::string& name, int flags, DNSARecordV6CB cb, void* userData) const
{
#ifdef ENABLE_LRU_CACHE
  DNSARecordV6 cached;
  if (_enableLRUCache && _v6Cache.find(name, cached))
  {
    cb(cached, userData);
    return;
  }
#endif
  mutex_lock lock(_eventMutex);
  ResolveA6CB* pCB = new ResolveA6CB();
  pCB->cb = cb;
  pCB->user_data = userData;
  pCB->resolver = const_cast<DNSResolver*>(this);
  dns_submit_a6(_pContext->context(), name.c_str(), flags, dns_query_a6_cb, (void*)pCB);
}

static void dns_query_srv_cb(dns_ctx* pCtx, dns_rr_srv* pResult, void* pUserData)
{
  ResolveSRVCB* pCb = static_cast<ResolveSRVCB*>(pUserData);
  if (pCb)
  {
    DNSSRVRecord rr(pResult);
#ifdef ENABLE_LRU_CACHE
    DNSResolver* pResolver = pCb->resolver;
    if (pResolver->isLRUCacheEnabled())
    {
      pResolver->srvCache().insert(rr);
    }
#endif
    pCb->cb(rr, pCb->user_data);
    delete pCb;
  }
}

void DNSResolver::resolveSRV(const std::string& name, int flags, DNSSRVRecordCB cb, void* userData) const
{
#ifdef ENABLE_LRU_CACHE
  DNSSRVRecord cached;
  if (_enableLRUCache && _srvCache.find(name, cached))
  {
    cb(cached, userData);
    return;
  }
#endif

  std::string srv;
  std::string proto;

  char* tok = 0;
  tok = std::strtok((char*)name.c_str(), ".");
  if (!tok)
  {
    return;
  }
  srv = tok;

  tok = std::strtok(0, ".");
  if (!tok)
  {
    return;
  }
  proto = tok;

  mutex_lock lock(_eventMutex);
  ResolveSRVCB* pCB = new ResolveSRVCB();
  pCB->cb = cb;
  pCB->user_data = userData;
  pCB->resolver = const_cast<DNSResolver*>(this);
  dns_submit_srv(_pContext->context(),
    name.c_str() + srv.length() + proto.length() + 2, // domain less the srv and proto string plus two dots
    srv.c_str() + 1, // srv less the underscore
    proto.c_str() + 1,  // proto less the underscore
    flags,
    dns_query_srv_cb,
    (void*)pCB);
}

static void dns_query_naptr_cb(dns_ctx* pCtx, dns_rr_naptr* pResult, void* pUserData)
{
  ResolveNAPTRCB* pCb = static_cast<ResolveNAPTRCB*>(pUserData);
  if (pCb)
  {
    DNSNAPTRRecord rr(pResult);
#ifdef ENABLE_LRU_CACHE
    DNSResolver* pResolver = pCb->resolver;
    if (pResolver->isLRUCacheEnabled())
    {
      pResolver->naPtrCache().insert(rr);
    }
#endif
    pCb->cb(rr, pCb->user_data);
    delete pCb;
  }
}

void DNSResolver::resolveNAPTR(const std::string& name, int flags, DNSNAPTRRecordCB cb, void* userData) const
{
#ifdef ENABLE_LRU_CACHE
  DNSNAPTRRecord cached;
  if (_enableLRUCache && _naPtrCache.find(name, cached))
  {
    cb(cached, userData);
    return;
  }
#endif
  mutex_lock lock(_eventMutex);
  ResolveNAPTRCB* pCB = new ResolveNAPTRCB();
  pCB->cb = cb;
  pCB->user_data = userData;
  pCB->resolver = const_cast<DNSResolver*>(this);
  dns_submit_naptr(_pContext->context(), name.c_str(), flags, dns_query_naptr_cb, (void*)pCB);
}

static void dns_query_ptr_cb(dns_ctx* pCtx, dns_rr_ptr* pResult, void* pUserData)
{
  ResolvePTRCB* pCb = static_cast<ResolvePTRCB*>(pUserData);
  if (pCb)
  {
    DNSPTRRecord rr(pResult);
#ifdef ENABLE_LRU_CACHE
    DNSResolver* pResolver = pCb->resolver;
    if (pResolver->isLRUCacheEnabled())
    {
      pResolver->ptrCache().insert(rr);
    }
#endif
    pCb->cb(rr, pCb->user_data);
    delete pCb;
  }
}

void DNSResolver::resolvePTR4(const std::string& ip4address, DNSPTRRecordCB cb, void* userData) const
{
#ifdef ENABLE_LRU_CACHE
  DNSPTRRecord cached;
  if (_enableLRUCache && _ptrCache.find(ip4address, cached))
  {
    cb(cached, userData);
    return;
  }
#endif

  mutex_lock lock(_eventMutex);
  ResolvePTRCB* pCB = new ResolvePTRCB();
  pCB->cb = cb;
  pCB->user_data = userData;
  pCB->resolver = const_cast<DNSResolver*>(this);
  in_addr ip4;
  dns_pton(AF_INET, ip4address.c_str(), &ip4);
  dns_submit_a4ptr(_pContext->context(), &ip4, dns_query_ptr_cb, (void*)pCB);
}

void DNSResolver::resolvePTR6(const std::string& ip6address, DNSPTRRecordCB cb, void* userData) const
{
#ifdef ENABLE_LRU_CACHE
  DNSPTRRecord cached;
  if (_enableLRUCache && _ptrCache.find(ip6address, cached))
  {
    cb(cached, userData);
    return;
  }
#endif
  mutex_lock lock(_eventMutex);
  ResolvePTRCB* pCB = new ResolvePTRCB();
  pCB->cb = cb;
  pCB->user_data = userData;
  pCB->resolver = const_cast<DNSResolver*>(this);
  in6_addr ip6;
  dns_pton(AF_INET6, ip6address.c_str(), &ip6);
  dns_submit_a6ptr(_pContext->context(), &ip6, dns_query_ptr_cb, (void*)pCB);
}

static void dns_query_mx_cb(dns_ctx* pCtx, dns_rr_mx* pResult, void* pUserData)
{
  ResolveMXCB* pCb = static_cast<ResolveMXCB*>(pUserData);
  if (pCb)
  {
    DNSMXRecord rr(pResult);
#ifdef ENABLE_LRU_CACHE
    DNSResolver* pResolver = pCb->resolver;
    if (pResolver->isLRUCacheEnabled())
    {
      pResolver->mxCache().insert(rr);
    }
#endif
    pCb->cb(rr, pCb->user_data);
    delete pCb;
  }
}

void DNSResolver::resolveMX(const std::string& name, int flags, DNSMXRecordCB cb, void* userData) const
{
#ifdef ENABLE_LRU_CACHE
  DNSMXRecord cached;
  if (_enableLRUCache && _mxCache.find(name, cached))
  {
    cb(cached, userData);
    return;
  }
#endif
  mutex_lock lock(_eventMutex);
  ResolveMXCB* pCB = new ResolveMXCB();
  pCB->cb = cb;
  pCB->user_data = userData;
  pCB->resolver = const_cast<DNSResolver*>(this);
  dns_submit_mx(_pContext->context(), name.c_str(), flags, dns_query_mx_cb, (void*)pCB);
}

static void dns_query_txt_cb(dns_ctx* pCtx, dns_rr_txt* pResult, void* pUserData)
{
  ResolveTXTCB* pCb = static_cast<ResolveTXTCB*>(pUserData);
  if (pCb)
  {
    DNSTXTRecord rr(pResult);
#ifdef ENABLE_LRU_CACHE
    DNSResolver* pResolver = pCb->resolver;
    if (pResolver->isLRUCacheEnabled())
    {
      pResolver->txtCache().insert(rr);
    }
#endif
    pCb->cb(rr, pCb->user_data);
    delete pCb;
  }
}

void DNSResolver::resolveTXT(const std::string& name, int qcls, int flags, DNSTXTRecordCB cb, void* userData) const
{
#ifdef ENABLE_LRU_CACHE
  DNSTXTRecord cached;
  if (_enableLRUCache && _txtCache.find(name, cached))
  {
    cb(cached, userData);
    return;
  }
#endif
  mutex_lock lock(_eventMutex);
  ResolveTXTCB* pCB = new ResolveTXTCB();
  pCB->cb = cb;
  pCB->user_data = userData;
  pCB->resolver = const_cast<DNSResolver*>(this);
  dns_submit_txt(_pContext->context(), name.c_str(), qcls, flags, dns_query_txt_cb, (void*)pCB);
}

void DNSResolver::start()
{
  if (!_pThread)
    _pThread = new boost::thread(boost::bind(&DNSResolver::processEvents, this));
}

void DNSResolver::stop()
{
  _stopProcessingEvents = true;
  if (_pThread)
  {
    _pThread->join();
    delete _pThread;
    _pThread = 0;
  }
}

void DNSResolver::processEvents()
{
  fd_set fds;
  timeval tv;
  time_t now = 0;
  int nextTimeout = 0;

  FD_ZERO(&fds);
  while (!_stopProcessingEvents)
  {
    _eventMutex.lock();
    nextTimeout = dns_timeouts(_pContext->context(), -1, now);
    _eventMutex.unlock();
    if (nextTimeout <= 0)
      nextTimeout = 1;
    FD_SET(_pContext->getSocketFd(), &fds);
    tv.tv_sec = nextTimeout;
    tv.tv_usec = 0;
    now = time(0);
    if (select(_pContext->getSocketFd() + 1, &fds, 0, 0, &tv) > 0)
    {
      _eventMutex.lock();
      dns_ioevent(_pContext->context(), now);
      std::cout << "PROCESSING EVENTS" << std::endl;
      _eventMutex.unlock();
    }
  }
}



#endif // ENABLE_ASYNC_RESOLVE

} // namespace udnspp
