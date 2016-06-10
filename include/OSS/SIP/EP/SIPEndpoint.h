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

#ifndef OSS_SIPENDPOINT_H_INCLUDED
#define	OSS_SIPENDPOINT_H_INCLUDED



#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPStack.h"
#include "OSS/SIP/EP/SIPEndpointRetransmitter.h"


#define TCP_PORT_BASE 20000
#define TCP_PORT_MAX  30000
#define DEFAULT_SIP_PORT 5060
#define DEFAULT_SIP_TLS_PORT 5061
#define DEFAULT_SIP_WS_PORT 5062


namespace OSS {
namespace SIP {
namespace EP {
  
  
  
  
class SIPEndpoint : boost::noncopyable
{
public:
  
  enum EventType
  {
    IncomingRequest,
    IncomingResponse,
    IncomingAckFor2xx,
    Incoming2xxRetran,
    TransactionError,
    TransactionTermination,
    AckTimeout,
    EndpointTerminated
  };
  
  struct EndpointEvent
  {
    EventType eventType;
    SIPMessage::Ptr sipRequest; 
    SIPTransportSession::Ptr transportSession;
    SIPTransaction::Ptr transaction;
    SIPTransaction::Error transactionError;
  };
  
  typedef boost::shared_ptr<EndpointEvent> EndpointEventPtr;
  typedef BlockingQueue<EndpointEventPtr> EndpointEventQueue;
  typedef std::map<std::string, SIPEndpointRetransmitter::Ptr> RetransmitCache;
          
  
  SIPEndpoint();
  
  ~SIPEndpoint();
        
  bool addTransport(const OSS::Net::IPAddress& address);
  /// Add transport to the endpoint.  This must be done
  /// prior to calling run();)
  ///  
  
  virtual bool runEndpoint();
  /// Initialize the endpoint and start processing incoming SIP messages.
  /// This function will return right away.  If it returns false,
  /// something went wrong with initialization.  Consult logs.
  ///
  
  virtual void stopEndpoint();
  // Stop processing incoming messages and deinitialize the endpoint.
  
  void sendEndpointRequest(
    const SIPMessage::Ptr& pRequest,
    const OSS::Net::IPAddress& localAddress,
    const OSS::Net::IPAddress& remoteAddress);
  /// Send a SIP request to the specified destination
  
  void sendEndpointResponse(
    const SIPMessage::Ptr& pResponse,
    const SIPTransaction::Ptr& pTransaction,
    const OSS::Net::IPAddress& remoteAddress);
  /// Send a SIP response through this transaction

  
  void handleEndpointResponse(const SIPTransaction::Error& e, const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport, const SIPTransaction::Ptr& pTransaction);
  /// Handle an incoming response.

  void onTransactionTerminated(const SIPTransaction::Ptr& pTransaction);
  /// Callback for transaction termination
  
  void onHandleAckTimeout(const SIPMessage::Ptr& pRequest);
  /// 200 Ok retransmit timeout handler 
  
  
  SIPStack& stack();
    /// Return a reference to the SIP stack
  
  const std::string& getUserAgentName() const;
    /// Returns the user agent name to be used by the B2BUA if set

  void setUserAgentName(const std::string& userAgentName);
    /// Set the user-agent name.
  
  bool getExternalAddress(const OSS::Net::IPAddress& internalIp, std::string& externalIp) const;
    /// Return the assigned external address for a particular transport.
    /// This is normally used in relation to messages that has to passthrough
    /// a firewall.

  bool getExternalAddress(const std::string& proto, const OSS::Net::IPAddress& internalIp, std::string& externalIp) const;
    /// Return the assigned external address for a particular transport.
    /// This is normally used in relation to messages that has to passthrough
    /// a firewall.

  bool getInternalAddress(
    const OSS::Net::IPAddress& externalIp,
    OSS::Net::IPAddress& internalIp) const;
    /// Return the internal IP if the host:port for the external IP is known

  bool getInternalAddress(
    const std::string& proto,
    const OSS::Net::IPAddress& externalIp,
    OSS::Net::IPAddress& internalIp) const;
    /// Return the internal IP if the host:port for the external IP is known
  
  void receiveEndpointEvent(EndpointEventPtr& ev);
    /// receive a new event
  
  int getFd() const;
    /// return fd for polling read status of the event queue
  
  OSS::SIP::SIPTransaction::Ptr createClientTransaction(const OSS::SIP::SIPMessage::Ptr& pMsg);
    /// Create a new SIP client transaction for the SIP request
  
protected:
  virtual void handleRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg, 
    const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);
    /// This is the incoming request callback that will be attached to the stack

  virtual void handleAckOr2xxTransaction(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport);
    /// Handler of ACK and 200 Ok retransmission
  
protected:
  SIPStack _stack;
  std::string _userAgentName;
  EndpointEventQueue _endpointEventQueue;
  RetransmitCache _2xxRetransmitCache;
  OSS::mutex_critic_sec _2xxRetransmitCacheMutex;
};

//
// Inlines
//
inline SIPStack& SIPEndpoint::stack()
{
  return _stack;
}

inline const std::string& SIPEndpoint::getUserAgentName() const
{
  return _userAgentName;
}

inline void SIPEndpoint::setUserAgentName(const std::string& userAgentName)
{
  _userAgentName = userAgentName;
}

inline bool SIPEndpoint::getExternalAddress(
    const OSS::Net::IPAddress& internalIp,
    std::string& externalIp) const
{
  return const_cast<SIPTransportService&>(const_cast<SIPStack&>(_stack).transport()).getExternalAddress(internalIp, externalIp);
}

inline bool SIPEndpoint::getExternalAddress(
  const std::string& proto,
  const OSS::Net::IPAddress& internalIp,
  std::string& externalIp) const
{
  return const_cast<SIPTransportService&>(const_cast<SIPStack&>(_stack).transport()).getExternalAddress(proto, internalIp, externalIp);
}

inline bool SIPEndpoint::getInternalAddress(
  const OSS::Net::IPAddress& externalIp,
  OSS::Net::IPAddress& internalIp) const
{
  return const_cast<SIPTransportService&>(const_cast<SIPStack&>(_stack).transport()).getInternalAddress(
   externalIp, internalIp);
}

inline bool SIPEndpoint::getInternalAddress(
  const std::string& proto,
  const OSS::Net::IPAddress& externalIp,
  OSS::Net::IPAddress& internalIp) const
{
  return const_cast<SIPTransportService&>(const_cast<SIPStack&>(_stack).transport()).getInternalAddress(
    proto, externalIp, internalIp);
}

inline int SIPEndpoint::getFd() const
{
  return _endpointEventQueue.getFd();
}

} } } // OSS::SIP::EP

#endif // OSS_SIPENDPOINT_H_INCLUDED