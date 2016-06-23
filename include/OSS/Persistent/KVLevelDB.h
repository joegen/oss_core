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


#ifndef KVLEVELDB_H_INLCUDED
#define	KVLEVELDB_H_INLCUDED

#include "OSS/OSS.h"
#include "OSS/build.h"


#if OSS_HAVE_LEVELDB

#include <leveldb/db.h>

#include "OSS/UTL/Thread.h"
#include "OSS/Persistent/KeyValueStoreBase.h"


namespace OSS {
namespace Persistent {


class KVLevelDB : boost::noncopyable
{
public: 
  typedef KVRecord Record;
  typedef KVKeys Keys;
  typedef KVRecords Records;
  
  KVLevelDB();
  
  ~KVLevelDB();

  bool open(const std::string& path);

  bool isOpen();

  bool close();

  bool put(const std::string& key, const std::string& value);

  bool get(const std::string& key, std::string& value);
  
  bool del(const std::string& key);
  
  bool getKeys(Keys& keys);
  
  bool getKeys(const std::string& filter, Keys& keys);
  
  bool getRecords(Records& records);
  
  bool getRecords(const std::string& filter, Records& records);
  
  bool delKeys(const std::string& filter);
  
  const std::string getPath() const;
  
  const std::string& getKeyPrefix() const;
  
  void setKeyPrefix(const std::string& keyPrefix);
private:
  leveldb::DB* _pDb;
  std::string _path;
  std::string _keyPrefix;
};


//
// Inlines
//

inline const std::string& KVLevelDB::getKeyPrefix() const
{
  return _keyPrefix;
}
  
inline void KVLevelDB::setKeyPrefix(const std::string& keyPrefix)
{
  _keyPrefix = keyPrefix;
}

} } // OSS::Persistent

#endif //OSS_HAVE_LEVELDB

#endif	// KVLEVELDB_H_INLCUDED

