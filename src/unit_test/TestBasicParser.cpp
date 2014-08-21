#include "gtest/gtest.h"
#include "OSS/OSS.h"
#include "OSS/SIP/SIPParser.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/ABNF/ABNFParser.h"
#include "OSS/ABNF/ABNFSIPUserInfo.h"
#include "OSS/ABNF/ABNFSIPHostName.h"
#include "OSS/ABNF/ABNFSIPIPV4Address.h"
#include "OSS/ABNF/ABNFSIPIPV6Address.h"
#include "OSS/ABNF/ABNFSIPHost.h"
#include "OSS/ABNF/ABNFSIPHostPort.h"
#include "OSS/ABNF/ABNFSIPURIParameters.h"
#include "OSS/ABNF/ABNFSIPURIHeaders.h"
#include "OSS/ABNF/ABNFSIPURI.h"
#include "OSS/ABNF/ABNFSIPAbsoluteURI.h"
#include "OSS/ABNF/ABNFSIPRequestURI.h"
#include "OSS/ABNF/ABNFSIPVersion.h"
#include "OSS/ABNF/ABNFSIPRequestLine.h"
#include "OSS/ABNF/ABNFSIPStatusLine.h"

using namespace OSS::SIP;
using namespace OSS::ABNF;


TEST(ParserTest, test_header_tokenizer)
{
  std::ostringstream strm;
  strm << "INVITE sip:alice@atlanta.com" << CRLF;
  strm << "To: \"Alice\" <sip:alice@atlanta.com>" << LF;
  strm << "From: \"Bob\" <sip:bob@biloxy.com>" << CR;
  strm << "Contact:                " << CRLF;
  strm << "  <sip:bob@mach1.biloxy.com>" << CR;
  SIPHeaderTokens lines;
  SIPMessage::headerTokenize(lines, strm.str());
  ASSERT_TRUE(lines.size() == 4);
  ASSERT_TRUE(lines[0] == "INVITE sip:alice@atlanta.com");
  ASSERT_TRUE(lines[1] == "To: \"Alice\" <sip:alice@atlanta.com>");
  ASSERT_TRUE(lines[2] == "From: \"Bob\" <sip:bob@biloxy.com>");
  ASSERT_TRUE(lines[3] == "Contact: <sip:bob@mach1.biloxy.com>");
}

TEST(ParserTest, test_header_splitter)
{
  std::string header = "To: \"Alice\" <sip:alice@atlanta.com>\r\n";
  std::string name;
  std::string value;
  ASSERT_TRUE(SIPMessage::headerSplit(header, name, value));
  ASSERT_TRUE(name=="To");
  ASSERT_TRUE(value=="\"Alice\" <sip:alice@atlanta.com>");
}

TEST(ParserTest, test_message_splitter)
{
  ///Test header only message
  std::ostringstream h1;
  h1 << CRLF << CRLF << "INVITE sip:alice@atlanta.com" << CRLF;
  h1 << "To: \"Alice\" <sip:alice@atlanta.com>" << LF;
  h1 << "From: \"Bob\" <sip:bob@biloxy.com>" << CR;
  std::string messageBoby;
  std::string messageHeaders;
  std::string h = h1.str();
  ASSERT_TRUE(SIPMessage::messageSplit( h, messageHeaders, messageBoby ));
  ASSERT_TRUE(messageHeaders == h);
  ASSERT_TRUE(h1.str() != h);

  ///test CRLFCRLF terminated headers
  std::ostringstream h2;
  h2 << "INVITE sip:alice@atlanta.com" << CRLF;
  h2 << "To: \"Alice\" <sip:alice@atlanta.com>" << CRLF;
  h2 << "From: \"Bob\" <sip:bob@biloxy.com>";
  std::string h2Header = h2.str() + CRLFCRLF;
  ASSERT_TRUE(SIPMessage::messageSplit(h2Header, messageHeaders, messageBoby));
  ASSERT_TRUE(messageHeaders == h2.str());

  ///test LFLF terminated headers
  std::ostringstream h3;
  h3 << "INVITE sip:alice@atlanta.com" << CRLF;
  h3 << "To: \"Alice\" <sip:alice@atlanta.com>" << CRLF;
  h3 << "From: \"Bob\" <sip:bob@biloxy.com>";
  std::string h3Header = h3.str() + LFLF;
  ASSERT_TRUE(SIPMessage::messageSplit(h3Header, messageHeaders, messageBoby));
  ASSERT_TRUE(messageHeaders == h3.str());

  //test CRCR terminated headers
  std::ostringstream h4;
  h4 << "INVITE sip:alice@atlanta.com" << CRLF;
  h4 << "To: \"Alice\" <sip:alice@atlanta.com>" << CRLF;
  h4 << "From: \"Bob\" <sip:bob@biloxy.com>";
  std::string h4Header = h4.str() + CRCR;
  ASSERT_TRUE(SIPMessage::messageSplit(h4Header, messageHeaders, messageBoby));
  ASSERT_TRUE(messageHeaders == h4.str());
  
  //test CRLFCRLF terminated headers with a body
  std::ostringstream h5;
  h5 << "INVITE sip:alice@atlanta.com" << CRLF;
  h5 << "To: \"Alice\" <sip:alice@atlanta.com>" << CRLF;
  h5 << "From: \"Bob\" <sip:bob@biloxy.com>";
  std::string h5Header = h5.str() + CRLFCRLF;

  std::string b1;
  b1 += '\0';
  b1 += '1';
  b1 += '\0';
  b1 += '2';
  b1 += '\0';
  b1 += CRLF;
  h5Header = h5Header + b1;

  ASSERT_TRUE(SIPMessage::messageSplit(h5Header, messageHeaders, messageBoby));
  ASSERT_TRUE(messageHeaders == h5.str());
  ASSERT_TRUE(messageBoby.size() == 7);
  ASSERT_TRUE(messageBoby[1] == '1' && messageBoby[3] =='2');
  ASSERT_TRUE(messageBoby == b1);

  //test CRLFCRLF terminated headers with a body using HeaderTokens overload
  SIPHeaderTokens headerTokens;
  ASSERT_TRUE(SIPMessage::messageSplit(h5Header, headerTokens, messageBoby));
  ASSERT_TRUE(headerTokens.size() == 3);
  ASSERT_TRUE(headerTokens[0] == "INVITE sip:alice@atlanta.com");
  ASSERT_TRUE(headerTokens[1] == "To: \"Alice\" <sip:alice@atlanta.com>");
  ASSERT_TRUE(headerTokens[2] == "From: \"Bob\" <sip:bob@biloxy.com>");
  ASSERT_TRUE(messageBoby.size() == 7);
  ASSERT_TRUE(messageBoby[1] == '1' && messageBoby[3] =='2');
  ASSERT_TRUE(messageBoby == b1);
}

TEST(ParserTest, test_message_construction)
{
  SIPMessage crlfConsumer;
  std::string crlf = "\r\n\r\n";
  crlfConsumer.consume(crlf.c_str(), crlf.c_str() + crlf.length());

  std::ostringstream msg;
  msg << CRLF << CRLF << "INVITE sip:9001@192.168.0.152 SIP/2.0" << CRLF;
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
  msg << "Content-Length: 178" << CRLF;
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

  SIPMessage consumer;
  consumer.consume(msg.str().c_str(), msg.str().c_str() + msg.str().length());

  SIPMessage message(msg.str());
  boost::tribool isrequest = message.isRequest();
  if(!isrequest)
    ASSERT_TRUE(false);

  std::string tid;
  ASSERT_TRUE(message.getTransactionId(tid));
  ASSERT_TRUE(tid == "invite1z9hG4bK-d87543-419889160-1--d87543-");

  ASSERT_TRUE(message.hdrPresent(OSS::SIP::HDR_FROM) == 1);
  ASSERT_TRUE(message.hdrPresent(OSS::SIP::HDR_TO) == 1);
  ASSERT_TRUE(message.hdrPresent(OSS::SIP::HDR_VIA) == 1);
  ASSERT_TRUE(message.hdrPresent(OSS::SIP::HDR_CONTACT) == 1);
  ASSERT_TRUE(message.hdrPresent("CSEQ") == 1);
  ASSERT_TRUE(message.hdrPresent("CoNtenT-LeNgTh") == 1);
  ASSERT_TRUE(message.hdrPresent(OSS::SIP::HDR_P_ASSERTED_IDENTITY) == 0);

  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_FROM) == "9011<sip:9011@192.168.0.103>;tag=6657e067");
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_TO) != "9011<sip:9011@192.168.0.103>;tag=6657e067");
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_P_ASSERTED_IDENTITY) == "");
  message.hdrSet(OSS::SIP::HDR_P_ASSERTED_IDENTITY, "<sip:9011@192.168.0.103>");
  message.hdrSet(OSS::SIP::HDR_SERVER, "SIP Parser Test");
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_P_ASSERTED_IDENTITY) == "<sip:9011@192.168.0.103>");

  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_ROUTE, 0) == "<sip:10.0.0.1;lr>");
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_ROUTE, 1) == "<sip:10.0.0.2;lr>");
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_ROUTE, 2) == "<sip:10.0.0.3;lr>");
  message.hdrListAppend(OSS::SIP::HDR_ROUTE, "<sip:10.0.0.4;lr>");
  ASSERT_TRUE(message.hdrGetSize(OSS::SIP::HDR_ROUTE) == 4);
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_ROUTE, 3) == "<sip:10.0.0.4;lr>");

  bool thrown = false;
  try
  {
    message.hdrGet(OSS::SIP::HDR_ROUTE, 4);
  }catch(const std::exception & e)
  {
    thrown = true;
  }
  ASSERT_TRUE(thrown);

  /// Test copy operator
  SIPMessage message_2;
  message_2 = message;
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_FROM) == "9011<sip:9011@192.168.0.103>;tag=6657e067");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_TO) != "9011<sip:9011@192.168.0.103>;tag=6657e067");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_P_ASSERTED_IDENTITY) == "<sip:9011@192.168.0.103>");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_ROUTE, 0) == "<sip:10.0.0.1;lr>");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_ROUTE, 1) == "<sip:10.0.0.2;lr>");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_ROUTE, 2) == "<sip:10.0.0.3;lr>");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_ROUTE, 3) == "<sip:10.0.0.4;lr>");

  /// Test object copying itself
  message_2 = message_2;
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_FROM) == "9011<sip:9011@192.168.0.103>;tag=6657e067");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_TO) != "9011<sip:9011@192.168.0.103>;tag=6657e067");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_P_ASSERTED_IDENTITY) == "<sip:9011@192.168.0.103>");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_ROUTE, 0) == "<sip:10.0.0.1;lr>");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_ROUTE, 1) == "<sip:10.0.0.2;lr>");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_ROUTE, 2) == "<sip:10.0.0.3;lr>");
  ASSERT_TRUE(message_2.hdrGet(OSS::SIP::HDR_ROUTE, 3) == "<sip:10.0.0.4;lr>");
  message_2.hdrSet(OSS::SIP::HDR_SUBJECT, "Testing Commit");
  message_2.commitData();

  SIPMessage::Ptr ack = SIPMessage::Ptr(new SIPMessage);
  *(ack.get()) = message_2;
  size_t methodPos = message_2.startLine().find(' ');
  ASSERT_TRUE(methodPos != std::string::npos);
  ack->startLine() = "ACK";
  ack->startLine() += message_2.startLine().c_str() + methodPos;
  std::string st = ack->startLine();
  ASSERT_TRUE(st == "ACK sip:9001@192.168.0.152 SIP/2.0");

  SIPCSeq cSeq;
  cSeq = message_2.hdrGet(OSS::SIP::HDR_CSEQ);
  std::string ackCSeq = cSeq.getNumber();
  ackCSeq += " ACK";
  ack->hdrSet(OSS::SIP::HDR_CSEQ, ackCSeq);
  ASSERT_TRUE(ack->hdrGet(OSS::SIP::HDR_CSEQ) == "1 ACK");

  ack->body() = "";
  ack->hdrSet(OSS::SIP::HDR_CONTENT_LENGTH, "0");
  ack->hdrSet(OSS::SIP::HDR_CONTENT_TYPE, "text/plain");

  ack->commitData();

  //std::cout << ack->data() << std::endl;
}


TEST(ParserTest, test_compound_segment)
{
     const char* p1 =
        "INVITE sip:01023@domain.com;sipx-userforward=false;sipx-noroute=Voicemail SIP/2.0\r\n"
        "Route: <sip:192.33.44.55:5060;lr>\r\n"
        "Record-Route: <sip:192.33.44.55:5060;lr;sipXecs-rs=%2Aauth%7E.%2Afrom%7ENWQwNTcwMGEtMTNjNC00ZGM5OTk0Ni1kMmUyN2M0ZC0yNDU3YTdkYQ%60%60%21cafd6de877a56a627ee847d97303f437>\r\n"
        "From: <sip:6239255975@10.111.0.93;isup-oli=0;pstn-params=808481808882>;tag=5d05700a-13c4-4dc99946-d2e27c4d-2457a7da\r\n"
        "To: <sip:31002@domain.com?X-sipX-Authidentity=%3Csip:~~id~media%40domain.com%3Bsignature%3D4DC99945::c181beb2880b79b71df6abdf4ec68741>\r\n"
        "Call-Id: CXC-18-5df916f0-5d05700a-13c4-4dc99946-d2e27c4b-728c2fc@10.111.0.93\r\n"
        "Cseq: 1 INVITE\r\n"
        "Via: SIP/2.0/TCP 192.33.44.55;branch=z9hG4bK-sipXecs-d385741ef1c3d1eacb2c99760a5b8afaf585~75565b8987882fcc1706babdcc4d1494\r\n"
        "Via: SIP/2.0/UDP 192.33.44.55;branch=z9hG4bK-sipXecs-d3337d838502a8a1b373e0770970913b69f8~e61d9bee64737c18fc0c54e587fdc527\r\n"
        "Via: SIP/2.0/UDP 10.111.0.93:5060;branch=z9hG4bK-b9e8b4-4dc99946-d2e27c4d-5b13140b\r\n"
        "Max-Forwards: 17\r\n"
        "Contact: <sip:6239255975@10.111.0.93:5060;maddr=10.111.0.93;transport=udp;x-sipX-nonat>\r\n"
        "Referred-By: <sip:10.222.111.11:15060>\r\n"
        "Content-Type: application/SDP\r\n"
        "Content-Length: 372\r\n"
        "Date: Tue, 10 May 2011 20:00:05 GMT\r\n"
        "X-Sipx-Spiral: true\r\n"
        "Expires: 180\r\n"
        "X-Sipx-Authidentity: <sip:Reception-All@domain.com;signature=4DC99945%3A1c4a5c5a287a7298bec176e6f5210dcd>\r\n"
        "\r\n"
        "v=0\r\n"
        "o=BOGUS_UAC 21952 32372 IN IP4 10.111.0.93\r\n"
        "s=SIP Media Capabilities\r\n"
        "c=IN IP4 10.111.0.93\r\n"
        "t=0 0\r\n"
        "m=audio 24354 RTP/AVP 0 8 100\r\n"
        "a=rtpmap:0 PCMU/8000\r\n"
        "a=rtpmap:8 PCMA/8000\r\n"
        "a=rtpmap:100 telephone-event/8000\r\n"
        "a=fmtp:100 0-15\r\n"
        "a=maxptime:20\r\n"
        "a=x-cxc:aT0wNGMzZjdmMzA5MDIzY2EwLTI7Yj0xNjIuOTcuMTgyLjQ7ZD0xNjIuOTcuMTgyLjQ6NTI3NjI7YT0xMC4xMTIuNS45MzoyNDM1NDs=\r\n"
        "a=sendrecv\r\n";

    const char* p2 =
        "INVITE sip:01026@10.57.56.118;x-sipX-nonat SIP/2.0\r\n"
        "Route: <sip:192.33.44.55:5060;lr>\r\n"
        "Record-Route: <sip:192.33.44.55:5060;lr;sipXecs-rs=%2Aauth%7E.%2Afrom%7ENWQwNTcwMGEtMTNjNC00ZGM5OTk0Ni1kMmUyN2M0ZC0yNDU3YTdkYQ%60%60%21cafd6de877a56a627ee847d97303f437>\r\n"
        "From: <sip:6239255975@10.111.0.93;isup-oli=0;pstn-params=808481808882>;tag=5d05700a-13c4-4dc99946-d2e27c4d-2457a7da\r\n"
        "To: <sip:31002@domain.com?X-sipX-Authidentity=%3Csip:~~id~media%40domain.com%3Bsignature%3D4DC99945::c181beb2880b79b71df6abdf4ec68741>\r\n"
        "Call-Id: CXC-18-5df916f0-5d05700a-13c4-4dc99946-d2e27c4b-728c2fc@10.111.0.93\r\n"
        "Cseq: 1 INVITE\r\n"
        "Via: SIP/2.0/TCP 192.33.44.55;branch=z9hG4bK-sipXecs-d3d6be7d51aa602292eb45727c3242b4e554~50d1cd1c36bb2d8b9827d67c21fa8018\r\n"
        "Via: SIP/2.0/TCP 192.33.44.55;branch=z9hG4bK-sipXecs-d367894a40c6f8bc016e187b31b100380dac~75565b8987882fcc1706babdcc4d1494\r\n"
        "Via: SIP/2.0/UDP 192.33.44.55;branch=z9hG4bK-sipXecs-d3337d838502a8a1b373e0770970913b69f8~e61d9bee64737c18fc0c54e587fdc527\r\n"
        "Via: SIP/2.0/UDP 10.111.0.93:5060;branch=z9hG4bK-b9e8b4-4dc99946-d2e27c4d-5b13140b\r\n"
        "Max-Forwards: 15\r\n"
        "Contact: <sip:6239255975@10.111.0.93:5060;maddr=10.111.0.93;transport=udp;x-sipX-nonat>\r\n"
        "Referred-By: <sip:10.222.111.11:15060>\r\n"
        "Content-Type: application/SDP\r\n"
        "Content-Length: 372\r\n"
        "Date: Tue, 10 May 2011 20:00:05 GMT\r\n"
        "X-Sipx-Spiral: true\r\n"
        "Expires: 180\r\n"
        "X-Sipx-Authidentity: <sip:Reception-All@domain.com;signature=4DC99945%3A1c4a5c5a287a7298bec176e6f5210dcd>\r\n"
        "\r\n"
        "v=0\r\n"
        "o=BOGUS_UAC 21952 32372 IN IP4 10.111.0.93\r\n"
        "s=SIP Media Capabilities\r\n"
        "c=IN IP4 10.111.0.93\r\n"
        "t=0 0\r\n"
        "m=audio 24354 RTP/AVP 0 8 100\r\n"
        "a=rtpmap:0 PCMU/8000\r\n"
        "a=rtpmap:8 PCMA/8000\r\n"
        "a=rtpmap:100 telephone-event/8000\r\n"
        "a=fmtp:100 0-15\r\n"
        "a=maxptime:20\r\n"
        "a=x-cxc:aT0wNGMzZjdmMzA5MDIzY2EwLTI7Yj0xNjIuOTcuMTgyLjQ7ZD0xNjIuOTcuMTgyLjQ6NTI3NjI7YT0xMC4xMTIuNS45MzoyNDM1NDs=\r\n"
        "a=sendrecv\r\n";

    const char* p3 =
        "INVITE sip:01020@domain.com;sipx-userforward=false;sipx-noroute=Voicemail SIP/2.0\r\n"
        "Route: <sip:192.33.44.55:5060;lr>\r\n"
        "Record-Route: <sip:192.33.44.55:5060;lr;sipXecs-rs=%2Aauth%7E.%2Afrom%7ENWQwNTcwMGEtMTNjNC00ZGM5OTk0Ni1kMmUyN2M0ZC0yNDU3YTdkYQ%60%60%21cafd6de877a56a627ee847d97303f437>\r\n"
        "From: <sip:6239255975@10.111.0.93;isup-oli=0;pstn-params=808481808882>;tag=5d05700a-13c4-4dc99946-d2e27c4d-2457a7da\r\n"
        "To: <sip:31002@domain.com?X-sipX-Authidentity=%3Csip:~~id~media%40domain.com%3Bsignature%3D4DC99945::c181beb2880b79b71df6abdf4ec68741>\r\n"
        "Call-Id: CXC-18-5df916f0-5d05700a-13c4-4dc99946-d2e27c4b-728c2fc@10.111.0.93\r\n"
        "Cseq: 1 INVITE\r\n"
        "Via: SIP/2.0/TCP 192.33.44.55;branch=z9hG4bK-sipXecs-d387f3b353eab0006eae9f90044ac6042d80~75565b8987882fcc1706babdcc4d1494\r\n"
        "Via: SIP/2.0/UDP 192.33.44.55;branch=z9hG4bK-sipXecs-d3337d838502a8a1b373e0770970913b69f8~e61d9bee64737c18fc0c54e587fdc527\r\n"
        "Via: SIP/2.0/UDP 10.111.0.93:5060;branch=z9hG4bK-b9e8b4-4dc99946-d2e27c4d-5b13140b\r\n"
        "Max-Forwards: 17\r\n"
        "Contact: <sip:6239255975@10.111.0.93:5060;maddr=10.111.0.93;transport=udp;x-sipX-nonat>\r\n"
        "Referred-By: <sip:10.222.111.11:15060>\r\n"
        "Content-Type: application/SDP\r\n"
        "Content-Length: 372\r\n"
        "Date: Tue, 10 May 2011 20:00:05 GMT\r\n"
        "X-Sipx-Spiral: true\r\n"
        "Expires: 180\r\n"
        "X-Sipx-Authidentity: <sip:Reception-All@domain.com;signature=4DC99945%3A1c4a5c5a287a7298bec176e6f5210dcd>\r\n"
        "\r\n"
        "v=0\r\n"
        "o=BOGUS_UAC 21952 32372 IN IP4 10.111.0.93\r\n"
        "s=SIP Media Capabilities\r\n"
        "c=IN IP4 10.111.0.93\r\n"
        "t=0 0\r\n"
        "m=audio 24354 RTP/AVP 0 8 100\r\n"
        "a=rtpmap:0 PCMU/8000\r\n"
        "a=rtpmap:8 PCMA/8000\r\n"
        "a=rtpmap:100 telephone-event/8000\r\n"
        "a=fmtp:100 0-15\r\n"
        "a=maxptime:20\r\n"
        "a=x-cxc:aT0wNGMzZjdmMzA5MDIzY2EwLTI7Yj0xNjIuOTcuMTgyLjQ7ZD0xNjIuOTcuMTgyLjQ6NTI3NjI7YT0xMC4xMTIuNS45MzoyNDM1NDs=\r\n"
        "a=sendrecv\r\n";

    const char* p4 =
        "ACK sip:01004@domain.com;sipx-userforward=false;sipx-noroute=Voicemail SIP/2.0\r\n"
        "Route: <sip:192.33.44.55:5060;lr>\r\n"
        "Contact: <sip:6239255975@10.111.0.93:5060;maddr=10.111.0.93;transport=udp;x-sipX-nonat>\r\n"
        "From: <sip:6239255975@10.111.0.93;isup-oli=0;pstn-params=808481808882>;tag=5d05700a-13c4-4dc99946-d2e27c4d-2457a7da\r\n"
        "To: <sip:31002@domain.com?X-sipX-Authidentity=%3Csip:~~id~media%40domain.com%3Bsignature%3D4DC99945::c181beb2880b79b71df6abdf4ec68741>;tag=9efd44cb\r\n"
        "Call-Id: CXC-18-5df916f0-5d05700a-13c4-4dc99946-d2e27c4b-728c2fc@10.111.0.93\r\n"
        "Cseq: 1 ACK\r\n"
        "Max-Forwards: 20\r\n"
        "Via: SIP/2.0/TCP 192.33.44.55;branch=z9hG4bK-sipXecs-d357c1eac55c03e73442d84a33313d4418d8~75565b8987882fcc1706babdcc4d1494\r\n"
        "Content-Length: 0\r\n\r\n";

    std::stringstream compoundBuffer;
    compoundBuffer << p1 << p2 << p3 << p4;

    SIPMessage m1, m2, m3, m4;
    std::size_t bytesRead;
    ASSERT_TRUE(m1.read(compoundBuffer, bytesRead));
    ASSERT_TRUE(m2.read(compoundBuffer, bytesRead));
    ASSERT_TRUE(m3.read(compoundBuffer, bytesRead));
    ASSERT_TRUE(m4.read(compoundBuffer, bytesRead));
    
}

TEST(ParserTest, test_corrupted_segment)
{
  const char* segment =
        "t=0 0\r\n"
        "m=audio 24354 RTP/AVP 0 8 100\r\n"
        "a=rtpmap:0 PCMU/8000\r\n"
        "a=rtpmap:8 PCMA/8000\r\n"
        "a=rtpmap:100 telephone-event/8000\r\n"
        "a=fmtp:100 0-15\r\n"
        "a=maxptime:20\r\n"
        "a=x-cxc:aT0wNGMzZjdmMzA5MDIzY2EwLTI7Yj0xNjIuOTcuMTgyLjQ7ZD0xNjIuOTcuMTgyLjQ6NTI3NjI7YT0xMC4xMTIuNS45MzoyNDM1NDs=\r\n"
        "a=sendrecv\r\n"
        "ACK sip:01004@domain.com;sipx-userforward=false;sipx-noroute=Voicemail SIP/2.0\r\n"
        "Route: <sip:192.33.44.55:5060;lr>\r\n"
        "Contact: <sip:6239255975@10.111.0.93:5060;maddr=10.111.0.93;transport=udp;x-sipX-nonat>\r\n"
        "From: <sip:6239255975@10.111.0.93;isup-oli=0;pstn-params=808481808882>;tag=5d05700a-13c4-4dc99946-d2e27c4d-2457a7da\r\n"
        "To: <sip:31002@domain.com?X-sipX-Authidentity=%3Csip:~~id~media%40domain.com%3Bsignature%3D4DC99945::c181beb2880b79b71df6abdf4ec68741>;tag=9efd44cb\r\n"
        "Call-Id: CXC-18-5df916f0-5d05700a-13c4-4dc99946-d2e27c4b-728c2fc@10.111.0.93\r\n"
        "Cseq: 1 ACK\r\n"
        "Max-Forwards: 20\r\n"
        "Via: SIP/2.0/TCP 192.33.44.55;branch=z9hG4bK-sipXecs-d357c1eac55c03e73442d84a33313d4418d8~75565b8987882fcc1706babdcc4d1494\r\n"
        "Content-Length: 0\r\n\r\n";

    std::stringstream compoundBuffer;
    compoundBuffer << segment;

    SIPMessage m1;
    std::size_t bytesRead;
    ASSERT_FALSE(m1.read(compoundBuffer, bytesRead));
    ASSERT_FALSE(bytesRead);
}

TEST(ParserTest, test_buffered_read_message_construction)
{
  std::ostringstream buffer1;
  buffer1 << "INVITE sip:9001@192.168.0.152 SIP/2.0" << CRLF;
  buffer1 << "To: <sip:9001@192.168.0.152>" << CRLF;
  buffer1 << "From: 9011<sip:9011@192.168.0.103>;tag=6657e067" << CRLF;
  buffer1 << "Via: SIP/2.0/UDP 192.168.0.152:9644;branch=z9hG4bK-d87543-419889160-1--d87543-;rport, "
      << "SIP/2.0/UDP 192.168.0.150:9644;branch=z9hG4bK-d87543-419889160-2--d87543-;rport" << CRLF;

  std::ostringstream buffer2;
  buffer2 << "Call-ID: 885e5e180c04c509" << CRLF;
  buffer2 << "CSeq: 1 INVITE" << CRLF;
  buffer2 << "Contact: <sip:9011@192.168.0.152:9644>" << CRLF;
  buffer2 << "Max-Forwards: 70" << CRLF;
  buffer2 << "Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, NOTIFY, MESSAGE, SUBSCRIBE, INFO" << CRLF;
  buffer2 << "Content-Type: application/sdp" << CRLF;

  std::ostringstream buffer3;
  buffer3 << "Record-Route: <sip:10.0.0.1;lr>" << CRLF;
  buffer3 << "Record-Route: <sip:10.0.0.2;lr>" << CRLF;
  buffer3 << "Record-Route: <sip:10.0.0.3;lr>" << CRLF;
  buffer3 << "Content-Length: 178" << CRLF;
  buffer3 << CRLF; /// End of headers
  buffer3 << "v=0" << CRLF;
  buffer3 << "o=- 10818229 10818359 IN IP4 192.168.0.152" << CRLF;

  std::ostringstream buffer4;
  buffer4 << "s=-" << CRLF;
  buffer4 << "c=IN IP4 192.168.0.152" << CRLF;
  buffer4 << "t=0 0" << CRLF;
  buffer4 << "m=audio 35000 RTP/AVP 0 8 101" << CRLF;
  buffer4 << "a=fmtp:101 0-15" << CRLF;
  buffer4 << "a=rtpmap:101 telephone-event/8000" << CRLF;
  buffer4 << "a=sendrecv";
  buffer4 << "this should not be part of the body";

  std::string b1 = buffer1.str();
  std::string b2 = buffer2.str();
  std::string b3 = buffer3.str();
  std::string b4 = buffer4.str();

  SIPMessage message;
  boost::tuple<boost::tribool, const char*> result;
  result = message.consume(b1.c_str(), b1.c_str() + b1.length());
  ASSERT_TRUE(boost::indeterminate(result.get<0>()) );
  result = message.consume(b2.c_str(), b2.c_str() + b2.length());
  ASSERT_TRUE(boost::indeterminate(result.get<0>()));
  result = message.consume(b3.c_str(), b3.c_str() + b3.length());
  ASSERT_TRUE(boost::indeterminate(result.get<0>()));
  result = message.consume(b4.c_str(), b4.c_str() + b4.length());
  ASSERT_TRUE(result.get<0>() == true);
  ASSERT_STREQ(result.get<1>(), "this should not be part of the body");

  boost::tribool isrequest = message.isRequest();
  if(!isrequest)
    ASSERT_TRUE(false);

  std::string tid;
  ASSERT_TRUE(message.getTransactionId(tid));
  ASSERT_TRUE(tid == "invite1z9hG4bK-d87543-419889160-1--d87543-");

  ASSERT_TRUE(message.hdrPresent(OSS::SIP::HDR_FROM) == 1);
  ASSERT_TRUE(message.hdrPresent(OSS::SIP::HDR_TO) == 1);
  ASSERT_TRUE(message.hdrPresent(OSS::SIP::HDR_VIA) == 1);
  ASSERT_TRUE(message.hdrPresent(OSS::SIP::HDR_CONTACT) == 1);
  ASSERT_TRUE(message.hdrPresent("CSEQ") == 1);
  ASSERT_TRUE(message.hdrPresent("CoNtenT-LeNgTh") == 1);
  ASSERT_TRUE(message.hdrPresent(OSS::SIP::HDR_P_ASSERTED_IDENTITY) == 0);

  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_FROM) == "9011<sip:9011@192.168.0.103>;tag=6657e067");
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_TO) != "9011<sip:9011@192.168.0.103>;tag=6657e067");
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_P_ASSERTED_IDENTITY) == "");
  message.hdrSet(OSS::SIP::HDR_P_ASSERTED_IDENTITY, "<sip:9011@192.168.0.103>");
  message.hdrSet(OSS::SIP::HDR_SERVER, "SIP Parser Test");
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_P_ASSERTED_IDENTITY) == "<sip:9011@192.168.0.103>");

  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_RECORD_ROUTE, 0) == "<sip:10.0.0.1;lr>");
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_RECORD_ROUTE, 1) == "<sip:10.0.0.2;lr>");
  ASSERT_TRUE(message.hdrGet(OSS::SIP::HDR_RECORD_ROUTE, 2) == "<sip:10.0.0.3;lr>");
}


TEST(ParserTest, test_message_response_construction)
{
  std::ostringstream msg;
  msg << "INVITE sip:9001@192.168.0.152 SIP/2.0" << CRLF;
  msg << "To: <sip:9001@192.168.0.152>" << CRLF;
  msg << "From: 9011<sip:9011@192.168.0.103>;tag=6657e067" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.152:9644;branch=z9hG4bK-d87543-419889160-1--d87543-;rport, "
      << "SIP/2.0/UDP 192.168.0.150:9644;branch=z9hG4bK-d87543-419889160-2--d87543-;rport" << CRLF;
  msg << "Call-ID: 885e5e180c04c509" << CRLF;
  msg << "CSeq: 1 INVITE" << CRLF;
  msg << "Contact: <sip:9011@192.168.0.152:9644>" << CRLF;
  msg << "Max-Forwards: 70" << CRLF;
  msg << "Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, NOTIFY, MESSAGE, SUBSCRIBE, INFO" << CRLF;
  msg << "Content-Type: application/sdp" << CRLF;
  msg << "Record-Route: <sip:10.0.0.1;lr>" << CRLF;
  msg << "Record-Route: <sip:10.0.0.2;lr>" << CRLF;
  msg << "Record-Route: <sip:10.0.0.3;lr>" << CRLF;
  msg << "Content-Length: 178" << CRLF;
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

  SIPMessage invite;
  invite.consume(msg.str().c_str(), msg.str().c_str() + msg.str().length());

  SIPMessage::Ptr response;
  response = invite.createResponse(200, "", "12345", "sip:reponse_test@myhost");
  //std::cout << response->data() << std::endl;

  std::ostringstream rMsg;
  rMsg << "SIP/2.0 400 Bad Request" << CRLF;
  rMsg << "To: <sip:9001@192.168.0.152>;tag=9999" << CRLF;
  rMsg << "From: 9011<sip:9011@192.168.0.103>;tag=8888" << CRLF;
  rMsg << "Via: SIP/2.0/UDP 192.168.0.152:9655;branch=z9hG4bK-d87543-419889160-1--d87543-;rport, "
      << "SIP/2.0/UDP 192.168.0.150:9666;branch=z9hG4bK-d87543-419889160-2--d87543-;rport" << CRLF;
  rMsg << "Call-ID: 885e5e180c04c509" << CRLF;
  rMsg << "CSeq: 1 INVITE" << CRLF;
  rMsg << "Contact: <sip:9011@192.168.0.152:9644>" << CRLF;
  rMsg << "Record-Route: <sip:10.0.0.1;lr>" << CRLF;
  rMsg << "Record-Route: <sip:10.0.0.2;lr>" << CRLF;
  rMsg << "Record-Route: <sip:10.0.0.3;lr>" << CRLF;
  rMsg << "Content-Length: 0" << CRLF;

  SIPMessage::Ptr r2(new SIPMessage(rMsg.str()));
  SIPMessage::Ptr r3 = invite.reformatResponse(r2);
  ASSERT_TRUE(r3);
  //std::cout << r3->data() << std::endl;
}

TEST(ParserTest, test_abnf_parser)
{
  {
    //
    // Test iterator finder
    //
    const char* pointer = "I think, therefore I am!";
    std::string resultIter = findNextIterFromString(",", pointer);
    ASSERT_STREQ(resultIter.c_str(), " therefore I am!");
    resultIter = findLastIterFromString(",", pointer);
    ASSERT_STREQ(resultIter.c_str(), " therefore I am!");

    resultIter = findNextIterFromString("therefore", pointer);
    ASSERT_STREQ(resultIter.c_str(), " I am!");
    resultIter = findLastIterFromString("therefore", pointer);
    ASSERT_STREQ(resultIter.c_str(), " I am!");

    resultIter = findNextIterFromString("I", pointer);
    ASSERT_STREQ(resultIter.c_str(), " think, therefore I am!");
    resultIter = findLastIterFromString("I", pointer);
    ASSERT_STREQ(resultIter.c_str(), " am!");
    

  }
  {
    /// Test string literal comparison
    const char* a = "INVITE";
    const char* b = "inViTe";
    const char* c = "INVITEe";

    ABNFEvaluate<ABNFStrictStringLiteral<'I', 'N', 'V', 'I', 'T', 'E'> >t0;
    ASSERT_TRUE(t0(a));
    ASSERT_TRUE(!t0(b));
    ASSERT_TRUE(!t0(c));

    ABNFEvaluate<ABNFStringLiteral<'I', 'N', 'V', 'I', 'T', 'E'> >t1;
    ASSERT_TRUE(t1(a));
    ASSERT_TRUE(t1(b));
    ASSERT_TRUE(!t1(c));

    ABNFEvaluate<ABNFStrictStringLiteral<':', ':'> > t2;
    ASSERT_TRUE(!t2("::FFFF"));
  }

  {
    /// Test basic char comparison with LR Sequence
    const char * a = "aaa";
    const char * b = "bbb";
    ABNFEvaluate<ABNFCharComparison<'a'> > t0;
    ASSERT_TRUE(!t0(a));
    ASSERT_TRUE(!t0(b));
    ABNFEvaluate<ABNFLRSequence3<ABNFCharComparison<'a'>,ABNFCharComparison<'a'>,ABNFCharComparison<'a'> > >t1;
    ASSERT_TRUE(t1(a));
    ASSERT_TRUE(!t1(b));
  }

  {
    /// Test AnyOf operator with char sequence
    ABNFEvaluate<ABNFAnyOfChars<'A', 'B', 'C'> > t0;
    ASSERT_TRUE(t0("A"));
    ASSERT_TRUE(t0("B"));
    ASSERT_TRUE(t0("C"));
    ASSERT_TRUE(!t0("D"));
    ASSERT_TRUE(!t0("c"));
    ASSERT_TRUE(!t0("E"));

    /// Test nested AnyOf rules
    ABNFEvaluate<ABNFAnyOfMultiple3<
      ABNFAnyOfChars<'A', 'B', 'C'>,
      ABNFAnyOfChars<'a', 'b', 'c'>,
      ABNFAnyOfChars<'D', 'E', 'F'>
    > > t1;
    ASSERT_TRUE(t1("A"));
    ASSERT_TRUE(t1("B"));
    ASSERT_TRUE(t1("C"));
    ASSERT_TRUE(t1("a"));
    ASSERT_TRUE(t1("b"));
    ASSERT_TRUE(t1("c"));
    ASSERT_TRUE(!t1("f"));
    ASSERT_TRUE(t1("F"));
    ASSERT_TRUE(!t1("e"));
    ASSERT_TRUE(!t1("g"));
  }

  {
    // Test rule iteration
    typedef ABNFAnyOfMultiple4<
      ABNFAnyOfChars<'A', 'B', 'C'>,
      ABNFAnyOfChars<'a', 'b', 'c'>,
      ABNFAnyOfChars<'D', 'E', 'F'>,
      ABNFAnyOfChars<'1', '2', '3'>
    > validChars;

    ABNFEvaluate<ABNFLoopUntil<validChars, ABNFLoopExitIfNul, 1, 1024> > t0;
    ASSERT_TRUE(t0("ABC"));
    ASSERT_TRUE(t0("abc"));
    ASSERT_TRUE(t0("DEF"));
    ASSERT_TRUE(t0("123"));
    ASSERT_TRUE(t0("AaD1"));
    ASSERT_TRUE(t0("BbE2"));
    ASSERT_TRUE(t0("CcF3"));
    ASSERT_TRUE(t0("ABCabcDEF123"));
    ASSERT_TRUE(!t0("zBbE2"));
    ASSERT_TRUE(!t0("ABCabcDEF123567"));

    {
      /// lets do a strict test this time
      ABNFLoopUntil<validChars, ABNFLoopExitIfNul, 1, 1024> strictTest;
      const char* offSet = "ABCabcDEF123567";
      char* newOffSet = strictTest.parse(offSet);
      std::string token(offSet, (const char*)newOffSet);
      ASSERT_TRUE(token != "ABCabcDEF123567");
      ASSERT_TRUE(token == "ABCabcDEF123"); /// trailing "567" did not meet criteria so the loop exits after 3
    }

    {
      /// do a strict test using custom exit rule
      typedef ABNFLoopExitChars<'F'> exitIfF;
      ABNFLoopUntil<validChars, exitIfF, 1, 1024> strictTest;
      const char* offSet = "ABCabcDEF123567";
      char* newOffSet = strictTest.parse(offSet);
      std::string token(offSet, (const char*)newOffSet);
      ASSERT_TRUE(token == "ABCabcDE");
    }
  }

  {
    /// test find match
    ABNFFindMatch<'l', 'a', 'z', 'y'> findMatch;
    const char* offSet = "The Quick Brown Fox Jumps Over The Lazy Dog";
    char* newOffSet = findMatch.parse( offSet );
    std::string token(offSet, (const char*)newOffSet);
    ASSERT_TRUE(token == "The Quick Brown Fox Jumps Over The Lazy");
  }

  //
  //  Start Of SIP ABNF Test
  //

  {
    /// RFC 3261 ABNF Rule for
    /// userinfo         =  ( user / telephone-subscriber ) [ ":" password ] "@"
    ABNFEvaluate<ABNFSIPUserInfo> parser;
    ASSERT_TRUE(parser("a@"));
    ASSERT_TRUE(parser("alice:password@"));
    ASSERT_TRUE(!parser("alice:pass:word@"));  // ":" is reserved and cant be used inside password!
    ASSERT_TRUE(parser("alice:pass&word@"));  // password allows ampersand
    ASSERT_TRUE(parser("12127773456@"));
    ASSERT_TRUE(!parser("12127773456@aol.com"));
    ASSERT_TRUE(!parser(":12127773456@aol.com"));
    ASSERT_TRUE(!parser("@host.com"));

    ABNFSIPUserInfo tokenizer;
    std::vector<std::string> tokens;
    tokenizer.parseTokens("alice:password@", tokens);
    ASSERT_TRUE(tokens[0] == "alice");
    ASSERT_TRUE(tokens[1] == ":password");
    ASSERT_TRUE(tokens[2] == "@");
    tokens.clear();

    tokenizer.parseTokens("alice@", tokens);
    ASSERT_TRUE(tokens[0] == "alice");
    ASSERT_TRUE(tokens[1].empty());
    ASSERT_TRUE(tokens[2] == "@");

  }

  {
    /// RFC 3261 ABNF Rule for
    /// hostname = *( domainlabel "." ) toplabel [ "." ]
    ABNFEvaluate<ABNFSIPHostName> parser;
    ASSERT_TRUE(parser("localhost"));
    ASSERT_TRUE(parser("opensipstack.org"));
    ASSERT_TRUE(parser("p1.opensipstack.org"));
    ASSERT_TRUE(!parser("12345"));
    ASSERT_TRUE(!parser("alice@atlanta.com"));
    ASSERT_TRUE(!parser("10.0.0.1")); /// IP addresses have have separate ABNF rules

    ABNFSIPHostName hostToken;
    const char * host1 = "p1.opensipstack.org";
    char * offSet = hostToken.parse(host1);
    std::string token = std::string( host1, (const char*)offSet );
    ASSERT_TRUE(token == host1);

    const char * host2 = "localhost";
    offSet = hostToken.parse(host2);
    token = std::string( host2, (const char*)offSet );
    ASSERT_TRUE(token == host2);

    const char * host3 = "192.168.0.1";
    offSet = hostToken.parse(host3);
    token = std::string( host3, (const char*)offSet );
    ASSERT_TRUE(token.empty());
  }

  {
    /// RFC 3261 ABNF Rule for
    /// IPv4address    =  1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT
    ABNFEvaluate<ABNFSIPIPV4Address> parser;
    ASSERT_TRUE(parser("127.0.0.1"));
    ASSERT_TRUE(parser("10.0.0.10"));
    ASSERT_TRUE(parser("255.255.255.255"));
    ASSERT_TRUE(!parser("localhost"));
    ASSERT_TRUE(!parser("..."));
    ASSERT_TRUE(!parser(". . . "));
    ASSERT_TRUE(!parser("1..1."));
    ASSERT_TRUE(!parser("1.1"));
    ASSERT_TRUE(!parser("1234.1234.1234.1234"));

    ABNFSIPIPV4Address ipv4Token;
    const char * host1 = "192.168.0.1";
    char * offSet = ipv4Token.parse(host1);
    std::string token = std::string( host1, (const char*)offSet );
    ASSERT_TRUE(token == host1);
  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    /// IPv6address  =  hexpart [ ":" IPv4address ]
    ABNFEvaluate<ABNFSIPIPV6Address> parser;
    ASSERT_TRUE(parser("2001:0db8:0000:0000:0000:0000:1428:57ab"));
    ASSERT_TRUE(parser("2001:0db8:0000:0000:0000::1428:57ab"));
    ASSERT_TRUE(parser("2001:0db8:0:0:0:0:1428:57ab"));
    ASSERT_TRUE(parser("2001:0db8:0:0::1428:57ab"));
    ASSERT_TRUE(parser("2001:0db8::1428:57ab"));
    ASSERT_TRUE(parser("2001:db8::1428:57ab"));
    ASSERT_TRUE(parser("::ffff:12.34.56.78"));
    ASSERT_TRUE(parser("::ffff:0c22:384e"));


    ABNFSIPIPV6Address ipv6Token;
    const char * host1 = "2001:0db8:0000:0000:0000:0000:1428:57ab";
    char * offSet = ipv6Token.parse(host1);
    std::string token = std::string( host1, (const char*)offSet );
    ASSERT_TRUE(token == host1);
  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    /// host  =  hostname / IPv4address / IPv6reference
    ABNFEvaluate<ABNFSIPHost> parser;
    ASSERT_TRUE(parser("localhost"));
    ASSERT_TRUE(parser("p1.atlanta.com"));
    ASSERT_TRUE(parser("127.0.0.1"));
    ASSERT_TRUE(parser("192.168.0.10"));
    ASSERT_TRUE(parser("[::ffff:0c22:384e]"));
    ASSERT_TRUE(!parser("::ffff:0c22:384e"));  // no brackets
    ASSERT_TRUE(!parser("..."));
    ASSERT_TRUE(!parser(". . . "));
    ASSERT_TRUE(!parser("1..1."));
    ASSERT_TRUE(!parser("1.1"));
    ASSERT_TRUE(!parser("1234.1234.1234.1234"));
  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    /// hostport =  host [ ":" port ]
    ABNFEvaluate<ABNFSIPHostPort> parser;
    ASSERT_TRUE(parser("localhost"));
    ASSERT_TRUE(parser("p1.atlanta.com"));
    ASSERT_TRUE(parser("127.0.0.1"));
    ASSERT_TRUE(parser("192.168.0.10"));
    ASSERT_TRUE(parser("[::ffff:0c22:384e]"));
    ASSERT_TRUE(!parser("::ffff:0c22:384e"));  // no brackets

    ASSERT_TRUE(parser("localhost:5060"));
    ASSERT_TRUE(parser("p1.atlanta.com:5060"));
    ASSERT_TRUE(parser("127.0.0.1:5060"));
    ASSERT_TRUE(parser("192.168.0.10:5060"));
    ASSERT_TRUE(parser("[::ffff:0c22:384e]:5060"));
    ASSERT_TRUE(!parser("::ffff:0c22:384e:5060"));  // no brackets


    ABNFSIPHostPort tokenizer;
    std::vector<std::string> tokens;

    tokenizer.parseTokens("localhost:5060", tokens);
    ASSERT_TRUE(tokens[0] == "localhost");
    ASSERT_TRUE(tokens[1] == ":5060");
    tokens.clear();

    tokenizer.parseTokens("192.168.0.1", tokens);
    ASSERT_TRUE(tokens[0] == "192.168.0.1");
    ASSERT_TRUE(tokens[1].empty());
    tokens.clear();

    tokenizer.parseTokens("192.168.0.1:5060", tokens);
    ASSERT_TRUE(tokens[0] == "192.168.0.1");
    ASSERT_TRUE(tokens[1] == ":5060");
    tokens.clear();

    tokenizer.parseTokens("[::ffff:0c22:384e]:5060", tokens);
    ASSERT_TRUE(tokens[0] == "[::ffff:0c22:384e]");
    ASSERT_TRUE(tokens[1] == ":5060");
    tokens.clear();

    tokenizer.parseTokens("[::ffff:0c22:384e]", tokens);
    ASSERT_TRUE(tokens[0] == "[::ffff:0c22:384e]");
    ASSERT_TRUE(tokens[1].empty());
    tokens.clear();

  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    /// uri-parameters    =  *( ";" uri-parameter)
    ABNFEvaluate<ABNFSIPURIParameters> parser;
    ASSERT_TRUE(parser(";user=phone;lr"));
    ASSERT_TRUE(!parser("lr;user=ip;maddr=0.0.0.0")); /// no leading semi-colon
    ASSERT_TRUE(parser(";lr"));
    ASSERT_TRUE(parser(";lr;user=ip;maddr=0.0.0.0"));
    ASSERT_TRUE(!parser(";;;"));
    ASSERT_TRUE(!parser("=;=;="));

    ABNFSIPURIParameters tokenizer;
    std::vector<std::string> tokens;
    tokenizer.parseTokens( ";user=phone;lr", tokens );
    ASSERT_TRUE(tokens[0] == ";user=phone");
    ASSERT_TRUE(tokens[1] == ";lr");
    tokens.clear();

    tokenizer.parseTokens(";lr;user=ip;maddr=0.0.0.0", tokens);
    ASSERT_TRUE(tokens[0] == ";lr");
    ASSERT_TRUE(tokens[1] == ";user=ip");
    ASSERT_TRUE(tokens[2] == ";maddr=0.0.0.0");

  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    /// headers  =  "?" header *( "&" header )
    ABNFEvaluate<ABNFSIPURIHeaders> parser;
    ASSERT_TRUE(parser("?header1=value1&header2=value2"));
    ASSERT_TRUE(!parser("? header1=value1&header2=value2")); /// space after ? is illegal
    ASSERT_TRUE(!parser("?header1=value1& header2=value2")); /// space after & is illegal but would return true because the first header is satisfied
    ASSERT_TRUE(!parser("?lr&header1=value1"));
    ASSERT_TRUE(!parser("?=&=&="));

    ABNFSIPURIHeaders tokenizer;
    std::vector<std::string> tokens;
    tokenizer.parseTokens("?header1=value1&header2=value2&header3=value3", tokens);
    ASSERT_TRUE(tokens[0] == "?");
    ASSERT_TRUE(tokens[1] == "header1=value1");
    ASSERT_TRUE(tokens[2] == "&header2=value2&header3=value3");  /// this needs to be further tokenized
  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    /// SIP-URI  =  "sip:" [ userinfo ] hostport  uri-parameters [ headers ]
    ABNFEvaluate<ABNFSIPURI> parser;
    ASSERT_TRUE(parser("sip:atlanta.com"));
    ASSERT_TRUE(parser("sip:p1.atlanta.com"));
    ASSERT_TRUE(parser("sip:alice@atlanta.com"));
    ASSERT_TRUE(parser("sip:alice:password@atlanta.com"));
    ASSERT_TRUE(parser("sip:alice:password@atlanta.com;user=phone"));
    ASSERT_TRUE(parser("sip:alice:password@atlanta.com;lr;user=phone"));
    ASSERT_TRUE(parser("sip:alice:password@atlanta.com;lr;user=phone?h1=v1"));
    ASSERT_TRUE(parser("sip:alice:password@atlanta.com;lr;user=phone?h1=v1&h2=v2"));
    ASSERT_TRUE(parser("sip:alice:password@[2001:0db8:0:0:0:0:1428:57ab]"));

    ABNFSIPURI tokenizer;
    std::vector<std::string> tokens;

    tokenizer.parseTokens("sip:atlanta.com", tokens);
    ASSERT_TRUE(tokens[0] == "sip:");
    ASSERT_TRUE(tokens[1].empty());
    ASSERT_TRUE(tokens[2] == "atlanta.com");
    tokens.clear();

    tokenizer.parseTokens("sips:alice@atlanta.com", tokens);
    ASSERT_TRUE(tokens[0] == "sips:");
    ASSERT_TRUE(tokens[1] == "alice@");
    ASSERT_TRUE(tokens[2] == "atlanta.com");
    tokens.clear();

    tokenizer.parseTokens("mailto:alice@atlanta.com", tokens);
    ASSERT_TRUE(tokens[0] == "mailto:");
    ASSERT_TRUE(tokens[1] == "alice@");
    ASSERT_TRUE(tokens[2] == "atlanta.com");
    tokens.clear();

    tokenizer.parseTokens("sips:alice:password@atlanta.com", tokens);
    ASSERT_TRUE(tokens[0] == "sips:");
    ASSERT_TRUE(tokens[1] == "alice:password@");
    ASSERT_TRUE(tokens[2] == "atlanta.com");
    tokens.clear();

    tokenizer.parseTokens("sip:alice:password@p1.atlanta.com:5060", tokens);
    ASSERT_TRUE(tokens[0] == "sip:");
    ASSERT_TRUE(tokens[1] == "alice:password@");
    ASSERT_TRUE(tokens[2] == "p1.atlanta.com:5060");
    tokens.clear();

    tokenizer.parseTokens("sip:alice:password@p1.atlanta.com:5060;user=ip", tokens);
    ASSERT_TRUE(tokens[0] == "sip:");
    ASSERT_TRUE(tokens[1] == "alice:password@");
    ASSERT_TRUE(tokens[2] == "p1.atlanta.com:5060");
    ASSERT_TRUE(tokens[3] == ";user=ip");
    tokens.clear();

    tokenizer.parseTokens("sip:alice:password@p1.atlanta.com:5060;user=ip;lr", tokens);
    ASSERT_TRUE(tokens[0] == "sip:");
    ASSERT_TRUE(tokens[1] == "alice:password@");
    ASSERT_TRUE(tokens[2] == "p1.atlanta.com:5060");
    ASSERT_TRUE(tokens[3] == ";user=ip;lr");
    tokens.clear();

    tokenizer.parseTokens("sip:alice:password@p1.atlanta.com:5060;user=ip;lr?method=INVITE", tokens);
    ASSERT_TRUE(tokens[0] == "sip:");
    ASSERT_TRUE(tokens[1] == "alice:password@");
    ASSERT_TRUE(tokens[2] == "p1.atlanta.com:5060");
    ASSERT_TRUE(tokens[3] == ";user=ip;lr");
    ASSERT_TRUE(tokens[4] == "?method=INVITE");
    tokens.clear();
  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    /// absoluteURI    =  scheme ":" ( hier-part / opaque-part )
    ABNFSIPAbsoluteURI tokenizer;
    std::vector<std::string> tokens;
    tokenizer.parseTokens( "http://www.opensipstack.org", tokens );
    ASSERT_TRUE(tokens[0] == "http:");
    ASSERT_TRUE(tokens[1] == "//www.opensipstack.org" );
  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    /// Request-URI  =  SIP-URI / SIPS-URI / absoluteURI
    ABNFSIPRequestURI tokenizer;
    const char * uri1 = "sip:alice@atlanta.com";
    const char * uri2 = "http://www.opensipstack.org";
    const char * uri3 = "http://localhost";
    const char * uri4 = "sip:localhost";

    std::string tkn1 = std::string(uri1, (const char *)tokenizer.parse(uri1));
    ASSERT_TRUE(tkn1 == uri1);

    std::string tkn2 = std::string(uri2, (const char *)tokenizer.parse(uri2));
    ASSERT_TRUE(tkn2 == uri2);

    std::string tkn3 = std::string(uri3, (const char *)tokenizer.parse(uri3));
    ASSERT_TRUE(tkn3 == uri3);

    std::string tkn4 = std::string(uri4, (const char *)tokenizer.parse(uri4));
    ASSERT_TRUE(tkn4 == uri4);
  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    /// SIP-Version    =  "SIP" "/" 1*DIGIT "." 1*DIGIT
    ABNFSIPVersion tokenizer;
    const char * v1 = "SIP/2.0";
    std::string tkn1 = std::string(v1, (const char *)tokenizer.parse(v1));
    ASSERT_TRUE(tkn1 == v1);
  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    /// Request-Line   =  Method SP Request-URI SP SIP-Version CRLF
    ABNFEvaluate<ABNFSIPRequestLine> parser;
    ASSERT_TRUE(parser("INVITE sip:localhost SIP/2.0\r\n"));
    ASSERT_TRUE(parser("INVITE sip:localhost SIP/2.0")); /// test no CRLF
    ASSERT_TRUE(!parser("INVITE   sip:localhost SIP/2.0"));/// multiple spaces
    ASSERT_TRUE(parser("INVITE http://www.opensipstack.org SIP/2.0"));/// believe it or not, this is allowed by the BNF

    ABNFSIPRequestLine tokenizer;
    std::vector<std::string> tokens;
    const char * rline = "INVITE sip:alice@atlanta.com;transport=tcp SIP/2.0\r\n";
    tokenizer.parseTokens(rline, tokens);
    ASSERT_TRUE(tokens[0] == "INVITE");
    ASSERT_TRUE(tokens[1] == " ");
    ASSERT_TRUE(tokens[2] == "sip:alice@atlanta.com;transport=tcp");
    ASSERT_TRUE(tokens[3] == " ");
    ASSERT_TRUE(tokens[4] == "SIP/2.0");
    ASSERT_TRUE(tokens[5] == "\r\n");
  }

  {
    /// Satisfies RFC 3261 ABNF Rule for
    ///Status-Line     =  SIP-Version SP Status-Code SP Reason-Phrase CRLF
    ABNFEvaluate<ABNFSIPStatusLine> parser;
    ASSERT_TRUE(parser("SIP/2.0 100 Trying\r\n"));
    ASSERT_TRUE(parser("SIP/2.0 100 Trying"));/// No CRLF
    ASSERT_TRUE(!parser("SIP/2.0    100    Trying"));/// extra spaces
    ASSERT_TRUE(!parser("SIP/2.0 1000 Trying"));/// 4 digit status code
    ASSERT_TRUE(!parser("NOT-SIP/2.0 100 Trying"));/// wrong version
    ASSERT_TRUE(parser("SIP/2.0 200 Ok\r\n"));
    ASSERT_TRUE(parser("SIP/2.0 180 The Phone Is Ringing???\r\n"));

    ABNFSIPStatusLine tokenizer;
    std::vector<std::string> tokens;
    const char * sline = "SIP/2.0 404 Not Found\r\n";
    tokenizer.parseTokens(sline, tokens);
    ASSERT_TRUE(tokens[0] == "SIP/2.0");
    ASSERT_TRUE(tokens[1] == " ");
    ASSERT_TRUE(tokens[2] == "404");
    ASSERT_TRUE(tokens[3] == " ");
    ASSERT_TRUE(tokens[4] == "Not Found");
    ASSERT_TRUE(tokens[5] == "\r\n");
  }

  {
    typedef ABNFFindMatch<'b', 'r', 'a', 'n', 'c', 'h', '='> _pvar1;
    ABNFLRSequence2<_pvar1, ABNF_SIP_pvalue> parser;
    std::string via = "Via: SIP/2.0/UDP pc33.atlanta.com;branch=z9hG4bK776asdhds;rport";
    ABNFTokens tokens;
    parser.parseTokens(via.c_str(), tokens);
    ASSERT_TRUE(tokens.size() == 2);
    ASSERT_TRUE(tokens[1] == "z9hG4bK776asdhds");
  }

  {
    // same test as above but using findNextIterFromString
    const char* via = "Via: SIP/2.0/UDP pc33.atlanta.com;branch=z9hG4bK776asdhds;rport";
    char* startIter = findNextIterFromString("branch=", via);
    ASSERT_TRUE(startIter != via);
    ABNF_SIP_pvalue parser;
    char* newIter = parser.parse(startIter);
    std::string token = std::string(startIter, newIter);
    ASSERT_TRUE(token == "z9hG4bK776asdhds");
  }

  {
    // Test findNextIterFromRule function template
    const char* cseq = "INVITE           9999";
    char* startIter = findNextIterFromRule<ABNF_SIP_LWS>(cseq);
    ASSERT_TRUE(startIter != cseq);
    std::string token(startIter);
    ASSERT_TRUE(token == "9999");
  }
}