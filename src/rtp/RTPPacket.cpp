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

#include <algorithm>
#include "OSS/RTP/RTPPacket.h"


namespace OSS {
namespace RTP {


RTPPacket::RTPPacket() :
  _packetSize(0),
  _packetSourcePort(0),
  _packetDestinationPort(0)
{
}

RTPPacket::RTPPacket(const RTPPacket& packet) :
  _packetSize(0),
  _packetSourcePort(0),
  _packetDestinationPort(0)
{
  _packet.contributingSourceCount = packet._packet.contributingSourceCount;
  _packet.extension = packet._packet.extension;
  _packet.padding = packet._packet.padding;
  _packet.version = packet._packet.version;
  _packet.payloadType = packet._packet.payloadType;
  _packet.marker = packet._packet.marker;
  _packet.sequenceNumber = packet._packet.sequenceNumber;
  _packet.timeStamp = packet._packet.timeStamp;
  _packet.synchronizationSource = packet._packet.synchronizationSource;
  memcpy(_packet.contributingSource, packet._packet.contributingSource, packet._packet.contributingSourceCount);
  memcpy(_packet.payload, packet._packet.payload, 8192);

  _packetSize = packet._packetSize;
  _packetSourceIp = packet._packetSourceIp;
  _packetSourcePort = packet._packetSourcePort;
  _packetDestinationIp = packet._packetDestinationIp;
  _packetDestinationPort = packet._packetDestinationPort;
}

RTPPacket::RTPPacket(const char* packet, unsigned int size) :
  _packetSize(0),
  _packetSourcePort(0),
  _packetDestinationPort(0)
{
  parse(packet ,size);
}

RTPPacket::~RTPPacket()
{
}

void RTPPacket::swap(RTPPacket& packet)
{
  std::swap(_packetSize, packet._packetSize);
  std::swap(_packetSourceIp, packet._packetSourceIp);
  std::swap(_packetSourcePort, packet._packetSourcePort);
  std::swap(_packetDestinationIp, packet._packetDestinationIp);
  std::swap(_packetDestinationPort, packet._packetDestinationPort);

  Packet copy;

  copy.contributingSourceCount  = packet._packet.contributingSourceCount;
  copy.extension = packet._packet.extension;
  copy.padding = packet._packet.padding;
  copy.version = packet._packet.version;
  copy.payloadType = packet._packet.payloadType;
  copy.marker = packet._packet.marker;
  copy.sequenceNumber = packet._packet.sequenceNumber;
  copy.timeStamp = packet._packet.timeStamp;
  copy.synchronizationSource = packet._packet.synchronizationSource;


  packet._packet.contributingSourceCount  = _packet.contributingSourceCount;
  packet._packet.extension = _packet.extension;
  packet._packet.padding = _packet.padding;
  packet._packet.version = _packet.version;
  packet._packet.payloadType = _packet.payloadType;
  packet._packet.marker = _packet.marker;
  packet._packet.sequenceNumber = _packet.sequenceNumber;
  packet._packet.timeStamp = _packet.timeStamp;
  packet._packet.synchronizationSource = _packet.synchronizationSource;

  for (int i = 0; i < 16; i++)
    std::swap(_packet.contributingSource[i], packet._packet.contributingSource[i]);

  for (int i = 0; i < 8192; i++)
    std::swap(_packet.payload[i], packet._packet.payload[i]);

  _packet.contributingSourceCount  = copy.contributingSourceCount;
  _packet.extension = copy.extension;
  _packet.padding = copy.padding;
  _packet.version = copy.version;
  _packet.payloadType = copy.payloadType;
  _packet.marker = copy.marker;
  _packet.sequenceNumber = copy.sequenceNumber;
  _packet.timeStamp = copy.timeStamp;
  _packet.synchronizationSource = copy.synchronizationSource;


}

RTPPacket& RTPPacket::operator=(const RTPPacket& packet)
{
  RTPPacket clonable(packet);
  swap(clonable);
  return *this;
}

bool RTPPacket::parse(const char* packet, unsigned int size)
{
  if (size < this->getHeaderSize())
    return false;

  memcpy(&_packet, packet, size);
  _packetSize = size;
  return true;
}


} } // OSS::RTP






