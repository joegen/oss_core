// OSS Software Solutions Application Programmer Interface
// Package: OSSAPI
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSSAPI API.
//
// Copyright (c) OSS Software Solutions
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


#ifndef OSS_IPAddress_H_INCLUDED
#define OSS_IPAddress_H_INCLUDED

#include <boost/asio.hpp>

#include "OSS/OSS.h"


namespace OSS {

class OSS_API IPAddress
  /// This is a helper class on top of the asio address_v4 and address_v6 objects
{
public:
  IPAddress();
    /// Default constructor

  explicit IPAddress(const std::string& address);
    /// Create an address from a string

  explicit IPAddress(unsigned long address);
    /// Create an address from an unsigned long

  explicit IPAddress(const boost::asio::ip::address_v4& address);
    /// Create and IP address from an asio address_v4

  explicit IPAddress(const boost::asio::ip::address_v6& address);
    /// Create and IP address from an asio address_v6

  IPAddress(const std::string& address, unsigned short port);
    /// Create a new IP address with port

  IPAddress(const IPAddress& address);
    /// Copy constructor

  IPAddress& operator = (const std::string& address);
    /// Copy an address from a string

  IPAddress& operator = (unsigned long address);
    /// Copy an address from an unsigned long

  IPAddress& operator = (const boost::asio::ip::address& address);
    /// Copy and IP address from an asio address_v4 or address_v6

  IPAddress& operator = (const IPAddress& address);
    /// Copy and IP address from another IPAddress

  bool operator == (const boost::asio::ip::address& address) const;
    /// Equality operator against another address

  bool operator == (const IPAddress& address) const;
    /// Equality operator against another address

  bool operator < (const boost::asio::ip::address& address) const;
    /// Lessthan operator against another address

  bool operator < (const IPAddress& address) const;
    /// Lessthan operator against another address

  bool operator > (const boost::asio::ip::address& address) const;
    /// Greater operator against another address

  bool operator > (const IPAddress& address) const;
    /// Greater operator against another address

  bool operator != (const boost::asio::ip::address& address) const;
    /// Non-equality operator against another address

  bool operator != (const IPAddress& address) const;
    /// Non-equality operator against another address

  bool compare(const IPAddress& address, bool includePort) const;
    /// Compare hosts and optional port

  void swap(IPAddress& address);
    /// Exchanges the content of two messages.
  
  std::string toString() const;
    /// Get the address in its string format

  unsigned short& cidr();
    /// Return the CIDR segment of a CIDR address

  const unsigned short& getPort() const;
    /// Return the port element of an address:port tuple

  void setPort(unsigned short port);
    /// Set the port number

  std::string toCidrString() const;
    /// Get the address in CIDR format

  std::string toIpPortString() const;
    /// Get the address in ip:port format

  bool isValid() const;
    /// Return true if the address is valid

  boost::asio::ip::address& address();
    /// Return a direct reference to the asio address

  std::string& externalAddress();
    /// Return the external address.  This is a custom property used
    /// by applications that would want to retain a map between an internal
    /// and external addresses in cases of servers deplyed within NAT

  bool isPrivate();
    /// Returns true if the IP address is of private type.  eg 192.168.x.x

  bool isLocalAddress() const;
    /// Returns true if this IP Address is bound to a local interface

  static IPAddress fromHost(const char* host);
    /// Returns an IP address from a host

  static IPAddress fromV4IPPort(const char* ipPortTuple);
    /// Returns an IP Address from IP:PORT tupple

  static IPAddress fromV4DWORD(OSS::UInt32 ip4);
    /// Returns an IP Addres from a DWORD

protected:
  boost::asio::ip::address _address;
  std::string _externalAddress;
  unsigned short _port;
  unsigned short _cidr;
};

//
// Inlines
//

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const IPAddress& addr)
{
  os << addr.toString();
  return os;
}

inline boost::asio::ip::address& IPAddress::address()
{
  return _address;
}

inline std::string& IPAddress::externalAddress()
{
  return _externalAddress;
}


inline const unsigned short& IPAddress::getPort() const
{
  return _port;
}

inline void IPAddress::setPort(unsigned short port)
{
  _port = port;
}

inline unsigned short& IPAddress::cidr()
{
  return _cidr;
}

inline IPAddress& IPAddress::operator = (const std::string& address)
{
  try
  {
    this->_address = boost::asio::ip::address(boost::asio::ip::address::from_string(address));
  }
  catch(...)
  {
    this->_address = boost::asio::ip::address();
  }
  return *this;
}

inline IPAddress& IPAddress::operator = (unsigned long address)
{
  this->_address = boost::asio::ip::address(boost::asio::ip::address_v4(address));
  return *this;
}

inline IPAddress& IPAddress::operator = (const boost::asio::ip::address& address)
{
  this->_address = address;
  return *this;
}

inline IPAddress& IPAddress::operator = (const IPAddress& address)
{
  IPAddress swapable(address);
  swap(swapable);
  return *this;
}

inline bool IPAddress::operator == (const boost::asio::ip::address& address) const
{
  return _address == address;
}

inline bool IPAddress::operator == (const IPAddress& address) const
{
  return _address == address._address && _port == address._port;
}

inline bool IPAddress::compare(const IPAddress& address, bool includePort) const
{
  if (includePort)
    return _address == address._address && _port == address._port;
  else
    return _address == address._address;
}

inline bool IPAddress::operator < (const boost::asio::ip::address& address) const
{
  return _address < address;
}

inline bool IPAddress::operator < (const IPAddress& address) const
{
  if (_address == address._address)
    return _port < address._port;
  return _address < address._address;
}

inline bool IPAddress::operator > (const boost::asio::ip::address& address) const
{
  return !IPAddress::operator < (address);
}

inline bool IPAddress::operator > (const IPAddress& address) const
{
  return !IPAddress::operator < (address);
}

inline bool IPAddress::operator != (const boost::asio::ip::address& address) const
{
  return _address != address;
}

inline bool IPAddress::operator != (const IPAddress& address) const
{
  return _address != address._address || _port != address._port;
}
  
inline std::string IPAddress::toCidrString() const
{
  boost::system::error_code e;
  std::string ipString = _address.to_string(e);
  if (e)
    return "";

  std::stringstream strm;
  strm << ipString << "/" << _cidr;
  return strm.str();
}

inline std::string IPAddress::toIpPortString() const
{
  boost::system::error_code e;
  std::string ipString = _address.to_string(e);
  if (e)
    return "";
  std::stringstream strm;
  strm << ipString;
  if (_port != 0)
    strm << ":" << _port;
  return strm.str();
}

inline bool IPAddress::isValid() const
{
  if (_address.is_v4())
    return _address.to_v4().to_ulong() != 0;
  else if(_address.is_v6())
    return true;
  return false;
}

inline std::string IPAddress::toString() const
{
  try
  {
    return _address.to_string();
  }
  catch(...)
  {
    return "";
  }
}

inline IPAddress IPAddress::fromV4DWORD(OSS::UInt32 ip4)
{
  return IPAddress(ip4);
}

} // OSS
#endif // OSS_IPAddress_H_INCLUDED

