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
  startTimerMaxLifetime(300000); /// five minutes
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

  bool is2xx = pMsg->is2xx();

  switch (pTransaction->getState())
  {
  case SIPTransaction::TRN_STATE_IDLE:
  case TRYING:
    if (pMsg->is1xx())
    {
      pTransaction->setState(PROCEEDING);
      if (!pTransaction->isParent() && pTransaction->getParent()->getState() < PROCEEDING)
        pTransaction->getParent()->setState(PROCEEDING);

      pTransaction->informTU(pMsg, pTransport);
    }
    else 
    {
      if (pTransaction->isParent() || !is2xx)
      {
        //
        // If we are the parent or this is and error response
        // cancel e and f timers
        //
        cancelTimerE();
        cancelTimerF();
      }
      else if (!pTransaction->isParent() && is2xx)
      {
        //
        // If this is a branch and it is a 2xx reponse,
        // then cancel both parents timers as sell
        //
        cancelTimerE();
        cancelTimerF();
        pTransaction->getParent()->fsm()->cancelTimerE();
        pTransaction->getParent()->fsm()->cancelTimerF();
        pTransaction->getParent()->setState(COMPLETED);
      }

      pTransaction->setState(COMPLETED);
      pTransaction->informTU(pMsg, pTransport);
      
      if (!pTransport->isReliableTransport())
      {
        //
        // Start the longest time we want to handle retransmissions of 200 OK
        //
        startTimerK();
      }
      else
      {
        pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
        if (is2xx || pTransaction->allBranchesCompleted())
          pTransaction->getParent()->setState(SIPTransaction::TRN_STATE_TERMINATED);

      }
    }
    break;
  case PROCEEDING:
    if (pMsg->is1xx())
    {
      pTransaction->informTU(pMsg, pTransport);
    }
    else 
    {
      if (pTransaction->isParent() || !is2xx)
      {
        //
        // If we are the parent or this is and error response
        // cancel e and f timers
        //
        cancelTimerE();
        cancelTimerF();
      }
      else if (!pTransaction->isParent() && is2xx)
      {
        //
        // If this is a branch and it is a 2xx reponse,
        // then cancel both parents timers as sell
        //
        cancelTimerE();
        cancelTimerF();
        pTransaction->getParent()->fsm()->cancelTimerE();
        pTransaction->getParent()->fsm()->cancelTimerF();
        pTransaction->getParent()->setState(COMPLETED);
      }
      
      pTransaction->setState(COMPLETED);
      pTransaction->informTU(pMsg, pTransport);
      if (!pTransport->isReliableTransport())
      {
        //
        // Start the longest time we want to handle retransmissions of 200 OK
        //
        startTimerK();
      }
      else
      {
        pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
        if (is2xx || pTransaction->allBranchesCompleted())
          pTransaction->getParent()->setState(SIPTransaction::TRN_STATE_TERMINATED);

      }
    }
    break;
  case COMPLETED:
    break;
  }

  if (pTransaction->getState() == SIPTransaction::TRN_STATE_TERMINATED)
      pTransaction->terminate();

  if (pTransaction->getParent()->getState() == SIPTransaction::TRN_STATE_TERMINATED)
      pTransaction->getParent()->terminate();

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

bool SIPNict::isCompleted() const
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return true; /// If we can't lock the transaction pointer it means it is termianted
  return  pTransaction->getState() >= COMPLETED;
}

} } // OSS::SIP


