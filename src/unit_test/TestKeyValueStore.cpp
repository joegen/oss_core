
#include "gtest/gtest.h"
#include <boost/filesystem.hpp>
#include "OSS/Persistent/KeyValueStore.h"
#include "OSS/Core.h"

using namespace OSS::Persistent;

KeyValueStore kv;
#define kvfile "KeyValueStore.test"
#define data "The Quick Brown fox Jumps Over The Lazy Dog!"
#define iterations 10000

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

