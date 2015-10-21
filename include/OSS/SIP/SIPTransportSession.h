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


#ifndef SIP_SIPTransportSession_INCLUDED
#define SIP_SIPTransportSession_INCLUDED

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>

#include "OSS/Net/Net.h"
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/Net/AccessControl.h"

namespace OSS {
namespace SIP {


class SIPFSMDispatch;
class SIPTransactionPool;

class OSS_API SIPTransportSession
  /// The SIPTransportSession is the base class to the connection 
  /// that received the message.  This will be used by the 
  /// transaction to send mid-transaction SIP Messages
  /// ensuring that a transaction always uses the same
  /// socket for all requests.
  ///
  /// The transport session propagates up to the core layer
  /// to allow the core to learn about the transport properties
  /// of SIP messages.
{
public:
  typedef boost::shared_ptr<SIPTransportSession> Ptr;
  typedef OSS::Net::AccessControl SIPTransportRateLimitStrategy;
  typedef boost::function<void(SIPMessage::Ptr, SIPTransportSession::Ptr)> Dispatch;

  SIPTransportSession();
    /// Creates a new SIPTransportSession

  virtual ~SIPTransportSession();
    /// Destroys a transport session

  virtual void writeMessage(SIPMessage::Ptr msg) = 0;
    /// Send a SIP message using this session.

  virtual void writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port) = 0;
    /// Send a SIP message using this session.  This is used by the UDP tranport

  virtual bool writeKeepAlive();
    /// Send a connection specific keep-alive
    /// This is normally invoked by the application layer to
    /// keep NAT port bindings open as well as to poke
    /// reliability of the transport for stream based connections.
    /// The default behavior sends nothing

  virtual bool writeKeepAlive(const std::string& ip, const std::string& port);
    /// Send a connection specific keep-alive
    /// This is normally invoked by the application layer to
    /// keep NAT port bindings open as well as to poke
    /// reliability of the transport for stream based connections.
    /// The default packet is CRLF/CRLF

  virtual void start(const SIPTransportSession::Dispatch& dispatch) = 0;
    /// Start the first asynchronous operation for the connection.

  virtual void stop() = 0;
    /// Stop all asynchronous operations associated with the connection.

  virtual void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData = 0) = 0;
    /// Handle completion of a read operation.

  virtual void handleWrite(const boost::system::error_code& e) = 0;
    /// Handle completion of a write operation.


  virtual void handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter) = 0;
    /// Handle completion of async connect

  virtual void handleClientHandshake(const boost::system::error_code& error) = 0;
    /// Handle a secure hand shake from remote endpoint

  virtual void handleServerHandshake(const boost::system::error_code& error) = 0;
    /// Handle a secure handshake from remote endpoint
  virtual OSS::Net::IPAddress getLocalAddress() const = 0;
    /// Returns the local address binding for this transport

  virtual OSS::Net::IPAddress getRemoteAddress() const = 0;
    /// Returns the last read source address

  virtual void clientBind(const OSS::Net::IPAddress& ip, unsigned short portBase, unsigned short portMax) = 0;
    /// Bind the local client

  virtual bool clientConnect(const OSS::Net::IPAddress& target) = 0;
    /// Connect to a remote host
  
  bool isConnected() const;
    /// Returns true if the socket is connected
  
  void setConnected(bool connected);
    /// Set the connected flag

  unsigned long getLastReadCount() const;

  bool isReliableTransport() const;
    /// Returns true if the transport is reliable such as TCP and TLS

  SIPFSMDispatch*& dispatch();
    /// Returns the FSM dispatch associated with a connection

  void setIdentifier(OSS::UInt64 identifier);
  OSS::UInt64 getIdentifier() const;

  static SIPTransportRateLimitStrategy& rateLimit();

  const std::string& getTransportScheme() const;

  const OSS::Net::IPAddress& getConnectAddress() const;
  void setConnectAddress(const OSS::Net::IPAddress& address);

  const std::string& getExternalAddress() const;
    /// Returns the external address to be used for signaling in case
    /// the server is deployed within a NAT

  void setExternalAddress(const std::string& externalAddress);
    /// Set set the external address
  
  void setMessageDispatch(const Dispatch& dispatch);
  
  void dispatchMessage(const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport);
  
  bool& isClient();
  const bool& isClient() const;
  
  void setReconnectAddress(const OSS::Net::IPAddress& reconnectAddress);
    /// Set the reconnect address if a server connection is lost.
    ///
  
  const OSS::Net::IPAddress& getReconnectAddress() const;
    /// Return the reconnect address
    /// 
  
  bool isEndpoint() const;
    /// Returns rue if this is an Endpoint connection
    ///
  
  const std::string& getEndpointName() const;
    /// Returns the name of the endpoint for this connection.
    /// This would be empty for non-endpoint connections
    ///
  
  void setCurrentTransactionId(const std::string& currentTransactionId);
    /// Set the current id of the transaciton using this transport.
    /// This is currently used by reliable connections to report connection
    /// errors to the transaction


  const std::string& getCurrentTransactionId() const;
    /// Return the transaction id
  
  void setTransactionPool(SIPTransactionPool* pTransactionPool);
  
protected:
  static SIPTransportRateLimitStrategy _rateLimit;

  bool _isReliableTransport;
  SIPFSMDispatch* _pDispatch;
  unsigned long _bytesTransferred;
  unsigned long _bytesRead;
  OSS::UInt64 _identifier;
  std::string _transportScheme;
  OSS::Net::IPAddress _connectAddress;
  std::string _externalAddress;
  Dispatch _messageDispatch;
  bool _isClient;
  OSS::Net::IPAddress _reconnectAddress;
  bool _isEndpoint;
  std::string _endpointName;
  std::string _currentTransactionId;
  SIPTransactionPool* _pTransactionPool;
  bool _isConnected;
private:
    SIPTransportSession(const SIPTransportSession&);
    SIPTransportSession& operator = (const SIPTransportSession&);
};

//
// Inlines
//

inline bool SIPTransportSession::isEndpoint() const
{
  return _isEndpoint;
}

inline const std::string& SIPTransportSession::getEndpointName() const
{
  return _endpointName;
}

inline bool SIPTransportSession::isReliableTransport() const
{
  return _isReliableTransport;
}

inline SIPFSMDispatch*& SIPTransportSession::dispatch()
{
  return _pDispatch;
}

inline unsigned long SIPTransportSession::getLastReadCount() const
{
  return _bytesRead;
}

inline bool SIPTransportSession::writeKeepAlive()
{
  return false;
}

inline bool SIPTransportSession::writeKeepAlive(const std::string& ip, const std::string& port)
{
  return false;
}

inline void SIPTransportSession::setIdentifier(OSS::UInt64 identifier)
{
  _identifier = identifier;
}

inline OSS::UInt64 SIPTransportSession::getIdentifier() const
{
  return _identifier;
}

inline const std::string& SIPTransportSession::getTransportScheme() const
{
  OSS_VERIFY(!_transportScheme.empty());
  return _transportScheme;
}

inline const OSS::Net::IPAddress& SIPTransportSession::getConnectAddress() const
{
  return _connectAddress;
}

inline void SIPTransportSession::setConnectAddress(const OSS::Net::IPAddress& address)
{
  _connectAddress = address;
}

inline const std::string& SIPTransportSession::getExternalAddress() const
{
  return _externalAddress;
}

inline void SIPTransportSession::setExternalAddress(const std::string& externalAddress)
{
  _externalAddress = externalAddress;
}

inline void SIPTransportSession::setMessageDispatch(const Dispatch& dispatch)
{
  _messageDispatch = dispatch;
}
  
inline void SIPTransportSession::dispatchMessage(const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport)
{
  if (_messageDispatch)
  {
    _messageDispatch(pMsg, pTransport);
  }
}

inline bool& SIPTransportSession::isClient()
{
  return _isClient;
}

inline const bool& SIPTransportSession::isClient() const
{
  return _isClient;
}

inline void SIPTransportSession::setReconnectAddress(const OSS::Net::IPAddress& reconnectAddress)
{
  _reconnectAddress = reconnectAddress;
}
  
inline const OSS::Net::IPAddress& SIPTransportSession::getReconnectAddress() const
{
  return _reconnectAddress;
}

inline void SIPTransportSession::setCurrentTransactionId(const std::string& currentTransactionId)
{
  _currentTransactionId = currentTransactionId;
}

inline const std::string& SIPTransportSession::getCurrentTransactionId() const
{
  return _currentTransactionId;
}

inline void SIPTransportSession::setTransactionPool(SIPTransactionPool* pTransactionPool)
{
  _pTransactionPool = pTransactionPool;
}

inline bool SIPTransportSession::isConnected() const
{
  return _isConnected;
}
 
inline void SIPTransportSession::setConnected(bool connected)
{
  _isConnected = connected;
}

} } // OSS::SIP




#endif // SIP_SIPTransportSession_INCLUDED

