// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare 
// derivative works of the Software, all subject to the 
// "GNU Lesser General Public License (LGPL)".
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef SIP_SIPTransactionTimers_INCLUDED
#define SIP_SIPTransactionTimers_INCLUDED


#include "OSS/SIP/SIP.h"


namespace OSS {
namespace SIP {


class OSS_API SIPTransactionTimers
  /// This class encapsulates the timer expiration values
  /// that transactions would use.  Applications would
  /// normally override the default values of the
  /// timers if a different transaction timeout rate is
  /// required.  
{
public:
  SIPTransactionTimers();
    /// Creates a SIPTransactionTimers Object

  SIPTransactionTimers(const SIPTransactionTimers& timers);
    /// Creates a SIPTransactionTimers Object from another.

  SIPTransactionTimers& operator = (const SIPTransactionTimers& timers);
    /// Copies another SIPTransactionTimers object

  void swap(SIPTransactionTimers& timers);
    /// Exchanges the value between two SIPTransactionTimers objects.

  unsigned long& timerA();
    /// Returns a reference to the value of timer A

  unsigned long& timerB();
    /// Returns a reference to the value of timer B

  unsigned long& timerC();
    /// Returns a reference to the value of timer C

  unsigned long& timerD();
    /// Returns a reference to the value of timer D

  unsigned long& timerE();
    /// Returns a reference to the value of timer E

  unsigned long& timerF();
    /// Returns a reference to the value of timer F

  unsigned long& timerG();
    /// Returns a reference to the value of timer G

  unsigned long& timerH();
    /// Returns a reference to the value of timer H

  unsigned long& timerI();
    /// Returns a reference to the value of timer I

  unsigned long& timerJ();
    /// Returns a reference to the value of timer J

  unsigned long& timerK();
    /// Returns a reference to the value of timer K

  unsigned long& timerT1();
    /// Returns a reference to the value of timer T1

  unsigned long& timerT2();
    /// Returns a reference to the value of timer T2

  unsigned long& timerT4();
    /// Returns a reference to the value of timer T4

  unsigned long& timer_ICT_18x();
    /// Returns a reference to the value of timer ICT 18x

  unsigned long& timer_ICT_2xx();
    /// Returns a reference to the value of timer ICT 2xx

private:
  unsigned long _timerA;
  unsigned long _timerB;
  unsigned long _timerC;
  unsigned long _timerD;
  unsigned long _timerE;
  unsigned long _timerF;
  unsigned long _timerG;
  unsigned long _timerH;
  unsigned long _timerI;
  unsigned long _timerJ;
  unsigned long _timerK;
  unsigned long _timerT1;
  unsigned long _timerT2;
  unsigned long _timerT4;
  unsigned long _timer_ICT_18x;
  unsigned long _timer_ICT_2xx;
};

//
// Inlines
//

inline unsigned long& SIPTransactionTimers::timerA()
{
  return _timerA;
}

inline unsigned long& SIPTransactionTimers::timerB()
{
  return _timerB;
}

inline unsigned long& SIPTransactionTimers::timerC()
{
  return _timerC;
}

inline unsigned long& SIPTransactionTimers::timerD()
{
  return _timerD;
}

inline unsigned long& SIPTransactionTimers::timerE()
{
  return _timerE;
}

inline unsigned long& SIPTransactionTimers::timerF()
{
  return _timerF;
}

inline unsigned long& SIPTransactionTimers::timerG()
{
  return _timerG;
}

inline unsigned long& SIPTransactionTimers::timerH()
{
  return _timerH;
}

inline unsigned long& SIPTransactionTimers::timerI()
{
  return _timerI;
}

inline unsigned long& SIPTransactionTimers::timerJ()
{
  return _timerJ;
}

inline unsigned long& SIPTransactionTimers::timerK()
{
  return _timerK;
}

inline unsigned long& SIPTransactionTimers::timerT1()
{
  return _timerT1;
}

inline unsigned long& SIPTransactionTimers::timerT2()
{
  return _timerT2;
}

inline unsigned long& SIPTransactionTimers::timerT4()
{
  return _timerT4;
}

inline unsigned long& SIPTransactionTimers::timer_ICT_18x()
{
  return _timer_ICT_18x;
}

inline unsigned long& SIPTransactionTimers::timer_ICT_2xx()
{
  return _timer_ICT_2xx;
}

} } //OSS::SIP

#endif //SIP_SIPTransactionTimers_INCLUDED





