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
  boost::asio::ip::address addr = boost::asio::ip::address::from_string(getAddress());
  _socket = new boost::asio::ip::udp::socket(pTransportService->ioService(), boost::asio::ip::udp::endpoint(addr, atoi(port.c_str())));
  _pNewConnection.reset(new SIPUDPConnection(pTransportService->ioService(), *_socket));
}

SIPUDPListener::~SIPUDPListener()
{
  delete _socket;
}

void SIPUDPListener::run()
{
  _pNewConnection->setExternalAddress(_externalAddress);
  _pNewConnection->start(_dispatch);
}

void SIPUDPListener::handleStop()
{
  _pNewConnection->stop();
  _socket->close();
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

