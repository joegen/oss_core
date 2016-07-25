
#include "gtest/gtest.h"

#include "OSS/build.h"
#if ENABLE_FEATURE_RESTKV
#if OSS_HAVE_LEVELDB

#include <boost/filesystem.hpp>
#include <Poco/StreamCopier.h>
#include "OSS/Persistent/KeyValueStore.h"
#include "OSS/Persistent/RESTKeyValueStore.h"
#include "OSS/UTL/CoreUtils.h"
#include "Poco/Util/ServerApplication.h"

#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/JSON/elements.h"

#include "OSS/Net/TLSManager.h"

using namespace OSS::Persistent;
using namespace OSS::Net;

#define kvfile "KeyValueStore.test"
#define restkvfile "RESTKeyValueStore.test"
#define restkvport 10111
#define restkv_secure_port 10112
#define restkvhost "127.0.0.1"
#define data "The Quick Brown fox Jumps Over The Lazy Dog!"
#define iterations 100

static KeyValueStore kv;
static RESTKeyValueStore restkv;
static RESTKeyValueStore::Client restkv_client(restkvhost, restkvport);

static RESTKeyValueStore restkv_secure(&restkv);
static RESTKeyValueStore::Client restkv_client_secure(restkvhost, restkv_secure_port, true);
static bool input_proc_put_invoked = false;
static bool input_proc_del_invoked = false;

static KVInputProcessor::Action input_proc_put(const std::string& key, const std::string& value, unsigned int expireInSeconds)
{
  input_proc_put_invoked = true;
  return KVInputProcessor::Allow;
}

static KVInputProcessor::Action input_proc_del(const std::string& key)
{
  input_proc_del_invoked = true;
  return KVInputProcessor::Allow;
}


TEST(KeyValueStoreTest, test_open_close)
{
  boost::filesystem::remove_all(kvfile);
  ASSERT_FALSE(boost::filesystem::exists(kvfile));
  ASSERT_TRUE(kv.open(kvfile));
  ASSERT_TRUE(kv.isOpen());
  ASSERT_TRUE(kv.put("test-id", "test-data"));
  ASSERT_TRUE(boost::filesystem::exists(kvfile));
  ASSERT_TRUE(kv.close());
  ASSERT_FALSE(kv.isOpen());
  boost::filesystem::remove_all(kvfile);
  ASSERT_FALSE(boost::filesystem::exists(kvfile));

  //
  //  Set an input processor
  //
  KVInputProcessor inputProc("test_input_proc");
  inputProc.put = boost::bind(input_proc_put, _1, _2, _3);
  inputProc.del = boost::bind(input_proc_del, _1);
  kv.addInputProcessor(inputProc);
  input_proc_put_invoked = false;
  
  //
  // Now leave it open for the remaining test
  //
  ASSERT_TRUE(kv.open(kvfile));
  ASSERT_TRUE(kv.isOpen());
  ASSERT_TRUE(kv.put("init", "init"));
  ASSERT_TRUE(input_proc_put_invoked);
}

TEST(KeyValueStoreTest, test_put)
{
  input_proc_put_invoked = false;
  for (int i = 0; i < iterations; i++)
  {
    std::string key = OSS::string_from_number(i);
    std::string value = key + data;
    ASSERT_TRUE(kv.put(key, value));
  }
  ASSERT_TRUE(input_proc_put_invoked);
}

TEST(KeyValueStoreTest, test_get)
{
  for (int i = 0; i < iterations; i++)
  {
    std::string key = OSS::string_from_number(i);
    std::string value = key + data;

    std::string result;
    ASSERT_TRUE(kv.get(key, result));
    ASSERT_STREQ(result.c_str(), value.c_str());
  }
}

TEST(KeyValueStoreTest, test_delete)
{
  input_proc_put_invoked = false;
  for (int i = 0; i < iterations; i++)
  {
    std::string key = OSS::string_from_number(i);
    std::string result;
    ASSERT_TRUE(kv.del(key));
    ASSERT_FALSE(kv.get(key, result));
  }
  ASSERT_TRUE(input_proc_del_invoked);
}

TEST(KeyValueStoreTest, test_expires)
{
  std::string key = "test-expires";
  ASSERT_TRUE(kv.put(key, data, 3));
  OSS::thread_sleep(2000);
  std::string result;
  ASSERT_TRUE(kv.get(key, result));
  ASSERT_STREQ(result.c_str(), data);
  OSS::thread_sleep(2000);
  ASSERT_FALSE(kv.get(key, result));
}

TEST(KeyValueStoreTest, test_update)
{
  std::string key = "test-update";
  std::string updateData = "test-update-data";
  ASSERT_TRUE(kv.put(key, data));
  std::string result;
  ASSERT_TRUE(kv.get(key, result));
  ASSERT_STREQ(result.c_str(), data);
  ASSERT_TRUE(kv.put(key, updateData));
  ASSERT_TRUE(kv.get(key, result));
  ASSERT_STREQ(result.c_str(), updateData.c_str());
}

TEST(KeyValueStoreTest, test_filter_multiple)
{
  std::string testdata = "test-data";
  
  for (int i = 0; i < 100;i++)
  {
    std::ostringstream strm;
    strm << "getkey_a." << i;  
    ASSERT_TRUE(kv.put(strm.str().c_str(), testdata.c_str()));
  }
  
  for (int i = 0; i < 100;i++)
  {
    std::ostringstream strm;
    strm << "getkey_b." << i;  
    ASSERT_TRUE(kv.put(strm.str().c_str(), testdata.c_str()));
  }
  
  KVKeys all, partial_a, partial_b;
  ASSERT_TRUE(kv.getKeys("getkey_*", all));
  ASSERT_EQ(all.size(), 200);
  
  ASSERT_TRUE(kv.getKeys("getkey_a*",  partial_a));
  ASSERT_EQ(partial_a.size(), 100);
  
  ASSERT_TRUE(kv.getKeys("getkey_b*",  partial_b));
  ASSERT_EQ(partial_b.size(), 100);
  
  KVRecords partialrec_b;
  
  ASSERT_TRUE(kv.getRecords("getkey_b*",  partialrec_b));
  ASSERT_EQ(partialrec_b.size(), 100);
  
  for (KVRecords::const_iterator iter = partialrec_b.begin(); iter != partialrec_b.end(); iter++)
    ASSERT_STREQ(iter->value.c_str(), testdata.c_str());
  
  ASSERT_TRUE(kv.delKeys("getkey_b*"));
  partialrec_b.clear();
  ASSERT_TRUE(kv.getRecords("getkey_b*",  partialrec_b));
  ASSERT_EQ(partialrec_b.size(), 0);
}


TEST(KeyValueStoreTest, test_rest_init_auth)
{
  restkv.setCredentials("user", "password");
  ASSERT_TRUE(restkv.start(restkvhost, restkvport, false));
}



TEST(KeyValueStoreTest, test_rest_put_get_auth)
{ 
  boost::filesystem::remove_all("people");

  int status = 0;
  
  restkv_client.setCredentials("user", "password");
 
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp1/fname", "Joegen", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp1/sname", "Baclor", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp1/addr/street/name", "San Pablo Street", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp1/addr/street/number", "17", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp1/addr/city", "Pasig", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp1/country/province", "Metro Manila", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp1/country/areaCode", "1603", status));
  
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp2/fname", "Che", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp2/sname", "Sto. Domingo", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp2/addr/street/name", "San Pablo Street", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp2/addr/street/number", "17", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp2/addr/city", "Pasig", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp2/country/province", "Bulacan", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp2/country/areaCode", "1800", status));
  ASSERT_TRUE(restkv_client.restPUT("/root/people/emp2/quotes", "\"\"''", status));
  
  std::ostringstream result;
  ASSERT_TRUE(restkv_client.restGET("/root/people/", result, status));
  
  std::stringstream input;

  input << result.str();
  
  std::cout << result.str() << std::endl;
   
  try 
  {
    json::Object jsonObject;
    json::Reader::Read(jsonObject, input);
    json::Writer::Write(jsonObject, std::cout);
    
    ASSERT_TRUE(((json::String&)jsonObject["people"]["emp1"]["fname"]).Value() == "Joegen");
    ASSERT_TRUE(((json::String&)jsonObject["people"]["emp2"]["fname"]).Value() == "Che");
    
    ASSERT_TRUE(((json::String&)jsonObject["people"]["emp1"]["country"]["province"]).Value() == "Metro Manila");
    ASSERT_TRUE(((json::String&)jsonObject["people"]["emp2"]["country"]["province"]).Value() == "Bulacan");
    
    ASSERT_TRUE(((json::String&)jsonObject["people"]["emp2"]["quotes"]).Value() == "\"\"''");
  }
  catch(json::Exception e)
  {
    std::cout << e.what() << std::endl;
    ASSERT_TRUE(false);
  }
}

TEST(KeyValueStoreTest, test_rest_tls)
{
  if (!boost::filesystem::exists("rootcert.pem"))
    return;
  
  if (!boost::filesystem::exists("any.pem"))
    return;
  
  OSS::Net::TLSManager::instance().initialize("any.pem", "any.pem", "rootcert.pem", "secret", true, OSS::Net::TLSManager::VERIFY_RELAXED);
  
  restkv_secure.setCredentials("user", "password");
  ASSERT_TRUE(restkv_secure.start(restkvhost, restkv_secure_port, true));
  
}

TEST(KeyValueStoreTest, test_rest_tls_put_get)
{
  if (!boost::filesystem::exists("rootcert.pem"))
    return;
  
  if (!boost::filesystem::exists("any.pem"))
    return;
  
  int status;
  restkv_client_secure.setCredentials("user", "password");
  ASSERT_TRUE(restkv_client_secure.restPUT("/root/secure/test/item", "Secure Test", status));
   
  std::ostringstream result;
  ASSERT_TRUE(restkv_client_secure.restGET("/root/secure/", result, status));
  
  std::stringstream input;

  input << result.str();
  
  std::cout << result.str() << std::endl;
  
  std::ostringstream result2;
  ASSERT_TRUE(restkv_client.restGET("/root/secure/", result2, status));
}

#else
TEST(NullTest, null_test_kv){}
#endif
#endif

