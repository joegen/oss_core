#include "OSS/Net/FramedTcpListener.h"
#include "OSS/UTL/Logger.h"

namespace OSS {

FramedTcpListener::FramedTcpListener() :
  _ioService(),
  _acceptor(_ioService),
  _resolver(_ioService),
  _inactivityThreashold(180)
{
  OSS_LOG_DEBUG( "FramedTcpListener CREATED");
}

FramedTcpListener::~FramedTcpListener()
{
  OSS_LOG_DEBUG( "FramedTcpListener DESTROYED");
}

void FramedTcpListener::run(const std::string& address, const std::string& port)
{
  _pNewConnection = FramedTcpConnection::Ptr(new FramedTcpConnection(*this));

  boost::system::error_code ec;
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *_resolver.resolve(query, ec);
  
  if (!ec)
  {
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();
    _acceptor.async_accept(dynamic_cast<FramedTcpConnection*>(_pNewConnection.get())->socket(),
        boost::bind(&FramedTcpListener::handleAccept, this,
          boost::asio::placeholders::error));

    OSS_LOG_INFO( "FramedTcpListener::run "
      << " started accepting connections at bind address tcp://" << address << ":" << port);
  }
  else
  {
    OSS_LOG_ERROR( "FramedTcpListener::run " << boost::diagnostic_information(ec));
  }
}

void FramedTcpListener::handleAccept(const boost::system::error_code& e)
{
  addConnection(_pNewConnection);
  if (_acceptor.is_open())
  {
    _pNewConnection.reset(new FramedTcpConnection(*this));
    _acceptor.async_accept(dynamic_cast<FramedTcpConnection*>(_pNewConnection.get())->socket(),
      boost::bind(&FramedTcpListener::handleAccept, this,
        boost::asio::placeholders::error));
  }
}


void FramedTcpListener::addConnection(FramedTcpConnection::Ptr conn)
{
  mutex_lock lock(_mutex);
  _connections[conn.get()] = conn;
  OSS_LOG_INFO( "FramedTcpListener::addConnection "
    << " connection accepted "  << conn.get());
  conn->start();
}

void FramedTcpListener::destroyConnection(FramedTcpConnection::Ptr conn)
{
  mutex_lock lock(_mutex);
  _connections.erase(conn.get());
  OSS_LOG_INFO( "FramedTcpListener::destroyConnection "
    << " connection removed - " << conn.get());
  conn->stop();
}

void FramedTcpListener::onIncomingRequest(FramedTcpConnection& connection, const char* data, std::size_t len)
{

}

} // OSS


