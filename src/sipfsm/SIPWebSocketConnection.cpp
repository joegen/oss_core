
// Library: OSS Software Solutions Application Programmer Interface
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


#include "OSS/SIP/SIPWebSocketConnection.h"


namespace OSS {
namespace SIP {


SIPWebSocketConnection::SIPWebSocketConnection() :
  _isServerTransport(false)
{
}

SIPWebSocketConnection::SIPWebSocketConnection(const websocketpp::server::connection_ptr& pConnection) :
  _pServer(pConnection),
  _isServerTransport(true)
{
}

SIPWebSocketConnection::~SIPWebSocketConnection()
{
}

void SIPWebSocketConnection::writeMessage(SIPMessage::Ptr msg)
    /// Send a SIP message using this session.
{
  if (_isServerTransport)
    _pServer->send(msg->data(), websocketpp::frame::opcode::BINARY);
  else if (_pClient)
    _pClient->send(msg->data(), websocketpp::frame::opcode::BINARY);
}

void SIPWebSocketConnection::writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port)
    /// Send a SIP message using this session.  This is used by the UDP tranport
{
  //
  // Not implemented for websockets
  //
  OSS_ASSERT(false);
}

bool SIPWebSocketConnection::writeKeepAlive()
    /// Send a connection specific keep-alive
    /// This is normally invoked by the application layer to
    /// keep NAT port bindings open as well as to poke
    /// reliability of the transport for stream based connections.
    /// The default behavior sends nothing
{
  return false;
}

bool SIPWebSocketConnection::writeKeepAlive(const std::string& ip, const std::string& port)
    /// Send a connection specific keep-alive
    /// This is normally invoked by the application layer to
    /// keep NAT port bindings open as well as to poke
    /// reliability of the transport for stream based connections.
    /// The default packet is CRLF/CRLF
{
  //
  // Not implemented for websockets
  //
  OSS_ASSERT(false);
  return false;
}

void SIPWebSocketConnection::start(SIPFSMDispatch* pDispatch)
    /// Start the first asynchronous operation for the connection.
{
  //
  // Not implemented in websockets
  //
}

void SIPWebSocketConnection::stop()
    /// Stop all asynchronous operations associated with the connection.
{
  //
  // Not implemented in websockets
  //
}

void SIPWebSocketConnection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred)
    /// Handle completion of a read operation.
{

}

void SIPWebSocketConnection::handleWrite(const boost::system::error_code& e)
    /// Handle completion of a write operation.
{

}

void SIPWebSocketConnection::handleResolve(boost::asio::ip::tcp::resolver::iterator endPointIter)
    /// Handle completion of connect resolve query
{

}

void SIPWebSocketConnection::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter)
    /// Handle completion of async connect
{

}

void SIPWebSocketConnection::handleHandshake(const boost::system::error_code& error)
    /// Handle a secure hand shake from remote endpoint
{

}

OSS::IPAddress SIPWebSocketConnection::getLocalAddress() const
    /// Returns the local address binding for this transport
{
  return OSS::IPAddress();
}

OSS::IPAddress SIPWebSocketConnection::getRemoteAddress() const
    /// Returns the last read source address
{
  return OSS::IPAddress();
}

void SIPWebSocketConnection::clientBind(const OSS::IPAddress& ip, unsigned short portBase, unsigned short portMax)
    /// Bind the local client
{

}

void SIPWebSocketConnection::clientConnect(const OSS::IPAddress& target)
    /// Connect to a remote host
{

}


} } /// OSS::SIP



