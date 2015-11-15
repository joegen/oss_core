#include "gtest/gtest.h"
#include "OSS/Net/AccessControl.h"
#include "OSS/UTL/AdaptiveDelay.h"

static OSS::Persistent::KeyValueStore store;
static const std::string DB_PATH = "access-control";
static const std::string DOCUMENT_ROOT = "/root/" + DB_PATH;


TEST(AccessControlTest, BlackList)
{
  
  
  OSS::Net::AccessControl acc;
  acc.enabled() = true;
  
  acc.banAddress("192.168.1.1");
  acc.banAddress("192.168.1.2");
  acc.banAddress("192.168.1.3");
  
  ASSERT_TRUE(acc.isBannedAddress("192.168.1.1"));
  ASSERT_TRUE(acc.isBannedAddress("192.168.1.2"));
  ASSERT_TRUE(acc.isBannedAddress("192.168.1.3"));
  
  ASSERT_FALSE(acc.isBannedAddress("192.168.1.4"));
  ASSERT_FALSE(acc.isBannedAddress("192.168.1.5"));
}

TEST(AccessControlTest, WhiteList)
{
  OSS::Net::AccessControl acc;
  acc.enabled() = true;
  
  acc.whiteListAddress("192.168.1.1");
  acc.whiteListAddress("192.168.1.2");
  acc.whiteListAddress("192.168.1.3");
  
  ASSERT_TRUE(acc.isWhiteListed("192.168.1.1"));
  ASSERT_TRUE(acc.isWhiteListed("192.168.1.2"));
  ASSERT_TRUE(acc.isWhiteListed("192.168.1.3"));
  
  ASSERT_FALSE(acc.isWhiteListed("192.168.1.4"));
  ASSERT_FALSE(acc.isWhiteListed("192.168.1.5"));
}

TEST(AccessControlTest, WhiteListNetwork)
{
  OSS::Net::AccessControl acc;
  acc.enabled() = true;
  
  acc.whiteListNetwork("192.168.1.0/24");
  acc.whiteListNetwork("192.168.2.0/24");
  acc.whiteListNetwork("192.168.3.0/24");

  ASSERT_TRUE(acc.isWhiteListed("192.168.1.1"));
  ASSERT_TRUE(acc.isWhiteListed("192.168.2.2"));
  ASSERT_TRUE(acc.isWhiteListed("192.168.3.3"));
  
  ASSERT_FALSE(acc.isWhiteListed("192.168.4.4"));
  ASSERT_FALSE(acc.isWhiteListed("192.168.5.5"));
}

TEST(AccessControlTest, LogPacket)
{
  OSS::Net::AccessControl acc;
  acc.enabled() = true;
  acc.autoBanThresholdViolators() = true;
  acc.setPacketsPerSecondThreshold(100);
  acc.setThresholdViolationRate(50);
  acc.setBanLifeTime(3600);
  
  //
  // Simulate 192.168.1.100 violating the threshold sending packets ever 5 ms
  //
  OSS::UTL::AdaptiveDelay ad(5);
  OSS::Net::AccessControl::ViolationReport vr;
  while (!vr.thresholdViolated)
  {
    acc.logPacket("192.168.1.100", 100, &vr);
    ad.wait();
  }
  ASSERT_TRUE(acc.isBannedAddress("192.168.1.100"));
}
