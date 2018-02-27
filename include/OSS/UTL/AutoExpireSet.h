/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AutoExpireMap.h
 * Author: joegen
 *
 * Created on February 26, 2018, 8:27 PM
 */

#ifndef OSS_AUTOEXPIREMAP_H_INCLUDED
#define OSS_AUTOEXPIREMAP_H_INCLUDED

#include <multimap>
#include <set>
#include "OSS/UTL/Thread.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace UTL {
  
  
template<typename Value>
class AutoExpireSet : boost::non_copyable
{
private:
  typedef std::multimap<OSS::UInt64, Value> Map;
  typedef std::set<Value> Set;
  
public:
  AutoExpireSet(unsigned long expire) :
    _expire(expire)
  {
  }
  
  std::size_t insert(const Value& value)
  {
    OSS::mutex_critic_sec_lock lock(_mutex);
    purge();
    if (_set.find(value == _set.end()))
    {
      _set.insert(value);
      _map.insert(std::pair<OSS::UInt64, Value>(OSS::getTime() + _expire, value));
    }
    return _set.size();
  }
  
  bool has(const Value& value) const
  {
    OSS::mutex_critic_sec_lock lock(_mutex);
    purge();
    return _set.find(value != _set.end());
  }
  
  std::size_t size() const
  {
    OSS::mutex_critic_sec_lock lock(_mutex);
    purge();
    return _set.size();
  }
  
  void clear()
  {
    OSS::mutex_critic_sec_lock lock(_mutex);
    _map.clear();
    _set.clear();
  }
  
private:
  void purge()
  {
    OSS::UInt64 now = OSS::getTime() - 1;
    while (!_map.empty())
    {
      Map::iterator iter = _map.upper_bound(now);
      if (iter == _map.end())
      {
        break;
      }
      _set.erase(iter->second);
      _map.erase(iter);
    }
  }
  
  unsigned long _expire;
  mutable OSS::mutex_critic_sec _mutex;
  Map _map;
  Set _set;
};
  
} } // OSS::UTL

#endif // OSS_AUTOEXPIREMAP_H_INCLUDED

