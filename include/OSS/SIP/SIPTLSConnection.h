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


#ifndef SIP_SIPTLSConnection_INCLUDED
#define SIP_SIPTLSConnection_INCLUDED


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


class SIPTLSConnectionManager;
class SIPTLSConnection;
class SIPFSMDispatch;

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

class OSS_API SIPTLSConnection: 
  public SIPTransportSession,
  public boost::enable_shared_from_this<SIPTLSConnection>,
  private boost::noncopyable
{
public:

  explicit SIPTLSConnection(
      boost::asio::io_service& ioService,
      boost::asio::ssl::context& tlsContext,
      SIPTLSConnectionManager& manager);
    /// Creates a TLS connection using the given I/O service

  ssl_socket& socket();
    /// Get the socket associated with the connection.

  void start(SIPFSMDispatch* pDispatch);
    /// Start the first asynchronous operation for the connection.

  void stop();
    /// Stop all asynchronous operations associated with the connection.

  void writeMessage(SIPMessage::Ptr msg);
    /// Send a SIP message using this transport.

  void clientBind(const OSS::IPAddress& listener, unsigned short portBase, unsigned short portMax);
    /// Bind the local client

  void clientConnect(const OSS::IPAddress& target);
    /// Connect to a remote host
private:
  virtual void writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port);
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
    /// Handle a secure hand shake from remote endpoint

  OSS::IPAddress getLocalAddress() const;
    /// Returns the local address binding for this transport

  OSS::IPAddress getRemoteAddress() const;
    /// Returns the last read source address

protected:

  boost::asio::ssl::context& _tlsContext;
    /// The TLS context

  ssl_socket _socket;
    /// SSL Socket for the connection.

  SIPTLSConnectionManager& _connectionManager;
    /// The manager for this connection.

  boost::array<char, 8192> _buffer;
    /// Buffer for incoming data.

  SIPMessage * _pRequest;
    /// Incoming SIP Message parser

  SIPFSMDispatch* _pDispatch;
    /// The FSM dispatcher for incoming message
};


//
// Inlines
//

inline ssl_socket& SIPTLSConnection::socket()
{
  return _socket;
}

} } // OSS::SIP
#endif // SIP_SIPTLSConnection_INCLUDED
