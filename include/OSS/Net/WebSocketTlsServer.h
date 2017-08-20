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

#ifndef OSS_WEBSOCKETTLSSERVER_H_INCLUDED
#define OSS_WEBSOCKETTLSSERVER_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_WEBSOCKETS

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "OSS/Net/WebSocket.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/Thread.h"
#include "OSS/Net/ServerTransport.h"
#include "OSS/Net/TlsContext.h"


namespace OSS {
namespace Net {


class WebSocketTlsServer : public ServerTransport
{
public:
  WebSocketTlsServer();
  virtual ~WebSocketTlsServer();
  
  virtual bool listen(const std::string& bindAddress);
  
  virtual void close();
  
  //
  // Connection callbacks
  //
  
  virtual void onOpen(int connectionId);
    /// A new connection is created
  
  virtual void onClose(int connectionId);
    /// The connection was closed 
  
  virtual void onError(int connectionId, const std::string& error);
    /// Conneciton encountered an error
  
  virtual void onMessage(int connectionId, const std::string& message);
    /// Connection receveid a new message
  
  virtual void onPing(int connectionId);
    /// Connection received a ping
  
  virtual void onPong(int connectionId);
    /// Connection received a pong
  
  virtual void onPongTimeout(int connectionId);
    /// Pong not received on time
  
  virtual bool sendMessage(int connectionId, const std::string& msg);
    /// Send a message using the connection identified by connectionId
  
  bool initializeTlsContext(
    bool verifyPeer,
    const std::string& peerCaFile, // can be empty
    const std::string& caDirectory, // can be empty
    const std::string& certPassword, // can be empty
    const std::string& certFile, // required
    const std::string& privateKey // required
  );
  
  const TlsContext::Context& getTlsContext() const;
  
protected:
  typedef std::map<int, websocketpp::server_tls::connection_ptr> Connections;
  
  void internal_listen();
  void handleOpen(websocketpp::server_tls::connection_ptr pConnection);
  void handleClose(websocketpp::server_tls::connection_ptr pConnection);
  void handleError(websocketpp::server_tls::connection_ptr pConnection, const std::string& error);
  void handleMessage(websocketpp::server_tls::connection_ptr pConnection, const std::string& message);
  void handlePing(websocketpp::server_tls::connection_ptr pConnection);
  void handlePong(websocketpp::server_tls::connection_ptr pConnection);
  void handlePongTimeout(websocketpp::server_tls::connection_ptr pConnection);

  boost::thread* _pServerThread;
  websocketpp::server_tls::handler::ptr _pServerAcceptHandler;
  websocketpp::server_tls* _pServerEndPoint;
  IPAddress _bindAddress;
  bool _hasStarted;
  OSS::mutex_critic_sec _connectionsMutex;
  Connections _connections;
  TlsContext _tlsContext;
  
  friend class ServerAcceptHandler;
  friend class ServerReadWriteHandler;
};

//
// Inlines
//

inline void  WebSocketTlsServer::onOpen(int connectionId)
{
}

inline void  WebSocketTlsServer::onClose(int connectionId)
{
}

inline void  WebSocketTlsServer::onError(int connectionId, const std::string& error)
{
}

inline void  WebSocketTlsServer::onMessage(int connectionId, const std::string& message)
{
}

inline void  WebSocketTlsServer::onPing(int connectionId)
{
}

inline void  WebSocketTlsServer::onPong(int connectionId)
{
}

inline void  WebSocketTlsServer::onPongTimeout(int connectionId)
{
}

inline bool WebSocketTlsServer::initializeTlsContext(
  bool verifyPeer,
  const std::string& peerCaFile, // can be empty
  const std::string& caDirectory, // can be empty
  const std::string& certPassword, // can be empty
  const std::string& certFile, // required
  const std::string& privateKey // required
)
{
  return _tlsContext.initialize(&_pServerEndPoint->get_io_service(), verifyPeer, peerCaFile, caDirectory, certPassword, certFile, privateKey);
}

inline const TlsContext::Context& WebSocketTlsServer::getTlsContext() const
{
  return _tlsContext.getServerContext();
}

} } // OSS::NET

#endif // ENABLE_FEATURE_WEBSOCKETS
#endif // OSS_WEBSOCKETTLSSERVER_H_INCLUDED

