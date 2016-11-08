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

#ifndef UDNSPP_DNSCACHE_INCLUDED
#define UDNSPP_DNSCACHE_INCLUDED

#ifdef HAVE_BOOSTLIBS
#if !defined(NDEBUG)
#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

#include <boost/thread.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#define ENABLE_LRU_CACHE
#endif


#ifdef ENABLE_LRU_CACHE

#include <map>
#include <cassert>
#include <udnspp/dnsrrcommon.h>


#ifdef WINDOWS
#include <Windows.h>
#include <sys/types.h> 
#include <sys/timeb.h>
#endif

namespace udnspp {

#if WINDOWS
  typedef unsigned __int64 DNSCacheExpireTime;
#else
#if defined(__LP64__)
  typedef unsigned long      DNSCacheExpireTime;
#else
  typedef unsigned long long DNSCacheExpireTime;
#endif
#endif

  template <typename T>
  class DNSCache
  {
  public:
    typedef boost::mutex mutex;
    typedef boost::lock_guard<mutex> mutex_lock;

    struct qname{};
    struct lrutime{};
    struct entry
    {
      DNSCacheExpireTime lruTime;
      std::string qname;
      T rec;
    };
    typedef boost::multi_index_container<
      entry,
      boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<
      boost::multi_index::tag<qname>, BOOST_MULTI_INDEX_MEMBER(entry, std::string, qname)>,
      boost::multi_index::ordered_non_unique<
      boost::multi_index::tag<lrutime>, BOOST_MULTI_INDEX_MEMBER(entry, DNSCacheExpireTime, lruTime)> >
    > LRUCache;
    
    DNSCache(std::size_t maxSize) :
      _maxSize(maxSize)
    {
      assert(_maxSize);
    }

    void makeRoom()
    {
      mutex_lock lock(_mutex);
      while (_cache.size() > _maxSize)
      {
        _cache.template get<1>().erase(_cache.template get<1>().begin());
      }
    }

    void insert(const T& record)
    {
      makeRoom();

      entry rec;
      rec.rec  = record;
      rec.lruTime = getExpireTime();
      rec.qname = record.getQName();

      mutex_lock lock(_mutex);
      _cache.insert(rec);
    }

    bool find(const std::string& key, T& record)
    {
      mutex_lock lock(_mutex);
      
      typedef typename LRUCache::template nth_index<0>::type rr_set;

      typename rr_set::iterator iter = _cache.template get<0>().find(key);

      if (iter == _cache.template get<0>().end())
        return false;
      DNSCacheExpireTime lruTime = iter->lruTime;
      DNSCacheExpireTime expireTime = (iter->rec.getTTL() * 1000) + lruTime;
      DNSCacheExpireTime now = getExpireTime();

      if (now > expireTime)
      {
        _cache.template get<0>().erase(iter);
        return false;
      }

      record = iter->rec;
      return true;
    }

  protected:
    DNSCacheExpireTime getExpireTime()
    {
#ifdef WINDOWS
      struct _timeb sTimeB;
      _ftime(&sTimeB);
      return (DNSCacheExpireTime)(sTimeB.time * 1000 + sTimeB.millitm);
#else
      struct timeval sTimeVal;
      gettimeofday(&sTimeVal, NULL);
      return (DNSCacheExpireTime)(sTimeVal.tv_sec * 1000 + (sTimeVal.tv_usec / 1000));
#endif
    }
  private:
    std::size_t _maxSize;
    LRUCache _cache;
    mutex _mutex;
  };

}  // namespace udnspp

#endif // ENABLE_LRU_CACHE

#endif // UDNSPP_DNSCACHE_INCLUDED