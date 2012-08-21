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


#ifndef SIP_SIPTransaction_INCLUDED
#define SIP_SIPTransaction_INCLUDED


#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "OSS/Core.h"
#include "OSS/Thread.h"
#include "OSS/Net.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/SIPFsm.h"


namespace OSS {
namespace SIP {


class SIPTransportService;
class SIPTransactionPool;
class SIPFsm;

class OSS_API SIPTransaction: public boost::enable_shared_from_this<SIPTransaction>
	/// Base class for ICT, IST, NICT, NIST state machines
{
public:
  static const int TRN_STATE_CHILD = 0xFC;
  static const int TRN_STATE_IDLE = 0xFD;
  static const int TRN_STATE_ACK_PENDING = 0xFE;
  static const int TRN_STATE_TERMINATED = 0xFF;
  typedef boost::shared_ptr<SIPTransaction> Ptr;
  typedef boost::weak_ptr<SIPTransaction> WeakPtr;
  typedef boost::shared_ptr<OSS::Exception> Error;
  typedef boost::function<void(const SIPTransaction::Error&, const SIPMessage::Ptr&, const SIPTransportSession::Ptr&, const SIPTransaction::Ptr&)> Callback;
	typedef boost::function<void(const SIPMessage::Ptr&, const SIPTransportSession::Ptr&, const SIPTransaction::Ptr&)> RequestCallback;
  
  enum Type
  {
    TYPE_UNKNOWN,
    TYPE_ICT,
    TYPE_IST,
    TYPE_NICT,
    TYPE_NIST
  };
  
  SIPTransaction();
		/// Creates the SIPTransaction.

  ~SIPTransaction();
		/// Destroys the SIPTransaction, cancelling all pending tasks.
	
  void onReceivedMessage(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport);
    /// This method is called when a SIP message is received from the transport.
    ///
    /// The SIP message will simply be passed to the FSM 
    /// where the transaction state will be processed.
    ///
    /// Take note that this is called directly from the 
    /// transport proactor thread and should therefore
    /// not block and result to a transport sleep.

  void informTU(SIPMessage::Ptr pMsg, SIPTransportSession::Ptr pTransport);
    /// Propagate the SIP message to the upper layer
    ///
    /// Take note that this is called as a direct result
    /// of onReceivedMessage and should therefore likewise
    /// not block and result to a transport sleep.

  void sendRequest(
    const SIPMessage::Ptr& pRequest,
    const OSS::IPAddress& localAddress,
    const OSS::IPAddress& remoteAddress,
    SIPTransaction::Callback callback);
    /// Send a new SIP (REQUEST) message to the Network.
    ///
    /// This is a none-blocking function call for sending
    /// SIP (REQUEST) messages to the network.  The local interface
    /// to be used must be always specified to add extra
    /// flexibility for applications that bridges multi-homed
    /// networks with complex routing rules.
    ///
    /// The remote address must be in the form of an IP address.
    /// OSSSIP supports both IPV4 and IPV6 destinations.
    /// DNS lookup will not be performed by the transport layer.
    /// Thus, this method expects that the remote address has 
    /// already been resolved using the mechanisms exposed by
    /// OSSADNS or a third party DNS client.
    ///
    /// This function call is a none blocking call.  All responses
    /// will be sent back through the SIPTransaction::Callback function.
    /// If an error occured, the callback function will receive
    /// the SIPException as the first parameter and must always be checked
    /// prior to processing of the rest of the callback parameters.
    /// Normal cause of errors are transaction timeouts.
    ///
    /// This is called directly by the FSM dispatch upon creation of a new 
    /// Client transaction

    void sendResponse(
      const SIPMessage::Ptr& pRequest,
      const OSS::IPAddress& sendAddress);
    /// Send a new SIP (RESPONSE) message to the Network.
    ///
    /// This is a none-blocking function call for sending
    /// SIP (RESPONSE) messages to the network.  
    ///
    /// For UDP, The remote address must be in the form of an IP address.
    /// OSSSIP supports both IPV4 and IPV6 destinations.
    /// DNS lookup will not be performed by the transport layer.
    /// Thus, this method expects that the remote address has 
    /// already been resolved using the mechanisms exposed by
    /// OSSADNS or a third party DNS client if required.
    ///
    /// Applications may also opt to send using the remoteAddress
    /// which is directly accessible via the transaction if the
    /// applicaiton is sure that the remote endpoint expects the
    /// responses to be received using the same transport it was sent.
    ///
    /// Take note that via processing is not handled in the transaction layer.
    /// This is an intentional behavior to allow the application layer
    /// to perform proprietary NAT traversal techniques.
    ///
    /// The send address MAY be set to anything if the transport is reliable like TCP or TLS
    /// since it will be ignored.  TCP and TLS will always use the same transport used
    /// to send the server transaction request.

  void sendResponse(
      const SIPMessage::Ptr& pRequest,
      const OSS::IPAddress& sendAddress,
      SIPTransaction::Callback callback);
    /// Send a new SIP ACKable (RESPONSE) message to the Network.
    ///
    /// This is a none-blocking function call for sending
    /// SIP (RESPONSE) messages to the network.  
    ///
    /// For UDP, The remote address must be in the form of an IP address.
    /// OSSSIP supports both IPV4 and IPV6 destinations.
    /// DNS lookup will not be performed by the transport layer.
    /// Thus, this method expects that the remote address has 
    /// already been resolved using the mechanisms exposed by
    /// OSSADNS or a third party DNS client if required.
    ///
    /// Applications may also opt to send using the remoteAddress
    /// which is directly accessible via the transaction if the
    /// applicaiton is sure that the remote endpoint expects the
    /// responses to be received using the same transport it was sent.
    ///
    /// Take note that via processing is not handled in the transaction layer.
    /// This is an intentional behavior to allow the application layer
    /// to perform proprietary NAT traversal techniques.
    ///
    /// The send address MAY be set to anything if the transport is reliable like TCP or TLS
    /// since it will be ignored.  TCP and TLS will always use the same transport used
    /// to send the server transaction request.

  void sendAckFor2xx(
    const SIPMessage::Ptr& pAck,
    const OSS::IPAddress& dialogTarget);
    /// Send a new SIP (ACK REQUEST) message to the Network.
    ///
    /// This method is used by ICT to ACK 2xx responses

  void writeMessage(SIPMessage::Ptr pMsg);
    /// Send a message using the transaction transport.
    ///
    /// Do not call this directly.  Instead, use sendRequest() or sendResponse()

  void writeMessage(SIPMessage::Ptr pMsg, const OSS::IPAddress& remoteAddress);
    /// Send a message using the transaction transport.
    ///
    /// Do not call this directly.  Instead, use sendRequest() or sendResponse()


  SIPTransactionPool*& owner();
    /// Returns a reference to the transaction pool pointer

  SIPFsm::Ptr& fsm();
    /// Returns a reference to the fsm pointer

  void terminate();
    /// Puts the transaction in the terminated state
    /// and calls SIPTransactionPool::removeTransaction()
    ///
    /// Since all references to the SIP transaction are
    /// in the form of auto pointers, this will not invalidate
    /// the pointer if a thread still shares ownership of the
    /// transaction pointer.

  void setState(int state);
    /// Set the transaction state value.
    ///
    /// If the state is already set to TRN_STATE_TERMINATED
    /// and a new call to setState() is called,
    /// a SIPInvalidStateException will be thrown.

  int getState() const;
    /// Returns the transaction state value
    ///
    /// Take note that this is not thread safe since
    /// the internal state of the transaction may change
    /// after this function has returned.
    /// 
    /// This is with the exception of checking against 
    /// TRN_STATE_TERMINATED which could never change state again
    /// and thus guaranty the correctness of the state atomically.

  void setId(const std::string &id);
    /// Set the transaction id 
    ///
    /// transaction-id = method  cseq  (via-branch / callid)

  const std::string& getId() const;
    /// Returns the transaction-Id
    ///
    /// transaction-id = method  cseq  (via-branch / callid)

  void handleTimeoutICT();
    /// Called by the FSM when an outgoing invite times out.

  void handleTimeoutNICT();
    /// Called by the FSM when an outgoing non-invite request times out.

	SIPTransportSession::Ptr& transport();
    /// Returns a pointer to the transport

  OSS::IPAddress& localAddress();
    /// Returns the local address used by this transaction
    ///
    /// This is either set during the initial sendRequest or 
    /// when a request is received from the transport

  OSS::IPAddress& remoteAddress();
    /// Returns the remote address used by this transaction
    ///
    /// This is either set during the initial sendRequest or 
    /// when a request is received from the transport

  OSS::IPAddress& sendAddress();
    /// Returns the remote address where messages will be sent.
    ///
    /// The default behavior of transactions is to use the 
    /// _remoteAddress value as the destinaton of responses.
    /// However, the applicaiton layer is given to override 
    /// this by setting the _sendAddress

  OSS::IPAddress& dialogTarget();
    /// Returns a direct reference where ACK requests for 2xx will be sent


  SIPTransportService*& transportService();
    /// Returns a reference to the transport service

  SIPTransaction::Type& type();
    /// Returns the type of transaction (ICT, IST, NICT, NIST)

  bool& willSendAckFor2xx();
    /// Returns a direct reference to the flag indicating
    /// whether INVITE trasnactions would terminate after
    /// sending or receipt of a 2xx response.
    ///
    /// UA implementation would usually set this to false
    /// so that retransmissions of the 2xx response will still
    /// be caught by the transaction.

  SIPTransaction::Callback& ackHandler();
    /// Handler for ACK request for IST.
    ///

  const std::string& getLogId() const;
    /// Return the transaction logid

  void setLogId(const std::string& logId);
    /// Set the transaction log id

  bool isXOREncrypted() const;
    /// Returns true if this transaction is XOR encrypted

  SIPTransaction::Ptr createChildTransactionFromResponse(const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport, bool mayExist);
  SIPTransaction::Ptr createChildTransactionFromResponse(const std::string& id, bool mayExist);
    /// Creates a new child transaction due to a forked response.
    /// If the transaction already exists, the existing pointer will be returned.
    /// A new transaction will be created if one does not exist

  SIPTransaction::Ptr findChildTransactionFromResponse(const SIPMessage::Ptr& pMsg);
  SIPTransaction::Ptr findChildTransaction(const std::string& id);
    /// Returns the pointer to a child transaction.
    /// Null will be returned if the transaction does not exists

  void setRemoteTag(const std::string& tag);
    /// Set the remote tag for early dialogs.

  const std::string& getRemoteTag();
    /// Get the remote tag for early dialogs

  bool isChildTransaction() const;


protected:
  SIPTransaction::Callback _responseTU;
  SIPTransaction::Callback _ackHandler;
  Type _type;
private:
	SIPTransaction(const SIPTransaction&);
	SIPTransaction& operator = (const SIPTransaction&);
  std::string _id;
	SIPTransactionPool* _owner;
  SIPFsm::Ptr _fsm;
  SIPTransportSession::Ptr _transport;
  SIPTransportService* _transportService;
  int _state;
  OSS::IPAddress _localAddress;
  OSS::IPAddress _remoteAddress;
  OSS::IPAddress _sendAddress;
  OSS::IPAddress _dialogTarget; 
  bool _willSendAckFor2xx;
  mutable OSS::mutex _mutex;
  mutable OSS::mutex_read_write _stateMutex;
  std::string _logId;
  bool _isXOREncrypted;
  bool _isInitialRequest;
  SIPTransaction::WeakPtr _parent;
  std::map<std::string, SIPTransaction::Ptr> _children;
  std::string _remoteTag;
  bool _isChildTransaction;
	friend class SIPTransactionPool;
};


//
// Inlines
//
inline void SIPTransaction::setId(const std::string &id)
{
  _id = id;
}

inline const std::string& SIPTransaction::getId() const
{
  return _id;
}

inline SIPFsm::Ptr& SIPTransaction::fsm()
{
  return _fsm;
}

inline SIPTransportSession::Ptr& SIPTransaction::transport()
{
  return _transport;
}

inline OSS::IPAddress& SIPTransaction::localAddress()
{
  return _localAddress;
}

inline OSS::IPAddress& SIPTransaction::remoteAddress()
{
  return _remoteAddress;
}

inline OSS::IPAddress& SIPTransaction::sendAddress()
{
  return _sendAddress;
}

inline SIPTransportService*& SIPTransaction::transportService()
{
  return _transportService;
}

inline SIPTransaction::Type& SIPTransaction::type()
{
  return _type;
}

inline bool& SIPTransaction::willSendAckFor2xx()
{
  return _willSendAckFor2xx;
}

inline OSS::IPAddress& SIPTransaction::dialogTarget()
{
  return _dialogTarget;
}

inline SIPTransaction::Callback& SIPTransaction::ackHandler()
{
  return _ackHandler;
}

inline const std::string& SIPTransaction::getLogId() const
{
  return _logId;
}

inline void SIPTransaction::setLogId(const std::string& logId)
{
  _logId = logId;
}

inline bool SIPTransaction::isXOREncrypted() const
{
  return _isXOREncrypted;
}

inline void SIPTransaction::setRemoteTag(const std::string& tag)
{
  _remoteTag = tag;
}

inline const std::string& SIPTransaction::getRemoteTag()
{
  return _remoteTag;
}

inline bool SIPTransaction::isChildTransaction() const
{
  return _isChildTransaction;
}

} } // namespace OSS::SIP


#endif // SIP_SIPTransaction_INCLUDED
