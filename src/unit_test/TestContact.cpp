
#include "gtest/gtest.h"
#include "OSS/OSS.h"
#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPMessage.h"

TEST(ParserTest, test_contact_parser)
{
  OSS::SIP::SIPContact contact_0("sip:user1@host1:5060;user=phone, sip:user2@host2:5060;user=phone");
  ASSERT_TRUE(contact_0.getSize() == 2);

  OSS::SIP::SIPContact contact_1("<sip:localhost;user=phone>;method=\"INVITE, ACK, BYE\", sip:user1@host1:5060;user=phone, sip:user2@host2:5060;user=phone");
  ASSERT_TRUE(contact_1.getSize() == 3);

  OSS::SIP::ContactURI uri_0;
  OSS::SIP::ContactURI uri_1;
  OSS::SIP::ContactURI uri_2;
  OSS::SIP::ContactURI uri_3;

  ASSERT_TRUE(contact_1.getAt(uri_0, 0));
  ASSERT_TRUE(contact_1.getAt(uri_1, 1));
  ASSERT_TRUE(contact_1.getAt(uri_2, 2));
  ASSERT_TRUE(!contact_1.getAt(uri_3, 3));

  ASSERT_TRUE(uri_0.data() == "<sip:localhost;user=phone>;method=\"INVITE, ACK, BYE\"");
  ASSERT_TRUE(uri_1.data() == "sip:user1@host1:5060;user=phone");
  ASSERT_TRUE(uri_2.data() == "sip:user2@host2:5060;user=phone");
  ASSERT_STREQ(uri_3.data().c_str(), OSS::SIP::SIPURI::EMPTY_URI);

  ASSERT_TRUE(contact_1.setAt("sip:user3@host3:5060;user=phone", 1));
  ASSERT_TRUE(contact_1.getAt(uri_1, 1));
  ASSERT_TRUE(uri_1.data() == "sip:user3@host3:5060;user=phone");

  ASSERT_TRUE(contact_1.setAt("<sip:user4@host4;user=phone>;method=\"INVITE, ACK, BYE\"", 0));
  ASSERT_TRUE(contact_1.getAt(uri_1, 0));
  ASSERT_TRUE(uri_1.data() == "<sip:user4@host4;user=phone>;method=\"INVITE, ACK, BYE\"");

  ASSERT_TRUE(contact_1.setAt("sip:user5@host5:5060;user=phone", 2));
  ASSERT_TRUE(contact_1.getAt(uri_1, 2));
  ASSERT_TRUE(uri_1.data() == "sip:user5@host5:5060;user=phone");

  ASSERT_TRUE(!contact_1.setAt("sip:user5@host5:5060;user=phone", 4));

  OSS::SIP::SIPContact astraContact("\"7143388861\" <sip:7143388861@10.10.10.84:5060>;expires=3600;+sip.instance=\"<urn:uuid:00000000-0000-1000-8000-00085D1BB667>\"");
  OSS::SIP::ContactURI astraCuri;
  ASSERT_TRUE(astraContact.getAt(astraCuri, 0));
  ASSERT_STREQ(astraCuri.data().c_str(), "\"7143388861\" <sip:7143388861@10.10.10.84:5060>;expires=3600;+sip.instance=\"<urn:uuid:00000000-0000-1000-8000-00085D1BB667>\"");
  OSS::SIP::SIPURI astraUri(astraCuri.getURI());
  ASSERT_STREQ(astraUri.getHostPort().c_str(), "10.10.10.84:5060");

  OSS::SIP::SIPMessage msg_7142040030(
    "SIP/2.0 200 OK\r\n"
    "From: 7142040030 <sip:7142040030@69.0.0.15>;tag=2874d2c1eca419d6o0\r\n"
    "To: 7142040030  <sip:7142040030@69.0.0.15>;tag=69.0.0.15+1+0+3810e05b\r\n"
    "Via: SIP/2.0/UDP 199.0.0.5:5060;rport=5060;branch=z9hG4bKe23c9190-2909-11e0-bb4e-0015177b0998\r\n"
    "Server: DC-SIP/2.0\r\n"
    "Content-Length: 0\r\n"
    "Call-ID: bc1af2d7-c0f38724@10.10.10.53\r\n"
    "CSeq: 70514 REGISTER\r\n"
    "Contact: <sip:7142040030@199.0.0.5:5060;sbc-reg=14400287697715549924>;Expires=3600\r\n"
    "Contact: <sip:Bl3enDzGLly-kesf8TiMT5Dy-N00IA7ygTt5lQgWhovklFjUA@69.0.0.3:5060>;EXPIRES=822\r\n");

  std::vector<std::string> bindings;
  ASSERT_TRUE(OSS::SIP::SIPContact::msgGetContacts(&msg_7142040030, bindings) != 0);
  std::string contact_7142040030 = bindings[0];
  ASSERT_STREQ(contact_7142040030.c_str(), "<sip:7142040030@199.0.0.5:5060;sbc-reg=14400287697715549924>;Expires=3600");
}
