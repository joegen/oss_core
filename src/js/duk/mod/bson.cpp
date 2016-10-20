
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

#if 0

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
#endif

#include "OSS/JS/DUK/InternalModules.h"
#include "OSS/BSON/BSONParser.h"
#include "OSS/JS/DUK/duk_context_helper.h"


namespace OSS {
namespace JS {
namespace DUK {
namespace MOD {

using namespace OSS::BSON;

static duk_functions_t gFunctions;

duk_ret_t bson_create(duk_context* ctx)
{
  duk_push_pointer(ctx, new BSONParser());
  return 1;
}

duk_ret_t bson_destroy(duk_context* ctx)
{
  void* ptr = duk_require_pointer(ctx, 0);
  if (ptr)
  {
    delete (BSONParser*)ptr;
  }
  return 0;
}

duk_ret_t bson_append_string(duk_context* ctx)
{
  void* ptr = duk_require_pointer(ctx, 0);
  const char* key = duk_require_string(ctx, 1);
  const char* val = duk_require_string(ctx, 2);
  duk_push_boolean(ctx, ((BSONParser*)ptr)->appendString(key, val));
  return 1;
}

duk_ret_t bson_append_boolean(duk_context* ctx)
{
  void* ptr = duk_require_pointer(ctx, 0);
  const char* key = duk_require_string(ctx, 1);
  bool val = duk_require_boolean(ctx, 2);
  duk_push_boolean(ctx, ((BSONParser*)ptr)->appendBoolean(key, val));
  return 1;
}

duk_ret_t bson_append_int(duk_context* ctx)
{
  void* ptr = duk_require_pointer(ctx, 0);
  const char* key = duk_require_string(ctx, 1);
  int32_t val = duk_require_int(ctx, 2);
  duk_push_boolean(ctx, ((BSONParser*)ptr)->appendInt32(key, val));
  return 1;
}

duk_ret_t bson_append_double(duk_context* ctx)
{
  void* ptr = duk_require_pointer(ctx, 0);
  const char* key = duk_require_string(ctx, 1);
  double val = duk_require_number(ctx, 2);
  duk_push_boolean(ctx, ((BSONParser*)ptr)->appendDouble(key, val));
  return 1;
}

duk_ret_t bson_get_string(duk_context* ctx)
{
  void* ptr = duk_require_pointer(ctx, 0);
  const char* key = duk_require_string(ctx, 1);
  std::string val;
  if (((BSONParser*)ptr)->getString(key, val))
  {
    duk_push_string(ctx, val.c_str());
    return 1;
  }
  return 0;
}

duk_ret_t bson_get_boolean(duk_context* ctx)
{
  void* ptr = duk_require_pointer(ctx, 0);
  const char* key = duk_require_string(ctx, 1);
  bool val;
  if (((BSONParser*)ptr)->getBoolean(key, val))
  {
    duk_push_boolean(ctx, val);
    return 1;
  }
  return 0;
}

duk_ret_t bson_get_int(duk_context* ctx)
{
  void* ptr = duk_require_pointer(ctx, 0);
  const char* key = duk_require_string(ctx, 1);
  int32_t val;
  if (((BSONParser*)ptr)->getInt32(key, val))
  {
    duk_push_int(ctx, val);
    return 1;
  }
  return 0;
}

duk_ret_t bson_get_double(duk_context* ctx)
{
  void* ptr = duk_require_pointer(ctx, 0);
  const char* key = duk_require_string(ctx, 1);
  double val;
  if (((BSONParser*)ptr)->getDouble(key, val))
  {
    duk_push_number(ctx, val);
    return 1;
  }
  return 0;
}


duk_ret_t bson_mod_init(duk_context* ctx)
{
  gFunctions.push_back( duktape_function( "bson_create", bson_create, 0 ));
  gFunctions.push_back( duktape_function( "bson_destroy", bson_destroy, 1 ));
  gFunctions.push_back( duktape_function( "bson_append_string", bson_append_string, 3 ));
  gFunctions.push_back( duktape_function( "bson_append_boolean", bson_append_boolean, 3 ));
  gFunctions.push_back( duktape_function( "bson_append_int", bson_append_int, 3 ));
  gFunctions.push_back( duktape_function( "bson_append_double", bson_append_double, 3 ));
  gFunctions.push_back( duktape_function( "bson_get_string", bson_get_string, 2 ));
  gFunctions.push_back( duktape_function( "bson_get_boolean", bson_get_boolean, 2 ));
  gFunctions.push_back( duktape_function( "bson_get_int", bson_get_int, 2 ));
  gFunctions.push_back( duktape_function( "bson_get_double", bson_get_double, 2 ));
  gFunctions.push_back( duktape_function( 0, 0, 0 ));
  duk_push_object(ctx);
  duk_export_functions(ctx, gFunctions);
  return 1;
}

} } } }
