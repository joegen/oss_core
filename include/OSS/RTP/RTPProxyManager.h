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


#ifndef RTP_RTPProxyManager_INCLUDED
#define RTP_RTPProxyManager_INCLUDED

#include "OSS/OSS.h"

#include <map>
#include <boost/unordered_map.hpp>
#include "OSS/Thread.h"
#include "OSS/RTP/RTPProxySession.h"
#include "OSS/RTP/RTPProxyRecord.h"
#include "OSS/RTP/RTPProxy.h"
#include "OSS/RedisClient.h"

#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/JSON/elements.h"

namespace xmlrpc_c 
{
  //
  // Forward declaration so we dont have to include the headers here.
  // xmlrpc-c redefines CRLF which collides with our own
  //
  class serverAbyss;
}

namespace OSS {
namespace RTP {
  

typedef boost::unordered_map<std::string, RTPProxySession::Ptr> RTPProxySessionList;
//TODO: Document the usage of RTPProxyCounter
typedef std::map<std::string, std::size_t> RTPProxyCounter;

class OSS_API RTPProxyManager : private boost::noncopyable
{
public:
  

  RTPProxyManager(int houseKeepingInterval = 5000);//TODO:magic value
    /// Creates a new RTPProxyManager

  ~RTPProxyManager();
    /// Destroys the RTPProxyManager

  void run(int threadCount, int readTimeout = 600000);//TODO:magic value
    /// Runs the io service for async operations
    ///
    /// The threadCount parameter specifies the number
    /// of threads that will participate in polling
    /// for requests.
    ///
    /// This function will return immediately
    ///

  void runRpc(unsigned short port);
    /// Starts the RPC server on this port.

  void stopRpc();
    /// Stop the RPC service

  void recycleState();
    /// This method recycle state files that weren't deleted by the previous
    /// instance.  Assuming this was dues to a restart, recycling state would
    /// allow the previously affected streams to recontinue as if nothing happened.

  void stop();
    /// Stop the event loop.  This function will block until all threads have exited.

  void onHouseKeepingTimer(const boost::system::error_code& e);
    /// Called by the internal timer to perform house-keeping task.
    ///
    /// The default interval is every 5 seconds

  int& houseKeepingInterval();
    /// The house keeping timer interval in seconds

  void handleSDP(
    const std::string& logId,
    const std::string& sessionId,
    const OSS::IPAddress& sentBy,
    const OSS::IPAddress& packetSourceIP,
    const OSS::IPAddress& packetLocalInterface,
    const OSS::IPAddress& route,
    const OSS::IPAddress& routeLocalInterface,
    RTPProxySession::RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);
    /// Process the incoming SDP

  void handleSDP(const std::string& method,
    const json::Object& args,
    json::Object& response);
    /// Callback handler for RPC based operation.  Returns the processed
    /// SDP in the response

  unsigned short getUDPPortBase() const;
    /// Return the UDP Port Base

  void setUdpPortBase(unsigned short portBase);
    /// Set the current UDP port base

  unsigned short getUDPPortMax() const;
    /// Return the UDP port max

  void setUdpPortMax(unsigned short portMax);
    /// Set the current maximum port for UDP

  unsigned short getNextAvailablePortTuple();
    /// Return the next available port tuple for data and control listeners

  void removeSession(const std::string& sessionId);
    /// closes and deletes the rtp session

  void removeSession(const std::string& method,
    const json::Object& args,
    json::Object& response);
    /// RPC call for remove session

  void changeSessionState(const std::string& sessionId, RTPProxySession::State state);
    /// manually change the state of a session

  void collectInactiveSessions();
    /// Garbage collector for inactive sessions.
    /// This is called by the housekeeping thread
    /// but maybe called explicitly by applications as
    /// well to force garbage collection immediately

  unsigned& rtpSessionMax();
     /// return the RTP session max variable

  const boost::filesystem::path& getStateDirectory() const;
    /// return the state directory where rtp session state files will
    /// be stored.

  void setStateDirectory(const boost::filesystem::path& stateDirectory);
    /// Set the state directory where state files will be stored

  OSS::mutex_critic_sec& sessionListMutex();
    /// return the session list mutex to allow upper layer to safely access
    /// session pointers

  RTPProxySessionList& sessionList();
    /// return the active session lists.  Muts be used together with sessionListMutex
    /// for thread safety

  RedisBroadcastClient& redisClient();
    /// return a reference to the redis client for the rtp proxy db

  bool redisConnect(const std::vector<RedisClient::ConnectionInfo>& connections);
    /// Connect to redis database for state persistence

  bool hasRtpDb() const;
    /// returns true if redis client is connected to the rtp proxy db

  void incrementSessionCount(const std::string& address);
    /// increase the session count by one.  create if doesnt exist.
  
  void decrementSessionCount(const std::string& address);
    /// Decrement session count.  Remove if count is zero, after decrement;
  
  std::size_t getSessionCount(const std::string& address) const;
    /// Return the number of sessions terminating to a particular address

  bool persistStateFiles() const;
    /// If this flag is true, state files for RTP will be stored in the diretory
    /// specified by _rtpStateDirectory.  The default value is false and is
    /// set together with _rtpStateDirectory.

  void disable();
    /// Disable the RTP proxy.
  
  void alwaysProxyMedia(bool alwaysProxyMedia = true);
    /// Global flag to always enable media proxying regardless of the handler option parameter
private:
  boost::asio::io_service _ioService;
  mutable OSS::mutex_critic_sec _sessionListMutex;
  mutable RTPProxySessionList _sessionList;
  int _houseKeepingInterval;
  boost::asio::deadline_timer _houseKeepingTimer;
  unsigned short _rtpProxyUDPPortBase;
  unsigned short _rtpProxyUDPPortMax;
  unsigned short _rtpProxyUDPPortCurrent;
  unsigned int _rtpProxyThreadCount;
  boost::filesystem::path _rtpStateDirectory;
  mutable OSS::mutex_critic_sec _csPortMutex;
  std::vector<boost::shared_ptr<boost::thread> > _threadPool;
  int _readTimeout;
  unsigned _rtpSessionMax;
  bool _canRecycleState;
  RedisBroadcastClient _redisClient;
  bool _hasRtpDb;
  mutable OSS::mutex_critic_sec _sessionCounterMutex;
  mutable RTPProxyCounter _sessionCounter;
  boost::thread* _pRpcServerThread;
  xmlrpc_c::serverAbyss* _pRpcServer;
  unsigned short _rpcServerPort;
  bool _persistStateFiles;
  bool _enabled;
  bool _alwaysProxyMedia;

  friend class RTPProxy;
  friend class RTPProxySession;
  friend class RTPProxyRPCServer;
};

//
// Inlines
//


inline RedisBroadcastClient& RTPProxyManager::redisClient()
{
  return _redisClient;
}

inline bool RTPProxyManager::hasRtpDb() const
{
  return _hasRtpDb;
}

inline int& RTPProxyManager::houseKeepingInterval()
{
  return _houseKeepingInterval;
}

inline unsigned short RTPProxyManager::getUDPPortBase() const
{
  return _rtpProxyUDPPortBase;
}

inline unsigned short RTPProxyManager::getUDPPortMax() const
{
  return _rtpProxyUDPPortMax;
}

inline void RTPProxyManager::setUdpPortBase(unsigned short portBase)
{
  _rtpProxyUDPPortBase = portBase;
}


inline void RTPProxyManager::setUdpPortMax(unsigned short portMax)
{
  _rtpProxyUDPPortMax = portMax;
}


inline unsigned& RTPProxyManager::rtpSessionMax()
{
  return _rtpSessionMax;
}

inline const boost::filesystem::path& RTPProxyManager::getStateDirectory() const
{
  return _rtpStateDirectory;
}

inline void RTPProxyManager::setStateDirectory(const boost::filesystem::path& stateDirectory)
{
  _rtpStateDirectory = stateDirectory;
  _persistStateFiles = true;
}

inline OSS::mutex_critic_sec& RTPProxyManager::sessionListMutex()
{
  return _sessionListMutex;
}

inline RTPProxySessionList& RTPProxyManager::sessionList()
{
  return _sessionList;
}

inline bool RTPProxyManager::persistStateFiles() const
{
  return _persistStateFiles;
}

inline void RTPProxyManager::disable()
{
  _enabled = false;
}

inline void RTPProxyManager::alwaysProxyMedia(bool alwaysProxyMedia)
{
  _alwaysProxyMedia = alwaysProxyMedia;
}

} } //OSS::RTP
#endif //RTP_RTPProxyManager_INCLUDED
