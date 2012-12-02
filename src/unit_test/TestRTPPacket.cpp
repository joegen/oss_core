#include "gtest/gtest.h"
#include "OSS/RTP/RTPPacket.h"

using namespace OSS;
using namespace OSS::RTP;

TEST(RTPPacketTest, test_basic_packet_parser)
{
  char pkt1[] =
  {
    0x80, 0x12, 0x00, 0xb5, 0x00, 0x2c, 0xcb, 0x6c,
    0x00, 0x00, 0x3a, 0x87, 0x22, 0xb3, 0x40, 0x77,
    0x02, 0x6d, 0x21, 0x37, 0xc3, 0x82, 0x26, 0xda,
    0x7f, 0xe4, 0xe8, 0x58, 0xd6, 0xa2, 0x3c, 0x5a
  };

  RTPPacket packet1;
  ASSERT_TRUE(packet1.parse(pkt1, 32));
  ASSERT_EQ(packet1.getSynchronizationSource(), 0x3A87);
  ASSERT_EQ(packet1.getTimeStamp(), 2935660);
  ASSERT_EQ(packet1.getPayloadType(), 18);
  ASSERT_EQ(packet1.getVersion(), 2);
  ASSERT_EQ(packet1.getSequenceNumber(), 181);

  RTPPacket packet2;
  packet1 = packet2;

  ASSERT_EQ(packet1.getSynchronizationSource(), packet1.getSynchronizationSource());
  ASSERT_EQ(packet1.getTimeStamp(), packet1.getTimeStamp());
  ASSERT_EQ(packet1.getPayloadType(), packet1.getPayloadType());
  ASSERT_EQ(packet1.getVersion(), packet1.getVersion());
  ASSERT_EQ(packet1.getSequenceNumber(), packet1.getSequenceNumber());

  unsigned int len;
  char payload1[8192];
  char payload2[8192];
  packet1.getPayload(payload1, len);
  packet2.getPayload(payload2, len);

  ASSERT_EQ(len, packet1.getPayloadSize());
  for (int i = 0; i < len; i++)
  {
    ASSERT_EQ(payload1[i], payload2[i]);
  }
}
