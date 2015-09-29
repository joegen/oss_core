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

#ifndef OSS_REDISCLIENT_H_INCLUDED
#define	OSS_REDISCLIENT_H_INCLUDED

#include "OSS/UTL/Logger.h"
#include "hiredis/hiredis.h"
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/JSON/elements.h"
#include "OSS/UTL/CoreUtils.h"
#include <map>


namespace OSS {

namespace Persistent {


class RedisClient : boost::noncopyable
{
public:
  typedef boost::recursive_mutex mutex;
  typedef boost::lock_guard<mutex> mutex_lock;

  struct ConnectionInfo
  {
    std::string host;
    std::string password;
    unsigned short port;
  };

  enum Type
  {
    TCP,
    UNIX
  };

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
  bool _connected;
  
public:
  RedisClient();

  RedisClient(const std::string& tcpHost, int tcpPort);

  RedisClient(const std::string& unixSocketPath);

  ~RedisClient();

  bool connect(const std::string& password_ = "", int db = 0);

  void disconnect();

protected:
  void freeReply(redisReply* reply);

  static void vectorToCArray(const std::vector<std::string>& args, char*** argv);

  static void freeCArray(int argc, char*** argv);

  redisReply* execute(const std::vector<std::string>& args);

  redisReply* execute(int argc, char** argv);
  
  std::string getReplyString(const std::vector<std::string>& args) const;

  bool getReplyInt(const std::vector<std::string>& args, long long& result) const;

  std::vector<std::string> getReplyStringArray(const std::vector<std::string>& args) const;

  std::string getStatusString(const std::vector<std::string>& args) const;
  
public:
  void execute(const std::vector<std::string>& args, std::ostream& strm) const;
  
  bool set(const std::string& key, const json::Object& value, int expires = -1);

  bool set(const std::string& key, const std::string& value, int expires = -1);
  
  bool incrby(const std::string& key, int increment, long long& result);
  
  bool decrby(const std::string& key, int increment, long long& result);

  bool hset(const std::string& key, const std::string& name, const std::string& value);

  bool hincrby(const std::string& key, const std::string& name, int increment, long long& result);

  bool hmset(const std::string& key, const std::map<std::string, std::string>& hmap);

  bool get(const std::string& key, json::Object& value) const;

  bool get(const std::string& key, std::string& value) const;
  
  bool getAll(std::vector<std::string>& values, const std::string& pattern = "*");

  bool getAll(std::vector<json::Object>& values, const std::string& pattern = "*");

  bool hget(const std::string& key, std::string& value) const;

  bool hgetall(const std::string& key, std::vector<std::string>& value) const;

  bool hmget(const std::string& key, const std::vector<std::string>& fields, std::vector<std::string>& value) const;

  bool getKeys(const std::string& pattern, std::vector<std::string>& keys);

  bool del(const std::string& key);

  bool hdel(const std::string& key, const std::string& field);
  
  bool publish(const std::string& channel, const std::string& eventData);
  
  bool receive(std::vector<std::string>& reply) const;
  
  bool subscribe(const std::string& channelName, std::vector<std::string>& reply);
};

class RedisBroadcastClient
{
public:
  typedef std::map<std::string, RedisClient*> Pool;

  RedisBroadcastClient();

  ~RedisBroadcastClient();

  bool connect(const std::string& tcpHost, int tcpPort, const std::string& password = "", int db = 0, bool allowReconnect = true);

  void disconnect();

  bool set(const std::string& key, const json::Object& value, int expires = -1);

  bool set(const std::string& key, const std::string& value, int expires = -1);

  bool hset(const std::string& key, const std::string& name, const std::string& value);

  bool get(const std::string& key, json::Object& value) const;

  bool get(const std::string& key, std::string& value) const;

  bool getKeys(const std::string& pattern, std::vector<std::string>& keys);

  bool del(const std::string& key);
  
protected:
  Pool _pool;
  RedisClient* _defaultClient;
};

} } // OSS::Persistent


#endif	/* OSS_REDISCLIENT_H_INCLUDED */

