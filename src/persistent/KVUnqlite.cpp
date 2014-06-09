
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


#include "OSS/Persistent/KVUnqlite.h"
#include "OSS/Logger.h"
#include "OSS/Core.h"


extern "C"
{
  #include "unqlite/unqlite.h"
}


#define PERSISTENT_STORE_MAX_VALUE_SIZE 65536
#define PERSISTENT_STORE_MAX_KEY_SIZE 1024
static const std::string PERSISTENT_STORE_EXPIRES_SUFFIX = ".KV_EXPIRES";


namespace OSS {
namespace Persistent {

  
struct KeyConsumer
{
  std::string filter;
  KVUnqlite::Keys* keys;
};

struct RecordConsumer
{
  std::string filter;
  std::string key;
  KVUnqlite::Records* records;
};

static int RecordConsumerCallback(const void *pData,unsigned int nDatalen,void *pUserData)
{
  RecordConsumer* pConsumer = static_cast<RecordConsumer*>(pUserData);
  
  if (pConsumer && nDatalen && pData)
  {
    std::string data((const char*)pData, nDatalen);
    
    if (pConsumer->key.empty())
    {
      //
      // There is no key yet so we assume this is a key assignment
      //
      if (!pConsumer->filter.empty() && pConsumer->filter != "*")
      {
        if (!OSS::string_wildcard_compare(pConsumer->filter.c_str(), data))
        {
          return UNQLITE_OK;
        }
      }
      
      //
      // Do not process expiration keys
      //
      if (!OSS::string_ends_with(pConsumer->key, PERSISTENT_STORE_EXPIRES_SUFFIX.c_str()))
      {
        pConsumer->key = data;
      }
      else
      {
        return UNQLITE_OK;
      }
    }
    else
    {
      //
      // There is a key.  This is a data assignment
      //
      KVUnqlite::Record record;
      record.key = pConsumer->key;
      record.value = data;
      pConsumer->records->push_back(record);
      pConsumer->key.clear();
    }
  }
  
  return UNQLITE_OK;
}

static int KeyConsumerCallback(const void *pData,unsigned int nDatalen,void *pUserData)
{
  KeyConsumer* pConsumer = static_cast<KeyConsumer*>(pUserData);
  
  if (pConsumer && nDatalen && pData)
  {
    std::string data((const char*)pData, nDatalen);
    
    if (!pConsumer->filter.empty() && pConsumer->filter != "*")
    {
      if (!OSS::string_wildcard_compare(pConsumer->filter.c_str(), data))
        return UNQLITE_OK;
    }
    
    //
    // Do not process expiration keys
    //
    if (!OSS::string_ends_with(data, PERSISTENT_STORE_EXPIRES_SUFFIX.c_str()))
    {
      pConsumer->keys->push_back(data);
    }
    else
    {
      return UNQLITE_OK;
    }
  }
  
  return UNQLITE_OK;
}

KVUnqlite::KVUnqlite() :
  _pDbHandle(0)
{
}

KVUnqlite::KVUnqlite(const std::string& path) :
  _pDbHandle(0)
{
  open(path);
}

KVUnqlite::~KVUnqlite()
{
  close();
}

void KVUnqlite::log_error()
{
  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
    return;

  const char *zBuf = 0;
  int iLen;
  unqlite_config(pDbHandle,UNQLITE_CONFIG_ERR_LOG,&zBuf,&iLen);
  if( iLen > 0 )
  {
    OSS_LOG_ERROR("KVUnqlite Exception:  " << zBuf);
  }
}

bool KVUnqlite::close()
{
  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (pDbHandle)
  {
    if (unqlite_close(pDbHandle) == UNQLITE_OK)
    {
      _pDbHandle = 0;
      return true;
    }
    else
    {
      log_error();
      return false;
    }
  }
  return true;
}

bool KVUnqlite::open(const std::string& path)
{
  if (isOpen())
    return false;

  _path = path;

  unqlite* pDbHandle = 0;

  if (unqlite_open(&pDbHandle, path.c_str(), UNQLITE_OPEN_CREATE | UNQLITE_OPEN_READWRITE) != UNQLITE_OK || !pDbHandle)
  {
    log_error();
    return false;
  }

  _pDbHandle = pDbHandle;

  return true;
}

bool KVUnqlite::put(const std::string& key, const std::string& value)
{
  if (key.size() > PERSISTENT_STORE_MAX_KEY_SIZE || value.size() > PERSISTENT_STORE_MAX_VALUE_SIZE)
  {
    OSS_LOG_ERROR("KVUnqlite::put:  Maximum data/ size exceeded");
    return false;
  }
  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
    return false;

  OSS::mutex_write_lock lock(_mutex);

 // if (unqlite_begin(pDbHandle) != UNQLITE_OK)
 // {
 //   log_error();
 //   return false;
 // }

  if (unqlite_kv_store(pDbHandle, key.c_str(), key.size(), value.c_str(), value.size()) != UNQLITE_OK)
  {
    log_error();
   // unqlite_rollback(pDbHandle);
    return false;
  }
  
  //unqlite_commit(pDbHandle);
    
  return true;
}

bool KVUnqlite::put(const std::string& key, const std::string& value, unsigned int expireInSeconds)
{
  if (key.size() > PERSISTENT_STORE_MAX_KEY_SIZE || value.size() > PERSISTENT_STORE_MAX_VALUE_SIZE)
  {
    OSS_LOG_ERROR("KVUnqlite::put:  Maximum data/ size exceeded");
    return false;
  }

  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
    return false;

  OSS::mutex_write_lock lock(_mutex);

 // if (unqlite_begin(pDbHandle) != UNQLITE_OK)
 // {
 //   log_error();
 //   return false;
 // }

  if (unqlite_kv_store(pDbHandle, key.c_str(), key.size(), value.c_str(), value.size()) != UNQLITE_OK)
  {
    log_error();
  //  unqlite_rollback(pDbHandle);
    return false;
  }

  OSS::UInt64 expires = OSS::getTime() + (expireInSeconds*1000);
  std::string expireString = OSS::string_from_number(expires);
  std::string expireKey = key + PERSISTENT_STORE_EXPIRES_SUFFIX;

  if (unqlite_kv_store(pDbHandle, expireKey.c_str(), expireKey.size(), expireString.c_str(), expireString.size()) != UNQLITE_OK)
  {
    log_error();
  //  unqlite_rollback(pDbHandle);
    return false;
  }

  // unqlite_commit(pDbHandle);

  return true;
}

bool KVUnqlite::get(const std::string& key, std::string& value)
{
  if (is_expired(key))
  {
    purge_expired(key);
    return false;
  }
    
  return _get(key, value);
}

bool KVUnqlite::_get(const std::string& key, std::string& value)
{
  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
  {
    return false;
  }

  OSS::mutex_read_lock lock(_mutex);

  char buff[PERSISTENT_STORE_MAX_VALUE_SIZE];

  unqlite_int64 nBytes = PERSISTENT_STORE_MAX_VALUE_SIZE;
  if (unqlite_kv_fetch(pDbHandle, key.c_str(),key.size(), buff, &nBytes) != UNQLITE_OK)
  {
    log_error();
    return false;
  }

  value = std::string(buff, nBytes);

  return true;
}

bool KVUnqlite::is_expired(const std::string& key)
{
  std::string expireKey = key + PERSISTENT_STORE_EXPIRES_SUFFIX;

  std::string expires;
  if (!_get(expireKey, expires))
    return false;

  OSS::UInt64 expireTime = OSS::string_to_number<OSS::UInt64>(expires);
  
  return expireTime <= OSS::getTime();
}

bool KVUnqlite::purge_expired(const std::string& key)
{
  std::string expireKey = key + PERSISTENT_STORE_EXPIRES_SUFFIX;
  if (!del(key))
    return false;
  return del(expireKey);
}


bool KVUnqlite::del(const std::string& key)
{
  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
    return false;

  OSS::mutex_write_lock lock(_mutex);

  if (unqlite_kv_delete(pDbHandle, key.c_str(), key.size()) != UNQLITE_OK)
  {
    log_error();
    return false;
  }

  return true;
}

bool KVUnqlite::getKeys(const std::string& filter, Keys& keys)
{
  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
  {
    return false;
  }

  OSS::mutex_read_lock lock(_mutex);
  
  unqlite_kv_cursor* pCursor = 0;
  
  if (unqlite_kv_cursor_init(pDbHandle, &pCursor) != UNQLITE_OK || !pCursor)
  {
    log_error();
    return false;
  }
  
  KeyConsumer consumer;
  consumer.filter = filter;
  consumer.keys = &keys;
  
  /* Point to the first record */
	for (
    unqlite_kv_cursor_first_entry(pCursor); 
    unqlite_kv_cursor_valid_entry(pCursor);
    unqlite_kv_cursor_next_entry(pCursor))
  {
    unqlite_kv_cursor_key_callback(pCursor, KeyConsumerCallback,(void*)&consumer);
  }
  
  unqlite_kv_cursor_release(pDbHandle, pCursor);
  
  return true;
}

bool KVUnqlite::getRecords(const std::string& filter, Records& records)
{
  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
  {
    return false;
  }

  OSS::mutex_read_lock lock(_mutex);
  
  unqlite_kv_cursor* pCursor = 0;
  
  if (unqlite_kv_cursor_init(pDbHandle, &pCursor) != UNQLITE_OK || !pCursor)
  {
    log_error();
    return false;
  }
  
  RecordConsumer consumer;
  consumer.filter = filter;
  consumer.records = &records;
  
  /* Point to the first record */
	for (
    unqlite_kv_cursor_first_entry(pCursor); 
    unqlite_kv_cursor_valid_entry(pCursor);
    unqlite_kv_cursor_next_entry(pCursor))
  {
    unqlite_kv_cursor_key_callback(pCursor, RecordConsumerCallback,(void*)&consumer);
    if (!consumer.key.empty())
    {
      //
      // We got a key so consume the data
      //
      unqlite_kv_cursor_data_callback(pCursor, RecordConsumerCallback,(void*)&consumer);
    }
  }
  
  unqlite_kv_cursor_release(pDbHandle, pCursor);
  
  return true;
}

bool KVUnqlite::delKeys(const std::string& filter)
{
  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
    return false;
  
  Keys keys;
  if (!getKeys(filter, keys))
    return false;
  
  if (!keys.empty())
  {
    OSS::mutex_write_lock lock(_mutex);
    for (Keys::const_iterator iter = keys.begin(); iter != keys.end(); iter++)
    {
      if (unqlite_kv_delete(pDbHandle, iter->c_str(), iter->size()) != UNQLITE_OK)
      {
        log_error();
        return false;
      }
    }
  }
   
  return true;
}

} } // OSS::Persistent




