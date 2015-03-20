/*
 * Copyright (C) 2012  OSS Software Solutions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */


#ifndef RTP_RTPProxy_INCLUDED
#define RTP_RTPProxy_INCLUDED

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "OSS/UTL/Exception.h"
#include "OSS/Net/Net.h"
#include "OSS/UTL/Thread.h"
#include "OSS/SIP/SIP.h"
#include "OSS/RTP/RTPResizer.h"
#include "OSS/RTP/RTPPacket.h"

namespace OSS {
namespace RTP {


OSS_CREATE_INLINE_EXCEPTION(RTPProxyException, OSS::IOException, "RTP Proxy Exception");
OSS_CREATE_INLINE_EXCEPTION(RTPProxyTooManySession, RTPProxyException, "Too Many RTP Proxy Sessions");

class RTPProxyManager;
class RTPProxySession;

class OSS_API RTPProxy : boost::noncopyable, public boost::enable_shared_from_this<RTPProxy>
  /// RTP UDP Proxy acts as a UDP bridge between 2 remote
  /// UDP endpoints.  Common use case for this class is for
  /// implementing RTP Proxy applications.
{
public:
  typedef boost::shared_ptr<RTPProxy> Ptr;

  struct Attributes
  {
    bool verbose;
    bool forceCreate;
    bool forcePEAEncryption;
    std::string callId;
    std::string from;
    std::string to;
    int resizerSamplesLeg1;
    int resizerSamplesLeg2;
    bool countSessions;
    bool isRemoteRpc;

    Attributes()
    {
      verbose = false;
      forceCreate = false;
      forcePEAEncryption = false;
      resizerSamplesLeg1 = 0;
      resizerSamplesLeg2 = 0;
      countSessions = false;
      isRemoteRpc = false;
    }
  };

  enum Type
  {
    Data,
    Control
  };

  RTPProxy(Type type, RTPProxyManager* pManager, RTPProxySession* pSession, const std::string& identifier, bool isXORDisabled = false);
    /// Creates a new RTPProxy

  ~RTPProxy();
    /// Destroys RTPProxy

  bool open(
    const OSS::Net::IPAddress& leg1Listener,
    const OSS::Net::IPAddress& leg2Listener);
    /// Opens the UDP Proxy sockets

  void start();
    /// Start polling socket events

  void stop();
    /// Stop Polling socket events

  void shutdown();
    /// Shutdown read and write operations

  void close();

  void resetLeg1();
    /// Reset the states perhaps due to a reoffer

  void resetLeg2();
    /// Reset the states perhaps due to a reoffer

  RTPProxyManager*& manager();
    /// Returns a direct pointer to the manager

  boost::asio::ip::udp::socket*& leg1Socket();
    /// Returns a direct pointer to the Leg 1 UDP Socket

  boost::asio::ip::udp::socket*& leg2Socket();
    /// Returns a direct pointer to the Leg 2 UDP Socket

  bool adjustSenderFromPacketSource();
    /// Flag whether remote address will be adjusted
    /// after receiving the first packet

  std::string& identifier();
    /// Returns a reference to the identifier

  OSS::Net::IPAddress getLeg1Address() const;
    /// Returns the IP Address and port to be used for SDP attributes.
    /// This can be the real IP address that is bound to the local listener
    /// or an external IP address assigned by STUN or DMZ NAT

  OSS::Net::IPAddress getLeg2Address() const;
    /// Returns the IP Address and port to be used for SDP attributes.
    /// This can be the real IP address that is bound to the local listener
    /// or an external IP address assigned by STUN or DMZ NAT

  boost::asio::ip::udp::endpoint& leg1Destination();
    /// Return a reference to the leg 1 RTP destination

  boost::asio::ip::udp::endpoint& leg2Destination();
    /// Return a reference to the leg 2 RTP destination

  bool isInactive() const;
    /// Returns true if the rtp proxy is stoppped due to inactivity

  void setInactive();

  bool& isLeg1XOREncrypted();
    /// Returns a reference to the XOR flag for leg1

  bool& isLeg2XOREncrypted();
    /// Returns a reference to the XOR flag for leg2

  bool &inXORDisabled();
    /// Returns true if XOR is explicitly disabled

  void setResizerSamples(int leg1, int leg2);
    /// Enable resizing of RTP packets

  bool& isPooled();

  Type& type();

  static bool validateBuffer(boost::array<char, RTP_PACKET_BUFFER_SIZE>& buff, int size);
protected:
  void handleLeg1FrameRead(
    const boost::system::error_code& e,
    std::size_t bytes_transferred);
    /// Called by the transport proactor upon arrival of a data packet for Leg 1

  void handleLeg2FrameRead(
    const boost::system::error_code& e,
    std::size_t bytes_transferred);
    /// Called by the transport proactor upon arrival of a data packet for Leg 2

  void handleLeg1FrameWrite(const boost::system::error_code& e);
    /// Called by the transport layer after write operation on leg1 socket

  void handleLeg2FrameWrite(const boost::system::error_code& e);
    /// Called by the transport layer after write operation on leg2 socket

  void handleLeg1SocketReadTimeout(const boost::system::error_code& e);
    /// signals a timeout on leg-1 read operation

  void handleLeg2SocketReadTimeout(const boost::system::error_code& e);
    /// signals a timeout on leg-2 read operation

  void processResizerQueue();
    /// Process resizer buffers for leg1 and leg2 simultaneously

  void onResizerDequeue(RTPResizer& resizer, OSS::RTP::RTPPacket& packet);
  
  const std::string& logId() const;
private:
  std::string _identifier;
  RTPProxyManager* _pManager;
  boost::asio::ip::udp::socket* _pLeg1Socket;
  boost::asio::ip::udp::socket* _pLeg2Socket;
  boost::asio::deadline_timer _leg1ReadTimer;
  boost::asio::deadline_timer _leg2ReadTimer;
  bool _adjustSenderFromPacketSource;
  boost::asio::ip::udp::endpoint _localEndPointLeg1;
  boost::asio::ip::udp::endpoint _localEndPointLeg2;
  std::string _localEndPointLeg1External;
  std::string _localEndPointLeg2External;
  boost::asio::ip::udp::endpoint _senderEndPointLeg1;
  boost::asio::ip::udp::endpoint _senderEndPointLeg2;
  boost::asio::ip::udp::endpoint _lastSenderEndPointLeg1;
  boost::asio::ip::udp::endpoint _lastSenderEndPointLeg2;
  boost::array<char, RTP_PACKET_BUFFER_SIZE> _leg1Buffer;
  boost::array<char, RTP_PACKET_BUFFER_SIZE> _leg2Buffer;
  RTPResizer _leg1Resizer;
  RTPResizer _leg2Resizer;
  OSS::mutex_critic_sec _csLeg1Mutex;
  OSS::mutex_critic_sec _csLeg2Mutex;
  OSS::mutex_critic_sec _csSessionMutex;
  bool _leg1Reset;
  bool _leg2Reset;
  bool _isStarted;
  bool _isInactive;
  bool _isLeg1XOREncrypted;
  bool _isLeg2XOREncrypted;
  bool _isXORDisabled;
  RTPProxySession* _pSession;
  Type _type;
  bool _isPooled;
  std::string _logId;
  bool _verbose;
  friend class RTPProxySession;
  friend class RTPResizer;
};

//
// Inlines
//

inline std::string& RTPProxy::identifier()
{
  return _identifier;
}

inline RTPProxyManager*& RTPProxy::manager()
{
  return _pManager;
}

inline boost::asio::ip::udp::socket*& RTPProxy::leg1Socket()
{
  return _pLeg1Socket;
}

inline boost::asio::ip::udp::socket*& RTPProxy::leg2Socket()
{
  return _pLeg2Socket;
}

inline bool RTPProxy::adjustSenderFromPacketSource()
{
  return true;
}

inline boost::asio::ip::udp::endpoint& RTPProxy::leg1Destination()
{
  return _senderEndPointLeg1;
}

inline boost::asio::ip::udp::endpoint& RTPProxy::leg2Destination()
{
  return _senderEndPointLeg2;
}

inline bool RTPProxy::isInactive() const
{
  return _isInactive;
}

inline void RTPProxy::setInactive()
{
  _isInactive = false;
}

inline bool& RTPProxy::isLeg1XOREncrypted()
{
  return _isLeg1XOREncrypted;
}

inline bool& RTPProxy::isLeg2XOREncrypted()
{
  return _isLeg2XOREncrypted;
}

inline bool& RTPProxy::inXORDisabled()
{
  return _isXORDisabled;
}

inline void RTPProxy::handleLeg1FrameWrite(const boost::system::error_code& e)
{
}

inline void RTPProxy::handleLeg2FrameWrite(const boost::system::error_code& e)
{
}

inline bool& RTPProxy::isPooled()
{
  return _isPooled;
}

inline RTPProxy::Type& RTPProxy::type()
{
  return _type;
}

inline void RTPProxy::close()
{
  shutdown();
  stop();
}

inline const std::string& RTPProxy::logId() const
{
  return _logId;
}

} } // OSS::RTP
#endif // RTP_RTPProxy_INCLUDED

