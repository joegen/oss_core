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

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

#include "OSS/RTP/RTPProxyManager.h"
#include "OSS/Logger.h"



namespace OSS {
namespace RTP {

class HandleSDP : public xmlrpc_c::method
{
public:
  RTPProxyManager& _rpc;
  HandleSDP(RTPProxyManager& rpc) :
    _rpc(rpc)
  {
  }

  void execute(xmlrpc_c::paramList const& paramList,
          xmlrpc_c::value *   const  retvalP)
  {
    json::Object params;
    json::Object response;
    try
    {
      std::stringstream strm;
      strm << paramList.getString(0);
      paramList.verifyEnd(1);
      json::Reader::Read(params, strm);
      _rpc.handleSDP(method(), params, response);
      std::ostringstream responseStrm;
      json::Writer::Write(response, responseStrm);
      *retvalP = xmlrpc_c::value_string(responseStrm.str());
    }
    catch(json::Exception& e)
    {
      OSS_LOG_ERROR("[RPC] HanleSDP::execute Exception: " << e.what());
    }
    catch(std::exception& e)
    {
      OSS_LOG_ERROR("[RPC] HanleSDP::execute Exception: " << e.what());
    }
    catch(...)
    {
      OSS_LOG_ERROR("[RPC] HanleSDP::execute: Unknown exception");
    }
  }

  static std::string method()
  {
    return "rtp.handleSDP";
  }
};

class RemoveSession : public xmlrpc_c::method
{
public:
  RTPProxyManager& _rpc;
  RemoveSession(RTPProxyManager& rpc) :
    _rpc(rpc)
  {
  }

  void execute(xmlrpc_c::paramList const& paramList,
          xmlrpc_c::value *   const  retvalP)
  {
    json::Object params;
    json::Object response;
    try
    {
      std::stringstream strm;
      strm << paramList.getString(0);
      paramList.verifyEnd(1);
      json::Reader::Read(params, strm);
      _rpc.removeSession(method(), params, response);
      std::ostringstream responseStrm;
      json::Writer::Write(response, responseStrm);
      *retvalP = xmlrpc_c::value_string(responseStrm.str());
    }
    catch(json::Exception& e)
    {
      OSS_LOG_ERROR("[RPC] HanleSDP::execute Exception: " << e.what());
    }
    catch(std::exception& e)
    {
      OSS_LOG_ERROR("[RPC] HanleSDP::execute Exception: " << e.what());
    }
    catch(...)
    {
      OSS_LOG_ERROR("[RPC] HanleSDP::execute: Unknown exception.");
    }
  }

  static std::string method()
  {
    return "rtp.removeSession";
  }
};


RTPProxyManager::RTPProxyManager(int houseKeepingInterval) :
  _ioService(),
  _houseKeepingInterval(houseKeepingInterval),
  _houseKeepingTimer(_ioService, boost::posix_time::milliseconds(houseKeepingInterval)),
  _rtpProxyUDPPortBase(30000), //TODO: magic value
  _rtpProxyUDPPortMax(60000), //TODO: magic value
  _rtpProxyUDPPortCurrent(0),
  _rtpProxyThreadCount(0),
  _readTimeout(0),
  _rtpSessionMax(1000), //TODO: magic value
  _canRecycleState(true),
  _hasRtpDb(false),
  _pRpcServerThread(0),
  _pRpcServer(0),
  _rpcServerPort(0),
  _persistStateFiles(false),
  _enabled(true),
  _alwaysProxyMedia(false)
{
}

RTPProxyManager::~RTPProxyManager()
{
  stopRpc();
  stop();
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
	//TODO: What if _redisClient is already connected?

  for (std::vector<RedisClient::ConnectionInfo>::const_iterator iter = connections.begin(); iter != connections.end(); iter++)
  {
	  //TODO: This needs to be disconnected on destructor or stop
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
  if (!_persistStateFiles)
    return;

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
          boost::filesystem::path currentFile = operator/(_rtpStateDirectory, OSS::boost_file_name(itr->path()));
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
  {
    _threadPool[i]->join();
  }
  _threadPool.clear();
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

  if (!_enabled)
    return;

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
  
  if (_alwaysProxyMedia)
    rtpAttribute.forceCreate = true;
  
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

      //TODO: Document why aren't sessions always counted
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

void RTPProxyManager::handleSDP(const std::string& /*method*/,
    const json::Object& args,
    json::Object& response)
{
  std::string lid;
  try
  {
	  //TODO: Refactor json encoding/decoding of args so that all json
	  //operations for handleSDP are done in the same place to minimize the risk
	  // of mistyping something.
    json::String logId = args["logId"];
    lid = logId.Value();
    json::String sessionId = args["sessionId"];
    json::String sentBy = args["sentBy"];
    json::String packetSourceIP = args["packetSourceIP"];
    json::String packetLocalInterface = args["packetLocalInterface"];
    json::String route = args["route"];
    json::String routeLocalInterface = args["routeLocalInterface"];
    json::Number requestType = args["requestType"];
    json::String sdp = args["sdp"];
    json::Boolean attr_verbose = args["attr.verbose"];
    json::Boolean attr_forceCreate = args["attr.forceCreate"];
    json::Boolean attr_forcePEAEncryption = args["attr.forcePEAEncryption"];
    json::String attr_callId = args["attr.callId"];
    json::String attr_from = args["attr.from"];
    json::String attr_to = args["attr.to"];
    json::Number attr_resizerSamplesLeg1 = args["attr.resizerSamplesLeg1"];
    json::Number attr_resizerSamplesLeg2 = args["attr.resizerSamplesLeg2"];

    std::string logId_(logId.Value());
    std::string sessionId_(sessionId.Value());
    OSS::IPAddress sentBy_ = OSS::IPAddress::fromV4IPPort(sentBy.Value().c_str());
    OSS::IPAddress packetSourceIP_ = OSS::IPAddress::fromV4IPPort(packetSourceIP.Value().c_str());
    OSS::IPAddress packetLocalInterface_ = OSS::IPAddress::fromV4IPPort(packetLocalInterface.Value().c_str());
    OSS::IPAddress route_ = OSS::IPAddress::fromV4IPPort(route.Value().c_str());
    OSS::IPAddress routeLocalInterface_ = OSS::IPAddress::fromV4IPPort(routeLocalInterface.Value().c_str());
    RTPProxySession::RequestType requestType_((RTPProxySession::RequestType)requestType.Value());
    std::string sdp_(sdp.Value());

    RTPProxy::Attributes attr;
    attr.callId = attr_callId.Value();
    attr.forceCreate = attr_forceCreate.Value();
    attr.forcePEAEncryption = attr_forcePEAEncryption.Value();
    attr.from = attr_from.Value();
    attr.resizerSamplesLeg1 = attr_resizerSamplesLeg1.Value();
    attr.resizerSamplesLeg2 = attr_resizerSamplesLeg2.Value();
    attr.to = attr_to.Value();
    attr.verbose = attr_verbose.Value();
    attr.isRemoteRpc = true;
    
    handleSDP(logId_, sessionId_, sentBy_, packetSourceIP_, packetLocalInterface_, route_, routeLocalInterface_, requestType_, sdp_, attr);

    response["sdp"] = json::String(sdp_);
  }
  catch(json::Exception& e)
  {
    OSS_LOG_ERROR(lid << " RTP RTPProxy::handleSDP Exception: " << e.what());
  }
  catch(std::exception& e)
  {
    OSS_LOG_ERROR(lid << " RTP RTPProxy::handleSDP Exception: " << e.what());
  }
  catch(...)
  {
    OSS_LOG_ERROR(lid << " RTP RTPProxy::handleSDP Unknown Exception");
  }
}

void RTPProxyManager::removeSession(const std::string& method,
  const json::Object& args,
  json::Object& response)
{
  try
  {
    json::String sessionId = args["sessionId"];
    removeSession(sessionId.Value());
    response["errorString"] = json::String("ok");
  }
  catch(json::Exception& e)
  {
    OSS_LOG_ERROR(" RTP RTPProxy::handleSDP Exception: " << e.what());
  }
  catch(std::exception& e)
  {
    OSS_LOG_ERROR(" RTP RTPProxy::handleSDP Exception: " << e.what());
  }
  catch(...)
  {
    OSS_LOG_ERROR(" RTP RTPProxy::handleSDP Unknown Exception");
  }
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
    //TODO: Document criteria to consider a session inactive
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
    _rtpProxyUDPPortCurrent = _rtpProxyUDPPortBase + 2; //TODO: magic value
    _csPortMutex.unlock();
    return _rtpProxyUDPPortBase;
  }
  unsigned short current = _rtpProxyUDPPortCurrent;
  _rtpProxyUDPPortCurrent += 2; //TODO: magic value
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

void RTPProxyManager::runRpc(unsigned short port)
{
  if (!_pRpcServer)
  {
    _pRpcServerThread = new boost::thread(boost::bind(&RTPProxyManager::runRpc, this, port));
    return;
  }

  _rpcServerPort = port;

  try
  {
    xmlrpc_c::registry registry;

    xmlrpc_c::methodPtr const handleSdp(new HandleSDP(*this));
    xmlrpc_c::methodPtr const removeSession(new RemoveSession(*this));
    
    registry.addMethod(HandleSDP::method().c_str(), handleSdp);
    registry.addMethod(RemoveSession::method().c_str(), removeSession);

    _pRpcServer = new xmlrpc_c::serverAbyss(
      registry,
      _rpcServerPort
    );
    _pRpcServer->run();
  } catch (std::exception const e)
  {
    OSS_LOG_ERROR("[RPC] KarooRtpProxyRPCServer::internal_run Exception: " << e.what());
  }
}

void RTPProxyManager::stopRpc()
{
  if (_pRpcServer)
    _pRpcServer->terminate();

  if (_pRpcServerThread)
  {
    _pRpcServerThread->join();
    delete _pRpcServerThread;
    _pRpcServerThread = 0;
  }

  if (_pRpcServer)
  {
    delete _pRpcServer;
    _pRpcServer = 0;
  }
}

} } //OSS::RTP

