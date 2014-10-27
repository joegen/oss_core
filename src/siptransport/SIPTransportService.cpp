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

 
SIPTransportService::SIPTransportService(SIPFSMDispatch* pDispatch):
  _ioService(),
  _pIoServiceThread(0),
  _resolver(_ioService),
  _pDispatch(pDispatch),
  _tcpConMgr(_pDispatch),
  _wsConMgr(_pDispatch),
  _tlsConMgr(_pDispatch),
  _udpListeners(),
  _tcpListeners(),
  _tlsListeners(),
  _udpEnabled(true),
  _tcpEnabled(true),
  _wsEnabled(true),
  _tlsEnabled(false),
  _tcpPortBase(10000),
  _tcpPortMax(20000),
  _wsPortBase(10000),
  _wsPortMax(20000)
{
}

SIPTransportService::~SIPTransportService()
{
  if (_pIoServiceThread)
  {
    _pIoServiceThread->join();
    delete _pIoServiceThread;
    _pIoServiceThread = 0;
  }
}

void SIPTransportService::initialize(const boost::filesystem::path& cfgDirectory)
{

}

void SIPTransportService::deinitialize()
{

}

void SIPTransportService::run()
{

#if 0
  for (UDPListeners::iterator iter = _udpListeners.begin(); iter != _udpListeners.end(); iter++)
  {
    OSS::thread_pool::static_schedule(boost::bind(&SIPUDPListener::detectNATBinding, iter->second, "stun01.sipphone.com"));
  }
  OSS::thread_sleep(10);
#else
  for (UDPListeners::iterator iter = _udpListeners.begin(); iter != _udpListeners.end(); iter++)
  {
    iter->second->run();
    OSS_LOG_INFO("Started UDP Listener " << iter->first);
  }
#endif


  for (TCPListeners::iterator iter = _tcpListeners.begin(); iter != _tcpListeners.end(); iter++)
  {
    iter->second->run();
    OSS_LOG_INFO("Started TCP Listener " << iter->first);
  }

  for (WSListeners::iterator iter = _wsListeners.begin(); iter != _wsListeners.end(); iter++)
  {
    iter->second->run();
    OSS_LOG_INFO("Started WebSocket Listener " << iter->first);
  }

  for (TLSListeners::iterator iter = _tlsListeners.begin(); iter != _tlsListeners.end(); iter++)
  {
    iter->second->run();
    OSS_LOG_INFO("Started TLS Listener " << iter->first);
  }

  _pIoServiceThread = new boost::thread(boost::bind(&boost::asio::io_service::run, &_ioService));
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

  WSListeners::iterator wsIter;
  for (wsIter = _wsListeners.begin(); wsIter != _wsListeners.end(); wsIter++)
    wsIter->second->handleStop();

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
  else if (_wsListeners.find(key) != _wsListeners.end())
    return true;
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
  else if (proto == "ws")
    return _wsListeners.find(key) != _wsListeners.end();
  else if (proto == "tls")
    return _tlsListeners.find(key) != _tlsListeners.end();
  return false;
}

void SIPTransportService::addUDPTransport(std::string& ip, std::string& port, const std::string& externalIp)
{
  OSS_LOG_INFO("Adding UDP SIP Listener " << ip << ":" << port << " (" << externalIp << ")");
  std::string key;
  OSS::string_sprintf_string<256>(key, "%s:%s", ip.c_str(), port.c_str());
  if (_udpListeners.find(key) != _udpListeners.end())
    throw OSS::SIP::SIPException("Duplicate UDP Transport detected while calling addUDPTransport()");
  SIPUDPListener::Ptr udpListener(new SIPUDPListener(this, _pDispatch, ip, port));
  udpListener->setExternalAddress(externalIp);
  _udpListeners[key] = udpListener;

  boost::system::error_code ec;
  boost::asio::ip::address whiteList = boost::asio::ip::address::from_string(ip, ec);
  if (!ec)
    SIPTransportSession::rateLimit().clearAddress(whiteList, true);

  OSS_LOG_INFO("UDP SIP Listener " << ip << ":" << port << " (" << externalIp << ") ACTIVE");
}

void SIPTransportService::addTCPTransport(std::string& ip, std::string& port, const std::string& externalIp)
{
  OSS_LOG_INFO("Adding TCP SIP Listener " << ip << ":" << port << " (" << externalIp << ")");
  std::string key;
  OSS::string_sprintf_string<256>(key, "%s:%s", ip.c_str(), port.c_str());
  if (_tcpListeners.find(key) != _tcpListeners.end())
    throw OSS::SIP::SIPException("Duplicate TCP transport while calling addTCPTransport()");
  SIPTCPListener::Ptr pTcpListener = SIPTCPListener::Ptr(new SIPTCPListener(this, _pDispatch, ip, port, _tcpConMgr));
  pTcpListener->setExternalAddress(externalIp);
  _tcpListeners[key] = pTcpListener;
  boost::system::error_code ec;
  boost::asio::ip::address whiteList = boost::asio::ip::address::from_string(ip, ec);
  if (!ec)
    SIPTransportSession::rateLimit().clearAddress(whiteList, true);
  OSS_LOG_INFO("TCP SIP Listener " << ip << ":" << port << " (" << externalIp << ") ACTIVE");
}

void SIPTransportService::addWSTransport(std::string& ip, std::string& port, const std::string& externalIp)
{
  OSS_LOG_INFO("Adding WebSocket SIP Listener " << ip << ":" << port << " (" << externalIp << ")");
  std::string key;
  OSS::string_sprintf_string<256>(key, "%s:%s", ip.c_str(), port.c_str());
  if (_wsListeners.find(key) != _wsListeners.end())
    throw OSS::SIP::SIPException("Duplicate WebSocket transport while calling addWSTransport()");
  SIPWebSocketListener::Ptr pWsListener = SIPWebSocketListener::Ptr(new SIPWebSocketListener(this, ip, port, _wsConMgr));
  pWsListener->setExternalAddress(externalIp);
  _wsListeners[key] = pWsListener;
  boost::system::error_code ec;
  boost::asio::ip::address whiteList = boost::asio::ip::address::from_string(ip, ec);
  if (!ec)
    SIPTransportSession::rateLimit().clearAddress(whiteList, true);
  OSS_LOG_INFO("WebSocket SIP Listener " << ip << ":" << port << " (" << externalIp << ") ACTIVE");
}

void SIPTransportService::addTLSTransport(
  std::string& ip, 
  std::string& port,
  const std::string& externalIp,
  const std::string& tlsCertFile,
  const std::string& diffieHellmanParamFile,
  const std::string& tlsPassword)
{
  OSS_LOG_INFO("Adding TLS SIP Listener " << ip << ":" << port << " (" << externalIp << ")");
  std::string key;
  OSS::string_sprintf_string<256>(key, "%s:%s", ip.c_str(), port.c_str());
  if (_tlsListeners.find(key) != _tlsListeners.end())
    throw OSS::SIP::SIPException("Duplicate TSL transport while calling addTLSTransport()");
  SIPTLSListener::Ptr pTlsListener = SIPTLSListener::Ptr(new SIPTLSListener(this, _pDispatch, ip, port, tlsCertFile, diffieHellmanParamFile, tlsPassword));
  pTlsListener->setExternalAddress(externalIp);
  _tlsListeners[key] = pTlsListener;
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
  OSS_LOG_DEBUG( logId << "SIPTransportService::createClientTransport(" <<
    " SRC: " << localAddress.toIpPortString() <<
    " DST: " << remoteAddress.toIpPortString() <<
    " Proto: " << proto_ <<
    " TransportId: " << transportId << ")");


  std::string localIp = localAddress.toString();
  std::string localPort = OSS::string_from_number<unsigned short>(localAddress.getPort());
  std::string proto = proto_;
  if (transportId == "0")
    proto = "UDP";

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
    bool isAlive = false;
    if (!transportId.empty())
    {
      OSS_LOG_INFO(logId << "SIPTransportService::createClientTransport - Finding persistent connection with ID " <<  transportId);
      pTCPConnection = _tcpConMgr.findConnectionById(OSS::string_to_number<OSS::UInt64>(transportId.c_str()));
      if (pTCPConnection)
        isAlive = pTCPConnection->writeKeepAlive();
    }

    if (!pTCPConnection)
    {
      OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId
          << ". Trying remote-address=" << remoteAddress.toIpPortString());
      pTCPConnection = _tcpConMgr.findConnectionByAddress(remoteAddress);
      if (pTCPConnection)
        isAlive = pTCPConnection->writeKeepAlive();
    }

    if (!pTCPConnection || !isAlive)
    {
      OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId
          << " with remote-address=" << remoteAddress.toIpPortString()
          << " creating new connection.");
      pTCPConnection = createClientTcpTransport(localAddress, remoteAddress);
    }
    return pTCPConnection;
  }
  else if (proto == "WS" || proto == "ws")
  {
    SIPTransportSession::Ptr pWSConnection;

    //
    // Recycle old connection if it's still there
    //
    bool isAlive = false;
    if (!transportId.empty())
    {
      pWSConnection = _wsConMgr.findConnectionById(OSS::string_to_number<OSS::UInt64>(transportId.c_str()));
      if (pWSConnection)
        isAlive = pWSConnection->writeKeepAlive();
    }

    if (!pWSConnection)
    {
      pWSConnection = _wsConMgr.findConnectionByAddress(remoteAddress);
      if (pWSConnection)
        isAlive = pWSConnection->writeKeepAlive();
    }

    if (!pWSConnection)
    {
      OSS_LOG_WARNING(logId << "SIPTransportService::createClientTransport - Unable to find persistent connection for transport-id=" <<  transportId);
    }
    else if (!isAlive)
    {
      return SIPTransportSession::Ptr();
    }
    
    return pWSConnection;
  }
  else if (proto == "TLS" || proto == "tls")
  {
    //
    // Make sure we implement this ASAP!
    //
  }

  return SIPTransportSession::Ptr();
}

SIPTransportSession::Ptr SIPTransportService::createClientTcpTransport(
    const OSS::Net::IPAddress& localAddress,
    const OSS::Net::IPAddress& remoteAddress)
{
  SIPTransportSession::Ptr pTCPConnection(new SIPStreamedConnection(_ioService, _tcpConMgr));
  pTCPConnection->clientBind(localAddress, _tcpPortBase, _tcpPortMax);
  pTCPConnection->clientConnect(remoteAddress);
  return pTCPConnection;
}

SIPTransportSession::Ptr SIPTransportService::createClientWsTransport(
    const OSS::Net::IPAddress& localAddress,
    const OSS::Net::IPAddress& remoteAddress)
{
  //
  // We dont have a concept of client connection for websockets
  //
  return SIPTransportSession::Ptr();
}

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
  else if (proto == "ws" || proto == "WS")
  {
    WSListeners::const_iterator iter = _wsListeners.find(key);
    if (iter != _wsListeners.end())
    {
      externalIp = iter->second->getExternalAddress();
      return true;
    }
  }
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
  //
  // UDP and TCP always have the same port map so just use UDP
  //
	  // TODO: Does WebSockets have the same port ???
  if (isLocalTransport(externalIp))
  {
    internalIp = externalIp;
    if (const_cast<OSS::Net::IPAddress&>(externalIp).externalAddress().empty())
    {
      std::string external;
      getExternalAddress(externalIp, external);
      internalIp.externalAddress() = external;
      return true;
    }
  }

  if (getInternalAddress("udp", externalIp, internalIp))
    return true;
  else if (getInternalAddress("tls", externalIp, internalIp))
    return true;
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
    if (const_cast<OSS::Net::IPAddress&>(externalIp).externalAddress().empty())
    {
      std::string external;
      getExternalAddress(proto, externalIp, external);
      internalIp.externalAddress() = external;
      return true;
    }
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


