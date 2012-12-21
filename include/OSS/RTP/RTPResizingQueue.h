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


#ifndef RTPRESIZINGQUEUE_H_
#define RTPRESIZINGQUEUE_H_


#include <queue>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include "OSS/Thread.h"
#include "OSS/RTP/RTPPacket.h"
#include "OSS/Core.h"

namespace OSS {
namespace RTP {


class RTPResizingQueue : boost::noncopyable
{
public:
  typedef std::queue<RTPPacket> Queue;
  typedef boost::array<char, RTP_PACKET_BUFFER_SIZE> Data;
  RTPResizingQueue();

  RTPResizingQueue(
      unsigned int payloadType,
      unsigned int clockRate,
      unsigned int baseSampleSize,
      unsigned int packetizationBaseTimeMillis,
      unsigned int packetizationTargetTimeMillis
      );

  ~RTPResizingQueue();


  bool enqueue(const RTPPacket& packet);
  bool dequeue(RTPPacket& packet);
  bool enqueue(Data& buff, std::size_t& size);
  bool dequeue(Data& buff, std::size_t& size);

  unsigned int getPayloadType() const;
  void setPayloadType(unsigned int payloadType);

  unsigned int getClockRate() const;
  void setClockRate(unsigned int clockRate);

  unsigned int getBaseSampleSize() const;
  void setBaseSampleSize(unsigned int baseSampleSize);

  unsigned int getPacketizationBaseTime() const;
  void setPacketizationBaseTime(unsigned int packetizationBaseTime);

  unsigned int getPacketizationTargetTime() const;
  void setPacketizationTargetTime(unsigned int packetizationTargetTime);

  unsigned int getLastTimeSent() const;
  unsigned int getLastSequence() const;

  unsigned int getTargetSize() const;
  unsigned int getTargetClockRate() const;
  bool& verbose();

private:
  Queue _in;
  Queue _out;
  OSS::mutex_critic_sec _mutex;

  unsigned int _payloadType;
  unsigned int _clockRate;
  unsigned int _baseSampleSize;
  unsigned int _lastSequence;
  unsigned int _lastTimeSent;
  unsigned int _packetizationBaseTime;
  unsigned int _packetizationTargetTime;
  bool _verbose;
  bool _isResizing;
};

//
// Inlines
//

inline unsigned int RTPResizingQueue::getPayloadType() const
{
  return _payloadType;
}

inline void RTPResizingQueue::setPayloadType(unsigned int payloadType)
{
  _payloadType = payloadType;
}

inline unsigned int RTPResizingQueue::getClockRate() const
{
  return _clockRate;
}

inline void RTPResizingQueue::setClockRate(unsigned int clockRate)
{
  _clockRate = clockRate;
}

inline unsigned int RTPResizingQueue::getBaseSampleSize() const
{
  return _baseSampleSize;
}

inline void RTPResizingQueue::setBaseSampleSize(unsigned int baseSampleSize)
{
  _baseSampleSize = baseSampleSize;
}

inline unsigned int RTPResizingQueue::getPacketizationBaseTime() const
{
  return _packetizationBaseTime;
}

inline void RTPResizingQueue::setPacketizationBaseTime(unsigned int packetizationBaseTime)
{
  _packetizationBaseTime = packetizationBaseTime;
}

inline unsigned int RTPResizingQueue::getPacketizationTargetTime() const
{
  return _packetizationTargetTime;
}

inline void RTPResizingQueue::setPacketizationTargetTime(unsigned int packetizationTargetTime)
{
  _packetizationTargetTime = packetizationTargetTime;
}


inline unsigned int RTPResizingQueue::getLastTimeSent() const
{
  return _lastTimeSent;
}

inline unsigned int RTPResizingQueue::getLastSequence() const
{
  return _lastSequence;
}

inline unsigned int RTPResizingQueue::getTargetSize() const
{
  return (_packetizationTargetTime / _packetizationBaseTime) * _baseSampleSize;
}

inline unsigned int RTPResizingQueue::getTargetClockRate() const
{
  return ((getTargetSize()/_baseSampleSize) * _clockRate);
}

inline bool& RTPResizingQueue::verbose()
{
  return _verbose;
}

} } // OSS::RTP



#endif /* RTPRESIZINGQUEUE_H_ */
