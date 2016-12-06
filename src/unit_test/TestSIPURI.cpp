#include "gtest/gtest.h"

#include "OSS/SIP/SIPURI.h"
#include <iostream>
#include <sstream>

using namespace OSS;

TEST(ParserTest, test_sip_uri_parser)
{
  SIP::SIPURI uri_0;
  uri_0 = "sip:alice@atlanta.com?h1=v1;v1.1&h2=v2;v2.1";
  ASSERT_TRUE(uri_0.getScheme() == "sip");
  ASSERT_TRUE(uri_0.getUser() == "alice");
  ASSERT_TRUE(uri_0.getPassword().empty());
  ASSERT_TRUE(uri_0.getHostPort() == "atlanta.com");
  ASSERT_TRUE(uri_0.getParam("user").empty());
  ASSERT_TRUE(uri_0.setParam("user", "phone"));
  ASSERT_TRUE(uri_0.getParam("user") == "phone");
  ASSERT_STREQ(uri_0.getHeaders().c_str(), "?h1=v1;v1.1&h2=v2;v2.1");
  ASSERT_STREQ(uri_0.getHeader("h1").c_str(), "v1;v1.1");
  ASSERT_STREQ(uri_0.getHeader("h2").c_str(), "v2;v2.1");

  uri_0.setParam("tport", "5060");
  uri_0.setParam("t", "garbage");
  ASSERT_STREQ(uri_0.getParam("tport").c_str(), "5060");
  ASSERT_STREQ(uri_0.getParam("t").c_str(), "garbage");

  SIP::SIPURI uri_1("sip:alice:alicepass@atlanta.com?h1=v1&h2=v2");
  ASSERT_TRUE(uri_1.getScheme() == "sip");
  ASSERT_TRUE(uri_1.getUser() == "alice");
  ASSERT_TRUE(uri_1.getPassword() == "alicepass");
  ASSERT_TRUE(uri_1.getHostPort() == "atlanta.com");
  ASSERT_TRUE(uri_1.getParam("user").empty());

  SIP::SIPURI uri_2("sip:atlanta.com:5060?h1=v1&h2=v2");
  ASSERT_TRUE(uri_2.getScheme() == "sip");
  ASSERT_TRUE(uri_2.getUser().empty());
  ASSERT_TRUE(uri_2.getPassword().empty());
  ASSERT_TRUE(uri_2.getHostPort() == "atlanta.com:5060");
  ASSERT_TRUE(uri_2.getParam("user").empty());

  SIP::SIPURI uri_3("sip:+12127773456@atlanta.com:5060;user=phone;transport=udp?h1=v1&h2=v2");
  ASSERT_TRUE(uri_3.getScheme() == "sip");
  ASSERT_TRUE(uri_3.getUser() == "+12127773456");
  ASSERT_TRUE(uri_3.getPassword().empty());
  ASSERT_TRUE(uri_3.getHostPort() == "atlanta.com:5060");
  ASSERT_TRUE(uri_3.getParam("user") == "phone");
  ASSERT_TRUE(uri_3.getParam("transport") == "udp");
  ASSERT_TRUE(uri_3.setParams(";user=ip;transport=tcp"));
  ASSERT_TRUE(uri_3.getParam("user") == "ip");
  ASSERT_TRUE(uri_3.getParam("transport") == "tcp");
  ASSERT_TRUE(uri_3.setParam("user", "phone"));
  ASSERT_TRUE(uri_3.setParam("transport", "udp"));
  ASSERT_TRUE(uri_3.setParam("pname_1", "pvalue_1"));
  ASSERT_TRUE(uri_3.getParam("user") == "phone");
  ASSERT_TRUE(uri_3.getParam("transport") == "udp");
  ASSERT_TRUE(uri_3.getParam("pname_1") == "pvalue_1");
  ASSERT_TRUE(uri_3.setScheme("sips"));
  ASSERT_TRUE(uri_3.getScheme() == "sips");
  ASSERT_STREQ(uri_3.getIdentity().c_str(), "sips:+12127773456@atlanta.com:5060");

  SIP::SIPURI uri_4("sip:localhost?h1=v1&h2=v2");
  ASSERT_TRUE(uri_4.setUserInfo("alice"));
  ASSERT_TRUE(uri_4.getUser() == "alice");
  ASSERT_TRUE(uri_4.setUserInfo("bob"));
  ASSERT_TRUE(uri_4.getUser() == "bob");
  ASSERT_TRUE(uri_4.setUserInfo("alice:alicepass"));
  ASSERT_TRUE(uri_4.getUser() == "alice");
  ASSERT_TRUE(uri_4.getPassword() == "alicepass");
  ASSERT_TRUE(uri_4.setUserInfo(0));
  ASSERT_TRUE(uri_4.data() == "sip:localhost?h1=v1&h2=v2");
  ASSERT_TRUE(uri_4.setUserInfo(0));
  ASSERT_TRUE(uri_4.data() == "sip:localhost?h1=v1&h2=v2");
  ASSERT_TRUE(uri_4.setHostPort("192.168.0.152:5060"));
  ASSERT_TRUE(uri_4.setParams(";user=ip;transport=tcp"));
  ASSERT_TRUE(uri_4.setHostPort("10.0.0.1:5060"));

  std::string user = "John Smith";
  SIP::SIPURI::escapeUser(user, user.c_str());
  ASSERT_TRUE(user == "John%20Smith");
  ASSERT_TRUE(uri_4.setUserInfo(user.c_str()));
  ASSERT_TRUE(uri_4.getUser() == user);
  SIP::SIPURI::unescape(user, user.c_str());

  ASSERT_FALSE(uri_4.setUserInfo(user.c_str()));
  ASSERT_TRUE(uri_4.getUser() == "John%20Smith");
  ASSERT_TRUE(uri_4.setHeaders("?header_1=value1&header2=value2"));
  ASSERT_TRUE(uri_4.getHeaders() == "?header_1=value1&header2=value2");

  OSS::SIP::SIPURI::header_tokens headers;
  ASSERT_TRUE(uri_4.getHeaders(headers));
  ASSERT_STREQ(headers["header_1"].c_str(), "value1"); 
  ASSERT_STREQ(headers["header2"].c_str(), "value2");
  ASSERT_FALSE(uri_4.setHeaders("?header_1=val ue1&header2=value2"));

  ASSERT_TRUE(!SIP::SIPURI::verify("sip:alice:mypassword@invalid_hostpart.com:5070;param=xxx"));
  ASSERT_TRUE(SIP::SIPURI::verify("sip:alice:mypassword@validhostpart.com:5070;param=xxx"));


  SIP::SIPURI uri_5("sip:10.0.0.10");
  ASSERT_TRUE(uri_5.setParam("lr", ""));
  ASSERT_TRUE(uri_5.setParam("ftag", "001"));
  ASSERT_TRUE(uri_5.hasParam("lr"));
  ASSERT_STREQ(uri_5.data().c_str(), "sip:10.0.0.10;lr;ftag=001");
  ASSERT_STREQ(uri_5.getParam("ftag").c_str(), "001");

  {
  SIP::SIPURI uri_6("sip:user@10.0.0.1:5060;transport=udp");
  std::string host, port, transport;
  host = uri_6.getHost();
  port = uri_6.getPort();
  transport = uri_6.getParam("transport");
  ASSERT_STREQ(host.c_str(), "10.0.0.1");
  ASSERT_STREQ(port.c_str(), "5060");
  }

}
