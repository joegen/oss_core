#include "gtest/gtest.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPVia.h"
#include <iostream>
#include <sstream>

using namespace OSS;
using OSS::SIP::CRLF;

TEST(ParserTest, test_via_parser)
{
  std::ostringstream msg;
  msg << "INVITE sip:9001@192.168.0.152 SIP/2.0" << CRLF;
  msg << "To: <sip:9001@192.168.0.152>" << CRLF;
  msg << "From: 9011<sip:9011@192.168.0.103>;tag=6657e067" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.1;branch=001;rport, SIP/2.0/UDP 192.168.0.2;branch=002, SIP/2.0/UDP 192.168.0.3;branch=003" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.4;branch=004" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.5;branch=005;rport=9090;received=54.242.115.5" << CRLF;
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

  std::string hdrValue = message.hdrGet(OSS::SIP::HDR_VIA);
  ASSERT_TRUE(!hdrValue.empty());

  SIP::SIPVia via_0;
  via_0 = hdrValue; // test string copy operator
  ASSERT_TRUE(via_0.data() == hdrValue);

  SIP::SIPVia via_1(via_0); // test copy constructor
  ASSERT_TRUE(via_1.data() == hdrValue);

  via_1 = via_1; // test copy self operator
  ASSERT_TRUE(via_1.data() == hdrValue);

  ASSERT_TRUE(via_1.getBranch() == "001");
  ASSERT_TRUE(via_1.hasParam("rport"));
  ASSERT_TRUE(via_1.hasParam("RpOrT"));
  ASSERT_TRUE(via_1.getRPort().empty());
  ASSERT_TRUE(via_1.getSentBy() == "192.168.0.1");
  ASSERT_TRUE(via_1.getTransport() == "UDP");

  ASSERT_TRUE(via_1.setParam("rport", "5060"));
  ASSERT_TRUE(via_1.getRPort() == "5060");

  ASSERT_TRUE(via_1.setParam("received", "192.168.0.153"));
  ASSERT_TRUE(via_1.getReceived() == "192.168.0.153");

  ASSERT_TRUE(via_1.setParam("rport", "5070"));
  ASSERT_TRUE(via_1.getRPort() == "5070");

  bool hasThrown = false;
  try
  {
    via_1.setParam("rport", "\"5060\""); /// pvalue does not allow quotes
  }
  catch( ... )
  {
    hasThrown = true;
  }
  ASSERT_TRUE(hasThrown);

  //
  // Test handling of bottom vias
  //
  std::string bottomRPort;
  ASSERT_TRUE(SIP::SIPVia::msgGetBottomViaRPort(&message, bottomRPort));
  ASSERT_STREQ(bottomRPort.c_str(), "9090");

  std::string bottomReceived;
  ASSERT_TRUE(SIP::SIPVia::msgGetBottomViaReceived(&message, bottomReceived));
  ASSERT_STREQ(bottomReceived.c_str(), "54.242.115.5");

  std::string bottomSentBy;
  ASSERT_TRUE(SIP::SIPVia::msgGetBottomViaSentBy(&message, bottomSentBy));
  ASSERT_STREQ(bottomSentBy.c_str(), "192.168.0.5");

  //
  // Test popping top vias
  //
  ASSERT_TRUE(message.hdrGetSize(OSS::SIP::HDR_VIA) == 3);
  std::string msgTopVia;
  ASSERT_TRUE(SIP::SIPVia::msgPopTopVia(&message, msgTopVia));
  ASSERT_TRUE(msgTopVia == "SIP/2.0/UDP 192.168.0.1;branch=001;rport");
  message.commitData();
  ASSERT_TRUE(message.hdrGetSize(OSS::SIP::HDR_VIA) == 3);
  //std::cout << message.data() << std::endl;
  ASSERT_TRUE(SIP::SIPVia::msgPopTopVia(&message, msgTopVia));
  ASSERT_TRUE(msgTopVia == "SIP/2.0/UDP 192.168.0.2;branch=002");
  message.commitData();
  ASSERT_TRUE(message.hdrGetSize(OSS::SIP::HDR_VIA) == 3);
  //std::cout << message.data() << std::endl;
  ASSERT_TRUE(SIP::SIPVia::msgPopTopVia(&message, msgTopVia));
  ASSERT_TRUE(msgTopVia == "SIP/2.0/UDP 192.168.0.3;branch=003");
  message.commitData();
  //std::cout << message.data() << std::endl;
  ASSERT_TRUE(message.hdrGetSize(OSS::SIP::HDR_VIA) == 2);
  ASSERT_TRUE(SIP::SIPVia::msgPopTopVia(&message, msgTopVia));
  ASSERT_TRUE(msgTopVia == "SIP/2.0/UDP 192.168.0.4;branch=004");
  message.commitData();
  //std::cout << message.data() << std::endl;
  ASSERT_TRUE(message.hdrGetSize(OSS::SIP::HDR_VIA) == 1);
  ASSERT_TRUE(SIP::SIPVia::msgGetTopVia(&message, msgTopVia));
  ASSERT_TRUE(msgTopVia == "SIP/2.0/UDP 192.168.0.5;branch=005;rport=9090;received=54.242.115.5");

  SIP::SIPVia::msgAddVia(&message, "SIP/2.0/UDP 192.168.0.4;branch=004");
  ASSERT_TRUE(message.hdrGetSize(OSS::SIP::HDR_VIA) == 2);
  message.commitData();
  //std::cout << message.data() << std::endl;


  std::string multiple = "SIP/2.0/UDP 192.168.0.1:5060;branch=0001, SIP/2.0/UDP 192.168.0.2:5060;branch=0002, SIP/2.0/UDP 192.168.0.3:5060;branch=0003";
  ASSERT_TRUE(SIP::SIPVia::countElements(multiple) == 3);
  std::vector<std::string> elements;
  ASSERT_TRUE(SIP::SIPVia::splitElements(multiple, elements) == 3);
  ASSERT_TRUE(elements.size() == 3);
  ASSERT_TRUE(elements[0] == "SIP/2.0/UDP 192.168.0.1:5060;branch=0001");
  ASSERT_TRUE(elements[1] == "SIP/2.0/UDP 192.168.0.2:5060;branch=0002");
  ASSERT_TRUE(elements[2] == "SIP/2.0/UDP 192.168.0.3:5060;branch=0003");

  std::string topVia;
  ASSERT_TRUE(SIP::SIPVia::getTopVia(multiple, topVia));
  ASSERT_TRUE(topVia == "SIP/2.0/UDP 192.168.0.1:5060;branch=0001");

  std::string tail = SIP::SIPVia::popTopVia(multiple, topVia);
  ASSERT_TRUE(topVia == "SIP/2.0/UDP 192.168.0.1:5060;branch=0001");

  tail = SIP::SIPVia::popTopVia(tail, topVia);
  ASSERT_TRUE(topVia == "SIP/2.0/UDP 192.168.0.2:5060;branch=0002");

  tail = SIP::SIPVia::popTopVia(tail, topVia);
  ASSERT_TRUE(topVia == "SIP/2.0/UDP 192.168.0.3:5060;branch=0003");
  ASSERT_TRUE(tail.empty());



  std::string blankRPORT = "SIP/2.0/UDP 211.126.199.245:5060;branch=z9hG4bK-968922472;rport";
  std::string rportBuff;
  SIP::SIPVia::getRPort(blankRPORT, rportBuff);
  ASSERT_TRUE(rportBuff.empty());

}
