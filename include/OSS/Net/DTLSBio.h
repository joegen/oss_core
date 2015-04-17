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

#ifndef OSS_DTLSBIO_H_INCLUDED
#define	OSS_DTLSBIO_H_INCLUDED


#include <openssl/err.h>
#include <openssl/dh.h>
#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "OSS/OSS.h"


namespace OSS {
namespace Net {

class DTLSBio
{
  // 
  // SSL_read() read unencrypted data which is stored in the input BIO.
  // SSL_write() write unencrypted data into the output BIO.
  // BIO_write() write encrypted data into the input BIO.
  // BIO_read() read encrypted data from the output BIO.
  //
public:
  
  typedef boost::function<int(char*, int)> ReadHandler;
  typedef boost::function<int(const char*,int)> WriteHandler;
  
  DTLSBio();
  ~DTLSBio();
  
  int sslRead(char* buf, int bufLen);
  /// 1. Read available data from the external input (probably libnice)
  /// If packet is not a DTLS packet, return the read value, else goto 2
  /// 2. Store (encrypted) read data into the input BIO using BIO_write
  /// 3. Call SSL_read to decrypt the data
  /// Returns return value of SSL_read
  ///
  
  int sslWrite(const char* buf, int bufLen);
  /// Note:  For RTP and STUN packets, you must use writeDirect to comply with RFC 5764
  /// 1. Write unencrypted data using SSL_write
  /// 2. Read encrypted data from outbound BIO using BIO_read
  /// 3. Write encrypted data to external output (probably libnice) 
  /// Returns return value of external output
  ///
  
  int readDirect(char* buf, int bufLen);
  /// Read unencrypted data directly from the external source (probably libnice)
  ///
  
  int writeDirect(const char* buf, int bufLen);
  ///  Write unencrypted data directly to external output (probably libnice)
  ///
  
  void attachSSL(SSL* pSSL);
  /// Calls SSL_set_bio and attach the pointer
  ///
  
  void setReadHandler(const ReadHandler& readHandler);
  /// Set the external read handler
  ///
  
  void setWriteHandler(const WriteHandler& writeHandler);
  /// Set the external write handler
  ///
  
  int connect();
  /// call the client handshake
  /// 1.  Call SSL_do_handshake to start the handshake procedure
  /// 2.  Read the ClientHello data from the output BIO and send to external output
  /// 3.  Read the ServerHelloDone from external input and write it to the input BIO
  /// 4.  Call SSL_do_handshake again to process ServerHelloDone
  /// 5.  Repeat 2-4 to complete Certificate exchange
  ///
  
  int accept();
  /// call the server handshake

protected:
  
  
  BIO* _pInBIO;                                                                        /* we use memory read bios */
  BIO* _pOutBIO; 
  SSL* _pSSL;
  WriteHandler _writeHandler;
  ReadHandler _readHandler;
  friend class DTLSSession;
};


//
// Inlines
//


inline void DTLSBio::setReadHandler(const ReadHandler& readHandler)
{
  _readHandler = readHandler;
}
  
inline void DTLSBio::setWriteHandler(const WriteHandler& writeHandler)
{
  _writeHandler = writeHandler;
}


} } // OSS::Net

#endif	// OSS_DTLSBIO_H_INCLUDED

