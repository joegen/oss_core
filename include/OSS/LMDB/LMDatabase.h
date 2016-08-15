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


#ifndef OSS_LMDATABASE_H_INCLUDED
#define	OSS_LMDATABASE_H_INCLUDED

#include "OSS/OSS.h"
#include "OSS/UTL/Thread.h"
#include <boost/noncopyable.hpp>
#include <string>
#include <ostream>


namespace OSS {
namespace LMDB {

  
#define LMDB_SIZE_MB 50

class LMDatabase : boost::noncopyable
{
public:
  struct Options
  {
    Options()
    {
      size_mb = LMDB_SIZE_MB;
      env_flags = 0;
    }
    
    std::size_t size_mb;
    std::string name;
    std::string path;
    int env_flags;
  };
  
  class Transaction : boost::noncopyable
  {
  public:
    Transaction(LMDatabase* db);
    ~Transaction();
    inline LMDatabase* db() { return _db; }
    inline void* transaction() { return _transaction; };
    
    bool begin();
    bool end();
  private:
    LMDatabase* _db;
    void* _transaction;
  };
  
  class TransactionLock : boost::noncopyable
  {
  public:
    TransactionLock(Transaction& transaction);
    ~TransactionLock();
    inline Transaction& transaction() {return _transaction; }
  private:
    Transaction& _transaction;
  };
  
  class CursorImpl : boost::noncopyable
  {
  public:
    typedef boost::shared_ptr<CursorImpl> Ptr;
    
    ~CursorImpl();
    
    bool top();
    bool find(const std::string& key);
    bool next();
    bool bottom();
    std::string value() const;
    std::string key() const;
    void destroy();
    
  protected:
    friend class LMDatabase;
    CursorImpl(LMDatabase& db, LMDatabase::Transaction& transaction);
    void* _cursor;
    LMDatabase& _db;
    Transaction& _transaction;
    std::string _key;
    std::string _value;    
  };
  
  typedef CursorImpl::Ptr Cursor;
  
  
  LMDatabase();
  ~LMDatabase();
  
  bool initialize(const Options& opt);
  const LMDatabase::Options& opt() const;
  void printStats(Transaction& transaction, std::ostream& strm);
  
  bool set(Transaction& transaction, const std::string& key, const std::string& value);
  bool set(Transaction& transaction, const std::string& key, bool value);
  bool set(Transaction& transaction, const std::string& key, double value);
  bool set(Transaction& transaction, const std::string& key, int32_t value);
  bool set(Transaction& transaction, const std::string& key, int64_t value);
  bool set(Transaction& transaction, const std::string& key, void* value, std::size_t len);
  
  bool get(Transaction& transaction, const std::string& key, std::string& value);
  bool get(Transaction& transaction, const std::string& key, bool& value);
  bool get(Transaction& transaction, const std::string& key, double& value);
  bool get(Transaction& transaction, const std::string& key, int32_t& value);
  bool get(Transaction& transaction, const std::string& key, int64_t& value);
  bool get(Transaction& transaction, const std::string& key, void** value, std::size_t& len);
  
  bool del(Transaction& transaction, const std::string& key);
  
  std::size_t count(Transaction& transaction);
  
  void stop();
  
  Cursor createCursor(Transaction& transaction);
  
protected:
  friend class Transaction;
  friend class TransactionLock;
  friend class CursorImpl;
  void* _env;
  void* _db;
  Options _opt;
  OSS::mutex _mutex;
  bool _stopped;
};
  
//
// Inlines
//

inline const LMDatabase::Options& LMDatabase::opt() const
{
  return _opt;
}

inline bool LMDatabase::set(Transaction& transaction, const std::string& key, const std::string& value)
{
  return set(transaction, key, (void*)value.data(), value.size());
}

inline bool LMDatabase::set(Transaction& transaction, const std::string& key, bool value)
{
  return set(transaction, key, (void*)&value, sizeof(bool));
}

inline bool LMDatabase::set(Transaction& transaction, const std::string& key, double value)
{
  return set(transaction, key, (void*)&value, sizeof(double));
}

inline bool LMDatabase::set(Transaction& transaction, const std::string& key, int32_t value)
{
  return set(transaction, key, (void*)&value, sizeof(int32_t));
}

inline bool LMDatabase::set(Transaction& transaction, const std::string& key, int64_t value)
{
  return set(transaction, key, (void*)&value, sizeof(int64_t));
}

} } // OSS::LMDB


#endif	// OSS_LMDATABASE_H_INCLUDED

