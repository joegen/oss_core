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


#ifndef OSS_STUNCLIENT_H
#define	OSS_STUNCLIENT_H


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include "OSS/OSS.h"
#include "OSS/IPAddress.h"
#include "OSS/Thread.h"
#include "OSS/STUN/STUN.h"
#include "OSS/STUN/STUNProto.h"

namespace OSS {
namespace STUN {

enum NatType
{
   STUN_TYPE_UNKNOWN=0,
   STUN_TYPE_FAILURE,
   STUN_TYPE_OPEN,
   STUN_TYPE_BLOCKED,
   STUN_TYPE_CONE_NAT,
   STUN_TYPE_RESTRICTED_NAT,
   STUN_TYPE_PORT_RESTRICTED_NAT,
   STUN_TYPE_SYM_NAT,
   STUN_TYPE_FIREWALL
};

class OSS_API STUNClient :
  public boost::enable_shared_from_this<STUNClient>,
  private boost::noncopyable
{
public:
  typedef boost::shared_ptr<STUNClient> Ptr;

  STUNClient(boost::asio::io_service& ioService);
    /// Create a new STUN Client

  ~STUNClient();
    /// Destroy the NAT client

  int getNatType(
    const std::string& stunServer,
    OSS::IPAddress& localAddress);
    /// Get the NAT Type

  bool createSingleSocket(
    const std::string& stunServer,
    boost::asio::ip::udp::socket& socket,
    const OSS::IPAddress& localAddress,
    OSS::IPAddress& externalAddress);
    /// Create a single socket

  bool createRTPSocketTuple(
    const std::string& stunServer,
    boost::asio::ip::udp::socket& dataSocket,
    boost::asio::ip::udp::socket& constrolSocket,
    const OSS::IPAddress& localDataAddress,
    const OSS::IPAddress& localControlAddress,
    OSS::IPAddress& externalDataAddress,
    OSS::IPAddress& externalDataPort,
    OSS::IPAddress& externalControlAddress,
    OSS::IPAddress& externaControlPort);
    /// Create two sockets with consecutive ports for use by RTP

  static std::string getTypeString(int);
    /// Return the type in string format

  static bool getNATAddress(
    boost::asio::io_service& ioService,
    const std::string& stunServer,
    const OSS::IPAddress& localAddress,
    OSS::IPAddress& externalAddress);
    /// Return the external NAT address facing a certain interface

protected:
  void sendTestRequest(
    boost::asio::ip::udp::socket& sock,
    OSS::IPAddress& dest,
    int testNum);
    /// Send a test request to the server

  void handleReadNatType(const boost::system::error_code& e, std::size_t bytes_transferred);
    /// Handle completion of a read operation.

  void handleWrite(const boost::system::error_code& e);
    /// Handle completion of a write operation.

private:
  boost::asio::io_service& _ioService;
  boost::array<char, STUN_MAX_MESSAGE_SIZE> _buffer;
  boost::asio::ip::udp::endpoint _senderEndPoint;
  boost::asio::ip::udp::resolver _resolver;
  OSS::semaphore _semReadEvent;
  mutable OSS::mutex_critic_sec _csGlobal;
  bool _test1Responded;
  bool _test2Responded;
  bool _test3Responded;
  bool _test10Responded;
  OSS::IPAddress _test1ChangedAddr;
  OSS::IPAddress _test1MappedAddr;
  OSS::IPAddress _test10MappedAddr;
  int _sendCount;
};

} } // OSS::STUN
#endif //OSS_STUNCLIENT_H

