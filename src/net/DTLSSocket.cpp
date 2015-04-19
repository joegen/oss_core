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


namespace OSS {
namespace Net {

  
DTLSSocket::DTLSSocket() :
  _fd(-1)
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
    socket(bindAddr.ss.ss_family, SOCK_DGRAM, 0);
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
    OSS_LOG_ERROR("DTLSSocket::bind - Exception:  Invalid connect address");
    return 0;
  }
  
  _remoteAddress = remoteAddress;
  
  return ret;
}

int DTLSSocket::read(char* buf, int bufLen)
{
  return recv(_fd, buf, bufLen, 0);
}

int DTLSSocket::write(const char* buf, int bufLen)
{
  return send(_fd, buf, bufLen, 0);
}

int DTLSSocket::close()
{
  return ::close(_fd);
}
  

} } // OSS::Net



