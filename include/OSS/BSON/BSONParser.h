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

#ifndef OSS_BSONPARSER_H_INCLUDED
#define OSS_BSONPARSER_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <boost/any.hpp>
#include <stdint.h>
#include "OSS/OSS.h"
#include "OSS/BSON/BSONIterator.h"


namespace OSS {
namespace BSON {

  
class BSONParser
{
public:  
  typedef BSONIterator::Ptr iterator;
  BSONParser();
  BSONParser(const BSONParser& bson);
  BSONParser(const uint8_t* bson, std::size_t len);
  ~BSONParser();
  
  void reset(const uint8_t* bson, std::size_t len);
  BSONParser& operator=(const BSONParser& bson);
  
  bool appendString(const std::string& key, const std::string& value);
  bool appendBoolean(const std::string& key, bool value);
  bool appendInt32(const std::string& key, int32_t value);
  bool appendInt64(const std::string& key, int64_t value);
  bool appendIntptr(const std::string& key, intptr_t value);
  bool appendDouble(const std::string& key, double value);  
  bool appendUndefined(const std::string& key);
  
  bool appendDocumentBegin(const std::string& key);
  bool appendDocumentEnd(const std::string& key);
  
  bool appendArrayBegin(const std::string& key);
  bool appendArrayEnd(const std::string& key);
  
  bool updateBoolean(const std::string& key, bool value);
  bool updateInt32(const std::string& key, int32_t value);
  bool updateInt64(const std::string& key, int64_t value);
  bool updateIntptr(const std::string& key, intptr_t value);
  bool updateDouble(const std::string& key, double value); 
  
  bool getString(const std::string& key, std::string& value) const;
  bool getBoolean(const std::string& key, bool& value) const;
  bool getInt32(const std::string& key, int32_t& value) const;
  bool getInt64(const std::string& key, int64_t& value) const;
  bool getIntptr(const std::string& key, intptr_t& value) const;
  bool getDouble(const std::string& key, double& value) const;
  
  bool hasKey(const std::string& key) const;
  std::string stringify();

  const uint8_t* getData();
  std::size_t getDataLength();
  
  BSONParser* clone();  
  
  BSONParser::iterator begin();
  BSONParser::iterator find(const std::string& key);
protected:
  void*  startSubDocument(const std::string& key);
  void*  endSubDocument(const std::string& key);
  void* _bson;
  std::vector<std::string> _subKey;
  std::map<std::string, void*> _subDocuments;
  std::string _currentKey;
  void* _parent;
  
};

  
} } // OSS::BSON



#endif // OSS_BSONPARSER_H_INCLUDED

