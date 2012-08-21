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


#ifndef SIP_SIPTransactionPool_INCLUDED
#define SIP_SIPTransactionPool_INCLUDED


#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPTransaction.h"


namespace OSS {
namespace SIP {


class SIPFSMDispatch;

class OSS_API SIPTransactionPool : private boost::noncopyable
  /// The SIP Transaction pool groups SIPTransaction using
  /// an internal unordered hash map.  Each SIPTransaction
  /// int the hash map is identifiable using an identifier
  ///
  ///     transaction-id = method  cseq  (via-branch / callid)
  /// 
  /// If a call to getTransaction() hits the end() of the map,
  /// a SIPTransaction is automatically created.  
  /// When a transaction hits the terminated state, it must
  /// be destroyed using a call to removeTransaction().
  ///
  /// To keep the transaction pool size small, the recommended
  /// implementation is to maintain a separate pool for each
  /// SIP state machine type.
  ///
{
public:
  typedef boost::unordered_map<std::string, SIPTransaction::Ptr> TransactionPool;

  SIPTransactionPool(SIPFSMDispatch* dispatch);
    /// Creates a new SIPTransactionPool object.

  virtual ~SIPTransactionPool();
    /// Destroys the SIPTransactionPool object

  SIPTransaction::Ptr findTransaction(const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport, bool canCreateTrn = true);
  SIPTransaction::Ptr findTransaction(const std::string& id, bool canCreateTrn = false);
    /// Returns a shared pointer to a transaction.
    ///
    /// The transaction is identifiable using an identifier
    ///
    ///     transaction-id = method  cseq  (via-branch / callid)
    ///
    /// The transaction will be created if it does not exist in the pool
    ///

  bool removeTransaction(const std::string &id);
    /// Removes the transaction from the transaction pool.
    ///
    /// The transaction is identifiable using an identifier
    ///
    ///     transaction-id = method  cseq  (via-branch / callid)
    ///
    /// This function will return false if the transaction
    /// does not exist in the transaction pool

  virtual void onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport);
    /// This method is called when a SIP message is received from the transport.
    ///
    /// If a transaction is not existing to handle the SIPMessage, 
    /// it is automatically created

  virtual void onAttachFSM(const SIPTransaction::Ptr& pTransaction) = 0;
    /// Attach an FSM to a new created transaction

  void onHouseKeepingTimer(const boost::system::error_code& e);
    /// Called by the internal timer to perform house-keeping task.
    /// 
    /// The default interval is every 5 seconds

  SIPTransactionTimers& timerProps();
    /// Return the SIP Transaction Timer expire property object.
    ///
    /// This will be used by the FSM or timer values.  This is where
    /// application may adjust the expire values for SIP timers as needed.

  SIPFSMDispatch* dispatch();
    /// Returns a raw pointer to the FSMDispatch

  void stop();
    /// Forcibly terminate all transactions

  virtual SIPTransaction::Ptr findChildTransaction(const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport, const SIPTransaction::Ptr& parent);
    /// Called within findTransaction to determine if forking is required

protected:
  boost::asio::io_service& _ioService;
  SIPTransactionTimers _timerProps;

private:
  boost::mutex _mutex;
  TransactionPool _transactionPool;
  boost::shared_ptr<boost::thread> _ioServiceThread;
  boost::asio::deadline_timer _houseKeepingTimer;
  SIPFSMDispatch* _pDispatch;
};

//
// Inlines
//

inline SIPTransactionTimers& SIPTransactionPool::timerProps()
{
  return _timerProps;
}

inline SIPFSMDispatch* SIPTransactionPool::dispatch()
{
  return _pDispatch;
}

inline SIPTransaction::Ptr SIPTransactionPool::findChildTransaction(const SIPMessage::Ptr& /*pMsg*/, const SIPTransportSession::Ptr& /*pTransport*/, const SIPTransaction::Ptr& /*parent*/)
{
  return SIPTransaction::Ptr();
}

} } // OSS::SIP
#endif //SIP_SIPTransactionPool_INCLUDED

