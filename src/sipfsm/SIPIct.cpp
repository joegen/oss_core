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
#include "OSS/UTL/Logger.h"

namespace OSS {
namespace SIP {


SIPIct::SIPIct(
  boost::asio::io_service& ioService,
  const SIPTransactionTimers& timerProps) :
  SIPFsm(ioService, timerProps),
  _timerAValue(0),
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
    if (pMsg->getProperty(OSS::PropertyMap::PROP_TransactionTimeout, sTimeout) && !sTimeout.empty())
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
  }

  return true;
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
      pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
      //
      // The first 2xx response sets the transaction-set to terminated
      //
      if (pParent)
        pParent->setState(SIPTransaction::TRN_STATE_TERMINATED);
    }
    else if (pMsg->isErrorResponse())
    {
      pTransaction->setState(COMPLETED);
      pTransaction->informTU(pMsg, pTransport);
      handleSendAck(pMsg, pTransport);
    }
    break;
  case PROCEEDING:
    if (pMsg->is1xx())
    {
      pTransaction->informTU(pMsg, pTransport);
    }
    else if (pMsg->is2xx())
    {
      pTransaction->informTU(pMsg, pTransport);
      pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
      //
      // The first 2xx response sets the transaction-set to terminated
      //
      if (pParent)
        pParent->setState(SIPTransaction::TRN_STATE_TERMINATED);
    }
    else if (pMsg->isErrorResponse())
    {
      pTransaction->setState(COMPLETED);
      pTransaction->informTU(pMsg, pTransport);
      handleSendAck(pMsg, pTransport);
    }
    break;
  case COMPLETED:
    if (pMsg->isErrorResponse())
    {
      //
      // Resend ACK but don't start timer.  It has already been started in prior states
      //
      handleSendAck(pMsg, pTransport, false);
    }
    break;
  }

  if (pTransaction->getState() == SIPTransaction::TRN_STATE_TERMINATED)
  {
    pTransaction->terminate();
    if (pParent && pParent->getActiveBranchCount() == 0)
    {
      pParent->terminate();
    }
  }
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

void SIPIct::handleSendAck(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport, bool startTimer)
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction || pTransaction->getState() == SIPTransaction::TRN_STATE_TERMINATED)
    return;

  pTransaction->setState(COMPLETED);

  if (!_pAck)
  {
    _pAck = SIPMessage::Ptr(new SIPMessage());
    *(_pAck.get()) = *(_pRequest.get());

    size_t methodPos = _pRequest->startLine().find(' ');
    if (methodPos == std::string::npos)
      return;

    _pAck->startLine() = "ACK";
    _pAck->startLine() += _pRequest->startLine().c_str() + methodPos;

    _pAck->hdrSet(OSS::SIP::HDR_TO, pMsg->hdrGet(OSS::SIP::HDR_TO));

    SIPCSeq cSeq;
    cSeq = _pRequest->hdrGet(OSS::SIP::HDR_CSEQ);
    std::string ackCSeq = cSeq.getNumber();
    ackCSeq += " ACK";
    _pAck->hdrSet(OSS::SIP::HDR_CSEQ, ackCSeq);

    _pAck->body() = "";
    _pAck->hdrRemove(OSS::SIP::HDR_CONTENT_LENGTH);
    _pAck->hdrRemove(OSS::SIP::HDR_CONTENT_TYPE);

    _pAck->commitData();
  }

  std::ostringstream logMsg;
  logMsg << pTransaction->getLogId() << ">>> " << _pAck->startLine()
  << " LEN: " << _pAck->data().size()
  << " SRC: " << pTransport->getLocalAddress().toIpPortString()
  << " DST: " << pTransaction->remoteAddress().toString()
  << " ENC: " << pTransaction->isXOREncrypted()
  << " PROT: " << pTransport->getTransportScheme();

  OSS::log_information(logMsg.str());
  
  if (OSS::log_get_level() >= OSS::PRIO_DEBUG)
    OSS::log_debug(_pAck->createLoggerData());

  if (pTransport->isReliableTransport())
  {
    pTransport->writeMessage(_pAck);
    pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
  }
  else
  {
    if (startTimer)
    {
      startTimerD();
      pTransport->writeMessage(_pAck,
      pTransaction->remoteAddress().toString(),
        OSS::string_from_number<unsigned short>(pTransaction->remoteAddress().getPort()));
    }
  }
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


