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

#include "OSS/Persistent/RedisClient.h"


namespace OSS {
namespace Persistent {
  


RedisClient::RedisClient() :
    _context(0),
    _type(TCP),
    _tcpHost("127.0.0.1"),
    _tcpPort(6379),
    _db(0),
    _connected(false)
{
  OSS_LOG_INFO("[REDIS] << Redis client CREATED - " << _tcpHost << ":" << _tcpPort);
}

RedisClient::RedisClient(const std::string& tcpHost, int tcpPort) :
    _context(0),
    _type(TCP),
    _tcpHost(tcpHost),
    _tcpPort(tcpPort),
    _db(0),
    _connected(false)
{
  OSS_LOG_INFO("[REDIS] << Redis client CREATED - " << _tcpHost << ":" << _tcpPort);
}

RedisClient::RedisClient(const std::string& unixSocketPath) :
    _context(0),
    _type(UNIX),
    _tcpPort(6379),
    _unixSocketPath(unixSocketPath),
    _db(0),
    _connected(false)
{
}

RedisClient::~RedisClient()
{
  disconnect();
}

bool RedisClient::connect(const std::string& password_, int db)
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
    OSS_LOG_INFO("[REDIS] Connecting to tcp:" << _tcpHost << ":" << _tcpPort);
    _context = redisConnect(_tcpHost.c_str(), _tcpPort);
  }else if (_type == UNIX)
  {
    OSS_LOG_INFO("[REDIS] Connecting to unix:" << _unixSocketPath);
    _context = redisConnectUnix(_unixSocketPath.c_str());
  }

  if (!_context)
  {
    if (_type == TCP)
    {
      OSS_LOG_ERROR("[REDIS] Unable to connect to tcp:" << _tcpHost << ":" << _tcpPort);
    }else if (_type == UNIX)
    {
      OSS_LOG_ERROR("[REDIS] Unable to connect to unix:" << _unixSocketPath);
    }
    return false;
  }
  else
  {
    if (_type == TCP)
    {
      OSS_LOG_INFO("[REDIS] Connected to tcp:" << _tcpHost << ":" << _tcpPort);
    }else if (_type == UNIX)
    {
      OSS_LOG_INFO("[REDIS] Connected to unix:" << _unixSocketPath);
    }
  }

  if (_context->err)
  {
    _lastError = _context->errstr;
    if (_type == TCP)
    {
      OSS_LOG_ERROR("[REDIS] Error connecting to tcp:" << _tcpHost << ":" << _tcpPort << " - " << _lastError);
    }else if (_type == UNIX)
    {
      OSS_LOG_ERROR("[REDIS] Error connecting to unix:" << _unixSocketPath << " - " << _lastError);
    }

    disconnect();
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
      freeReply(reply);
      reply = 0;
      if (!authenticated)
      {
        if (_type == TCP)
        {
          OSS_LOG_ERROR("[REDIS] Unable to authenticate with tcp:" << _tcpHost << ":" << _tcpPort);
        }else if (_type == UNIX)
        {
          OSS_LOG_ERROR("[REDIS] Unable to authenticate with unix:" << _unixSocketPath);
        }

        disconnect();
        return false;
      }
    }
    else
    {
      if (_type == TCP)
      {
        OSS_LOG_ERROR("[REDIS] Unable to authenticate with tcp:" << _tcpHost << ":" << _tcpPort << " - NOREPLY");
      }else if (_type == UNIX)
      {
        OSS_LOG_ERROR("[REDIS] Unable to authenticate with unix:" << _unixSocketPath << " - NOREPLY");
      }

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

      if (!selected)
      {
        if (reply->type == REDIS_REPLY_STATUS)
        {
          if (_type == TCP)
          {
            OSS_LOG_ERROR("[REDIS] Unable to select database from tcp:" << _tcpHost << ":" << _tcpPort << " - " << reply->str);
          }else if (_type == UNIX)
          {
            OSS_LOG_ERROR("[REDIS] Unable to select database from unix:" << _unixSocketPath << " - " << reply->str);
          }
        }

        freeReply(reply);
        reply = 0;

        disconnect();
        return false;
      }

      freeReply(reply);
      reply = 0;

    }
    else
    {
      if (_type == TCP)
      {
        OSS_LOG_ERROR("[REDIS] Unable to select database from tcp:" << _tcpHost << ":" << _tcpPort << " - NOREPLY");
      }else if (_type == UNIX)
      {
        OSS_LOG_ERROR("[REDIS] Unable to select database from unix:" << _unixSocketPath << " - NOREPLY");
      }

      disconnect();
      return false;
    }
  }

  return _connected = true;
}

void RedisClient::disconnect()
{
  mutex_lock lock(_mutex);
  if (_context)
  {
    redisFree(_context);
    _context = 0;
    if (_type == TCP)
    {
      OSS_LOG_INFO("[REDIS] Disconnecting from " << _tcpHost << ":" << _tcpPort << " - " << _lastError);
    }else if (_type == UNIX)
    {
      OSS_LOG_ERROR("[REDIS] Disconnecting from " << _unixSocketPath << " - " << _lastError);
    }
  }
  _connected = false;
}


void RedisClient::freeReply(redisReply* reply)
{
  if (reply)
    ::freeReplyObject(reply);
}

void RedisClient::vectorToCArray(const std::vector<std::string>& args, char*** argv)
{
  *argv = (char**)std::malloc((args.size() + 1) * sizeof(char*));
  int i=0;
  for(std::vector<std::string>::const_iterator iter = args.begin();
      iter != args.end();
      iter++, ++i)
  {
    std::string arg = *iter;
    (*argv)[i] = (char*)std::malloc((arg.length()+1) * sizeof(char));
    std::strcpy((*argv)[i], arg.c_str());
  }
  (*argv)[args.size()] = NULL; // argv must be NULL terminated
}

void RedisClient::freeCArray(int argc, char*** argv)
{
  for (int i = 0; i < argc; i++)
    free((*argv)[i]);
  free(*argv);
}

redisReply* RedisClient::execute(const std::vector<std::string>& args)
{
  char** argv;
  vectorToCArray(args, &argv);
  redisReply* reply = execute(args.size(), argv);
  freeCArray(args.size(), &argv);
  return reply;
}

redisReply* RedisClient::execute(int argc, char** argv)
{
  mutex_lock lock(_mutex);

  if (!_context)
  {
    OSS_LOG_WARNING("[REDIS] Context is NULL when calling execute.  Creating a new context.");
    if (!connect())
    {
      OSS_LOG_ERROR("[REDIS] Connect FAILED.  Unable to create a new context for execution.");
      return 0;
    }
  }

  if (!_context)
  {
    OSS_LOG_ERROR("[REDIS] Connect FAILED.  Unable to create a new context for execution.");
    return 0;
  }

  redisReply* reply = 0;
  reply = (redisReply*)redisCommandArgv(_context, argc, (const char**)argv, 0);

  if (_context->err)
  {
    _lastError = _context->errstr;
     if (_lastError.empty())
      _lastError = "Unknown exception";
    freeReply(reply);
    reply = 0;

    OSS_LOG_ERROR("[REDIS] Execute FAILED.  - " << _lastError);

    if (_context->err == REDIS_ERR_EOF || _context->err == REDIS_ERR_IO)
    {
      //
      // Try to reconnect
      //
      if (connect())
        reply = (redisReply*)redisCommandArgv(_context, argc, (const char**)argv, 0);

      if (_context->err)
      {
        _lastError = _context->errstr;
        if (_lastError.empty())
          _lastError = "Unknown exception";
        freeReply(reply);
        reply = 0;
        OSS_LOG_ERROR("[REDIS] Execute Retry FAILED.  - " << _lastError);
      }
    }
  }

  return reply;
}


std::string RedisClient::getReplyString(const std::vector<std::string>& args) const
{
  redisReply* reply = const_cast<RedisClient*>(this)->execute(args);
  std::string value;
  if (reply && (reply->type == REDIS_REPLY_STRING || reply->type == REDIS_REPLY_ERROR) && reply->len > 0)
  {
    value = std::string(reply->str, reply->len);
  }
  else if (reply && reply->type == REDIS_REPLY_INTEGER)
  {
    try
    {
      value = boost::lexical_cast<std::string>(reply->integer);
    }
    catch(...)
    {
    }
  }

  if (reply)
    const_cast<RedisClient*>(this)->freeReply(reply);

  return value;
}

bool RedisClient::getReplyInt(const std::vector<std::string>& args, long long& result) const
{
  redisReply* reply = const_cast<RedisClient*>(this)->execute(args);
  if (reply && reply->type == REDIS_REPLY_INTEGER)
  {
    try
    {
      result = reply->integer;
      const_cast<RedisClient*>(this)->freeReply(reply);
      return true;
    }
    catch(...)
    {
      return false;
    }
  }
  return false;
}

std::vector<std::string> RedisClient::getReplyStringArray(const std::vector<std::string>& args) const
{
  redisReply* reply = const_cast<RedisClient*>(this)->execute(args);
  std::vector<std::string> array;

  if (reply && reply->type == REDIS_REPLY_ARRAY && reply->elements > 0)
  {
    for (size_t i = 0; i < reply->elements; i++)
    {
      redisReply* item = reply->element[i];
      if (item && item->type == REDIS_REPLY_STRING && item->len > 0)
      {
        array.push_back(std::string(item->str, item->len));
      }
      else if (item && item->type == REDIS_REPLY_INTEGER)
      {
        try
        {
          array.push_back(boost::lexical_cast<std::string>(item->integer));
        }
        catch(...)
        {
        }
      }

    }
    const_cast<RedisClient*>(this)->freeReply(reply);
  }
  return array;
}

std::string RedisClient::getStatusString(const std::vector<std::string>& args) const
{
  redisReply* reply = const_cast<RedisClient*>(this)->execute(args);
  std::string value;
  if (reply && (reply->type == REDIS_REPLY_STATUS || reply->type == REDIS_REPLY_ERROR) && reply->len > 0)
  {
    value = std::string(reply->str, reply->len);
  }
  else
  {
    if (!reply)
    {
      OSS_LOG_ERROR("[REDIS] Redis::getStatusString - NULL reply received.");
    }
    else
    {
      if (!(reply->type == REDIS_REPLY_STATUS || reply->type == REDIS_REPLY_ERROR))
      {
         OSS_LOG_ERROR("[REDIS] Redis::getStatusString - Reply is of wrong type.  Expecting " << REDIS_REPLY_STATUS
           << " or " << REDIS_REPLY_ERROR << " but we got " << reply->type);
      }
      else if (reply->len <= 0)
      {
        OSS_LOG_ERROR("[REDIS] Redis::getStatusString - Empty reply received.");
      }
    }
  }

  if (reply)
    const_cast<RedisClient*>(this)->freeReply(reply);

  return value;
}


void RedisClient::execute(const std::vector<std::string>& args, std::ostream& strm) const
{
  redisReply* reply = const_cast<RedisClient*>(this)->execute(args);

  if (reply && (reply->type == REDIS_REPLY_STATUS || reply->type == REDIS_REPLY_STRING) && reply->len > 0)
  {
    strm << std::string(reply->str, reply->len);
  }
  else if (reply && reply->type == REDIS_REPLY_INTEGER)
  {
    strm << reply->integer;
  }
  else if (reply && reply->type == REDIS_REPLY_ARRAY && reply->elements > 0)
  {
    for (size_t i = 0; i < reply->elements; i++)
    {
      redisReply* item = reply->element[i];
      if (item && item->type == REDIS_REPLY_STRING && item->len > 0)
      {
        strm << std::string(item->str, item->len);
      }
      else if (item && item->type == REDIS_REPLY_INTEGER)
      {
        strm << item->integer;
      }

      strm << " ";
    }
  }
  else if (reply && reply->type == REDIS_REPLY_ERROR)
  {
    strm << std::string(reply->str, reply->len);
  }
  else if (_context->err)
  {
    strm << _context->errstr;
  }
  else if (!reply)
  {
    strm << "redis did not respond.";
  }

  if (reply)
    const_cast<RedisClient*>(this)->freeReply(reply);
}

bool RedisClient::set(const std::string& key, const json::Object& value, int expires)
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

bool RedisClient::set(const std::string& key, const std::string& value, int expires)
{
  std::vector<std::string> args;
  if (expires == -1)
  {
    args.push_back("SET");
    args.push_back(key);
    args.push_back(value);
  }
  else
  {
    try
    {
      args.push_back("SETEX");
      args.push_back(key);
      args.push_back(boost::lexical_cast<std::string>(expires));
      args.push_back(value);
    }
    catch(const std::exception& e)
    {
      OSS_LOG_ERROR("[REDIS] RedisClient::Set ERROR: " << e.what());
    }

  }
  std::string status = getStatusString(args);
  OSS::string_to_lower(status);
  if (!status.empty() && status != "ok")
  {
    OSS_LOG_ERROR("[REDIS]  RedisClient::Get ERROR: " << status);
  }

  return status == "ok";
}

bool RedisClient::incrby(const std::string& key, int increment, long long& result)
{
  try
  {
    std::vector<std::string> args;
    args.push_back("INCRBY");
    args.push_back(key);
    args.push_back(boost::lexical_cast<std::string>(increment));
    return getReplyInt(args, result);
  }
  catch(...)
  {
    return false;
  }
}

bool RedisClient::publish(const std::string& channel, const std::string& eventData)
{
  try
  {
    std::vector<std::string> args;
    args.push_back("PUBLISH");
    args.push_back(channel);
    args.push_back(eventData);
    long long result;
    if(getReplyInt(args, result) && result == 1)
    {
      return true;
    }
  }
  catch(...)
  {
    return false;
  }
  
  return false;
}

bool RedisClient::decrby(const std::string& key, int increment, long long& result)
{
  try
  {
    std::vector<std::string> args;
    args.push_back("DECRBY");
    args.push_back(key);
    args.push_back(boost::lexical_cast<std::string>(increment));
    return getReplyInt(args, result);
  }
  catch(...)
  {
    return false;
  }
}

bool RedisClient::hset(const std::string& key, const std::string& name, const std::string& value)
{
  std::vector<std::string> args;
  args.push_back("HSET");
  args.push_back(key);
  args.push_back(value);
  std::string status = getStatusString(args);
  return status == "0" || status == "1";
}

bool RedisClient::hincrby(const std::string& key, const std::string& name, int increment, long long& result)
{
  try
  {
    std::vector<std::string> args;
    args.push_back("HINCRBY");
    args.push_back(key);
    args.push_back(name);
    args.push_back(boost::lexical_cast<std::string>(increment));
    return getReplyInt(args, result);
  }
  catch(...)
  {
    return false;
  }
}

bool RedisClient::hmset(const std::string& key, const std::map<std::string, std::string>& hmap)
{
  std::vector<std::string> args;
  args.push_back("HMSET");

  for (std::map<std::string, std::string>::const_iterator iter = hmap.begin(); iter != hmap.end(); iter++)
  {
    args.push_back(iter->first);
    args.push_back(iter->second);
  }
  std::string status = getStatusString(args);
  return status == "0" || status == "1";
}

bool RedisClient::get(const std::string& key, json::Object& value) const
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
    OSS_LOG_ERROR("[REDIS] RedisClient::Get ERROR: " << error.what());
    return false;
  }
  return true;
}

bool RedisClient::get(const std::string& key, std::string& value) const
{
  std::vector<std::string> args;
  args.push_back("GET");
  args.push_back(key);
  value = getReplyString(args);
  return !value.empty();
}

bool RedisClient::getAll(std::vector<std::string>& values, const std::string& pattern)
{
  std::vector<std::string> keys;
  getKeys(pattern, keys);
  for (std::vector<std::string>::const_iterator iter = keys.begin(); iter != keys.end(); iter++)
  {
    std::string value;
    if (get(*iter, value))
      values.push_back(value);
  }

  return !values.empty();
}

bool RedisClient::getAll(std::vector<json::Object>& values, const std::string& pattern)
{
  std::vector<std::string> strValues;
  if (!getAll(strValues, pattern))
    return false;

  for (std::vector<std::string>::const_iterator iter = strValues.begin(); iter != strValues.end(); iter++)
  {
    try
    {
      std::stringstream strm;
      strm << *iter;
      json::Object value;
      json::Reader::Read(value, strm);
      values.push_back(value);
    }
    catch(std::exception& e)
    {
      OSS_LOG_ERROR("[REDIS] JSON Parser exception: " << e.what());
      return false;
    }
    catch(...)
    {
      OSS_LOG_ERROR("[REDIS] Unknown JSON Parser exception.");
      return false;
    }
  }
  return !values.empty();
}

bool RedisClient::hget(const std::string& key, std::string& value) const
{
  std::vector<std::string> args;
  args.push_back("HGET");
  args.push_back(key);
  value = getReplyString(args);
  return !value.empty();
}

bool RedisClient::hgetall(const std::string& key, std::vector<std::string>& value) const
{
  std::vector<std::string> args;
  args.push_back("HGETALL");
  args.push_back(key);
  value = getReplyStringArray(args);
  return !value.empty();
}

bool RedisClient::hmget(const std::string& key, const std::vector<std::string>& fields, std::vector<std::string>& value) const
{
  std::vector<std::string> args;
  args.push_back("HMGET");
  args.push_back(key);
  for (std::vector<std::string>::const_iterator iter = fields.begin(); iter != fields.end(); iter++)
    args.push_back(*iter);
  value = getReplyStringArray(args);
  return !value.empty();
}

bool RedisClient::getKeys(const std::string& pattern, std::vector<std::string>& keys)
{
  std::vector<std::string> args;
  args.push_back("KEYS");
  args.push_back(pattern);
  keys = getReplyStringArray(args);
  return !keys.empty();
}

bool RedisClient::del(const std::string& key)
{
  std::vector<std::string> args;
  args.push_back("DEL");
  args.push_back(key);
  long long result = 0;
  return getReplyInt(args, result) && result > 0;
}

bool RedisClient::hdel(const std::string& key, const std::string& field)
{
  std::vector<std::string> args;
  args.push_back("HDEL");
  args.push_back(key);
  args.push_back(field);
  long long result = 0;
  return getReplyInt(args, result) && result > 0;
}

RedisBroadcastClient::RedisBroadcastClient() :
  _defaultClient(0)
{
}

RedisBroadcastClient::~RedisBroadcastClient()
{
  disconnect();
  //
  // delete _defaultClient;   // disconnect() will delete the pointer
  //
}

bool RedisBroadcastClient::connect(const std::string& tcpHost, int tcpPort, const std::string& password, int db, bool allowReconnect)
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
        if (!allowReconnect)
        {
          delete _defaultClient;
          _defaultClient = 0;
          return false;
        }
        else
        {
          _defaultClient->disconnect();
          OSS_LOG_ERROR("[REDIS] Reconnect enabled for " << tcpHost << ":" << tcpPort);
        }
      }
      client = _defaultClient;
    }
    else
    {
      client = new RedisClient(tcpHost, tcpPort);
      if (!client->connect(password, db))
      {
        if (!allowReconnect)
        {
          delete client;
          return false;
        }
        else
        {
          client->disconnect();
          OSS_LOG_ERROR("[REDIS] Reconnect enabled for " << tcpHost << ":" << tcpPort);
        }
      }
    }
    _pool[key.str()] = client;
  }
  return true;
}

void RedisBroadcastClient::disconnect()
{
  for (Pool::iterator iter = _pool.begin(); iter != _pool.end(); iter++)
  {
    RedisClient* client = iter->second;
    client->disconnect();
    delete client;
  }
  _pool.clear();
}

bool RedisBroadcastClient::set(const std::string& key, const json::Object& value, int expires)
{
  for (Pool::iterator iter = _pool.begin(); iter != _pool.end(); iter++)
  {
    RedisClient* client = iter->second;
    client->set(key, value, expires);
  }
  return true;
}

bool RedisBroadcastClient::set(const std::string& key, const std::string& value, int expires)
{
  bool ok = false;
  for (Pool::iterator iter = _pool.begin(); iter != _pool.end(); iter++)
  {
    RedisClient* client = iter->second;
    ok ? client->set(key, value, expires) : ok = client->set(key, value, expires);
  }
  return ok;
}

bool RedisBroadcastClient::hset(const std::string& key, const std::string& name, const std::string& value)
{
  bool ok = false;
  for (Pool::iterator iter = _pool.begin(); iter != _pool.end(); iter++)
  {
    RedisClient* client = iter->second;
    ok ? client->hset(key, name, value) : ok = client->hset(key, name, value);
  }
  return ok;
}

bool RedisBroadcastClient::get(const std::string& key, json::Object& value) const
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
        continue;
    }
    catch(...)
    {
      continue;
    }
  }

  return false;
}

bool RedisBroadcastClient::get(const std::string& key, std::string& value) const
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
        continue;
    }
    catch(...)
    {
      continue;
    }
  }

  return false;
}

bool RedisBroadcastClient::getKeys(const std::string& pattern, std::vector<std::string>& keys)
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
        continue;
    }
    catch(...)
    {
      continue;
    }
  }

  return false;
}

bool RedisBroadcastClient::del(const std::string& key)
{
  for (Pool::iterator iter = _pool.begin(); iter != _pool.end(); iter++)
  {
    RedisClient* client = iter->second;
    client->del(key);
  }
  return true;
}
  
} } // OSS::Persistent





