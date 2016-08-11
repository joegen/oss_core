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


#ifndef OSS_BSONITERATOR_H_INCLUDED
#define OSS_BSONITERATOR_H_INCLUDED


#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "OSS/OSS.h"


namespace OSS {
namespace BSON {
  
  
class BSONIterator : boost::noncopyable
{
public:
  typedef boost::shared_ptr<BSONIterator> Ptr;
  
  static const int BSON_TYPE_DOUBLE;
  static const int BSON_TYPE_STRING;
  static const int BSON_TYPE_DOCUMENT;
  static const int BSON_TYPE_ARRAY;
  static const int BSON_TYPE_UNDEFINED;
  static const int BSON_TYPE_BOOL;
  static const int BSON_TYPE_INT32;
  static const int BSON_TYPE_INT64;
  
  ~BSONIterator();
  
  bool getString(std::string& value) const;
  bool getBoolean(bool& value) const;
  bool getInt32(int32_t& value) const;
  bool getInt64(int64_t& value) const;
  bool getIntptr(intptr_t& value) const;
  bool getDouble(double& value) const;
  
  bool getKey(std::string& key) const;
  int getType() const;
  
  bool next();
  bool eof();
  
  Ptr recurse() const;
  
protected:
  friend class BSONObject;
  BSONIterator(bool isChild);
  void* _iter;
  bool _eof;
  bool _isChild;
};


//
// Inlines
//
inline bool BSONIterator::eof()
{
  return _eof || !_iter;
}
  
} } // OSS::BSON

#endif // OSS_BSONITERATOR_H_INCLUDED

