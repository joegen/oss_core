


#include "gtest/gtest.h"
#include "OSS/OSS.h"
#include "OSS/SDP/SDPSession.h"
#include "OSS/Net/IPAddress.h"


using OSS::SDP::SDPHeader;
using OSS::SDP::SDPHeaderList;
using OSS::SDP::SDPMedia;
using OSS::SDP::SDPSession;

TEST(ParserTest, test_sdp_parser)
{
  std::stringstream sdp;

  sdp << "v=0" << "\r\n";
  sdp << "o=jdoe 2890844526 2890842807 IN IP4 10.47.16.5" << "\r\n";
  sdp << "s=SDP Seminar" << "\r\n";
  sdp << "i=A Seminar on the session description protocol" << "\r\n";
  sdp << "u=http://www.example.com/seminars/sdp.pdf" << "\r\n";
  sdp << "e=j.doe@example.com (Jane Doe)" << "\r\n";
  sdp << "c=IN IP4 224.2.17.12/127" << "\r\n";
  sdp << "t=2873397496 2873404696" << "\r\n";
  sdp << "a=recvonly" << "\r\n";
  sdp << "m=audio 49170 RTP/AVP 0" << "\r\n";
  sdp << "m=video 51372 RTP/AVP 99" << "\r\n";
  sdp << "a=rtpmap:99 h263-1998/90000" << "\r\n";

  SDPHeaderList headerList(sdp.str().c_str());
  ASSERT_TRUE(headerList.isValid());

  SDPMedia audioSDP;
  audioSDP.setMediaType(SDPMedia::TYPE_AUDIO);
  audioSDP.setDataPort(49230);
  audioSDP.setControlPort(49241);
  audioSDP.addPayload(0);
  audioSDP.addPayload(8);
  audioSDP.addPayload(96);
  audioSDP.addPayload(97);
  audioSDP.addPayload(98);
  audioSDP.addPayload(101);
  audioSDP.setAddressV4("10.0.0.10");
  audioSDP.setRTPMap(96, "L8/8000");
  audioSDP.setRTPMap(97, "L16/8000");
  audioSDP.setRTPMap(98, "L16/11025/2");
  audioSDP.setRTPMap(101, "telephone-event/8000");
  audioSDP.setRTPMap(200, "this is a test");
  audioSDP.removeRTPMap(200);
  audioSDP.setFTMP(101, "0-15");
  audioSDP.setPtime(20);
  audioSDP.setDirection(SDPMedia::MEDIA_SEND_ONLY);
  audioSDP.setDirection(SDPMedia::MEDIA_SEND_AND_RECEIVE);


  ASSERT_TRUE(audioSDP.getMediaType() == SDPMedia::TYPE_AUDIO);
  ASSERT_TRUE(audioSDP.getDataPort() == 49230);
  ASSERT_TRUE(audioSDP.getControlPort() == 49241);
  ASSERT_TRUE(audioSDP.hasPayload(0));
  ASSERT_TRUE(audioSDP.hasPayload(8));
  ASSERT_TRUE(audioSDP.hasPayload(96));
  ASSERT_TRUE(audioSDP.hasPayload(97));
  ASSERT_TRUE(audioSDP.hasPayload(98));
  ASSERT_TRUE(audioSDP.hasPayload(101));
  ASSERT_TRUE(audioSDP.getAddress() == "10.0.0.10");
  ASSERT_TRUE(audioSDP.getRTPMap(96) == "L8/8000");
  ASSERT_TRUE(audioSDP.getRTPMap(97) == "L16/8000");
  ASSERT_TRUE(audioSDP.getRTPMap(98) == "L16/11025/2");
  ASSERT_TRUE(audioSDP.getRTPMap(101) == "telephone-event/8000");
  ASSERT_TRUE(audioSDP.getRTPMap(200).empty());
  ASSERT_TRUE(audioSDP.getFMTP(101) == "0-15");
  ASSERT_TRUE(audioSDP.getPtime() == 20);
  ASSERT_TRUE(audioSDP.getDirection() == SDPMedia::MEDIA_SEND_AND_RECEIVE);
  audioSDP.removePayload(96);
  ASSERT_TRUE(!audioSDP.hasPayload(96));


  std::stringstream audio;
  audio << "m=audio 49230 RTP/AVP 0 8 96 97 98 101" << "\r\n";
  audio << "c=IN IP4 10.0.0.10" << "\r\n";
  audio << "a=rtpmap:96 L8/8000" << "\r\n";
  audio << "a=rtpmap:97 L16/8000" << "\r\n";
  audio << "a=rtpmap:98 L16/11025/2" << "\r\n";
  audio << "a=rtpmap:101 telephone-event/8000" << "\r\n";
  audio << "a=fmtp:101 0-15" << "\r\n";
  audio << "a=sendonly" << "\r\n";
  audio << "a=ptime:20" << "\r\n";
  audio << "m=audio 49230 RTP/AVP 0 8 96 97 98 101" << "\r\n";
  audio << "c=IN IP4 10.0.0.10" << "\r\n";
  audio << "a=rtpmap:96 L8/8000" << "\r\n";
  audio << "a=rtpmap:97 L16/8000" << "\r\n";
  audio << "a=rtpmap:98 L16/11025/2" << "\r\n";
  audio << "a=rtpmap:101 telephone-event/8000" << "\r\n";
  audio << "a=fmtp:101 0-15" << "\r\n";
  audio << "a=sendonly" << "\r\n";
  audio << "a=ptime:20" << "\r\n";


  SDPMedia audioSDP1(audio.str().c_str());
  SDPMedia audioSDP2;
  audioSDP2 = audioSDP1;
  audioSDP1.reset();

  ASSERT_TRUE(audioSDP2.getMediaType() == SDPMedia::TYPE_AUDIO);
  ASSERT_TRUE(audioSDP2.getDataPort() == 49230);
  ASSERT_TRUE(audioSDP2.getControlPort() == 49231);
  ASSERT_TRUE(audioSDP2.hasPayload(0));
  ASSERT_TRUE(audioSDP2.hasPayload(8));
  ASSERT_TRUE(audioSDP2.hasPayload(96));
  ASSERT_TRUE(audioSDP2.hasPayload(97));
  ASSERT_TRUE(audioSDP2.hasPayload(98));
  ASSERT_TRUE(audioSDP2.hasPayload(101));
  ASSERT_TRUE(audioSDP2.getAddress() == "10.0.0.10");
  ASSERT_TRUE(audioSDP2.getRTPMap(96) == "L8/8000");
  ASSERT_TRUE(audioSDP2.getRTPMap(97) == "L16/8000");
  ASSERT_TRUE(audioSDP2.getRTPMap(98) == "L16/11025/2");
  ASSERT_TRUE(audioSDP2.getRTPMap(101) == "telephone-event/8000");
  ASSERT_TRUE(audioSDP2.getRTPMap(200).empty());
  ASSERT_TRUE(audioSDP2.getFMTP(101) == "0-15");
  ASSERT_TRUE(audioSDP2.getPtime() == 20);
  ASSERT_TRUE(audioSDP2.getDirection() == SDPMedia::MEDIA_SEND_ONLY);

  SDPSession s1(sdp.str().c_str());
  SDPSession s2 = s1;
  SDPSession s3(s2);
  s3 = s3;
  ASSERT_TRUE(s3.getMediaCount(SDPMedia::TYPE_AUDIO) == 1);
  ASSERT_TRUE(s3.getMediaCount(SDPMedia::TYPE_VIDEO) == 1);
  s3.removeMedia(SDPMedia::TYPE_AUDIO, 0);
  ASSERT_TRUE(s3.getMediaCount(SDPMedia::TYPE_AUDIO) == 0);
  s3.addMedia(SDPMedia::Ptr(new SDPMedia(audioSDP)));
  ASSERT_TRUE(s3.getMediaCount(SDPMedia::TYPE_AUDIO) == 1);



  std::ostringstream faxSDP;
  faxSDP << "v=0" << std::endl;
  faxSDP << "o=- 324805632 1357327348 IN IP4 10.100.151.5" << std::endl;
  faxSDP << "s=-" << std::endl;
  faxSDP << "c=IN IP4 10.100.151.5" << std::endl;
  faxSDP << "t=0 0" << std::endl;
  faxSDP << "a=sendrecv" << std::endl;
  faxSDP << "m=image 12416 udptl t38" << std::endl;
  faxSDP << "a=T38FaxVersion:0" << std::endl;
  faxSDP << "a=T38MaxBitRate:14400" << std::endl;
  faxSDP << "a=T38FaxRateManagement:transferredTCF" << std::endl;
  faxSDP << "a=T38FaxMaxBuffer:300" << std::endl;
  faxSDP << "a=T38FaxMaxDatagram:122" << std::endl;
  faxSDP << "a=T38FaxUdpEC:t38UDPRedundancy" << std::endl;
  faxSDP << "a=silenceSupp:off - - - -" << std::endl;
  faxSDP << "a=ptime:20" << std::endl;
  faxSDP << "a=maxptime:40" << std::endl;

  SDPSession faxSession(faxSDP.str().c_str());
  SDPMedia::Ptr fax = faxSession.getMedia(SDPMedia::TYPE_FAX);
  ASSERT_TRUE(fax);

  std::string sessionAddress = faxSession.getAddress();
  std::string address = fax->getAddress();
  ASSERT_TRUE(address.empty());
  ASSERT_FALSE(sessionAddress.empty());
  OSS::Net::IPAddress mediaAddress(sessionAddress);
  ASSERT_TRUE(mediaAddress.isPrivate());
  fax->setDataPort(10000);
  faxSession.changeAddress("10.0.0.1", "IP4");

  std::cout << faxSession.toString() << std::endl;

}