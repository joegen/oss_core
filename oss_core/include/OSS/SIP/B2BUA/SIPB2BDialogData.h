// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "GNU Lesser General Public License (LGPL)".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

/*

 leg-2 :
{
  call-id = "b2d968e6-f9fc70df-21724b98@192.168.1.98";
  from = "\"Joegen Baclor\" <sip:2017@openuc.ezuce.com>;tag=80C5FE17-A5B702D0";
  to = "<sip:32008@openuc.ezuce.com;user=phone>;tag=2t20XFeQ6FUyF";
  remote-contact = "<sip:32008@172.31.1.5:15060;transport=udp>";
  local-cseq = 2;
  local-contact = "2017 <sip:sbc-reg-2017-17717761976213673066@174.142.82.79:5060;transport=udp>";
  local-rr = "<sip:174.142.82.79:5060;transport=udp;sbc-session-id=61623637535573960411566608137;sbc-call-index=2;lr>";
  remote-ip = "107.23.34.40:5060";
  transport-id = "0";
  target-transport = "udp";
  remote-sdp = "v=0\r\no=FreeSWITCH 1345073993 1345073994 IN IP4 172.31.1.5\r\ns=FreeSWITCH\r\nc=IN IP4 172.31.1.5\r\nt=0 0\r\nm=audio 30498 RTP/AVP 9 101\r\nc=IN IP4 107.23.34.40\r\na=rtpmap:9 G722/8000\r\na=rtpmap:101 telephone-event/8000\r\na=fmtp:101 0-16\r\na=silenceSupp:off - - - -\r\na=ptime:20\r\na=x-sipx-ntap:X172.31.1.5-107.23.34.40;82\r\n";
  local-sdp = "v=0\r\no=- 1345000425 1345000425 IN IP4 192.168.1.98\r\ns=Polycom IP Phone\r\nc=IN IP4 174.142.82.79\r\nt=0 0\r\na=sendrecv\r\nm=audio 35014 RTP/AVP 9 0 8 18 101\r\na=rtpmap:9 G722/8000\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:8 PCMA/8000\r\na=rtpmap:18 G729/8000\r\na=fmtp:18 annexb=no\r\na=rtpmap:101 telephone-event/8000\r\na=rtcp:35015\r\n";
  routes = [ "<sip:107.23.34.40:5060;lr;sipXecs-CallDest=AL%2CAL;sipXecs-rs=%2Aauth%7E.%2Afrom%7EODBDNUZFMTctQTVCNzAyRDA%60.900_ntap%2Aid%7EMzQ0Ni0xMjc%60%213a9741786545331be7fc3f0a2a846d54;x-sipX-done>", "<sip:174.142.82.79:5060;transport=udp;sbc-session-id=61623637535573960411566608137;sbc-call-index=2;lr>" ];
};
leg-1 :
{
  call-id = "b2d968e6-f9fc70df-21724b98@192.168.1.98";
  from = "<sip:32008@openuc.ezuce.com;user=phone>;tag=2t20XFeQ6FUyF";
  to = "\"Joegen Baclor\" <sip:2017@openuc.ezuce.com>;tag=80C5FE17-A5B702D0";
  remote-contact = "<sip:2017@192.168.1.98>";
  local-contact = "2017 <sip:2017@174.142.82.79:5060;transport=udp>";
  local-rr = "<sip:174.142.82.79:5060;transport=udp;sbc-session-id=61623637535573960411566608137;sbc-call-index=1;lr>";
  remote-ip = "112.209.124.182:5060";
  transport-id = "0";
  target-transport = "udp";
  local-sdp = "v=0\r\no=KarooBridge 1345073993 1345073994 IN IP4 172.31.1.5\r\ns=KarooBridge\r\nc=IN IP4 174.142.82.79\r\nt=0 0\r\nm=audio 35012 RTP/AVP 9 101\r\nc=IN IP4 174.142.82.79\r\na=rtpmap:9 G722/8000\r\na=rtpmap:101 telephone-event/8000\r\na=fmtp:101 0-16\r\na=silenceSupp:off - - - -\r\na=ptime:20\r\na=x-sipx-ntap:X172.31.1.5-107.23.34.40;82\r\na=rtcp:35013\r\n";
  remote-sdp = "v=0\r\no=- 1345000425 1345000425 IN IP4 192.168.1.98\r\ns=Polycom IP Phone\r\nc=IN IP4 192.168.1.98\r\nt=0 0\r\na=sendrecv\r\nm=audio 2222 RTP/AVP 9 0 8 18 101\r\na=rtpmap:9 G722/8000\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:8 PCMA/8000\r\na=rtpmap:18 G729/8000\r\na=fmtp:18 annexb=no\r\na=rtpmap:101 telephone-event/8000\r\n";
  routes = [ "<sip:174.142.82.79:5060;transport=udp;sbc-session-id=61623637535573960411566608137;sbc-call-index=1;lr>" ];
};


 */

#ifndef SIPB2BDIALOGDATA_H
#define	SIPB2BDIALOGDATA_H


#include <string>
#include <vector>


namespace OSS {
namespace SIP {


struct SIPB2BDialogData
{
public:
  struct LegInfo
  {
    std::string dialogId;
    std::string callId;
    std::string from;
    std::string to;
    std::string remoteContact;
    std::string localContact;
    std::string localRecordRoute;
    std::string remoteIp;
    std::string transportId;
    std::string targetTransport;
    std::string localSdp;
    std::string remoteSdp;
    std::vector<std::string> routeSet;
    std::string encryption;
    bool noRtpProxy;
    unsigned long localCSeq;
  };

  std::string sessionId;
  LegInfo leg1;
  LegInfo leg2;
};

} } // OSS::SIP

#endif	/* SIPB2BDIALOGDATA_H */

