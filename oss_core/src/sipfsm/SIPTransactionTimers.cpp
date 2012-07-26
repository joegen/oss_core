// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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


#include "OSS/Core.h"
#include "OSS/SIP/SIPTransactionTimers.h"


namespace OSS {
namespace SIP {


#define SECONDS(v)v*1000
#define MINUTE(v)v*60000


static const int SIP_TIMER_T1_VAL = 500; /// T1 500ms default - RTT Estimate
static const int SIP_TIMER_T2_VAL = SECONDS(4); /// T2 4s - The maximum retransmit interval for non-INVITE requests and INVITE responses
static const int SIP_TIMER_T4_VAL = SECONDS(5); /// T4 5s - Maximum duration a message will remain in the network
static const int SIP_TIMER_A_VAL = SIP_TIMER_T1_VAL; /// Timer A  initially T1 - INVITE request retransmit interval, for UDP only
static const int SIP_TIMER_B_VAL = SIP_TIMER_T1_VAL * 64; /// Timer B  64*T1 - INVITE transaction timeout timer
static const int SIP_TIMER_C_VAL = MINUTE(3) + SECONDS(1); /// Timer C > 3min - proxy INVITE transaction timeout
static const int SIP_TIMER_D_VAL = SECONDS(32) + SECONDS(1); /// Timer D  > 32s for UDP / 0s for TCP/SCTP - Wait time for response retransmits
static const int SIP_TIMER_E_VAL = SIP_TIMER_T1_VAL; /// Timer E initially T1 - non-INVITE request retransmit interval, UDP only
static const int SIP_TIMER_F_VAL = SIP_TIMER_T1_VAL * 64; /// Timer F 64*T1 - non-INVITE transaction timeout timer
static const int SIP_TIMER_G_VAL = SIP_TIMER_T1_VAL; /// Timer G initially T1 - INVITE response retransmit interval
static const int SIP_TIMER_H_VAL = 64 * SIP_TIMER_T1_VAL; /// Timer H  64*T1 - Wait time for ACK receipt
static const int SIP_TIMER_I_VAL = SIP_TIMER_T4_VAL; ///Timer I  T4 for UDP / 0s for TCP/SCTP - Wait time for ACK retransmits
static const int SIP_TIMER_J_VAL = 64 * SIP_TIMER_T1_VAL; /// Timer J  64*T1 for UDP / 0s for TCP/SCTP - Wait time for non-INVITE request retransmits
static const int SIP_TIMER_K_VAL = SIP_TIMER_T4_VAL; /// Timer K  T4 for UDP / 0s for TCP/SCTP - Wait time for response retransmits
static const int SIP_TIMER_100REL_VAL = SIP_TIMER_T1_VAL; /// Timer 100 Rel T1 - Retransmission Timer for reliable provisional response
static const int SIP_TIMER_100RELACK_VAL = SIP_TIMER_F_VAL; /// Timer 100 Rel ACK Timer F - Retransmission timeout timer for reliable provisional response

SIPTransactionTimers::SIPTransactionTimers() :
  _timerA(SIP_TIMER_A_VAL),
  _timerB(SIP_TIMER_B_VAL),
  _timerC(SIP_TIMER_C_VAL),
  _timerD(SIP_TIMER_D_VAL),
  _timerE(SIP_TIMER_E_VAL),
  _timerF(SIP_TIMER_F_VAL),
  _timerG(SIP_TIMER_G_VAL),
  _timerH(SIP_TIMER_H_VAL),
  _timerI(SIP_TIMER_I_VAL),
  _timerJ(SIP_TIMER_J_VAL),
  _timerK(SIP_TIMER_K_VAL),
  _timerT1(SIP_TIMER_T1_VAL),
  _timerT2(SIP_TIMER_T2_VAL),
  _timerT4(SIP_TIMER_T4_VAL),
  _timer_ICT_18x(SIP_TIMER_B_VAL),
  _timer_ICT_2xx(SIP_TIMER_B_VAL*2)
{
}

SIPTransactionTimers::SIPTransactionTimers(const SIPTransactionTimers& t)
{
  _timerA = t._timerA;
  _timerB = t._timerB;
  _timerC = t._timerC;
  _timerD = t._timerD;
  _timerE = t._timerE;
  _timerF = t._timerF;
  _timerG = t._timerG;
  _timerH = t._timerH;
  _timerI = t._timerI;
  _timerJ = t._timerJ;
  _timerK = t._timerK;
  _timerT1 = t._timerT1;
  _timerT2 = t._timerT2;
  _timerT4 = t._timerT4;
  _timer_ICT_18x = t._timer_ICT_18x;
  _timer_ICT_2xx = t._timer_ICT_2xx;
}

SIPTransactionTimers& SIPTransactionTimers::operator = (const SIPTransactionTimers& t)
{
  SIPTransactionTimers clonable(t);
  swap(clonable);
  return *this;
}

void SIPTransactionTimers::swap(SIPTransactionTimers& t)
{
  std::swap(_timerA, t._timerA);
  std::swap(_timerB, t._timerB);
  std::swap(_timerC, t._timerC);
  std::swap(_timerD, t._timerD);
  std::swap(_timerE, t._timerE);
  std::swap(_timerF, t._timerF);
  std::swap(_timerG, t._timerG);
  std::swap(_timerH, t._timerH);
  std::swap(_timerI, t._timerI);
  std::swap(_timerJ, t._timerJ);
  std::swap(_timerK, t._timerK);
  std::swap(_timerT1, t._timerT1);
  std::swap(_timerT2, t._timerT2);
  std::swap(_timerT4, t._timerT4);
  std::swap(_timer_ICT_18x, t._timer_ICT_18x);
  std::swap(_timer_ICT_2xx, t._timer_ICT_2xx);
}

} } //OSS::SIP

