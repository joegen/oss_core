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


#ifndef SIP_SIPListener_INCLUDED
#define SIP_SIPListener_INCLUDED

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "OSS/SIP/SIP.h"
#include "OSS/Net/IPAddress.h"


namespace OSS {
namespace SIP {

class SIPTransportService;

class OSS_API SIPListener
{
public:
  
  typedef std::vector<std::string> SubNets;
  
  SIPListener(SIPTransportService* pTransportService, const std::string& address, const std::string& port);
    /// Construct the server to listen on the specified TCP address and port.
  
  SIPListener(SIPTransportService* pTransportService, const std::string& address, const std::string& port, const std::string& alias);
    /// Construct the server to listen on the specified TCP address and port.

  virtual ~SIPListener();
    /// Destroys the server.

  virtual void run() = 0;
    /// Run the server's io_service loop.

  virtual void handleAccept(const boost::system::error_code& e, OSS_HANDLE userData = 0) = 0;
    /// Handle completion of an asynchronous accept operation.

  virtual void handleStart() = 0;
    /// Handle a request to start the server.
  
  virtual void handleStop() = 0;
    /// Handle a request to stop the server.
  
  virtual void restart(boost::system::error_code& e);
    /// Restart a temporarily closed listener.  
  
  virtual void closeTemporarily(boost::system::error_code& e);
    /// Temporarily close the transport with a intention to restart it later on
  
  virtual bool canBeRestarted() const;
    /// returns true if the listener can safely be restarted
  
  bool isEndpoint() const;
    /// returns true if this listener is an endpoint

  void setAddress(const std::string& address);
      /// Set the address

  const std::string& getAddress() const;
  /// Returns the address where the listener is bound

  void setPort(const std::string& port);
  /// Set the port
  
  const std::string& getPort() const;
    /// Returns the address where the listener is bound

  const std::string& getExternalAddress() const;
    /// Returns the external address to be used for signaling in case
    /// the server is deployed within a NAT

  void setExternalAddress(const std::string& externalAddress);
    /// Set set the external address

  SIPTransportService* getTransportService() const;
  
  const SubNets& subNets() const;
    /// Return the subnets reachable using this listener
  
  SubNets& subNets();
    /// Return the subnets reachable using this listener
  
  bool isAcceptableDestination(const std::string& address) const;
  
  bool isVirtual() const;
    /// Returns true if the IP address is a virtual address.  This is used for CARP address identification
  
  void setVirtual(bool isVirtual = true);
    /// Flag this listener as virtual
  
  bool hasStarted() const;
    /// Returns true if the listener has started accepting connections
  
  void setTransportAlias(const std::string& alias);
  
  const std::string& getTransportAlias() const;
  
  bool isV4() const;
  
  bool isV6() const;
  
  static void dumpHepPacket(OSS::Net::IPAddress::Protocol proto, const OSS::Net::IPAddress& srcAddress, const OSS::Net::IPAddress& dstAddress, const std::string& data);
   
protected:
  SIPListener(const SIPListener&);
  SIPListener& operator = (const SIPListener&);
  boost::asio::io_service* _pIoService;
  SIPTransportService* _pTransportService;
    /// The io_service used to perform asynchronous operations.
  std::string _externalAddress;
  SubNets _subNets;

  std::string _alias;
  std::string _address;
  std::string _port;
  bool _isVirtual;
  bool _hasStarted;
  bool _isEndpoint;
  bool _isRunning;
  OSS::Net::IPAddress _netAddress;
};

//
// Inlines
//

inline const std::string& SIPListener::getAddress() const
{
  return _address;
}

inline const std::string& SIPListener::getPort() const
{
  return _port;
}

inline const std::string& SIPListener::getExternalAddress() const
{
  return _externalAddress;
}

inline void SIPListener::setExternalAddress(const std::string& externalAddress)
{
  _externalAddress = externalAddress;
}

inline SIPTransportService* SIPListener::getTransportService() const
{
  return _pTransportService;
}

inline const SIPListener::SubNets& SIPListener::subNets() const
{
  return _subNets;
}
  
inline SIPListener::SubNets& SIPListener::subNets()
{
  return _subNets;
}

inline bool SIPListener::isVirtual() const
{
  return _isVirtual;
}
  
inline void SIPListener::setVirtual(bool isVirtual)
{
  _isVirtual = isVirtual;
}

inline bool SIPListener::hasStarted() const
{
  return _hasStarted;
}

inline bool SIPListener::isEndpoint() const
{
  return _isEndpoint;
}

inline void SIPListener::setAddress(const std::string& address)
{
  _address = address;
}

inline void SIPListener::setPort(const std::string& port)
{
  _port = port;
}

inline void SIPListener::setTransportAlias(const std::string& alias)
{
  _alias = alias;
}
  
inline const std::string& SIPListener::getTransportAlias() const
{
  return _alias;
}

inline bool SIPListener::isV4() const
{
  return _netAddress.address().is_v4();
}
  
inline bool SIPListener::isV6() const
{
  return _netAddress.address().is_v6();
}

} } // OSS::SIP
#endif // SIP_SIPListener_INCLUDED

