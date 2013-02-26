/*
 * Copyright (C) 2012  OSS Software Solutions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */


#include <boost/array.hpp>

#include "OSS/Core.h"
#include "OSS/Logger.h"
#include "OSS/RTP/RTPProxy.h"
#include "OSS/RTP/RTPProxyManager.h"
#include "OSS/RTP/RTPProxySession.h"
#include "OSS/SIP/SIPXOR.h"

namespace OSS {
namespace RTP {

RTPProxy::RTPProxy(Type type, RTPProxyManager* pManager, RTPProxySession* pSession, const std::string& identifier, bool isXORDisabled) :
  _identifier(identifier),
  _pManager(pManager),
  _pLeg1Socket(0),
  _pLeg2Socket(0),
  _leg1ReadTimer(_pManager->_ioService, boost::posix_time::milliseconds(_pManager->_readTimeout)),
  _leg2ReadTimer(_pManager->_ioService, boost::posix_time::milliseconds(_pManager->_readTimeout)),
  _adjustSenderFromPacketSource(true),
  _leg1Resizer(this, 1),
  _leg2Resizer(this, 2),
  _leg1Reset(false),
  _leg2Reset(false),
  _isStarted(false),
  _isInactive(false),
  _isLeg1XOREncrypted(false),
  _isLeg2XOREncrypted(false),
  _isXORDisabled(isXORDisabled),
  _pSession(pSession),
  _type(type),
  _isPooled(false)
{
}

RTPProxy::~RTPProxy()
{
  close();
}

bool RTPProxy::open(
  const OSS::IPAddress& leg1Listener,
  const OSS::IPAddress& leg2Listener)
{
  stop();

  _csSessionMutex.lock();
  _pLeg1Socket = new boost::asio::ip::udp::socket(_pManager->_ioService);
  _pLeg2Socket = new boost::asio::ip::udp::socket(_pManager->_ioService);
  _pLeg1Socket->open(boost::asio::ip::udp::v4());
  _pLeg2Socket->open(boost::asio::ip::udp::v4());

  {
  std::ostringstream logMsg;
  logMsg <<  _pSession->logId() << " RTP Opening sockets " <<  leg1Listener.toIpPortString() << "/"
    << leg2Listener.toIpPortString();
  OSS::log_debug(logMsg.str());
  }

  boost::asio::ip::address addr1 = const_cast<OSS::IPAddress&>(leg1Listener).address();
  _localEndPointLeg1 = boost::asio::ip::udp::endpoint(addr1, leg1Listener.getPort());
  _localEndPointLeg1External = const_cast<OSS::IPAddress&>(leg1Listener).externalAddress();
  boost::system::error_code ec;
  
  _pLeg1Socket->bind(_localEndPointLeg1, ec);
  if (!ec)
  {
    boost::asio::ip::address addr2 = const_cast<OSS::IPAddress&>(leg2Listener).address();
    _localEndPointLeg2 = boost::asio::ip::udp::endpoint(addr2, leg2Listener.getPort());
    _localEndPointLeg2External = const_cast<OSS::IPAddress&>(leg2Listener).externalAddress();
    _pLeg2Socket->bind(_localEndPointLeg2, ec);
    if (!ec)
    {
      _csSessionMutex.unlock();
      return true;
    }
  }
  _csSessionMutex.unlock();

  stop();

  return false;
}

void RTPProxy::start()
{
  OSS::mutex_critic_sec_lock lock(_csSessionMutex);
  if (!_pLeg1Socket || !_pLeg2Socket)
    throw RTPProxyException("Socket is NULL while calling RTPProxy::start");

  if (_isStarted)
  {
    resetLeg1();
    resetLeg2();
    return;
  }
  _pLeg1Socket->async_receive_from(boost::asio::buffer(_leg1Buffer), _senderEndPointLeg1,
    boost::bind(&RTPProxy::handleLeg1FrameRead, shared_from_this(),
      boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

  _leg1ReadTimer.async_wait(boost::bind(&RTPProxy::handleLeg1SocketReadTimeout, this, boost::asio::placeholders::error));

  _pLeg2Socket->async_receive_from(boost::asio::buffer(_leg2Buffer), _senderEndPointLeg2,
    boost::bind(&RTPProxy::handleLeg2FrameRead, shared_from_this(),
      boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

  _leg2ReadTimer.async_wait(boost::bind(&RTPProxy::handleLeg2SocketReadTimeout, this, boost::asio::placeholders::error));

  _isStarted = true;
}

void RTPProxy::stop()
{
  _csSessionMutex.lock();

  _leg1Resizer.stop();
  _leg2Resizer.stop();

  _csLeg1Mutex.lock();
  if (_pLeg1Socket)
    _pLeg1Socket->close();
  delete _pLeg1Socket;_pLeg1Socket=0;
  _csLeg1Mutex.unlock();

  _csLeg2Mutex.lock();
  if (_pLeg2Socket)
    _pLeg2Socket->close();
  delete _pLeg2Socket;_pLeg2Socket=0;
  _csLeg2Mutex.unlock();

  _isStarted = false;
  _csSessionMutex.unlock();
}

void RTPProxy::shutdown()
{
  boost::system::error_code e;
  _csLeg1Mutex.lock();
  if (_pLeg1Socket)
    _pLeg1Socket->shutdown(boost::asio::ip::udp::socket::shutdown_both, e);
  delete _pLeg1Socket;_pLeg1Socket=0;
  _csLeg1Mutex.unlock();

  _csLeg2Mutex.lock();
  if (_pLeg2Socket)
    _pLeg2Socket->shutdown(boost::asio::ip::udp::socket::shutdown_both, e);
  delete _pLeg2Socket;_pLeg2Socket=0;
  _csLeg2Mutex.unlock();

}
    /// Shutdown read and write operations and close the sockets


void RTPProxy::resetLeg1()
{
  _csLeg1Mutex.lock();
  _leg1Reset = true;
  _csLeg1Mutex.unlock();
}

void RTPProxy::resetLeg2()
{
  _csLeg2Mutex.lock();
  _leg2Reset = true;
  _csLeg2Mutex.unlock();
}


void RTPProxy::handleLeg1FrameRead(
  const boost::system::error_code& e,
  std::size_t bytes_transferred)
{
  if (!e && bytes_transferred >= 2)
  {
    _leg1ReadTimer.cancel();
    _isInactive = false;

    // _isLeg1XOREncrypted = ((_leg1Buffer[0]>>6)&3) != 2;
    _isLeg1XOREncrypted = OSS::SIP::SIPXOR::isEnabled() ? !validateBuffer(_leg1Buffer, bytes_transferred) : false;
    _csLeg2Mutex.lock();

    bool isResizing = _leg2Resizer.isEnabled() && _type == Data;

    if (_pLeg2Socket && _pLeg2Socket->is_open())
    {
      if (_senderEndPointLeg2.port() != 0)
      {
        if (!OSS::SIP::SIPXOR::isEnabled())
        {
          if (isResizing)
          {
            if (!_leg2Resizer.enqueue(_leg1Buffer, bytes_transferred))
            {
              isResizing = false;
              _pLeg2Socket->async_send_to(boost::asio::buffer(_leg1Buffer, bytes_transferred), _senderEndPointLeg2,
              boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                      boost::asio::placeholders::error));
            }
          }
          else
          {
            _pLeg2Socket->async_send_to(boost::asio::buffer(_leg1Buffer, bytes_transferred), _senderEndPointLeg2,
              boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                      boost::asio::placeholders::error));
          }
        }
        else
        {
          if (_isLeg1XOREncrypted && !_isLeg2XOREncrypted && !_isXORDisabled)
          {
            //std::cout << "sending to leg2 as unencrypted rtp" <<std::endl;
            OSS::SIP::SIPXOR::rtpDecrypt(_leg1Buffer, bytes_transferred);
            if (isResizing)
            {
              if (!_leg2Resizer.enqueue(_leg1Buffer, bytes_transferred))
              {
                isResizing = false;
                //
                // if it fails to queue up, send it immediately
                //
                _pLeg2Socket->async_send_to(boost::asio::buffer(_leg1Buffer, bytes_transferred), _senderEndPointLeg2,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
              }
            }
            else
            {
              _pLeg2Socket->async_send_to(boost::asio::buffer(_leg1Buffer, bytes_transferred), _senderEndPointLeg2,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
            }
          }
          else if (!_isLeg1XOREncrypted && _isLeg2XOREncrypted && !_isXORDisabled)
          {
            //std::cout << "sending to leg2 as encrypted rtp" <<std::endl;
            if (isResizing)
            {

              if (!_leg2Resizer.enqueue(_leg1Buffer, bytes_transferred))
              {
                isResizing = false;
                //
                // if it fails to queue up, send it immediately
                //
                OSS::SIP::SIPXOR::rtpEncrypt(_leg1Buffer, bytes_transferred);
                _pLeg2Socket->async_send_to(boost::asio::buffer(_leg1Buffer, bytes_transferred), _senderEndPointLeg2,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
              }
            }
            else
            {
              OSS::SIP::SIPXOR::rtpEncrypt(_leg1Buffer, bytes_transferred);
              _pLeg2Socket->async_send_to(boost::asio::buffer(_leg1Buffer, bytes_transferred), _senderEndPointLeg2,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
            }
          }
          else if (_isLeg1XOREncrypted && _isLeg2XOREncrypted && !_isXORDisabled)
          {
            //std::cout << "sending to leg2 as encrypted rtp" <<std::endl;
            OSS::SIP::SIPXOR::rtpDecrypt(_leg1Buffer, bytes_transferred);
            if (isResizing)
            {
              if (!_leg2Resizer.enqueue(_leg1Buffer, bytes_transferred))
              {
                isResizing = false;
                //
                // if it fails to queue up, send it immediately
                //
                OSS::SIP::SIPXOR::rtpEncrypt(_leg1Buffer, bytes_transferred);
                _pLeg2Socket->async_send_to(boost::asio::buffer(_leg1Buffer, bytes_transferred), _senderEndPointLeg2,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
              }
            }
            else
            {
              OSS::SIP::SIPXOR::rtpEncrypt(_leg1Buffer, bytes_transferred);
              _pLeg2Socket->async_send_to(boost::asio::buffer(_leg1Buffer, bytes_transferred), _senderEndPointLeg2,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
            }
          }
          else
          {
            //std::cout << "sending to leg2 as untouched rtp" <<std::endl;

            if (isResizing)
            {
              if (!_leg2Resizer.enqueue(_leg1Buffer, bytes_transferred))
              {
                isResizing = false;
                //
                // if it fails to queue up, send it immediately
                //
                _pLeg2Socket->async_send_to(boost::asio::buffer(_leg1Buffer, bytes_transferred), _senderEndPointLeg2,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
              }
            }
            else
            {
              _pLeg2Socket->async_send_to(boost::asio::buffer(_leg1Buffer, bytes_transferred), _senderEndPointLeg2,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
            }
          }
        }

        if (_pSession->verbose() && !isResizing)
        {
          try
          {
            std::ostringstream logMsg;
            logMsg  << _pSession->logId() << " RTP " << "BYTES=" << bytes_transferred
                    << " SRC (Leg1): " << _senderEndPointLeg1.address().to_string() << ":"
                    << _senderEndPointLeg1.port() << "/"
                    << _pLeg1Socket->local_endpoint().address().to_string() << ":"
                    << _pLeg1Socket->local_endpoint().port() << "/"
                    << "ENC=" << _isLeg1XOREncrypted << " >>> "
                    << "DST: " << _pLeg2Socket->local_endpoint().address().to_string()  << ":"
                    << _pLeg2Socket->local_endpoint().port() << "/"
                    << _senderEndPointLeg2.address().to_string() << ":"
                    << _senderEndPointLeg2.port() << "/"
                    << "ENC=" << _isLeg2XOREncrypted;
            OSS::log_information(logMsg.str());
          }
          catch(...)
          {
          }
        }
      }
    }
    _csLeg2Mutex.unlock();

    _csLeg1Mutex.lock();
    if (_pLeg1Socket && _pLeg1Socket->is_open())
    {
      if (_leg1Reset)
      {
        _leg1Reset = false;
        _pLeg1Socket->async_receive_from(boost::asio::buffer(_leg1Buffer), _lastSenderEndPointLeg1,
        boost::bind(&RTPProxy::handleLeg1FrameRead, shared_from_this(),
          boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

      }
      else
      {

        _pLeg1Socket->async_receive_from(boost::asio::buffer(_leg1Buffer), _senderEndPointLeg1,
        boost::bind(&RTPProxy::handleLeg1FrameRead, shared_from_this(),
          boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

      }

      _leg1ReadTimer.expires_from_now(boost::posix_time::milliseconds(_pManager->_readTimeout));
      _leg1ReadTimer.async_wait(boost::bind(&RTPProxy::handleLeg1SocketReadTimeout, this, boost::asio::placeholders::error));
    }
    _csLeg1Mutex.unlock();

    //
    // Process the resize buffer if we are resizing
    //
    processResizerQueue();
  }
}

void RTPProxy::handleLeg2FrameRead(
  const boost::system::error_code& e,
  std::size_t bytes_transferred)
{
  if (!e && bytes_transferred >= 2)
  {
    _leg2ReadTimer.cancel();
    _isInactive = false;

    //_isLeg2XOREncrypted = ((_leg2Buffer[0]>>6)&3) != 2;
    _isLeg2XOREncrypted = OSS::SIP::SIPXOR::isEnabled() ? !validateBuffer(_leg2Buffer, bytes_transferred) : false;

    bool isResizing = _leg1Resizer.isEnabled() && _type == Data;

    _csLeg1Mutex.lock();
    if (_pLeg1Socket && _pLeg1Socket->is_open())
    {
      if (_senderEndPointLeg1.port() != 0)
      {
        if (!OSS::SIP::SIPXOR::isEnabled())
        {
          if (isResizing)
          {
            if (!_leg1Resizer.enqueue(_leg2Buffer, bytes_transferred))
            {
              isResizing = false;
              //
              // if it fails to queue up, send it immediately
              //
              _pLeg1Socket->async_send_to(boost::asio::buffer(_leg2Buffer, bytes_transferred), _senderEndPointLeg1,
              boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                      boost::asio::placeholders::error));
            }
          }
          else
          {
            _pLeg1Socket->async_send_to(boost::asio::buffer(_leg2Buffer, bytes_transferred), _senderEndPointLeg1,
              boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                      boost::asio::placeholders::error));
          }
        }
        else
        {
          if (!_isLeg1XOREncrypted && _isLeg2XOREncrypted && !_isXORDisabled)
          {
            //
            // Leg 2 is XORed so decrypt it
            //
            OSS::SIP::SIPXOR::rtpDecrypt(_leg2Buffer, bytes_transferred);

            if (isResizing)
            {
              if (!_leg1Resizer.enqueue(_leg2Buffer, bytes_transferred))
              {
                isResizing = false;
                //
                // if it fails to queue up, send it immediately
                //
                _pLeg1Socket->async_send_to(boost::asio::buffer(_leg2Buffer, bytes_transferred), _senderEndPointLeg1,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
              }
            }
            else
            {
              _pLeg1Socket->async_send_to(boost::asio::buffer(_leg2Buffer, bytes_transferred), _senderEndPointLeg1,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
            }
          }
          else if (_isLeg1XOREncrypted && !_isLeg2XOREncrypted && !_isXORDisabled)
          {
            //std::cout << "sending to leg2 as encrypted rtp" <<std::endl;

            if (isResizing)
            {
              if (!_leg1Resizer.enqueue(_leg2Buffer, bytes_transferred))
              {
                isResizing = false;
                //
                // if it fails to queue up, send it immediately
                //
                OSS::SIP::SIPXOR::rtpEncrypt(_leg2Buffer, bytes_transferred);
                _pLeg1Socket->async_send_to(boost::asio::buffer(_leg2Buffer, bytes_transferred), _senderEndPointLeg1,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
              }
            }
            else
            {
              OSS::SIP::SIPXOR::rtpEncrypt(_leg2Buffer, bytes_transferred);
              _pLeg1Socket->async_send_to(boost::asio::buffer(_leg2Buffer, bytes_transferred), _senderEndPointLeg1,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
            }
          }
          else if (_isLeg1XOREncrypted && _isLeg2XOREncrypted && !_isXORDisabled)
          {
            //std::cout << "sending to leg2 as encrypted rtp" <<std::endl;
            OSS::SIP::SIPXOR::rtpDecrypt(_leg2Buffer, bytes_transferred);

            if (isResizing)
            {
              if (!_leg1Resizer.enqueue(_leg2Buffer, bytes_transferred))
              {
                isResizing = false;
                //
                // if it fails to queue up, send it immediately
                //
                OSS::SIP::SIPXOR::rtpEncrypt(_leg2Buffer, bytes_transferred);
                _pLeg1Socket->async_send_to(boost::asio::buffer(_leg2Buffer, bytes_transferred), _senderEndPointLeg1,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
              }
            }
            else
            {
              OSS::SIP::SIPXOR::rtpEncrypt(_leg2Buffer, bytes_transferred);
              _pLeg1Socket->async_send_to(boost::asio::buffer(_leg2Buffer, bytes_transferred), _senderEndPointLeg1,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
            }
          }
          else
          {
            //
            // Evrything else is just lrelayed
            //
            if (isResizing)
            {
              if (!_leg1Resizer.enqueue(_leg2Buffer, bytes_transferred))
              {
                isResizing = false;
                //
                // if it fails to queue up, send it immediately
                //
                _pLeg1Socket->async_send_to(boost::asio::buffer(_leg2Buffer, bytes_transferred), _senderEndPointLeg1,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
              }
            }
            else
            {
              _pLeg1Socket->async_send_to(boost::asio::buffer(_leg2Buffer, bytes_transferred), _senderEndPointLeg1,
                boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                        boost::asio::placeholders::error));
            }
          }
        }

        if (_pSession->verbose() && !isResizing)
        {
          try
          {
            std::ostringstream logMsg;
            logMsg  << _pSession->logId() << " RTP " << "BYTES=" << bytes_transferred
                    << " SRC (Leg2): " << _senderEndPointLeg2.address().to_string() << ":"
                    << _senderEndPointLeg2.port() << "/"
                    << _pLeg2Socket->local_endpoint().address().to_string() << ":"
                    << _pLeg2Socket->local_endpoint().port() << "/"
                    << "ENC=" << _isLeg2XOREncrypted << " >>> "
                    << "DST: " << _pLeg1Socket->local_endpoint().address().to_string() << ":"
                    << _pLeg1Socket->local_endpoint().port() << "/"
                    << _senderEndPointLeg1.address().to_string() << ":"
                    << _senderEndPointLeg1.port() << "/"
                    << "ENC=" << _isLeg1XOREncrypted;
            OSS::log_information(logMsg.str());
          }
          catch(...)
          {
          }
        }
      }
    }
    _csLeg1Mutex.unlock();

    _csLeg2Mutex.lock();
    if (_pLeg2Socket && _pLeg2Socket->is_open())
    {
      if (_leg2Reset)
      {
        _leg2Reset = false;
          _pLeg2Socket->async_receive_from(boost::asio::buffer(_leg2Buffer), _senderEndPointLeg2,
        boost::bind(&RTPProxy::handleLeg2FrameRead, shared_from_this(),
          boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
      }
      else
      {
        _pLeg2Socket->async_receive_from(boost::asio::buffer(_leg2Buffer), _lastSenderEndPointLeg2,
        boost::bind(&RTPProxy::handleLeg2FrameRead, shared_from_this(),
          boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
      }

      _leg2ReadTimer.expires_from_now(boost::posix_time::milliseconds(_pManager->_readTimeout));
      _leg2ReadTimer.async_wait(boost::bind(&RTPProxy::handleLeg1SocketReadTimeout, this, boost::asio::placeholders::error));
    }
    _csLeg2Mutex.unlock();

    //
    // Process the resize buffer if we are resizing
    //
    processResizerQueue();
  }
}

void RTPProxy::processResizerQueue()
{
  if (_type != Data)
    return;

  _leg1Resizer.queue().verbose() = _leg2Resizer.queue().verbose() = _pSession->verbose();

  boost::array<char, RTP_PACKET_BUFFER_SIZE> buff;
  std::size_t size = 0;

  if (_leg2Resizer.isEnabled())
  {
    _csLeg2Mutex.lock();
    while (_leg2Resizer.dequeue(buff, size))
    {
      if (_pLeg2Socket && size)
      {
        if (_isLeg2XOREncrypted && !_isXORDisabled)
          OSS::SIP::SIPXOR::rtpEncrypt(buff, size);

        _pLeg2Socket->async_send_to(boost::asio::buffer(buff, size), _senderEndPointLeg2,
                    boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                            boost::asio::placeholders::error));
      }
    }
    _csLeg2Mutex.unlock();
  }


  if (_leg1Resizer.isEnabled())
  {
    _csLeg1Mutex.lock();
    while (_leg1Resizer.dequeue(buff, size))
    {
        if (_pLeg1Socket && size)
        {
          if (_isLeg1XOREncrypted && !_isXORDisabled)
            OSS::SIP::SIPXOR::rtpEncrypt(buff, size);

          _pLeg1Socket->async_send_to(boost::asio::buffer(buff, size), _senderEndPointLeg1,
                      boost::bind(&RTPProxy::handleLeg1FrameWrite, shared_from_this(),
                              boost::asio::placeholders::error));
        }
    }
    _csLeg1Mutex.unlock();
  }
}

void RTPProxy::onResizerDequeue(RTPResizer& resizer, OSS::RTP::RTPPacket& packet)
{
  if (_type != Data)
    return;

  boost::array<char, RTP_PACKET_BUFFER_SIZE> buff;
  std::size_t size = 0;
  memcpy(buff.data(), packet.data(), packet.getPacketSize());
  size = packet.getPacketSize();

  if (resizer.legIndex() == 1 && _leg1Resizer.isEnabled())
  {
    _csLeg1Mutex.lock();
    if (_pLeg1Socket && size)
    {
      if (_isLeg1XOREncrypted && !_isXORDisabled)
        OSS::SIP::SIPXOR::rtpEncrypt(buff, size);

      _pLeg1Socket->async_send_to(boost::asio::buffer(buff, size), _senderEndPointLeg1,
                  boost::bind(&RTPProxy::handleLeg1FrameWrite, shared_from_this(),
                          boost::asio::placeholders::error));
    }
    _csLeg1Mutex.unlock();
  }
  else if (resizer.legIndex() == 2 && _leg2Resizer.isEnabled())
  {
    _csLeg2Mutex.lock();
    if (_pLeg2Socket && size)
    {
      if (_isLeg2XOREncrypted && !_isXORDisabled)
        OSS::SIP::SIPXOR::rtpEncrypt(buff, size);

      _pLeg2Socket->async_send_to(boost::asio::buffer(buff, size), _senderEndPointLeg2,
                  boost::bind(&RTPProxy::handleLeg2FrameWrite, shared_from_this(),
                          boost::asio::placeholders::error));
    }
    _csLeg2Mutex.unlock();
  }
}

void RTPProxy::handleLeg1SocketReadTimeout(const boost::system::error_code& e)
{
  if (!e)
  {
    //
    // see RTPProxyManager::collectInactiveSessions()
    //
    _isInactive = true;
  }
}

void RTPProxy::handleLeg2SocketReadTimeout(const boost::system::error_code& e)
{
  if (!e)
  {
    //
    // see RTPProxyManager::collectInactiveSessions()
    //
    _isInactive = true;
  }
}

OSS::IPAddress RTPProxy::getLeg1Address() const
{
  OSS::IPAddress addr(_localEndPointLeg1.address().to_string().c_str());
  addr.setPort(_localEndPointLeg1.port());
  addr.externalAddress() = _localEndPointLeg1External;
  return addr;
}

OSS::IPAddress RTPProxy::getLeg2Address() const
{
  OSS::IPAddress addr(_localEndPointLeg2.address().to_string().c_str());
  addr.setPort(_localEndPointLeg2.port());
  addr.externalAddress() = _localEndPointLeg2External;
  return addr;
}

void RTPProxy::setResizerSamples(int leg1, int leg2)
{
  _leg1Resizer.setSamples(leg1);
  _leg2Resizer.setSamples(leg2);
}

bool RTPProxy::validateBuffer(boost::array<char, RTP_PACKET_BUFFER_SIZE>& buff, int size)
{
  OSS::RTP::RTPPacket packet((u_char*)buff.data(), (unsigned int)size);
  return packet.getVersion() == 2; //TODO: magic value
}

} } // OSS::RTP



