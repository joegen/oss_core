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


namespace OSS {
namespace SIP {

class SIPTransportService;

class OSS_API SIPListener
{
public:
  
  SIPListener(SIPTransportService* pTransportService, const std::string& address, const std::string& port);
    /// Construct the server to listen on the specified TCP address and port.

  virtual ~SIPListener();
    /// Destroys the server.

  virtual void run() = 0;
    /// Run the server's io_service loop.

  virtual void handleAccept(const boost::system::error_code& e, OSS_HANDLE userData = 0) = 0;
    /// Handle completion of an asynchronous accept operation.

  virtual void handleStop() = 0;
    /// Handle a request to stop the server.

  const std::string& getAddress() const;
    /// Returns the address where the listener is bound

  const std::string& getPort() const;
    /// Returns the address where the listener is bound

  const std::string& getExternalAddress() const;
    /// Returns the external address to be used for signaling in case
    /// the server is deployed within a NAT

  void setExternalAddress(const std::string& externalAddress);
    /// Set set the external address

  SIPTransportService* getTransportService() const;
protected:
  SIPListener(const SIPListener&);
  SIPListener& operator = (const SIPListener&);
  boost::asio::io_service* _pIoService;
  SIPTransportService* _pTransportService;
    /// The io_service used to perform asynchronous operations.
  std::string _externalAddress;
private:
  std::string _address;
  std::string _port;
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

} } // OSS::SIP
#endif // SIP_SIPListener_INCLUDED

