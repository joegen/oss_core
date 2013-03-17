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


#ifndef SIP_SIPTCPConnection_INCLUDED
#define SIP_SIPTCPConnection_INCLUDED


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPTransportSession.h"


namespace OSS {
namespace SIP {


class SIPTCPConnectionManager;
class SIPTCPConnection;
class SIPFSMDispatch;

class OSS_API SIPTCPConnection: 
  public SIPTransportSession,
  public boost::enable_shared_from_this<SIPTCPConnection>,
  private boost::noncopyable
{
public:

  typedef boost::asio::ip::tcp::socket::endpoint_type EndPoint;
  explicit SIPTCPConnection(
      boost::asio::io_service& ioService,
      SIPTCPConnectionManager& manager);
    /// Creates a TCP connection using the given I/O service

  ~SIPTCPConnection();
    /// Destroys the TCP connection

  boost::asio::ip::tcp::socket& socket();
    /// Get the socket associated with the connection.

  void start(SIPFSMDispatch* kpDispatch);
    /// Start the first asynchronous operation for the connection.

  void stop();
    /// Stop all asynchronous operations associated with the connection.

  void writeMessage(SIPMessage::Ptr msg);
    /// Send a SIP message using this transport.

  bool writeKeepAlive();
    /// Sends a keep-alive packet to remote to check if transport is still alive
private:
  void writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port);
    /// Send a SIP message using this session.  This is used by the UDP tranport

  void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData = 0);
    /// Handle completion of a read operation.

  void handleWrite(const boost::system::error_code& e);
    /// Handle completion of a write operation.

  void handleResolve(boost::asio::ip::tcp::resolver::iterator endPointIter);
    /// Handle completion of connect resolve query

  void handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter);
    /// Handle completion of async connect

  void handleHandshake(const boost::system::error_code& error);

  OSS::IPAddress getLocalAddress()const;
    /// Returns the local address binding for this transport

  OSS::IPAddress getRemoteAddress() const;
    /// Returns the last read source address

  void clientBind(const OSS::IPAddress& listener, unsigned short portBase, unsigned short portMax);
    /// Bind the local client

  void clientConnect(const OSS::IPAddress& target);
    /// Connect to a remote host

protected:

  boost::asio::ip::tcp::socket _socket;
    /// Socket for the connection.

  boost::asio::ip::tcp::resolver _resolver;
    /// the TCP query resolver

  SIPTCPConnectionManager& _connectionManager;
    /// The manager for this connection.

  boost::array<char, 8192> _buffer;
    /// Buffer for incoming data.

  SIPMessage::Ptr _pRequest;
    /// Incoming SIP Message parser

  SIPFSMDispatch* _pDispatch;

  mutable OSS::IPAddress _localAddress;
  mutable OSS::IPAddress _lastReadAddress;

  int _readExceptionCount;
};


//
// Inlines
//

inline boost::asio::ip::tcp::socket& SIPTCPConnection::socket()
{
  return _socket;
}

} } // OSS::SIP
#endif // SIP_SIPTCPConnection_INCLUDED
