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
//


#include "OSS/STUN/STUNMappedAddress.h"


namespace OSS {
namespace STUN {

STUNMappedAddress::STUNMappedAddress() :
  OSS::Net::IPAddress()
{

}

STUNMappedAddress::STUNMappedAddress(const std::string& address) :
  OSS::Net::IPAddress(address)
{

}

STUNMappedAddress::STUNMappedAddress(unsigned long address) :
  OSS::Net::IPAddress(address)
{

}

STUNMappedAddress::STUNMappedAddress(const boost::asio::ip::address_v4& address) :
  OSS::Net::IPAddress(address)
{

}

STUNMappedAddress::STUNMappedAddress(const boost::asio::ip::address_v6& address) :
  OSS::Net::IPAddress(address)
{

}

STUNMappedAddress::STUNMappedAddress(const IPAddress& address) :
  OSS::Net::IPAddress(address)
{

}

STUNMappedAddress::STUNMappedAddress(const std::string& address, unsigned short port) :
  OSS::Net::IPAddress(address, port)
{

}

STUNMappedAddress::STUNMappedAddress(const std::string& address, unsigned short port,
    const std::string& mappedAddress, unsigned short mappedPort) :
  OSS::Net::IPAddress(address, port),
  _mappedAddress(mappedAddress, mappedPort)
{

}

STUNMappedAddress::STUNMappedAddress(const IPAddress& address, const IPAddress& mappedAddress) :
  OSS::Net::IPAddress(address),
  _mappedAddress(mappedAddress)
{

}

STUNMappedAddress::STUNMappedAddress(const STUNMappedAddress& address) :
  OSS::Net::IPAddress(address),
  _mappedAddress(address._mappedAddress),
  _stunServer(address._stunServer)
{

}

STUNMappedAddress::~STUNMappedAddress()
{

}

OSS::Net::IPAddress STUNMappedAddress::getMappedAddress(
  const STUNClient::Ptr& stunClient,
  const std::string& stunServer,
  boost::asio::ip::udp::socket& socket)
{
  if (!socket.is_open())
    return OSS::Net::IPAddress();

  *((OSS::Net::IPAddress*)this) = OSS::Net::IPAddress(socket.local_endpoint().address().to_v4());
  setPort(socket.local_endpoint().port());

  _stunServer = stunServer;
  _mappedAddress = OSS::Net::IPAddress();

  stunClient->createSingleSocket(
    stunServer,
    socket,
    *this,
    _mappedAddress);
  
  return _mappedAddress;
}

STUNMappedAddress& STUNMappedAddress::operator= (const STUNMappedAddress& clone)
{
  STUNMappedAddress clonable(clone);
  swap(clonable);
  return *this;
}

void STUNMappedAddress::swap(STUNMappedAddress& clone)
{
  std::swap(dynamic_cast<OSS::Net::IPAddress&>(clone), dynamic_cast<OSS::Net::IPAddress&>(*this));
  std::swap(clone._mappedAddress, _mappedAddress);
  std::swap(clone._stunServer, _stunServer);
}

bool STUNMappedAddress::operator < (const STUNMappedAddress& other) const
{
  return dynamic_cast<const OSS::Net::IPAddress&>(*this) < dynamic_cast<const OSS::Net::IPAddress&>(other);
}


} } // OSS::STUN







