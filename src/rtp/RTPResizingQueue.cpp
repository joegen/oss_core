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


#include "OSS/RTP/RTPResizingQueue.h"


namespace OSS {
namespace RTP {


RTPResizingQueue::RTPResizingQueue() :
  _payloadType(0),
  _clockRate(0),
  _baseSampleSize(0),
  _lastSequence(0),
  _lastTimeSent(0),
  _packetizationBaseTime(0),
  _packetizationTargetTime(0)
{
}

RTPResizingQueue::RTPResizingQueue(
  unsigned int payloadType,
  unsigned int clockRate,
  unsigned int baseSampleSize,
  unsigned int packetizationBaseTimeMillis,
  unsigned int packetizationTargetTimeMillis
) : _payloadType(payloadType),
    _clockRate(clockRate),
    _baseSampleSize(baseSampleSize),
    _lastSequence(0),
    _lastTimeSent(0),
    _packetizationBaseTime(packetizationBaseTimeMillis),
    _packetizationTargetTime(packetizationTargetTimeMillis)

{
}

RTPResizingQueue::~RTPResizingQueue()
{
}

bool RTPResizingQueue::enqueue(const RTPPacket& packet)
{
	OSS::mutex_critic_sec_lock lock(_mutex);
	if (packet.getPayloadType() != _payloadType)
	  return false;

	//
	// Check if we have initialized sequence number
	//
	if (!_lastSequence)
	  _lastSequence = packet.getSequenceNumber() -1;


	//
	// Check if packet is of correct size
	//
	if (packet.getPayloadSize() % _baseSampleSize > 0)
	{
	  //
	  // Packet not correct size.  Send it right away
	  //
	  _out.push(packet);
	  return true;
	}

	//
	// Check if packet is already equal to our target
	//
	unsigned  int targetSize = (_packetizationTargetTime / _packetizationBaseTime) * _baseSampleSize;
	if (targetSize == packet.getPayloadSize())
	{
	  //
    // Packet is already the correct size.  Send it right away
    //
    _out.push(packet);
    return true;
	}

	//
	//  Let the dequeue do the resizing
	//
  _in.push(packet);

  return true;
}

bool RTPResizingQueue::dequeue(RTPPacket& packet)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  if (!_out.empty())
  {
    packet = _out.front();
    _lastTimeSent = packet.getTimeStamp();
    packet.setSequenceNumber(++_lastSequence);
    _out.pop();
    return true;
  }

  //
  // Discard late packets
  //
  while (!_in.empty())
  {
    if (_in.front().getTimeStamp() < _lastTimeSent)
      _in.pop();
    else
      break;
  }

  if (_in.empty())
    return false;

  //
  // Check if we have enough samples in the _in queue
  //
  unsigned int targetSize =  (_packetizationTargetTime / _packetizationBaseTime) * _baseSampleSize;
  if (targetSize < _in.front().getPayloadSize())
  {
    //
    // We are resizing down
    //
    if  (_in.front().getPayloadSize() % targetSize > 0)
    {
      //
      // Ratio is not proportional to the target.  We cannot resize this packet.
      //
      packet = _in.front();
      _lastTimeSent = packet.getTimeStamp();
      packet.setSequenceNumber(++_lastSequence);
      _in.pop();
      return true;
    }

    int packetSize = _in.front().getPayloadSize() / targetSize;
    unsigned len;
    u_char buff[RTP_PACKET_BUFFER_SIZE];
    packet.getPayload(buff, len);

    //
    // enqueue the packets
    //
    unsigned int offset = 0;
    unsigned int sampleCount = _in.front().getPayloadSize() / targetSize;
    for (unsigned int i = 0; i < sampleCount; i++)
    {
      RTPPacket sample(packet);
      //
      // Calculate the time stamp
      //
      unsigned int ts = packet.getTimeStamp() - (sampleCount - i) * ((targetSize/_baseSampleSize) * _clockRate);
      sample.setTimeStamp(ts);
      //
      // Set the new payload
      //
      sample.setPayload(buff + offset, targetSize);
      offset += targetSize;
      _out.push(sample);
    }
    _in.pop();
  }
  else
  {
    //
    // We are resizing up
    //
    unsigned int sampleCountRequired = targetSize / _in.front().getPayloadSize();
    if (_in.size() >= sampleCountRequired)
    {
      //
      // We have enough samples
      //
      u_char resizedBuff[RTP_PACKET_BUFFER_SIZE];
      unsigned int offset = 0;
      unsigned int ts;
      RTPPacket sample = _in.front();
      for (int i = 0; i < sampleCountRequired; i++)
      {
        u_char buff[RTP_PACKET_BUFFER_SIZE];
        unsigned int len = 0;
        _in.front().getPayload(buff, len);
        memcpy(resizedBuff + offset, buff, len);
        offset += len;
        ts = _in.front().getTimeStamp();
        _in.pop();
      }
      sample.setPayload(resizedBuff, offset);
      sample.setTimeStamp(ts);
      _out.push(sample);
    }
  }

  if (!_out.empty())
  {
    packet = _out.front();
    _lastTimeSent = packet.getTimeStamp();
    packet.setSequenceNumber(++_lastSequence);
    _out.pop();
    return true;
  }

  return false;
}



} } // OSS::RTP




