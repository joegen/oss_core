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

#ifndef OSS_WEBSOCKETSERVER_H_INCLUDED
#define OSS_WEBSOCKETSERVER_H_INCLUDED

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

namespace OSS {
namespace Net {


class WebSocketServer : public ServerTransport
{
public:
  WebSocketServer();
  virtual ~WebSocketServer();
  
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
  
protected:
  typedef std::map<int, websocketpp::server::connection_ptr> Connections;
  
  void internal_listen();
  void handleOpen(websocketpp::server::connection_ptr pConnection);
  void handleClose(websocketpp::server::connection_ptr pConnection);
  void handleError(websocketpp::server::connection_ptr pConnection, const std::string& error);
  void handleMessage(websocketpp::server::connection_ptr pConnection, const std::string& message);
  void handlePing(websocketpp::server::connection_ptr pConnection);
  void handlePong(websocketpp::server::connection_ptr pConnection);
  void handlePongTimeout(websocketpp::server::connection_ptr pConnection);

  boost::thread* _pServerThread;
  websocketpp::server::handler::ptr _pServerAcceptHandler;
  websocketpp::server* _pServerEndPoint;
  IPAddress _bindAddress;
  bool _hasStarted;
  OSS::mutex_critic_sec _connectionsMutex;
  Connections _connections;
  
  friend class ServerAcceptHandler;
  friend class ServerReadWriteHandler;
};

//
// Inlines
//

inline void  WebSocketServer::onOpen(int connectionId)
{
}

inline void  WebSocketServer::onClose(int connectionId)
{
}

inline void  WebSocketServer::onError(int connectionId, const std::string& error)
{
}

inline void  WebSocketServer::onMessage(int connectionId, const std::string& message)
{
}

inline void  WebSocketServer::onPing(int connectionId)
{
}

inline void  WebSocketServer::onPong(int connectionId)
{
}

inline void  WebSocketServer::onPongTimeout(int connectionId)
{
}

} } // OSS::NET

#endif // ENABLE_FEATURE_WEBSOCKETS
#endif // OSS_WEBSOCKETSERVER_H_INCLUDED

