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

#ifndef OSS_BERKELEYDB_H_INCLUDED
#define	OSS_BERKELEYDB_H_INCLUDED


#include <db_cxx.h>
#include <string>
#include <sstream>

#include "OSS/UTL/CoreUtils.h"


namespace OSS {


class BerkeleyDb
{
public:

  BerkeleyDb() :
    _isOpen(false),
    _pDb(0),
    _pCursor(0)
  {
  }

  ~BerkeleyDb()
  {
    close();
  }

  bool open(const std::string& path)
  {
    _path = path;
    if (_isOpen || _pDb || _pCursor)
      return false;

    _pDb = new Db(0, 0);
    int ret = _pDb->open(0, path.c_str(), 0, DB_BTREE, DB_CREATE | DB_THREAD,0);
    if (ret != 0)
    {
      delete _pDb;
      _pDb = 0;
      _isOpen = false;
      return false;
    }

    _pDb->cursor(0,&_pCursor,0);
    if (!_pCursor)
    {
      _pDb->close(0);
      delete _pDb;
      _pDb = 0;
      _isOpen = false;
      return false;
    }

    _isOpen = true;
    return _isOpen;
  }

  void close()
  {
    if (!_isOpen)
      return;

    if (_pCursor)
      _pCursor->close();

    if (_pDb)
      _pDb->close(0);

    //
    // cursor pointer is owned by the DB
    //
    _pCursor = 0;

    delete _pDb;
    _pDb = 0;
  }

  const std::string& getPath() const
  {
    return _path;
  }

  bool isOpen() const
  {
    return _isOpen;
  }

  bool set(const std::string& key_, const std::string& value)
  {
    if (!_pDb)
      return false;

    Dbt key( (void*)key_.data(), (::u_int32_t)key_.size() );
    Dbt data( (void*)value.data(), (::u_int32_t)value.size() );
    int ret;
    ret = _pDb->put(0, &key, &data, 0);
    if ( ret != 0 )
      return false;
    _pDb->sync(0);
    return true;
  }

  bool get(const std::string& key_, std::string& value) const
  {
    if (!_pDb)
      return false;

    Dbt key( (void*)key_.data(), (::u_int32_t)key_.size() );
    Dbt data;
    data.set_flags(DB_DBT_MALLOC);

    int ret;

    ret = _pDb->get(0, &key, &data, 0);

    if (ret == DB_NOTFOUND)
    {
      // key not found
      if ( data.get_data() )
         free( data.get_data() );
      return false;
    }

    if( ret != 0 )
      return false;

    std::string result( reinterpret_cast<const char*>(data.get_data()), data.get_size() );
    if ( data.get_data() )
      free( data.get_data() );
    if (result.empty())
    {
      // this should never happen
      return false;
    }

    value = result;

    return true;
  }

  bool erase(const std::string& key_)
  {
    if (!_pDb)
      return false;

    Dbt key( (void*) key_.data(), (::u_int32_t)key_.size() );
    _pDb->del(0, &key, 0);
    _pDb->sync(0);
    return true;
  }

bool nextKey(std::string& nextKey, bool first) const
{
  if (!_pDb || !_pCursor)
      return false;

  Dbt key, data;
  int ret;

  ret = _pCursor->get(&key,&data, first ? DB_FIRST : DB_NEXT);
  if ( ret == DB_NOTFOUND )
    return false;

  if (ret != 0 || key.get_size() <= 0)
    return false;

  nextKey = std::string(reinterpret_cast<const char*>(key.get_data()), key.get_size() );
  return true;
}

bool getKeys(std::vector<std::string>& keys) const
{
  bool first = true;
  std::string key;
  while(nextKey(key, first))
  {
    first = false;
    keys.push_back(key);
  }
  return !keys.empty();
}

bool getKeys(const std::string& pattern, std::vector<std::string>& keys) const
{
  bool first = true;
  std::string key;
  while(nextKey(key, first))
  {
    first = false;
    if (OSS::string_wildcard_compare(pattern.c_str(), key))
    {
      keys.push_back(key);
    }
  }
  return !keys.empty();
}

void clear()
{
  std::string key;
  std::vector<std::string> keys;
  getKeys(keys);

  for (std::vector<std::string>::const_iterator iter = keys.begin(); iter != keys.end(); iter++)
    erase(*iter);
}

protected:
  bool _isOpen;
  Db* _pDb;
  Dbc* _pCursor;
  std::string _path;
private:
  BerkeleyDb(const BerkeleyDb&){};
  BerkeleyDb& operator = (const BerkeleyDb&){return *this;};
};

} // OSS


#endif	/* BERKELEYDB_H */

