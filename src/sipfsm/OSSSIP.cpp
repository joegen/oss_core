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


#include "OSS/SIP/OSSSIP.h"
#include "OSS/OSS.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/Logger.h"


using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;
using OSS::Persistent::PersistenceException;

namespace OSS {
namespace SIP {


OSSSIP::OSSSIP() :
  _fsmDispatch(),
  _enableUDP(true),
  _enableTCP(true),
  _enableTLS(false),
  _udpListeners(),
  _tcpListeners(),
  _tlsListeners(),
  _tlsCertFile(),
  _tlsDiffieHellmanParamFile(),
  _tlsPassword()
{
}

OSSSIP::~OSSSIP()
{
}

void OSSSIP::run()
{
  _fsmDispatch.transport().run();
}

void OSSSIP::transportInit()
{
  bool hasUDP = _udpListeners.size() > 0;
  bool hasTCP = _tcpListeners.size() > 0;
  bool hasTLS = _tcpListeners.size() > 0;
  
  if (!hasUDP && !hasTCP && !hasTLS)
    throw OSS::SIP::SIPException("No Listener Address Configured");

  //
  // Prepare the UDP Transport
  //
  if (_enableUDP)
  {
    for (std::size_t i = 0; i < _udpListeners.size(); i++)
    {
      OSS::IPAddress& iface = _udpListeners[i];
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
      OSS::IPAddress& iface = _tcpListeners[i];
      std::string ip = iface.address().to_string();
      std::string port = OSS::string_from_number(iface.getPort());
      _fsmDispatch.transport().addTCPTransport(ip, port, iface.externalAddress());
    }
  }

  //
  // Prepare the TLS Transport
  //
  if (_enableTLS)
  {
    for (std::size_t i = 0; i < _tlsListeners.size(); i++)
    {
      OSS::IPAddress& iface = _tlsListeners[i];
      std::string ip = iface.address().to_string();
      std::string port = OSS::string_from_number(iface.getPort());
      _fsmDispatch.transport().addTLSTransport(ip, port, iface.externalAddress(), _tlsCertFile, _tlsDiffieHellmanParamFile, _tlsPassword);
    }
  }
}

void OSSSIP::transportInit(unsigned short udpPortBase, unsigned short udpPortMax,
    unsigned short tcpPortBase, unsigned short tcpPortMax,
    unsigned short tlsPortBase, unsigned short tlsPortMax)
{
  OSS_VERIFY(udpPortBase <= udpPortMax);
  OSS_VERIFY(tcpPortBase <= tcpPortMax);
  OSS_VERIFY(tlsPortBase <= tlsPortMax);

  bool hasUDP = _udpListeners.size() > 0;
  bool hasTCP = _tcpListeners.size() > 0;
  bool hasTLS = _tcpListeners.size() > 0;

  if (!hasUDP && !hasTCP && !hasTLS)
    throw OSS::SIP::SIPException("No Listener Address Configured");

  //
  // Reset to false so we could use it as success indicator
  //
  hasUDP = false;
  hasTCP = false;
  hasTLS = false;

  //
  // Prepare the UDP Transport
  //
  if (_enableUDP)
  {
    for (std::size_t i = 0; i < _udpListeners.size(); i++)
    {
      OSS::IPAddress& iface = _udpListeners[i];
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
      OSS::IPAddress& iface = _tcpListeners[i];
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
  // Prepare the TLS Transport
  //
  if (_enableTLS)
  {
    for (std::size_t i = 0; i < _tlsListeners.size(); i++)
    {
      OSS::IPAddress& iface = _tlsListeners[i];
      std::string ip = iface.address().to_string();
      for(unsigned short p = tlsPortBase; p <= tlsPortMax; p++)
      {
        try
        {
          std::string port = OSS::string_from_number<unsigned short>(p);
          _fsmDispatch.transport().addTLSTransport(ip, port, iface.externalAddress(), _tlsCertFile, _tlsDiffieHellmanParamFile, _tlsPassword);
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

  if (!hasUDP && !hasTCP && !hasTLS)
    throw OSS::SIP::SIPException("No Listener Address Configured");
}


void OSSSIP::initTransportFromConfig(const boost::filesystem::path& cfgFile)
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
    
    bool udpEnabled = true;
    if (iface.exists("udp-enabled"))
      udpEnabled = (bool)iface["udp-enabled"];

    int sipPort = iface.exists("sip-port") ?  (int)iface["sip-port"] : 5060;
    int tlsPort = iface.exists("tls-port") ?  (int)iface["tls-port"] : 5061;

    if (!hasFoundDefault)
    {
      if (iface.exists("default"))
      {
        hasFoundDefault = ((bool)iface["default"]);
        if (hasFoundDefault  && udpEnabled)
        {
          OSS::IPAddress listener;
          listener = ip;
          listener.externalAddress() = external;
          listener.setPort(sipPort);
          _fsmDispatch.transport().defaultListenerAddress() = listener;
        }
        else if (hasFoundDefault  && tcpEnabled)
        {
          OSS::IPAddress listener;
          listener.externalAddress() = external;
          listener = ip;
          listener.setPort(sipPort);
          _fsmDispatch.transport().defaultListenerAddress() = listener;
        }
        else if (hasFoundDefault  && tlsEnabled)
        {
          OSS::IPAddress listener;
          listener.externalAddress() = external;
          listener = ip;
          listener.setPort(tlsPort);
          _fsmDispatch.transport().defaultListenerAddress() = listener;
        }
      }
    }

    if (udpEnabled)
    {
      OSS::IPAddress listener;
      listener = ip;
      listener.externalAddress() = external;
      listener.setPort(sipPort);
      _udpListeners.push_back(listener);
    }

    if (tcpEnabled)
    {
      OSS::IPAddress listener;
      listener = ip;
      listener.externalAddress() = external;
      listener.setPort(sipPort);
      _tcpListeners.push_back(listener);
    }

    if (tlsEnabled)
    {
      OSS::IPAddress listener;
      listener = ip;
      listener.externalAddress() = external;
      listener.setPort(tlsPort);
      _tlsListeners.push_back(listener);
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
      OSS::IPAddress defaultInterface((const char*)defaultIface);
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
      OSS::IPAddress listener;
      listener = ip;
      listener.setPort(port);
      _fsmDispatch.transport().defaultListenerAddress() = listener;
    }
    else if (iface.exists("tcp-enabled") && (bool)iface["tcp-enabled"])
    {
      int port = iface.exists("sip-port") ? (int)iface["sip-port"] : 5060;
      OSS::IPAddress listener;
      listener = ip;
      listener.setPort(port);
      _fsmDispatch.transport().defaultListenerAddress() = listener;
    }
    else if (iface.exists("tls-enabled") && (bool)iface["tls-enabled"])
    {
      int port = iface.exists("tls-port") ? (int)iface["tsl-port"] : 5061;
      OSS::IPAddress listener;
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

void OSSSIP::stop()
{
  _fsmDispatch.stop();
  _fsmDispatch.transport().stop();
}

void OSSSIP::sendRequest(
  const SIPMessage::Ptr& pRequest,
  const OSS::IPAddress& localAddress,
  const OSS::IPAddress& remoteAddress,
  SIPTransaction::Callback callback)
{
  _fsmDispatch.sendRequest(pRequest, localAddress, remoteAddress, callback);
}

void OSSSIP::sendRequestDirect(const SIPMessage::Ptr& pRequest,
  const OSS::IPAddress& localAddress,
  const OSS::IPAddress& remoteAddress)
{

  std::string transport;
  if (SIPVia::msgGetTopViaTransport(pRequest.get(), transport))
  {
    std::string transportId;
    pRequest->getProperty("transport-id", transportId);
    OSS_LOG_DEBUG("Sending request directly protocol=" << transport << " id=" << transportId);
    SIPTransportSession::Ptr client = _fsmDispatch.transport().createClientTransport(localAddress, remoteAddress, transport, transportId);
    if (client)
      client->writeMessage(pRequest, remoteAddress.toString(), OSS::string_from_number(remoteAddress.getPort()));
    else
      OSS_LOG_ERROR("OSSSIP::sendRequestDirect failed - Unable to create client transport");
  }
  else
  {
    OSS_LOG_ERROR("OSSSIP::sendRequestDirect failed - Unable to determine transport protocol.")
  }
}





} } // OSS::SIP

