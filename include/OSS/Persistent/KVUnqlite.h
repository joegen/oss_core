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


#ifndef KVUNQLITE_H_INLCUDED
#define	KVUNQLITE_H_INLCUDED


#include "OSS/OSS.h"
#include "OSS/Thread.h"
#include "OSS/Persistent/KeyValueStoreBase.h"


namespace OSS {
namespace Persistent {


class KVUnqlite : boost::noncopyable
{
public: 
  typedef KVRecord Record;
  typedef KVKeys Keys;
  typedef KVRecords Records;
  
  KVUnqlite();
  
  KVUnqlite(const std::string& path);

  ~KVUnqlite();

  bool open(const std::string& path);

  bool isOpen();

  bool close();

  bool put(const std::string& key, const std::string& value);

  bool put(const std::string& key, const std::string& value, unsigned int expireInSeconds);

  bool get(const std::string& key, std::string& value);

  bool del(const std::string& key);
  
  bool getKeys(Keys& keys);
  
  bool getKeys(const std::string& filter, Keys& keys);
  
  bool getRecords(Records& records);
  
  bool getRecords(const std::string& filter, Records& records);
  
  bool delKeys(const std::string& filter);
  
  const std::string getPath() const;
private:

  void log_error();

  bool is_expired(const std::string& key);

  bool purge_expired(const std::string& key);

  bool _get(const std::string& key, std::string& value);

  OSS_HANDLE _pDbHandle;

  std::string _path;

  OSS::mutex_read_write _mutex;
};


//
// Inlines
//

inline bool KVUnqlite::isOpen()
{
  return _pDbHandle != 0;
}

inline const std::string KVUnqlite::getPath() const
{
  return _path;
}

inline bool KVUnqlite::getKeys(Keys& keys)
{
  return getKeys("", keys);
}

inline bool KVUnqlite::getRecords(Records& records)
{
  return getRecords("", records);
}

} } // OSS::Persistent

#endif	// KVUNQLITE_H_INLCUDED

