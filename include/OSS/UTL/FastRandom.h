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

#ifndef OSS_FAST_RANDOM
#define	OSS_FAST_RANDOM


#include <ctime>
#include <cstdlib>
#include <boost/thread.hpp>

namespace OSS {
namespace UTL {

template <typename BaseType, typename LockStrategy, BaseType rand_max>
class FastRandomBase
{
  //
  // Portable random number generator algorithm floating around the web.
  // I've tested this against rand() function and it is a bit faster
  // but has less linear distribution accuracy.
  //
public:
  FastRandomBase(BaseType seed1, BaseType seed2) :
    _s1(seed1),
    _s2(seed2)
  {
  }

  BaseType rand()
  {
    _s1 = 36969 * (_s1 & 65535) + (_s1 >> 16);
    _s2 = 18000 * (_s2 & 65535) + (_s2 >> 16);
    return ((((_s1 << 16) + _s2) + 1.0) * 2.328306435454494e-10) * rand_max;
  }
public:
  LockStrategy _lockStrategy;
  BaseType _s1;
  BaseType _s2;
};

struct FastRandomNullLockStrategy
{
  void lock(){}
  void unlock(){}
};

typedef FastRandomBase<unsigned int, FastRandomNullLockStrategy, RAND_MAX> FastRandom;
typedef FastRandomBase<unsigned int, boost::mutex, RAND_MAX> FastRandomMt;

} } // OSS::UTL

#endif // OSS_FAST_RANDOM
