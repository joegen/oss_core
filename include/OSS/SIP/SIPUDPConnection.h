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


#ifndef SIP_SIPUDPConnection_INCLUDED
#define SIP_SIPUDPConnection_INCLUDED


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPTransportSession.h"


namespace OSS {
namespace SIP {


class SIPUDPConnection;
class SIPFSMDispatch;

class OSS_API SIPUDPConnection: 
  public SIPTransportSession,
  public boost::enable_shared_from_this<SIPUDPConnection>,
  private boost::noncopyable
{
public:

  explicit SIPUDPConnection(
      boost::asio::io_service& ioService,
      boost::asio::ip::udp::socket& socket);
    /// Creates a UDP connection using the given I/O service

  virtual ~SIPUDPConnection();
  
  boost::asio::ip::udp::socket& socket();
    /// Get the socket associated with the connection.

  void start(SIPFSMDispatch* pDispatch);
    /// Start the first asynchronous operation for the connection.

  void stop();
    /// Stop all asynchronous operations associated with the connection.

  void writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port);
    /// Send a SIP message using this session.  This is used by the UDP tranport

  bool writeKeepAlive(const std::string& ip, const std::string& port);
    /// Send a connection specific keep-alive
    /// This is normally invoked by the application layer to
    /// keep NAT port bindings open as well as to poke
    /// reliability of the transport for stream based connections.
    /// The default packet is CRLF/CRLF

  void clientBind(const OSS::Net::IPAddress& listener, unsigned short portBase, unsigned short portMax);
    /// Bind the local client.  Take note that this is not implemented at all for UDP.

  void clientConnect(const OSS::Net::IPAddress& target);
    /// Connect to a remote host.  Take note that this is not implemented at all for UDP.

  OSS::Net::IPAddress getLocalAddress() const;
    /// Returns the local address binding for this transport

  OSS::Net::IPAddress getRemoteAddress() const;
    /// Returns the last read source address

private:
  void writeMessage(SIPMessage::Ptr msg);
    /// Send a SIP message using this transport.

  void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData = 0);
    /// Handle completion of a read operation.

  void handleWrite(const boost::system::error_code& e);
    /// Handle completion of a write operation.

  void handleResolve(boost::asio::ip::tcp::resolver::iterator endPointIter);
    /// Handle completion of connect resolve query

  void handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter);
    /// Handle completion of async connect

  void handleHandshake(const boost::system::error_code& error);
    /// Handle remote handshake.  Only significant to TSL



protected:

  boost::asio::ip::udp::socket& _socket;
    /// Socket for the connection.

  boost::array<char, OSS_SIP_MAX_PACKET_SIZE> _buffer;
    /// Buffer for incoming data.

  boost::asio::ip::udp::endpoint _senderEndPoint;
    /// The remote endpoint

  boost::asio::ip::udp::resolver _resolver;

  SIPMessage::Ptr _pRequest;
    /// Incoming SIP Message parser

  friend class SIPUDPConnectionClone;
};


//
// Inlines
//

inline boost::asio::ip::udp::socket& SIPUDPConnection::socket()
{
  return _socket;
}

} } // OSS::SIP
#endif // SIP_SIPUDPConnection_INCLUDED
