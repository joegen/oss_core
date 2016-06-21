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


#include "OSS/SIP/SIPTransportService.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {

 
SIPTransportService::SIPTransportService(const SIPTransportSession::Dispatch& dispatch):
  _ioService(),
  _pIoServiceThread(0),
  _resolver(_ioService),
  _tlsServerContext(_ioService, boost::asio::ssl::context::sslv23_server),
  _tlsClientContext(_ioService, boost::asio::ssl::context::sslv23_client),
  _dispatch(dispatch),
  _tcpConMgr(_dispatch),
  _tlsConMgr(_dispatch),
  _udpListeners(),
  _tcpListeners(),
  _tlsListeners(),
  _udpEnabled(true),
  _tcpEnabled(true),
  _tlsEnabled(false),
  _tcpPortBase(10000),
  _tcpPortMax(20000)
#if ENABLE_FEATURE_WEBSOCKETS
 ,_wsConMgr(_dispatch),
  _wsEnabled(true),
  _wsPortBase(10000),
  _wsPortMax(20000)
#endif
{
}

SIPTransportService::~SIPTransportService()
{
  stop();
}

void SIPTransportService::initialize(const boost::filesystem::path& cfgDirectory)
{

}

void SIPTransportService::deinitialize()
{

}

void SIPTransportService::run()
{

  assert(!_pIoServiceThread);
  

  for (UDPListeners::iterator iter = _udpListeners.begin(); iter != _udpListeners.end(); iter++)
  {
    if (!iter->second->isVirtual() && !iter->second->hasStarted())
    {
      iter->second->run();
      OSS_LOG_INFO("Started UDP Listener " << iter->first);
    }
  }

  for (TCPListeners::iterator iter = _tcpListeners.begin(); iter != _tcpListeners.end(); iter++)
  {
    if (!iter->second->isVirtual() && !iter->second->hasStarted())
    {
      iter->second->run();
      OSS_LOG_INFO("Started TCP Listener " << iter->first);
    }
  }

#if ENABLE_FEATURE_WEBSOCKETS  
  for (WSListeners::iterator iter = _wsListeners.begin(); iter != _wsListeners.end(); iter++)
  {
    if (!iter->second->isVirtual() && !iter->second->hasStarted())
    {
      iter->second->run();
      OSS_LOG_INFO("Started WebSocket Listener " << iter->first);
    }
  }
#endif

  for (TLSListeners::iterator iter = _tlsListeners.begin(); iter != _tlsListeners.end(); iter++)
  {
    if (!iter->second->isVirtual() && !iter->second->hasStarted())
    {
      iter->second->run();
      OSS_LOG_INFO("Started TLS Listener " << iter->first);
    }
  }

  _pIoServiceThread = new boost::thread(boost::bind(&boost::asio::io_service::run, &_ioService));
}

void SIPTransportService::runVirtualTransports()
{
  for (UDPListeners::iterator iter = _udpListeners.begin(); iter != _udpListeners.end(); iter++)
  {
    if (iter->second->isVirtual())
    {
      if (!iter->second->hasStarted())
      {
        iter->second->run();
        OSS_LOG_INFO("Started Virtual UDP Listener " << iter->first);
      }
      else
      {
        boost::system::error_code e;
        iter->second->restart(e);
        
        if (e)
        {
          OSS_LOG_ERROR("SIPTransportService::runVirtualTransports(UDP) Exception: " << e.message());
        }
      }
    }
  }

  for (TCPListeners::iterator iter = _tcpListeners.begin(); iter != _tcpListeners.end(); iter++)
  {
    if (iter->second->isVirtual())
    {
      if (!iter->second->hasStarted())
      {
        iter->second->run();
        OSS_LOG_INFO("Started Virtual TCP Listener " << iter->first);
      }
      else
      {
        boost::system::error_code e;
        iter->second->restart(e);
        
        if (e)
        {
          OSS_LOG_ERROR("SIPTransportService::runVirtualTransports(TCP) Exception: " << e.message());
        }
      }
    }
  }

#if ENABLE_FEATURE_WEBSOCKETS
  for (WSListeners::iterator iter = _wsListeners.begin(); iter != _wsListeners.end(); iter++)
  {
    if (iter->second->isVirtual())
    {
      if (!iter->second->hasStarted())
      {
        iter->second->run();
        OSS_LOG_INFO("Started Virtual WS Listener " << iter->first);
      }
      else
      {
        boost::system::error_code e;
        iter->second->restart(e);
        
        if (e)
        {
          OSS_LOG_ERROR("SIPTransportService::runVirtualTransports(WS) Exception: " << e.message());
        }
      }
    }
  }
#endif

  for (TLSListeners::iterator iter = _tlsListeners.begin(); iter != _tlsListeners.end(); iter++)
  {
    if (iter->second->isVirtual())
    {
      if (!iter->second->hasStarted())
      {
        iter->second->run();
        OSS_LOG_INFO("Started Virtual TLS Listener " << iter->first);
      }
      else
      {
        boost::system::error_code e;
        iter->second->restart(e);
        
        if (e)
        {
          OSS_LOG_ERROR("SIPTransportService::runVirtualTransports(TLS) Exception: " << e.message());
        }
      }
    }
  }
}
  
void SIPTransportService::stopVirtualTransports()
{
  for (UDPListeners::iterator iter = _udpListeners.begin(); iter != _udpListeners.end(); iter++)
  {
    if (iter->second->isVirtual())
    {
      if (iter->second->hasStarted())
      {
        boost::system::error_code e;
        OSS_LOG_INFO("Closing Virtual UDP Listener " << iter->first);
        iter->second->closeTemporarily(e);
        if (e)
        {
          OSS_LOG_ERROR("SIPTransportService::stopVirtualTransports(UDP) Exception: " << e.message());
        }
      }
    }
  }

  for (TCPListeners::iterator iter = _tcpListeners.begin(); iter != _tcpListeners.end(); iter++)
  {
    if (iter->second->isVirtual())
    {
      if (iter->second->hasStarted())
      {
        boost::system::error_code e;
        OSS_LOG_INFO("Closing Virtual TCP Listener " << iter->first);
        iter->second->closeTemporarily(e);
        if (e)
        {
          OSS_LOG_ERROR("SIPTransportService::stopVirtualTransports(TCP) Exception: " << e.message());
        }
      }
    }
  }

 #if ENABLE_FEATURE_WEBSOCKETS
  for (WSListeners::iterator iter = _wsListeners.begin(); iter != _wsListeners.end(); iter++)
  {
    if (iter->second->isVirtual())
    {
      if (iter->second->hasStarted())
      {
        boost::system::error_code e;
        OSS_LOG_INFO("Closing Virtual WS Listener " << iter->first);
        iter->second->closeTemporarily(e);
        if (e)
        {
          OSS_LOG_ERROR("SIPTransportService::stopVirtualTransports(WS) Exception: " << e.message());
        }
      }
    }
  }
#endif
  
  for (TLSListeners::iterator iter = _tlsListeners.begin(); iter != _tlsListeners.end(); iter++)
  {
    if (iter->second->isVirtual())
    {
      if (iter->second->hasStarted())
      {
        boost::system::error_code e;
        OSS_LOG_INFO("Closing Virtual TLS Listener " << iter->first);
        iter->second->closeTemporarily(e);
        if (e)
        {
          OSS_LOG_ERROR("SIPTransportService::stopVirtualTransports(TLS) Exception: " << e.message());
        }
      }
    }
  }
}

void SIPTransportService::stop()
{
  // Post a call to the stop function so that server::stop() is safe to call
  // from any thread.
  _ioService.post(boost::bind(&SIPTransportService::handleStop, this));
  if (_pIoServiceThread)
  {
    _pIoServiceThread->join();
    delete _pIoServiceThread;
    _pIoServiceThread = 0;
  }
}

void SIPTransportService::handleStop()
{
  UDPListeners::iterator udpIter;
  for (udpIter = _udpListeners.begin(); udpIter != _udpListeners.end(); udpIter++)
    udpIter->second->handleStop();
  
  TCPListeners::iterator tcpIter;
  for (tcpIter = _tcpListeners.begin(); tcpIter != _tcpListeners.end(); tcpIter++)
    tcpIter->second->handleStop();
#if ENABLE_FEATURE_WEBSOCKETS
  WSListeners::iterator wsIter;
  for (wsIter = _wsListeners.begin(); wsIter != _wsListeners.end(); wsIter++)
    wsIter->second->handleStop();
#endif

  TLSListeners::iterator tlsIter;
  for (tlsIter = _tlsListeners.begin(); tlsIter != _tlsListeners.end(); tlsIter++)
    tlsIter->second->handleStop();

  _ioService.stop();

  _udpListeners.clear();
  _tcpListeners.clear();
  _tlsListeners.clear();
}

bool SIPTransportService::isLocalTransport(const OSS::Net::IPAddress& transportAddress) const
{
  std::string key = transportAddress.toIpPortString();
  if (_udpListeners.find(key) != _udpListeners.end())
    return true;
  else if (_tcpListeners.find(key) != _tcpListeners.end())
    return true;
#if ENABLE_FEATURE_WEBSOCKETS
  else if (_wsListeners.find(key) != _wsListeners.end())
    return true;
#endif
  else if (_tlsListeners.find(key) != _tlsListeners.end())
    return true;
  return false;
}

bool SIPTransportService::isLocalTransport(const std::string& proto,
    const std::string& ip,
    const std::string& port) const
{
  std::string key;
  OSS::string_sprintf_string<256>(key, "%s:%s", ip.c_str(), port.c_str());
  if (proto == "udp")
    return _udpListeners.find(key) != _udpListeners.end();
  else if (proto == "tcp")
    return _tcpListeners.find(key) != _tcpListeners.end();
#if ENABLE_FEATURE_WEBSOCKETS
  else if (proto == "ws")
    return _wsListeners.find(key) != _wsListeners.end();
#endif
  else if (proto == "tls")
    return _tlsListeners.find(key) != _tlsListeners.end();
  return false;
}

const SIPListener* SIPTransportService::getTransportForDestination(const std::string& proto, const std::string& address) const
{
  if (proto == "udp" || proto == "UDP")
  {
    for (UDPListeners::const_iterator iter = _udpListeners.begin(); iter != _udpListeners.end(); iter++)
    {
      if (iter->second->isAcceptableDestination(address))
      {
        return iter->second.get();
      }
    }
  }
  else if (proto == "tcp" || proto == "TCP")
  {
    for (TCPListeners::const_iterator iter = _tcpListeners.begin(); iter != _tcpListeners.end(); iter++)
    {
      if (iter->second->isAcceptableDestination(address))
      {
        return iter->second.get();
      }
    }
  }
  else if (proto == "tls" || proto == "TLS")
  {
    for (TLSListeners::const_iterator iter = _tlsListeners.begin(); iter != _tlsListeners.end(); iter++)
    {
      if (iter->second->isAcceptableDestination(address))
      {
        return iter->second.get();
      }
    }
  }
#if ENABLE_FEATURE_WEBSOCKETS
  else if (proto == "ws" || proto == "WS")
  {
    for (WSListeners::const_iterator iter = _wsListeners.begin(); iter != _wsListeners.end(); iter++)
    {
      if (iter->second->isAcceptableDestination(address))
      {
        return iter->second.get();
      }
    }
  }
#endif
  
  return 0;
}

bool SIPTransportService::addEndpoint(EndpointListener* pEndpoint)
{
  std::string key = pEndpoint->getEndpointName();
  OSS_LOG_NOTICE("Adding Endpoint " << key);
  
  if (_endpoints.find(key) != _endpoints.end())
  {
    OSS_LOG_ERROR("Duplicate Endpoint " << key);
    return false;
  }
  
  pEndpoint->setDispatch(_dispatch);
  
  _endpoints[key] = pEndpoint;
  
  return true;
}
void SIPTransportService::addUDPTransport(const std::string& ip, const std::string& port, const std::string& externalIp, const SIPListener::SubNets& subnets, bool isVirtualIp, const std::string& alias)
{
  OSS_LOG_INFO("Adding UDP SIP Listener " << ip << ":" << port << " (" << externalIp << ")");
  std::string key;
  OSS::string_sprintf_string<256>(key, "%s:%s", ip.c_str(), port.c_str());
  if (_udpListeners.find(key) != _udpListeners.end())
    throw OSS::SIP::SIPException("Duplicate UDP Transport detected while calling addUDPTransport()");
  SIPUDPListener::Ptr udpListener(new SIPUDPListener(this, _dispatch, ip, port));
  
  udpListener->setVirtual(isVirtualIp);
  udpListener->setExternalAddress(externalIp);
  udpListener->subNets() = subnets;
  _udpListeners[key] = udpListener;
  
  if (!alias.empty())
  {
    udpListener->setTransportAlias(alias);
    _udpListeners[alias] = udpListener;
  }

  boost::system::error_code ec;
  boost::asio::ip::address whiteList = boost::asio::ip::address::from_string(ip, ec);
  if (!ec)
    SIPTransportSession::rateLimit().clearAddress(whiteList, true);

  OSS_LOG_INFO("UDP SIP Listener " << ip << ":" << port << " (" << externalIp << ") ACTIVE");
}

void SIPTransportService::addTCPTransport(const std::string& ip, const std::string& port, const std::string& externalIp, const SIPListener::SubNets& subnets, bool isVirtualIp, const std::string& alias)
{
  OSS_LOG_INFO("Adding TCP SIP Listener " << ip << ":" << port << " (" << externalIp << ")");
  std::string key;
  OSS::string_sprintf_string<256>(key, "%s:%s", ip.c_str(), port.c_str());
  if (_tcpListeners.find(key) != _tcpListeners.end())
    throw OSS::SIP::SIPException("Duplicate TCP transport while calling addTCPTransport()");
  SIPTCPListener::Ptr pTcpListener = SIPTCPListener::Ptr(new SIPTCPListener(this, _dispatch, ip, port, _tcpConMgr));
  
  pTcpListener->setVirtual(isVirtualIp);
  pTcpListener->setExternalAddress(externalIp);
  pTcpListener->subNets() = subnets;
  _tcpListeners[key] = pTcpListener;
  
  if (!alias.empty())
  {
    pTcpListener->setTransportAlias(alias);
    _tcpListeners[alias] = pTcpListener;
  }
  
  boost::system::error_code ec;
  boost::asio::ip::address whiteList = boost::asio::ip::address::from_string(ip, ec);
  if (!ec)
    SIPTransportSession::rateLimit().clearAddress(whiteList, true);
  OSS_LOG_INFO("TCP SIP Listener " << ip << ":" << port << " (" << externalIp << ") ACTIVE");
}

#if ENABLE_FEATURE_WEBSOCKETS
void SIPTransportService::addWSTransport(const std::string& ip, const std::string& port, const std::string& externalIp, const SIPListener::SubNets& subnets, bool isVirtualIp, const std::string& alias)
{
  OSS_LOG_INFO("Adding WebSocket SIP Listener " << ip << ":" << port << " (" << externalIp << ")");
  std::string key;
  OSS::string_sprintf_string<256>(key, "%s:%s", ip.c_str(), port.c_str());
  if (_wsListeners.find(key) != _wsListeners.end())
    throw OSS::SIP::SIPException("Duplicate WebSocket transport while calling addWSTransport()");
  SIPWebSocketListener::Ptr pWsListener = SIPWebSocketListener::Ptr(new SIPWebSocketListener(this, ip, port, _wsConMgr));
  
  pWsListener->setVirtual(isVirtualIp);
  pWsListener->setExternalAddress(externalIp);
  pWsListener->subNets() = subnets;
  _wsListeners[key] = pWsListener;
  
  if (!alias.empty())
  {
    pWsListener->setTransportAlias(alias);
    _wsListeners[alias] = pWsListener;
  }
  
  boost::system::error_code ec;
  boost::asio::ip::address whiteList = boost::asio::ip::address::from_string(ip, ec);
  if (!ec)
    SIPTransportSession::rateLimit().clearAddress(whiteList, true);
  OSS_LOG_INFO("WebSocket SIP Listener " << ip << ":" << port << " (" << externalIp << ") ACTIVE");
}
#endif

void SIPTransportService::addTLSTransport(const std::string& ip, const std::string& port, const std::string& externalIp, const SIPListener::SubNets& subnets, bool isVirtualIp, const std::string& alias)
{
  OSS_LOG_INFO("Adding TLS SIP Listener " << ip << ":" << port << " (" << externalIp << ")");
  std::string key;
  OSS::string_sprintf_string<256>(key, "%s:%s", ip.c_str(), port.c_str());
  if (_tlsListeners.find(key) != _tlsListeners.end())
    throw OSS::SIP::SIPException("Duplicate TSL transport while calling addTLSTransport()");
  SIPTLSListener::Ptr pTlsListener = SIPTLSListener::Ptr(new SIPTLSListener(this, _dispatch, ip, port, _tlsConMgr));
  
  pTlsListener->setVirtual(isVirtualIp);
  pTlsListener->setExternalAddress(externalIp);
  pTlsListener->subNets() = subnets;
  _tlsListeners[key] = pTlsListener;
  
  if (!alias.empty())
  {
    pTlsListener->setTransportAlias(alias);
    _tlsListeners[alias] = pTlsListener;
  }
  
  boost::system::error_code ec;
  boost::asio::ip::address whiteList = boost::asio::ip::address::from_string(ip, ec);
  if (!ec)
    SIPTransportSession::rateLimit().clearAddress(whiteList, true);
  OSS_LOG_INFO("TLS SIP Listener " << ip << ":" << port << " (" << externalIp << ") ACTIVE");
}

SIPTransportSession::Ptr SIPTransportService::createClientTransport(
  const OSS::SIP::SIPMessage::Ptr& pMsg,
  const OSS::Net::IPAddress& localAddress,
  const OSS::Net::IPAddress& remoteAddress,
  const std::string& proto_,
  const std::string& transportId)
{
  std::string logId = pMsg->createContextId(true);
  std::string requirePersistentValue;
  pMsg->getProperty(OSS::PropertyMap::PROP_RequirePersistentConnection, requirePersistentValue);
  bool requirePersistent = pMsg->isResponse() || !requirePersistentValue.empty();
  
  if (!transportId.empty())
  {
    OSS_LOG_DEBUG( logId << "SIPTransportService::createClientTransport(" <<
      " SRC: " << localAddress.toIpPortString() <<
      " DST: " << remoteAddress.toIpPortString() <<
      " Proto: " << proto_ <<
      " TransportId: " << transportId << ")");
  }
  else
  {
    OSS_LOG_DEBUG( logId << "SIPTransportService::createClientTransport(" <<
      " SRC: " << localAddress.toIpPortString() <<
      " DST: " << remoteAddress.toIpPortString() <<
      " Proto: " << proto_ << ")");
  }


  std::string localIp = localAddress.toString();
  std::string localPort = OSS::string_from_number<unsigned short>(localAddress.getPort());
  std::string proto = proto_;
  if (transportId == "0" && proto.empty())
  {
    proto = "UDP";
  }

  if (proto == "UDP" || proto == "udp")
  {
    //
    // UDP is easy.  There's only one connection for all transations
    //
    std::string key;
    OSS::string_sprintf_string<256>(key, "%s:%s", localIp.c_str(), localPort.c_str());
    OSS_LOG_DEBUG(logId << "SIPTransportService::createClientTransport - Find UDP transport for " << key);
    if (_udpListeners.find(key) != _udpListeners.end())
    {
      OSS_LOG_DEBUG(logId << "SIPTransportService::createClientTransport - UDP transport for " << key << " FOUND");
      return _udpListeners[key]->connection();
    }
    else
    {
      OSS_LOG_DEBUG(logId << "SIPTransportService::createClientTransport - UDP transport for " << key << " NOT FOUND. Trying first IP matching " << localIp);
      UDPListeners::iterator iter;
      for (iter = _udpListeners.begin(); iter != _udpListeners.end(); iter++)
      {
        if (iter->second && iter->second->getAddress() == localIp)
        {
          OSS_LOG_DEBUG(logId << "SIPTransportService::createClientTransport - UDP transport for " << localIp << " FOUND");
          return iter->second->connection();
        }
      }
    }
  }
  else if (proto == "TCP" || proto == "tcp")
  {
    SIPTransportSession::Ptr pTCPConnection;

    //
    // Recycle old connection if it's still there
    //
    if (!transportId.empty())
    {
      OSS_LOG_INFO(logId << "SIPTransportService::createClientTransport - Finding persistent TCP connection with ID " <<  transportId);
      pTCPConnection = _tcpConMgr.findConnectionById(OSS::string_to_number<OSS::UInt64>(transportId.c_str()));
      
      if (requirePersistent && !pTCPConnection)
      {
        OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId
          << " and a persistent connection has been requested.  Giving up...");
        return pTCPConnection;
      }
    }

    if (!pTCPConnection)
    {
      if (!transportId.empty())
      {
        OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId
          << ". Trying remote-address=" << remoteAddress.toIpPortString());
      }
      
      pTCPConnection = _tcpConMgr.findConnectionByAddress(remoteAddress);
    }
    
    if (!pTCPConnection)
    {
      OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId
          << " with remote-address=" << remoteAddress.toIpPortString()
          << " creating new connection.");
      pTCPConnection = createClientTcpTransport(localAddress, remoteAddress);
    }
    return pTCPConnection;
  }
#if ENABLE_FEATURE_WEBSOCKETS
  else if (proto == "WS" || proto == "ws")
  {
    SIPTransportSession::Ptr pWSConnection;

    //
    // Recycle old connection if it's still there
    //
    if (!transportId.empty())
    {
      pWSConnection = _wsConMgr.findConnectionById(OSS::string_to_number<OSS::UInt64>(transportId.c_str()));
      if (requirePersistent && !pWSConnection)
      {
        OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId
          << " and a persistent connection has been requested.  Giving up...");
        return pWSConnection;
      }
    }

    if (!pWSConnection)
    {
      pWSConnection = _wsConMgr.findConnectionByAddress(remoteAddress);
    }

    if (!pWSConnection)
    {
      if (!transportId.empty())
      {
        OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId);
      }
    }
    return pWSConnection;
  }
#endif
  else if (proto == "TLS" || proto == "tls")
  {
    SIPTransportSession::Ptr pTLSConnection;

    //
    // Recycle old connection if it's still there
    //
    if (!transportId.empty())
    {
      OSS_LOG_INFO(logId << "SIPTransportService::createClientTransport - Finding persistent TLS connection with ID " <<  transportId);
      pTLSConnection = _tlsConMgr.findConnectionById(OSS::string_to_number<OSS::UInt64>(transportId.c_str()));
      if (requirePersistent && !pTLSConnection)
      {
        OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId
          << " and a persistent connection has been requested.  Giving up...");
        return pTLSConnection;
      }
    }

    if (!pTLSConnection)
    {
      if (!transportId.empty())
      {
        OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId
          << ". Trying remote-address=" << remoteAddress.toIpPortString());
      }
      
      pTLSConnection = _tlsConMgr.findConnectionByAddress(remoteAddress);
    }

    if (!pTLSConnection)
    {
      OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId
          << " with remote-address=" << remoteAddress.toIpPortString()
          << " creating new connection.");
      pTLSConnection = createClientTlsTransport(localAddress, remoteAddress);
    }
    return pTLSConnection;
  }
  else
  {
    OSS_LOG_DEBUG(logId << "SIPTransportService::createClientTransport - Find transport for endpoint " << proto);
    //
    // check if this is a client endpoint 
    //
    std::string endpoint = proto;
    OSS::string_to_lower(endpoint);
    Endpoints::iterator iter = _endpoints.find(endpoint);
    if (iter != _endpoints.end())
    {
      OSS_LOG_DEBUG(logId << "SIPTransportService::createClientTransport - Found endpoint connection for " << proto << "  transport-id=" << iter->second->getConnection()->getIdentifier());
      return iter->second->getConnection();
    }
    else
    {
      OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for endpoint " << proto);
    }
  }

  return SIPTransportSession::Ptr();
}

SIPTransportSession::Ptr SIPTransportService::createClientTcpTransport(
    const OSS::Net::IPAddress& localAddress,
    const OSS::Net::IPAddress& remoteAddress)
{
  SIPTransportSession::Ptr pTCPConnection(new SIPStreamedConnection(_ioService, _tcpConMgr, 0));
  pTCPConnection->isClient() = true;
  pTCPConnection->clientBind(localAddress, _tcpPortBase, _tcpPortMax);
  pTCPConnection->clientConnect(remoteAddress);
  return pTCPConnection;
}

SIPTransportSession::Ptr SIPTransportService::createClientTlsTransport(
    const OSS::Net::IPAddress& localAddress,
    const OSS::Net::IPAddress& remoteAddress)
{
  SIPTransportSession::Ptr pTlsConnection(new SIPStreamedConnection(_ioService, &_tlsClientContext, _tlsConMgr, 0));
  pTlsConnection->isClient() = true;
  pTlsConnection->clientBind(localAddress, _tcpPortBase, _tcpPortMax);
  pTlsConnection->clientConnect(remoteAddress);
  return pTlsConnection;
}

#if ENABLE_FEATURE_WEBSOCKETS
SIPTransportSession::Ptr SIPTransportService::createClientWsTransport(
    const OSS::Net::IPAddress& localAddress,
    const OSS::Net::IPAddress& remoteAddress)
{
  //
  // We dont have a concept of client connection for websockets
  //
  return SIPTransportSession::Ptr();
}
#endif

void SIPTransportService::sendUDPKeepAlive(const OSS::Net::IPAddress& localAddress,
    const OSS::Net::IPAddress& target)
{
  std::string localIp = localAddress.toString();
  std::string localPort = OSS::string_from_number<unsigned short>(localAddress.getPort());
  std::string targetIp = target.toString();
  std::string targetPort = OSS::string_from_number<unsigned short>(target.getPort());
  std::string key;
  OSS::string_sprintf_string<256>(key, "%s:%s", localIp.c_str(), localPort.c_str());
  if (_udpListeners.find(key) != _udpListeners.end())
  {
    SIPTransportSession::Ptr conn = _udpListeners[key]->connection();
    if (conn)
      conn->writeKeepAlive(targetIp, targetPort);
  }
}

std::list<std::string> SIPTransportService::resolve(
  const std::string& host,
  const std::string& service)
{
  std::list<std::string> results;
  boost::asio::ip::tcp::resolver::query query(host, service);
  boost::asio::ip::tcp::resolver::iterator endpoint_iterator = _resolver.resolve(query);
  boost::asio::ip::tcp::resolver::iterator end;
  while (endpoint_iterator != end)
  {
    boost::asio::ip::tcp::endpoint ep = *endpoint_iterator;
    boost::asio::ip::address addr = ep.address();
    results.push_back(addr.to_string());
    endpoint_iterator++;
  }
  return results;
}

SIPUDPListener::Ptr SIPTransportService::findUDPListener(const std::string& key) const
{
  UDPListeners::const_iterator iter = _udpListeners.find(key);
  if (iter != _udpListeners.end())
  {
    return iter->second;
  }
  return SIPUDPListener::Ptr();
}
  
SIPTCPListener::Ptr SIPTransportService::findTCPListener(const std::string& key) const
{
  TCPListeners::const_iterator iter = _tcpListeners.find(key);
  if (iter != _tcpListeners.end())
  {
    return iter->second;
  }
  return SIPTCPListener::Ptr();
}
  
#if ENABLE_FEATURE_WEBSOCKETS
SIPWebSocketListener::Ptr SIPTransportService::findWSListener(const std::string& key) const
{
  WSListeners::const_iterator iter = _wsListeners.find(key);
  if (iter != _wsListeners.end())
  {
    return iter->second;
  }
  return SIPWebSocketListener::Ptr();
}
#endif
  
SIPTLSListener::Ptr SIPTransportService::findTLSListener(const std::string& key) const
{
  TLSListeners::const_iterator iter = _tlsListeners.find(key);
  if (iter != _tlsListeners.end())
  {
    return iter->second;
  }
  return SIPTLSListener::Ptr();
}

bool SIPTransportService::getExternalAddress(
  const OSS::Net::IPAddress& internalIp,
  std::string& externalIp) const
{
  //
  // UDP and TCP always have the same port map so just use UDP
  //
  // TODO: Does WebSockets have the same port ???
  if (getExternalAddress("udp", internalIp, externalIp))
    return true;
  else if (getExternalAddress("tls", internalIp, externalIp))
    return true;
  return false;
}

bool SIPTransportService::getExternalAddress(
  const std::string& proto,
  const OSS::Net::IPAddress& internalIp,
  std::string& externalIp) const
{
  std::string key = internalIp.toIpPortString();
  if (proto == "udp" || proto == "UDP")
  {
    UDPListeners::const_iterator iter = _udpListeners.find(key);
    if (iter != _udpListeners.end())
    {
      externalIp = iter->second->getExternalAddress();
      return true;
    }
  }
  else if (proto == "tcp" || proto == "TCP")
  {
    TCPListeners::const_iterator iter = _tcpListeners.find(key);
    if (iter != _tcpListeners.end())
    {
      externalIp = iter->second->getExternalAddress();
      return true;
    }
  }
#if ENABLE_FEATURE_WEBSOCKETS
  else if (proto == "ws" || proto == "WS")
  {
    WSListeners::const_iterator iter = _wsListeners.find(key);
    if (iter != _wsListeners.end())
    {
      externalIp = iter->second->getExternalAddress();
      return true;
    }
  }
#endif
  else if (proto == "tls" || proto == "TLS")
  {
    TLSListeners::const_iterator iter = _tlsListeners.find(key);
    if (iter != _tlsListeners.end())
    {
      externalIp = iter->second->getExternalAddress();
      return true;
    }
  }

  return false;
}

bool SIPTransportService::getInternalAddress(
    const OSS::Net::IPAddress& externalIp,
    OSS::Net::IPAddress& internalIp) const
{
  if (getInternalAddress("udp", externalIp, internalIp))
    return true;
  if (getInternalAddress("tcp", externalIp, internalIp))
    return true;
  else if (getInternalAddress("tls", externalIp, internalIp))
    return true;
#if ENABLE_FEATURE_WEBSOCKETS
  else if (getInternalAddress("ws", externalIp, internalIp))
    return true;
#endif
  return false;
}

bool SIPTransportService::getInternalAddress(
  const std::string& proto,
  const OSS::Net::IPAddress& externalIp,
  OSS::Net::IPAddress& internalIp) const
{
  if (isLocalTransport(externalIp))
  {
    internalIp = externalIp;
    return true;
  }

  std::string ip = externalIp.toString();
  if (proto == "udp" || proto == "UDP")
  {
    for (UDPListeners::const_iterator iter = _udpListeners.begin();
      iter != _udpListeners.end(); iter++)
    {
      unsigned short port = OSS::string_to_number<unsigned short>(iter->second->getPort().c_str());
      if (iter->second->getExternalAddress() == ip && port == externalIp.getPort())
      {
        internalIp = OSS::Net::IPAddress(iter->second->getAddress(), port);
        internalIp.externalAddress() = ip;
        return true;
      }
    }
  }
  else if (proto == "tcp" || proto == "TCP")
  {
    for (TCPListeners::const_iterator iter = _tcpListeners.begin();
      iter != _tcpListeners.end(); iter++)
    {
      unsigned short port = OSS::string_to_number<unsigned short>(iter->second->getPort().c_str());
      if (iter->second->getExternalAddress() == ip && port == externalIp.getPort())
      {
        internalIp = OSS::Net::IPAddress(iter->second->getAddress(), port);
        internalIp.externalAddress() = ip;
        return true;
      }
    }
  }
#if ENABLE_FEATURE_WEBSOCKETS
  else if (proto == "ws" || proto == "WS")
  {
    for (WSListeners::const_iterator iter = _wsListeners.begin();
      iter != _wsListeners.end(); iter++)
    {
      unsigned short port = OSS::string_to_number<unsigned short>(iter->second->getPort().c_str());
      if (iter->second->getExternalAddress() == ip && port == externalIp.getPort())
      {
        internalIp = OSS::Net::IPAddress(iter->second->getAddress(), port);
        internalIp.externalAddress() = ip;
        return true;
      }
    }
  }
#endif
  else if (proto == "tls" || proto == "TLS")
  {
    for (TLSListeners::const_iterator iter = _tlsListeners.begin();
      iter != _tlsListeners.end(); iter++)
    {
      unsigned short port = OSS::string_to_number<unsigned short>(iter->second->getPort().c_str());
      if (iter->second->getExternalAddress() == ip && port == externalIp.getPort())
      {
        internalIp = OSS::Net::IPAddress(iter->second->getAddress(), port);
        internalIp.externalAddress() = ip;
        return true;
      }
    }
  }
  return false;
}


} } // OSS::SIP


