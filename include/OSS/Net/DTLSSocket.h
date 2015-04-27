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
#include "OSS/Net/DTLSSocketInterface.h"


namespace OSS {
namespace Net {

  
class DTLSSocket : public DTLSSocketInterface
{
public:
  DTLSSocket(DTLSSession::Type type);
  virtual ~DTLSSocket();
  
  virtual int bind(const OSS::Net::IPAddress& localAddress);
  virtual int connect(const OSS::Net::IPAddress& remoteAddress);
  virtual int read(char* buf, int bufLen);
  virtual int write(const char* buf, int bufLen);
  virtual int close();
  virtual int fd() const;
  
  virtual int readFrom(char* buf, int bufLen, OSS::Net::IPAddress& remoteAddress);
  virtual bool hasReadFrom() const;
  virtual int peek(char* buf, int bufLen);
  virtual bool hasPeek() const;
  
  void setReadTimeout(int timeout);
  int getReadTimeout() const;
protected:
  int _fd;
  bool _connected;
  int _readTimeout;
};

//
// Inlines
//

inline int DTLSSocket::fd() const
{
  return _fd;
}

inline bool DTLSSocket::hasReadFrom() const
{
  return true;
}

inline bool DTLSSocket::hasPeek() const
{
  return true;
}

inline void DTLSSocket::setReadTimeout(int timeout)
{
  _readTimeout = timeout;
}

inline int DTLSSocket::getReadTimeout() const
{
  return _readTimeout;
}
  
} } // OSS::Net


#endif	// OSS_DTLSSOCKET_H_INCLUDED

