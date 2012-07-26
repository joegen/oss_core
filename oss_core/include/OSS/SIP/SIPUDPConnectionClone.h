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


#ifndef SIP_SIPUDPConnectionClone_INCLUDED
#define SIP_SIPUDPConnectionClone_INCLUDED


#include "OSS/SIP/SIPUDPConnection.h"


namespace OSS {
namespace SIP {


class SIPUDPConnectionClone;
class SIPFSMDispatch;

class OSS_API SIPUDPConnectionClone: 
  public SIPTransportSession,
  public boost::enable_shared_from_this<SIPUDPConnectionClone>,
  private boost::noncopyable
{
public:

  SIPUDPConnectionClone(SIPUDPConnection::Ptr clonable);
    /// Creates a UDP connection using the given I/O service

  boost::asio::ip::udp::socket& socket();
    /// Get the socket associated with the connection.

  void start(SIPFSMDispatch* pDispatch);
    /// Start the first asynchronous operation for the connection.

  void stop();
    /// Stop all asynchronous operations associated with the connection.

  void writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port);
    /// Send a SIP message using this session.  This is used by the UDP tranport

  void clientBind(const OSS::IPAddress& listener, unsigned short portBase, unsigned short portMax);
    /// Bind the local client.  Take note that this is not implemented at all for UDP.

  void clientConnect(const OSS::IPAddress& target);
    /// Connect to a remote host.  Take note taht this is not implemented at all for UDP.

  OSS::IPAddress getLocalAddress() const;
    /// Returns the local address binding for this transport

  OSS::IPAddress getRemoteAddress() const;
    /// Returns the last read source address

private:
  void writeMessage(SIPMessage::Ptr msg);
    /// Send a SIP message using this transport.

  void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred);
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

  boost::asio::ip::udp::endpoint _senderEndPoint;
    /// The remote endpoint

  SIPMessage::Ptr _pRequest;
    /// Incoming SIP Message parser

  SIPUDPConnection::Ptr _orginalConnection;
};


//
// Inlines
//

inline boost::asio::ip::udp::socket& SIPUDPConnectionClone::socket()
{
  return _socket;
}

} } // OSS::SIP
#endif // SIP_SIPUDPConnectionClone_INCLUDED
