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


#include "OSS/RTP/RTPProxyManager.h"
#include "OSS/Logger.h"



namespace OSS {
namespace RTP {
  
RTPProxyManager::RTPProxyManager(int houseKeepingInterval) :
  _ioService(),
  _houseKeepingInterval(houseKeepingInterval),
  _houseKeepingTimer(_ioService, boost::posix_time::milliseconds(houseKeepingInterval)),
  _rtpProxyUDPPortBase(30000),
  _rtpProxyUDPPortMax(60000),
  _rtpProxyUDPPortCurrent(0),
  _rtpProxyThreadCount(0),
  _readTimeout(0),
  _rtpSessionMax(1000),
  _canRecycleState(true),
  _hasRtpDb(false)
{
}

RTPProxyManager::~RTPProxyManager()
{
}

void RTPProxyManager::run(int threadCount, int readTimeout)
{
  if (_rtpProxyThreadCount > 0)
    return;

  _rtpProxyThreadCount = threadCount;
  _readTimeout = readTimeout;
  //
  // start the houseKeepingTimer to keep the io_service busy
  //
  _houseKeepingTimer.async_wait(boost::bind(&RTPProxyManager::onHouseKeepingTimer, this, boost::asio::placeholders::error));
  //
  // Create a pool of threads to run all of the io_services.
  //
  for (int i = 0; i < threadCount; ++i)
  {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, &_ioService)));
    _threadPool.push_back(thread);
  }
}

bool RTPProxyManager::redisConnect(const std::vector<RedisClient::ConnectionInfo>& connections)
{
  for (std::vector<RedisClient::ConnectionInfo>::const_iterator iter = connections.begin(); iter != connections.end(); iter++)
  {
    if (!_redisClient.connect(iter->host, iter->port, iter->password, RedisClient::SBC_RTPDB))
    {
      OSS_LOG_ERROR("Unable to add rtp proxy database - " << iter->host << ":" << iter->port);
      _hasRtpDb = false;
    }
    else
    {
      OSS_LOG_INFO("Added rtp proxy database - " << iter->host << ":" << iter->port);
      _hasRtpDb = true;
    }
  }
  return _hasRtpDb;
}
    /// Connect to redis database for state persistence

void RTPProxyManager::recycleState()
{
  if (!boost::filesystem::exists(_rtpStateDirectory))
  {
    boost::filesystem::create_directory(_rtpStateDirectory);
    if (!boost::filesystem::exists(_rtpStateDirectory))
      return;
  }

  if (!_hasRtpDb)
  {
    try
    {
      boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
      for (boost::filesystem::directory_iterator itr(_rtpStateDirectory);
            itr != end_itr;
            ++itr)
      {
        if (boost::filesystem::is_directory(itr->status()))
        {
          continue;
        }
        else
        {
          boost::filesystem::path currentFile = operator/(_rtpStateDirectory, itr->path().filename().native());
          if (boost::filesystem::is_regular(currentFile))
          {
            _sessionListMutex.lock();
            RTPProxySession::Ptr session = RTPProxySession::reconstructFromStateFile(this, currentFile);
            if (session)
              _sessionList[session->getIdentifier()] = session;
            _sessionListMutex.unlock();
          }
        }
      }
    }
    catch(OSS::Exception e)
    {
      std::ostringstream logMsg;
      logMsg << "RTPProxyManager::recycleState) Failure - "
        << e.message();
      OSS::log_warning(logMsg.str());
    }
  }
  else
  {
    std::vector<std::string> keys;
    _redisClient.getKeys("*", keys);
    for (std::vector<std::string>::iterator iter = keys.begin(); iter != keys.end(); iter++)
    {
      _sessionListMutex.lock();
      RTPProxySession::Ptr session = RTPProxySession::reconstructFromRedis(this, *iter);
      if (session)
        _sessionList[session->getIdentifier()] = session;
      _sessionListMutex.unlock();
    }
  }
}

void RTPProxyManager::stop()
{
  _houseKeepingTimer.cancel();
  _ioService.stop();
  //
  // Wait for all threads in the pool to exit.
  //
  for (std::size_t i = 0; i < _threadPool.size(); ++i)
    _threadPool[i]->join();
}

void RTPProxyManager::onHouseKeepingTimer(const boost::system::error_code& e)
{
  if (e != boost::asio::error::operation_aborted)
  {
    collectInactiveSessions();
    _houseKeepingTimer.expires_from_now(boost::posix_time::milliseconds(_houseKeepingInterval));
    _houseKeepingTimer.async_wait(boost::bind(&RTPProxyManager::onHouseKeepingTimer, this, boost::asio::placeholders::error));
  }
}

void RTPProxyManager::handleSDP(
  const std::string& logId,
  const std::string& sessionId,
  const OSS::IPAddress& sentBy,
  const OSS::IPAddress& packetSourceIP,
  const OSS::IPAddress& packetLocalInterface,
  const OSS::IPAddress& route,
  const OSS::IPAddress& routeLocalInterface,
  RTPProxySession::RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{

  OSS_LOG_DEBUG( logId << "Handling SDP: " 
    << " session-id=" << sessionId
    << " sent-by=" << sentBy.toIpPortString()
    << " remote-src=" << packetSourceIP.toIpPortString()
    << " local-src=" << packetLocalInterface.toIpPortString()
    << " local-src-ext=(" << const_cast<OSS::IPAddress&>(packetLocalInterface).externalAddress() << ")"
    << " target-local-src=" << routeLocalInterface.toIpPortString()
    << " target-local-src-ext=" << const_cast<OSS::IPAddress&>(routeLocalInterface).externalAddress()
    << " target-address=" << route.toIpPortString() );

  bool verbose = rtpAttribute.verbose;
  
  RTPProxySession::Ptr proxy;
  if (requestType == RTPProxySession::INVITE || requestType == RTPProxySession::INVITE_RESPONSE)
  {
    _sessionListMutex.lock();
    RTPProxySessionList::iterator proxyIter = _sessionList.find(sessionId);
    if ( proxyIter != _sessionList.end())
    {
      proxy = proxyIter->second;
      if (!rtpAttribute.callId.empty())
      {
        proxy->callId() = rtpAttribute.callId;
        proxy->from() = rtpAttribute.from;
        proxy->to() = rtpAttribute.to;
      }
    }
    else
    {
      //
      // Create a new proxy
      //
      if (_sessionList.size() > _rtpSessionMax)
      {
        _sessionListMutex.unlock();
        OSS::log_critical("RTP Proxy Session Max Reached");
        throw RTPProxyTooManySession();
      }
      proxy = RTPProxySession::Ptr(new RTPProxySession(this, sessionId));
      if (!rtpAttribute.callId.empty())
      {
        proxy->callId() = rtpAttribute.callId;
        proxy->from() = rtpAttribute.from;
        proxy->to() = rtpAttribute.to;
      }

      if (rtpAttribute.countSessions)
      {
        proxy->setMonitoredRoute(route.toString());
        incrementSessionCount(proxy->getMonitoredRoute());
      }
      //
      // Set to the resizer value format
      //
      if (rtpAttribute.resizerSamplesLeg1 > 0 && rtpAttribute.resizerSamplesLeg2 > 0)
      {
        OSS_LOG_WARNING(logId << "RTP: Will be resizing packets to " << rtpAttribute.resizerSamplesLeg1 << "/" << rtpAttribute.resizerSamplesLeg2 << " ms samples");
        proxy->setResizerSamples(rtpAttribute.resizerSamplesLeg1, rtpAttribute.resizerSamplesLeg2);
      }
      _sessionList[sessionId] = proxy;
    }
    _sessionListMutex.unlock();
  }
  else
  {
     _sessionListMutex.lock();
    RTPProxySessionList::iterator proxyIter = _sessionList.find(sessionId);
    if ( proxyIter == _sessionList.end())
    {
      _sessionListMutex.unlock();
      throw RTPProxyException("RTP Proxy does not exist");
    }
     proxy = proxyIter->second;
    _sessionListMutex.unlock();
  }
  OSS_VERIFY(proxy);
  proxy->logId() = logId;
  proxy->verbose() = verbose;
  proxy->handleSDP(sentBy, packetSourceIP, packetLocalInterface, route,
    routeLocalInterface, requestType, sdp, rtpAttribute);
}

void RTPProxyManager::removeSession(const std::string& sessionId)
{
  _sessionListMutex.lock();
  RTPProxySessionList::iterator proxyIter = _sessionList.find(sessionId);
  if ( proxyIter != _sessionList.end())
  {
    RTPProxySession::Ptr proxy = proxyIter->second;
    if (proxy)
    {
      proxy->stop();
      if (!proxy->getMonitoredRoute().empty())
        decrementSessionCount(proxy->getMonitoredRoute());
      _sessionList.erase(proxyIter);
    }
  }
  _sessionListMutex.unlock();
}

void RTPProxyManager::changeSessionState(const std::string& sessionId, RTPProxySession::State state)
{
  _sessionListMutex.lock();
  RTPProxySessionList::iterator proxyIter = _sessionList.find(sessionId);
  if ( proxyIter != _sessionList.end())
  {
    RTPProxySession::Ptr proxy = proxyIter->second;
    if (proxy)
      proxy->setState(state);
  }
  _sessionListMutex.unlock();
}
    /// closes and deletes the rtp session

void RTPProxyManager::collectInactiveSessions()
{
  _sessionListMutex.lock();
  RTPProxySessionList::iterator iter = _sessionList.begin();
  while( iter != _sessionList.end() )
  {
    RTPProxySession::Ptr proxy = iter->second;
    if (proxy && (proxy->isVoiceInactive() || proxy->isAuthTimeout()))
    {
      if (!proxy->getMonitoredRoute().empty())
        decrementSessionCount(proxy->getMonitoredRoute());

      proxy->stop();
      iter = _sessionList.erase(iter);
    }else if (!proxy)
    {
      iter = _sessionList.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
  _sessionListMutex.unlock();
}

unsigned short RTPProxyManager::getNextAvailablePortTuple()
{
  _csPortMutex.lock();
  if (_rtpProxyUDPPortCurrent == 0)
  {
    _rtpProxyUDPPortCurrent = _rtpProxyUDPPortBase + 2;
    _csPortMutex.unlock();
    return _rtpProxyUDPPortBase;
  }
  unsigned short current = _rtpProxyUDPPortCurrent;
  _rtpProxyUDPPortCurrent += 2;
  if (_rtpProxyUDPPortCurrent > _rtpProxyUDPPortMax)
    _rtpProxyUDPPortCurrent = _rtpProxyUDPPortBase;
  _csPortMutex.unlock();
  return current;
}

void RTPProxyManager::incrementSessionCount(const std::string& address)
{
  OSS::mutex_critic_sec_lock lock(_sessionCounterMutex);
  RTPProxyCounter::iterator iter = _sessionCounter.find(address);
  if (iter != _sessionCounter.end())
    iter->second++;
  else
    _sessionCounter[address] = 1;
}

void RTPProxyManager::decrementSessionCount(const std::string& address)
{
  OSS::mutex_critic_sec_lock lock(_sessionCounterMutex);
  RTPProxyCounter::iterator iter = _sessionCounter.find(address);
  if (iter != _sessionCounter.end())
    iter->second--;
}

std::size_t RTPProxyManager::getSessionCount(const std::string& address) const
{
  OSS::mutex_critic_sec_lock lock(_sessionCounterMutex);
  RTPProxyCounter::const_iterator iter = _sessionCounter.find(address);
  if (iter == _sessionCounter.end())
    return 0;
  return iter->second;
}

} } //OSS::RTP

