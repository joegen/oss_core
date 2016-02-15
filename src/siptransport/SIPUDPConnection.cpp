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

#include <iostream>
#include <vector>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPUDPConnection.h"
#include "OSS/SIP/SIPUDPConnectionClone.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/SIP/SIPXOR.h"
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/PropertyMap.h"
#include "OSS/SIP/SIPListener.h"


namespace OSS {
namespace SIP {


SIPUDPConnection::SIPUDPConnection(
  boost::asio::io_service& ioService,
  boost::asio::ip::udp::socket& socket,
  SIPListener* pListener) :
    SIPTransportSession(pListener),
    _socket(socket),
    _resolver(ioService),
    _pRequest()
{
  _isReliableTransport = false;
  _transportScheme = "udp";
}

SIPUDPConnection::~SIPUDPConnection()
{  
}

void SIPUDPConnection::start(const SIPTransportSession::Dispatch& dispatch)
{
  setMessageDispatch(dispatch);
  
  boost::asio::socket_base::receive_buffer_size recBuffSize(25165824);
  boost::asio::socket_base::send_buffer_size sendBuffSize(25165824);
  _socket.set_option(recBuffSize);
  _socket.set_option(sendBuffSize);
  _socket.async_receive_from(boost::asio::buffer(_buffer), _senderEndPoint,
      boost::bind(&SIPUDPConnection::handleRead, shared_from_this(),
        boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred, (void*)0));
}

static bool isSIPPacket(const char* p)
{
  if (p[0]=='A' && p[1]=='C' && p[2]=='K' && p[3]==' ') /* ACK */
    return true;
  else if(p[0]=='B' && p[1]=='Y' && p[2]=='E' && p[3]==' ') /* BYE */
    return true;
  else if (p[0]=='C' && p[1]=='A' && p[2]=='N' && p[3]=='C' ) /* CANCEL */
    return true;
  else if (p[0]=='I' && p[1]=='N' && p[2]=='F' && p[3]=='O' ) /* INFO */
    return true;
  else if (p[0]=='I' && p[1]=='N' && p[2]=='V' && p[3]=='I' ) /* INVITE */
    return true;
  else if (p[0]=='N' && p[1]=='O' && p[2]=='T' && p[3]=='I' ) /* NOTIFY */
    return true;
  else if (p[0]=='O' && p[1]=='P' && p[2]=='T' && p[3]=='I' ) /* OPTIONS */
    return true;
  else if (p[0]=='P' && p[1]=='R' && p[2]=='A' && p[3]=='C' ) /* PRACK */
    return true;
  else if (p[0]=='P' && p[1]=='U' && p[2]=='B' && p[3]=='L' ) /* PUBLISH */
    return true;
  else if (p[0]=='R' && p[1]=='E' && p[2]=='F' && p[3]=='E' ) /* REFER */
    return true;
  else if (p[0]=='R' && p[1]=='E' && p[2]=='G' && p[3]=='I' ) /* REGISTER */
    return true;
  else if (p[0]=='S' && p[1]=='U' && p[2]=='B' && p[3]=='S' ) /* SUBSCRIBE */
    return true;
  else if (p[0]=='U' && p[1]=='P' && p[2]=='D' && p[3]=='A' ) /* UPDATE */
    return true;
  else if (p[0]=='E' && p[1]=='X' && p[2]=='E' && p[3]=='C' ) /* EXEC */
    return true;
  else if (p[0]=='S' && p[1]=='I' && p[2]=='P' && p[3]=='/' ) /* RESPONSE */
    return true;

  return false;
}

void SIPUDPConnection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData)
{
  if (!e)
  {
    if (_pRequest == 0)
      _pRequest = SIPMessage::Ptr(new SIPMessage());

    _bytesRead =  bytes_transferred;
    if (_bytesRead > 20)
    {
      try
      {
        if (rateLimit().isBannedAddress(getRemoteAddress().address()))
        {
          OSS_LOG_DEBUG("ALERT: Dropping " << bytes_transferred << " bytes from blocked address "
            << getRemoteAddress().address().to_string());

          _pRequest.reset();

          if (_socket.is_open())
          {
            _socket.async_receive_from(boost::asio::buffer(_buffer), _senderEndPoint,
              boost::bind(&SIPUDPConnection::handleRead, shared_from_this(),
                boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred, (void*)0));
          }
          return;
        }

        rateLimit().logPacket(getRemoteAddress().address(), bytes_transferred);
      }
      catch(std::exception& e)
      {
        OSS_LOG_ERROR("Rate Limit Exception: " << e.what());
      }
      catch(...)
      {
        OSS_LOG_ERROR("Rate Limit Exception: Unknown exception.");
      }

      std::string buffer(_buffer.data(), _buffer.data() + bytes_transferred);

      if (!SIPXOR::isEnabled())
      {
        _pRequest->setData(buffer);
      }
      else if (isSIPPacket(buffer.c_str()))
      {
        _pRequest->setData(buffer);
      }
      else
      {
        SIPXOR::sipDecrypt(_buffer, bytes_transferred);
        buffer = std::string(_buffer.begin(), bytes_transferred);//(char*)&newBuff[0];
        if (!isSIPPacket(buffer.c_str()))
        {
          _pRequest.reset();
          if (_socket.is_open())
          {
            _socket.async_receive_from(boost::asio::buffer(_buffer), _senderEndPoint,
              boost::bind(&SIPUDPConnection::handleRead, shared_from_this(),
                boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred, (void*)0));
          }
          return;
        }
        _pRequest->setData(buffer);
        _pRequest->setProperty(OSS::PropertyMap::PROP_XOR, "1");
      }

      //
      // Clone the current connection so that the dispatcher gets a static snapshot
      // since the old connection will be reused by the transport for UDP
      //
      SIPUDPConnectionClone* clone = new SIPUDPConnectionClone(shared_from_this());
      SIPTransportSession::Ptr pClone(clone);
      dispatchMessage(_pRequest, pClone);
    }
    else if (_bytesRead == 4 &&
        (char)_buffer[0] == '\r' &&
        (char)_buffer[1] == '\n' &&
        (char)_buffer[2] == '\r' &&
        (char)_buffer[3] == '\n')
    {
      static std::string pong = "\r\n";
      //
      // This is a keep-alive
      //
      std::string sport = boost::lexical_cast<std::string>(getRemoteAddress().getPort());
      boost::asio::ip::udp::resolver::iterator ep;
      boost::asio::ip::address addr = getRemoteAddress().address();
      boost::asio::ip::udp::resolver::query query(addr.is_v4() ? boost::asio::ip::udp::v4()
        : boost::asio::ip::udp::v6(), addr.to_string(),  sport == "0" || sport.empty() ? "5060" : sport);
      ep = _resolver.resolve(query);
      _socket.async_send_to(boost::asio::buffer(pong.c_str(), pong.size()), *ep,
          boost::bind(&SIPUDPConnection::handleWrite, shared_from_this(),
                  boost::asio::placeholders::error));
    }
    
    _pRequest.reset();

    if (_socket.is_open())
    {
      _socket.async_receive_from(boost::asio::buffer(_buffer), _senderEndPoint,
        boost::bind(&SIPUDPConnection::handleRead, shared_from_this(),
          boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred, (void*)0));
    }
  }
}

void SIPUDPConnection::writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port)
{
  if (_socket.is_open())
  {
    boost::asio::ip::udp::resolver::iterator ep;
    boost::asio::ip::address addr = boost::asio::ip::address::from_string(ip);
    boost::asio::ip::udp::resolver::query
      query(addr.is_v4() ? boost::asio::ip::udp::v4() : boost::asio::ip::udp::v6(),
      addr.to_string(), port == "0" || port.empty() ? "5060" : port);
    ep = _resolver.resolve(query);

    if (!SIPXOR::isEnabled())
    {
      _socket.async_send_to(boost::asio::buffer(msg->data(), msg->data().size()), *ep,
        boost::bind(&SIPUDPConnection::handleWrite, shared_from_this(),
                boost::asio::placeholders::error));
    }else
    {
      std::string isXOR;
      if (!msg->getProperty(OSS::PropertyMap::PROP_XOR, isXOR) || isXOR != "1")
      {
        _socket.async_send_to(boost::asio::buffer(msg->data(), msg->data().size()), *ep,
        boost::bind(&SIPUDPConnection::handleWrite, shared_from_this(),
                boost::asio::placeholders::error));
      }
      else
      {
        boost::array<char, OSS_SIP_MAX_PACKET_SIZE> newBuff;
        for (size_t i = 0; i < msg->data().size(); i++)
          newBuff[i] = msg->data()[i];
        size_t len = msg->data().size();
        SIPXOR::sipEncrypt(newBuff, len);

#if 0
        _socket.async_send_to(boost::asio::buffer(newBuff, len), *ep,
        boost::bind(&SIPUDPConnection::handleWrite, shared_from_this(),
                boost::asio::placeholders::error));
#else
        boost::system::error_code ec;
        _socket.send_to(boost::asio::buffer(newBuff, len), *ep, 0, ec);
#endif
      }
    }
  }
}

bool SIPUDPConnection::writeKeepAlive(const std::string& ip, const std::string& port)
{
  if (_socket.is_open())
  {
    boost::asio::ip::udp::resolver::iterator ep;
    boost::asio::ip::address addr = boost::asio::ip::address::from_string(ip);
    boost::asio::ip::udp::resolver::query query(addr.is_v4() ? boost::asio::ip::udp::v4()
      : boost::asio::ip::udp::v6(), addr.to_string(), port == "0" || port.empty() ? "5060" : port);
    ep = _resolver.resolve(query);

#if 0
    _socket.async_send_to(boost::asio::buffer("\r\n\r\n", 4), *ep,
        boost::bind(&SIPUDPConnection::handleWrite, shared_from_this(),
                boost::asio::placeholders::error));
#else
    boost::system::error_code ec;
    _socket.send_to(boost::asio::buffer("\r\n\r\n", 4), *ep, 0, ec);
#endif
    return !ec;
  }
  return false;
}

void SIPUDPConnection::handleWrite(const boost::system::error_code& e)
{
  // This is only significant for stream based connections (TCP/TLS)
}

void SIPUDPConnection::stop()
{
  // This is only significant for stream based connections (TCP/TLS)
}

void SIPUDPConnection::writeMessage(SIPMessage::Ptr msg)
{
  // This is only significant for stream based connections (TCP/TLS)
  throw OSS::SIP::SIPException("Invalid UDP Transport Operation");
}

void SIPUDPConnection::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter, boost::system::error_code* out_ec, Semaphore* pSem)
{
  // This is only significant for stream based connections (TCP/TLS)
}

void SIPUDPConnection::handleClientHandshake(const boost::system::error_code& error)
{
  // This is only significant for stream based connections (TCP/TLS)
}

void SIPUDPConnection::handleServerHandshake(const boost::system::error_code& error)
{
  // this is only significant for TLS
	OSS_ASSERT(false);
}

void SIPUDPConnection::clientBind(const OSS::Net::IPAddress& listener, unsigned short portBase, unsigned short portMax)
{
 // This is only significant for stream based connections (TCP/TLS)
}

bool SIPUDPConnection::clientConnect(const OSS::Net::IPAddress& target)
{
 // This is only significant for stream based connections (TCP/TLS)
  return false;
}

OSS::Net::IPAddress SIPUDPConnection::getLocalAddress() const
{
  boost::asio::ip::address ip = _socket.local_endpoint().address();
  return OSS::Net::IPAddress(ip.to_string(), _socket.local_endpoint().port());
}

OSS::Net::IPAddress SIPUDPConnection::getRemoteAddress() const
{
   boost::asio::ip::address ip = _senderEndPoint.address();
  return OSS::Net::IPAddress(ip.to_string(), _senderEndPoint.port());
}

} } // OSS::SIP

