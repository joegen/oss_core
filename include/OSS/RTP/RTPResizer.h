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

#ifndef RTPRESIZER_H_INCLUDED
#define	RTPRESIZER_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_RTP

#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <OSS/UTL/Thread.h>
#include <OSS/RTP/RTPResizingQueue.h>


namespace OSS {
namespace RTP {


class RTPProxy;

class RTPResizer : boost::noncopyable
{
public:
  RTPResizer(RTPProxy* pProxy, unsigned int legIndex);
  ~RTPResizer();
  void setSamples(int samples);
  int getSamples() const;
  bool isEnabled() const;

  bool enqueue(OSS::RTP::RTPResizingQueue::Data& buff, std::size_t& size);
  bool dequeue(OSS::RTP::RTPResizingQueue::Data& buff, std::size_t& size);
  OSS::RTP::RTPResizingQueue& queue();
  const unsigned int legIndex() const;
  
protected:
  void wait();
  /// Adaptive syncrhonous wait
  void reset();
  /// Reset the timers internals states
  OSS::UInt64 getTime();
  /// Return microseconds elapsed since class creation

  void stop();
  void run();
  void internal_run();
private:
  RTPProxy* _pProxy;
  OSS::RTP::RTPResizingQueue _queue;
  int _samples;
  struct timeval _bootTime;
  OSS::UInt64 _lastTick;
  unsigned long _duration;
  boost::thread* _pResizerThread;
  bool _isTerminating;
  std::size_t _lastQueuedSize;
  unsigned int _legIndex;
  friend class RTPProxy;
};

//
// Inlines
//

inline bool RTPResizer::isEnabled() const
{
  return _samples > 0;
}

inline OSS::RTP::RTPResizingQueue& RTPResizer::queue()
{
  return _queue;
}

inline const unsigned int RTPResizer::legIndex() const
{
  return _legIndex;
}

} } // OSS::RTP

#endif // ENABLE_FEATURE_RTP

#endif	// RTPRESIZER_H_INCLUDED

