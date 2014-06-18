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


#include "OSS/Persistent/KVLevelDB.h"


namespace OSS {
namespace Persistent {

 
KVLevelDB::KVLevelDB() :
  _pDb(0)
{
  
}
  
KVLevelDB::~KVLevelDB()
{
  close();
}

bool KVLevelDB::open(const std::string& path)
{
  _path = path;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, path, &_pDb);
  return status.ok();
}

bool KVLevelDB::isOpen()
{
  return _pDb != 0;
}

bool KVLevelDB::close()
{
  delete _pDb;
  _pDb = 0;
  return true;
}

bool KVLevelDB::put(const std::string& key, const std::string& value)
{
  return _pDb->Put(leveldb::WriteOptions(), key, value).ok();
}

bool KVLevelDB::get(const std::string& key, std::string& value)
{
  return _pDb->Get(leveldb::ReadOptions(), key, &value).ok();
}

bool KVLevelDB::del(const std::string& key)
{
  return _pDb->Delete(leveldb::WriteOptions(), key).ok();
}

bool KVLevelDB::getKeys(Keys& keys)
{
  leveldb::Iterator* it = _pDb->NewIterator(leveldb::ReadOptions());
  for (it->SeekToFirst(); it->Valid(); it->Next()) 
  {
    std::string key = it->key().ToString();
    keys.push_back(key);
  }
  
  bool status = it->status().ok();
  delete it;
  return status;
}

bool KVLevelDB::getKeys(const std::string& filter, Keys& keys)
{
  if (filter == "*" || filter.empty())
    return getKeys(keys);
  
  std::string startKey = OSS::string_left(filter, filter.size() - 1);
  
  leveldb::Iterator* it = _pDb->NewIterator(leveldb::ReadOptions());
  for (it->Seek(startKey); it->Valid(); it->Next()) 
  {
    std::string key = it->key().ToString();
    bool validKey = true;
    validKey = OSS::string_wildcard_compare(filter.c_str(), key);

    if (validKey)
      keys.push_back(key);
    else
      break;
  }
  
  bool status = it->status().ok();
  delete it;
  return status;
}

bool KVLevelDB::getRecords(Records& records)
{
  leveldb::Iterator* it = _pDb->NewIterator(leveldb::ReadOptions());
  for (it->SeekToFirst(); it->Valid(); it->Next()) 
  {
    Record record;
    record.key = it->key().ToString();
    record.value = it->value().ToString();
    records.push_back(record);
  }
  
  bool status = it->status().ok();
  delete it;
  return status;
}

bool KVLevelDB::getRecords(const std::string& filter, Records& records)
{
  if (filter == "*" || filter.empty())
    return getRecords(records);
  
  std::string key = OSS::string_left(filter, filter.size() - 1);
  
  leveldb::Iterator* it = _pDb->NewIterator(leveldb::ReadOptions());
  for (it->Seek(key); it->Valid(); it->Next()) 
  {
    Record record;
    record.key = it->key().ToString();
    bool validKey = true;
    if (!filter.empty() && filter != "*")
      validKey = OSS::string_wildcard_compare(filter.c_str(), record.key);

    if (validKey)
    {
      record.value = it->value().ToString();
      records.push_back(record);
    }
    else
    {
      break;
    }
  }
  
  bool status = it->status().ok();
  delete it;
  return status;
}

bool KVLevelDB::delKeys(const std::string& filter)
{
  Keys keys;
  if (!getKeys(filter, keys))
  return false;
  
  for (Keys::const_iterator iter= keys.begin(); iter != keys.end(); iter++)
    del(*iter);
  
  return true;
}

const std::string KVLevelDB::getPath() const
{
  return _path;
}



} } // OSS::Persistent



