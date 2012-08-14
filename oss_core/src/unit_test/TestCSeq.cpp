#include "gtest/gtest.h"
#include "OSS/OSS.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPCSeq.h"
#include <iostream>
#include <sstream>

using OSS::SIP::CRLF;

TEST(ParserTest, test_cseq_parser)
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
  OSS::SIP::SIPMessage message( msg.str() );
  message.parse();

  std::string hdrValue = message.hdrGet("cseq");
  ASSERT_TRUE(!hdrValue.empty());

  OSS::SIP::SIPCSeq cseq_0(hdrValue);
  cseq_0 = cseq_0;
  ASSERT_TRUE(cseq_0.getNumber() == "1");
  ASSERT_TRUE(cseq_0.setNumber("314159"));
  ASSERT_TRUE(cseq_0.getNumber() == "314159");
  ASSERT_TRUE(cseq_0.getMethod() == "INVITE");
  ASSERT_TRUE(cseq_0.setMethod("ACK"));
  ASSERT_TRUE(cseq_0.getMethod() == "ACK");
}

