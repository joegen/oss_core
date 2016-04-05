// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare 
// derivative works of the Software, all subject to the 
// "GNU Lesser General Public License (LGPL)".
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#include <string>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Exception.h"

#include "OSS/Net/Net.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Exception.h"
#include "OSS/UTL/Thread.h"
#include "OSS/UTL/Application.h"
#include "OSS/UTL/Logger.h"

#if !defined(OSS_OS_FAMILY_WINDOWS)
#include <ifaddrs.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
namespace OSS {

network_interface::interface_address_list network_interface::_table;
//
// Private namespace
// 
namespace Private {

class net_singletons
{
public:
  net_singletons() :
    _ioService(),
    _resolver(_ioService),
    _houseKeepingTimer(_ioService, boost::posix_time::milliseconds(3600000)),
    _pIoServiceThread(0)
  {
  }

  void onHouseKeepingTimer(const boost::system::error_code& e)
  {
    //
    // This will fire every hour.  Right now we dont ahve use for it but this is a good place
    // to put connectivity checks and garbage collection
    // 
    if (e != boost::asio::error::operation_aborted)
    {
      _houseKeepingTimer.expires_from_now(boost::posix_time::milliseconds(3600000));
      _houseKeepingTimer.async_wait(boost::bind(&net_singletons::onHouseKeepingTimer, this, boost::asio::placeholders::error));
    }
  }

  boost::asio::io_service _ioService;
  boost::asio::ip::tcp::resolver _resolver;
  boost::asio::deadline_timer _houseKeepingTimer;
  boost::thread* _pIoServiceThread;

};

static net_singletons* _pNetSingletons = 0;


void net_init()
{
  if (!_pNetSingletons)
  {
    _pNetSingletons = new net_singletons();
    _pNetSingletons->_houseKeepingTimer.expires_from_now(boost::posix_time::milliseconds(3600000));
    _pNetSingletons->_houseKeepingTimer.async_wait(boost::bind(&net_singletons::onHouseKeepingTimer, _pNetSingletons, boost::asio::placeholders::error));
    _pNetSingletons->_pIoServiceThread = new boost::thread(boost::bind(&boost::asio::io_service::run, &(_pNetSingletons->_ioService)));
  }

#if !defined(OSS_OS_FAMILY_WINDOWS)


	struct ifaddrs *list;
	getifaddrs(&list);
	{
    struct ifaddrs *cur;
    for(cur = list; cur != 0; cur = cur->ifa_next)
    {
      if(!cur || !cur->ifa_addr || cur->ifa_addr->sa_family != AF_INET)
        continue;

      struct sockaddr_in *addrStruct = (struct sockaddr_in *)cur->ifa_addr;
      struct sockaddr_in *netmaskStruct = (struct sockaddr_in *)cur->ifa_netmask;
      struct sockaddr_in *broadcastStruct = (struct sockaddr_in *)cur->ifa_broadaddr;
      struct sockaddr_in *destAddrStruct = (struct sockaddr_in *)cur->ifa_dstaddr;
      
      network_interface iface;
      iface._ipAddress = inet_ntoa(addrStruct->sin_addr);
      iface._netMask = inet_ntoa(netmaskStruct->sin_addr);
      iface._broadcastAddress = inet_ntoa(broadcastStruct->sin_addr);
      iface._destAddr = inet_ntoa(destAddrStruct->sin_addr);
      iface._flags = cur->ifa_flags;
      iface._ifName = cur->ifa_name;
      network_interface::_table.push_back(iface);
    }
	}
  freeifaddrs(list);


#endif
}

void net_deinit()
{
  if (_pNetSingletons)
  {
    _pNetSingletons->_houseKeepingTimer.cancel();
    _pNetSingletons->_ioService.stop();
    _pNetSingletons->_pIoServiceThread->join();
    delete _pNetSingletons->_pIoServiceThread;
    delete _pNetSingletons;
    _pNetSingletons = 0;
  }
}
}

boost::asio::io_service& net_io_service()
{
  OSS_ASSERT_NULL(OSS::Private::_pNetSingletons);
  return OSS::Private::_pNetSingletons->_ioService;
}

boost::asio::ip::tcp::resolver& net_resolver()
{
  OSS_ASSERT_NULL(OSS::Private::_pNetSingletons);
  return OSS::Private::_pNetSingletons->_resolver;
}

//
// OSS::Net::IPAddress helper functions
//
bool socket_address_cidr_verify(const std::string& ip, const std::string& cidr)
{
  std::vector<std::string> cidr_tokens;
  cidr_tokens = OSS::string_tokenize(cidr, "/-");

  unsigned bits = 24;
  std::string start_ip;
  if (cidr_tokens.size() == 2)
  {
    bits = OSS::string_to_number<unsigned>(cidr_tokens[1].c_str());
    start_ip = cidr_tokens[0];
  }
  else
  {
    start_ip = cidr;
  }

  boost::system::error_code ec;
  boost::asio::ip::address_v4 ipv4;
  ipv4 = boost::asio::ip::address_v4::from_string(ip, ec);
  if (!ec)
  {
    unsigned long ipv4ul = ipv4.to_ulong();
    boost::asio::ip::address_v4 start_ip_ipv4;
    start_ip_ipv4 = boost::asio::ip::address_v4::from_string(start_ip, ec);
    if (!ec)
    {
      long double numHosts = pow((long double)2, (int)(32-bits)) - 1;
      unsigned long start_ip_ipv4ul = start_ip_ipv4.to_ulong();
      unsigned long ceiling = start_ip_ipv4ul + numHosts;
      return ipv4ul >= start_ip_ipv4ul && ipv4ul <= ceiling;
    }
  }
  return false;
}


bool socket_address_range_verify(const std::string& low, const std::string& high, const std::string& strIp)
{
  bool lowCheck = false;
  bool highCheck = false;
  
  OSS::Net::IPAddress ipLow(low);
  OSS::Net::IPAddress ipHigh(high);
  OSS::Net::IPAddress ip(strIp);
  
  if (!ipLow.isValid() || !ipHigh.isValid() || !ip.isValid())
    return false;

  if (ipLow == ip || ipHigh == ip)
    return true;
  
  lowCheck = !(ip.address() < ipLow.address());
  highCheck = ip.address() < ipHigh.address();
  
  return lowCheck && highCheck; 
}


//
// Common Socket Functions
//

static Poco::Timespan poco_timespan(OSS::UInt32 timeout)
{
  if (timeout < 1000)
  {
    return Poco::Timespan(0, timeout * 1000);
  }
  else
  {
    OSS::UInt32 excess = timeout % 1000;
    return Poco::Timespan(timeout / 1000, excess * 1000);
  }
}

void socket_free(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  delete sock;
}

bool socket_poll(socket_handle handle, const OSS::UInt32 timeout, ModeSelect mode)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Timespan timeSpan = poco_timespan(timeout);
  try
  {
    return sock->poll(timeSpan, mode);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
  return false;
}

int socket_available(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return sock->available();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_set_send_buffer_size(socket_handle handle, int size)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    sock->setSendBufferSize(size);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}
	
int socket_get_send_buffer_size(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return sock->getSendBufferSize();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_set_receive_buffer_size(socket_handle handle, int size)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    sock->setReceiveBufferSize(size);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}
	
int socket_get_receive_buffer_size(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return sock->getReceiveBufferSize();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_set_send_timeout(socket_handle handle, OSS::UInt32 timeout)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Timespan timeSpan = poco_timespan(timeout);
  try
  {
    sock->setSendTimeout(timeSpan);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

OSS::UInt32 socket_get_send_timeout(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return (OSS::UInt32)sock->getSendTimeout().totalMilliseconds();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_set_receive_timeout(socket_handle handle, OSS::UInt32 timeout)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Timespan timeSpan = poco_timespan(timeout);
  try
  {
    sock->setReceiveTimeout(timeSpan);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

OSS::UInt32 socket_get_receive_timeout(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return (OSS::UInt32)sock->getReceiveTimeout().totalMilliseconds();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_set_linger(socket_handle handle, bool on, int seconds)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    sock->setLinger(on, seconds);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}
	
void socket_get_linger(socket_handle handle, bool& on, int& seconds)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    sock->getLinger(on, seconds);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_set_no_delay(socket_handle handle, bool flag)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    sock->setNoDelay(flag);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}
	
bool socket_get_no_delay(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return sock->getNoDelay();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_set_keep_alive(socket_handle handle, bool flag)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    sock->setKeepAlive(flag);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}
	
bool socket_get_keep_alive(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return sock->getKeepAlive();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_set_reuse_address(socket_handle handle, bool flag)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    sock->setReuseAddress(flag);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

bool socket_get_reuse_address(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return sock->getReuseAddress();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_set_reuse_port(socket_handle handle, bool flag)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    sock->setReusePort(flag);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

bool socket_get_reuse_port(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return sock->getReusePort();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}
	
void socket_set_oob_inline(socket_handle handle, bool flag)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    sock->setOOBInline(flag);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

bool socket_get_oob_inline(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return sock->getOOBInline();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_set_blocking(socket_handle handle, bool flag)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    sock->setBlocking(flag);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

bool socket_get_blocking(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  try
  {
    return sock->getBlocking();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

bool socket_is_tcp(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::StreamSocket* tcpCast = dynamic_cast<Poco::Net::StreamSocket*>(sock);
  if (tcpCast != 0)
    return true;

  Poco::Net::ServerSocket* tcpServerCast = dynamic_cast<Poco::Net::ServerSocket*>(sock);
  if (tcpServerCast != 0)
    return true;

  return false;
}

bool socket_is_udp(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::DatagramSocket* udpCast = dynamic_cast<Poco::Net::DatagramSocket*>(sock);
  return udpCast != 0;
}

OSS::Net::IPAddress socket_get_address(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  
  std::string proto = "ip";
  if (socket_is_tcp(handle))
    proto = "tcp";
  else if (socket_is_udp(handle))
    proto = "udp";

  Poco::Net::SocketAddress sockAddress;
  try
  {
     sockAddress = sock->address();
     OSS::Net::IPAddress addr(sockAddress.host().toString(), sockAddress.port());
     return addr;
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}
	
OSS::Net::IPAddress socket_get_peer_address(socket_handle handle)
{
  Poco::Net::Socket* sock = static_cast<Poco::Net::Socket*>(handle);
  
  Poco::Net::SocketAddress sockAddress;
  try
  {
    sockAddress = sock->peerAddress();
    OSS::Net::IPAddress addr(sockAddress.host().toString(), sockAddress.port());
    return addr;
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

bool socket_supports_ip4()
{
  return Poco::Net::Socket::supportsIPv4();
}
	
bool socket_supports_ip6()
{
  return Poco::Net::Socket::supportsIPv6();
}

bool socket_supports_poll()
{
#if defined(POCO_HAVE_FD_POLL)
  return true;
#else
  return false;
#endif
}

//
// TCP Socket Client Functions
//

socket_handle socket_tcp_client_create()
{
  return new Poco::Net::StreamSocket();
}

socket_handle socket_tcp_server_create()
{
  return new Poco::Net::ServerSocket();
}

void socket_tcp_client_connect(socket_handle handle, const std::string& address, OSS::UInt16 port)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::StreamSocket* sock = dynamic_cast<Poco::Net::StreamSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  Poco::Net::SocketAddress addr(address, port);
  try
  {
    sock->connect(addr);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_tcp_client_connect(socket_handle handle, const std::string& address, OSS::UInt16 port, OSS::UInt32 timeout)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::StreamSocket* sock = dynamic_cast<Poco::Net::StreamSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  Poco::Net::SocketAddress addr(address, port);
  
  Poco::Timespan timeSpan = poco_timespan(timeout);
 
  try
  {
    sock->connect(addr, timeSpan);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_tcp_client_connect_nb(socket_handle handle, const std::string& address, OSS::UInt16 port)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::StreamSocket* sock = dynamic_cast<Poco::Net::StreamSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  Poco::Net::SocketAddress addr(address, port);
  try
  {
    sock->connectNB(addr);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_tcp_client_shutdown_receive(socket_handle handle)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::StreamSocket* sock = dynamic_cast<Poco::Net::StreamSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    sock->shutdownReceive();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}
	
void socket_tcp_client_shutdown_send(socket_handle handle)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::StreamSocket* sock = dynamic_cast<Poco::Net::StreamSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    sock->shutdownSend();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}
	
void socket_tcp_client_shutdown(socket_handle handle)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::StreamSocket* sock = dynamic_cast<Poco::Net::StreamSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    sock->shutdown();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

int socket_tcp_client_send_bytes(socket_handle handle, const void* buffer, int length, int flags)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::StreamSocket* sock = dynamic_cast<Poco::Net::StreamSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    return sock->sendBytes(buffer, length, flags);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

int socket_tcp_client_receive_bytes(socket_handle handle, void* buffer, int length, int flags)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::StreamSocket* sock = dynamic_cast<Poco::Net::StreamSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    return sock->receiveBytes(buffer, length, flags);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_tcp_client_send_urgent(socket_handle handle, unsigned char data)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::StreamSocket* sock = dynamic_cast<Poco::Net::StreamSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    sock->sendUrgent(data);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_tcp_server_bind(socket_handle handle, const std::string& address, OSS::UInt16 port, bool reuseAddress)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::ServerSocket* sock = dynamic_cast<Poco::Net::ServerSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    Poco::Net::SocketAddress addr(address, port);
    sock->bind(addr, reuseAddress);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_tcp_server_bind(socket_handle handle, OSS::UInt16 port, bool reuseAddress)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::ServerSocket* sock = dynamic_cast<Poco::Net::ServerSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    sock->bind(port, reuseAddress);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}
	
void socket_tcp_server_listen(socket_handle handle, int backlog)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::ServerSocket* sock = dynamic_cast<Poco::Net::ServerSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    sock->listen(backlog);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

socket_handle socket_tcp_server_accept_connection(socket_handle handle, std::string& address, OSS::UInt16& port)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::ServerSocket* sock = dynamic_cast<Poco::Net::ServerSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  Poco::Net::StreamSocket* newCon = new Poco::Net::StreamSocket();
  try
  {
    Poco::Net::SocketAddress addr;
    *newCon = sock->acceptConnection(addr);
    address = addr.host().toString();
    port = addr.port();
    return newCon;
  }
  catch(const Poco::Exception& e)
  {
    delete newCon;
    throw OSS::NetException(e.displayText(), e.code());
  }
}

socket_handle socket_tcp_server_accept_connection(socket_handle handle)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::ServerSocket* sock = dynamic_cast<Poco::Net::ServerSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  Poco::Net::StreamSocket* newCon = new Poco::Net::StreamSocket();
  try
  {
    *newCon = sock->acceptConnection();
    return newCon;
  }
  catch(const Poco::Exception& e)
  {
    delete newCon;
    throw OSS::NetException(e.displayText(), e.code());
  }
}


//
// UDP Socket functions
//

socket_handle socket_udp_create()
{
  return new Poco::Net::DatagramSocket();
}

void socket_udp_connect(socket_handle handle, const std::string& address, OSS::UInt16 port)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::DatagramSocket* sock = dynamic_cast<Poco::Net::DatagramSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    Poco::Net::SocketAddress addr(address, port);
    sock->connect(addr);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_udp_bind(socket_handle handle, const std::string& address, OSS::UInt16 port, bool reuseAddress)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::DatagramSocket* sock = dynamic_cast<Poco::Net::DatagramSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    Poco::Net::SocketAddress addr(address, port);
    sock->bind(addr, reuseAddress);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

int socket_udp_send_bytes(socket_handle handle, const void* buffer, int length, int flags)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::DatagramSocket* sock = dynamic_cast<Poco::Net::DatagramSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    return sock->sendBytes(buffer, length, flags);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

int socket_udp_receive_bytes(socket_handle handle, void* buffer, int length, int flags)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::DatagramSocket* sock = dynamic_cast<Poco::Net::DatagramSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    return sock->receiveBytes(buffer, length, flags);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

int socket_udp_send_to(socket_handle handle, const void* buffer, int length, const std::string& address, OSS::UInt16 port, int flags)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::DatagramSocket* sock = dynamic_cast<Poco::Net::DatagramSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    Poco::Net::SocketAddress addr(address, port);
    return sock->sendTo(buffer, length, addr, flags);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

int socket_udp_receive_from(socket_handle handle, void* buffer, int length, std::string& address, OSS::UInt16& port, int flags)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::DatagramSocket* sock = dynamic_cast<Poco::Net::DatagramSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    Poco::Net::SocketAddress addr;
    int ret = sock->receiveFrom(buffer, length, addr, flags);
    address = addr.host().toString();
    port = addr.port();
    return ret;
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

void socket_udp_set_broadcast(socket_handle handle, bool flag)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::DatagramSocket* sock = dynamic_cast<Poco::Net::DatagramSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    sock->setBroadcast(flag);
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

bool socket_udp_get_broadcast(socket_handle handle)
{
  Poco::Net::Socket* rawSock = static_cast<Poco::Net::Socket*>(handle);
  Poco::Net::DatagramSocket* sock = dynamic_cast<Poco::Net::DatagramSocket*>(rawSock);
  OSS_VERIFY_NULL(sock);
  try
  {
    return sock->getBroadcast();
  }
  catch(const Poco::Exception& e)
  {
    throw OSS::NetException(e.displayText(), e.code());
  }
}

bool socket_ip_tos_set(int fd, int family, int tos)
{
    if (family != AF_INET6) {
        return (setsockopt(fd, IPPROTO_IP, IP_TOS, (const void*)&tos, sizeof(tos)) < 0) ? false : true;
    } else {
        return (setsockopt(fd, IPPROTO_IPV6, IPV6_TCLASS, &tos, sizeof(tos)) < 0) ? false : true;
    }
}

//
// Net Timer functions
//

NetTimer::NetTimer()
{
  _timerHandle = 0;
}

NetTimer::~NetTimer()
{
  delete reinterpret_cast<boost::asio::deadline_timer*>(_timerHandle);
}

void NetTimer::onTimer(const boost::system::error_code& e)
{
  if (_handler && e != boost::asio::error::operation_aborted)
  {
    try
    {
      OSS::thread_pool::static_schedule(_handler);
    }
    catch(const std::exception& e)
    {
      OSS_LOG_ERROR("NetTimer::onTimer Exception: " << e.what());
    }
  }
}


NET_TIMER_HANDLE net_io_timer_create(int millis, net_timer_func handler)
{
  OSS_VERIFY_NULL(OSS::Private::_pNetSingletons);
  NET_TIMER_HANDLE handle = NET_TIMER_HANDLE(new NetTimer());
 
  boost::asio::deadline_timer* pTimer = 
    new boost::asio::deadline_timer(OSS::Private::_pNetSingletons->_ioService, boost::posix_time::milliseconds(millis));

  handle->_timerHandle = (OSS_HANDLE)pTimer;
  handle->_handler = handler;
  
  pTimer->expires_from_now(boost::posix_time::milliseconds(millis));
  pTimer->async_wait(boost::bind(&NetTimer::onTimer, handle, boost::asio::placeholders::error));
  

  return handle;
}

void net_io_timer_cancel(NET_TIMER_HANDLE timerHandle)
{
  boost::asio::deadline_timer* pTimer = 
    reinterpret_cast<boost::asio::deadline_timer*>(timerHandle->_timerHandle);
  pTimer->cancel();
}

bool net_get_default_interface_name(std::string& iface)
{
  FILE *f;
  char line[100] , *p , *c;
  f = fopen("/proc/net/route" , "r");

  while(fgets(line , 100 , f))
  {
    p = strtok(line , " \t");
    c = strtok(NULL , " \t");

    if(p!=NULL && c!=NULL)
    {
      if(strcmp(c , "00000000") == 0)
      {
        iface = p;
        return true;
      }
    }
  }
  return false;
}

bool net_get_interface_address(const std::string iface, std::string& address, int fm)
{
  struct ifaddrs *ifaddr, *ifa;
  int family , s;
  char host[NI_MAXHOST];

  if (getifaddrs(&ifaddr) == -1) 
  {
    return false;
  }

  //Walk through linked list, maintaining head pointer so we can free list later
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
  {
    if (ifa->ifa_addr == NULL)
    {
      continue;
    }

    family = ifa->ifa_addr->sa_family;

    if(strcmp( ifa->ifa_name , iface.c_str()) == 0)
    {
      if (family == fm) 
      {
        s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6) , host , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);

        if (s != 0) 
        {
          freeifaddrs(ifaddr);
          return false;
        }
        else
        {
          address = host;
          break;
        }
      }
    }
  }

  freeifaddrs(ifaddr);
  return true;
}

bool net_get_default_interface_address(std::string& address, int fm)
{
  std::string iface;
  if (!net_get_default_interface_name(iface))
  {
    return false;
  }
  return net_get_interface_address(iface, address, fm);
}

} // OSS

