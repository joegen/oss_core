// Library: JSON-RPC Client API
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


#include "OSS/SIP/SBC/SBCJsonRpcConnector.h"
#include "OSS/JSON/JsonRpcClient.h"


namespace OSS {
namespace SIP {
namespace SBC {

  
static const int CLOCK_TICK_MS = 500;
static const int DEFAULT_PING_INTERVAL_SEC = 60; // Send ping every minute
static const int DEFAULT_RECONNECT_INTERVAL_SEC = 5; // Reconnect every 5 seconds
static const int DEFAULT_PING_TIMEOUT = 1000;
static const int DEFAULT_CONNECT_TIMEOUT = 1000;

SBCJsonRpcConnector::SBCJsonRpcConnector() :
  _pingInterval(DEFAULT_PING_INTERVAL_SEC),
  _pingTimeout(DEFAULT_PING_TIMEOUT),
  _reconnectInterval(DEFAULT_RECONNECT_INTERVAL_SEC),
  _connectTimeout(DEFAULT_CONNECT_TIMEOUT),
  _isTerminating(false),
  _pThread(0)
{
}

SBCJsonRpcConnector::~SBCJsonRpcConnector()
{
}

bool SBCJsonRpcConnector::run()
{
  OSS::mutex_critic_sec_lock lock(_threadMutex);
  if (_pThread || _isTerminating)
  {
    return false;
  }
  _isTerminating = false;
  _pThread = new boost::thread(boost::bind(&SBCJsonRpcConnector::internal_run, this));
  return true;
}

void SBCJsonRpcConnector::stop()
{
  OSS::mutex_critic_sec_lock lock(_threadMutex);
  if (!_pThread)
  {
    return;
  }
  _isTerminating = true;
  _pThread->join();
  delete _pThread;
  _pThread = 0;
}

void SBCJsonRpcConnector::internal_run()
{
  int pingExpire = (_pingInterval*1000);
  int reconnectExpire = (_reconnectInterval*1000);
  
  OSS_LOG_INFO("SBCJsonRpcConnector Thread STARTED.  ping-interval=" <<  _pingInterval << " reconnect-interval="  << _reconnectInterval);
  while(!_isTerminating)
  {
    OSS::thread_sleep(CLOCK_TICK_MS);
    pingExpire -= CLOCK_TICK_MS;
    reconnectExpire -= CLOCK_TICK_MS;
    if (!_isTerminating)
    {
      if (reconnectExpire <= 0)
      {
        processReconnect();
        reconnectExpire = (_reconnectInterval*1000);
      }
      
      if (pingExpire <= 0)
      {
        processKeepAlive();
        pingExpire = (_pingInterval*1000);
      }
    }
  }
  OSS_LOG_INFO("Media Server Thread EXITED");
}


SBCJsonRpcConnector::JsonRpcClientPtr SBCJsonRpcConnector::createConnection(const std::string& connectionId, const std::string& url, int timeout = DEFAULT_CONNECT_TIMEOUT)
{
  JsonRpcClientPtr pRpc = JsonRpcClientPtr(new JsonRpcClient());
  pRpc->identifier = connectionId;
  pRpc->closeHandler = boost::bind(&SBCJsonRpcConnector::on_close, this, _1);
  pRpc->failHandler = boost::bind(&SBCJsonRpcConnector::on_fail, this, _1);
  if (pRpc->connect(url, timeout))
  {
    OSS::mutex_critic_sec_lock lock(_connectorsMutex);
    _connected[connectionId] = pRpc;
  }
  else
  {
    OSS::mutex_critic_sec_lock lock(_connectorsMutex);
    _unconnected[connectionId] = pRpc;
  }
  return pRpc;
}

SBCJsonRpcConnector::JsonRpcClientPtr SBCJsonRpcConnector::findConnection(const std::string& connectionId)
{
  OSS::mutex_critic_sec_lock lock(_connectorsMutex);
  Connectors::iterator iter = _connected.find(connectionId);
  if (iter != _connected.end())
  {
    if (iter->second->isConnected())
    {
      return iter->second;
    }
    else
    {
      _unconnected[connectionId] = iter->second;
      _connected.erase(iter);
    }
  }
  return SBCJsonRpcConnector::JsonRpcClientPtr();
}

void SBCJsonRpcConnector::destroyConnection(const std::string& connectionId)
{
  OSS::mutex_critic_sec_lock lock(_connectorsMutex);
  _connected.erase(connectionId);
  _unconnected.erase(connectionId);
}

bool SBCJsonRpcConnector::sendPing(const JsonRpcClientPtr& pRpc, long interval, int timeout)
{
  try
  {
    json::Object params;
    json::Object response;
    params["interval"] = json::Number(interval);
    if (!pRpc->call("ping", params, response, timeout))
    {
      return false;
    }
    json::Object result = response["result"];
    json::String pong = result["value"];
    return pong.Value() == "pong";
  }
  catch(json::Exception& e)
  {
    return false;
  }
  return true;
}

void SBCJsonRpcConnector::processKeepAlive()
{
  OSS::mutex_critic_sec_lock lock(_connectorsMutex);
  std::vector<std::string> dead;
  for (Connectors::iterator iter = _connected.begin(); iter != _connected.end(); iter++)
  {
    if (!_isTerminating)
    {
      if (!sendPing(iter->second, _pingInterval * 1000, DEFAULT_PING_TIMEOUT))
      {
        dead.push_back(iter->first);
      }
    }
  }
  
  for (std::vector<std::string>::iterator iter = dead.begin(); iter != dead.end(); iter++)
  {
    if (!_isTerminating)
    {
      Connectors::iterator deadConnection = _connected.find(*iter);
      _unconnected[*iter] = deadConnection->second;
      _connected.erase(deadConnection);
    }
  }
}

void SBCJsonRpcConnector::processReconnect()
{
  OSS::mutex_critic_sec_lock lock(_connectorsMutex);
  std::vector<std::string> alive;
  for (Connectors::iterator iter = _unconnected.begin(); iter != _unconnected.end(); iter++)
  {
    if (!_isTerminating)
    {
      OSS_LOG_DEBUG("SBCJsonRpcConnector " << iter->first << " reconnecting to " << iter->second->getUrl());
      if (iter->second->connect(iter->second->getUrl(), _connectTimeout))
      {
        OSS_LOG_DEBUG("SBCJsonRpcConnector " << iter->first << " CONNECTED");
        alive.push_back(iter->first);
      }
    }
  }
  
  for (std::vector<std::string>::iterator iter = alive.begin(); iter != alive.end(); iter++)
  {
    if (!_isTerminating)
    {
      Connectors::iterator connected = _unconnected.find(*iter);
      _connected[*iter] = connected->second;
      _unconnected.erase(connected);
    }
  }
}

void SBCJsonRpcConnector::on_close(const std::string& connectionId)
{
  if (_isTerminating)
  {
    return;
  }
  OSS_LOG_WARNING("SBCJsonRpcConnector " << connectionId << " DISCONNECTED");
  OSS::mutex_critic_sec_lock lock(_connectorsMutex);
  Connectors::iterator deadConnection = _connected.find(connectionId);
  if (deadConnection != _connected.end())
  {
    _unconnected[connectionId] = deadConnection->second;
    _connected.erase(connectionId);
  }
}

void SBCJsonRpcConnector::on_fail(const std::string& connectionId)
{
  if (_isTerminating)
  {
    return;
  }
  OSS_LOG_WARNING("SBCJsonRpcConnector " << connectionId << " FAILED");
  OSS::mutex_critic_sec_lock lock(_connectorsMutex);
  Connectors::iterator deadConnection = _connected.find(connectionId);
  if (deadConnection != _connected.end())
  {
    _unconnected[connectionId] = deadConnection->second;
    _connected.erase(connectionId);
  }
}

  
} } } // OSS::SBC



