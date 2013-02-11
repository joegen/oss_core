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

#include <boost/array.hpp>

#include "OSS/RTP/RTPResizer.h"
#include "OSS/RTP/RTPProxy.h"


namespace OSS {
namespace RTP {


RTPResizer::RTPResizer(RTPProxy* pProxy, unsigned int legIndex) :
  _pProxy(pProxy),
  _queue(18, 80, 10, 10, 0),
  _samples(0),
  _lastTick(0),
  _duration(0),
  _pResizerThread(0),
  _isTerminating(false),
  _lastQueuedSize(0),
  _legIndex(legIndex)
{
  gettimeofday(&_bootTime, NULL);
}

RTPResizer::~RTPResizer()
{
  stop();
}

void RTPResizer::setSamples(int samples)
{
  _samples = samples;
  _queue.setPacketizationTargetTime(samples);
  _duration = (samples * 1000);
}

int RTPResizer::getSamples() const
{
  return _samples;
}

/// Reset the timers internals states
OSS::UInt64 RTPResizer::getTime()
{
  struct timeval sTimeVal, ret;
	gettimeofday( &sTimeVal, NULL );
  timersub(&sTimeVal, &_bootTime, &ret);
  return (OSS::UInt64)( (sTimeVal.tv_sec * 1000000) + sTimeVal.tv_usec );
}

void RTPResizer::wait()
{
  if (!_duration)
    return;

  unsigned long nextWait = _duration;
  if (!_lastTick)
  {
    _lastTick = getTime();
  }
  else
  {
    OSS::UInt64 now = getTime();
    OSS::UInt64 accuracy = now - _lastTick;

    _lastTick = now;
    if (accuracy < _duration)
    {
      //
      // Timer fired too early.  compensate by adding more ticks
      //
      nextWait = _duration + (_duration - accuracy);
      _lastTick = _lastTick + (_duration - accuracy);
    }
    else if (accuracy > _duration)
    {
      //
      // Timer fired too late.  compensate by removing some ticks
      //
      if ((accuracy - _duration) < _duration)
      {
        nextWait = _duration - (accuracy - _duration);
        _lastTick = _lastTick - (accuracy - _duration);
      }
      else
      {
        //
        // We are late by more than the duration value.
        // remove the entire duration value + the delta
        //
        nextWait = 0;
        //
        // We will fire now and offset the differce to the next iteration
        //
        _lastTick = _lastTick - ((accuracy - _duration) - _duration);
      }
    }
  }

  if (nextWait)
  {
    timeval sTimeout = { nextWait / 1000000, ( nextWait % 1000000 ) };
    select( 0, 0, 0, 0, &sTimeout );
  }
}

void RTPResizer::reset()
{
  _lastTick = 0;
}

void RTPResizer::stop()
{
  _isTerminating = true;
  if (_pResizerThread)
  {
    _pResizerThread->join();
    delete _pResizerThread;
    _pResizerThread = 0;
  }
  reset();
}

void RTPResizer::run()
{
  stop();
  _pResizerThread = new boost::thread(boost::bind(&RTPResizer::internal_run, this));
}

bool RTPResizer::enqueue(OSS::RTP::RTPResizingQueue::Data& buff, std::size_t& size)
{
  if (!_queue.enqueue(buff, size))
    return false;
  
  _lastQueuedSize = size;
  return true;
}

bool RTPResizer::dequeue(OSS::RTP::RTPResizingQueue::Data& buff, std::size_t& size)
{
  if (_pResizerThread || !_queue.dequeue(buff, size))
    return false;
  
  if (!_pResizerThread &&  _lastQueuedSize > size)
  {
    //
    // We are resizing down.  start the resize thread
    //
    run();
  }
  return true;
}


void RTPResizer::internal_run()
{
  _isTerminating = false;
  while (!_isTerminating)
  {
    wait();
    OSS::RTP::RTPPacket packet;
    if (_queue.dequeue(packet))
    {
      _pProxy->onResizerDequeue(*this, packet);
    }
  }
}


} } // OSS::RTP



