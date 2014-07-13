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


#ifndef OSS_KEYVALUESTOREBASE_H_INCLUDED
#define OSS_KEYVALUESTOREBASE_H_INCLUDED


#include "OSS/OSS.h"
#include "OSS/Core.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>


namespace OSS {
namespace Persistent {

#define PERSISTENT_STORE_EXPIRES_SUFFIX ".KV_EXPIRES"
  
struct KVRecord
{
  std::string key;
  std::string value;
};

typedef std::vector<std::string> KVKeys;
typedef std::vector<KVRecord> KVRecords;

 struct KVInputProcessor
{
  enum Action
  {
    Allow,
    Ignore,
    ActionMax
  };

  KVInputProcessor(const std::string name) : _name(name) {};

  boost::function<Action(const std::string& /*key*/, const std::string& /*value*/, unsigned int /*expireInSeconds*/)> put;
  boost::function<Action(const std::string& /*key*/)> del;
  boost::function<Action(const std::string& /*filter*/)> delKeys;
  std::string _name;
};

typedef std::vector<KVInputProcessor> KVInputProcessors;
  
template <typename KV>
class KeyValueStoreBase : boost::noncopyable
{
public:
  KeyValueStoreBase()
  {
  }
  
  ~KeyValueStoreBase()
  {
  }

  bool open(const std::string& path)
  {
    return _impl.open(path);
  }

  bool isOpen()
  {
    return _impl.isOpen();
  }

  bool close()
  {
    return _impl.close();
  }

  bool put(const std::string& key, const std::string& value)
  {
    for (KVInputProcessors::iterator iter = _inputProc.begin(); iter != _inputProc.end(); iter++)
    {
      if (iter->put)
        if (iter->put(key, value, -1) == KVInputProcessor::Ignore)
          return false;
    }
    return _impl.put(key, value);
  }

  bool put(const std::string& key, const std::string& value, unsigned int expireInSeconds)
  {
    for (KVInputProcessors::iterator iter = _inputProc.begin(); iter != _inputProc.end(); iter++)
    {
      if (iter->put)
        if (iter->put(key, value, expireInSeconds) == KVInputProcessor::Ignore)
          return false;
    }
    
    OSS::UInt64 expires = OSS::getTime() + (expireInSeconds*1000);
    std::string expireString = OSS::string_from_number(expires);
    std::string expireKey = key + PERSISTENT_STORE_EXPIRES_SUFFIX;
  
    if (!_impl.put(expireKey, expireString))
      return false;
    
    return _impl.put(key, value);
  }

  bool get(const std::string& key, std::string& value)
  {
    if (is_expired(key))
    {
      purge_expired(key);
      return false;
    }
    
    return _impl.get(key, value);
  }
  
  bool get(const std::string& key, std::string& value, bool purgeExpired)
  {
    if (purgeExpired)
    {
      if (is_expired(key))
      {
        purge_expired(key);
        return false;
      }
    }
    return _impl.get(key, value);
  }

  bool del(const std::string& key)
  {
    for (KVInputProcessors::iterator iter = _inputProc.begin(); iter != _inputProc.end(); iter++)
    {
      if (iter->del)
        if (iter->del(key) == KVInputProcessor::Ignore)
          return false;
    }
    return _impl.del(key);
  }
  
  bool delKeys(const std::string& filter)
  {
    for (KVInputProcessors::iterator iter = _inputProc.begin(); iter != _inputProc.end(); iter++)
    {
      if (iter->delKeys)
        if (iter->delKeys(filter) == KVInputProcessor::Ignore)
          return false;
    }
    return _impl.delKeys(filter);
  }
  
  bool getKeys(KVKeys& keys)
  {
    return _impl.getKeys(keys);
  }
  
  bool getKeys(const std::string& filter, KVKeys& keys)
  {
    return _impl.getKeys(filter, keys);
  }
  
  bool getRecords(KVRecords& records)
  {
    return _impl.getRecords(records);
  }
  
  bool getRecords(const std::string& filter, KVRecords& records)
  {
    return _impl.getRecords(filter, records);
  }
  
  std::string getPath() const
  {
    return _impl.getPath();
  }
  
  KV& getDB()
  {
    return _impl;
  }
  
  const std::string& getKeyPrefix() const
  {
    return _impl.getKeyPrefix();
  }
  
  void setKeyPrefix(const std::string& keyPrefix)
  {
    _impl.setKeyPrefix(keyPrefix);
  }
  
  void addInputProcessor(const KVInputProcessor& inputProcessor)
  {
    _inputProc.push_back(inputProcessor);
  }
  
protected:
  bool is_expired(const std::string& key)
  {
    std::string expireKey = key + PERSISTENT_STORE_EXPIRES_SUFFIX;

    std::string expires;
    if (!get(expireKey, expires, false))
      return false;

    OSS::UInt64 expireTime = OSS::string_to_number<OSS::UInt64>(expires);

    return expireTime <= OSS::getTime();
  }

  bool purge_expired(const std::string& key)
  {
    std::string expireKey = key + PERSISTENT_STORE_EXPIRES_SUFFIX;
    if (!del(key))
      return false;
    return del(expireKey);
  }
  
  KV _impl;  
  KVInputProcessors _inputProc;
};


} } // OSS::Persistent

#endif