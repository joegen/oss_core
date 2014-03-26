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


#include "OSS/Persistent/KeyValueStore.h"
#include "OSS/Logger.h"
#include "OSS/Core.h"


extern "C"
{
  #include "unqlite/unqlite.h"
}



namespace OSS {
namespace Persistent {


KeyValueStore::KeyValueStore() :
  _pDbHandle(0)
{
}

KeyValueStore::KeyValueStore(const std::string& path) :
  _pDbHandle(0)
{
  open(path);
}

KeyValueStore::~KeyValueStore()
{
  close();
}

void KeyValueStore::log_error()
{
  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
    return;

  const char *zBuf = 0;
  int iLen;
  unqlite_config(pDbHandle,UNQLITE_CONFIG_ERR_LOG,&zBuf,&iLen);
  if( iLen > 0 )
  {
    OSS_LOG_ERROR("KeyValueStore Exception:  " << zBuf);
  }
}

bool KeyValueStore::close()
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

bool KeyValueStore::open(const std::string& path)
{
  if (isOpen())
    return false;

  _path = path;

  unqlite* pDbHandle = 0;

  if (unqlite_open(&pDbHandle, path.c_str(), UNQLITE_OPEN_CREATE) != UNQLITE_OK || !pDbHandle)
  {
    log_error();
    return false;
  }

  _pDbHandle = pDbHandle;

  return true;
}

bool KeyValueStore::put(const std::string& key, const std::string& value)
{
  if (key.size() > PERSISTENT_STORE_MAX_KEY_SIZE || value.size() > PERSISTENT_STORE_MAX_VALUE_SIZE)
  {
    OSS_LOG_ERROR("KeyValueStore::put:  Maximum data/ size exceeded");
    return false;
  }
  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
    return false;

  OSS::mutex_write_lock lock(_mutex);

  if (unqlite_begin(pDbHandle) != UNQLITE_OK)
  {
    log_error();
    return false;
  }

  if (unqlite_kv_store(pDbHandle, key.c_str(), key.size(), value.c_str(), value.size()) != UNQLITE_OK)
  {
    log_error();
    unqlite_rollback(pDbHandle);
    return false;
  }
  return true;
}

bool KeyValueStore::put(const std::string& key, const std::string& value, unsigned int expireInSeconds)
{
  if (key.size() > PERSISTENT_STORE_MAX_KEY_SIZE || value.size() > PERSISTENT_STORE_MAX_VALUE_SIZE)
  {
    OSS_LOG_ERROR("KeyValueStore::put:  Maximum data/ size exceeded");
    return false;
  }

  unqlite* pDbHandle = static_cast<unqlite*>(_pDbHandle);
  if (!pDbHandle)
    return false;

  OSS::mutex_write_lock lock(_mutex);

  if (unqlite_begin(pDbHandle) != UNQLITE_OK)
  {
    log_error();
    return false;
  }

  if (unqlite_kv_store(pDbHandle, key.c_str(), key.size(), value.c_str(), value.size()) != UNQLITE_OK)
  {
    log_error();
    unqlite_rollback(pDbHandle);
    return false;
  }

  OSS::UInt64 expires = OSS::getTime() + (expireInSeconds*1000);
  std::string expireString = OSS::string_from_number(expires);
  std::string expireKey = key + std::string(".expires");

  if (unqlite_kv_store(pDbHandle, expireKey.c_str(), expireKey.size(), expireString.c_str(), expireString.size()) != UNQLITE_OK)
  {
    log_error();
    unqlite_rollback(pDbHandle);
    return false;
  }

  unqlite_commit(pDbHandle);

  return true;
}

bool KeyValueStore::get(const std::string& key, std::string& value)
{
  if (is_expired(key))
  {
    purge_expired(key);
    return false;
  }
    
  return _get(key, value);
}

bool KeyValueStore::_get(const std::string& key, std::string& value)
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

bool KeyValueStore::is_expired(const std::string& key)
{
  std::string expireKey = key + std::string(".expires");

  std::string expires;
  if (!_get(expireKey, expires))
    return false;

  OSS::UInt64 expireTime = OSS::string_to_number<OSS::UInt64>(expires);
  
  return expireTime <= OSS::getTime();
}

bool KeyValueStore::purge_expired(const std::string& key)
{
  std::string expireKey = key + std::string(".expires");
  if (!del(key))
    return false;
  return del(expireKey);
}


bool KeyValueStore::del(const std::string& key)
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



} } // OSS::Persistent
