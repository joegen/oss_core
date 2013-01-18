// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
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

#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPTLSConnection.h"
#include "OSS/SIP/SIPTLSConnectionManager.h"
#include "OSS/SIP/SIPFSMDispatch.h"


namespace OSS {
namespace SIP {


SIPTLSConnection::SIPTLSConnection(
  boost::asio::io_service& ioService,
  boost::asio::ssl::context& tlsContext,
  SIPTLSConnectionManager& manager): 
    _tlsContext(tlsContext),
    _socket(ioService, _tlsContext),
    _connectionManager(manager),
    _pRequest(0)
{
  _transportScheme = "tls";
}

void SIPTLSConnection::start(SIPFSMDispatch* pDispatch)
{
  _pDispatch = pDispatch;
  _socket.async_read_some(boost::asio::buffer(_buffer),
      boost::bind(&SIPTLSConnection::handleRead, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred, (void*)0));
}

void SIPTLSConnection::stop()
{
  _socket.lowest_layer().close();
}

void SIPTLSConnection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData)
{
  if (!e)
  {
    if (_pRequest == 0)
      _pRequest = new SIPMessage();

    boost::tribool result;
    boost::tie(result, boost::tuples::ignore) = 
      _pRequest->consume(_buffer.data(), _buffer.data() + bytes_transferred);

    if (result)
    {
      _pDispatch->onReceivedMessage(_pRequest->shared_from_this(), shared_from_this());
    }
    else if (!result)
    {
      delete _pRequest;
      _pRequest = 0;
    }
    else
    {
      _socket.async_read_some(boost::asio::buffer(_buffer),
          boost::bind(&SIPTLSConnection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred, (void*)0));
    }
  }
  else if (e != boost::asio::error::operation_aborted)
  {
    _connectionManager.stop(shared_from_this());
  }
}

void SIPTLSConnection::handleWrite(const boost::system::error_code& e)
{
  if (!e)
  {
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    _socket.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }

  if (e != boost::asio::error::operation_aborted)
  {
    _connectionManager.stop(shared_from_this());
  }
}

void SIPTLSConnection::writeMessage(SIPMessage::Ptr msg)
{
  boost::asio::async_write(_socket, boost::asio::buffer(msg->data(), msg->data().size()),
          boost::bind(&SIPTLSConnection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error));
}

void SIPTLSConnection::writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port)
{
  // This is only used by UDP connection
}

void SIPTLSConnection::clientBind(const OSS::IPAddress& listener, unsigned short portBase, unsigned short portMax)
{
  // Implement me!!
}

void SIPTLSConnection::clientConnect(const OSS::IPAddress& target)
{
  // Implement me!!
}

void SIPTLSConnection::handleResolve(boost::asio::ip::tcp::resolver::iterator endPointIter)
{
  _socket.lowest_layer().async_connect(endPointIter->endpoint(),
                        boost::bind(&SIPTLSConnection::handleConnect, shared_from_this(),
                        boost::asio::placeholders::error, endPointIter));
}

void SIPTLSConnection::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter)
{
  if (!e)
  {
    _socket.async_handshake(boost::asio::ssl::stream_base::client,
          boost::bind(&SIPTLSConnection::handleHandshake, shared_from_this(),
            boost::asio::placeholders::error));
  }
}

void SIPTLSConnection::handleHandshake(const boost::system::error_code& e)
{
  if (!e)
  {
    _socket.async_read_some(boost::asio::buffer(_buffer),
        boost::bind(&SIPTLSConnection::handleRead, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred, (void*)0));
  }
}

OSS::IPAddress SIPTLSConnection::getLocalAddress() const
{
  boost::asio::ip::address ip = _socket.lowest_layer().local_endpoint().address();
  return OSS::IPAddress(ip.to_string(), _socket.lowest_layer().local_endpoint().port());
}

OSS::IPAddress SIPTLSConnection::getRemoteAddress() const
{
  boost::asio::ip::address ip = _socket.lowest_layer().remote_endpoint().address();
  return OSS::IPAddress(ip.to_string(), _socket.lowest_layer().local_endpoint().port());
}

} } // OSS::SIP

