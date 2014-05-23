
#include "gtest/gtest.h"
#include <boost/filesystem.hpp>
#include "OSS/Persistent/KeyValueStore.h"
#include "OSS/Persistent/RESTKeyValueStore.h"
#include "OSS/Core.h"
#include "Poco/Util/ServerApplication.h"

using namespace OSS::Persistent;
using namespace OSS::Net;

#define kvfile "KeyValueStore.test"
#define restkvfile "RESTKeyValueStore.test"
#define restkvport 10111
#define restkvhost "127.0.0.1"
#define data "The Quick Brown fox Jumps Over The Lazy Dog!"
#define iterations 10000

KeyValueStore kv;
RESTKeyValueStore restkv;
RESTKeyValueStore::Client restkv_client(restkvhost, restkvport);
RESTKeyValueStore::Client restkv_client_secure(restkvhost, restkvport, true);
Poco::Util::ServerApplication app;




TEST(KeyValueStoreTest, test_open_close)
{
  boost::filesystem::remove(kvfile);
  ASSERT_FALSE(boost::filesystem::exists(kvfile));
  ASSERT_TRUE(kv.open(kvfile));
  ASSERT_TRUE(kv.isOpen());
  ASSERT_TRUE(kv.put("test-id", "test-data"));
  ASSERT_TRUE(boost::filesystem::exists(kvfile));
  ASSERT_TRUE(kv.close());
  ASSERT_FALSE(kv.isOpen());
  boost::filesystem::remove(kvfile);
  ASSERT_FALSE(boost::filesystem::exists(kvfile));

  //
  // Now leave it open for the remaining test
  //
  ASSERT_TRUE(kv.open(kvfile));
  ASSERT_TRUE(kv.isOpen());
  ASSERT_TRUE(kv.put("init", "init"));
}

TEST(KeyValueStoreTest, test_put)
{
  for (int i = 0; i < iterations; i++)
  {
    std::string key = OSS::string_from_number(i);
    std::string value = key + data;
    ASSERT_TRUE(kv.put(key, value));
  }
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
  for (int i = 0; i < iterations; i++)
  {
    std::string key = OSS::string_from_number(i);
    std::string result;
    ASSERT_TRUE(kv.del(key));
    ASSERT_FALSE(kv.get(key, result));
  }
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

TEST(KeyValueStoreTest, test_rest_init)
{
  boost::filesystem::remove(restkvfile);
  ASSERT_TRUE(restkv.open(restkvfile));
  ASSERT_TRUE(restkv.start(restkvhost, restkvport, false));
}

TEST(KeyValueStoreTest, test_rest_set_get)
{
  ASSERT_TRUE(restkv_client.set("mykey", "myvalue"));
  std::string result;
  ASSERT_TRUE(restkv_client.get("mykey", result));
  ASSERT_STREQ(result.c_str(), "myvalue");
  ASSERT_TRUE(boost::filesystem::exists(restkvfile));
}

TEST(KeyValueStoreTest, test_rest_init_auth)
{
  restkv.stop();
  restkv.setCredentials("user", "password");
  ASSERT_TRUE(restkv.start(restkvhost, restkvport, false));
}

TEST(KeyValueStoreTest, test_rest_bad_auth)
{
  restkv_client.setCredentials("user", "badpass");
  ASSERT_FALSE(restkv_client.set("mykey-auth", "myvalue-auth"));
}

TEST(KeyValueStoreTest, test_rest_set_get_auth)
{
  restkv_client.setCredentials("user", "password");
  ASSERT_TRUE(restkv_client.set("mykey-auth", "myvalue-auth"));
  std::string result;
  ASSERT_TRUE(restkv_client.get("mykey-auth", result));
  ASSERT_STREQ(result.c_str(), "myvalue-auth");
  ASSERT_TRUE(boost::filesystem::exists(restkvfile));
}



