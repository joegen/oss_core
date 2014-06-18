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


#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "OSS/Logger.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPTCPConnection.h"
#include "OSS/SIP/SIPTCPConnectionManager.h"
#include "OSS/SIP/SIPFSMDispatch.h"


namespace OSS {
namespace SIP {


SIPTCPConnection::SIPTCPConnection(
  boost::asio::io_service& ioService,
  SIPTCPConnectionManager& manager) :
    _socket(ioService),
    _resolver(ioService),
    _connectionManager(manager),
  _pDispatch(0),
  _readExceptionCount(0)
{
  _transportScheme = "tcp";
}

SIPTCPConnection::~SIPTCPConnection()
{
}
    /// Destroys the TCP connection

void SIPTCPConnection::start(SIPFSMDispatch* pDispatch)
{
  _pDispatch = pDispatch;
  _socket.async_read_some(boost::asio::buffer(_buffer),
      boost::bind(&SIPTCPConnection::handleRead, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred, (void*)0));
}

void SIPTCPConnection::stop()
{
  OSS_LOG_WARNING("SIPTCPConnection stopped reading from transport (" << getIdentifier() << ") " << getLocalAddress().toIpPortString() <<
    "->" << getRemoteAddress().toIpPortString() );
  _socket.close();
}

void SIPTCPConnection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE /*userData*/)
{
  if (!e && bytes_transferred)
  {
    //
    // set the last read address
    //
    if (!_lastReadAddress.isValid())
    {
      boost::system::error_code ec;
      EndPoint ep = _socket.remote_endpoint(ec);
      if (!ec)
      {
        boost::asio::ip::address ip = ep.address();
        _lastReadAddress = OSS::IPAddress(ip.to_string(), ep.port());
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
      _pDispatch->onReceivedMessage(_pRequest->shared_from_this(), shared_from_this());
      if (tail >= end)
      {
        //
        // The end of the SIPMessage is reached so we can simply reset the
        // request buffer and start the read operation all over again
        //
        _pRequest.reset();
        _socket.async_read_some(boost::asio::buffer(_buffer),
            boost::bind(&SIPTCPConnection::handleRead, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred, (void*)0));
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
        OSS_LOG_DEBUG("SIPTCPConnection::handleRead() detects compound message frames");
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
            OSS_LOG_DEBUG("SIPTCPConnection::handleRead() parsed " << tailIteration << " more SIP Request.");
            _pDispatch->onReceivedMessage(_pRequest->shared_from_this(), shared_from_this());
            continue;
          }
          else if (!boost::indeterminate(result))
          {
            //
            // The message is not parsed correctly
            //
            OSS_LOG_WARNING("SIPTCPConnection::handleRead() is not able to parse segment " << tailIteration);
            continue;
          }
          else
          {
            OSS_LOG_WARNING("SIPTCPConnection::handleRead() has marked segment " << tailIteration << " as partial request.")
            //
            // The message has been parsed but it is marked as indeterminate.
            // We will not reset the buffer but instead continue the next read operation
            //
            _socket.async_read_some(boost::asio::buffer(_buffer),
            boost::bind(&SIPTCPConnection::handleRead, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred, (void*)0));
            //
            // We are done
            //
            return;
          }
        }
        _pRequest.reset();
        _socket.async_read_some(boost::asio::buffer(_buffer),
            boost::bind(&SIPTCPConnection::handleRead, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred, (void*)0));
      }
    }
    else if (!result)
    {
      _pRequest.reset();
      _socket.async_read_some(boost::asio::buffer(_buffer),
          boost::bind(&SIPTCPConnection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred, (void*)0));
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
        _socket.write_some(boost::asio::buffer(pong.c_str(), pong.size()), ec);

        if (ec)
        {
          OSS_LOG_WARNING("SIPTCPConnection::handleRead() Keep-Alive Exception - " << ec.message());
          boost::system::error_code ignored_ec;
          _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
          _connectionManager.stop(shared_from_this());
          return;
        }
        _pRequest.reset();
      }

      _socket.async_read_some(boost::asio::buffer(_buffer),
          boost::bind(&SIPTCPConnection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred, (void*)0));

      return;
    }
  }
  else
  {

    OSS_LOG_WARNING("SIPTCPConnection::handleRead() Exception " << e.message());
    if (++_readExceptionCount < 5)
    {
      //
      // Try reading again until exception reaches 5 iterations
      //
      _socket.async_read_some(boost::asio::buffer(_buffer),
          boost::bind(&SIPTCPConnection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred, (void*)0));
    }
    else
    {
      OSS_LOG_ERROR("SIPTCPConnection::handleRead has reached maximum exception count.  Bailing out.");
      boost::system::error_code ignored_ec;
      _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
      _connectionManager.stop(shared_from_this());
    }
  }
}

void SIPTCPConnection::handleWrite(const boost::system::error_code& e)
{
  if (e)
  {
    // Initiate graceful connection closure.
    OSS_LOG_WARNING("SIPTCPConnection::handleWrite() Exception " << e.message());
    boost::system::error_code ignored_ec;
    _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    _connectionManager.stop(shared_from_this());
  }

}

void SIPTCPConnection::writeMessage(SIPMessage::Ptr msg)
{
  boost::asio::async_write(_socket, boost::asio::buffer(msg->data(), msg->data().size()),
          boost::bind(&SIPTCPConnection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error));
}

bool SIPTCPConnection::writeKeepAlive()
{
  if (!_socket.is_open())
    return false;

  std::string keepAlive("\r\n\r\n");
  boost::system::error_code ec;
  bool ok = _socket.write_some(boost::asio::buffer(keepAlive, keepAlive.size()), ec) > 0;

  if (ec)
  {
    OSS_LOG_WARNING("SIPTCPConnection::writeKeepAlive() Exception - " << ec.message());
    boost::system::error_code ignored_ec;
    _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    _connectionManager.stop(shared_from_this());
  }

  return ok;
}

void SIPTCPConnection::writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port)
{
  //
  // This is connection oriented so ignore the remote target
  //
  writeMessage(msg);
}

void SIPTCPConnection::handleResolve(boost::asio::ip::tcp::resolver::iterator endPointIter)
{
  _socket.async_connect(endPointIter->endpoint(),
                        boost::bind(&SIPTCPConnection::handleConnect, shared_from_this(),
                        boost::asio::placeholders::error, endPointIter));
}

void SIPTCPConnection::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter)
{
  if (!e)
  {
    _connectionManager.start(shared_from_this());
  }
  else
  {
    OSS_LOG_WARNING("SIPTCPConnection::handleConnect() Exception " << e.message());
  }
}

void SIPTCPConnection::handleHandshake(const boost::system::error_code& error)
{
  // this is only significant for TLS
}

OSS::IPAddress SIPTCPConnection::getLocalAddress() const
{
  if (_localAddress.isValid())
    return _localAddress;

  if (_socket.is_open())
  {
    boost::system::error_code ec;
    EndPoint ep = _socket.local_endpoint(ec);
    if (!ec)
    {
      boost::asio::ip::address ip = ep.address();
      _localAddress = OSS::IPAddress(ip.to_string(), ep.port());
      return _localAddress;
    }
    else
    {
      OSS_LOG_WARNING("SIPTCPConnection::getLocalAddress() Exception " << ec.message());
    }
  }

  return OSS::IPAddress();
}

OSS::IPAddress SIPTCPConnection::getRemoteAddress() const
{
  if (_lastReadAddress.isValid())
    return _lastReadAddress;

  if (_socket.is_open())
  {

      boost::system::error_code ec;
      EndPoint ep = _socket.remote_endpoint(ec);
      if (!ec)
      {
        boost::asio::ip::address ip = ep.address();
        _lastReadAddress = OSS::IPAddress(ip.to_string(), ep.port());
        return _lastReadAddress;
      }
      else
      {
        OSS_LOG_WARNING("SIPTCPConnection::getRemoteAddress() Exception " << ec.message());
        return _connectAddress;
      }
  }
  return OSS::IPAddress();
}

void SIPTCPConnection::clientBind(const OSS::IPAddress& listener, unsigned short portBase, unsigned short portMax)
{
  if (!_socket.is_open())
  {
    bool isBound = false;
    unsigned short port = portBase;
    _socket.open(const_cast<OSS::IPAddress&>(listener).address().is_v4() ? boost::asio::ip::tcp::v4() : boost::asio::ip::tcp::v6());
    while(!isBound)
    {    
      boost::asio::ip::tcp::endpoint ep(const_cast<OSS::IPAddress&>(listener).address(), port);
      boost::system::error_code ec;
      _socket.bind(ep, ec);
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

void SIPTCPConnection::clientConnect(const OSS::IPAddress& target)
{
  if (_socket.is_open())
  {
    _connectAddress = target;
    std::string port = OSS::string_from_number<unsigned short>(target.getPort());
    boost::asio::ip::tcp::resolver::iterator ep;
    boost::asio::ip::address addr = const_cast<OSS::IPAddress&>(target).address();
    boost::asio::ip::tcp::resolver::query
    query(addr.is_v4() ? boost::asio::ip::tcp::v4() : boost::asio::ip::tcp::v6(),
      addr.to_string(), port == "0" || port.empty() ? "5060" : port);
    ep = _resolver.resolve(query);
    _socket.async_connect(*ep, boost::bind(&SIPTCPConnection::handleConnect, shared_from_this(),
      boost::asio::placeholders::error, ep));
  }
}




} } // OSS::SIP

