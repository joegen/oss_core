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


#ifndef OSS_CACHE_H_INCLUDED
#define OSS_CACHE_H_INCLUDED

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/noncopyable.hpp>

#include "OSS/OSS.h"


namespace OSS {

class Cacheable : boost::noncopyable
{
public:
  typedef boost::shared_ptr<Cacheable> Ptr;
  Cacheable(const std::string& id,  const boost::any& data);
    /// Create a new cachable object.
    /// A cacheable object accepts a polymorphic type
    /// identifiable by an string ID

  boost::any& data();
    /// Return the data

  const std::string& getIdentifier() const;
    /// Return the cache identifier

private:
  boost::any _data;
  std::string _identifier;
};


class CacheManager : boost::noncopyable
{
public:
  CacheManager(int expireInSeconds);
    /// Create a new cache mamanager
  
  ~CacheManager();
    /// Delete cache manager

  void add(const std::string& id, const boost::any& obj);
    /// Insert a new cacheable object into the cache.
    /// If an object with the same id already exists,
    /// it will be overwritten

  void add(Cacheable::Ptr pCacheable);
    /// Insert a new cacheable object into the cache.
    /// If an object with the same id already exists,
    /// it will be overwritten

  Cacheable::Ptr get(const std::string& id) const;
    /// Get a pointer to a cacheable object specified by id

  Cacheable::Ptr pop(const std::string& id);
    /// Pop out a cacheable object specified by id

  void remove(const std::string& id);
    /// Erase a cacheable specified by id

  bool has(const std::string& id) const;
    /// Return true if the object is in cache

  void clear();
    /// Clear all entries

private:
  OSS::OSS_HANDLE _manager;
};

//
// Inlines
//

inline boost::any& Cacheable::data()
{
  return _data;
}

inline const std::string& Cacheable::getIdentifier() const
{
  return _identifier;
}

class StringPairCache : boost::noncopyable
{
public:
  StringPairCache(int expireInSeconds);
    /// Create a new cache mamanager

  ~StringPairCache();
    /// Delete cache manager

  void add(const std::string& id, const std::string& value);
    /// Insert a new cacheable object into the cache.
    /// If an object with the same id already exists,
    /// it will be overwritten

  std::string get(const std::string& id) const;
    /// Get a pointer to a cacheable object specified by id

  std::string pop(const std::string& id);
    /// Pop out a cacheable object specified by id

  void remove(const std::string& id);
    /// Erase a cacheable specified by id

  bool has(const std::string& id) const;
    /// Return true if the object is in cache

  void clear();
    /// Clear all entries

private:
  OSS::OSS_HANDLE _manager;
};

} // OSS

#endif // OSS_CACHE_H_INCLUDED


