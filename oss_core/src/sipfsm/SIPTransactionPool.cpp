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


#include "OSS/SIP/SIPTransactionPool.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPTransportService.h"


namespace OSS {
namespace SIP {


SIPTransactionPool::SIPTransactionPool(SIPFSMDispatch* dispatch):
  _ioService(dispatch->transport().ioService()),
  _houseKeepingTimer(_ioService, boost::posix_time::seconds(0)),
  _pDispatch(dispatch)
{
  _houseKeepingTimer.expires_from_now(boost::posix_time::seconds(5));
  _houseKeepingTimer.async_wait(boost::bind(&SIPTransactionPool::onHouseKeepingTimer, this, boost::asio::placeholders::error));
  //
  // It is a bad idea to use a separate io service for transaction timers becuase
  // This will introduce a race condition between received messages an timer expirations
  //
  //_ioServiceThread = boost::shared_ptr<boost::thread>(
  //  new boost::thread(boost::bind(&boost::asio::io_service::run, &_ioService)));
}

SIPTransactionPool::~SIPTransactionPool()
{
  //
  // The pool no lnger owns the ioservice
  //
  //_ioService.stop();
  //_ioServiceThread->join();
}

void SIPTransactionPool::onHouseKeepingTimer(const boost::system::error_code& e)
{
  if (!e)
  {
    _houseKeepingTimer.expires_from_now(boost::posix_time::seconds(5));
    _houseKeepingTimer.async_wait(boost::bind(&SIPTransactionPool::onHouseKeepingTimer, this, boost::asio::placeholders::error));
  }
}

SIPTransaction::Ptr SIPTransactionPool::findTransaction(const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport, bool canCreateTrn)
{
  std::string id;
  if (!pMsg->getTransactionId(id))
    return SIPTransaction::Ptr();

  SIPTransaction::Ptr trn = findTransaction(id, canCreateTrn);

  if (trn)
  {
    SIPTransaction::Ptr child = findChildTransaction(pMsg, pTransport, trn);
    if(child)
      return child;
  }
  return trn;
}



SIPTransaction::Ptr SIPTransactionPool::findTransaction(const std::string& id, bool canCreateTrn)
{
  boost::lock_guard<boost::mutex> lock(_mutex);
  TransactionPool::iterator iter = _transactionPool.find(id);
  if (iter != _transactionPool.end())
    return iter->second;
  else if (!canCreateTrn)
    return SIPTransaction::Ptr();

  SIPTransaction::Ptr trn = SIPTransaction::Ptr(new SIPTransaction());
  trn->owner() = this;
  onAttachFSM(trn);
  trn->setId(id);
  _transactionPool[id] = trn;
  return trn;
}

bool SIPTransactionPool::removeTransaction(const std::string &id)
{
  boost::lock_guard<boost::mutex> lock(_mutex);

  TransactionPool::iterator iter = _transactionPool.find(id);
  if (iter == _transactionPool.end())
    return false;
  _transactionPool.erase(id);
  return true;
}

void SIPTransactionPool::onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport)
{
  boost::lock_guard<boost::mutex> lock(_mutex);

  SIPTransaction::Ptr trn = findTransaction(pMsg, pTransport);
  if (trn)
    trn->onReceivedMessage(pMsg, pTransport);
}

void SIPTransactionPool::stop()
{
  boost::lock_guard<boost::mutex> lock(_mutex);
  for (TransactionPool::iterator iter = _transactionPool.begin(); iter != _transactionPool.end(); iter++)
  {
    SIPTransaction::Ptr pTrn = iter->second;
    pTrn->setState(SIPTransaction::TRN_STATE_TERMINATED);
    pTrn->fsm()->cancelAllTimers();
  }
  _transactionPool.clear();
  //_ioService.stop();
}

} } // OSS::SIP
