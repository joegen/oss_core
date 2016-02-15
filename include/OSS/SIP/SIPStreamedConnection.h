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


#ifndef SIP_SIPStreamedConnection_INCLUDED
#define SIP_SIPStreamedConnection_INCLUDED


#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPTransportSession.h"


namespace OSS {
namespace SIP {


class SIPStreamedConnectionManager;
class SIPStreamedConnection;
class SIPFSMDispatch;

#define STREAMED_CONNECTION_BUFFER_SIZE 8192

class OSS_API SIPStreamedConnection: 
  public SIPTransportSession,
  public boost::enable_shared_from_this<SIPStreamedConnection>
{
public:

  typedef boost::asio::ip::tcp::socket::endpoint_type EndPoint;
  typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;
  typedef boost::asio::ip::tcp::socket tcp_socket;
  
  enum ConnectionError
  {
    CONNECTION_ERROR_READ,
    CONNECTION_ERROR_WRITE,
    CONNECTION_ERROR_CONNECT,
    CONNECTION_ERROR_CLIENT_HANDSHAKE,
    CONNECTION_ERROR_SERVER_HANDSHAKE,
    CONNECTION_ERROR_MAX        
  };
  
  explicit SIPStreamedConnection(
      boost::asio::io_service& ioService,
      SIPStreamedConnectionManager& manager,
      SIPListener* pListener);
    /// Creates a TCP connection using the given I/O service
  
  explicit SIPStreamedConnection(
      boost::asio::io_service& ioService,
      boost::asio::ssl::context* pTlsContext,
      SIPStreamedConnectionManager& manager,
      SIPListener* pListener);
    /// Creates a TLS connection using the given I/O service and TLS context
  
  virtual ~SIPStreamedConnection();
    /// Destroys the TCP connection

  boost::asio::ip::tcp::socket& socket();
    /// Get the socket associated with the connection.

  void start(const SIPTransportSession::Dispatch& dispatch);
    /// Start the first asynchronous operation for the connection.

  void stop();
    /// Stop all asynchronous operations associated with the connection.

  void writeMessage(const std::string& buf);
    /// Write the content of the buffer
  
  void writeMessage(const std::string& buf, boost::system::error_code& ec);
    /// Write the content of the buffer
  
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

  void handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter, boost::system::error_code* out_ec, Semaphore* pSem);
    /// Handle completion of async connect

  void handleClientHandshake(const boost::system::error_code& error);
  
  void handleServerHandshake(const boost::system::error_code& error);
  
  OSS::Net::IPAddress getLocalAddress()const;
    /// Returns the local address binding for this transport

  OSS::Net::IPAddress getRemoteAddress() const;
    /// Returns the last read source address

  void clientBind(const OSS::Net::IPAddress& listener, unsigned short portBase, unsigned short portMax);
    /// Bind the local client

  bool clientConnect(const OSS::Net::IPAddress& target);
  bool clientConnect(const OSS::Net::IPAddress& target, boost::posix_time::time_duration timeout);
    /// Connect to a remote host

  void readSome();
    /// read some bytes into the buffer
  
  
protected:
  void handleConnectTimeout(const boost::system::error_code& e);

  boost::asio::io_service& _ioService;
    /// The IO Service
  
  tcp_socket* _pTcpSocket;
    /// Socket for the connection.
  
  boost::asio::ssl::context* _pTlsContext;
    /// The TLS context
  
  boost::asio::deadline_timer _deadline;
    /// Deadline timer

  ssl_socket* _pTlsStream;
    /// SSL Socket for the connection.

  boost::asio::ip::tcp::resolver _resolver;
    /// the TCP query resolver

  SIPStreamedConnectionManager& _connectionManager;
    /// The manager for this connection.

  boost::array<char, STREAMED_CONNECTION_BUFFER_SIZE> _buffer;
    /// Buffer for incoming data.

  SIPMessage::Ptr _pRequest;
    /// Incoming SIP Message parser

  SIPFSMDispatch* _pDispatch;

  mutable OSS::Net::IPAddress _localAddress;
  mutable OSS::Net::IPAddress _lastReadAddress;
  
  int _readExceptionCount;
};


//
// Inlines
//


} } // OSS::SIP
#endif // SIP_SIPStreamedConnection_INCLUDED
