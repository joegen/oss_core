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


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "OSS/SIP/SIPUDPListener.h"
#include "OSS/SIP/SIPTransportService.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Net/Net.h"

namespace OSS {
namespace SIP {


SIPUDPListener::SIPUDPListener(
  SIPTransportService* pTransportService,
  const SIPTransportSession::Dispatch& dispatch,
  const std::string& address, 
  const std::string& port):
  SIPListener(pTransportService, address, port),
  _socket(0),
  _dispatch(dispatch)
{
  _pStunClient = OSS::STUN::STUNClient::Ptr(new OSS::STUN::STUNClient(pTransportService->ioService()));
}

SIPUDPListener::~SIPUDPListener()
{
  delete _socket;
}

void SIPUDPListener::run()
{
  if (!_hasStarted)
  {
    assert(!_socket);
    boost::asio::ip::address addr = boost::asio::ip::address::from_string(getAddress());
    _socket = new boost::asio::ip::udp::socket(_pTransportService->ioService(), boost::asio::ip::udp::endpoint(addr, atoi(_port.c_str())));
    socket_ip_tos_set(_socket->native(), addr.is_v4() ? AF_INET : AF_INET6, 96 /*DSCP=24(CS3) ECN=00*/);
    _pNewConnection.reset(new SIPUDPConnection(_pTransportService->ioService(), *_socket, this));
    _pNewConnection->setExternalAddress(_externalAddress);
    _pNewConnection->start(_dispatch);
    _hasStarted = true;
  }
}

void SIPUDPListener::handleStart()
{
}


void SIPUDPListener::handleStop()
{
  _pNewConnection->stop();
  _socket->close();
}

void SIPUDPListener::restart(boost::system::error_code& e)
{
  if (canBeRestarted())
  {
    boost::asio::ip::address addr = boost::asio::ip::address::from_string(getAddress());
    _socket->open(boost::asio::ip::udp::v4());
    _socket->bind(boost::asio::ip::udp::endpoint(addr, atoi(_port.c_str())), e);
    
    if (!e)
    {
      _pNewConnection->setExternalAddress(_externalAddress);
      _pNewConnection->start(_dispatch);
      OSS_LOG_NOTICE("SIPUDPListener::restart() address: " << _address << ":" << _port << " Ok");
    }
    else
    {
      OSS_LOG_ERROR("SIPUDPListener::restart() address: " << _address << ":" << _port << " Exception: " << e.message());
    }
  }
}
  
void SIPUDPListener::closeTemporarily(boost::system::error_code& e)
{
  _socket->close(e);
  OSS_LOG_NOTICE("SIPTLSListener::closeTemporarily INVOKED");
}
  
bool SIPUDPListener::canBeRestarted() const
{
  return _hasStarted && _socket && !_socket->is_open();
}

void SIPUDPListener::handleAccept(const boost::system::error_code& e, OSS_HANDLE userData)
{
  // This is only significant for stream based connections (TCP/TLS)
}

OSS::Net::IPAddress SIPUDPListener::detectNATBinding(const std::string& stunServer)
{
  OSS_VERIFY_NULL(_socket);
  OSS::Net::IPAddress external = _stunMappedAddress.getMappedAddress(_pStunClient, stunServer, *_socket);
  OSS_LOG_INFO("Started UDP Listener " 
    << _socket->local_endpoint().address().to_string()
    << ":" << _socket->local_endpoint()
    << " NAT: " << external.toIpPortString());
  run();
  return external;
}

} } // OSS::SIP

