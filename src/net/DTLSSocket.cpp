/*
 * Copyright (C) OSS Software Solutions
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


#include "OSS/Net/DTLSSocket.h"
#include "OSS/UTL/Logger.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace Net {

static const int DTLS_DEFAULT_READ_TIMEOUT = 500;  
  
  
DTLSSocket::DTLSSocket(DTLSSession::Type type) :
  DTLSSocketInterface(type),
  _fd(-1),
  _connected(false),
  _readTimeout(DTLS_DEFAULT_READ_TIMEOUT)
{
}

DTLSSocket::~DTLSSocket()
{
  close();
}
  
int DTLSSocket::bind(const OSS::Net::IPAddress& localAddress)
{
  IPAddress::SockAddr bindAddr;
  int ret = 0;
  if (!localAddress.toSockAddr(bindAddr))
  {
    OSS_LOG_ERROR("DTLSSocket::bind - Exception:  Unable to convert IP Address to sockaddr");
    return 0;
  }
  
  if (_fd == -1)
  {
    if (localAddress.address().is_v4())
    {
      _fd = socket(AF_INET, SOCK_DGRAM, 0);
    }
    else
    {
      _fd = socket(AF_INET6, SOCK_DGRAM, 0);
    }
  }
  
  if (localAddress.address().is_v4())
  {
    ret = ::bind(_fd, (const struct sockaddr *) &bindAddr, sizeof(struct sockaddr_in));
  }
  else if (localAddress.address().is_v6())
  {
    ret = ::bind(_fd, (const struct sockaddr *) &bindAddr, sizeof(struct sockaddr_in6));
  }
  else
  {
    OSS_LOG_ERROR("DTLSSocket::bind - Exception:  Invalid bind address");
    return 0;
  }
  
  _localAddress = localAddress;
  
  return ret;
}

int DTLSSocket::connect(const OSS::Net::IPAddress& remoteAddress)
{
  int ret = 0;
  IPAddress::SockAddr addr;
  remoteAddress.toSockAddr(addr);
  if (remoteAddress.address().is_v4())
  {
    ret = ::connect(_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
  }
  else if (remoteAddress.address().is_v6())
  {
    ret = ::connect(_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in6));
  }
  else
  {
    OSS_LOG_ERROR("DTLSSocket::connect - Exception:  Invalid connect address");
    return -1;
  }
  
  _remoteAddress = remoteAddress;
  
  _connected = (ret==0);
  
  return ret;
}

int DTLSSocket::read(char* buf, int bufLen)
{
  struct pollfd fd;
  int ret = 0;

  fd.fd = _fd;
  fd.events = POLLIN;
  ret = poll(&fd, 1, _readTimeout);

  if (ret > 0)
  {
    if (!_remoteAddress.isValid())
    {
      //
      // Use readFrom so we can set the initial value of _remoteAddress
      //
      ret = readFrom(buf, bufLen, _remoteAddress);

      if (_remoteAddress.isValid())
      {
        connect(_remoteAddress);
      }
    }
    else
    {
      ret = recv(_fd, buf, bufLen, 0);
    }
  }
  
  return ret;
}

int DTLSSocket::readFrom(char* buf, int bufLen, OSS::Net::IPAddress& remoteAddress)
{
  struct sockaddr_storage peer_addr;
  socklen_t peer_addr_len;
  int ret;
  peer_addr_len = sizeof(struct sockaddr_storage);
  
  struct pollfd fd;
  fd.fd = _fd;
  fd.events = POLLIN;
  ret = poll(&fd, 1, _readTimeout);
  
  if (ret <= 0)
  {
    return ret;
  }
  
  ret = recvfrom(_fd, buf, bufLen, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
  
  if (ret > 0)
  {
    
    if (peer_addr.ss_family == AF_INET)
    {
      struct sockaddr_in& addr = (struct sockaddr_in&) peer_addr;
      remoteAddress = OSS::Net::IPAddress::fromSockAddr4(addr);
    }
    else if (peer_addr.ss_family == AF_INET6)
    {
      struct sockaddr_in6& addr = (struct sockaddr_in6&) peer_addr;
      remoteAddress = OSS::Net::IPAddress::fromSockAddr6(addr);
    }
  }
  
  return ret;
}

int DTLSSocket::write(const char* buf, int bufLen)
{
  return send(_fd, buf, bufLen, 0);
}

int DTLSSocket::close()
{
  return ::close(_fd);
}


int DTLSSocket::peek(char* buf, int bufLen)
{
  return recv(_fd, buf, bufLen, MSG_PEEK);
}

  

} } // OSS::Net



