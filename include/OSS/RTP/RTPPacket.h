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


#ifndef OSS_RTPPACKET_H_INCLUDED
#define OSS_RTPPACKET_H_INCLUDED


#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <byteswap.h>
#include <string>


namespace OSS {
namespace RTP {

#define RTP_PACKET_BUFFER_SIZE 8192

class RTPPacket
{
public:


  struct Packet
  {
    Packet()
    {
      contributingSourceCount = 0;
      extension = 0;
      padding = 0;
      version = 0;
      payloadType = 0;
      marker = 0;
      sequenceNumber = 0;
      timeStamp = 0;
      synchronizationSource = 0;
    }


  #if BYTE_ORDER == BIG_ENDIAN
    unsigned int version:2;
    unsigned int padding:1;
    unsigned int extension:1;
    unsigned int contributingSourceCount:4;
    unsigned int marker:1;
    unsigned int payloadType:7;
  #elif  BYTE_ORDER == LITTLE_ENDIAN
    unsigned int contributingSourceCount:4;
    unsigned int extension:1;
    unsigned int padding:1;
    unsigned int version:2;
    unsigned int payloadType:7;
    unsigned int marker:1;
  #endif
    unsigned int sequenceNumber:16;
    unsigned int timeStamp;
    unsigned int synchronizationSource;
    unsigned int contributingSource[16];
    u_char payload[RTP_PACKET_BUFFER_SIZE];
  } __attribute__((packed));


	RTPPacket();
	RTPPacket(const RTPPacket& packet);
	RTPPacket(const u_char* packet, unsigned int size);
	~RTPPacket();
	void swap(RTPPacket& packet);
	RTPPacket& operator=(const RTPPacket& packet);

	unsigned int getVersion() const;
  unsigned int getPadding() const;
  unsigned int getExtension() const;
  unsigned int getContributingSourceCount() const;
  unsigned int getMarker() const;
  unsigned int getPayloadType() const;
  unsigned int getSequenceNumber() const;
  unsigned int getTimeStamp() const;
  unsigned int getSynchronizationSource() const;
  unsigned int getContributingSource(unsigned int index) const;
  void getPayload(u_char* payload , unsigned int& length) const;
  unsigned int getHeaderSize() const;
  unsigned int getPacketSize() const;
  unsigned int getPayloadSize() const;

  void setVersion(unsigned int version);
  void setPadding(unsigned int padding);
  void setExtension(unsigned int extension);
  void setContributingSourceCount(unsigned int contributingSourceCount);
  void setMarker(unsigned int marker);
  void setPayloadType(unsigned int payloadType);
  void setSequenceNumber(unsigned int sequenceNumber);
  void setTimeStamp(unsigned int timeStamp);
  void setSynchronizationSource(unsigned int synchronizationSource);
  void setContributingSource(unsigned int contributingSource, unsigned int index);
  void setPayload(u_char* payload, unsigned int size);
	bool parse(const u_char* packet, unsigned int size);
  const u_char* data() const;
  void setPacketSourceIp(const std::string& packetSourceIp);
  const std::string& getPacketSourceIp() const;
  void setPacketSourcePort(unsigned short sourcePort);
  unsigned short getPacketSourcePort() const;
  void setPacketDestinationIp(const std::string& packetDestinationIp);
  const std::string& getPacketDestinationIp() const;
  void setPacketDestinationPort(unsigned short destinationPort);
  unsigned short getPacketDestinationPort() const;

  unsigned int byteSwap16(unsigned int bytes) const;
  unsigned int byteSwap32(unsigned int bytes) const;

private:
	Packet _packet;
	unsigned int _packetSize;
	std::string _packetSourceIp;
	unsigned short _packetSourcePort;
	std::string _packetDestinationIp;
	unsigned short _packetDestinationPort;
};
  
//
// Inlines
//

inline unsigned int RTPPacket::byteSwap16(unsigned int bytes) const
{
  int ret = bytes;
#if BYTE_ORDER == LITTLE_ENDIAN
  ret = bswap_16(bytes);
#endif
  return ret;
}

inline unsigned int RTPPacket::byteSwap32(unsigned int bytes) const
{
  int ret = bytes;
  #if BYTE_ORDER == LITTLE_ENDIAN
    ret = bswap_32(bytes);
  #endif
  return ret;
}


inline unsigned int RTPPacket::getVersion() const
{
  return _packet.version;
}

inline unsigned int RTPPacket::getPadding() const
{
  return _packet.padding;
}

inline unsigned int RTPPacket::getExtension() const
{
  return _packet.extension;
}

inline unsigned int RTPPacket::getContributingSourceCount() const
{
  return _packet.contributingSourceCount;
}

inline unsigned int RTPPacket::getMarker() const
{
  return _packet.marker;
}

inline unsigned int RTPPacket::getPayloadType() const
{
  return _packet.payloadType;
}

inline unsigned int RTPPacket::getSequenceNumber() const
{
  return byteSwap16(_packet.sequenceNumber);
}

inline unsigned int RTPPacket::getTimeStamp() const
{
  return byteSwap32(_packet.timeStamp);
}

inline unsigned int RTPPacket::getSynchronizationSource() const
{
  return byteSwap32(_packet.synchronizationSource);
}

inline unsigned int RTPPacket::getContributingSource(unsigned int index) const
{
  if (index >= _packet.contributingSourceCount)
    return 0;
  return byteSwap32(_packet.contributingSource[index]);
}

inline unsigned int RTPPacket::getHeaderSize() const
{
  return (unsigned int) (12 + (4 * _packet.contributingSourceCount));
}

inline unsigned int RTPPacket::getPacketSize() const
{
  return _packetSize;
}

inline unsigned int RTPPacket::getPayloadSize() const
{
  if (!_packetSize || _packetSize < getHeaderSize())
    return 0;
  return (unsigned int)_packetSize - getHeaderSize();
}

inline void RTPPacket::getPayload(u_char* payload , unsigned int& length) const
{
  if (!getPayloadSize())
  {
    payload = 0;
    length = 0;
    return;
  }
  length = getPayloadSize();
  memcpy(payload, _packet.payload, length);
}

inline void RTPPacket::setPayload(u_char* payload, unsigned int length)
{
  memcpy(_packet.payload, payload, length);
  _packetSize = getHeaderSize() + length;
}

inline void RTPPacket::setVersion(unsigned int version)
{
  _packet.version = version;
}

inline void RTPPacket::setPadding(unsigned int padding)
{
  _packet.padding = padding;
}

inline void RTPPacket::setExtension(unsigned int extension)
{
  _packet.extension = extension;
}

inline void RTPPacket::setContributingSourceCount(unsigned int contributingSourceCount)
{
  _packet.contributingSourceCount = contributingSourceCount;
}

inline void RTPPacket::setMarker(unsigned int marker)
{
  _packet.marker = marker;
}

inline void RTPPacket::setPayloadType(unsigned int payloadType)
{
  _packet.payloadType = payloadType;
}

inline void RTPPacket::setSequenceNumber(unsigned int sequenceNumber)
{
  _packet.sequenceNumber = byteSwap16(sequenceNumber);
}

inline void RTPPacket::setTimeStamp(unsigned int timeStamp)
{
  _packet.timeStamp = byteSwap32(timeStamp);
}

inline void RTPPacket::setSynchronizationSource(unsigned int synchronizationSource)
{
  _packet.synchronizationSource = byteSwap32(synchronizationSource);
}

inline void RTPPacket::setContributingSource(unsigned int contributingSource, unsigned int index)
{
  assert(index < 16);
  _packet.contributingSource[index] = byteSwap32(contributingSource);
}

inline const u_char* RTPPacket::data() const
{
  return (const u_char*) &_packet;
}

inline void RTPPacket::setPacketSourceIp(const std::string& packetSourceIp)
{
  _packetSourceIp = packetSourceIp;
}

inline const std::string& RTPPacket::getPacketSourceIp() const
{
  return _packetSourceIp;
}

inline void RTPPacket::setPacketSourcePort(unsigned short sourcePort)
{
  _packetSourcePort = sourcePort;
}

inline unsigned short RTPPacket::getPacketSourcePort() const
{
  return _packetSourcePort;
}

inline void RTPPacket::setPacketDestinationIp(const std::string& packetDestinationIp)
{
  _packetDestinationIp = packetDestinationIp;
}

inline const std::string& RTPPacket::getPacketDestinationIp() const
{
  return _packetDestinationIp;
}

inline void RTPPacket::setPacketDestinationPort(unsigned short destinationPort)
{
  _packetDestinationPort = destinationPort;
}

inline unsigned short RTPPacket::getPacketDestinationPort() const
{
  return _packetDestinationPort;
}


} } // OSS::RTP


#endif // OSS_RTPPACKET_H_INCLUDED

