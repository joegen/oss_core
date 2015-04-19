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

#include "Poco/Net/NetworkInterface.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/Net.h"
#include "OSS/ABNF/ABNFSIPIPV4Address.h"
#include "OSS/ABNF/ABNFSIPIPV6Address.h"


namespace OSS {
namespace Net {

using boost::asio::ip::address_v4;
using boost::asio::ip::address_v6;
using boost::system::error_code;

IPAddress::IPAddress() :
  _address(),
  _port(0),
  _cidr(0),
  _isVirtual(false)
{
}

IPAddress::IPAddress(const std::string& address) :
  _port(0),
  _cidr(0),
  _isVirtual(false)
{
  try
  {
    _address = boost::asio::ip::address(boost::asio::ip::address::from_string(address));
  }
  catch(...)
  {
    _address = boost::asio::ip::address();
  }
}

IPAddress::IPAddress(unsigned long address) :
  _address(address_v4(address)),
  _port(0),
  _cidr(0),
  _isVirtual(false)
{
}

IPAddress::IPAddress(const boost::asio::ip::address_v4& address) :
  _address(address),
  _port(0),
  _cidr(0),
  _isVirtual(false)
{
}

IPAddress::IPAddress(const boost::asio::ip::address_v6& address) :
  _address(address),
  _port(0),
  _cidr(0),
  _isVirtual(false)
{
}

IPAddress::IPAddress(const IPAddress& address)
{
  _address = address._address;
  _externalAddress = address._externalAddress;
  _port = address._port;
  _cidr = address._cidr;
  _isVirtual = address._isVirtual;

}

IPAddress::IPAddress(const std::string& address, unsigned short port) :
  _cidr(0)
{
  try
  {
    _address = boost::asio::ip::address(boost::asio::ip::address::from_string(address));
    _port = port;
  }
  catch(...)
  {
    _address = boost::asio::ip::address();
    _port = 0;
  }
  
}

void IPAddress::swap(IPAddress& address)
{
  std::swap(_address, address._address);
  std::swap(_externalAddress, address._externalAddress);
  std::swap(_port, address._port);
  std::swap(_cidr, address._cidr);
  std::swap(_isVirtual, address._isVirtual);
}

bool IPAddress::isPrivate()
{
  if (_address.is_v6())
    return false;

  unsigned long address = _address.to_v4().to_ulong();
	// 10/8 (10.0.0.0 - 10.255.255.255)
	if ( address >= 0x0a000000 && address <= 0x0affffff )
		return true;
	// 172.16/12 (172.16.0.0 - 172.31.255.255)
	if ( address >= 0xac100000 && address <= 0xac1fffff )
		return true;
	// 192.168/16 (192.168.0.0 - 192.168.255.255)
	if ( address >= 0xc0a80000 && address <= 0xc0a8ffff )
		return true;

  return false;
}

IPAddress IPAddress::fromHost(const char* host)
{
  try
  {
    boost::asio::ip::tcp::resolver::query query(host, "0");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = OSS::net_resolver().resolve(query);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator_end;
    if (endpoint_iterator != endpoint_iterator_end)
    {
      boost::asio::ip::tcp::endpoint ep = *endpoint_iterator;
      return IPAddress(ep.address().to_string());
    }
  }
  catch(...)
  {
  }
  return IPAddress();
}

IPAddress IPAddress::fromV4IPPort(const char* ipPortTuple)
{
  std::vector<std::string> tokens = OSS::string_tokenize(ipPortTuple, ":");
  IPAddress addr;
  if (tokens.size() == 1)
    addr = fromHost(tokens[0].c_str());
  else if(tokens.size() == 2)
  {
    addr = fromHost(tokens[0].c_str());
    addr.setPort(OSS::string_to_number<unsigned short>(tokens[1].c_str()));
  }
  return addr;
}

bool IPAddress::isLocalAddress() const
{
  //
  // Check if this is a real interface on this server
  //
  try
  {
    Poco::Net::IPAddress localIP(toString());
    Poco::Net::NetworkInterface::forAddress(localIP);
  }
  catch(...)
  {
    return false;
  }

  return true;
}

bool IPAddress::isV4Address(const std::string& address)
{
  static OSS::ABNF::ABNFEvaluate<OSS::ABNF::ABNFSIPIPV4Address> isIPV4;
  return isIPV4(address.c_str());
}

  
bool IPAddress::isV6Address(const std::string& address)
{
  static OSS::ABNF::ABNFEvaluate<OSS::ABNF::ABNFSIPIPV6Address> isIPV6;
  return isIPV6(address.c_str());
}

bool IPAddress::isIPAddress(const std::string& address)
{
  return IPAddress::isV4Address(address) || IPAddress::isV6Address(address); 
}

bool IPAddress::toSockAddr(SockAddr& socketaddr_) const
{
  memset((void *) &socketaddr_, 0, sizeof(struct sockaddr_storage));
  
  if (address().is_v4())
  {
    boost::system::error_code e;
    std::string addr = address().to_v4().to_string(e);
    if (e)
    {
      return false;
    }
    inet_pton(AF_INET, addr.c_str(), &socketaddr_.s4.sin_addr);
    socketaddr_.s4.sin_family = AF_INET;
    socketaddr_.s4.sin_port = htons(getPort());
    return true;
  }
  else if (address().is_v6())
  {
    boost::system::error_code e;
    std::string addr = address().to_v6().to_string(e);
    if (e)
    {
      return false;
    }
    inet_pton(AF_INET6, addr.c_str(), &socketaddr_.s6.sin6_addr);
    socketaddr_.s6.sin6_family = AF_INET6;
    socketaddr_.s6.sin6_port = htons(getPort());
    return true;
  }
  else
  {
    return false;
  }
}

IPAddress IPAddress::fromSockAddr4(sockaddr_in& in)
{
  IPAddress ip;
  char ipbuf[256];
  unsigned short port;
  inet_ntop(AF_INET, &in.sin_addr, ipbuf, sizeof(struct sockaddr_in));
  port = ntohs(in.sin_port);
  ip = OSS::Net::IPAddress(ipbuf);
  ip.setPort(port);
  return ip;
}
IPAddress IPAddress::fromSockAddr6(sockaddr_in6& in6)
{
  IPAddress ip;
  char ipbuf[256];
  unsigned short port;
  inet_ntop(AF_INET, &in6.sin6_addr, ipbuf, sizeof(struct sockaddr_in6));
  port = ntohs(in6.sin6_port);
  ip = OSS::Net::IPAddress(ipbuf);
  ip.setPort(port);
  return ip;
}


} } // OSS::Net


