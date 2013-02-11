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
  _packetSize = packet._packetSize;
  _packetSourceIp = packet._packetSourceIp;
  _packetSourcePort = packet._packetSourcePort;
  _packetDestinationIp = packet._packetDestinationIp;
  _packetDestinationPort = packet._packetDestinationPort;
  parse(packet.data(), _packetSize);
}

RTPPacket::RTPPacket(const u_char* packet, unsigned int size) :
  _packetSize(0),
  _packetSourcePort(0),
  _packetDestinationPort(0)
{
  parse(packet ,size);
}

RTPPacket::~RTPPacket()
{
}

RTPPacket& RTPPacket::operator=(const RTPPacket& packet)
{
  _packetSize = packet._packetSize;
  _packetSourceIp = packet._packetSourceIp;
  _packetSourcePort = packet._packetSourcePort;
  _packetDestinationIp = packet._packetDestinationIp;
  _packetDestinationPort = packet._packetDestinationPort;
  parse(packet.data(), _packetSize);
  return *this;
}

bool RTPPacket::parse(const u_char* packet, unsigned int size)
{
  if (size < this->getHeaderSize())
    return false;

  memcpy(&_packet, packet, size);
  _packetSize = size;
  return _packet.version == 2;
}



} } // OSS::RTP






