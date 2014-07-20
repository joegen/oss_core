// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include <sys/select.h>
#include <sys/time.h>
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/AdaptiveDelay.h"


namespace OSS {
namespace UTL {


AdaptiveDelay::AdaptiveDelay(unsigned long milliseconds) :
  _lastTick(0),
  _duration(milliseconds * 1000)
{
  gettimeofday(&_bootTime, NULL);
}

AdaptiveDelay::~AdaptiveDelay()
{
}

OSS::UInt64 AdaptiveDelay::getTime()
{
  struct timeval sTimeVal, ret;
	gettimeofday( &sTimeVal, NULL );
  timersub(&sTimeVal, &_bootTime, &ret);
  return (OSS::UInt64)( (sTimeVal.tv_sec * 1000000) + sTimeVal.tv_usec );
}

void AdaptiveDelay::wait()
{
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


void AdaptiveDelay::reset()
{
  _lastTick = 0;
}



} } // OSS::UTL



