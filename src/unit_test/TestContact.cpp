
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
  ASSERT_TRUE(uri_3.data().empty());

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

TEST(ParserTest, test_comma_in_user_info)
{
  const char* pMsg = "REGISTER sip:sip.biloxy.com SIP/2.0\r\n" 
    "Via: SIP/2.0/UDP 10.5.10.50:5060;branch=z9hG4bKjar7i5004g50u2hbb3e1.1\r\n" 
    "From: <sip:%3cOpenUC%20%23,ie,%2019195551212%3e@sip.biloxy.com>;tag=qcccWUSx7\r\n" 
    "To: sip:%3cOpenUC%20%23,ie,%2019195551212%3e@sip.biloxy.com\r\n" 
    "CSeq: 34 REGISTER\r\n" 
    "Call-ID: bvxlIej1fx\r\n" 
    "Max-Forwards: 69\r\n" 
    "Supported: outbound\r\n" 
    "Contact: <sip:%3cOpenUC%20%23,ie,%2019195551212%3e-1sqld00p76pu8@10.5.10.50:5060;transport=udp>;+sip.instance=\"<urn:uuid:c2bcbd2b-18e5-42f9-913e-1513cc37ada8>\"\r\n" 
    "Expires: 3600\r\n" 
    "User-Agent: LinphoneIphone/2.2.4.1-27-ge8ab36a (belle-sip/1.3.3)\r\n" 
    "Route: <sip:10.5.10.245:5060;lr>\r\n"; 

  OSS::SIP::SIPMessage msg(pMsg);
  
  std::vector<std::string> bindings;
  ASSERT_TRUE(OSS::SIP::SIPContact::msgGetContacts(&msg, bindings) != 0);

  ASSERT_TRUE(bindings.size() == 1);
  std::string contact = bindings[0];
  
  ASSERT_STREQ(contact.c_str(), "<sip:%3cOpenUC%20%23,ie,%2019195551212%3e-1sqld00p76pu8@10.5.10.50:5060;transport=udp>;+sip.instance=\"<urn:uuid:c2bcbd2b-18e5-42f9-913e-1513cc37ada8>\"");

  OSS::SIP::ContactURI hContact(contact);
  std::string user = hContact.getUser();
  ASSERT_STREQ(user.c_str(), "%3cOpenUC%20%23,ie,%2019195551212%3e-1sqld00p76pu8");
}
