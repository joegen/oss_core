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


#include "OSS/BSON/BSONObject.h"
#include "OSS/BSON/bson.h"
#include <sstream>


namespace OSS {
namespace BSON {

  
static bool bson_find_element(bson_t* bson, const std::string& key, bson_iter_t& iter)
{
  bson_iter_t inner_iter;
  return bson_iter_init (&inner_iter, bson) && bson_iter_find_descendant(&inner_iter, key.c_str(), &iter);
}
 
BSONObject::BSONObject()
{
  _bson = new bson_t();
  _parent = _bson;
  bson_init((bson_t*)_parent);
}

BSONObject::BSONObject(const BSONObject& bson)
{
  _bson = bson_copy((bson_t*)bson._bson);
  _parent = _bson;
}

BSONObject::~BSONObject()
{
  bson_destroy((bson_t*)_bson);
  delete (bson_t*)_bson;
}

 BSONObject& BSONObject::operator=(const BSONObject& bson)
 {
   if (&bson == this)
   {
     return *this;
   }
   bson_destroy((bson_t*)_bson);
   _bson = bson_copy((bson_t*)bson._bson);
   _parent = _bson;
   return *this;
 }

bool BSONObject::appendString(const std::string& key, const std::string& value)
{
  return bson_append_utf8 ((bson_t*)_parent, key.data(), key.length(), value.data(), value.length());
}

bool BSONObject::appendBoolean(const std::string& key, bool value)
{
   return bson_append_bool((bson_t*)_parent, key.data(), key.length(), value);
}

bool BSONObject::appendInt32(const std::string& key, int32_t value)
{
  return bson_append_int32((bson_t*)_parent, key.data(), key.length(), value);
}

bool BSONObject::appendInt64(const std::string& key, int64_t value)
{
  return bson_append_int64((bson_t*)_parent, key.data(), key.length(), value);
}

bool BSONObject::appendIntptr(const std::string& key, intptr_t value)
{
  if (sizeof(intptr_t) == sizeof(int32_t))
  {
    return bson_append_int32((bson_t*)_parent, key.data(), key.length(), value);
  }
  else
  {
    return bson_append_int64((bson_t*)_parent, key.data(), key.length(), value);
  }
}

bool BSONObject::appendDouble(const std::string& key, double value)
{
  return bson_append_double((bson_t*)_parent, key.data(), key.length(), value);
}

bool BSONObject::appendUndefined(const std::string& key)
{
  return bson_append_undefined((bson_t*)_parent, key.data(), key.length());
}

bool BSONObject::updateBoolean(const std::string& key, bool value)
{
  bson_iter_t iter;
  if (bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_BOOL(&iter))
  {
    bson_iter_overwrite_bool(&iter, value);
    return true;
  }
  return false;
}

bool BSONObject::updateInt32(const std::string& key, int32_t value)
{
  bson_iter_t iter;
  if (bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_INT32(&iter))
  {
    bson_iter_overwrite_int32(&iter, value);
    return true;
  }
  return false;
}

bool BSONObject::updateInt64(const std::string& key, int64_t value)
{
  bson_iter_t iter;
  if (bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_INT64(&iter))
  {
    bson_iter_overwrite_int64(&iter, value);
    return true;
  }
  return false;
}

bool BSONObject::updateIntptr(const std::string& key, intptr_t value)
{
  if (sizeof(intptr_t) == sizeof(int32_t))
  {
    return updateInt32(key, (int32_t)value);
  }
  else
  {
    return updateInt64(key, (int64_t)value);
  }
}

bool BSONObject::updateDouble(const std::string& key, double value)
{
  bson_iter_t iter;
  if (bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_DOUBLE(&iter))
  {
    bson_iter_overwrite_double(&iter, value);
    return true;
  }
  return false;
}

void* BSONObject::startSubDocument(const std::string& key)
{
  _subKey.push_back(key);
  std::ostringstream strm;
  for (std::vector<std::string>::iterator iter = _subKey.begin(); iter != _subKey.end(); iter++)
  {
    strm << *iter << "/";
  }
  _currentKey = strm.str();
  bson_t* current = new bson_t;
  _subDocuments[_currentKey] = current;
  return current;
}

void* BSONObject::endSubDocument(const std::string& key)
{
  if (key != _subKey.back())
  {
    return 0;
  }
    
  assert(_subDocuments.find(_currentKey) != _subDocuments.end());
  _subDocuments.erase(_currentKey);
  
  _subKey.pop_back();
  std::ostringstream strm;
  for (std::vector<std::string>::iterator iter = _subKey.begin(); iter != _subKey.end(); iter++)
  {
    strm << *iter << "/";
  }
  _currentKey = strm.str();
  if (_currentKey.empty())
  {
    return _bson;
  }  
  
  assert(_subDocuments.find(_currentKey) != _subDocuments.end());
  return _subDocuments[_currentKey];
}
  
bool BSONObject::appendDocumentBegin(const std::string& key)
{
  bson_t* child = (bson_t*)startSubDocument(key);
  if (!child)
  {
    return false;
  }
  if (!bson_append_document_begin((bson_t*)_parent, key.data(), key.length(), child))
  {
    return false;
  }
  _parent = child;
  return true;
}

bool BSONObject::appendDocumentEnd(const std::string& key)
{
  bson_t* parent = (bson_t*)endSubDocument(key);
  if (!parent)
  {
    return false;
  }
  bson_append_document_end(parent, (bson_t*)_parent);
  assert(_parent != parent);
  delete (bson_t*)_parent;
  _parent = parent;
  return true;
}

bool BSONObject::appendArrayBegin(const std::string& key)
{
  bson_t* child = (bson_t*)startSubDocument(key);
  if (!child)
  {
    return false;
  }
  if (!bson_append_array_begin((bson_t*)_parent, key.data(), key.length(), child))
  {
    return false;
  }
  _parent = child;
  return true;
}

bool BSONObject::appendArrayEnd(const std::string& key)
{
  bson_t* parent = (bson_t*)endSubDocument(key);
  if (!parent)
  {
    return false;
  }
  bson_append_array_end(parent, (bson_t*)_parent);
  assert(_parent != parent);
  delete (bson_t*)_parent;
  _parent = parent;
  return true;
}

bool BSONObject::getString(const std::string& key, std::string& value) const
{
  bson_iter_t iter;
  if (!(bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_UTF8(&iter)))
  {
    return false;
  }
  value = bson_iter_utf8(&iter, 0);
  return true;
}

bool BSONObject::getBoolean(const std::string& key, bool& value) const
{
  bson_iter_t iter;
  if (!(bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_BOOL(&iter)))
  {
    return false;
  }
  value = bson_iter_bool(&iter);
  return true;
}

bool BSONObject::getInt32(const std::string& key, int32_t& value) const
{
  bson_iter_t iter;
  if (!(bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_INT32(&iter)))
  {
    return false;
  }
  value = bson_iter_int32(&iter);
  return true;
}

bool BSONObject::getInt64(const std::string& key, int64_t& value) const
{
  bson_iter_t iter;
  if (!(bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_INT64(&iter)))
  {
    return false;
  }
  value = bson_iter_int64(&iter);
  return true;
}

bool BSONObject::getIntptr(const std::string& key, intptr_t& value) const
{
  bool ret = false;
  if (sizeof(intptr_t) == sizeof(int32_t))
  {
    int32_t val;
    if (getInt32(key, val))
    {
      value = (intptr_t)val;
      ret = true;
    }
  }
  else
  {
    int64_t val;
    if (getInt64(key, val))
    {
      value = (intptr_t)val;
      ret = true;
    }
  }
  return ret;
}

bool BSONObject::getDouble(const std::string& key, double& value) const
{
  bson_iter_t iter;
  if (!(bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_DOUBLE(&iter)))
  {
    return false;
  }
  value = bson_iter_double(&iter);
  return true;
}

bool BSONObject::hasKey(const std::string& key) const
{
  return bson_has_field((bson_t*)_parent, key.c_str());
}

std::string BSONObject::stringify()
{
  std::string ret;
  size_t len;
  char *str = bson_as_json((bson_t*)_parent, &len);
  if (len && str) 
  {
    ret = std::string(str, len);
    bson_free(str);
  }
  return ret;
}

BSONObject* BSONObject::clone()
{
  return new BSONObject(*this);
}
 
  
} } //OSS::BSON