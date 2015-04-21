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

#ifndef OSS_DTLSSOCKETINTERFACE_H_INCLUDED
#define	OSS_DTLSSOCKETINTERFACE_H_INCLUDED


#include <boost/shared_ptr.hpp>
#include "OSS/Net/IPAddress.h"
#include "OSS/Net/DTLSSession.h"


namespace OSS {
namespace Net {

  
class DTLSSocketInterface
{
public:
  typedef boost::shared_ptr<DTLSSocketInterface> Ptr;
  DTLSSocketInterface(DTLSSession::Type type);
  
  virtual ~DTLSSocketInterface();
  
  //
  // Pure virtual socket implementation
  //
  virtual int bind(const OSS::Net::IPAddress& localAddress) = 0;
  virtual int connect(const OSS::Net::IPAddress& remoteAddress) = 0;
  virtual int read(char* buf, int bufLen) = 0;
  virtual int write(const char* buf, int bufLen) = 0;
  virtual int close() = 0;
  virtual int fd() const = 0;
  
  //
  // Optional Pure Virtuals
  //
  virtual int readFrom(char* buf, int bufLen, OSS::Net::IPAddress& remoteAddress) = 0;
  virtual bool hasReadFrom() const = 0;
  
  virtual int peek(char* buf, int bufLen) = 0;
  virtual bool hasPeek() const = 0;
  
  int sslRead(char* buf, int bufLen);
  /// Read some encrypted data
  /// Returns number of bytes read
  
  int sslWrite(const char* buf, int bufLen);
  /// Write some encrypted data
  /// Returns number of bytes written
  
  bool sslAccept();
  /// Perform the SSL Server Handshake
  
  bool sslConnect();
  /// Perform the SSL Client Handshake
  
  const OSS::Net::IPAddress& getLocalAddress() const;
  void setLocalAddress(const OSS::Net::IPAddress& localAddress);
  const OSS::Net::IPAddress& getRemoteAddress() const;
  void setRemoteAddress(const OSS::Net::IPAddress& remoteAddress);
  
  OSS::Net::DTLSSession& dtlsSession();
  
protected:
  OSS::Net::IPAddress _localAddress;
  OSS::Net::IPAddress _remoteAddress;
  OSS::Net::DTLSBio::Ptr _pBio;
  OSS::Net::DTLSSession _session;
};


//
// Inlines
//

inline const OSS::Net::IPAddress& DTLSSocketInterface::getLocalAddress() const
{  
  return _localAddress;
}

inline void DTLSSocketInterface::setLocalAddress(const OSS::Net::IPAddress& localAddress)
{
  _localAddress = localAddress;
}

inline const OSS::Net::IPAddress& DTLSSocketInterface::getRemoteAddress() const
{
  return _remoteAddress;
}

inline void DTLSSocketInterface::setRemoteAddress(const OSS::Net::IPAddress& remoteAddress)
{
  _remoteAddress = remoteAddress;
}

inline int DTLSSocketInterface::sslRead(char* buf, int bufLen)
{
  return _session.read(buf, bufLen);
}
  
inline int DTLSSocketInterface::sslWrite(const char* buf, int bufLen)
{
  return _session.write(buf, bufLen);
}

inline bool DTLSSocketInterface::sslAccept()
{
  return _session.bioAccept();
}
  
inline bool DTLSSocketInterface::sslConnect()
{
  return _session.bioConnect();
}

inline OSS::Net::DTLSSession& DTLSSocketInterface::dtlsSession()
{
  return _session;
}

  
} } // OSS::Net


#endif	// OSS_DTLSSOCKETINTERFACE_H_INCLUDED

