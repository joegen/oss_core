// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSS Core SDK.
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


#ifndef SIP_OSSSIPB2BTransaction_INCLUDED
#define SIP_OSSSIPB2BTransaction_INCLUDED


#include <queue>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "OSS/DNS.h"
#include "OSS/SIP/B2BUA/B2BUA.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"


namespace OSS {
namespace SIP {

//
// Base Exception
//
OSS_CREATE_INLINE_EXCEPTION(B2BUABaseException, OSS::IOException, "B2BUA Exception")

//
// Configuration related exceptions
//
OSS_CREATE_INLINE_EXCEPTION(B2BUAConfigException, B2BUABaseException, "B2BUA Configuration Exception")

//
// Script related exceptions
//
OSS_CREATE_INLINE_EXCEPTION(B2BUAScriptException, B2BUABaseException, "B2BUA Script Exception")

//
// State related exceptions
//
OSS_CREATE_INLINE_EXCEPTION(B2BUAStateException, B2BUABaseException, "B2BUA State Exception")

class SIPB2BTransactionManager;

class OSS_API SIPB2BTransaction : private boost::noncopyable, public boost::enable_shared_from_this<SIPB2BTransaction>
  /// Base class for SIP Call implementation
{
public:
  typedef boost::shared_ptr<SIPB2BTransaction> Ptr;

  explicit SIPB2BTransaction(SIPB2BTransactionManager* pManager);
    /// Creates a new SIPB2BTransaction object

  virtual ~SIPB2BTransaction();
    /// Destroys the SIPB2BTransaction object

  void handleResponse(
    const OSS::SIP::SIPTransaction::Error& e, 
    const OSS::SIP::SIPMessage::Ptr& pMsg, 
    const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);
    /// The transaction response handler

  virtual void runTask();
    /// Execute the transaction tasks
    ///
    /// This method runs in its own thread and will not block any operation
    /// in the subsystem.  It is therefore safe to call blocking functions
    /// in this method.
    ///

  virtual void runResponseTask();
    /// Execute the transaction tasks for handling responses
    ///
    /// This method runs in its own thread and will not block any operation
    /// in the subsystem.  It is therefore safe to call blocking functions
    /// in this method.
    ///
  
  virtual void runFailoverTask();
    /// Execute the transaction tasks for handling DNS/SRV failover
    ///
    /// This method runs in its own thread and will not block any operation
    /// in the subsystem.  It is therefore safe to call blocking functions
    /// in this method.
    ///

  bool isFailoverCandidate(
    const OSS::SIP::SIPTransaction::Error& e,
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);
    /// This method is called from handleResponse() to check if a DNS/SRV
    /// failover is applicable for the response type.  The standard behavior
    /// is to failover on 5xx response or a local exception
    /// has occured such as a transaction timeout

  bool onRouteResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest, 
    const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
    const OSS::SIP::SIPTransaction::Ptr& pTransaction,
    OSS::IPAddress& target);
    /// Determines the target address to be used for sending responses to a particular request.
    ///
    /// The result will be cached by the transaction and would be returned as the result
    /// for future calls to this method

  void setResponseTarget(const OSS::IPAddress& target);
    /// Set the target address for all responses.

  SIPMessage::Ptr& serverRequest();
    /// Returns a direct reference to the server request
    /// that created the transaction

  SIPTransportSession::Ptr& serverTransport();
    /// Returns a direct reference to the transport
    /// that initially created the transaction

  SIPTransaction::Ptr& serverTransaction();
    /// Returns the server transaction

  SIPMessage::Ptr& clientRequest();
    /// Returns the most recent client request sent by the transaciton

  SIPTransportSession::Ptr& clientTransport();
    /// Returns the most recent clietn transport used by the transaction

  SIPTransaction::Ptr& clientTransaction();
    /// Returns the most recent client transaction

  SIPB2BTransactionManager* manager() const;
    /// Return a raw pointer to the manager

    void setProperty(const std::string& property, const std::string& value);
    /// Set a custom property for this transaction.
    /// Custom properties are meant to simply hold
    /// arbitrary data to aid in how the transactions
    /// are processed.

  bool getProperty(const std::string&  property, std::string& value) const;
    /// Get a custom property of this transaction.
    /// Custom properties are meant to simply hold
    /// arbitrary data to aid in how the transactions
    /// are processed.

  bool hasProperty(const std::string& property) const;

  const std::string& getLogId() const;
    /// Return the log-id used for logging

  bool hasSentLocalResponse() const;
    /// returns true if final response has been sent locally

  bool isMidDialog() const;

  bool resolveSessionTarget(SIPMessage::Ptr& pClientRequest, OSS::IPAddress& initialTarget);

protected:
  SIPMessage::Ptr _pServerRequest;
  SIPTransportSession::Ptr _pServerTransport; 
  SIPTransaction::Ptr _pServerTransaction;

  SIPMessage::Ptr _pClientRequest;
  SIPTransportSession::Ptr _pClientTransport; 
  SIPTransaction::Ptr _pClientTransaction;

  SIPB2BTransactionManager* _pManager;
  OSS::SIP::SIPTransaction::Error _pTransactionError;

  Ptr* _pInternalPtr;
  OSS::mutex_critic_sec _responseQueueMutex;
  std::queue<SIPMessage::Ptr> _responseQueue;
  OSS::mutex_critic_sec _responseTargetMutex;
  OSS::IPAddress _responseTarget;

  typedef boost::shared_lock<boost::shared_mutex> ReadLock;
  typedef boost::lock_guard<boost::shared_mutex> WriteLock;
  mutable boost::shared_mutex _rwlock;
  OSS::mutex _resposeMutex;
  typedef std::map<std::string, std::string> CustomProperties;
  CustomProperties _properties;
  std::string _logId;
  bool _hasSentLocalResponse;
  bool _isMidDialog;
  void releaseInternalRef();
    /// release the internal reference and signal transaction destruction

  OSS::dns_srv_record_list _udpSrvTargets;
  OSS::dns_srv_record_list _tcpSrvTargets;
  OSS::dns_srv_record_list _tlsSrvTargets;
  unsigned int _failoverCount;
  OSS::IPAddress _localInterface;
  friend class SIPB2BTransactionManager;
};


//
// Inlines
//

inline void SIPB2BTransaction::setResponseTarget(const OSS::IPAddress& target)
{
  OSS::mutex_critic_sec_lock lock(_responseTargetMutex);
  _responseTarget = target;
}

inline SIPMessage::Ptr& SIPB2BTransaction::serverRequest()
{
  return _pServerRequest;
}

inline SIPTransportSession::Ptr& SIPB2BTransaction::serverTransport()
{
  return _pServerTransport;
}

inline SIPTransaction::Ptr& SIPB2BTransaction::serverTransaction()
{
  return _pServerTransaction;
}

inline SIPMessage::Ptr& SIPB2BTransaction::clientRequest()
{
  return _pClientRequest;
}

inline SIPTransportSession::Ptr& SIPB2BTransaction::clientTransport()
{
  return _pClientTransport;
}

inline SIPTransaction::Ptr& SIPB2BTransaction::clientTransaction()
{
  return _pClientTransaction;
}

inline SIPB2BTransactionManager* SIPB2BTransaction::manager() const
{
  return _pManager;
}

inline const std::string& SIPB2BTransaction::getLogId() const
{
  return _logId;
}

inline bool SIPB2BTransaction::hasSentLocalResponse() const
{
  return _hasSentLocalResponse;
}

inline bool SIPB2BTransaction::isMidDialog() const
{
    return _isMidDialog;
}

} } // OSS::SIP

#endif

