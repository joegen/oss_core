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


#ifndef OSS_SIPWEBSOCKETCONNECTION_H_INCLUDED
#define	OSS_SIPWEBSOCKETCONNECTION_H_INCLUDED


#include "OSS/websocketpp/roles/client.hpp"
#include "OSS/websocketpp/roles/server.hpp"
#include "OSS/websocketpp/websocketpp.hpp"
#include "OSS/SIP/SIPTransportSession.h"

namespace OSS {
namespace SIP {

    
class SIPWebSocketConnection : public SIPTransportSession
{
public:
  SIPWebSocketConnection();

  SIPWebSocketConnection(const websocketpp::server::connection_ptr& pConnection);

  ~SIPWebSocketConnection();

  virtual void writeMessage(SIPMessage::Ptr msg);
    /// Send a SIP message using this session.

  virtual void writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port);
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

  virtual void start(SIPFSMDispatch* pDispatch);
    /// Start the first asynchronous operation for the connection.

  virtual void stop();
    /// Stop all asynchronous operations associated with the connection.

  virtual void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData = 0);
    /// Handle completion of a read operation.

  virtual void handleWrite(const boost::system::error_code& e);
    /// Handle completion of a write operation.

  virtual void handleResolve(boost::asio::ip::tcp::resolver::iterator endPointIter);
    /// Handle completion of connect resolve query

  virtual void handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter);
    /// Handle completion of async connect

  virtual void handleHandshake(const boost::system::error_code& error);
    /// Handle a secure hand shake from remote endpoint

  virtual OSS::IPAddress getLocalAddress() const;
    /// Returns the local address binding for this transport

  virtual OSS::IPAddress getRemoteAddress() const;
    /// Returns the last read source address

  virtual void clientBind(const OSS::IPAddress& ip, unsigned short portBase, unsigned short portMax);
    /// Bind the local client

  virtual void clientConnect(const OSS::IPAddress& target);
    /// Connect to a remote host

protected:
  websocketpp::server::connection_ptr _pServer;
  websocketpp::client::connection_ptr _pClient;
private:
  bool _isServerTransport;
  friend class SIPWebSocketConnectionManager;
};


} } /// OSS::SIP


#endif	// OSS_SIPWEBSOCKETCONNECTION_H_INCLUDED

