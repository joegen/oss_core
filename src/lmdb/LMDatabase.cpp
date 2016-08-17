
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


#include "OSS/LMDB/LMDatabase.h"
#include "OSS/LMDB/liblmdb.h"
#include <boost/filesystem.hpp>
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/Exception.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace LMDB {


typedef LMDatabase::TransactionLock transaction_lock;  
 
LMDatabase::Transaction::Transaction(LMDatabase* db) :
  _db(db),
  _transaction(0),
  _cancelAdvised(false),
  _lastError(0)
{
  assert(_db && _db->_env);
}

LMDatabase::Transaction::~Transaction()
{
}

bool LMDatabase::Transaction::begin()
{
  _db->_mutex.lock();
  if (_db->_stopped)
  {
    _db->_mutex.unlock();
    return false;
  }
  assert(!_transaction);
  return mdb_txn_begin((MDB_env*)_db->_env, 0, 0, (MDB_txn**)&_transaction) == 0;
}

bool LMDatabase::Transaction::end()
{
  assert(_transaction);
  int ret = 0;
  ret = mdb_txn_commit((MDB_txn*)_transaction);
  _transaction = 0;
  //
  // TODO:  Issue a warning if cancel was advised
  //
  _cancelAdvised = false;
  _db->_mutex.unlock();
  return ret == 0;
}

void LMDatabase::Transaction::cancel()
{
  assert(_transaction);
  mdb_txn_abort((MDB_txn*)_transaction);
  _transaction = 0;
  _cancelAdvised = false;
  _db->_mutex.unlock();
}

LMDatabase::TransactionLock::TransactionLock(Transaction& transaction) :
  _transaction(transaction)
{
  if (!_transaction.begin())
  {
    throw OSS::Exception("TransactionLock - begin transaction failed");
  }
}

LMDatabase::TransactionLock::~TransactionLock()
{
  if (_transaction.cancelAdvised())
  {
    _transaction.cancel();
  }
  else if (!_transaction.end())
  {
    throw OSS::Exception("TransactionLock - end transaction failed");
  }
}

LMDatabase::Cursor::Cursor() :
  _cursor(0),
  _db(0)
{
}

LMDatabase::Cursor::~Cursor()
{
  destroy();
}

bool LMDatabase::Cursor::create(LMDatabase* db, LMDatabase::Transaction* transaction)
{
  assert(transaction->transaction());
  if (mdb_cursor_open((MDB_txn*)transaction->transaction(), *((MDB_dbi*)db->_db), (MDB_cursor**)&_cursor) != 0)
  {
    destroy();
    return false;
  }
  
  return true;
}
void LMDatabase::Cursor::destroy()
{
  mdb_cursor_close((MDB_cursor*)_cursor);
  _cursor = 0;
}

static bool lmdb_cursor_get(MDB_cursor* cursor, MDB_cursor_op op, std::string& key, std::string& value)
{
  MDB_val k, v;
  
  if (mdb_cursor_get(cursor, &k, &v, op) == 0 && 
    k.mv_data && k.mv_size && v.mv_data && v.mv_size)
  {
    key = std::string((char*)k.mv_data, k.mv_size);
    value = std::string((char*)v.mv_data, v.mv_size);
    return true;
  }
  
  key = std::string();
  value = std::string();
  return false;
}

bool LMDatabase::Cursor::top()
{
  if (!_cursor)
  {
    return false;
  }
  return lmdb_cursor_get((MDB_cursor*)_cursor, MDB_FIRST, _key, _value);
}

bool LMDatabase::Cursor::find(const std::string& key)
{
  if (!_cursor)
  {
    return false;
  }
  
  MDB_val k, v;
  k.mv_data = (void*)key.data();
  k.mv_size = key.size();
  
  if (mdb_cursor_get((MDB_cursor*)_cursor, &k, &v, MDB_SET) == 0 && 
    k.mv_data && k.mv_size && v.mv_data && v.mv_size)
  {
    _key = std::string((char*)k.mv_data, k.mv_size);
    _value = std::string((char*)v.mv_data, v.mv_size);
    return true;
  }
  
  _key = std::string();
  _value = std::string();
  return false;
}

bool LMDatabase::Cursor::next()
{
  if (!_cursor)
  {
    return false;
  }
  return lmdb_cursor_get((MDB_cursor*)_cursor, MDB_NEXT, _key, _value);
}

bool LMDatabase::Cursor::bottom()
{
  if (!_cursor)
  {
    return false;
  }
  return lmdb_cursor_get((MDB_cursor*)_cursor, MDB_LAST, _key, _value);
}

std::string LMDatabase::Cursor::value() const
{
  if (!_cursor)
  {
    return std::string();
  }
  return _value;
}

std::string LMDatabase::Cursor::key() const
{
  if (!_cursor)
  {
    return std::string();
  }
  return _key;
}


    

    
  
  
LMDatabase::LMDatabase() :
  _env(0),
  _db(0),
  _stopped(false)
{
  _db = malloc(sizeof(MDB_dbi));
}

LMDatabase::~LMDatabase()
{
  close();
}

void LMDatabase::close()
{
  _mutex.lock();
  if (!_stopped)
  {
    _stopped = true;
    if (_env)
    {
      mdb_dbi_close((MDB_env*)_env, *((MDB_dbi*)_db));
      mdb_env_close((MDB_env*)_env);
    }
    _env = 0;
    free(_db);
    _db = 0;
  }
  _mutex.unlock();
}

bool LMDatabase::drop(Transaction& transaction)
{
  assert(transaction.transaction());
  return mdb_drop((MDB_txn*)transaction.transaction(), *((MDB_dbi*)_db), 1) == 0;
}
  
bool LMDatabase::clear(Transaction& transaction)
{
  assert(transaction.transaction());
  return mdb_drop((MDB_txn*)transaction.transaction(), *((MDB_dbi*)_db), 0) == 0;
}

bool LMDatabase::initialize(const Options& opt)
{
  if (opt.name.empty())
  {
    return false;
  }
  
  
  if (!opt.size_mb)
  {
    return false;
  }
  
  
  boost::filesystem::path dir(opt.path.empty()? "./" : opt.path.c_str());
  std::string resolvedPath = OSS::boost_path(dir);
  
  if (!boost::filesystem::exists(dir))
  {
    boost::filesystem::create_directories(dir);
    if (!boost::filesystem::exists(dir))
    {
      return false;
    }
  }
  
  int err = 0;
  
  err = mdb_env_create((MDB_env**)&_env);
  if (err != 0 || !_env)
  {
    return false;
  }
    
  err = mdb_env_set_mapsize((MDB_env*)_env, opt.size_mb * 1024 * 1024);
  if (err != 0)
  {
    return false;
  }
  
  err = mdb_env_set_maxdbs((MDB_env*)_env, 1024);
  if (err != 0)
  {
    return false;
  }
  
  err = mdb_env_open((MDB_env*)_env, resolvedPath.c_str(), opt.env_flags, 0664);
  if (err != 0)
  {
    return false;
  }
  
  Transaction transaction(this);
  transaction_lock lock(transaction);
  err = mdb_dbi_open((MDB_txn*)transaction.transaction(), opt.name.c_str(), MDB_CREATE, (MDB_dbi*)_db);
  if (err != 0)
  {
    return false;
  }
  return true;
}

void LMDatabase::printStats(Transaction& transaction, std::ostream& strm)
{ 
  assert(transaction.transaction());
   
  MDB_env* env = (MDB_env*)_env;
  MDB_dbi* dbi = (MDB_dbi*)_db;
  MDB_stat stat = {};
  MDB_envinfo info = {};
  
  if (mdb_stat((MDB_txn*)transaction.transaction(), *dbi, &stat) == 0 && mdb_env_info(env, &info) == 0)
  {
    strm << "ms_psize: " << stat.ms_psize << std::endl;
    strm << "ms_depth: " << stat.ms_depth << std::endl;
    strm << "ms_branch_pages: " << stat.ms_branch_pages << std::endl;
    strm << "ms_leaf_pages: " << stat.ms_leaf_pages << std::endl;
    strm << "ms_overflow_pages: " << stat.ms_overflow_pages << std::endl;
    strm << "ms_entries: " << stat.ms_entries << std::endl;
    strm << "me_mapsize: " << info.me_mapsize;
  }
}

bool LMDatabase::set(Transaction& transaction, const std::string& key, void* value, std::size_t len)
{
  MDB_dbi* dbi = (MDB_dbi*)_db;
  int ret = 0;
  
  MDB_val k, v;
  k.mv_size = strlen(key.c_str());
  k.mv_data = (void*)key.data();
  
  v.mv_size = len;
  v.mv_data = value;
    
  ret = mdb_put((MDB_txn*)transaction.transaction(), *dbi, &k, &v, 0);
  if (ret != 0)
  {
    transaction.lastError() = ret;
    transaction.cancelAdvised() = true;
  }
  
  return ret == 0;
}

bool LMDatabase::get(Transaction& transaction, const std::string& key, void** value, std::size_t& len)
{
  assert(transaction.transaction());
  
  MDB_val k, v;
  k.mv_size = strlen(key.c_str());
  k.mv_data = (void*)key.data();
  
  if (mdb_get((MDB_txn*)transaction.transaction(), *(MDB_dbi*)_db, &k, &v) == 0 && v.mv_data && v.mv_size)
  {
      *value = v.mv_data;
      len = v.mv_size;
      return true;
  }
  return false;
}

bool LMDatabase::get(Transaction& transaction, const std::string& key, std::string& value)
{
  void* data = 0;
  std::size_t len = 0;
  if (get(transaction, key, &data, len))
  {
    value = std::string((const char*)data, len);
    return true;
  }
  return false;
}

template <typename T>
bool lmdb_get_value(LMDatabase::Transaction& transaction, LMDatabase* db, const std::string& key, T& value)
{
  void* data = 0;
  std::size_t len = 0;
  if (db->get(transaction, key, &data, len) && len == sizeof(T))
  {
    value = (T) (*((T*)data));
    return true;
  }
  return false;
}

bool LMDatabase::get(Transaction& transaction, const std::string& key, bool& value)
{
  return lmdb_get_value<bool>(transaction, this, key, value);
}

bool LMDatabase::get(Transaction& transaction, const std::string& key, double& value)
{
  return lmdb_get_value<double>(transaction, this, key, value);
}

bool LMDatabase::get(Transaction& transaction, const std::string& key, int32_t& value)
{
  return lmdb_get_value<int32_t>(transaction, this, key, value);
}

bool LMDatabase::get(Transaction& transaction, const std::string& key, int64_t& value)
{
  return lmdb_get_value<int64_t>(transaction, this, key, value);
}

bool LMDatabase::del(Transaction& transaction, const std::string& key)
{
  assert(transaction.transaction());
  MDB_val k, v;
  int ret = 0;
  k.mv_size = strlen(key.c_str());
  k.mv_data = (void*)key.data();
  ret = mdb_del((MDB_txn*)transaction.transaction(), *((MDB_dbi*)_db), &k, &v);
  
  if (ret != 0)
  {
    transaction.lastError() = ret;
    transaction.cancelAdvised() = true;
  }
  return ret == 0;
}

std::size_t LMDatabase::count(Transaction& transaction)
{
  assert(transaction.transaction());
  MDB_dbi* dbi = (MDB_dbi*)_db;
  MDB_stat stat = {};
  if (mdb_stat((MDB_txn*)transaction.transaction(), *dbi, &stat) == 0 )
  {
    return stat.ms_entries;
  }
  return 0;
}

bool LMDatabase::createCursor(Transaction& transaction, Cursor& cursor)
{
  return cursor.create(this, &transaction);
}

  
} } // OSS::LMDB




