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


#include "OSS/Net/DTLSBio.h"
#include "OSS/Net/DTLSSession.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace Net {

static const int DTLS_BIO_BUFFER_LEN = 4096;
  
// 
// SSL_read() read unencrypted data which is stored in the input BIO.
// SSL_write() write unencrypted data into the output BIO.
// BIO_write() write encrypted data into the input BIO.
// BIO_read() read encrypted data from the output BIO.
//

DTLSBio::DTLSBio() :
  _pInBIO(0),                                                                        /* we use memory read bios */
  _pOutBIO(0), 
  _pSSL(0)
{
  //
  // Create the bios
  //
  _pInBIO = BIO_new(BIO_s_mem());
  _pOutBIO = BIO_new(BIO_s_mem());
}

DTLSBio::~DTLSBio()
{
  if (!_pSSL)
  {
    //
    // The BIO has not been attached.  We need to free them here
    //
    BIO_free(_pInBIO);
    BIO_free(_pOutBIO);
  }
}

void DTLSBio::attachSSL(SSL* pSSL)
{
  _pSSL = pSSL;
  SSL_set_bio(_pSSL, _pInBIO, _pOutBIO);
}
  
  
int DTLSBio::sslRead(char* buf, int bufLen)
{
  /// 1. Read available data from the external input (probably libnice)
  /// If packet is not a DTLS packet, return the read value, else goto 2
  /// 2. Store (encrypted) read data into the input BIO using BIO_write
  /// 3. Call SSL_read to decrypt the data
  /// Returns return value of SSL_read
  ///
  if (!_pSSL)
  {
    OSS_LOG_ERROR("DTLSBio::sslRead - _pSSL is not set.");
    return 0;
  }
  
  if (!SSL_is_init_finished(_pSSL))
  {
    OSS_LOG_ERROR("DTLSBio::sslRead - SSL Handshake is not yet completed.");
    return 0;
  }
  
  if (!_readHandler)
  {
    OSS_LOG_ERROR("DTLSBio::sslRead - _readHandler is not set.");
    return 0;
  }
  
  int ret = 0;
  char readBuf[DTLS_BIO_BUFFER_LEN];
  
  ret = readDirect(readBuf, DTLS_BIO_BUFFER_LEN);
  
  if (ret > 0)
  {
    //
    // We have read some packets
    //
    if (DTLSSession::peek(readBuf) != DTLSSession::DTLS)
    {
      //
      // Not a DTLS packet.  return the raw packet
      //
      if (ret > bufLen)
      {
        //
        // We have read a packet that is bigger than the buffer provided
        // we do not have any better option but to truncate
        //
        OSS_LOG_WARNING("DTLSBio::sslRead - buffer size is smaller than the packet in the buffer.  Packet will be truncated!");
        ret = bufLen;
      }
      memcpy(buf, readBuf, ret);
      return ret;
    }
    
    ret = BIO_write(_pInBIO, readBuf, ret);
    if (ret > 0)
    {
      //
      // Packets are written to the IN BIO.  Read it back unencrypted.
      //
      ret = SSL_read(_pSSL, readBuf, bufLen); 
    }
  }
  return ret;
}
  
int DTLSBio::sslWrite(const char* buf, int bufLen)
{
  /// Note:  For RTP and STUN packets, you must use writeDirect to comply with RFC 5764
  /// 1. Write unencrypted data using SSL_write
  /// 2. Read encrypted data from outbound BIO using BIO_read
  /// 3. Write encrypted data to external output (probably libnice) 
  /// Returns return value of external output
  ///
  int ret = 0;
  char readBuf[DTLS_BIO_BUFFER_LEN];
  ret = SSL_write(_pSSL, buf, bufLen);
  
  if (ret > 0)
  {
    //
    // We have written something, read it back as encrypted data
    //
    ret = BIO_read(_pOutBIO, readBuf, DTLS_BIO_BUFFER_LEN);
    
    if (ret > 0)
    {
      //
      // We have read the encrypted data, write it to our external output
      //
      ret = writeDirect(readBuf, ret);
    }
  }
  return ret;
}
  
int DTLSBio::readDirect(char* buf, int bufLen)
{
  /// Read unencrypted data directly from the external source (probably libnice)
  ///
  if (!_readHandler)
  {
    OSS_LOG_ERROR("DTLSBio::readDirect - _readHandler is not set.");
    return 0;
  }
  return _readHandler(buf, bufLen);
}
  
int DTLSBio::writeDirect(const char* buf, int bufLen)
{
  ///  Write unencrypted data directly to external output (probably libnice)
  ///
  if (!_writeHandler)
  {
    OSS_LOG_ERROR("DTLSBio::writeDirect - _writeHandler is not set.");
    return 0;
  }
  return _writeHandler(buf, bufLen);
}
  
} } // OSS::Net



