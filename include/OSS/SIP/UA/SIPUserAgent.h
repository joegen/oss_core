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

#ifndef OSS_SIPUSERAGENT_H_INCLUDED
#define	OSS_SIPUSERAGENT_H_INCLUDED


#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/Net.h"


namespace OSS {
namespace SIP {
namespace UA {


class SIPUserAgent : boost::noncopyable
{
public:
  
  typedef boost::function<void()> ExitHandler; 
  
  SIPUserAgent( 
    const std::string& softwareId, //Software identifier
    OSS::UInt32 ctsz = 32, // Size of client transactions hashtable (power of 2)
    OSS::UInt32 stsz = 32, // Size of server transactions hashtable (power of 2)
    OSS::UInt32 tcsz = 32 // Size of SIP transport hashtable (power of 2)
  );
  
  ~SIPUserAgent();
  
  bool run();
  
  void stop(bool force /*Don't wait for transactions to complete*/);
   
  bool isRunning() const;
  
  void setExitHandler(ExitHandler exitHandler);
  
  OSS::socket_address_list& udpListeners();
    /// Returns the UDP Listener vector.
    ///
    /// All interfaces where the UDP listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts 
    /// a tuple object of type SIPStack::OSS::Net::IPAddress.
    ///
    /// This must be set before calling the SIPStack::run() method.

  OSS::socket_address_list& tcpListeners();
    /// Returns the TCP Listener vector.
    ///
    /// All interfaces where the TCP listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts 
    /// a tuple object of type OSS::OSS::Net::IPAddress.
    ///
    /// This must be set before calling the SIPStack::run() method

  OSS::socket_address_list& tlsListeners();
    /// Returns the TLS Listener vector.
    ///
    /// All interfaces where the TLS listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts 
    /// a tuple object of type OSS::OSS::Net::IPAddress.
    ///
    /// This must be set before calling the SIPStack::run() method
  
  OSS_HANDLE sip_stack_handle();
  OSS_HANDLE dns_client_handle();
  
protected:
  bool transportInit();
    /// Initialize the SIP Transport.
  friend void ua_exit_handler(void *);
  ExitHandler _exitHandler;
  
private:
  
  bool _isRunning;
  bool _isTransportReady;
  OSS_HANDLE _sip_stack_handle;
  OSS_HANDLE _dns_client_handle;
  OSS::socket_address_list _udpListeners;
  OSS::socket_address_list _tcpListeners;
  OSS::socket_address_list _tlsListeners;
};


//
// Inlines
//
inline OSS_HANDLE SIPUserAgent::sip_stack_handle()
{
  return _sip_stack_handle;
}

inline OSS_HANDLE SIPUserAgent::dns_client_handle()
{
  return _dns_client_handle;
}

inline bool SIPUserAgent::isRunning() const
{
  return _isRunning;
}

inline void SIPUserAgent::setExitHandler(ExitHandler exitHandler)
{
  _exitHandler = exitHandler;
}

inline OSS::socket_address_list& SIPUserAgent::udpListeners()
{
  return _udpListeners;
}

inline OSS::socket_address_list& SIPUserAgent::tcpListeners()
{
  return _tcpListeners;
}

inline OSS::socket_address_list& SIPUserAgent::tlsListeners()
{
  return _tlsListeners;
}

} } } // OSS::SIP::UA
  
#endif	// OSS_SIPUSERAGENT_H_INCLUDED

