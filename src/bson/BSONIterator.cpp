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


#include "OSS/BSON/BSONIterator.h"
#include "OSS/BSON/libbson.h"

namespace OSS {
namespace BSON {
  
  
const int BSONIterator::BSON_TYPE_DOUBLE = BSON_TYPE_DOUBLE;
const int BSONIterator::BSON_TYPE_STRING = BSON_TYPE_UTF8;
const int BSONIterator::BSON_TYPE_DOCUMENT = BSON_TYPE_DOCUMENT;
const int BSONIterator::BSON_TYPE_ARRAY = BSON_TYPE_ARRAY;
const int BSONIterator::BSON_TYPE_UNDEFINED = BSON_TYPE_UNDEFINED;
const int BSONIterator::BSON_TYPE_BOOL = BSON_TYPE_BOOL;
const int BSONIterator::BSON_TYPE_INT32 = BSON_TYPE_INT32;
const int BSONIterator::BSON_TYPE_INT64 = BSON_TYPE_INT64;  
  

BSONIterator::BSONIterator(bool isChild) : 
  _iter(0),
  _eof(false),
  _isChild(isChild)
{
  if (!_isChild)
  {
    _iter = malloc(sizeof(bson_iter_t));
  }
}

BSONIterator::~BSONIterator()
{
  if (!_isChild)
  {
    free(_iter);
  }
}

bool BSONIterator::next()
{
  if (eof())
  {
    return false;
  }
  _eof = !bson_iter_next((bson_iter_t*)_iter);
  return !_eof;
}

bool BSONIterator::getKey(std::string& key) const
{
  const char* key_ = bson_iter_key((bson_iter_t*)_iter);
  if (!key_)
  {
    return false;
  }
  key = key_;
  return !key.empty();
}

int BSONIterator::getType() const
{
  return bson_iter_type((bson_iter_t*)_iter);
}

bool BSONIterator::getString(std::string& value) const
{
  if (getType() != BSON_TYPE_STRING)
  {
    return false;
  }
  const char* val = bson_iter_utf8((bson_iter_t*)_iter, 0);
  if (!val)
  {
    return false;
  }
  
  value = val;
  return !value.empty();
}

bool BSONIterator::getBoolean(bool& value) const
{
  if (getType() != BSON_TYPE_BOOL)
  {
    return false;
  }
  value = bson_iter_bool((bson_iter_t*)_iter);
  return true;
}

bool BSONIterator::getInt32(int32_t& value) const
{
  if (getType() != BSON_TYPE_INT32)
  {
    return false;
  }
  value = bson_iter_int32((bson_iter_t*)_iter);
  return true;
}

bool BSONIterator::getInt64(int64_t& value) const
{
  if (getType() != BSON_TYPE_INT64)
  {
    return false;
  }
  value = bson_iter_int64((bson_iter_t*)_iter);
  return true;
}

bool BSONIterator::getIntptr(intptr_t& value) const
{
  int type = getType();
  if (type != BSON_TYPE_INT32 && type != BSON_TYPE_INT64)
  {
    return false;
  }
  if (type == BSON_TYPE_INT32)
  {
    value = bson_iter_int32((bson_iter_t*)_iter);
  }
  else
  {
    value = bson_iter_int64((bson_iter_t*)_iter);
  }
  return true;
}

bool BSONIterator::getDouble(double& value) const
{
  if (getType() != BSON_TYPE_DOUBLE)
  {
    return false;
  }
  value = bson_iter_double((bson_iter_t*)_iter);
  return true;
}

BSONIterator::Ptr BSONIterator::recurse() const
{
  int type = getType();
  if (type == BSON_TYPE_ARRAY || type == BSON_TYPE_DOCUMENT)
  {
    BSONIterator* pChild = new BSONIterator(true);
    if (bson_iter_recurse((bson_iter_t*)_iter, (bson_iter_t*)(pChild->_iter)))
    {
      return BSONIterator::Ptr(pChild);
    }
    delete pChild;
  }
  return BSONIterator::Ptr();
}


  
  
} } // OSS::BSON


