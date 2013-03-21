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


#include "OSS/SIP/SIPIst.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPIstPool.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/Logger.h"

namespace OSS {
namespace SIP {


SIPIst::SIPIst(
  boost::asio::io_service& ioService,
  const SIPTransactionTimers& timerProps) :
  SIPFsm(ioService, timerProps),
  _timerGValue(0),
  _istPool(0)
{
  _timerGFunc = boost::bind(&SIPIst::handleRetransmitResponse, this);
  _timerHFunc = boost::bind(&SIPIst::handleACKTimeout, this);
  _timerIFunc = boost::bind(&SIPIst::handleDelayedTerminate, this);
  _timerMaxLifetimeFunc = boost::bind(&SIPIst::handleDelayedTerminate, this);
}

SIPIst::~SIPIst()
{
}

void SIPIst::onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;

  if (_transactionId.empty())
    _transactionId = pTransaction->getId();

  if (pMsg->isRequest() && pTransaction->getState() == SIPTransaction::TRN_STATE_IDLE)
  {
     startTimerMaxLifetime(300000); /// five minutes
    _pRequest = pMsg;
    pTransaction->setState(PROCEEDING);
    if (dispatch()->requestHandler())
    {
      //
      // Set the rport and received parameters if requested
      //
      std::string topVia;
      SIPVia::msgPopTopVia(pMsg.get(), topVia);
      std::string rport;
      if (SIPVia::getRPort(topVia, rport))
      {
        rport = OSS::string_from_number(pTransport->getRemoteAddress().getPort());
        SIPVia::setParam(topVia, "rport", rport.c_str());
        SIPVia::setParam(topVia, "received", 
          OSS::string_from_number(pTransport->getRemoteAddress().toString().c_str()).c_str());
      }
      SIPVia::msgAddVia(pMsg.get(), topVia);
      dispatch()->requestHandler()(pMsg, pTransport, pTransaction->shared_from_this());
    }
  }
  else if (pTransaction->getState() == COMPLETED && pMsg->isRequest("ACK"))
  {
    cancelTimerH();
    if (!pTransaction->transport()->isReliableTransport())
    {
      pTransaction->setState(CONFIRMED);
      cancelTimerG();
      startTimerI();
    }
    else
    {
      pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
      pTransaction->terminate();
    }
  }
  else if(pTransaction->getState() == COMPLETED)
  {
    if (pMsg->isRequest())
    {
      OSS::mutex_critic_sec_lock responseLock(_responseMutex);
      if (_pResponse)
      {
        if (pTransaction->transport()->isReliableTransport())
          pTransaction->transport()->writeMessage(_pResponse);
        else
          pTransaction->transport()->writeMessage(_pResponse,
          pTransaction->sendAddress().toString(),
            OSS::string_from_number<unsigned short>(pTransaction->sendAddress().getPort()));
      }
    }
  }
}

bool SIPIst::onSendMessage(SIPMessage::Ptr pMsg)
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return false;

  if(!pMsg->isResponse())
    return false;

  {//localize
  OSS::mutex_critic_sec_lock responseLock(_responseMutex);
  if (_pResponse)
    _pResponse.reset();
  _pResponse = pMsg;
  }//localize

  if (pTransaction->getState() <= PROCEEDING)
  {
    if (pMsg->is1xx())
    {
    }
    else if (pMsg->is2xx())
    {
      pTransaction->setState(SIPTransaction::TRN_STATE_TERMINATED);
      pTransaction->terminate();
      
    }else
    {
      pTransaction->setState(COMPLETED);
      if (!pTransaction->transport()->isReliableTransport())
      {
        _timerGValue = _timerProps.timerG();
        startTimerG();
      }
      startTimerH();
    }
  }

  return true;
}

void SIPIst::handleRetransmitResponse()
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;

  OSS::mutex_critic_sec_lock responseLock(_responseMutex);
  if (_pResponse && (pTransaction->getState() == COMPLETED))
  {
    if (pTransaction->transport()->isReliableTransport())
      pTransaction->transport()->writeMessage(_pResponse);
    else
      pTransaction->transport()->writeMessage(_pResponse,
      pTransaction->sendAddress().toString(),
        OSS::string_from_number<unsigned short>(pTransaction->sendAddress().getPort()));
    //
    // Restart Timer G with a compounded value
    //
    if (pTransaction->getState() != SIPTransaction::TRN_STATE_TERMINATED)
    {
      _timerGValue = _timerGValue * 2 > _timerProps.timerT2() ? _timerProps.timerT2() : _timerGValue * 2;
      startTimerG(_timerGValue);
    }
  }
}

void SIPIst::handleACKTimeout()
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return;
  pTransaction->terminate();
}

void SIPIst::onTerminate()
{
  if (!_transactionId.empty() && _istPool)
  {
    SIPFSMDispatch* pDispatch = _istPool->dispatch();
    if (pDispatch)
    {
      pDispatch->blockIst(_transactionId);
    }
  }
}

bool SIPIst::isCompleted() const
{
  SIPTransaction::Ptr pTransaction = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  if (!pTransaction)
    return true; /// If we can't lock the transaction pointer it means it is termianted
  return  pTransaction->getState() >= COMPLETED;
}

} } // OSS::SIP
