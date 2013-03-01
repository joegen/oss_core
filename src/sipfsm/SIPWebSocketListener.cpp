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


#include <boost/asio/ip/tcp.hpp>

#include "OSS/SIP/SIPWebSocketListener.h"


namespace OSS {
namespace SIP {


SIPWebSocketListener::SIPWebSocketListener(
  SIPTransportService* pTransportService,
  const std::string& address,
  const std::string& port,
  SIPWebSocketConnectionManager& connectionManager) :
    SIPListener(pTransportService, address, port),
    _pServerEndPoint(0),
    _pClientEndPoint(0),
    _connectionManager(connectionManager),
    _pResolver(0)
{
  _pServerHandler = websocketpp::server::handler::ptr(new ServerHandler(*this));
  _pClientHandler = websocketpp::client::handler::ptr(new ClientHandler(*this));
  _pServerEndPoint = new websocketpp::server(_pServerHandler);
  _pClientEndPoint = new websocketpp::client(_pClientHandler);
  _pResolver = new boost::asio::ip::tcp::resolver(_pServerEndPoint->get_io_service());
}

SIPWebSocketListener::~SIPWebSocketListener()
{
  _pServerEndPoint->stop_listen(true);
  _pServerThread->join();
  delete _pServerThread;
  _pServerThread = 0;

  delete _pServerEndPoint;
  _pServerEndPoint = 0;
  delete _pClientEndPoint;
  _pClientEndPoint = 0;
  delete _pResolver;
  _pResolver = 0;
}

void SIPWebSocketListener::run()
{
  _pServerThread = new boost::thread(boost::bind(&SIPWebSocketListener::internal_run, this));
}

void SIPWebSocketListener::internal_run()
{
  boost::asio::ip::tcp::resolver::query query(getAddress(), getPort());
  boost::asio::ip::tcp::endpoint endpoint = *(_pResolver->resolve(query));
  _pServerEndPoint->listen(endpoint, 1);
}

void SIPWebSocketListener::handleAccept(const boost::system::error_code& e, OSS_HANDLE connectionPtr)
{
  if (!e)
  {
    websocketpp::server::connection_ptr* pConnection = 
      reinterpret_cast<websocketpp::server::connection_ptr*>(connectionPtr);
    OSS_VERIFY_NULL(pConnection);
    SIPWebSocketConnection::Ptr conn(new SIPWebSocketConnection(*pConnection));
    _connectionManager.start(conn);
  }
}

void SIPWebSocketListener::handleStop()
{

}

void SIPWebSocketListener::connect(const std::string& address, const std::string& port)
{

}

void SIPWebSocketListener::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter)
{

}

void SIPWebSocketListener::on_message(websocketpp::server::connection_ptr pConnection, websocketpp::server::handler::message_ptr pMsg)
{
  SIPWebSocketConnection::Ptr conn = _connectionManager.findConnectionByPtr(pConnection);
  if (conn)
  {
    boost::system::error_code ec;
    //conn->handleRead(ec);
  }

}

void SIPWebSocketListener::on_message(websocketpp::client::connection_ptr pConnection, websocketpp::client::handler::message_ptr pMsg)
{
}


} } // OSS::SIP



