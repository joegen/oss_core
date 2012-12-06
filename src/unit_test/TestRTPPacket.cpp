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

  RTPPacket packet2;
  packet1 = packet2;

  ASSERT_EQ(packet1.getSynchronizationSource(), packet2.getSynchronizationSource());
  ASSERT_EQ(packet1.getTimeStamp(), packet2.getTimeStamp());
  ASSERT_EQ(packet1.getPayloadType(), packet2.getPayloadType());
  ASSERT_EQ(packet1.getVersion(), packet2.getVersion());
  ASSERT_EQ(packet1.getSequenceNumber(), packet2.getSequenceNumber());

  unsigned int len;
  u_char payload1[8192];
  u_char payload2[8192];
  packet1.getPayload(payload1, len);
  packet2.getPayload(payload2, len);

  ASSERT_EQ(len, packet1.getPayloadSize());
  for (int i = 0; i < len; i++)
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

  u_char payload3[8192];
  packet3.getPayload(payload3, len);
  packet2.getPayload(payload2, len);

  ASSERT_EQ(len, packet3.getPayloadSize());
  for (int i = 0; i < len; i++)
  {
    ASSERT_EQ(payload3[i], payload2[i]);
  }
}

TEST(RTPPacketTest, test_pcap_reader)
{
  if (boost::filesystem::exists("sample_rtp_audio_g729.pcap"))
  {
    RTPPCAPReader reader;
    ASSERT_TRUE(reader.open("sample_rtp_audio_g729.pcap"));

    RTPPacket packet;
    while (reader.read(packet))
    {
        std::cout
          << "SSRC: " << packet.getSynchronizationSource() << " "
          << "SEQ: " << packet.getSequenceNumber() << " "
          << "TS: " << packet.getTimeStamp() << " "
          << "PT: " << packet.getPayloadType() << " "
          << "SZ: " << packet.getPayloadSize() << std::endl;
    }
  }
}

TEST(RTPPacketTest, test_rtp_resizer_upsize)
{
  if (boost::filesystem::exists("resizer_up.pcap"))
  {
    RTPPCAPReader reader;
    ASSERT_TRUE(reader.open("resizer_up.pcap"));
    RTPResizingQueue q(18, 80, 10, 10, 80);
    RTPPacket packet;
    while (reader.read(packet))
    {
      if (!q.enqueue(packet))
      {
        std::cout << "IMMEDIATE:\t"
               << "SEQ: " << packet.getSequenceNumber() << " "
               << "TS: " << packet.getTimeStamp() << " "
               << "SZ: " << packet.getPayloadSize() << std::endl;
      }

      while (q.dequeue(packet))
      {
        std::cout << "RESIZED: \t"
                       << "SEQ: " << packet.getSequenceNumber() << " "
                       << "TS: " << packet.getTimeStamp() << " "
                       << "SZ: " << packet.getPayloadSize() << std::endl;
      }
    }
  }
}

TEST(RTPPacketTest, test_rtp_resizer_downsize)
{
  if (boost::filesystem::exists("resizer_down.pcap"))
  {
    RTPPCAPReader reader;
    ASSERT_TRUE(reader.open("resizer_down.pcap"));
    RTPResizingQueue q(18, 80, 10, 10, 20);
    RTPPacket packet;
    while (reader.read(packet))
    {
      if (!q.enqueue(packet))
      {
        std::cout << "IMMEDIATE:\t"
               << "SEQ: " << packet.getSequenceNumber() << " "
               << "TS: " << packet.getTimeStamp() << " "
               << "SZ: " << packet.getPayloadSize() << std::endl;
      }

      while (q.dequeue(packet))
      {
        std::cout << "RESIZED: \t"
                       << "SEQ: " << packet.getSequenceNumber() << " "
                       << "TS: " << packet.getTimeStamp() << " "
                       << "SZ: " << packet.getPayloadSize() << std::endl;
      }
    }
  }
}
