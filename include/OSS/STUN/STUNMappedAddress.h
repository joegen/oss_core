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


#ifndef OSS_STUNMAPPEDADDRES_H
#define OSS_STUNMAPPEDADDRES_H

#include "OSS/build.h"
#if ENABLE_FEATURE_STUN

#include "OSS/Net/IPAddress.h"
#include "OSS/STUN/STUNClient.h"

namespace OSS {
namespace STUN {


class OSS_API STUNMappedAddress : public OSS::Net::IPAddress
{
public:
  STUNMappedAddress();
    /// Default constructor

  explicit STUNMappedAddress(const std::string& address);
    /// Create STUNMappedAddresss from a string

  explicit STUNMappedAddress(unsigned long address);
    /// Create STUNMappedAddress from an unsigned long

  explicit STUNMappedAddress(const boost::asio::ip::address_v4& address);
    /// Create STUNMappedAddress from an asio address_v4

  explicit STUNMappedAddress(const boost::asio::ip::address_v6& address);
    /// Create STUNMappedAddress from an asio address_v6

  explicit STUNMappedAddress(const IPAddress& address);
    /// Create STUNMappedAddress from an asio IPAddress

  STUNMappedAddress(const std::string& address, unsigned short port);
    /// Create STUNMappedAddress address with port

  STUNMappedAddress(const std::string& address, unsigned short port,
    const std::string& mappedAddress, unsigned short mappedPort);
    /// Create STUNMappedAddress address with port

  STUNMappedAddress(const IPAddress& address, const IPAddress& mappedAddress);
    /// Create STUNMappedAddress from an asio IPAddress

  STUNMappedAddress(const STUNMappedAddress& address);
    /// Copy constructor

  ~STUNMappedAddress();
    /// Destroys the STUNMappedAddress

  const OSS::Net::IPAddress& getMappedAddress() const;
    /// Returns a constant reference to the mapped address

  void setMappedAddress(const OSS::Net::IPAddress& mappedAddress);
    /// Manually set the mapped address

  const std::string& getStunServer() const;
    /// Returns the stun server

  void setStunServer(const std::string& stunServer);
    /// Manually set the stun server

  OSS::Net::IPAddress getMappedAddress(
    const STUNClient::Ptr& stunClient,
    const std::string& stunServer,
    boost::asio::ip::udp::socket& socket);
    /// Return the mapped address for a particular socket.
    /// Take note that the socket must alread by open prior to
    /// calling this function


  STUNMappedAddress& operator= (const STUNMappedAddress& clone);

  void swap(STUNMappedAddress& clone);

  bool operator < (const STUNMappedAddress& other) const;

private:
  OSS::Net::IPAddress _mappedAddress;
  std::string _stunServer;
};

//
// Inlines
//

inline const OSS::Net::IPAddress& STUNMappedAddress::getMappedAddress() const
{
  return _mappedAddress;
}

inline void STUNMappedAddress::setMappedAddress(const OSS::Net::IPAddress& mappedAddress)
{
  _mappedAddress = mappedAddress;
}

inline const std::string& STUNMappedAddress::getStunServer() const
{
  return _stunServer;
}

inline void STUNMappedAddress::setStunServer(const std::string& stunServer)
{
  _stunServer = stunServer;
}

} } // OSS::STUN

#endif // ENABLE_FEATURE_STUN

#endif // OSS_STUNMAPPEDADDRES_H



