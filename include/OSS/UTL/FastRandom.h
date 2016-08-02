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

#include "OSS/build.h"

#if OSS_HAVE_BOOST_RANDOM

#include <ctime>
#include <cstdlib>
#include <boost/version.hpp>
#include <boost/thread.hpp>
#include <boost/random.hpp>

#if BOOST_VERSION > 104100
#define FRAND_HAS_BOOST_RANDOM_DEVICE 1
#else
#define FRAND_HAS_BOOST_RANDOM_DEVICE 0
#endif

#if FRAND_HAS_BOOST_RANDOM_DEVICE
#include <boost/nondet_random.hpp>
#endif

namespace OSS {
namespace UTL {

template <typename BaseType, typename LockStrategy>
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
 #if FRAND_HAS_BOOST_RANDOM_DEVICE
    ,_gen(_rng, boost::random::uniform_int_distribution<>(0, std::numeric_limits<BaseType>::max()))
#endif
  {
  }

  //
  // Deterministic random algorithm
  //
  BaseType rand() const
  {
    _s1 = 36969 * (_s1 & 65535) + (_s1 >> 16);
    _s2 = 18000 * (_s2 & 65535) + (_s2 >> 16);
    return ((((_s1 << 16) + _s2) + 1.0) * 2.328306435454494e-10) * std::numeric_limits<BaseType>::max();
  }

  //
  // Non-deterministic but really heavy random generation
  //
  BaseType non_deterministic_rand() const
  {
#if FRAND_HAS_BOOST_RANDOM_DEVICE
    return _gen();
#else
    return rand();
#endif
  }

  BaseType operator()() const
  {
    return rand();
  }
public:
  mutable LockStrategy _lockStrategy;
  mutable BaseType _s1;
  mutable BaseType _s2;
#if FRAND_HAS_BOOST_RANDOM_DEVICE
  mutable boost::random::random_device _rng;
  mutable boost::random::variate_generator< boost::random::random_device&,
    boost::random::uniform_int_distribution<>
  > _gen;
#endif
};

struct FastRandomNullLockStrategy
{
  void lock(){}
  void unlock(){}
};

typedef FastRandomBase<int32_t, FastRandomNullLockStrategy> FastRandom;
typedef FastRandomBase<int32_t, boost::mutex > FastRandomMt;

} } // OSS::UTL

#endif // OSS_HAVE_BOOST_RANDOM

#endif // OSS_FAST_RANDOM
