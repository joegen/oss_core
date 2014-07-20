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


#include "OSS/UTL/Cache.h"
#include "Poco/ExpireCache.h"



namespace OSS {

typedef Poco::ExpireCache<std::string, OSS::Cacheable::Ptr> ExpireCache;
typedef Poco::SharedPtr<Cacheable::Ptr> ExpireCacheable;

Cacheable::Cacheable(const std::string& id,  const boost::any& data) :
  _data(data),
  _identifier(id)
{
}

CacheManager::CacheManager(int expireInSeconds)
{
  _manager = new ExpireCache(expireInSeconds * 1000);
}

CacheManager::~CacheManager()
{
  delete static_cast<ExpireCache*>(_manager);
}

void CacheManager::add(const std::string& id, const boost::any& obj)
{
  add(Cacheable::Ptr(new Cacheable(id, obj)));
}

void CacheManager::add(Cacheable::Ptr pCacheable)
{
  static_cast<ExpireCache*>(_manager)->add(pCacheable->getIdentifier(), pCacheable);
}

Cacheable::Ptr CacheManager::get(const std::string& id) const
{
  ExpireCacheable pCacheObj = static_cast<ExpireCache*>(_manager)->get(id);
  if (pCacheObj)
    return *pCacheObj;
  
  return Cacheable::Ptr();
}

Cacheable::Ptr CacheManager::pop(const std::string& id)
{
  ExpireCacheable pCacheObj = static_cast<ExpireCache*>(_manager)->get(id);
  if (pCacheObj)
  {
    static_cast<ExpireCache*>(_manager)->remove(id);
    return *pCacheObj;
  }

  return Cacheable::Ptr();
}

void CacheManager::remove(const std::string& id)
{
  static_cast<ExpireCache*>(_manager)->remove(id);
}

bool CacheManager::has(const std::string& id) const
{
  return static_cast<ExpireCache*>(_manager)->has(id);
}

void CacheManager::clear()
{
  return static_cast<ExpireCache*>(_manager)->clear();
}


/////////////////////////////////

typedef Poco::ExpireCache<std::string, std::string> StringPairExpireCache;
typedef Poco::SharedPtr<std::string> StringPairExpireCacheable;


StringPairCache::StringPairCache(int expireInSeconds)
{
  _manager = new StringPairExpireCache(expireInSeconds * 1000);
}

StringPairCache::~StringPairCache()
{
  delete static_cast<StringPairExpireCache*>(_manager);
}

void StringPairCache::add(const std::string& id, const std::string& value)
{
  static_cast<StringPairExpireCache*>(_manager)->add(id, value);
}

std::string StringPairCache::get(const std::string& id) const
{
  StringPairExpireCacheable pCacheObj = static_cast<StringPairExpireCache*>(_manager)->get(id);
  if (pCacheObj)
    return *pCacheObj;

  return std::string();
}

std::string StringPairCache::pop(const std::string& id)
{
  StringPairExpireCacheable pCacheObj = static_cast<StringPairExpireCache*>(_manager)->get(id);
  if (pCacheObj)
  {
    static_cast<StringPairExpireCache*>(_manager)->remove(id);
    return *pCacheObj;
  }

  return std::string();
}

void StringPairCache::remove(const std::string& id)
{
  static_cast<StringPairExpireCache*>(_manager)->remove(id);
}

bool StringPairCache::has(const std::string& id) const
{
  return static_cast<StringPairExpireCache*>(_manager)->has(id);
}

void StringPairCache::clear()
{
  return static_cast<StringPairExpireCache*>(_manager)->clear();
}


/////////////////////////////////






} // OSS

