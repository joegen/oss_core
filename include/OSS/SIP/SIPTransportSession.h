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


#ifndef SIP_SIPTransportSession_INCLUDED
#define SIP_SIPTransportSession_INCLUDED

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>

#include "OSS/Net.h"
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPTransportRateLimitStrategy.h"

namespace OSS {
namespace SIP {


class SIPFSMDispatch;

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

  virtual void start(SIPFSMDispatch* pDispatch) = 0;
    /// Start the first asynchronous operation for the connection.

  virtual void stop() = 0;
    /// Stop all asynchronous operations associated with the connection.

  virtual void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData = 0) = 0;
    /// Handle completion of a read operation.

  virtual void handleWrite(const boost::system::error_code& e) = 0;
    /// Handle completion of a write operation.

  virtual void handleResolve(boost::asio::ip::tcp::resolver::iterator endPointIter) = 0;
    /// Handle completion of connect resolve query

  virtual void handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter) = 0;
    /// Handle completion of async connect

  virtual void handleHandshake(const boost::system::error_code& error) = 0;
    /// Handle a secure hand shake from remote endpoint

  virtual OSS::IPAddress getLocalAddress() const = 0;
    /// Returns the local address binding for this transport

  virtual OSS::IPAddress getRemoteAddress() const = 0;
    /// Returns the last read source address

  virtual void clientBind(const OSS::IPAddress& ip, unsigned short portBase, unsigned short portMax) = 0;
    /// Bind the local client

  virtual void clientConnect(const OSS::IPAddress& target) = 0;
    /// Connect to a remote host

  unsigned long getLastReadCount() const;

  bool isReliableTransport() const;
    /// Returns true if the transport is reliable such as TCP and TLS

  SIPFSMDispatch*& dispatch();
    /// Returns the FSM dispatch associated with a connection

  void setIdentifier(OSS::UInt64 identifier);
  OSS::UInt64 getIdentifier() const;

  static SIPTransportRateLimitStrategy& rateLimit();

  const std::string& getTransportScheme() const;

  const OSS::IPAddress& getConnectAddress() const;
  void setConnectAddress(const OSS::IPAddress& address);

  const std::string& getExternalAddress() const;
    /// Returns the external address to be used for signaling in case
    /// the server is deployed within a NAT

  void setExternalAddress(const std::string& externalAddress);
    /// Set set the external address
protected:
  static SIPTransportRateLimitStrategy _rateLimit;

  bool _isReliableTransport;
  SIPFSMDispatch* _pDispatch;
  unsigned long _bytesTransferred;
  unsigned long _bytesRead;
  OSS::UInt64 _identifier;
  std::string _transportScheme;
  OSS::IPAddress _connectAddress;
  std::string _externalAddress;
private:
    SIPTransportSession(const SIPTransportSession&);
    SIPTransportSession& operator = (const SIPTransportSession&);
};

//
// Inlines
//

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

inline const OSS::IPAddress& SIPTransportSession::getConnectAddress() const
{
  return _connectAddress;
}

inline void SIPTransportSession::setConnectAddress(const OSS::IPAddress& address)
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

} } // OSS::SIP
#endif // SIP_SIPTransportSession_INCLUDED

