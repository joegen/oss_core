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

#ifndef OSS_ADAPTIVEDELAY_H_INCLUDED
#define	OSS_ADAPTIVEDELAY_H_INCLUDED


namespace OSS {
namespace UTL {

class AdaptiveDelay
  /// This class implements an adaptive recurring syncrhonous timer.
  /// It monitors the delta between waits and adjusts the wait time
  /// to compensate for the skew in between waits.  This guaranties
  /// that the timer stays within range of the real total wait time
  /// even if it is running for a very long time.
{
public:
  AdaptiveDelay(unsigned long milliseconds);
  /// Creates an adaptive deley timer
  ~AdaptiveDelay();
  /// Destroyes the timer
  void wait();
  /// Adaptive syncrhonous wait
  void reset();
  /// Reset the timers internals states
  OSS::UInt64 getTime();
  /// Return microseconds elapsed since class creation
private:
  struct timeval _bootTime;
  OSS::UInt64 _lastTick;
  unsigned long _duration;
};


} } // OSS::UTL



#endif	// OSS_ADAPTIVEDELAY_H_INCLUDED

