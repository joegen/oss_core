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


#ifndef SIP_SIPFSM_INCLUDED
#define SIP_SIPFSM_INCLUDED


#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPTransactionTimers.h"


namespace OSS {
namespace SIP {


class SIPTransaction;
class SIPFSMDispatch;

class OSS_API SIPFsm: 
  public boost::enable_shared_from_this<SIPFsm>,
  private boost::noncopyable
/// This is the base class for the following Finite State Machines for SIP
///
///     Invite Client Transaction -         ICT
///     Invite Server Transaction -         IST
///     None Invite Client Transaction -    NIST
///     None Invite Server Transaction -    NICT
///
/// We also intoduced FSM extensions for the following
///
///     Cancel Invite Server Transaction -  CIST
///     Rel 100 Invite Server Transaction - RIST
///     Rel 100 Invite Client Transaction - RICT
///
/// The basic pupose of the SIP FSM is to track the current state
/// of a SIP transaction based on messages received from the network
/// or from the core and if responses to them are in proper order.
/// It also keeps track of the timers responsible for scheduling 
/// retramission intervals for none reliable transport as
/// well as transaction timeouts.
{
public:
  enum TransactionType
  {
    InviteClientTransaction,
    InviteServerTransaction,
    NonInviteServerTransaction,
    NonInviteClientTransaction
  };
  
  typedef boost::shared_ptr<SIPFsm> Ptr;
  typedef boost::function<void()> TimerCallback;

  SIPFsm(
    TransactionType type,      
    boost::asio::io_service& ioService,
    const SIPTransactionTimers& timerProps);
    /// Creates a new FSM object

  virtual ~SIPFsm();
    /// Destroys the FSM object

  virtual void onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport) = 0;
    /// This method is called by the transaction when a 
    /// SIP message is received from the transport.
    ///
    /// Take note that this is called directly from the 
    /// transport proactor thread and should therefore
    /// not block and result to a transport sleep.

  virtual bool onSendMessage(SIPMessage::Ptr pMsg) = 0;
    /// This method is called by SIPTransaction::writeMessage 
    /// when a SIP message is needed to be sent to the transport 
    /// coming from the core layer.

  virtual void onTerminate();
    /// Called by the transaction before destruction

  SIPTransaction* getOwner() const;
    /// Returns a pointer to the owning transaction.

  void setOwner(OSS_HANDLE owner);
    /// Set the transaction owner.  This must be a pointer to a newly create SIPTransaction::WeakPtr

  OSS_HANDLE _owner;  /// The transaction attached to the FSM.
                                 /// Take note that this can't be a share_ptr since it will 
                                 /// result to a cyclic reference between the transaction and the FSM

  SIPTransactionTimers& timerProps();
    /// Return the SIP Transaction Timer expire property object

  void startTimerA(unsigned long expire = 0);
    /// Starts timer A.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerB(unsigned long expire = 0);
    /// Starts timer B.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerC(unsigned long expire = 0);
    /// Starts timer C.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerD(unsigned long expire = 0);
    /// Starts timer D.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerE(unsigned long expire = 0);
    /// Starts timer E.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerF(unsigned long expire = 0);
    /// Starts timer F.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerG(unsigned long expire = 0);
    /// Starts timer G.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerH(unsigned long expire = 0);
    /// Starts timer H.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerI(unsigned long expire = 0);
    /// Starts timer I.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerJ(unsigned long expire = 0);
    /// Starts timer J.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerK(unsigned long expire = 0);
    /// Starts timer K.  
    ///
    /// The default interval is set from RFC 3261 recommended value if not specified explicitly.

  void startTimerMaxLifetime(unsigned long expire);
    /// Starts the max lifetime timer.
    /// 
    /// This timer makes sure that the transaction doesnt end up staying forever.
    /// This function will most likely call terminate() function

  void cancelTimerA();
    /// Cancels Timer A execution.

  void cancelTimerB();
    /// Cancels Timer B execution.

  void cancelTimerC();
    /// Cancels Timer C execution.

  void cancelTimerD();
    /// Cancels Timer C execution.

  void cancelTimerE();
    /// Cancels Timer E execution.

  void cancelTimerF();
    /// Cancels Timer F execution.

  void cancelTimerG();
    /// Cancels Timer G execution.

  void cancelTimerH();
    /// Cancels Timer H execution.

  void cancelTimerI();
    /// Cancels Timer I execution.

  void cancelTimerJ();
    /// Cancels Timer J execution.

  void cancelTimerK();
    /// Cancels Timer K execution.

  void cancelAllTimers();
    /// Cancels all existing timers

  SIPFSMDispatch*& dispatch();
    /// Returns the FSM Dispatcher pointer

  SIPMessage::Ptr getRequest() const;
    /// Returns a pointer to the request

  void setRequest(const SIPMessage::Ptr& pRequest);

  virtual bool isCompleted() const = 0;
    /// Returns true if state is already completed or terminated

  virtual void handleDelayedTerminate();
    /// Callback for timers that requires termination of transactions

  TransactionType getType() const;
protected:
  TransactionType _type;
  SIPMessage::Ptr _pRequest;
  boost::asio::io_service& _ioService;
  SIPFSMDispatch* _pDispatch;
  SIPTransactionTimers _timerProps;
  boost::asio::deadline_timer _timerA;
  boost::asio::deadline_timer _timerB;
  boost::asio::deadline_timer _timerC;
  boost::asio::deadline_timer _timerD;
  boost::asio::deadline_timer _timerE;
  boost::asio::deadline_timer _timerF;
  boost::asio::deadline_timer _timerG;
  boost::asio::deadline_timer _timerH;
  boost::asio::deadline_timer _timerI;
  boost::asio::deadline_timer _timerJ;
  boost::asio::deadline_timer _timerK;
  boost::asio::deadline_timer _timerMaxLifetime;

  TimerCallback _timerAFunc;
  TimerCallback _timerBFunc;
  TimerCallback _timerCFunc;
  TimerCallback _timerDFunc;
  TimerCallback _timerEFunc;
  TimerCallback _timerFFunc;
  TimerCallback _timerGFunc;
  TimerCallback _timerHFunc;
  TimerCallback _timerIFunc;
  TimerCallback _timerJFunc;
  TimerCallback _timerKFunc;
  TimerCallback _timerMaxLifetimeFunc;

private:
  void handleTimerA(const boost::system::error_code& e);
    /// Handler for Timer A expiration

  void handleTimerB(const boost::system::error_code& e);
    /// Handler for Timer B expiration

  void handleTimerC(const boost::system::error_code& e);
    /// Handler for Timer C expiration

  void handleTimerD(const boost::system::error_code& e);
    /// Handler for Timer D expiration

  void handleTimerE(const boost::system::error_code& e);
    /// Handler for Timer E expiration

  void handleTimerF(const boost::system::error_code& e);
    /// Handler for Timer F expiration

  void handleTimerG(const boost::system::error_code& e);
    /// Handler for Timer G expiration

  void handleTimerH(const boost::system::error_code& e);
    /// Handler for Timer H expiration

  void handleTimerI(const boost::system::error_code& e);
    /// Handler for Timer I expiration

  void handleTimerJ(const boost::system::error_code& e);
    /// Handler for Timer J expiration

  void handleTimerK(const boost::system::error_code& e);
    /// Handler for Timer K expiration

  void handleTimerMaxLifetime(const boost::system::error_code& e);
    /// Handler for Timer MaxLifetime expiration

  friend class SIPTransaction;
  friend class SIPTransactionPool;
};

//
// Inlines
//

inline void SIPFsm::cancelTimerA()
{
  _timerA.cancel();
}

inline void SIPFsm::cancelTimerB()
{
  _timerB.cancel();
}

inline void SIPFsm::cancelTimerC()
{
  _timerC.cancel();
}

inline void SIPFsm::cancelTimerD()
{
  _timerD.cancel();
}

inline void SIPFsm::cancelTimerE()
{
  _timerE.cancel();
}

inline void SIPFsm::cancelTimerF()
{
  _timerF.cancel();
}

inline void SIPFsm::cancelTimerG()
{
  _timerG.cancel();
}

inline void SIPFsm::cancelTimerH()
{
  _timerH.cancel();
}

inline void SIPFsm::cancelTimerI()
{
  _timerI.cancel();
}

inline void SIPFsm::cancelTimerJ()
{
  _timerJ.cancel();
}

inline void SIPFsm::cancelTimerK()
{
  _timerK.cancel();
}

inline SIPTransactionTimers& SIPFsm::timerProps()
{
  return _timerProps;
}

inline SIPFSMDispatch*& SIPFsm::dispatch()
{
  return _pDispatch;
}

inline SIPMessage::Ptr SIPFsm::getRequest() const
{
  return _pRequest;
}

inline void SIPFsm::setRequest(const SIPMessage::Ptr& pRequest)
{
  _pRequest = pRequest;
}

inline SIPFsm::TransactionType SIPFsm::getType() const
{
  return _type;
}

} } // OSS::SIP
#endif // SIP_SIPFsm_INCLUDED



