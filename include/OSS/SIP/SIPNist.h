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


#ifndef SIP_SIPNist_INCLUDED
#define SIP_SIPNist_INCLUDED


#include <boost/shared_ptr.hpp>

#include "OSS/Thread.h"
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPFsm.h"


namespace OSS {
namespace SIP {


class OSS_API SIPNist:
    public SIPFsm
  /// An implementation of SIP RFC 3261 Invite Server Transaction State Machine.
{
public:
  enum State
  {
    TRYING=1,
    PROCEEDING,
    COMPLETED
  };

  SIPNist(
    boost::asio::io_service& ioService,
    const SIPTransactionTimers& timerProps);
    /// Creates a new ICT object. 

  ~SIPNist();
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

  virtual bool isCompleted() const;
private: 
  SIPMessage::Ptr _pResponse;
  OSS::mutex_critic_sec _responseMutex;
  friend class SIPTransaction;
  friend class SIPTransactionPool;
  friend class SIPFSMPool;
  friend class SIPNistPool;
};

//
// Inlines
//


} } // OSS::SIP
#endif // SIP_SIPNist_INCLUDED
