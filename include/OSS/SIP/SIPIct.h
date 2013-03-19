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


#ifndef SIP_SIPIct_INCLUDED
#define SIP_SIPIct_INCLUDED


#include <boost/shared_ptr.hpp>

#include "OSS/Thread.h"
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPFsm.h"


namespace OSS {
namespace SIP {


class OSS_API SIPIct:
    public SIPFsm
  /// An implementation of SIP RFC 3261 Invite Client Transaction State Machine.
{
public:
  enum State
  {
    TRYING=1,
    PROCEEDING,
    COMPLETED
  };

  SIPIct(
    boost::asio::io_service& ioService,
    const SIPTransactionTimers& timerProps);
    /// Creates a new ICT object. 

  ~SIPIct();
    /// Destroys an ICT object.

  virtual void onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport);
    /// This method is called by the transaction when a 
    /// SIP message is received from the transport.
    ///
    /// Take note that this is called directly from the 
    /// transport proactor thread and should therefore
    /// not block and result to a transport sleep.

  virtual bool onSendMessage(SIPMessage::Ptr pMsg);
    /// This method is called by SIPTransaction::writeMessage 
    /// when a SIP message is needed to be sent to the transport 
    /// coming from the core layer. 

  void handleSendAck(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport);
    /// Sends an ACK for 3xx-6xx responses
    ///
    /// Take note that ACK for 2xx should be handled by the UA layer

  void handleRetransmitInvite();
    /// Callback function for Timer A expiration

  void handleInviteTimeout();
    /// Callback function for Timer B expiration

  void handleAlertingTimeout();
    /// Callback function for Alerting Timer expiration

  void handleDelayedTerminate();
    /// Callback function for Timer D expiration

  SIPIct::Ptr clone() const;

  bool isCompleted() const;

private: 
  unsigned long _timerAValue;
  OSS::mutex_critic_sec _ackMutex;
  SIPMessage::Ptr _pAck;

  friend class SIPTransaction;
  friend class SIPTransactionPool;
  friend class SIPFSMPool;
  friend class SIPIctPool;
};

//
// Inlines
//


} } // OSS::SIP
#endif // SIP_SIPIct_INCLUDED
