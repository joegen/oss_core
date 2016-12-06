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


#include "OSS/BSON/BSONParser.h"
#include "OSS/BSON/libbson.h"
#include <sstream>


namespace OSS {
namespace BSON {

  
static bool bson_find_element(bson_t* bson, const std::string& key, bson_iter_t& iter)
{
  bson_iter_t inner_iter;
  return bson_iter_init (&inner_iter, bson) && bson_iter_find_descendant(&inner_iter, key.c_str(), &iter);
}
 
BSONParser::BSONParser()
{
  _bson = malloc(sizeof(bson_t));
  _parent = _bson;
  bson_init((bson_t*)_parent);
}

BSONParser::BSONParser(const BSONParser& bson)
{
  _bson = bson_copy((bson_t*)bson._bson);
  _parent = _bson;
}

BSONParser::BSONParser(const uint8_t* bson, std::size_t len)
{
  _bson = bson_new_from_data(bson, len);
  _parent = _bson;
}

BSONParser::~BSONParser()
{
  bson_destroy((bson_t*)_bson);
  // TODO: check if this is really a double free.  It causes a segfault  
  //free((bson_t*)_bson);
}

void BSONParser::reset(const uint8_t* bson, std::size_t len)
{
  bson_destroy((bson_t*)_bson);
  // TODO: check if this is really a double free.  It causes a segfault 
  //free ((bson_t*)_bson);
  _bson = bson_new_from_data(bson, len);
  _parent = _bson;
}

BSONParser& BSONParser::operator=(const BSONParser& bson)
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

bool BSONParser::appendString(const std::string& key, const std::string& value)
{
  return bson_append_utf8 ((bson_t*)_parent, key.data(), key.length(), value.data(), value.length());
}

bool BSONParser::appendBoolean(const std::string& key, bool value)
{
   return bson_append_bool((bson_t*)_parent, key.data(), key.length(), value);
}

bool BSONParser::appendInt32(const std::string& key, int32_t value)
{
  return bson_append_int32((bson_t*)_parent, key.data(), key.length(), value);
}

bool BSONParser::appendInt64(const std::string& key, int64_t value)
{
  return bson_append_int64((bson_t*)_parent, key.data(), key.length(), value);
}

bool BSONParser::appendIntptr(const std::string& key, intptr_t value)
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

bool BSONParser::appendDouble(const std::string& key, double value)
{
  return bson_append_double((bson_t*)_parent, key.data(), key.length(), value);
}

bool BSONParser::appendUndefined(const std::string& key)
{
  return bson_append_undefined((bson_t*)_parent, key.data(), key.length());
}

bool BSONParser::updateBoolean(const std::string& key, bool value)
{
  bson_iter_t iter;
  if (bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_BOOL(&iter))
  {
    bson_iter_overwrite_bool(&iter, value);
    return true;
  }
  return false;
}

bool BSONParser::updateInt32(const std::string& key, int32_t value)
{
  bson_iter_t iter;
  if (bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_INT32(&iter))
  {
    bson_iter_overwrite_int32(&iter, value);
    return true;
  }
  return false;
}

bool BSONParser::updateInt64(const std::string& key, int64_t value)
{
  bson_iter_t iter;
  if (bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_INT64(&iter))
  {
    bson_iter_overwrite_int64(&iter, value);
    return true;
  }
  return false;
}

bool BSONParser::updateIntptr(const std::string& key, intptr_t value)
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

bool BSONParser::updateDouble(const std::string& key, double value)
{
  bson_iter_t iter;
  if (bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_DOUBLE(&iter))
  {
    bson_iter_overwrite_double(&iter, value);
    return true;
  }
  return false;
}

void* BSONParser::startSubDocument(const std::string& key)
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

void* BSONParser::endSubDocument(const std::string& key)
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
  
bool BSONParser::appendDocumentBegin(const std::string& key)
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

bool BSONParser::appendDocumentEnd(const std::string& key)
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

bool BSONParser::appendArrayBegin(const std::string& key)
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

bool BSONParser::appendArrayEnd(const std::string& key)
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

bool BSONParser::getString(const std::string& key, std::string& value) const
{
  bson_iter_t iter;
  if (!(bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_UTF8(&iter)))
  {
    return false;
  }
  value = bson_iter_utf8(&iter, 0);
  return true;
}

bool BSONParser::getBoolean(const std::string& key, bool& value) const
{
  bson_iter_t iter;
  if (!(bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_BOOL(&iter)))
  {
    return false;
  }
  value = bson_iter_bool(&iter);
  return true;
}

bool BSONParser::getInt32(const std::string& key, int32_t& value) const
{
  bson_iter_t iter;
  if (!(bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_INT32(&iter)))
  {
    return false;
  }
  value = bson_iter_int32(&iter);
  return true;
}

bool BSONParser::getInt64(const std::string& key, int64_t& value) const
{
  bson_iter_t iter;
  if (!(bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_INT64(&iter)))
  {
    return false;
  }
  value = bson_iter_int64(&iter);
  return true;
}

bool BSONParser::getIntptr(const std::string& key, intptr_t& value) const
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

bool BSONParser::getDouble(const std::string& key, double& value) const
{
  bson_iter_t iter;
  if (!(bson_find_element((bson_t*)_parent, key, iter) && BSON_ITER_HOLDS_DOUBLE(&iter)))
  {
    return false;
  }
  value = bson_iter_double(&iter);
  return true;
}

bool BSONParser::hasKey(const std::string& key) const
{
  return bson_has_field((bson_t*)_parent, key.c_str());
}

std::string BSONParser::stringify()
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

BSONParser* BSONParser::clone()
{
  return new BSONParser(*this);
}

const uint8_t* BSONParser::getData()
{
  if (!_parent)
  {
    return 0;
  }
  return bson_get_data((bson_t*)_parent);
}

std::size_t BSONParser::getDataLength()
{
  if (!_parent)
  {
    return 0;
  }
  return ((bson_t*)_parent)->len;
}


BSONParser::iterator BSONParser::begin()
{
  BSONIterator* pIter = new BSONIterator(false);
  bson_iter_t* bson_iter = (bson_iter_t*)pIter->_iter;
  if (bson_iter_init(bson_iter, (bson_t*)_parent))
  {
    pIter->next();
    return BSONParser::iterator(pIter);
  }
  else
  {
    delete pIter;
    return BSONParser::iterator();
  }
}

BSONParser::iterator BSONParser::find(const std::string& key)
{
  BSONIterator* pIter = new BSONIterator(false);
  bson_iter_t* bson_iter = (bson_iter_t*)pIter->_iter;
  if (bson_iter_init_find(bson_iter, (bson_t*)_parent, key.c_str()))
  {
    return BSONParser::iterator(pIter);
  }
  else
  {
    delete pIter;
    return BSONParser::iterator();
  }
}
 
  
} } //OSS::BSON