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

#ifndef OSS_DTLSSESSION_H_INCLUDED
#define	OSS_DTLSSESSION_H_INCLUDED


#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/e_os2.h>
#include <openssl/rand.h>
#include <openssl/err.h>


#include "OSS/Net/DTLSContext.h"
#include "OSS/UTL/Exception.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/RTP/RTPPacket.h"


namespace OSS {
namespace Net {

class DTLSSession
//
// Wrapper class for DTLS handshake and I/O routines
//
{
public:
  enum Type
  {
    CLIENT,
    SERVER
  };
  
  enum PacketType
  {
    STUN,
    RTP,
    DTLS,
    UNKNOWN
  };
  
  DTLSSession(Type type);
  ~DTLSSession();
  
  Type getType() const;
  /// Returns the connection type (CLIENT, SERVER)
  //
  
  void attachSocket(int fd);
  /// Attach a UDDP socket to this session.
  /// This creates a new BIO structure.
  /// If a previous socket has already been attached, this will throw and 
  /// OSS::IllegalStateException
  ///
  
  int getFd() const;
  /// Returns the descriptor of the UDP socket
 
  bool connect(const OSS::Net::IPAddress& address, bool socketAlreadyConnected);
  /// Connects the socket to the remote address and perform the DTLS handshake.
  /// If socketAlreadyConnected is true, only the SSL handshake will be performed.
  /// This function should be used with a previously bound (bind()) socket
  /// Throws OSS::InvalidAccessException if used on a SERVER session
  /// Throws OSS::IllegalStateException if the socket is not open
  /// Throws OSS::InvalidArgumentException if address is not valid
  /// Returns true if successful and set the _connected flag
  ///
  
  bool accept(OSS::Net::IPAddress& peerAddress);
  /// Wait for new client connection.
  /// The address of the remote peer is stored in peedAddress.
  /// This function should be used with a previously bound (bind()) socket
  /// Throws OSS::InvalidAccessException if used on a CLIENT session
  /// Throws OSS::IllegalStateException if the socket is not open
  /// Returns true if successful and set the _connected flag
  ///
  
  bool isConnected() const;
  /// Returns the status of the _connected flag
  ///
  
  int read(char* buf, int bufLen);
  /// Read some data
  /// Returns number of bytes read
  
  int write(const char* buf, int bufLen);
  /// Write some data
  /// Returns number of bytes written
  
  PacketType peek();
  /// Check the packet type of the datagram that is currently in the read
  /// buffer.  
  ///
  
  int readRaw(char* buf, int bufLen);
  /// Read some data directly using the socket.
  /// This would bypass the DTLS layer.
  /// This is normally used after peek returns RTP or STUN
  /// Returns number of bytes read
  
  int writeRaw(const char* buf, int bufLen);
  /// Write some data directly using the socket.
  /// This would bypass the DTLS layer.
  /// This is normally used for writing RTP or STUN packets
  /// Returns number of bytes written
  
  void setReceiveTimeout(unsigned int seconds);
  /// Set the RECV timeout
  ///
  unsigned int getReceiveTimeout() const;
  /// Seet the RECV timeourt
  ///
private:
  Type _type;
  SSL* _pSSL;
  BIO* _pBIO;
  int _fd;
  bool _connected;
  OSS::Net::IPAddress _peerAddress;
  unsigned int _receiveTimeout;
};
  
//
// Inlines
//

inline DTLSSession::Type DTLSSession::getType() const
{
  return _type;
}

inline int DTLSSession::getFd() const
{
  return _fd;
}

inline bool DTLSSession::isConnected() const
{
  return _connected;
}

 inline unsigned int DTLSSession::getReceiveTimeout() const
 {
   return _receiveTimeout;
 }

} } // OSS::Net

#endif	// OSS_DTLSSESSION_H_INCLUDED

