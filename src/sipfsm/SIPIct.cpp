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


#include "OSS/SIP/SIPIct.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/Logger.h"

namespace OSS {
namespace SIP {


SIPIct::SIPIct(
  boost::asio::io_service& ioService,
  const SIPTransactionTimers& timerProps) :
  SIPFsm(ioService, timerProps),
  _timerAValue(0),
  _ackMutex(),
  _pAck()
{
  _timerAFunc = boost::bind(&SIPIct::handleRetransmitInvite, this);
  _timerBFunc = boost::bind(&SIPIct::handleInviteTimeout, this);
  _timerDFunc = boost::bind(&SIPIct::handleDelayedTerminate, this);
  _timerMaxLifetimeFunc = boost::bind(&SIPIct::handleDelayedTerminate, this);
}

SIPIct::~SIPIct()
{
}

bool SIPIct::onSendMessage(SIPMessage::Ptr pMsg)
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return false;

  if (pTransaction->getState() == SIPTransaction::TRN_STATE_IDLE)
  {
    startTimerMaxLifetime(300000); /// five minutes
    _pRequest = pMsg;
    pTransaction->setState(TRYING);

    std::string sTimeout;
    if (pMsg->getProperty("transaction-timeout", sTimeout) && !sTimeout.empty())
    {
      _timerAValue = OSS::string_to_number<unsigned long>(sTimeout.c_str()) / 64;
    }
    else
    {
      _timerAValue = _timerProps.timerA();
    }
    if (!pTransaction->transport()->isReliableTransport())
      startTimerA(_timerAValue);

    startTimerB(_timerAValue*64);
    return true;
  }
  else if (pMsg->isRequest("ACK"))
  {
    OSS::mutex_critic_sec_lock lock(_ackMutex);
    if (!_pAck)
      _pAck = pMsg;
    return true;
  }
  return false;
}

void SIPIct::onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;

  if (!pMsg->isResponse() || !pTransaction || pTransaction->getState() == SIPTransaction::TRN_STATE_TERMINATED)
    return;

  SIPTransaction::Ptr pParent = pTransaction->getParent();

  switch (pTransaction->getState())
  {
  case SIPTransaction::TRN_STATE_IDLE:
  case TRYING:
    cancelTimerA();
    cancelTimerB();

    if (pParent)
    {
      pParent->fsm()->cancelTimerA();
      pParent->fsm()->cancelTimerB();
    }

    if (pMsg->is1xx())
    {
      pTransaction->setState(PROCEEDING);
      if (pParent && pParent->getState() < PROCEEDING)
        pParent->setState(PROCEEDING);

      pTransaction->informTU(pMsg, pTransport);
    }
    else if (pMsg->is2xx())
    {
      pTransaction->informTU(pMsg, pTransport);

      if (!pTransaction->willSendAckFor2xx())
      {
        pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
      }
      else
      {
        if (!pTransport->isReliableTransport())
        {
          pTransaction->setState(SIPTransaction::TRN_STATE_ACK_PENDING);
          startTimerD();
        }
        else
          pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
      }
    }
    else if (pMsg->isErrorResponse())
    {
      pTransaction->setState(COMPLETED);
      pTransaction->informTU(pMsg, pTransport);
      if (!pTransport->isReliableTransport())
        startTimerD();
      else
        pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
    }
    break;
  case PROCEEDING:
    if (pMsg->is1xx())
    {
      #if 0
      if (dispatch()->dialogHandler())
      {
        dispatch()->dialogHandler()(pMsg, pTransport, pTransaction->shared_from_this());
      }
      #endif
      pTransaction->informTU(pMsg, pTransport);
    }
    else if (pMsg->is2xx())
    {
      #if 0
      if (dispatch()->dialogHandler())
      {
        dispatch()->dialogHandler()(pMsg, pTransport, pTransaction->shared_from_this());
      }
      #endif
      pTransaction->informTU(pMsg, pTransport);

      if (!pTransaction->willSendAckFor2xx())
      {
        pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
      }
      else
      {
        if (!pTransport->isReliableTransport())
        {
          pTransaction->setState(SIPTransaction::TRN_STATE_ACK_PENDING);
          startTimerD();
        }
        else
        {
          pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
        }
      }
    }
    else if (pMsg->isErrorResponse())
    {
      pTransaction->setState(COMPLETED);
      pTransaction->informTU(pMsg, pTransport);
      if (!pTransport->isReliableTransport())
        startTimerD();
      else
      {
        //
        // Send ACK right away for reliable transports then set state to terminated
        //
        handleSendAck(pMsg, pTransport);
        pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
      }
    }
    break;
  case SIPTransaction::TRN_STATE_ACK_PENDING:
    {//localize
      OSS::mutex_critic_sec_lock lock(_ackMutex);
      if (_pAck )
      {
        if (pMsg->is2xx())
        {
          if (pTransaction->transport()->isReliableTransport())
            pTransaction->transport()->writeMessage(_pAck);
          else
            pTransaction->transport()->writeMessage(_pAck,
            pTransaction->dialogTarget().toString(),
              OSS::string_from_number<unsigned short>(pTransaction->dialogTarget().getPort()));
        }
      }
    }//localize
    break;
  case COMPLETED:
    break;
  }

  if (pMsg->isErrorResponse() && pTransaction->getState() == COMPLETED)
    handleSendAck(pMsg, pTransport);
  else if (pTransaction->getState() == SIPTransaction::TRN_STATE_TERMINATED)
    pTransaction->terminate();
}

void SIPIct::handleRetransmitInvite()
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;

  if (pTransaction->getState() == TRYING)
  {
    if (pTransaction->transport()->isReliableTransport())
      pTransaction->transport()->writeMessage(_pRequest);
    else
      pTransaction->transport()->writeMessage(_pRequest,
      pTransaction->remoteAddress().toString(),
        OSS::string_from_number<unsigned short>(pTransaction->remoteAddress().getPort()));
    //
    // Restart Timer A with a compounded value
    //
    _timerAValue = _timerAValue * 2;
    startTimerA(_timerAValue);
  }
}

void SIPIct::handleInviteTimeout()
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;

  cancelTimerA();
  pTransaction->handleTimeoutICT();
  pTransaction->terminate();
}

void SIPIct::handleSendAck(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;

  SIPMessage::Ptr ack = SIPMessage::Ptr(new SIPMessage());
  *(ack.get()) = *(_pRequest.get());
  
  size_t methodPos = _pRequest->startLine().find(' ');
  if (methodPos == std::string::npos)
    return;

  ack->startLine() = "ACK";
  ack->startLine() += _pRequest->startLine().c_str() + methodPos;
  
  ack->hdrSet("To", pMsg->hdrGet("to"));

  SIPCSeq cSeq;
  cSeq = _pRequest->hdrGet("cseq");
  std::string ackCSeq = cSeq.getNumber();
  ackCSeq += " ACK";
  ack->hdrSet("CSeq", ackCSeq);

  ack->body() = "";
  ack->hdrRemove("Content-Length");
  ack->hdrRemove("Content-Type");

  ack->commitData();

  std::ostringstream logMsg;
  logMsg << pTransaction->getLogId() << ">>> " << ack->startLine()
  << " LEN: " << ack->data().size()
  << " SRC: " << pTransport->getLocalAddress().toIpPortString()
  << " DST: " << pTransaction->remoteAddress().toString()
  << " ENC: " << pTransaction->isXOREncrypted()
  << " PROT: " << pTransport->getTransportScheme();

  OSS::log_information(logMsg.str());
  
  if (OSS::log_get_level() >= OSS::PRIO_DEBUG)
    OSS::log_debug(ack->createLoggerData());

  if (pTransport->isReliableTransport())
    pTransport->writeMessage(ack);
  else
    pTransport->writeMessage(ack, 
    pTransaction->remoteAddress().toString(),
      OSS::string_from_number<unsigned short>(pTransaction->remoteAddress().getPort()));
}

void SIPIct::handleDelayedTerminate()
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;
  pTransaction->terminate();
}

SIPIct::Ptr SIPIct::clone() const
{
  return SIPIct::Ptr();
}

bool SIPIct::isCompleted() const
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return true; /// If we can't lock the transaction pointer it means it is termianted
  return  pTransaction->getState() >= COMPLETED;
}

} } // OSS::SIP


