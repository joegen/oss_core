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

#include "OSS/Net/WebSocketServer.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace Net {

  
static int get_next_index()
{
  static int index = 0;
  if (++index >= 0xFFFFFFF)
  {
    index = 1;
  }
  return index;
}

class ServerReadWriteHandler : public websocketpp::server::handler
{
public:
  ServerReadWriteHandler(WebSocketServer& listener) :
    _listener(listener)
  {
  }

  void on_close(websocketpp::server::connection_ptr pConnection)
  {
    _listener.handleClose(pConnection);
  }

  void on_message(websocketpp::server::connection_ptr pConnection, websocketpp::server::handler::message_ptr pMsg)
  {
    std::string payload = pMsg->get_payload();
    _listener.handleMessage(pConnection, payload);
  }
  
  void on_error(websocketpp::server::connection_ptr pConnection)
  {
    std::string error = pConnection->get_fail_reason();
    _listener.handleError(pConnection, error);
  }

  bool on_ping(websocketpp::server::connection_ptr pConnection)
  {
    _listener.handlePing(pConnection);
    return true;
  }
  
  void on_pong(websocketpp::server::connection_ptr pConnection)
  {
    _listener.handlePong(pConnection);
  }
  
  void on_pong_timeout(websocketpp::server::connection_ptr pConnection,std::string)
  {
    _listener.handlePongTimeout(pConnection);
  }

private:
  WebSocketServer& _listener;
};
  
class ServerAcceptHandler : public websocketpp::server::handler
{
public:
  ServerAcceptHandler(WebSocketServer& listener) :
    _listener(listener)
  {
  }

  void on_fail(websocketpp::server::connection_ptr pConnection)
  {
    OSS_LOG_ERROR("ServerAcceptHandler::on_fail reason: " << pConnection->get_fail_reason());
  }
  void on_open(websocketpp::server::connection_ptr pConnection)
  {
    OSS_LOG_DEBUG("SIPWebSocketListener::ServerAcceptHandler::on_open");
    _listener.handleOpen(pConnection);
  }
  void validate(websocketpp::server::connection_ptr pConnection)
  {
  }
  WebSocketServer& _listener;
};
  
WebSocketServer::WebSocketServer() :
  _pServerThread(0),
  _hasStarted(false)
{
  _pServerAcceptHandler = websocketpp::server::handler::ptr(new ServerAcceptHandler(*this));
  _pServerEndPoint = new websocketpp::server(_pServerAcceptHandler);
  if (PRIO_DEBUG == log_get_level())
  {
    _pServerEndPoint->alog().set_level(websocketpp::log::alevel::ALL);
    _pServerEndPoint->elog().set_level(websocketpp::log::elevel::ALL);
  }
}

WebSocketServer::~WebSocketServer()
{
  close();
  delete _pServerEndPoint;
}
  
bool WebSocketServer::listen(const std::string& bindAddress)
{
  _bindAddress = IPAddress::fromV4IPPort(bindAddress.c_str());
  if (!_bindAddress.isValid())
  {
    return false;
  }
  if (!_hasStarted)
  {
    assert(!_pServerThread);
    _pServerThread = new boost::thread(boost::bind(&WebSocketServer::internal_listen, this));
    _hasStarted = true;
  }
  return _hasStarted;
}

void WebSocketServer::internal_listen()
{
  try
  {
    boost::asio::ip::tcp::resolver resolver(_pServerEndPoint->get_io_service());
    boost::asio::ip::tcp::resolver::query query(_bindAddress.toString(), OSS::string_from_number<unsigned short>(_bindAddress.getPort()));
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
    _pServerEndPoint->listen(endpoint); // this will block
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR("WebSocketServer::listen " << _bindAddress.toIpPortString() << " Exception: " << e.what());
  }
  
  _hasStarted = false;
}

void WebSocketServer::close()
{
  _pServerEndPoint->stop_listen(true);
  if (_pServerThread)
  {
    _pServerThread->join();
    delete _pServerThread;
    _pServerThread = 0;
  }
}

bool  WebSocketServer::sendMessage(int connectionId, const std::string& msg)
{
  OSS::mutex_critic_sec_lock lock(_connectionsMutex);
  Connections::iterator iter = _connections.find(connectionId);
  if (iter == _connections.end())
  {
    return false;
  }
  try
  {
    iter->second->send(msg, websocketpp::frame::opcode::BINARY);
  }
  catch(std::exception& e)
  {
    return false;
  }
  return true;
}

void WebSocketServer::handleOpen(websocketpp::server::connection_ptr pConnection)
{
  pConnection->set_handler(websocketpp::server::handler::ptr(new ServerReadWriteHandler(*this)));
  
  _connectionsMutex.lock();
  pConnection->m_Identifier = get_next_index();
  
  OSS_LOG_DEBUG("WebSocketServer::handleOpen: Connection id " <<  pConnection->m_Identifier << " CREATED");
  _connections[pConnection->m_Identifier] = pConnection;
  _connectionsMutex.unlock();
  
  onOpen(pConnection->m_Identifier);
}

void WebSocketServer::handleClose(websocketpp::server::connection_ptr pConnection)
{
  OSS_LOG_DEBUG("WebSocketServer::handleClose: Connection id " <<  pConnection->m_Identifier << " CLOSED");
  onClose(pConnection->m_Identifier);
  _connectionsMutex.lock();
  _connections.erase(pConnection->m_Identifier);
  _connectionsMutex.unlock();
}

void WebSocketServer::handleError(websocketpp::server::connection_ptr pConnection, const std::string& error)
{
  OSS_LOG_DEBUG("WebSocketServer::handleError: Connection id " <<  pConnection->m_Identifier << " ERROR: " << error);
  onError(pConnection->m_Identifier, error);
}

void WebSocketServer::handleMessage(websocketpp::server::connection_ptr pConnection, const std::string& message)
{
  OSS_LOG_DEBUG("WebSocketServer::handleMessage: Connection id " <<  pConnection->m_Identifier << " received message " << message);
  onMessage(pConnection->m_Identifier, message);
}

void WebSocketServer::handlePing(websocketpp::server::connection_ptr pConnection)
{
  onPing(pConnection->m_Identifier);
}

void WebSocketServer::handlePong(websocketpp::server::connection_ptr pConnection)
{
  onPong(pConnection->m_Identifier);
}

void WebSocketServer::handlePongTimeout(websocketpp::server::connection_ptr pConnection)
{
  onPongTimeout(pConnection->m_Identifier);
}


} } // OSS::NET


