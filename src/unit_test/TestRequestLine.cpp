#include "gtest/gtest.h"
#include "OSS/OSS.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SIPStatusLine.h"
#include <iostream>
#include <sstream>

using namespace OSS;
using OSS::SIP::CRLF;


TEST(ParserTest, test_requestline_parser)
{
  std::ostringstream msg;
  msg << "INVITE sip:9001@192.168.0.152 SIP/2.0" << CRLF;
  msg << "To: <sip:9001@192.168.0.152>" << CRLF;
  msg << "From: 9011<sip:9011@192.168.0.103>;tag=6657e067" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.152:9644;branch=z9hG4bK-d87543-419889160-1--d87543-;rport" << CRLF;
  msg << "Call-ID: 885e5e180c04c509" << CRLF;
  msg << "CSeq: 1 INVITE" << CRLF;
  msg << "Contact: <sip:9011@192.168.0.152:9644>" << CRLF;
  msg << "Max-Forwards: 70" << CRLF;
  msg << "Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, NOTIFY, MESSAGE, SUBSCRIBE, INFO" << CRLF;
  msg << "Content-Type: application/sdp" << CRLF;
  msg << "Route: <sip:10.0.0.1;lr>" << CRLF;
  msg << "Route: <sip:10.0.0.2;lr>" << CRLF;
  msg << "Route: <sip:10.0.0.3;lr>" << CRLF;
  msg << "Content-Length: 237" << CRLF;
  msg << CRLF; /// End of headers
  msg << "v=0" << CRLF;
  msg << "o=- 10818229 10818359 IN IP4 192.168.0.152" << CRLF;
  msg << "s=-" << CRLF;
  msg << "c=IN IP4 192.168.0.152" << CRLF;
  msg << "t=0 0" << CRLF;
  msg << "m=audio 35000 RTP/AVP 0 8 101" << CRLF;
  msg << "a=fmtp:101 0-15" << CRLF;
  msg << "a=rtpmap:101 telephone-event/8000" << CRLF;
  msg << "a=sendrecv" << CRLF;
  SIP::SIPMessage message( msg.str() );
  message.parse();

  SIP::SIPRequestLine rLine;
  rLine = message.startLine();

  std::string method;
  ASSERT_TRUE(rLine.getMethod(method));
  ASSERT_TRUE(method == "INVITE");
  ASSERT_TRUE(rLine.setMethod("REGISTER"));
  ASSERT_TRUE(rLine.getMethod(method));
  ASSERT_TRUE(method == "REGISTER");

  std::string uri;
  ASSERT_TRUE(rLine.getURI(uri));
  ASSERT_TRUE(uri == "sip:9001@192.168.0.152");
  ASSERT_TRUE(rLine.setURI("sip:alice@atlanta.com"));
  ASSERT_TRUE(rLine.getURI(uri));
  ASSERT_TRUE(uri == "sip:alice@atlanta.com");

  std::string version;
  ASSERT_TRUE(rLine.getVersion(version));
  ASSERT_TRUE(version == "SIP/2.0");
  ASSERT_TRUE(rLine.setVersion("SIP/3.0")); // just for kicks.
  ASSERT_TRUE(rLine.getVersion(version));
  ASSERT_TRUE(version == "SIP/3.0");

  SIP::SIPRequestLine blank;
  ASSERT_TRUE(blank.setMethod("INVITE"));
  ASSERT_TRUE(blank.setURI("sip:bob@biloxy.com"));
  ASSERT_TRUE(blank.setVersion("SIP/2.0"));
  ASSERT_TRUE(blank.data() == "INVITE sip:bob@biloxy.com SIP/2.0");

  SIP::SIPMessage::Ptr response = message.createResponse(200, "How sweet!", "to-tag", "<sip:mycontact:5060>");
  SIP::SIPStatusLine sLine;
  sLine = response->startLine();

  ASSERT_TRUE(sLine.getVersion(version));
  ASSERT_TRUE(version == "SIP/2.0");
  ASSERT_TRUE(sLine.setVersion("SIP/3.0"));
  ASSERT_TRUE(sLine.getVersion(version));
  ASSERT_TRUE(version == "SIP/3.0");

  std::string code;
  ASSERT_TRUE(sLine.getStatusCode(code));
  ASSERT_TRUE(code == "200");
  ASSERT_TRUE(sLine.setStatusCode("183"));
  ASSERT_TRUE(sLine.getStatusCode(code));
  ASSERT_TRUE(code == "183");

  std::string phrase;
  ASSERT_TRUE(sLine.getReasonPhrase(phrase));
  ASSERT_TRUE(phrase == "How sweet!");
  ASSERT_TRUE(sLine.setReasonPhrase("All right im ringing!"));
  ASSERT_TRUE(sLine.getReasonPhrase(phrase));
  ASSERT_TRUE(phrase == "All right im ringing!");

  ASSERT_TRUE(sLine.data() == "SIP/3.0 183 All right im ringing!");
}

