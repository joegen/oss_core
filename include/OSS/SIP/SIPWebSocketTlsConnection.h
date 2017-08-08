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

//
// WebSocket++ has the following license :
//

/*
 * Copyright (c) 2011, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#ifndef OSS_SIPWEBSOCKETTLSCONNECTION_H_INCLUDED
#define	OSS_SIPWEBSOCKETTLSCONNECTION_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_WEBSOCKETS

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/Net/WebSocket.h"
#include "OSS/SIP/SIPTransportSession.h"


namespace OSS {
namespace SIP {

class SIPWebSocketTlsConnectionManager;
class SIPFSMDispatch;

class SIPWebSocketTlsConnection :
		public SIPTransportSession,
		public boost::enable_shared_from_this<SIPWebSocketTlsConnection>
{
protected:
  class ServerReadWriteHandler : public websocketpp::server_tls::handler
  {
  public:
    ServerReadWriteHandler(SIPWebSocketTlsConnection& rConnection);

    void on_close(websocketpp::server_tls::connection_ptr pConnection);

    void on_message(websocketpp::server_tls::connection_ptr pConnection, websocketpp::server_tls::handler::message_ptr pMsg);
    void on_error(websocketpp::server_tls::connection_ptr pConnection);

    bool on_ping(websocketpp::server_tls::connection_ptr pConnection);
    void on_pong(websocketpp::server_tls::connection_ptr pConnection);
    void on_pong_timeout(websocketpp::server_tls::connection_ptr pConnection,std::string);
    
    boost::shared_ptr<boost::asio::ssl::context> on_tls_init();

  private:
    SIPWebSocketTlsConnection& _rConnection;
  };

public:
  typedef boost::asio::ip::tcp::socket::endpoint_type EndPoint;

  SIPWebSocketTlsConnection(SIPWebSocketTlsConnectionManager& manager, SIPListener* pListener);

  SIPWebSocketTlsConnection(const websocketpp::server_tls::connection_ptr& pConnection, SIPWebSocketTlsConnectionManager& manager,
      SIPListener* pListener);

  virtual ~SIPWebSocketTlsConnection();

  void start(const SIPTransportSession::Dispatch& dispatch);
    /// Start the first asynchronous operation for the connection.

  void stop();
    /// Stop all asynchronous operations associated with the connection.

  void writeMessage(SIPMessage::Ptr msg);
    /// Send a SIP message using this session.

  bool writeKeepAlive();
    /// Send a connection specific keep-alive
    /// This is normally invoked by the application layer to
    /// keep NAT port bindings open as well as to poke
    /// reliability of the transport for stream based connections.
    /// The default behavior sends nothing

  void writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port);
    /// Send a SIP message using this session.  This is used by the UDP tranport

  bool writeKeepAlive(const std::string& ip, const std::string& port);
    /// Send a connection specific keep-alive
    /// This is normally invoked by the application layer to
    /// keep NAT port bindings open as well as to poke
    /// reliability of the transport for stream based connections.
    /// The default packet is CRLF/CRLF

  void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData = 0);
    /// Handle completion of a read operation.

  void handleWrite(const boost::system::error_code& e);
    /// Handle completion of a write operation.

  void handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter, boost::system::error_code* out_ec, Semaphore* pSem);
    /// Handle completion of async connect

  void handleClientHandshake(const boost::system::error_code& error);
    /// Handle a secure hand shake from remote endpoint

  void handleServerHandshake(const boost::system::error_code& error);
  
  OSS::Net::IPAddress getLocalAddress() const;
    /// Returns the local address binding for this transport

  OSS::Net::IPAddress getRemoteAddress() const;
    /// Returns the last read source address

  void clientBind(const OSS::Net::IPAddress& ip, unsigned short portBase, unsigned short portMax);
    /// Bind the local client

  bool clientConnect(const OSS::Net::IPAddress& target);
    /// Connect to a remote host

  websocketpp::server_tls::connection_ptr _pServerConnection;
  websocketpp::server_tls::handler::ptr _pServerConnectionHandler;

protected:
  SIPWebSocketTlsConnectionManager& _connectionManager;
    /// The manager for this connection.

  SIPMessage::Ptr _pRequest;
    /// Incoming SIP Message parser

private:

  mutable OSS::Net::IPAddress _localAddress;
  mutable OSS::Net::IPAddress _lastReadAddress;

  int _readExceptionCount;
};


} } /// OSS::SIP

#endif // ENABLE_FEATURE_WEBSOCKETS

#endif	// OSS_SIPWEBSOCKETTLSCONNECTION_H_INCLUDED

