#include "gtest/gtest.h"
#include "OSS/OSS.h"
#include "OSS/SIP/SIPReplaces.h"
#include <iostream>
#include <sstream>

TEST(ParserTest, test_replaces)
{
  std::string replaces = "12adf2f34456gs5;to-tag=12345;from-tag=54321;early-only";
  OSS::SIP::SIPReplaces hReplaces;
  hReplaces.setCallId("12adf2f34456gs5");
  hReplaces.setFromTag("54321");
  hReplaces.setToTag("12345");
  hReplaces.setEarlyFlag(true);
  
  std::string callId = hReplaces.getCallId();
  ASSERT_STREQ(callId.c_str(), "12adf2f34456gs5");
  
  std::string fromTag = hReplaces.getFromTag();
  ASSERT_STREQ(fromTag.c_str(), "54321");
  
  std::string toTag = hReplaces.getToTag();
  ASSERT_STREQ(toTag.c_str(), "12345");
  
  ASSERT_TRUE(hReplaces.isEarlyFlagSet());
  
  hReplaces.setEarlyFlag(false);
  
  ASSERT_FALSE(hReplaces.isEarlyFlagSet());
}
