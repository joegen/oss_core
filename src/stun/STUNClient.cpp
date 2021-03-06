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
//


#include "OSS/STUN/STUNClient.h"
#include "OSS/Net/Net.h"


namespace OSS {
namespace STUN {

  
#define DEFAULT_STUN_READ_TIMEOUT  5
#define DEFAULT_STUN_SEND_INTERVAL 100

using namespace OSS::STUN::Proto;


STUNClient::STUNClient(boost::asio::io_service& ioService) :
  _ioService(ioService),
  _resolver(ioService),
  _semReadEvent(0, 0xFFFF),
  _test1Responded(false),
  _test2Responded(false),
  _test3Responded(false),
  _test10Responded(false),
  _sendCount(0),
  _readTimeoutInSeconds(DEFAULT_STUN_READ_TIMEOUT)
{

}

STUNClient::~STUNClient()
{

}

void STUNClient::sendTestRequest(
  boost::asio::ip::udp::socket& sock,
  OSS::Net::IPAddress& dest,
  int testNum)
{
  //std::cout << "Sending TEST " << testNum << " " << sock.local_endpoint().address().to_string()
  //  << "->" << dest.toIpPortString() << std::endl;
  StunAtrString username;
  StunAtrString password;
  username.sizeValue = 0;
  password.sizeValue = 0;

  bool changePort=false;
  bool changeIP=false;

  switch (testNum)
  {
    case 1:
    case 10:
    case 11:
       break;
    case 2:
       //changePort=true;
       changeIP=true;
       break;
    case 3:
       changePort=true;
       break;
    case 4:
       changeIP=true;
       break;
    case 5:
       break;
    default:
       OSS_ASSERT(false);
  }

  StunMessage req;
  memset(&req, 0, sizeof(StunMessage));

  stunBuildReqSimple(&req, username, changePort , changeIP , testNum);

  char buf[STUN_MAX_MESSAGE_SIZE];
  int len = STUN_MAX_MESSAGE_SIZE;

  len = stunEncodeMessage(req, buf, len, password, false);

  sock.async_send_to(boost::asio::buffer(buf, len),
    boost::asio::ip::udp::endpoint(dest.address(), dest.getPort()),
    boost::bind(&STUNClient::handleWrite, shared_from_this(),
                      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

  sock.async_receive_from(boost::asio::buffer(_buffer), _senderEndPoint,
      boost::bind(&STUNClient::handleReadNatType, shared_from_this(),
        boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));

  _semReadEvent.tryWait(10);
}

int STUNClient::getNatType(
  const std::string& stunServer,
  const OSS::Net::IPAddress& localAddress,
  unsigned maxDetectionTimeInSeconds)
{
  OSS::mutex_critic_sec_lock globalLock(_csGlobal);

  _test1Responded = false;
  _test2Responded = false;
  _test3Responded = false;
  _test10Responded = false;
  _test1ChangedAddr = OSS::Net::IPAddress();
  _test1MappedAddr = OSS::Net::IPAddress();
  _test10MappedAddr = OSS::Net::IPAddress();
  _sendCount = 0;

  int port = localAddress.getPort();
  if (!port)
    port = stunRandomPort();
  OSS::Net::IPAddress targetAddress = OSS::Net::IPAddress::fromV4IPPort(stunServer.c_str());
  if (!targetAddress.isValid() || !localAddress.isValid())
    return StunTypeFailure;

  if (!targetAddress.getPort())
    targetAddress.setPort(STUN_PORT);


  boost::asio::ip::udp::socket sock(_ioService);
  sock.open(boost::asio::ip::udp::v4());
  boost::asio::ip::udp::endpoint ep;
  bool isBound = false;
  for (int i = 0; i < 10; i++)
  {
    ep = boost::asio::ip::udp::endpoint(localAddress.address(), port);
    boost::system::error_code ec;
    sock.bind(ep, ec);
    if (!ec)
    {
        isBound = true;
        break;
    }
    port++;
  }

  if (!isBound)
    return StunTypeFailure;

  int maxSendCount = (maxDetectionTimeInSeconds * 1000) / 100;
  while (_sendCount < maxSendCount)
  {
    _sendCount++;
    if (!_test1Responded)
      sendTestRequest(sock,  targetAddress, 1);
    if (!_test2Responded)
      sendTestRequest(sock,  targetAddress, 2);
    if (!_test3Responded)
      sendTestRequest(sock,  targetAddress, 3);
    if (!_test10Responded && _test1Responded)
    {
      if (_test1ChangedAddr.isValid())
        sendTestRequest(sock,  _test1ChangedAddr, 10);
    }
    OSS::thread_sleep(100);
  }
  if (!_test1Responded)
    return StunTypeBlocked;


  boost::asio::ip::udp::socket test_sock(_ioService);
  test_sock.open(boost::asio::ip::udp::v4());
  boost::asio::ip::udp::endpoint test_ep = boost::asio::ip::udp::endpoint(_test1MappedAddr.address(), _test1MappedAddr.getPort());
  boost::system::error_code ec;
  test_sock.bind(test_ep, ec);
  bool isNat = false;
  
  if (ec) {
      isNat = true;
  }

  if ( isNat )
  {
    if (_test1MappedAddr == _test10MappedAddr)
    {
      if (_test2Responded)
      {
        return StunTypeIndependentFilter;
      }
      else
      {
        if (_test3Responded)
        {
          return StunTypeDependentFilter;
        }
        else
        {
          return StunTypePortDependedFilter;
        }
      }
    }
    else // mappedIp is not same
    {
      return StunTypeDependentMapping;
    }
  }
  else  // isNat is false
  {
    if (_test2Responded)
    {
      return StunTypeOpen;
    }
    else
    {
      return StunTypeFirewall;
    }
  }
  return StunTypeUnknown;
}


bool STUNClient::createSingleSocket(
  const std::string& stunServer,
  boost::asio::ip::udp::socket& socket,
  const OSS::Net::IPAddress& lAddr,
  OSS::Net::IPAddress& externalAddress)
{
  OSS::mutex_critic_sec_lock globalLock(_csGlobal);

  if (!_readTimeoutInSeconds)
    _readTimeoutInSeconds = DEFAULT_STUN_READ_TIMEOUT;
  
  int maxSendCount = (_readTimeoutInSeconds * 1000) / DEFAULT_STUN_SEND_INTERVAL;
  
  _test1Responded = false;
  _test2Responded = false;
  _test3Responded = false;
  _test10Responded = false;
  _test1ChangedAddr = OSS::Net::IPAddress();
  _test1MappedAddr = OSS::Net::IPAddress();
  _test10MappedAddr = OSS::Net::IPAddress();
  _sendCount = 0;

  OSS::Net::IPAddress localAddress = lAddr;
  OSS::Net::IPAddress targetAddress = OSS::Net::IPAddress::fromV4IPPort(stunServer.c_str());
  if (!targetAddress.isValid() || !localAddress.isValid())
    return false;

  if (!targetAddress.getPort())
    targetAddress.setPort(STUN_PORT);

  if (!socket.is_open())
  {
    socket.open(boost::asio::ip::udp::v4());
    boost::asio::ip::udp::endpoint ep;

    ep = boost::asio::ip::udp::endpoint(localAddress.address(), localAddress.getPort());
    boost::system::error_code ec;
    socket.bind(ep, ec);
    if (ec)
      return false;
  }

  int currentSleepCount = 0;
  while (_sendCount < maxSendCount)
  {
    
    if (!_test1Responded)
    { 
      if (currentSleepCount == 0)
      {
        _sendCount++;
        sendTestRequest(socket,  targetAddress, 1);
      }
      _semReadEvent.wait(1);
      if (++currentSleepCount == DEFAULT_STUN_SEND_INTERVAL)
      {
        currentSleepCount = 0;
      }
    }
    else
    {
      break;
    }
  }

  if (_test1Responded)
  {
    externalAddress = _test1MappedAddr;
    return externalAddress.isValid();
  }

  return false;
}

void STUNClient::handleReadNatType(const boost::system::error_code& e, std::size_t bytes_transferred)
{
  if (e)
    return;

  StunMessage resp;
  memset(&resp, 0, sizeof(StunMessage));
  if (!stunParseMessage(_buffer.data(), bytes_transferred, resp, false))
    return;
  switch( resp.msgHdr.id.octet[0] )
  {
  case 1:
    if (!_test1Responded)
    {
      _test1ChangedAddr =  OSS::Net::IPAddress(resp.changedAddress.ipv4.addr);
      _test1ChangedAddr.setPort(resp.changedAddress.ipv4.port);
      _test1MappedAddr =  OSS::Net::IPAddress(resp.mappedAddress.ipv4.addr);
      _test1MappedAddr.setPort(resp.mappedAddress.ipv4.port);
    }
    _sendCount = 0;
    _test1Responded = true;
    break;
  case 2:
    _test2Responded=true;
    break;
  case 3:
    _test3Responded=true;
    break;
  case 10:
    if (!_test10Responded)
    {
       _test10MappedAddr = resp.mappedAddress.ipv4.addr;
       _test10MappedAddr.setPort(resp.mappedAddress.ipv4.port);
    }
    _test10Responded=true;
    break;
  }
  _semReadEvent.set();
}

void STUNClient::handleWrite(const boost::system::error_code& e, std::size_t bytes_transferred)
{

}

std::string STUNClient::getTypeString(int type)
{
  switch (type)
  {
   case STUN_TYPE_UNKNOWN:
     return "Unknown NAT Type";
   case STUN_TYPE_FAILURE:
     return "NAT Detection Failure";
   case STUN_TYPE_OPEN:
     return "OPEN";
   case STUN_TYPE_BLOCKED:
     return "BLOCKED";
  case STUN_TYPE_CONE_NAT:
     return "Full Cone NAT";
   case STUN_TYPE_RESTRICTED_NAT:
     return "Restricted NAT";
   case STUN_TYPE_PORT_RESTRICTED_NAT:
     return "Port Restricted NAT";
   case STUN_TYPE_SYM_NAT:
     return "Symmetric NAT";
   case STUN_TYPE_FIREWALL:
     return "Firewall";
  }
  return "Unknown NAT Type";
}

bool STUNClient::getNATAddress(
  const std::string& stunServer,
  const OSS::Net::IPAddress& localAddress,
  OSS::Net::IPAddress& externalAddress)
{
  boost::shared_ptr<STUNClient> stun(new STUNClient(OSS::net_io_service()));
  boost::asio::ip::udp::socket socket(OSS::net_io_service());
  return stun->createSingleSocket(stunServer, socket, localAddress, externalAddress);
}

int STUNClient::detectNATType(
    const std::string& stunServer,
    const OSS::Net::IPAddress& localAddress,
    unsigned maxDetectionTimeInSeconds)
{
  boost::shared_ptr<STUNClient> stun(new STUNClient(OSS::net_io_service()));
  return stun->getNatType(stunServer, localAddress, maxDetectionTimeInSeconds);
}


} } // OSS::STUN
