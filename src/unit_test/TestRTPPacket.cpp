/*
 * Copyright (C) 2012  OSS Software Solutions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */


#include <boost/filesystem.hpp>
#include "gtest/gtest.h"
#include "OSS/RTP/RTPPacket.h"
#include "OSS/RTP/RTPPCAPReader.h"
#include "OSS/RTP/RTPResizingQueue.h"

using namespace OSS;
using namespace OSS::RTP;

TEST(RTPPacketTest, test_basic_packet_parser)
{
  u_char pkt1[] =
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
  ASSERT_EQ(packet1.getPayloadSize(), 20);


  const u_char* pkt2 = packet1.data();
   for (int i = 0; i < 32; i++)
   {
     ASSERT_EQ(pkt1[i], pkt2[i]);
   }

  RTPPacket packet2;
  packet2 = packet1;

  const u_char* pkt3 = packet2.data();
  for (int i = 0; i < 32; i++)
  {
    ASSERT_EQ(pkt1[i], pkt3[i]);
  }


  ASSERT_EQ(packet1.getSynchronizationSource(), packet2.getSynchronizationSource());
  ASSERT_EQ(packet1.getTimeStamp(), packet2.getTimeStamp());
  ASSERT_EQ(packet1.getPayloadType(), packet2.getPayloadType());
  ASSERT_EQ(packet1.getVersion(), packet2.getVersion());
  ASSERT_EQ(packet1.getSequenceNumber(), packet2.getSequenceNumber());

  unsigned int len;
  u_char payload1[RTP_PACKET_BUFFER_SIZE];
  u_char payload2[RTP_PACKET_BUFFER_SIZE];
  packet1.getPayload(payload1, len);
  packet2.getPayload(payload2, len);

  ASSERT_EQ(len, packet1.getPayloadSize());
  for (unsigned int i = 0; i < len; i++)
  {
    ASSERT_EQ(payload1[i], payload2[i]);
  }

  RTPPacket packet3;
  packet3.parse(packet2.data(), packet2.getPacketSize());

  ASSERT_EQ(packet3.getSynchronizationSource(), packet2.getSynchronizationSource());
  ASSERT_EQ(packet3.getTimeStamp(), packet2.getTimeStamp());
  ASSERT_EQ(packet3.getPayloadType(), packet2.getPayloadType());
  ASSERT_EQ(packet3.getVersion(), packet2.getVersion());
  ASSERT_EQ(packet3.getSequenceNumber(), packet2.getSequenceNumber());

  u_char payload3[RTP_PACKET_BUFFER_SIZE];
  packet3.getPayload(payload3, len);
  packet2.getPayload(payload2, len);

  ASSERT_EQ(len, packet3.getPayloadSize());
  for (unsigned int i = 0; i < len; i++)
  {
    ASSERT_EQ(payload3[i], payload2[i]);
  }

  //
  // Check payload integrity
  //

  u_char pl[] = { 0x22, 0xb3, 0x40, 0x77,
    0x02, 0x6d, 0x21, 0x37, 0xc3, 0x82, 0x26, 0xda,
    0x7f, 0xe4, 0xe8, 0x58, 0xd6, 0xa2, 0x3c, 0x5a };

  RTPPacket plpacket;
  plpacket.setPayload(pl, 20);

  ASSERT_EQ(plpacket.getPayloadSize(), 20);
  u_char pl1[RTP_PACKET_BUFFER_SIZE] ;
  plpacket.getPayload(pl1, len);

  for (int i = 0; i < 20; i++)
    ASSERT_EQ(pl1[i], pl[i]);
}

#if 0

TEST(RTPPacketTest, test_pcap_reader)
{
  if (boost::filesystem::exists("sample_rtp_audio_g729.pcap"))
  {
    RTPPCAPReader reader;
    ASSERT_TRUE(reader.open("sample_rtp_audio_g729.pcap"));

    RTPPacket packet;
    while (reader.read(packet))
    {
    }
  }
}



TEST(RTPPacketTest, test_rtp_resizer_pcap_upsize)
{
  if (boost::filesystem::exists("resizer_up.pcap"))
  {
    RTPPCAPReader reader;
    ASSERT_TRUE(reader.open("resizer_up.pcap"));
    RTPResizingQueue q(18, 80, 10, 10, 80);
    RTPPacket packet;
    while (reader.read(packet))
    {
      ASSERT_TRUE(q.enqueue(packet));
      while (q.dequeue(packet))
      {
      }
    }
  }
}

TEST(RTPPacketTest, test_rtp_resizer_pcap_downsize)
{
  if (boost::filesystem::exists("resizer_down.pcap"))
  {
    RTPPCAPReader reader;
    ASSERT_TRUE(reader.open("resizer_down.pcap"));
    RTPResizingQueue q(18, 80, 10, 10, 20);
    RTPPacket packet;
    while (reader.read(packet))
    {
      ASSERT_TRUE(q.enqueue(packet));
      while (q.dequeue(packet))
      {
      }
    }
  }
}

TEST(RTPPacketTest, test_rtp_resizer_pcap_samesize)
{
  if (boost::filesystem::exists("resizer_down.pcap"))
  {
    RTPPCAPReader reader;
    ASSERT_TRUE(reader.open("resizer_down.pcap"));
    RTPResizingQueue q(18, 80, 10, 10, 80);
    RTPPacket packet;
    while (reader.read(packet))
    {
      ASSERT_TRUE(q.enqueue(packet));

      RTPPacket packet2;
      ASSERT_TRUE(q.dequeue(packet));

       unsigned int len = 0;
       u_char payload1[RTP_PACKET_BUFFER_SIZE];
       u_char payload2[RTP_PACKET_BUFFER_SIZE];
       packet.getPayload(payload1, len);
       packet2.getPayload(payload2, len);

       for (int i = 0; i < len; i++)
       {
         ASSERT_EQ(payload1[i], payload2[i]);
       }
    }
  }
}
#endif




TEST(RTPPacketTest, test_rtp_resizer_downsize)
{
  u_char pkt1[] = {
  0x80, 0x92, 0x17, 0xb0, 0x00, 0x00,
  0x02, 0x80, 0x75, 0xae, 0xb1, 0x14, 0x78, 0x52,
  0x80, 0xa0, 0x00, 0xfa, 0xc2, 0x00, 0x07, 0xd6,
  0xf9, 0x5b, 0x05, 0xe0, 0x00, 0xfa, 0xdd, 0x0b,
  0xe0, 0xf2, 0x11, 0x3b, 0x4b, 0x12, 0x81, 0xfa,
  0xd1, 0xb5, 0x00, 0x5e, 0xf8, 0x13, 0x80, 0xa0,
  0x00, 0xfa, 0xc2, 0x00, 0x07, 0xd6, 0xf0, 0x2a,
  0xc0, 0xa0, 0x00, 0xfa, 0xc2, 0x00, 0x07, 0xd6,
  0x70, 0x46, 0x80, 0xa0, 0x00, 0xfa, 0xc2, 0x00,
  0x07, 0xd6, 0x79, 0xa4, 0x40, 0xa0, 0x00, 0xfa,
  0xc2, 0x00, 0x07, 0xd6, 0x78, 0x4b, 0xc0, 0xa0,
  0x00, 0xfa, 0xc2, 0x00, 0x07, 0xd6 };

  u_char pld1[] = {0x78, 0x52,
      0x80, 0xa0, 0x00, 0xfa, 0xc2, 0x00, 0x07, 0xd6,
      0xf9, 0x5b, 0x05, 0xe0, 0x00, 0xfa, 0xdd, 0x0b,
      0xe0, 0xf2, 0x11, 0x3b, 0x4b, 0x12, 0x81, 0xfa,
      0xd1, 0xb5, 0x00, 0x5e, 0xf8, 0x13, 0x80, 0xa0,
      0x00, 0xfa, 0xc2, 0x00, 0x07, 0xd6, 0xf0, 0x2a,
      0xc0, 0xa0, 0x00, 0xfa, 0xc2, 0x00, 0x07, 0xd6,
      0x70, 0x46, 0x80, 0xa0, 0x00, 0xfa, 0xc2, 0x00,
      0x07, 0xd6, 0x79, 0xa4, 0x40, 0xa0, 0x00, 0xfa,
      0xc2, 0x00, 0x07, 0xd6, 0x78, 0x4b, 0xc0, 0xa0,
      0x00, 0xfa, 0xc2, 0x00, 0x07, 0xd6 };

  RTPPacket packet;

  ASSERT_TRUE(packet.parse(pkt1, 92));
  RTPResizingQueue q(18, 80, 10, 10, 20);
  ASSERT_EQ(q.getTargetSize(), 20);
  ASSERT_EQ(q.getTargetClockRate(), 160);

  ASSERT_TRUE(q.enqueue(packet));



  RTPPacket p1, p2, p3, p4, p5;
  ASSERT_TRUE(q.dequeue(p1));
  ASSERT_TRUE(q.dequeue(p2));
  ASSERT_TRUE(q.dequeue(p3));
  ASSERT_TRUE(q.dequeue(p4));
  ASSERT_FALSE(q.dequeue(p5));

  //
  // We are expecting 4 samples
  //
  unsigned int len = 0;
  u_char d0[80];
  u_char d1[80];
  u_char d2[80];
  u_char d3[80];
  u_char d4[80];

  packet.getPayload(d0, len);
  for (unsigned int i = 0; i < len; i++)
  {
    ASSERT_EQ(d0[i], pld1[i]);
  }



  p1.getPayload(d1, len);
  p2.getPayload(d2, len);
  p3.getPayload(d3, len);
  p4.getPayload(d4, len);

  ASSERT_EQ(packet.getPayloadSize(), 80);
  ASSERT_EQ(p1.getPayloadSize(), 20);
  ASSERT_EQ(p2.getPayloadSize(), 20);
  ASSERT_EQ(p3.getPayloadSize(), 20);
  ASSERT_EQ(p4.getPayloadSize(), 20);

  //
  // We are expecting incrementing sequence
  //
  ASSERT_EQ(p1.getSequenceNumber(), p2.getSequenceNumber() - 1);
  ASSERT_EQ(p2.getSequenceNumber(), p3.getSequenceNumber() - 1);
  ASSERT_EQ(p3.getSequenceNumber(), p4.getSequenceNumber() - 1);

  //
  // We are expecting incrementing timestamps by 160
  //
  ASSERT_EQ(p1.getTimeStamp(), p2.getTimeStamp() - 160);
  ASSERT_EQ(p2.getTimeStamp(), p3.getTimeStamp() - 160);
  ASSERT_EQ(p3.getTimeStamp(), p4.getTimeStamp() - 160);


  int j = 0;
  for (int i = 0; i < 20; i++)
  {
    ASSERT_EQ(d0[j], d1[i]);
    j++;
  }

  for (int i = 0; i < 20; i++)
  {
    ASSERT_EQ(d0[j], d2[i]);
    j++;
  }

  for (int i = 0; i < 20; i++)
  {
    ASSERT_EQ(d0[j], d3[i]);
    j++;
  }

  for (int i = 0; i < 20; i++)
  {
    ASSERT_EQ(d0[j], d4[i]);
    j++;
  }
}

TEST(RTPPacketTest, test_rtp_resizer_upsize)
{
  u_char pkt1[] = {
  0x80, 0x12, 0x00, 0xb5, 0x00, 0x2c,
  0xcb, 0x6c, 0x00, 0x00, 0x3a, 0x87, 0x22, 0xb3,
  0x40, 0x77, 0x02, 0x6d, 0x21, 0x37, 0xc3, 0x82,
  0x26, 0xda, 0x7f, 0xe4, 0xe8, 0x58, 0xd6, 0xa2,
  0x3c, 0x5a };

  u_char pkt2[] = {
  0x80, 0x12, 0x00, 0xb6, 0x00, 0x2c,
  0xcc, 0x0c, 0x00, 0x00, 0x3a, 0x87, 0x22, 0xb3,
  0x40, 0x6d, 0x2a, 0xcc, 0xa1, 0x36, 0xfb, 0xba,
  0x8d, 0xb2, 0x7f, 0xed, 0x41, 0xab, 0x1b, 0x1b,
  0xca, 0x5a };

  u_char pkt3[] = {
  0x80, 0x12, 0x00, 0xb7, 0x00, 0x2c,
  0xcc, 0xac, 0x00, 0x00, 0x3a, 0x87, 0x0d, 0x64,
  0x40, 0x76, 0x22, 0x64, 0x61, 0x27, 0xc3, 0x8c,
  0x49, 0x72, 0xfb, 0x84, 0xc0, 0x7a, 0x48, 0xb5,
  0xce, 0x58 };

  u_char pkt4[] = {
  0x80, 0x12, 0x00, 0xb8, 0x00, 0x2c,
  0xcd, 0x4c, 0x00, 0x00, 0x3a, 0x87, 0x22, 0xdb,
  0xc0, 0x76, 0x14, 0x5d, 0x61, 0x63, 0xc5, 0xcc,
  0x05, 0xba, 0xff, 0xe4, 0xc6, 0x6f, 0xd7, 0xda,
  0x0b, 0x7e };

  RTPPacket p1, p2, p3, p4;
  ASSERT_TRUE(p1.parse(pkt1, 32));
  ASSERT_TRUE(p2.parse(pkt2, 32));
  ASSERT_TRUE(p3.parse(pkt3, 32));
  ASSERT_TRUE(p4.parse(pkt4, 32));

  RTPResizingQueue q(18, 80, 10, 10, 80);
  ASSERT_EQ(q.getTargetSize(), 80);
  ASSERT_EQ(q.getTargetClockRate(), 160 * 4);

  RTPPacket resized;
  ASSERT_TRUE(q.enqueue(p1));
  ASSERT_FALSE(q.dequeue(resized));
  ASSERT_TRUE(q.enqueue(p2));
  ASSERT_FALSE(q.dequeue(resized));
  ASSERT_TRUE(q.enqueue(p3));
  ASSERT_FALSE(q.dequeue(resized));
  ASSERT_TRUE(q.enqueue(p4));
  ASSERT_TRUE(q.dequeue(resized));

  ASSERT_EQ(resized.getPayloadSize(), 80);

  u_char d0[80];
  u_char d1[20];
  u_char d2[20];
  u_char d3[20];
  u_char d4[20];

  unsigned int len;
  resized.getPayload(d0, len);
  p1.getPayload(d1, len);
  p2.getPayload(d2, len);
  p3.getPayload(d3, len);
  p4.getPayload(d4, len);


  int j = 0;
  for (int i = 0; i < 20; i++)
  {
    ASSERT_EQ(d0[j], d1[i]);
    j++;
  }

  for (int i = 0; i < 20; i++)
  {
    ASSERT_EQ(d0[j], d2[i]);
    j++;
  }

  for (int i = 0; i < 20; i++)
  {
    ASSERT_EQ(d0[j], d3[i]);
    j++;
  }

  for (int i = 0; i < 20; i++)
  {
    ASSERT_EQ(d0[j], d4[i]);
    j++;
  }
}


