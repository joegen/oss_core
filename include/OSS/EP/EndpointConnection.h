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

#ifndef ENDPOINTCONNECTION_H_INCLUDED
#define	ENDPOINTCONNECTION_H_INCLUDED


#include "OSS/SIP/SIPTransportSession.h"


namespace OSS {
namespace EP {

using OSS::SIP::SIPTransportSession;
using OSS::SIP::SIPMessage;

class EndpointListener;  

class EndpointConnection : public SIPTransportSession
{
public:
  EndpointConnection(EndpointListener* pEndpoint);
  
  ~EndpointConnection();
  
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

  virtual void start(const SIPTransportSession::Dispatch& dispatch);
    /// Start the first asynchronous operation for the connection.

  virtual void stop();
    /// Stop all asynchronous operations associated with the connection.

  virtual void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData = 0);
    /// Handle completion of a read operation.

  virtual void handleWrite(const boost::system::error_code& e);
    /// Handle completion of a write operation.


  virtual void handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter);
    /// Handle completion of async connect

  virtual void handleClientHandshake(const boost::system::error_code& error);
    /// Handle a secure hand shake from remote endpoint

  virtual void handleServerHandshake(const boost::system::error_code& error);
    /// Handle a secure handshake from remote endpoint
  virtual OSS::Net::IPAddress getLocalAddress() const;
    /// Returns the local address binding for this transport

  virtual OSS::Net::IPAddress getRemoteAddress() const;
    /// Returns the last read source address

  virtual void clientBind(const OSS::Net::IPAddress& ip, unsigned short portBase, unsigned short portMax);
    /// Bind the local client

  virtual void clientConnect(const OSS::Net::IPAddress& target);
    /// Connect to a remote host

protected:
  EndpointListener* _pEndpoint;
};

//
// Inlines
//
  
} } 

#endif // ENDPOINTCONNECTION_H_INCLUDED

