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

#ifndef OSS_DTLSSOCKET_H_INCLUDED
#define	OSS_DTLSSOCKET_H_INCLUDED


#include <sys/socket.h>
#include "OSS/Net/IPAddress.h"


namespace OSS {
namespace Net {

  
class DTLSSocket
{
public:
  DTLSSocket();
  ~DTLSSocket();
  
  int bind(const OSS::Net::IPAddress& localAddress);
  int connect(const OSS::Net::IPAddress& remoteAddress);
  int read(char* buf, int bufLen);
  int write(const char* buf, int bufLen);
  int close();
  int fd() const;
  const OSS::Net::IPAddress& getLocalAddress() const;
  const OSS::Net::IPAddress& getRemoteAddress() const;
protected:
  int _fd;
  OSS::Net::IPAddress _localAddress;
  OSS::Net::IPAddress _remoteAddress;
};

//
// Inlines
//

inline int DTLSSocket::fd() const
{
  return _fd;
}

inline const OSS::Net::IPAddress& DTLSSocket::getLocalAddress() const
{
  return _localAddress;
}

const OSS::Net::IPAddress& DTLSSocket::getRemoteAddress() const
{
  return _remoteAddress;
}
  
} } // OSS::Net


#endif	// OSS_DTLSSOCKET_H_INCLUDED

