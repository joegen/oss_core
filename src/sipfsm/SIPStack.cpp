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


#include "OSS/SIP/SIPStack.h"
#include "OSS/OSS.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/UTL/Logger.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>


using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;
using OSS::Persistent::PersistenceException;

namespace OSS {
namespace SIP {


SIPStack::SIPStack() :
  _fsmDispatch(),
  _enableUDP(true),
  _enableTCP(true),
  _enableWS(true),
  _enableTLS(true),
  _udpListeners(),
  _tcpListeners(),
  _wsListeners(),
  _tlsListeners(),
  _tlsCertPassword(),
  _pKeyStore(0)
{
}

SIPStack::~SIPStack()
{
}

void SIPStack::run()
{
  _fsmDispatch.transport().run();
}

void SIPStack::transportInit()
{
  bool hasUDP = _udpListeners.size() > 0;
  bool hasTCP = _tcpListeners.size() > 0;
  bool hasWS  = _wsListeners.size() > 0;
  bool hasTLS = _tlsListeners.size() > 0;
  
  if (!hasUDP && !hasTCP && !hasWS && !hasTLS)
    throw OSS::SIP::SIPException("No Listener Address Configured");

  //
  // Prepare the UDP Transport
  //
  if (_enableUDP)
  {
    for (std::size_t i = 0; i < _udpListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _udpListeners[i];
      std::string ip = iface.address().to_string();
      std::string port = OSS::string_from_number<unsigned long>(iface.getPort());
      _fsmDispatch.transport().addUDPTransport(ip, port, iface.externalAddress());
    }
  }

  //
  // Prepare the TCP Transport
  //
  if (_enableTCP)
  {
    for (std::size_t i = 0; i < _tcpListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _tcpListeners[i];
      std::string ip = iface.address().to_string();
      std::string port = OSS::string_from_number(iface.getPort());
      _fsmDispatch.transport().addTCPTransport(ip, port, iface.externalAddress());
    }
  }

  //
  // Prepare the WebSocket Transport
  //
  if (_enableWS)
  {
    for (std::size_t i = 0; i < _wsListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _wsListeners[i];
      std::string ip = iface.address().to_string();
      std::string port = OSS::string_from_number(iface.getPort());
      _fsmDispatch.transport().addWSTransport(ip, port, iface.externalAddress());
    }
  }

  //
  // Prepare the TLS Transport
  //
  if (_enableTLS)
  {
    for (std::size_t i = 0; i < _tlsListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _tlsListeners[i];
      std::string ip = iface.address().to_string();
      std::string port = OSS::string_from_number(iface.getPort());
      _fsmDispatch.transport().addTLSTransport(ip, port, iface.externalAddress());
    }
  }
}

void SIPStack::transportInit(unsigned short udpPortBase, unsigned short udpPortMax,
    unsigned short tcpPortBase, unsigned short tcpPortMax,
    unsigned short wsPortBase, unsigned short wsPortMax,
    unsigned short tlsPortBase, unsigned short tlsPortMax)
{
  OSS_VERIFY(udpPortBase <= udpPortMax);
  OSS_VERIFY(tcpPortBase <= tcpPortMax);
  OSS_VERIFY(tlsPortBase <= tlsPortMax);

  bool hasUDP = _udpListeners.size() > 0;
  bool hasTCP = _tcpListeners.size() > 0;
  bool hasWS = _wsListeners.size() > 0;
  bool hasTLS = _tcpListeners.size() > 0;

  if (!hasUDP && !hasTCP && !hasWS && !hasTLS)
    throw OSS::SIP::SIPException("No Listener Address Configured");

  //
  // Reset to false so we could use it as success indicator
  //
  hasUDP = false;
  hasTCP = false;
  hasWS = false;
  hasTLS = false;

  //
  // Prepare the UDP Transport
  //
  if (_enableUDP)
  {
    for (std::size_t i = 0; i < _udpListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _udpListeners[i];
      std::string ip = iface.address().to_string();
      for(unsigned short p = udpPortBase; p <= udpPortMax; p++)
      {
        try
        {
          std::string port = OSS::string_from_number<unsigned short>(p);
          _fsmDispatch.transport().addUDPTransport(ip, port, iface.externalAddress());
          iface.setPort(p);
          hasUDP = true;
          break;
        }
        catch(...)
        {
          continue;
        }
      }
    }
  }

  //
  // Prepare the TCP Transport
  //
  if (_enableTCP)
  {
    for (std::size_t i = 0; i < _tcpListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _tcpListeners[i];
      std::string ip = iface.address().to_string();
      for(unsigned short p = tcpPortBase; p <= tcpPortMax; p++)
      {
        try
        {
          std::string port = OSS::string_from_number<unsigned short>(p);
          _fsmDispatch.transport().addTCPTransport(ip, port,iface.externalAddress());
          iface.setPort(p);
          hasTCP = true;
          break;
        }
        catch(...)
        {
          continue;
        }
      }
    }
  }

  //
  // Prepare the WebSocket Transport
  //
  if (_enableWS)
  {
    for (std::size_t i = 0; i < _wsListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _wsListeners[i];
      std::string ip = iface.address().to_string();
      for(unsigned short p = wsPortBase; p <= wsPortMax; p++)
      {
        try
        {
          std::string port = OSS::string_from_number<unsigned short>(p);
          _fsmDispatch.transport().addWSTransport(ip, port,iface.externalAddress());
          iface.setPort(p);
          hasWS = true;
          break;
        }
        catch(...)
        {
          continue;
        }
      }
    }
  }

  //
  // Prepare the TLS Transport
  //
  if (_enableTLS)
  {
    for (std::size_t i = 0; i < _tlsListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _tlsListeners[i];
      std::string ip = iface.address().to_string();
      for(unsigned short p = tlsPortBase; p <= tlsPortMax; p++)
      {
        try
        {
          std::string port = OSS::string_from_number<unsigned short>(p);
          _fsmDispatch.transport().addTLSTransport(ip, port, iface.externalAddress());
          iface.setPort(p);
          hasTLS = true;
          break;
        }
        catch(...)
        {
          continue;
        }
      }
    }
  }

  if (!hasUDP && !hasTCP && !hasWS && !hasTLS)
    throw OSS::SIP::SIPException("No Listener Address Configured");
}


void SIPStack::initTransportFromConfig(const boost::filesystem::path& cfgFile)
{
  ClassType config;
  config.load(OSS::boost_path(cfgFile));
  DataType root = config.self();
  DataType listeners = root["listeners"];
  

  DataType interfaces = listeners["interfaces"];
  int ifaceCount = interfaces.getElementCount();
  bool hasFoundDefault = false;
  for (int i = 0; i < ifaceCount; i++)
  {
    DataType iface = interfaces[i];
    std::string ip = iface["ip-address"];
    std::string external;
    if (iface.exists("external-address"))
    {
      external = (const char*)iface["external-address"];
    }

    bool tlsEnabled = iface.exists("tls-enabled") && (bool)iface["tls-enabled"];
    bool tcpEnabled = iface.exists("tcp-enabled") && (bool)iface["tcp-enabled"];
    bool wsEnabled = iface.exists("ws-enabled") && (bool)iface["ws-enabled"];
    
    bool udpEnabled = true;
    if (iface.exists("udp-enabled"))
      udpEnabled = (bool)iface["udp-enabled"];

    int sipPort = iface.exists("sip-port") ?  (int)iface["sip-port"] : 5060;
    int tlsPort = iface.exists("tls-port") ?  (int)iface["tls-port"] : 5061;
    int wsPort = iface.exists("ws-port") ?  (int)iface["ws-port"] : 5062;

    if (!hasFoundDefault)
    {
      if (iface.exists("default"))
      {
        hasFoundDefault = ((bool)iface["default"]);
        bool transportEnabled = udpEnabled || tcpEnabled || wsEnabled || tlsEnabled;

        if (hasFoundDefault  && transportEnabled)
        {
          OSS::Net::IPAddress listener;
          listener = ip;
          listener.externalAddress() = external;
          listener.setPort(sipPort);
          _fsmDispatch.transport().defaultListenerAddress() = listener;
        }
      }
    }

    if (udpEnabled)
    {
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.externalAddress() = external;
      listener.setPort(sipPort);
      _udpListeners.push_back(listener);
    }

    if (tcpEnabled)
    {
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.externalAddress() = external;
      listener.setPort(sipPort);
      _tcpListeners.push_back(listener);
    }

    if (wsEnabled)
    {
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.externalAddress() = external;
      listener.setPort(wsPort);
      _wsListeners.push_back(listener);
    }

    if (tlsEnabled)
    {
      std::string tlsCertFile;
      std::string tlsPeerCa;
      std::string tlsPeerCaDirectory;
      std::string tlsCertPassword;
      bool tlsVerifyPeer = true;
      
      if (iface.exists("tls-cert-file"))
      {
        tlsCertFile = (const char*)iface["tls-cert-file"];
      }
      
      if (iface.exists("tls-peer-ca"))
      {
        tlsPeerCa = (const char*)iface["tls-peer-ca"];
      }
      
      if (iface.exists("tls-peer-ca-dir"))
      {
        tlsPeerCaDirectory = (const char*)iface["tls-peer-ca-dir"];
      }
      
      if (iface.exists("tls-cert-password"))
      {
        tlsCertPassword = (const char*)iface["tls-cert-password"];
      }
      
      if (iface.exists("tls-verify-peer"))
      {
        tlsVerifyPeer = (bool)iface["tls-verify-peer"];
      }
      
      if (initializeTlsContext(tlsCertFile, tlsCertPassword, tlsPeerCa, tlsPeerCaDirectory, tlsVerifyPeer))
      {
        OSS::Net::IPAddress listener;
        listener = ip;
        listener.externalAddress() = external;
        listener.setPort(tlsPort);
        _tlsListeners.push_back(listener);
      }
    }
  }

  if (!hasFoundDefault)
  {
    //
    // Set the default interface for the transport service the old fashioned way
    //
    if (listeners.exists("default-interface-address") &&
      listeners.exists("default-interface-port"))
    {
      hasFoundDefault = true;
      DataType defaultIface = listeners["default-interface-address"];
      DataType defaultPort = listeners["default-interface-port"];
      OSS::Net::IPAddress defaultInterface((const char*)defaultIface);
      defaultInterface.setPort((int)defaultPort);
      _fsmDispatch.transport().defaultListenerAddress() = defaultInterface;
    }
  }

  if (!hasFoundDefault && ifaceCount > 0)
  {
    //
    // We don't have the defualt interface yet.  Lets use the first configured listener
    //
    DataType iface = interfaces[0];
    std::string ip = iface["ip-address"];
    if (iface.exists("udp-enabled") && (bool)iface["udp-enabled"])
    {
      int port = iface.exists("sip-port") ? (int)iface["sip-port"] : 5060;
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.setPort(port);
      _fsmDispatch.transport().defaultListenerAddress() = listener;
    }
    else if (iface.exists("tcp-enabled") && (bool)iface["tcp-enabled"])
    {
      int port = iface.exists("sip-port") ? (int)iface["sip-port"] : 5060;
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.setPort(port);
      _fsmDispatch.transport().defaultListenerAddress() = listener;
    }
    else if (iface.exists("ws-enabled") && (bool)iface["ws-enabled"])
    {
      int port = iface.exists("sip-port") ? (int)iface["sip-port"] : 5060;
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.setPort(port);
      _fsmDispatch.transport().defaultListenerAddress() = listener;
    }
    else if (iface.exists("tls-enabled") && (bool)iface["tls-enabled"])
    {
      int port = iface.exists("tls-port") ? (int)iface["tsl-port"] : 5061;
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.setPort(port);
      _fsmDispatch.transport().defaultListenerAddress() = listener;
    }
  }

  //
  // Set the TCP port range
  //
  if (listeners.exists("sip-tcp-port-base") && listeners.exists("sip-tcp-port-max"))
  {
    unsigned int tcpPortBase = listeners["sip-tcp-port-base"];
    unsigned int tcpPortMax = listeners["sip-tcp-port-max"];
    if (tcpPortBase < tcpPortMax && tcpPortBase > 0)
    {
      OSS_LOG_INFO("Setting TCP port range to " << tcpPortBase << "-" << tcpPortMax);
      transport().setTCPPortRange((unsigned short)tcpPortBase, (unsigned short)tcpPortMax);
    }
    else
    {
      OSS_LOG_ERROR("Unable to set TCP port base " << tcpPortBase << "-" << tcpPortMax << " Using default values.");
    }
  }

  //
  // Set the TCP port range
  //
  if (listeners.exists("sip-ws-port-base") && listeners.exists("sip-ws-port-max"))
  {
    unsigned int wsPortBase = listeners["sip-ws-port-base"];
    unsigned int wsPortMax = listeners["sip-ws-port-max"];
    if (wsPortBase < wsPortMax && wsPortBase > 0)
    {
      OSS_LOG_INFO("Setting WebSocket port range to " << wsPortBase << "-" << wsPortMax);
      transport().setWSPortRange((unsigned short)wsPortBase, (unsigned short)wsPortMax);
    }
    else
    {
      OSS_LOG_ERROR("Unable to set WebSocket port base " << wsPortBase << "-" << wsPortMax << " Using default values.");
    }
  }


  if (listeners.exists("packet-rate-ratio"))
  {
    std::string packetRateRatio = (const char*)listeners["packet-rate-ratio"];
    std::vector<std::string> tokens = OSS::string_tokenize(packetRateRatio, "/");
    if (tokens.size() == 3)
    {
      unsigned long packetsPerSecondThreshold;
      unsigned long thresholdViolationRate;
      int banLifeTime;
      thresholdViolationRate = OSS::string_to_number<unsigned long>(tokens[0].c_str());
      packetsPerSecondThreshold = OSS::string_to_number<unsigned long>(tokens[1].c_str());
      banLifeTime = OSS::string_to_number<int>(tokens[2].c_str());

      if (packetsPerSecondThreshold > thresholdViolationRate)
      {
        SIPTransportSession::rateLimit().enabled() = true;
        SIPTransportSession::rateLimit().autoBanThresholdViolators() = true;
        SIPTransportSession::rateLimit().setPacketsPerSecondThreshold(packetsPerSecondThreshold);
        SIPTransportSession::rateLimit().setThresholdViolationRate(thresholdViolationRate);
        SIPTransportSession::rateLimit().setBanLifeTime(banLifeTime);
        
        if (_pKeyStore)
        {
          OSS::Persistent::KeyValueStore* pAccessControl = _pKeyStore->getStore("/root/access-control", true);
          SIPTransportSession::rateLimit().setPersistentStore(pAccessControl);
        }
        
        OSS_LOG_INFO("Enforcing packet rate limit = " << packetRateRatio);
      }

      if (listeners.exists("packet-rate-white-list"))
      {
        DataType whiteList = listeners["packet-rate-white-list"];
        int count = whiteList.getElementCount();
        for (int i = 0; i < count; i++)
        {
          DataType wl = whiteList[i];
          std::string entry;
          if (wl.exists("source-ip"))
          {
            entry = (const char*)wl["source-ip"];
            if (!entry.empty())
            {
              boost::system::error_code ec;
              boost::asio::ip::address ip = boost::asio::ip::address::from_string(entry, ec);
              if (!ec)
                SIPTransportSession::rateLimit().whiteListAddress(ip);
            }
          }
          else if (wl.exists("source-network"))
          {
            entry = (const char*)wl["source-network"];
            if (!entry.empty())
            SIPTransportSession::rateLimit().whiteListNetwork(entry);
          }
        }
      }
    }
  }

  transportInit();
}

bool SIPStack::initializeTlsContext(
    const std::string& tlsCertFile, // Includes key and certificate to be used by this server.  File should be in PEM format
    const std::string& tlsCertFilePassword, // Set this value if tlsCertFile is password protected
    const std::string& peerCaFile, // If the remote peer this server is connecting to uses a self signed certificate, this file is used to verify authenticity of the peer identity
    const std::string& peerCaPath, // A directory full of CA certificates. The files must be named with the CA subject name hash value. (see man SSL_CTX_load_verify_locations for more info)
    bool verifyPeer // If acting as a client, verify the peer certificates.  If the peer CA file is not set, set this value to false
)
{
  boost::asio::ssl::context& tlsClientContext = transport().tlsServerContext();
  //
  // configure the client context
  //
  if (verifyPeer)
  {
    tlsClientContext.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::verify_peer | boost::asio::ssl::context::verify_fail_if_no_peer_cert);
    OSS_LOG_INFO("SIPStack::initializeTlsContext - Peer Certificate Verification Enforcing");
  }
  else
  {
    tlsClientContext.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::verify_none);
    OSS_LOG_INFO("SIPStack::initializeTlsContext - Peer Certificate Verification Disabled");
  }
  
  if (!peerCaFile.empty())
  {
    try
    {
      tlsClientContext.load_verify_file(peerCaFile);
    }
    catch(...)
    {
      OSS_LOG_ERROR("SIPStack::initializeTlsContext - Unable to load peerCaFile " << peerCaFile);
      return false;
    }
    OSS_LOG_INFO("SIPStack::initializeTlsContext - Loaded peerCaFile " << peerCaFile);
  }

  if (!peerCaPath.empty())
  {
    try
    {
      tlsClientContext.add_verify_path(peerCaPath);
    }
    catch(...)
    {
      OSS_LOG_ERROR("SIPStack::initializeTlsContext - Unable to add peerCaPath " << peerCaPath);
      return false;
    }
    OSS_LOG_INFO("SIPStack::initializeTlsContext - Loaded peerCaPath " << peerCaPath);
  }
  
  //
  // Configure the server context
  //
  boost::asio::ssl::context& tlsServerContext = transport().tlsServerContext();
  _tlsCertPassword = tlsCertFilePassword;
  
  try
  {
    tlsServerContext.set_password_callback(boost::bind(&SIPStack::getTlsCertPassword, this));
    tlsServerContext.use_certificate_chain_file(tlsCertFile);
    tlsServerContext.use_private_key_file(tlsCertFile, boost::asio::ssl::context::pem);
  }
  catch(...)
  {
    OSS_LOG_ERROR("SIPStack::initializeTlsContext - Unable to add tlsCertFile " << tlsCertFile);
    return false;
  }
  
  OSS_LOG_INFO("SIPStack::initializeTlsContext - Server certificate " << tlsCertFile << " loaded");
  
  return true;
}

void SIPStack::stop()
{
  _fsmDispatch.stop();
  _fsmDispatch.transport().stop();
}

void SIPStack::sendRequest(
  const SIPMessage::Ptr& pRequest,
  const OSS::Net::IPAddress& localAddress,
  const OSS::Net::IPAddress& remoteAddress,
  SIPTransaction::Callback callback,
  SIPTransaction::TerminateCallback terminateCallback)
{
  _fsmDispatch.sendRequest(pRequest, localAddress, remoteAddress, callback, terminateCallback);
}

void SIPStack::sendRequestDirect(const SIPMessage::Ptr& pRequest,
  const OSS::Net::IPAddress& localAddress,
  const OSS::Net::IPAddress& remoteAddress)
{

  std::string transport;
  if (SIPVia::msgGetTopViaTransport(pRequest.get(), transport))
  {
    std::string transportId;
    pRequest->getProperty(OSS::PropertyMap::PROP_TransportId, transportId);
    if (transportId.empty())
      transportId="0";
    OSS_LOG_DEBUG("Sending request directly protocol=" << transport << " id=" << transportId);
    SIPTransportSession::Ptr client = _fsmDispatch.transport().createClientTransport(pRequest, localAddress, remoteAddress, transport, transportId);
    if (client)
      client->writeMessage(pRequest, remoteAddress.toString(), OSS::string_from_number(remoteAddress.getPort()));
    else
      OSS_LOG_ERROR("SIPStack::sendRequestDirect failed - Unable to create client transport");
  }
  else
  {
    OSS_LOG_ERROR("SIPStack::sendRequestDirect failed - Unable to determine transport protocol.")
  }
}





} } // OSS::SIP

