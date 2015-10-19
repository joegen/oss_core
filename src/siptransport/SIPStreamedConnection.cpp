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

//////////////////////////////////////////////////////////////////////////////// 
//
// CLIENT FLOW
//
// conn::clientConnect()
// |
// ---> conn::handleConnect()
//    |
//    ---> conn::handleClientHandshake() # this is skipped for TCP
//       |
//       ---> manager::start()
//          |
//          ---> conn::start()
//             |
//             ---> conn::readSome()
//
// SERVER FLOW
// 
// listener::handleAccept()
// |
// ---> manager::start()
//    |
//    ---> conn::start()
//       |
//       ---> conn::handleServerHandshake()
//          |
//          ---> conn::readSome()
////////////////////////////////////////////////////////////////////////////////
 


#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "OSS/UTL/Logger.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPStreamedConnection.h"
#include "OSS/SIP/SIPStreamedConnectionManager.h"
#include "OSS/SIP/SIPFSMDispatch.h"


namespace OSS {
namespace SIP {


SIPStreamedConnection::SIPStreamedConnection(
  boost::asio::io_service& ioService,
  SIPStreamedConnectionManager& manager) :
    _pTcpSocket(0),
    _pTlsContext(0),
    _pTlsStream(0),
    _resolver(ioService),
    _connectionManager(manager),
    _pDispatch(0),
    _readExceptionCount(0)
{
  _transportScheme = "tcp";
  _pTcpSocket = new boost::asio::ip::tcp::socket(ioService);
}

SIPStreamedConnection::SIPStreamedConnection(
  boost::asio::io_service& ioService,
  boost::asio::ssl::context* pTlsContext,
  SIPStreamedConnectionManager& manager) :
    _pTcpSocket(0),
    _pTlsContext(pTlsContext),
    _pTlsStream(0),
    _resolver(ioService),
    _connectionManager(manager),
    _pDispatch(0),
    _readExceptionCount(0)
{
  _transportScheme = "tls";
  _pTlsStream = new ssl_socket(ioService, *_pTlsContext);
  _pTcpSocket = &_pTlsStream->next_layer();
}

SIPStreamedConnection::~SIPStreamedConnection()
{
  if (!_pTlsStream)
  {
    //
    // This is a TCP connection.  We own the socket.
    //
    delete _pTcpSocket;
    _pTcpSocket = 0;
  }
  else
  {
    //
    // _pTcpSocket is owned by _pTlsStream.
    // There is no need to delete it here
    //
    delete _pTlsStream;
    _pTlsStream = 0;
  }
}

boost::asio::ip::tcp::socket& SIPStreamedConnection::socket()
{
  if (!_pTlsStream)
    return *_pTcpSocket;
  else
    return _pTlsStream->next_layer();
}
    /// Destroys the TCP connection

void SIPStreamedConnection::start(const SIPTransportSession::Dispatch& dispatch)
{
  setMessageDispatch(dispatch);
  
  if (_isClient)
  {
    //
    // Start is called after a successful client handshake with the server.
    // we can start reading from the secure stream at this point 
    //  
    readSome();
  }
  else
  {
    if (_pTlsStream)
    {
      //
      // Server initiates the handshake here
      //
      _pTlsStream->async_handshake(boost::asio::ssl::stream_base::server,
          boost::bind(&SIPStreamedConnection::handleServerHandshake, this,
            boost::asio::placeholders::error));
    }
    else
    {
      //
      // This is not a secure connection.  Start reading
      //
      readSome();
    }
  }
}

void SIPStreamedConnection::stop()
{
  OSS_LOG_WARNING("SIPStreamedConnection stopped reading from transport (" << getIdentifier() << ") " << getLocalAddress().toIpPortString() <<
    "->" << getRemoteAddress().toIpPortString() );
  _pTcpSocket->close();
}

void SIPStreamedConnection::readSome()
{
  if (_pTlsStream)
  {
    _pTlsStream->async_read_some(boost::asio::buffer(_buffer),
        boost::bind(&SIPStreamedConnection::handleRead, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred, (void*)0));
  }
  else
  {
    _pTcpSocket->async_read_some(boost::asio::buffer(_buffer),
        boost::bind(&SIPStreamedConnection::handleRead, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred, (void*)0));
  }
}

void SIPStreamedConnection::writeMessage(const std::string& buf)
{
  if (_pTlsStream)
  {
    ssl_socket& sock = *_pTlsStream;
    boost::asio::async_write(sock, boost::asio::buffer(buf, buf.size()),
          boost::bind(&SIPStreamedConnection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error));
  }
  else
  {
    tcp_socket& sock = *_pTcpSocket;
    boost::asio::async_write(sock, boost::asio::buffer(buf, buf.size()),
          boost::bind(&SIPStreamedConnection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error));
  }
}

void SIPStreamedConnection::writeMessage(const std::string& buf, boost::system::error_code& ec)
{
  if (_pTlsStream)
  {
    _pTlsStream->write_some(boost::asio::buffer(buf, buf.size()), ec);
  }
  else
  {
    _pTcpSocket->write_some(boost::asio::buffer(buf, buf.size()), ec);
  }
}

void SIPStreamedConnection::writeMessage(SIPMessage::Ptr msg)
{
  writeMessage(msg->data());
}

bool SIPStreamedConnection::writeKeepAlive()
{
  if (!_pTcpSocket->is_open())
    return false;

  std::string keepAlive("\r\n\r\n");
  boost::system::error_code ec;
  
  writeMessage(keepAlive, ec);

  if (ec)
  {
    OSS_LOG_WARNING("SIPStreamedConnection::writeKeepAlive() Exception - " << ec.message());
    boost::system::error_code ignored_ec;
    _pTcpSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    _connectionManager.stop(shared_from_this());
  }

  return !ec;
}

void SIPStreamedConnection::reportConnectionError(ConnectionError errorType, const boost::system::error_code& e)
{
  if (!_currentTransactionId.empty() && _pTransactionPool)
  {
    SIPTransaction::Ptr pTransaction = _pTransactionPool->findTransaction(_currentTransactionId, false);
    if (pTransaction)
    {
      pTransaction->handleConnectionError(errorType, e);
    }
  }
}

void SIPStreamedConnection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE /*userData*/)
{
  if (!e && bytes_transferred)
  {
    //
    // set the last read address
    //
    if (!_lastReadAddress.isValid())
    {
      boost::system::error_code ec;
      EndPoint ep = _pTcpSocket->remote_endpoint(ec);
      if (!ec)
      {
        boost::asio::ip::address ip = ep.address();
        _lastReadAddress = OSS::Net::IPAddress(ip.to_string(), ep.port());
      }
    }

    //
    // Reset the read exception count
    //
    _readExceptionCount = 0;

    if (!_pRequest)
      _pRequest = SIPMessage::Ptr(new SIPMessage());

    _bytesRead =  bytes_transferred;
    
    boost::tribool result;
    const char* begin = _buffer.data();
    const char* end = _buffer.data() + bytes_transferred;
    //boost::tie(result, boost::tuples::ignore) =
    boost::tuple<boost::tribool, const char*> ret =  _pRequest->consume(begin, end);
    result = ret.get<0>();
    const char* tail = ret.get<1>();
    if (result)
    {
      //
      // Message has been read in full
      //
      dispatchMessage(_pRequest->shared_from_this(), shared_from_this());
      
      if (tail >= end)
      {
        //
        // The end of the SIPMessage is reached so we can simply reset the
        // request buffer and start the read operation all over again
        //
        _pRequest.reset();
        readSome();
        //
        // We are done
        //
        return;
      }
      else
      {
        //
        // This will happen if the tail is within the range of the end of the read buffer.
        // We need to parse it as the start of the next message segment
        //
        OSS_LOG_DEBUG("SIPStreamedConnection::handleRead() detects compound message frames");
        int tailIteration = 0;
        while (tail < end && tailIteration < 10)
        {
          tailIteration++;
          /// Reset the SIP Message
          _pRequest.reset(new SIPMessage());

          ret = _pRequest->consume(tail, end);
          result = ret.get<0>();
          tail = ret.get<1>();
          if (result)
          {
            OSS_LOG_DEBUG("SIPStreamedConnection::handleRead() parsed " << tailIteration << " more SIP Request.");
            dispatchMessage(_pRequest->shared_from_this(), shared_from_this());
            continue;
          }
          else if (!boost::indeterminate(result))
          {
            //
            // The message is not parsed correctly
            //
            OSS_LOG_WARNING("SIPStreamedConnection::handleRead() is not able to parse segment " << tailIteration);
            continue;
          }
          else
          {
            OSS_LOG_WARNING("SIPStreamedConnection::handleRead() has marked segment " << tailIteration << " as partial request.")
            //
            // The message has been parsed but it is marked as indeterminate.
            // We will not reset the buffer but instead continue the next read operation
            //
            readSome();
            //
            // We are done
            //
            return;
          }
        }
        
        _pRequest.reset();
        readSome();
      }
    }
    else if (!result)
    {
      _pRequest.reset();
      readSome();
    }
    else
    {
      //
      // We read a partial message.
      // read again
      //
      if (_pRequest->idleBuffer() == "\r\n\r\n")
      {
        /// We received a PING, send a PONG
        std::string pong("\r\n");
        boost::system::error_code ec;
        writeMessage(pong, ec);
       
        if (ec)
        {
          OSS_LOG_WARNING("SIPStreamedConnection::handleRead() Keep-Alive Exception - " << ec.message());
          boost::system::error_code ignored_ec;
          _pTcpSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
          _connectionManager.stop(shared_from_this());
          return;
        }
        _pRequest.reset();
      }

      readSome();
      return;
    }
  }
  else
  {
    OSS_LOG_WARNING("SIPStreamedConnection::handleRead() Exception " << e.message());
    if (++_readExceptionCount < 5 && e != boost::asio::error::eof)
    {
      //
      // Try reading again until exception reaches 5 iterations
      //
      
      readSome();
    }
    else
    {
      reportConnectionError(SIPStreamedConnection::CONNECTION_ERROR_READ, e);
      
      OSS_LOG_ERROR("SIPStreamedConnection::handleRead has reached maximum exception count or error is final.  Bailing out.");
      boost::system::error_code ignored_ec;
      _pTcpSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
      _connectionManager.stop(shared_from_this());
    }
  }
}

void SIPStreamedConnection::handleWrite(const boost::system::error_code& e)
{
  if (e)
  {
    reportConnectionError(SIPStreamedConnection::CONNECTION_ERROR_WRITE, e);
    // Initiate graceful connection closure.
    OSS_LOG_WARNING("SIPStreamedConnection::handleWrite() Exception " << e.message());
    boost::system::error_code ignored_ec;
    _pTcpSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    _connectionManager.stop(shared_from_this());
  }

}

void SIPStreamedConnection::writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port)
{
  //
  // This is connection oriented so ignore the remote target
  //
  writeMessage(msg);
}

void SIPStreamedConnection::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter)
{
  if (!e && _isClient)
  {
    if (!_pTlsStream)
    {
      _connectionManager.start(shared_from_this());
    }
    else
    {
      _pTlsStream->async_handshake(boost::asio::ssl::stream_base::client,
          boost::bind(&SIPStreamedConnection::handleClientHandshake, shared_from_this(),
            boost::asio::placeholders::error));
    }
  }
  else
  {
    reportConnectionError(SIPStreamedConnection::CONNECTION_ERROR_CONNECT, e);
    OSS_LOG_WARNING("SIPStreamedConnection::handleConnect() Exception " << e.message());
    socket().close();
  }
}

void SIPStreamedConnection::handleServerHandshake(const boost::system::error_code& e)
{
  if (!e)
  {
    //
    // start reading from the connection
    //
    readSome();
  }
  else
  {
    reportConnectionError(SIPStreamedConnection::CONNECTION_ERROR_SERVER_HANDSHAKE, e);
    
    OSS_LOG_ERROR("SIPStreamedConnection::handleServerHandshake() Exception " << e.message() << " - " << ERR_GET_REASON(e.value()));
   
    std::string err = e.message();
    if (e.category() == boost::asio::error::get_ssl_category()) 
    {
        err = std::string(" (")
                +boost::lexical_cast<std::string>(ERR_GET_LIB(e.value()))+","
                +boost::lexical_cast<std::string>(ERR_GET_FUNC(e.value()))+","
                +boost::lexical_cast<std::string>(ERR_GET_REASON(e.value()))+") "
        ;
        //ERR_PACK /* crypto/err/err.h */
        char buf[128];
        ::ERR_error_string_n(e.value(), buf, sizeof(buf));
        err += buf;
        
        OSS_LOG_ERROR("SIPStreamedConnection::handleServerHandshake() Exception " << err);
    }
    
    socket().close();
  }
}

void SIPStreamedConnection::handleClientHandshake(const boost::system::error_code& e)
{
  // this is only significant for TLS
  if (!e && _isClient)
  {
    assert(_pTlsStream);
    _connectionManager.start(shared_from_this());
  }
  else
  {
    reportConnectionError(SIPStreamedConnection::CONNECTION_ERROR_CLIENT_HANDSHAKE, e);
    OSS_LOG_ERROR("SIPStreamedConnection::handleClientHandshake() Exception " << e.message() << " - " << ERR_GET_REASON(e.value()));
    socket().close();
  }
}

OSS::Net::IPAddress SIPStreamedConnection::getLocalAddress() const
{
  if (_localAddress.isValid())
    return _localAddress;

  if (_pTcpSocket->is_open())
  {
    boost::system::error_code ec;
    EndPoint ep = _pTcpSocket->local_endpoint(ec);
    if (!ec)
    {
      boost::asio::ip::address ip = ep.address();
      _localAddress = OSS::Net::IPAddress(ip.to_string(), ep.port());
      return _localAddress;
    }
    else
    {
      OSS_LOG_WARNING("SIPStreamedConnection::getLocalAddress() Exception " << ec.message());
    }
  }

  return OSS::Net::IPAddress();
}

OSS::Net::IPAddress SIPStreamedConnection::getRemoteAddress() const
{
  if (_lastReadAddress.isValid())
    return _lastReadAddress;

  if (_pTcpSocket->is_open())
  {

      boost::system::error_code ec;
      EndPoint ep = _pTcpSocket->remote_endpoint(ec);
      if (!ec)
      {
        boost::asio::ip::address ip = ep.address();
        _lastReadAddress = OSS::Net::IPAddress(ip.to_string(), ep.port());
        return _lastReadAddress;
      }
      else
      {
        OSS_LOG_WARNING("SIPStreamedConnection::getRemoteAddress() Exception " << ec.message());
        return _connectAddress;
      }
  }
  return OSS::Net::IPAddress();
}

void SIPStreamedConnection::clientBind(const OSS::Net::IPAddress& listener, unsigned short portBase, unsigned short portMax)
{
  if (!_pTcpSocket->is_open())
  {
    bool isBound = false;
    unsigned short port = portBase;
    _pTcpSocket->open(const_cast<OSS::Net::IPAddress&>(listener).address().is_v4() ? boost::asio::ip::tcp::v4() : boost::asio::ip::tcp::v6());
    while(!isBound)
    {    
      boost::asio::ip::tcp::endpoint ep(const_cast<OSS::Net::IPAddress&>(listener).address(), port);
      boost::system::error_code ec;
      _pTcpSocket->bind(ep, ec);
      isBound = !ec;
      port++;
      if (port >= portMax)
      {
        _localAddress = listener;
        _localAddress.setPort(port);
        break;
      }
    }
  }
}

void SIPStreamedConnection::clientConnect(const OSS::Net::IPAddress& target)
{
  _isClient = true;
  
  if (_pTcpSocket->is_open())
  {
    _connectAddress = target;
    std::string port = OSS::string_from_number<unsigned short>(target.getPort());
    boost::asio::ip::tcp::resolver::iterator ep;
    boost::asio::ip::address addr = const_cast<OSS::Net::IPAddress&>(target).address();
    boost::asio::ip::tcp::resolver::query
    query(addr.is_v4() ? boost::asio::ip::tcp::v4() : boost::asio::ip::tcp::v6(),
      addr.to_string(), port == "0" || port.empty() ? "5060" : port);
    ep = _resolver.resolve(query);
    _pTcpSocket->async_connect(*ep, boost::bind(&SIPStreamedConnection::handleConnect, shared_from_this(),
      boost::asio::placeholders::error, ep));
  }
}




} } // OSS::SIP

