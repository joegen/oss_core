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
#include "OSS/SIP/SIPListener.h"


namespace OSS {
namespace SIP {
  

static const int DEFAULT_STREAMED_CONNECTION_TIMEOUT = 2; // 2 seconds default timeout
  

SIPStreamedConnection::SIPStreamedConnection(
  boost::asio::io_service& ioService,
  SIPStreamedConnectionManager& manager,
  SIPListener* pListener) :
    SIPTransportSession(pListener),
    _ioService(ioService),
    _pTcpSocket(0),
    _pTlsContext(0),
    _deadline(ioService),
    _pTlsStream(0),
    _resolver(ioService),
    _connectionManager(manager),
    _pDispatch(0),
    _readExceptionCount(0),
    _isStopping(false)
{
  _transportScheme = "tcp";
  _pTcpSocket = new boost::asio::ip::tcp::socket(ioService);
  _isClient = false;
  _isClientStarted = false;
}

SIPStreamedConnection::SIPStreamedConnection(
  boost::asio::io_service& ioService,
  boost::asio::ssl::context* pTlsContext,
  SIPStreamedConnectionManager& manager,
  SIPListener* pListener) :
    SIPTransportSession(pListener),
    _ioService(ioService),
    _pTcpSocket(0),
    _pTlsContext(pTlsContext),
    _deadline(ioService),
    _pTlsStream(0),
    _resolver(ioService),
    _connectionManager(manager),
    _pDispatch(0),
    _readExceptionCount(0),
    _isStopping(false)
{
  _transportScheme = "tls";
  _pTlsStream = new ssl_socket(ioService, *_pTlsContext);
  _pTcpSocket = &_pTlsStream->next_layer();
  _isClient = false;
  _isClientStarted = false;
  
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
  _isConnected = true; 
  
  if (_isClient)
  {
    OSS::mutex_critic_sec_lock lock(_pendingMutex);
    _isClientStarted = true;
    //
    // Start is called after a successful client handshake with the server.
    // we can start reading from the secure stream at this point 
    //  
    readSome();
    while (!_pending.empty())
    {
      SIPMessage::Ptr pPending = _pending.front();
      _pending.pop();
      writeMessage(pPending->data());
    }
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
  if (_isStopping || (_isClient && !_isClientStarted))
  {
    return;
  }
  
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
  if (_isStopping || (_isClient && !_isClientStarted))
  {
    return;
  }
  
  if (_pTlsStream)
  {
    ssl_socket& sock = *_pTlsStream;
    boost::asio::async_write(sock, boost::asio::buffer(buf, buf.size()),
          boost::bind(&SIPStreamedConnection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  }
  else if (_pTcpSocket)
  {
    tcp_socket& sock = *_pTcpSocket;
    boost::asio::async_write(sock, boost::asio::buffer(buf, buf.size()),
          boost::bind(&SIPStreamedConnection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  }
}

void SIPStreamedConnection::writeMessage(const std::string& buf, boost::system::error_code& ec)
{
  if (_isStopping || (_isClient && !_isClientStarted))
  {
    return;
  }
  
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
  if (_isStopping)
  {
    return;
  }
  
  {
    OSS::mutex_critic_sec_lock lock(_pendingMutex);
    if (_isClient && !_isClientStarted)
    {
      if (!_pending.empty())
      {
        OSS_LOG_WARNING("SIPStreamedConnection::writeMessage - discarding message because there is a pending message in queue");
        return;
      }
      OSS_LOG_INFO("SIPStreamedConnection::writeMessage - delaying sending of SIP Request " << msg->startLine());
      _pending.push(msg);
      return;
    }
  }
  writeMessage(msg->data());
}

bool SIPStreamedConnection::writeKeepAlive()
{
  if (_isStopping)
  {
    return false;
  }
  
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
    _isStopping = true;
    _connectionManager.stop(shared_from_this());
  }

  return !ec;
}

void SIPStreamedConnection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE /*userData*/)
{
  if (_isStopping)
  {
    return;
  }
  
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
        _lastReadAddress = OSS::Net::IPAddress(ip.to_string(), ep.port(), _pTlsContext ? OSS::Net::IPAddress::TLS : OSS::Net::IPAddress::TCP);
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
      if (rateLimit().isBannedAddress(_lastReadAddress.address()))
      {
        OSS_LOG_DEBUG("ALERT: Dropping " << _pRequest->data().size() << " bytes from blocked address "
          << _lastReadAddress.address().to_string());
      }
      else
      {
        dispatchMessage(_pRequest->shared_from_this(), shared_from_this());
        rateLimit().logPacket(_lastReadAddress.address(), _pRequest->data().size());
      }
      
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
          _isStopping = true;
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
    if (++_readExceptionCount < 5 && e != boost::asio::error::eof)
    {
      //
      // Try reading again until exception reaches 5 iterations
      //
      readSome();
    }
    else
    {
      OSS_LOG_WARNING("SIPStreamedConnection::handleRead() Exception " << e.message());
      boost::system::error_code ignored_ec;
      _pTcpSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
      _isStopping = true;
      _connectionManager.stop(shared_from_this());
    }
  }
}

void SIPStreamedConnection::handleWrite(const boost::system::error_code& e, std::size_t bytes_transferred)
{
  if (_isStopping)
  {
    return;
  }
  
  if (e)
  {
    // Initiate graceful connection closure.
    OSS_LOG_WARNING("SIPStreamedConnection::handleWrite() Exception " << e.message());
    boost::system::error_code ignored_ec;
    _pTcpSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    _isStopping = true;
    _connectionManager.stop(shared_from_this());
  }
}

void SIPStreamedConnection::writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port)
{
  if (_isStopping)
  {
    return;
  }
  //
  // This is connection oriented so ignore the remote target
  //
  writeMessage(msg);
}


void SIPStreamedConnection::handleConnectTimeout(const boost::system::error_code& e)
{
  if (!e)
  {
    // The deadline has passed. The socket is closed so that any outstanding
    // asynchronous operations are cancelled. This allows the blocked
    // connect(), read() or write() functions to return.
    boost::system::error_code ignored_ec;
    socket().cancel(ignored_ec);
    OSS_LOG_DEBUG("SIPStreamedConnection::handleConnectTimeout - Socket I/O TIMEOUT");
  }
}


bool SIPStreamedConnection::clientConnect(const OSS::Net::IPAddress& target)
{
  return clientConnect(target, boost::posix_time::seconds(DEFAULT_STREAMED_CONNECTION_TIMEOUT));
}

bool SIPStreamedConnection::clientConnect(const OSS::Net::IPAddress& target, boost::posix_time::time_duration timeout)
{
  _isClient = true;
  
  if (_pTcpSocket->is_open())
  {
    boost::system::error_code ec;
    _connectAddress = target;
    _connectAddress.setProtocol(!_pTlsStream ? OSS::Net::IPAddress::TCP : OSS::Net::IPAddress::TLS);
    std::string port = OSS::string_from_number<unsigned short>(target.getPort());
    boost::asio::ip::tcp::resolver::iterator ep;
    boost::asio::ip::address addr = const_cast<OSS::Net::IPAddress&>(target).address();
    boost::asio::ip::tcp::resolver::query
    query(addr.is_v4() ? boost::asio::ip::tcp::v4() : boost::asio::ip::tcp::v6(),
      addr.to_string(), port == "0" || port.empty() ? "5060" : port);
    ep = _resolver.resolve(query, ec);
    
    if (ec)
    {
      OSS_LOG_ERROR( "SIPStreamedConnection::clientConnect " << boost::diagnostic_information(ec));
      return false;
    }
    
    boost::asio::ip::tcp::endpoint endpoint(addr, target.getPort() == 0 ? 5060 : target.getPort());
    
    // Set a deadline for the asynchronous operation. As a host name may
    // resolve to multiple endpoints, this function uses the composed operation
    // async_connect. The deadline applies to the entire operation, rather than
    // individual connection attempts.
    _deadline.expires_from_now(timeout);
    
    _pTcpSocket->async_connect(*ep, boost::bind(&SIPStreamedConnection::handleConnect, shared_from_this(), boost::asio::placeholders::error, ep, (boost::system::error_code*)0, (Semaphore*)0));
    _deadline.async_wait(boost::bind(&SIPStreamedConnection::handleConnectTimeout, shared_from_this(), boost::asio::placeholders::error));
  }
  else
  {
    OSS_LOG_ERROR("SIPStreamedConnection::clientConnect() INVOKED while socket is closed ");
    _isConnected = false;
    return false;
  }
  return true;
}


void SIPStreamedConnection::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter, boost::system::error_code* out_ec, Semaphore* pSem)
{
  _deadline.cancel();
  
  if (!e && _isClient)
  {
    boost::system::error_code ignore_ec;
    _deadline.cancel(ignore_ec);
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
    _isConnected = false;
    OSS_LOG_WARNING("SIPStreamedConnection::handleConnect() Exception " << e.message());
    socket().close();
  }
  
  if (out_ec)
  {
    *out_ec = e;
  }
  
  if (pSem)
  {
    pSem->signal();
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
    _isStopping = true;
    _connectionManager.stop(shared_from_this());
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
    OSS_LOG_ERROR("SIPStreamedConnection::handleClientHandshake() Exception " << e.message() << " - " << ERR_GET_REASON(e.value()));
    socket().close();
    _isStopping = true;
    _connectionManager.stop(shared_from_this());
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
      _localAddress = OSS::Net::IPAddress(ip.to_string(), ep.port(), _pTlsContext ? OSS::Net::IPAddress::TLS : OSS::Net::IPAddress::TCP);
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
  {
    return _lastReadAddress;
  }
  else if (_pTcpSocket->is_open())
  {

      boost::system::error_code ec;
      EndPoint ep = _pTcpSocket->remote_endpoint(ec);
      if (!ec)
      {
        boost::asio::ip::address ip = ep.address();
        _lastReadAddress = OSS::Net::IPAddress(ip.to_string(), ep.port(), _pTlsContext ? OSS::Net::IPAddress::TLS : OSS::Net::IPAddress::TCP);
        return _lastReadAddress;
      }
      else
      {
        OSS_LOG_WARNING("SIPStreamedConnection::getRemoteAddress() Exception " << ec.message() << ". Using connect address " << _connectAddress.toIpPortString());
        return _connectAddress;
      }
  }
  else if (_connectAddress.isValid())
  {
    return _connectAddress;
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






} } // OSS::SIP

