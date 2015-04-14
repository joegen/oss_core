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

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/bio.h>

#include "OSS/Net/DTLSSession.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace Net {

  
static const unsigned int DEFAULT_DTLS_RECEIVE_TIMEOUT = 5;  
  
BIO_METHOD* gpBIOMethod = 0;
static int (*bio_read_handler)(BIO *, char *, int); // holds the old bio handler. we override this method so we can have a raw handle on the packets   
  
static int read_dtls_packet(BIO* pBIO, char* buff, int buffLen)
{
  int ret = (*bio_read_handler)(pBIO, buff, buffLen);
  //
  // Excerp from RFC 5764
  //
  // The process for demultiplexing a packet is as follows.  The receiver
  // looks at the first byte of the packet.  If the value of this byte is
  // 0 or 1, then the packet is STUN.  If the value is in between 128 and
  // 191 (inclusive), then the packet is RTP (or RTCP, if both RTCP and
  // RTP are being multiplexed over the same destination port).  If the
  // value is between 20 and 63 (inclusive), the packet is DTLS.  This
  // process is summarized in Figure 3.
  //
  //                   +----------------+
  //                 | 127 < B < 192 -+--> forward to RTP
  //                 |                |
  //     packet -->  |  19 < B < 64  -+--> forward to DTLS
  //                 |                |
  //                 |       B < 2   -+--> forward to STUN
  //
  
  return ret;
}

static std::string get_socket_error() {
	switch (errno) {
		case EINTR:
			/* Interrupted system call.
			 * Just ignore.
			 */
			return "Interrupted system call!";
		case EBADF:
			/* Invalid socket.
			 * Must close connection.
			 */
			return "Invalid socket!";
		case EHOSTDOWN:
			/* Host is down.
			 * Just ignore, might be an attacker
			 * sending fake ICMP messages.
			 */
			return "Host is down!";

		case ECONNRESET:
			/* Connection reset by peer.
			 * Just ignore, might be an attacker
			 * sending fake ICMP messages.
			 */
			return "Connection reset by peer!";
		case ENOMEM:
			/* Out of memory.
			 * Must close connection.
			 */
			return "Out of memory!";
		case EACCES:
			/* Permission denied.
			 * Just ignore, we might be blocked
			 * by some firewall policy. Try again
			 * and hope for the best.
			 */
			return "Permission denied!";
		default:
			/* Something unexpected happened */
      
			return "Unexpected error!";
	}
	return 0;
}
  
DTLSSession::DTLSSession(Type type) :
  _type(type),
  _pSSL(0),
  _pBIO(0),
  _fd(-1),
  _connected(false),
  _receiveTimeout(DEFAULT_DTLS_RECEIVE_TIMEOUT)
{
  if (!DTLSContext::instance())
  {
    OSS_LOG_ERROR("DTLSSession::DTLSSession Exception:  Attempt to construct as session without a DTLS context");
    return;
  }
  
  _pSSL = SSL_new(&DTLSContext::instance()->sslContext());
  if (_type == DTLSSession::CLIENT)
  {
    SSL_set_connect_state(_pSSL);
  }
  else
  {
    SSL_set_accept_state(_pSSL);
  }
}

DTLSSession::~DTLSSession()
{
  SSL_free(_pSSL);
}

void DTLSSession::attachSocket(int fd)
{
  if (_fd != -1 || _pBIO || fd <= 0)
  {
    throw OSS::IllegalStateException();
  }

  if (!gpBIOMethod)
  {
    //
    // We will replace the default read handler with our own
    // So we get a glimpse of the raw packet.
    //
    gpBIOMethod = BIO_s_datagram();
    bio_read_handler = gpBIOMethod->bread;
    gpBIOMethod->bread = read_dtls_packet;
  }
  
  //
  // Create the BIO and attach our socket
  //
  //_pBIO = BIO_new_dgram(fd, BIO_NOCLOSE);
  _pBIO = BIO_new(gpBIOMethod);
  BIO_set_fd(_pBIO, fd, BIO_NOCLOSE);
  
  //
  // Set the receive timeout
  //
  setReceiveTimeout(_receiveTimeout);
  
  _fd = fd;
  
  SSL_set_bio(_pSSL, _pBIO, _pBIO);
}

void DTLSSession::setReceiveTimeout(unsigned int seconds)
{
  _receiveTimeout = seconds;
  
  if (_pBIO)
  {
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    BIO_ctrl(_pBIO, BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);
  }
}

bool DTLSSession::connect(const OSS::Net::IPAddress& address, bool socketAlreadyConnected)
{
  if (_fd <= 0 || !_pBIO)
  {
    OSS_LOG_ERROR("DTLSSession::connect Exception: FD or BIO not set.");
    throw OSS::IllegalStateException();
  }
  
  if (_type == DTLSSession::SERVER)
  {
    OSS_LOG_ERROR("DTLSSession::connect Exception: Illegal call to connect() using a SERVER session.");
    throw OSS::InvalidAccessException();
  }
  
  union 
  {
    struct sockaddr_storage ss;
    struct sockaddr_in s4;
    struct sockaddr_in6 s6;
  }remote_addr;
  memset((void *) &remote_addr, 0, sizeof(struct sockaddr_storage));

  if (!address.isValid())
  {
    OSS_LOG_ERROR("DTLSSession::connect Exception: Address is neither V4 or V6.");
    throw OSS::InvalidArgumentException();
  }

  if (address.address().is_v4())
  {
    remote_addr.s4.sin_family = AF_INET;
#ifdef HAVE_SIN_LEN
    remote_addr.s4.sin_len = sizeof(struct sockaddr_in);
#endif
    remote_addr.s4.sin_port = htons(address.getPort());
  }
  else if (address.address().is_v6())
  {
    remote_addr.s6.sin6_family = AF_INET6;
#ifdef HAVE_SIN6_LEN
    remote_addr.s6.sin6_len = sizeof(struct sockaddr_in6);
#endif
    remote_addr.s6.sin6_port = htons(address.getPort());
  }
  
  if (!socketAlreadyConnected)
  {
    int ret = 0;
    //
    // Do a connect() on this socket
    //
   
    if (address.address().is_v4())
    {
      ret = ::connect(_fd, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr_in));
    }
    else if (address.address().is_v6())
    {
      ret = ::connect(_fd, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr_in6));
    }

    if (ret != 0)
    {
      OSS_LOG_ERROR("DTLSSession::connect Exception: connect returned " << ret << " errno: " << errno << " desc: " << get_socket_error());
      return false;
    }
  }
  
  BIO_ctrl(_pBIO, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &remote_addr.ss);
    
  if (SSL_connect(_pSSL) < 0) 
  {
    char buf[512];
		OSS_LOG_ERROR("DTLSSession::connect Exception: " << ERR_error_string(ERR_get_error(), buf));
    return false;
	}
  _connected = true;
  _peerAddress = address;
  return _connected;
}

bool DTLSSession::accept(const OSS::Net::IPAddress& peerAddress)
{
  if (_fd <= 0 || !_pBIO)
  {
    OSS_LOG_ERROR("DTLSSession::accept Exception: FD or BIO not set.");
    throw OSS::IllegalStateException();
  }
  
  if (_type == DTLSSession::CLIENT)
  {
    OSS_LOG_ERROR("DTLSSession::accept Exception: Illegal call to connect() using a CLIENT session.");
    throw OSS::InvalidAccessException();
  }
  
  union 
  {
		struct sockaddr_storage ss;
		struct sockaddr_in s4;
		struct sockaddr_in6 s6;
	} client_addr;
  
   
  int ret = 0;
	do { ret = SSL_accept(_pSSL); }
	while (ret == 0);
  
  (void)BIO_dgram_get_peer(SSL_get_rbio(_pSSL), &client_addr); 
  
  //
  // Connect the socket
  //
  switch (client_addr.ss.ss_family)
  {
		case AF_INET:
			::connect(_fd, (struct sockaddr *) &client_addr, sizeof(struct sockaddr_in));
			break;
		case AF_INET6:
			::connect(_fd, (struct sockaddr *) &client_addr, sizeof(struct sockaddr_in6));
			break;
		default:
      OSS_LOG_ERROR("DTLSSession::accept FAILURE: Unable to determine remote peer address!");
			return false;
			break;
	}
   
	if (ret < 0) 
  {
		char buf[512];
		OSS_LOG_ERROR("DTLSSession::accept FAILURE: " << ERR_error_string(ERR_get_error(), buf));
    _connected = false;
	}
  else
  {
    _connected = true;
  }
  
  _connected = true;
  return _connected;
}

int DTLSSession::read(char* buf, int bufLen)
{
  if (_fd <= 0 || !_pBIO || !_pSSL || !_connected)
  {
    OSS_LOG_ERROR("DTLSSession::read Exception: FD/BIO not set or socket not connected.");
    throw OSS::IllegalStateException();
  }
  
  char errBuf[512];
  int ret = 0;
  ret = SSL_read(_pSSL, buf, bufLen);
  
  
  switch (SSL_get_error(_pSSL, ret)) 
  {
    case SSL_ERROR_NONE:
      break;
    case SSL_ERROR_WANT_READ:
      /* Handle socket timeouts */
      if (BIO_ctrl(SSL_get_rbio(_pSSL), BIO_CTRL_DGRAM_GET_RECV_TIMER_EXP, 0, NULL)) 
      {
        OSS_LOG_ERROR("DTLSSession::read Error: Read Timeout!");
      }
      break;
    case SSL_ERROR_ZERO_RETURN:
      OSS_LOG_ERROR("DTLSSession::read Error: SSL_ERROR_ZERO_RETURN");
      break;
    case SSL_ERROR_SYSCALL:
      OSS_LOG_ERROR("DTLSSession::read Error: SSL_read returned " << ret << " errno: " << errno << " desc: " << get_socket_error());
      break;
    case SSL_ERROR_SSL:
      OSS_LOG_ERROR("DTLSSession::read SSL FAILURE: " << ERR_error_string(ERR_get_error(), errBuf));
      break;
    default:
      OSS_LOG_ERROR("DTLSSession::read Unexpected error while reading!");
      break;
  }
  
  if (ret < 0)
    ret = 0;
  
  return ret;
}

int DTLSSession::write(const char* buf, int bufLen)
{
  if (_fd <= 0 || !_pBIO || !_pSSL || !_connected)
  {
    OSS_LOG_ERROR("DTLSSession::write Exception: FD/BIO not set or socket not connected.");
    throw OSS::IllegalStateException();
  }
  
  char errBuf[512];
  int ret = 0;
  ret = SSL_write(_pSSL, buf, bufLen);
  
  
  switch (SSL_get_error(_pSSL, ret)) 
  {
    case SSL_ERROR_NONE:
      break;
    case SSL_ERROR_WANT_WRITE:
      // Can't write because of a renegotiation, so
      // we actually have to retry sending this message...
      //
      break;
    case SSL_ERROR_ZERO_RETURN:
      OSS_LOG_ERROR("DTLSSession::write Error: SSL_ERROR_ZERO_RETURN");
      break;
    case SSL_ERROR_SYSCALL:
      OSS_LOG_ERROR("DTLSSession::write Error: SSL_write returned " << ret << " errno: " << errno << " desc: " << get_socket_error());
      break;
    case SSL_ERROR_SSL:
      OSS_LOG_ERROR("DTLSSession::write SSL FAILURE: " << ERR_error_string(ERR_get_error(), errBuf));
      break;
    default:
      OSS_LOG_ERROR("DTLSSession::write Unexpected error while writing!");
      break;
  }
  
  if (ret < 0)
    ret = 0;
  
  return ret;
}
  
  
} } // OSS::Net

