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


#ifndef SIP_SIPTransportService_INCLUDED
#define SIP_SIPTransportService_INCLUDED


#include <map>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>

#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPTCPConnectionManager.h"
#include "OSS/SIP/SIPTLSConnectionManager.h"
#include "OSS/SIP/SIPUDPListener.h"
#include "OSS/SIP/SIPTCPListener.h"
#include "OSS/SIP/SIPWebSocketListener.h"
#include "OSS/SIP/SIPTLSListener.h"



namespace OSS {
namespace SIP {


class SIPFSMDispatch;

class OSS_API SIPTransportService: private boost::noncopyable
{
public:
  typedef std::map<std::string, SIPUDPListener::Ptr> UDPListeners;
  typedef std::map<std::string, SIPTCPListener::Ptr> TCPListeners;
  typedef std::map<std::string, SIPWebSocketListener::Ptr> WSListeners;
  typedef std::map<std::string, SIPTLSListener::Ptr> TLSListeners;

  SIPTransportService(SIPFSMDispatch* pDispatch);

  ~SIPTransportService();
    /// Destroys the server.

  void initialize(const boost::filesystem::path& cfgDirectory);
    /// Initialize the service configuration using the configuration path specified.
    /// If an error occurs, this method will throw a PersistenceException.
    ///
    /// Take note that the configuration directory must be both readable and writeble
    /// by the user that owns the process.

  void deinitialize();
    /// Deinitialize the service.  This is usually called when the application
    /// is about the exit.  This is the place where the manager performs final
    /// trash management.

  void run();
    /// Run the server's io_service loop.
    /// This method will return immediately

  void stop();
    /// Stop the transport service.

  void handleStop();
    /// Handle a request to stop the transport service.

  void addUDPTransport(std::string& ip, std::string& port, const std::string& externalIp);
    /// Add a new UDP transport bound to the ip address and port.
    ///
    /// If the transport already exists, this function will throw
    /// a SIPDuplicateTransport exception

  void addTCPTransport(std::string& ip, std::string& port, const std::string& externalIp);
    /// Add a new TCP transport bound to the ip address and port.
    ///
    /// If the transport already exists, this function will throw
    /// a SIPDuplicateTransport exception

  void addWSTransport(std::string& ip, std::string& port, const std::string& externalIp);
    /// Add a new WebSocket transport bound to the ip address and port.
    ///
    /// If the transport already exists, this function will throw
    /// a SIPDuplicateTransport exception

  void addTLSTransport(
    std::string& ip, 
    std::string& port,
    const std::string& externalIp,
    const std::string& tlsCertFile,
    const std::string& diffieHellmanParamFile,
    const std::string& tlsPassword);
    /// Add a new TLS transport bound to the ip address and port.
    ///
    /// If the transport already exists, this function will throw
    /// a SIPDuplicateTransport exception

  SIPTransportSession::Ptr createClientTransport(
    const OSS::IPAddress& localAddress,
    const OSS::IPAddress& remoteAddress,
    const std::string& proto,
    const std::string& transportId = std::string());
    /// Creates a new client transport based 
    /// on local and remote address tuples

  SIPTransportSession::Ptr createClientTcpTransport(
    const OSS::IPAddress& localAddress,
    const OSS::IPAddress& remoteAddress);
    /// Creates a new client transport based
    /// on local and remote address tuples

  SIPTransportSession::Ptr createClientWsTransport(
    const OSS::IPAddress& localAddress,
    const OSS::IPAddress& remoteAddress);
    /// Creates a new client transport based
    /// on local and remote address tuples

  std::list<std::string> resolve(
    const std::string& host,
    const std::string& service = "0" );
    /// Resolves hostnames using getaddrinfo emulation

  OSS::IPAddress& defaultListenerAddress();
    /// Get the configured default listener

  void sendUDPKeepAlive(const OSS::IPAddress& localInterface,
    const OSS::IPAddress& target);
    /// send UDP Keep-alive packet

  bool isLocalTransport(const std::string& proto, const std::string& ip,
    const std::string& port) const;
    /// Returns true if the transport is a registered listener

  bool isLocalTransport(const OSS::IPAddress& transportAddress) const;
    /// Returns true if the transport is a registered listener
    /// Take note that this method only checks UDP listeners.

  void setUdpEnabled(bool enabled);
    /// Enable or disable UDP transport;

  bool isUdpEnabled() const;
    /// Flag whether UDP transport is enabled for this service

  void setTcpEnabled(bool enabled);
    /// Enable or disable TCP transport;

  bool isTcpEnabled() const;
    /// Flag whether TCP transport is enabled for this service

  void setWsEnabled(bool enabled);
    /// Enable or disable WebSocket transport;

  bool isWsEnabled() const;
    /// Flag whether WebSocket transport is enabled for this service

  void setTlsEnabled(bool enabled);
    /// Enable or disable TLS transport;

  bool isTlsEnabled() const;
    /// Flag whether TLS transport is enabled for this service

  bool getExternalAddress(
    const OSS::IPAddress& internalIp,
    std::string& externalIp) const;
    /// Return the external interface for a given internal listener

  bool getExternalAddress(
    const std::string& proto,
    const OSS::IPAddress& internalIp,
    std::string& externalIp) const;
    /// Return the external interface for a given internal listener

  bool getInternalAddress(
    const OSS::IPAddress& externalIp,
    OSS::IPAddress& internalIp) const;
    /// Return the internal IP if the host:port for the external IP is known

  bool getInternalAddress(
    const std::string& proto,
    const OSS::IPAddress& externalIp,
    OSS::IPAddress& internalIp) const;
    /// Return the internal IP if the host:port for the external IP is known

  void setTCPPortRange(unsigned short base, unsigned short max);
    /// Set the TCP port range.  Applies to both TCP and TLS transports

  void setWSPortRange(unsigned short base, unsigned short max);
    /// Set the WebSocket port range.  Applies to both WebSocket and WebSocket Secure transports

  unsigned short getTCPPortBase() const;
    /// Return the minimum port for TCP clients

  unsigned short getTCPPortMax() const;
    /// Return the maximum port for TCP clients

  boost::asio::io_service& ioService();

private:
  boost::asio::io_service _ioService;
  boost::thread* _pIoServiceThread;
  boost::asio::ip::tcp::resolver _resolver;
  SIPFSMDispatch* _pDispatch;
  SIPTCPConnectionManager _tcpConMgr;
  SIPWebSocketConnectionManager _wsConMgr;
  SIPTLSConnectionManager _tlsConMgr;
  UDPListeners _udpListeners;
  TCPListeners _tcpListeners;
  WSListeners _wsListeners;
  TLSListeners _tlsListeners;
  OSS::IPAddress _defaultListenerAddress;
  bool _udpEnabled;
  bool _tcpEnabled;
  bool _wsEnabled;
  bool _tlsEnabled;
  unsigned short _tcpPortBase;
  unsigned short _tcpPortMax;
  unsigned short _wsPortBase;
  unsigned short _wsPortMax;

};

//
// Inlines
//
inline OSS::IPAddress& SIPTransportService::defaultListenerAddress()
{
  return _defaultListenerAddress;
}

inline void SIPTransportService::setUdpEnabled(bool enabled)
{
  _udpEnabled = enabled;
}

inline bool SIPTransportService::isUdpEnabled() const
{
  return _udpEnabled;
}

inline void SIPTransportService::setTcpEnabled(bool enabled)
{
  _tcpEnabled = enabled;
}

inline bool SIPTransportService::isTcpEnabled() const
{
  return _tcpEnabled;
}

inline void SIPTransportService::setWsEnabled(bool enabled)
{
  _wsEnabled = enabled;
}

inline bool SIPTransportService::isWsEnabled() const
{
  return _wsEnabled;
}

inline void SIPTransportService::setTlsEnabled(bool enabled)
{
  _tlsEnabled = enabled;
}

inline bool SIPTransportService::isTlsEnabled() const
{
  return _tlsEnabled;
}

inline void SIPTransportService::setTCPPortRange(unsigned short base, unsigned short max)
{
  OSS_VERIFY(base < max);
  _tcpPortBase = base;
  _tcpPortMax = max;
}

inline void SIPTransportService::setWSPortRange(unsigned short base, unsigned short max)
{
  OSS_VERIFY(base < max);
  _wsPortBase = base;
  _wsPortMax = max;
}


inline unsigned short SIPTransportService::getTCPPortBase() const
{
  return _tcpPortBase;
}

inline unsigned short SIPTransportService::getTCPPortMax() const
{
  return _tcpPortMax;
}

inline boost::asio::io_service& SIPTransportService::ioService()
{
  return _ioService;
}

} } // OSS::SIP
#endif // SIP_SIPTransportService_INCLUDED

