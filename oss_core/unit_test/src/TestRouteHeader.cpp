#include "gtest/gtest.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/SIP/SIPURI.h"
#include <iostream>
#include <sstream>

using namespace OSS;
using OSS::SIP::CRLF;


TEST(ParserTest, test_route_header_parser)
{
  std::ostringstream msg;
  msg << "INVITE sip:9001@192.168.0.152 SIP/2.0" << CRLF;
  msg << "To: <sip:9001@192.168.0.152>" << CRLF;
  msg << "From: 9011<sip:9011@192.168.0.103>;tag=6657e067" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.1;branch=001;rport, SIP/2.0/UDP 192.168.0.2;branch=002, SIP/2.0/UDP 192.168.0.3;branch=003" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.4;branch=004" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.5;branch=005" << CRLF;
  msg << "Call-ID: 885e5e180c04c509" << CRLF;
  msg << "CSeq: 1 INVITE" << CRLF;
  msg << "Contact: <sip:9011@192.168.0.152:9644>" << CRLF;
  msg << "Max-Forwards: 70" << CRLF;
  msg << "Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, NOTIFY, MESSAGE, SUBSCRIBE, INFO" << CRLF;
  msg << "Content-Type: application/sdp" << CRLF;
  msg << "Route: <sip:10.0.0.1;lr>" << CRLF;
  msg << "Route: <sip:10.0.0.2;lr>, <sip:10.0.0.3;lr>" << CRLF;
  msg << "Route: <sip:10.0.0.4;lr>" << CRLF;
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


  {
    std::string route = "<sip:10.0.0.1;lr>, <sip:10.0.0.2;lr>, <sip:10.0.0.3;lr>";
    SIP::RouteURI topURI;
    SIP::SIPRoute::popTopURI(route, topURI);
    ASSERT_TRUE(topURI.data() == "<sip:10.0.0.1;lr>");
    ASSERT_TRUE(route == "<sip:10.0.0.2;lr>, <sip:10.0.0.3;lr>");
    SIP::SIPRoute::popTopURI(route, topURI);
    ASSERT_TRUE(topURI.data() == "<sip:10.0.0.2;lr>");
    ASSERT_TRUE(route == "<sip:10.0.0.3;lr>");
    SIP::SIPRoute::popTopURI(route, topURI);
    ASSERT_TRUE(topURI.data() == "<sip:10.0.0.3;lr>");
    ASSERT_TRUE(route.empty());
  }

  {
  std::list<std::string> routes;
  SIP::SIPRoute::msgGetRoutes(&message, routes);
  std::list<std::string>::iterator routeIter = routes.begin();
  ASSERT_STREQ(routeIter->c_str(), "<sip:10.0.0.1;lr>");
  routeIter++;
  ASSERT_STREQ(routeIter->c_str(), "<sip:10.0.0.2;lr>");
  routeIter++;
  ASSERT_STREQ(routeIter->c_str(), "<sip:10.0.0.3;lr>");
  routeIter++;
  ASSERT_STREQ(routeIter->c_str(), "<sip:10.0.0.4;lr>");

  SIP::RouteURI routeURI(*routeIter);
  SIP::SIPURI uri(routeURI.getURI());
  ASSERT_TRUE(uri.verify());
  ASSERT_TRUE(uri.hasParam("lr"));
  }

  {
  std::string topRoute;
  ASSERT_TRUE(SIP::SIPRoute::msgGetTopRoute(&message, topRoute));
  ASSERT_STREQ(topRoute.c_str(), "<sip:10.0.0.1;lr>");
  ASSERT_TRUE(SIP::SIPRoute::msgPopTopRoute(&message, topRoute));
  ASSERT_STREQ(topRoute.c_str(), "<sip:10.0.0.1;lr>");
 
  std::list<std::string> routes;
  ASSERT_TRUE(SIP::SIPRoute::msgGetRoutes(&message, routes));

  message.commitData();
  if (routes.size() != 3)
    std::cerr << message.data();

  ASSERT_EQ(routes.size(), 3);

  ASSERT_TRUE(SIP::SIPRoute::msgGetTopRoute(&message, topRoute));
  ASSERT_TRUE(topRoute == "<sip:10.0.0.2;lr>");

  ASSERT_TRUE(SIP::SIPRoute::msgAddRoute(&message, "<sip:10.0.0.1;lr>"));
  ASSERT_TRUE(SIP::SIPRoute::msgGetRoutes(&message, routes));

  message.commitData();
  if (routes.size() != 4)
    std::cerr << message.data();

  ASSERT_EQ(routes.size(), 4);

  ASSERT_TRUE(SIP::SIPRoute::msgGetTopRoute(&message, topRoute));
  ASSERT_STREQ(topRoute.c_str(), "<sip:10.0.0.1;lr>");
  }
}

