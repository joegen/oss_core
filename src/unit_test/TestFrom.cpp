#include "gtest/gtest.h"
#include "OSS/OSS.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPFrom.h"
#include <iostream>
#include <sstream>

using OSS::SIP::CRLF;


TEST(ParserTest, test_from_parser)
{
  OSS::SIP::SIPFrom from_0("sip:alice@atlanta.com;tag=1234");
  OSS::SIP::SIPFrom from_1("alice<sip:alice@atlanta.com>;tag=1234");
  OSS::SIP::SIPFrom from_2("\"alice\"<sip:alice@atlanta.com>;tag=1234");
  OSS::SIP::SIPFrom from_3("alice <sip:alice@atlanta.com>;tag=1234");
  OSS::SIP::SIPFrom from_4("\"alice\" <sip:alice@atlanta.com;user=phone>;tag=1234");
  from_0 = from_0;
  ASSERT_TRUE(from_0.getDisplayName().empty());
  ASSERT_TRUE(from_1.getDisplayName() == "alice");
  ASSERT_TRUE(from_2.getDisplayName() == "\"alice\"");
  ASSERT_TRUE(from_3.getDisplayName() == "alice");
  ASSERT_TRUE(from_4.getDisplayName() == "\"alice\"");
  ASSERT_TRUE(from_0.setDisplayName("alice"));
  ASSERT_TRUE(from_0.getDisplayName() == "alice");
  ASSERT_TRUE(from_0.setDisplayName("\"bob\""));
  ASSERT_TRUE(from_0.getDisplayName() == "\"bob\"");

  OSS::SIP::SIPFrom from_5("sip:alice@atlanta.com;tag=1234");
  ASSERT_TRUE(from_5.getURI() == "sip:alice@atlanta.com");
  ASSERT_TRUE(from_4.getURI() == "sip:alice@atlanta.com;user=phone");
  ASSERT_TRUE(from_5.setURI("sip:bob@biloxy.com"));
  ASSERT_TRUE(from_5.getURI() == "sip:bob@biloxy.com");
  ASSERT_TRUE(from_5.setURI("bob <sip:bob@biloxy.com;user=phone>"));
  ASSERT_TRUE(from_5.getURI() == "sip:bob@biloxy.com;user=phone");
  ASSERT_TRUE(from_5.setURI("\"Alice\" <sip:alice@atlanta.com;user=phone>"));
  ASSERT_TRUE(from_5.getURI() == "sip:alice@atlanta.com;user=phone");
  ASSERT_TRUE(from_5.getDisplayName() == "\"Alice\"");
  ASSERT_TRUE(from_5.setURI("sip:bob@biloxy.com;user=ip"));
  ASSERT_TRUE(from_5.getDisplayName() == "\"Alice\"");
  std::string noAnglesURI = from_5.getURI();
  ASSERT_STREQ(from_5.getURI().c_str(),  noAnglesURI.c_str());
  ASSERT_STREQ( noAnglesURI.c_str(), "sip:bob@biloxy.com;user=ip");

  OSS::SIP::SIPFrom from_6("sip:alice@atlanta.com;tag=1234");
  ASSERT_TRUE(from_6.getHeaderParams() == ";tag=1234");
  OSS::SIP::SIPFrom from_7("alice <sip:alice@atlanta.com;user=phone>;tag=1234");
  ASSERT_TRUE(from_7.getHeaderParams() == ";tag=1234");
  ASSERT_TRUE(from_6.setHeaderParams(";tag=4567"));
  ASSERT_TRUE(from_6.getHeaderParams() == ";tag=4567");
  ASSERT_TRUE(from_7.setHeaderParams(";tag=4567"));
  ASSERT_TRUE(from_7.getHeaderParams() == ";tag=4567");
  ASSERT_TRUE(from_6.setHeaderParam("tag", "6789"));
  ASSERT_TRUE(from_6.getHeaderParam("tag") == "6789");
  ASSERT_TRUE(from_7.setHeaderParam("tag", "6789"));
  ASSERT_TRUE(from_7.getHeaderParam("tag") == "6789");

  std::string from_8 = "\"8429001\"<sip:8429001@192.168.1.100:85>;tag=9d603601";
  ASSERT_TRUE(OSS::SIP::SIPFrom::setHostPort(from_8, "registrar.atlanta.com"));

  OSS::SIP::SIPReferTo referTo("<sip:alice2atlanta.com?replaces=calid-001;to-tag=totag-001;from-tag=fromtag-001>");
  OSS::SIP::SIPURI referToUri = referTo.getURI();
  std::string replaces = referToUri.getHeader("REPLACES");
  ASSERT_STREQ(replaces.c_str(), "calid-001;to-tag=totag-001;from-tag=fromtag-001");
}
