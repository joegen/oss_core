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


#include "OSS/SIP/SIPNict.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/SIPFSMDispatch.h"


namespace OSS {
namespace SIP {


SIPNict::SIPNict(
  boost::asio::io_service& ioService,
  const SIPTransactionTimers& timerProps) :
  SIPFsm(ioService, timerProps),
  _timerEValue(0),
  _timerEMultiplier(0)
{
  _timerEFunc = boost::bind(&SIPNict::handleRetransmitRequest, this);
  _timerFFunc = boost::bind(&SIPNict::handleRequestTimeout, this);
  _timerKFunc = boost::bind(&SIPNict::handleDelayedTerminate, this);
  _timerMaxLifetimeFunc = boost::bind(&SIPNict::handleDelayedTerminate, this);
}

SIPNict::~SIPNict()
{
}

bool SIPNict::onSendMessage(SIPMessage::Ptr pMsg)
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return false;

  if (pTransaction->getState() == SIPTransaction::TRN_STATE_IDLE)
  {
    _pRequest = pMsg;
    pTransaction->setState(TRYING);

    std::string sTimeout;
    if (pMsg->getProperty("transaction-timeout", sTimeout) && !sTimeout.empty())
    {
      _timerEValue = OSS::string_to_number<unsigned long>(sTimeout.c_str()) / 64;
    }
    else
    {
      _timerEValue = _timerProps.timerE();
    }

    if (!pTransaction->transport()->isReliableTransport())
      startTimerE(_timerEValue);

    startTimerF(_timerEValue*64);
    startTimerMaxLifetime(300000); /// five minutes
    return true;
  }
  return false;
}

void SIPNict::onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;


  int state = pTransaction->getState();
  if (!pMsg->isResponse() || state == SIPTransaction::TRN_STATE_TERMINATED || state == COMPLETED)
    return;

  switch (pTransaction->getState())
  {
  case TRYING:
    if (pMsg->is1xx())
    {
      pTransaction->setState(PROCEEDING);
      pTransaction->informTU(pMsg, pTransport);
    }
    else 
    {
      cancelTimerE();
      cancelTimerF();
      pTransaction->setState(COMPLETED);
      pTransaction->informTU(pMsg, pTransport);
      
      if (!pTransport->isReliableTransport())
        startTimerK();
      else
        pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
    }
    break;
  case PROCEEDING:
    if (pMsg->is1xx())
    {
      pTransaction->informTU(pMsg, pTransport);
    }
    else 
    {
      cancelTimerE();
      cancelTimerF();
      pTransaction->setState(COMPLETED);
      pTransaction->informTU(pMsg, pTransport);
      if (!pTransport->isReliableTransport())
        startTimerK();
      else
        pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
    }
    break;
  case COMPLETED:
    break;
  }

  if (pTransaction->getState() == SIPTransaction::TRN_STATE_TERMINATED)
    pTransaction->terminate();
}

void SIPNict::handleRetransmitRequest()
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;

  if (pTransaction->getState() == SIPTransaction::TRN_STATE_TERMINATED)
    return;

  if (pTransaction->getState() <= PROCEEDING)
  {
    if (pTransaction->transport()->isReliableTransport())
      pTransaction->transport()->writeMessage(_pRequest);
    else
      pTransaction->transport()->writeMessage(_pRequest,
        pTransaction->remoteAddress().toString(),
        OSS::string_from_number<unsigned short>(pTransaction->remoteAddress().getPort()));
    //
    // Restart Timer E with a compounded value
    //
    if (_timerEMultiplier == 0)
      _timerEMultiplier = 2;
    else
      _timerEMultiplier = 4;

    if ( pTransaction->getState() != PROCEEDING)
      _timerEValue = _timerEValue * _timerEMultiplier > _timerProps.timerT2() ? _timerProps.timerT2() : _timerProps.timerT1() *_timerEMultiplier;
    else
      _timerEValue = _timerProps.timerT2();

    startTimerE(_timerEValue);
  }
}

void SIPNict::handleRequestTimeout()
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;

  int state = pTransaction->getState();
  if (state == SIPTransaction::TRN_STATE_TERMINATED || state == COMPLETED)
    return;

  cancelTimerK();
  cancelTimerE();
  cancelTimerF();
  pTransaction->handleTimeoutNICT();
  pTransaction->terminate();
}

void SIPNict::handleDelayedTerminate()
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;

  if (pTransaction->getState() == SIPTransaction::TRN_STATE_TERMINATED)
    return;

  cancelTimerK();
  cancelTimerE();
  cancelTimerF();

  pTransaction->terminate();
}

} } // OSS::SIP


