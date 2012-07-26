// OSS Software Solutions Application Programmer Interface
// Package: OSSAPI
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSSAPI API.g
//
// Copyright (c) OSS Software Solutions
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

#ifndef OSS_REDISCLIENT_H_INCLUDED
#define	OSS_REDISCLIENT_H_INCLUDED

#include "hiredis/hiredis.h"
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include "json/reader.h"
#include "json/writer.h"
#include "json/elements.h"
#include <map>

namespace OSS {



class RedisClient : boost::noncopyable
{
public:
  typedef boost::recursive_mutex mutex;
  typedef boost::lock_guard<mutex> mutex_lock;

  enum RedisWorkspace
  {
    SBC_SYSTEMDB,
    SBC_REGDB,
    SBC_DIALOGDB,
    SBC_RTPDB,
    SBC_RESERVE_1,
    SBC_RESERVE_2,
    SBC_RESERVE_3,
    SBC_RESERVE_4,
    PROXY_DATASTORE,
  };

  enum Type
  {
    TCP,
    UNIX
  };

  RedisClient() :
    _context(0),
    _type(TCP),
    _tcpHost("127.0.0.1"),
    _tcpPort(6379),
    _db(0)
  {

  }

  RedisClient(const std::string& tcpHost, int tcpPort) :
    _context(0),
    _type(TCP),
    _tcpHost(tcpHost),
    _tcpPort(tcpPort),
    _db(0)
  {
  }

  RedisClient(const std::string& unixSocketPath) :
    _context(0),
    _type(UNIX),
    _tcpPort(6379),
    _unixSocketPath(unixSocketPath),
    _db(0)
  {
  }

  virtual ~RedisClient()
  {
    disconnect();
  }

  bool connect(const std::string& password_ = "", int db = 0)
  {
    disconnect();

    std::string password = password_;
    
    if (password.empty())
      password = _password;

    if (db == 0)
      db = _db;

    _password = password;
    _db = db;

    mutex_lock lock(_mutex);
    if (_type == TCP)
    {
      _context = redisConnect(_tcpHost.c_str(), _tcpPort);
    }else if (_type == UNIX)
    {
      _context = redisConnectUnix(_unixSocketPath.c_str());
    }
    if (_context == 0)
      return false;

    if (_context->err)
    {
      _lastError = _context->errstr;
      return false;
    }

    redisReply *reply;
    //
    // If password is set, then we send auth
    //
    if (!password.empty())
    {
      reply = (redisReply*)redisCommand(_context,"AUTH %s", password.c_str());
      bool authenticated = false;
      if (reply)
      {
        authenticated = reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"ok") == 0;
        freeReplyObject(reply);
        reply = 0;
      }
      if (!authenticated)
      {
        disconnect();
        return false;
      }
    }

    if (db != 0)
    {
      reply = (redisReply*)redisCommand(_context,"SELECT %d", db);
      bool selected = false;
      if (reply)
      {
        selected = reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"ok") == 0;
        freeReplyObject(reply);
        reply = 0;
      }
      if (!selected)
      {
        disconnect();
        return false;
      }
    }

    return true;
  }

  void disconnect()
  {
    mutex_lock lock(_mutex);
    if (_context)
    {
      redisFree(_context);
      _context = 0;
    }
  }

  virtual bool set(const std::string& key, const json::Object& value, int expires = -1)
  {
    try
    {
      std::ostringstream strm;
      json::Writer::Write(value, strm);
      std::string buff = strm.str();
      return set(key, buff, expires);
    }
    catch(std::exception& error)
    {
      return false;
    }
  }

  virtual bool set(const std::string& key, const std::string& value, int expires = -1)
  {
    mutex_lock lock(_mutex);

    if (!_context)
      if (!connect())
        return false;
    if (!_context)
      return false;
    
    redisReply *reply;
    if (expires == -1)
      reply = (redisReply*)redisCommand(_context,"SET %s %s",key.c_str(),value.c_str());
    else
      reply = (redisReply*)redisCommand(_context,"SETEX %s %d %s",key.c_str(), expires, value.c_str());

    if (_context->err)
    {
      _lastError = _context->errstr;
      freeReplyObject(reply);

      if (_context->err == REDIS_ERR_EOF || _context->err == REDIS_ERR_IO)
      {
        //
        // Try to reconnect
        //
        if (connect())
        {
          if (expires == -1)
            reply = (redisReply*)redisCommand(_context,"SET %s %s",key.c_str(),value.c_str());
          else
            reply = (redisReply*)redisCommand(_context,"SETEX %s %d %s",key.c_str(), expires, value.c_str());
          bool ok = reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"ok") == 0;
          freeReplyObject(reply);
          return ok;
        }
      }
      else
      {
        //
        // This is not an io error.  bail out
        //
        return false;
      }
    }

    bool ok = false;
    if (reply)
    {
      ok = reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"ok") == 0;
      freeReplyObject(reply);
    }
    return ok;
  }

  virtual bool get(const std::string& key, json::Object& value) const
  {
    std::string buff;
    if (!get(key, buff))
      return false;
    try
    {
      std::stringstream strm;
      strm << buff;
      json::Reader::Read(value, strm);
    }
    catch(std::exception& error)
    {
      return false;
    }
    return true;
  }

  virtual bool get(const std::string& key, std::string& value) const
  {
    mutex_lock lock(_mutex);

    if (!_context)
      if (!const_cast<RedisClient*>(this)->connect())
        return false;
    if (!_context)
      return false;

    bool ok = false;
    redisReply* reply = (redisReply*)redisCommand(_context,"GET %s", key.c_str());

    if (_context->err)
    {
      _lastError = _context->errstr;
      const_cast<RedisClient*>(this)->freeReplyObject(reply);
      if (_context->err == REDIS_ERR_EOF || _context->err == REDIS_ERR_IO)
      {
        //
        // Try to reconnect
        //
        if (const_cast<RedisClient*>(this)->connect())
        {
          reply = (redisReply*)redisCommand(_context,"GET %s", key.c_str());

          if (reply && reply->type == REDIS_REPLY_STRING && reply->len > 0)
          {
            value = std::string(reply->str, reply->len);
            ok = true;
          }
          const_cast<RedisClient*>(this)->freeReplyObject(reply);
          return ok;
        }
      }
      else
      {
        //
        // This is not an io error.  bail out
        //
        return false;
      }
    }

    if (reply && reply->type == REDIS_REPLY_STRING && reply->len > 0)
    {
      value = std::string(reply->str, reply->len);
      ok = true;
    }
    const_cast<RedisClient*>(this)->freeReplyObject(reply);

    if (_context->err)
    {
      _lastError = _context->errstr;
      if (_lastError.empty())
        _lastError = "Unknown exception";
      //throw std::runtime_error(_lastError);
      ok = false;
    }

    return ok;
  }

  virtual bool getKeys(const std::string& pattern, std::vector<std::string>& keys)
  {
    mutex_lock lock(_mutex);

    if (!_context)
      if (!connect())
        return false;
    if (!_context)
      return false;

    bool ok = false;
    redisReply* reply = (redisReply*)redisCommand(_context,"KEYS %s", pattern.c_str());

    if (_context->err)
    {
      _lastError = _context->errstr;
      freeReplyObject(reply);
      if (_context->err == REDIS_ERR_EOF || _context->err == REDIS_ERR_IO)
      {
        //
        // Try to reconnect
        //
        if (connect())
        {
          reply = (redisReply*)redisCommand(_context,"KEYS %s", pattern.c_str());

          if (reply && reply->type == REDIS_REPLY_ARRAY && reply->elements > 0)
          {
            for (size_t i = 0; i < reply->elements; i++)
            {
              redisReply* item = reply->element[i];
              if (item && item->type == REDIS_REPLY_STRING && item->len > 0)
              {
                keys.push_back(std::string(reply->str, reply->len));
                ok = true;
              }
            }

          }
          freeReplyObject(reply);
          return ok;
        }
      }
      else
      {
        //
        // This is not an io error.  bail out
        //
        return false;
      }
    }

    if (reply && reply->type == REDIS_REPLY_ARRAY && reply->elements > 0)
    {
      for (size_t i = 0; i < reply->elements; i++)
      {
        redisReply* item = reply->element[i];
        if (item && item->type == REDIS_REPLY_STRING && item->len > 0)
        {
          keys.push_back(std::string(item->str, item->len));
          ok = true;
        }
      }
    }

    freeReplyObject(reply);

    if (_context->err)
    {
      _lastError = _context->errstr;
      if (_lastError.empty())
        _lastError = "Unknown exception";
      //throw std::runtime_error(_lastError);
      ok = false;
    }

    return ok;
  }

  virtual bool erase(const std::string& key)
  {
    mutex_lock lock(_mutex);

    if (!_context)
      if (!connect())
        return false;
    if (!_context)
      return false;

    redisReply* reply = (redisReply*)redisCommand(_context,"DEL %s", key.c_str());
    
    if (_context->err)
    {
      _lastError = _context->errstr;
      freeReplyObject(reply);
      if (_context->err == REDIS_ERR_EOF || _context->err == REDIS_ERR_IO)
      {
        //
        // Try to reconnect
        //
        if (connect())
        {
          reply = (redisReply*)redisCommand(_context,"DEL %s", key.c_str());

           bool ok = reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"ok") == 0;
          freeReplyObject(reply);
          return ok;
        }
      }
      else
      {
        //
        // This is not an io error.  bail out
        //
        return false;
      }
    }

    bool ok = false;
    if (reply)
    {
      ok = reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"ok") == 0;
      freeReplyObject(reply);
    }
    return ok;
  }
  
  void freeReplyObject(redisReply* reply)
  {
    if (reply)
      ::freeReplyObject(reply);
  }

protected:
  mutable mutex _mutex;
  redisContext* _context;
  Type _type;
  std::string _tcpHost;
  int _tcpPort;
  std::string _unixSocketPath;
  mutable std::string _lastError;
  std::string _password;
  int _db;
};

class RedisBroadcastClient : public RedisClient
{
public:
  typedef std::map<std::string, RedisClient*> Pool;

  RedisBroadcastClient() :
    _defaultClient(0)
  {
  }

  ~RedisBroadcastClient()
  {
    disconnect();
    //
    // delete _defaultClient;   // disconnect() will delete the pointer
    //
  }

  bool connect(const std::string& tcpHost, int tcpPort, const std::string& password = "", int db = 0)
  {


    std::ostringstream key;
    key << tcpHost << ":" << tcpPort;
    Pool::iterator instance = _pool.find(key.str());
    if (instance == _pool.end())
    {
      RedisClient* client = 0;
      if (_defaultClient == 0)
      {
        _defaultClient = new RedisClient(tcpHost, tcpPort);
        if (!_defaultClient->connect(password, db))
        {
          delete _defaultClient;
          _defaultClient = 0;
          return false;
        }
        client = _defaultClient;
      }
      else
      {
        client = new RedisClient(tcpHost, tcpPort);
        if (!client->connect(password, db))
        {
          delete client;
          return false;
        }
      }
      if (client)
        _pool[key.str()] = client;
    }
    return true;
  }

  void disconnect()
  {
    for (Pool::iterator iter = _pool.begin(); iter != _pool.end(); iter++)
    {
      RedisClient* client = iter->second;
      client->disconnect();
      delete client;
    }
    _pool.clear();
  }

  bool set(const std::string& key, const json::Object& value, int expires = -1)
  {
    for (Pool::iterator iter = _pool.begin(); iter != _pool.end(); iter++)
    {
      RedisClient* client = iter->second;
      client->set(key, value, expires);
    }
    return true;
  }

  bool set(const std::string& key, const std::string& value, int expires = -1)
  {
    for (Pool::iterator iter = _pool.begin(); iter != _pool.end(); iter++)
    {
      RedisClient* client = iter->second;
      client->set(key, value, expires);
    }
    return true;
  }

  bool get(const std::string& key, json::Object& value) const
  {
    //
    // First try the default client
    //
    if (!_defaultClient)
      return false;

    try
    {
      if (_defaultClient->get(key, value))
        return true;
      else
        return false;
    }
    catch(...)
    {
    }
    
    for (Pool::const_iterator iter = _pool.begin(); iter != _pool.end(); iter++)
    {
      try
      {
        RedisClient* client = iter->second;
        if (client == _defaultClient)
          continue;

        if (client->get(key, value))
          return true;
        else
          return false;
      }
      catch(...)
      {
        continue;
      }
    }

    return false;
  }

  bool get(const std::string& key, std::string& value) const
  {
    //
    // First try the default client
    //
    if (!_defaultClient)
      return false;

    try
    {
      if (_defaultClient->get(key, value))
        return true;
      else
        return false;
    }
    catch(...)
    {
    }

    for (Pool::const_iterator iter = _pool.begin(); iter != _pool.end(); iter++)
    {
      try
      {
        RedisClient* client = iter->second;
        if (client == _defaultClient)
          continue;

        if (client->get(key, value))
          return true;
        else
          return false;
      }
      catch(...)
      {
        continue;
      }
    }

    return false;
  }

  bool getKeys(const std::string& pattern, std::vector<std::string>& keys)
  {
    //
    // First try the default client
    //
    if (!_defaultClient)
      return false;

    try
    {
      if (_defaultClient->getKeys(pattern, keys))
        return true;
      else
        return false;
    }
    catch(...)
    {
    }

    for (Pool::iterator iter = _pool.begin(); iter != _pool.end(); iter++)
    {
      try
      {
        RedisClient* client = iter->second;
        if (client == _defaultClient)
          continue;

        if (client->getKeys(pattern, keys))
          return true;
        else
          return false;
      }
      catch(...)
      {
        continue;
      }
    }

    return false;
  }

  bool erase(const std::string& key)
  {
    for (Pool::iterator iter = _pool.begin(); iter != _pool.end(); iter++)
    {
      RedisClient* client = iter->second;
      client->erase(key);
    }
    return true;
  }
  
protected:
  Pool _pool;
  RedisClient* _defaultClient;
};

}  // OSS


#endif	/* OSS_REDISCLIENT_H_INCLUDED */

