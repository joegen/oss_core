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
#ifndef OSS_SBCJSONRPCCONNECTOR_H_INCLUDED
#define OSS_SBCJSONRPCCONNECTOR_H_INCLUDED


#include "OSS/UTL/Thread.h"
#include "OSS/JSON/JsonRpcClient.h"
#include "OSS/Net/ZMQSocketClient.h"


namespace OSS {
namespace SIP {
namespace SBC {

class SBCJsonRpcConnector
{
public:
  typedef OSS::JSON::JsonRpcClient<OSS::Net::ZMQSocketClient> JsonRpcClient;
  typedef boost::shared_ptr<JsonRpcClient> JsonRpcClientPtr;
  typedef std::map<std::string, JsonRpcClientPtr> Connectors;
  
  SBCJsonRpcConnector();
  ~SBCJsonRpcConnector();
  
  bool run();
  void stop();
  
  JsonRpcClientPtr createConnection(const std::string& connectionId, const std::string& url, int timeout);
  JsonRpcClientPtr findConnection(const std::string& connectionId);
  void destroyConnection(const std::string& connectionId);
  void setPingInterval(int pingIntervalInSeconds);
  
protected:
  void processKeepAlive();
  void processReconnect();
  bool sendPing(const JsonRpcClientPtr& pRpc, long interval, int timeout);
  void on_close(const std::string& connectionId);
  void on_fail(const std::string& connectionId);
  void internal_run();
  
  OSS::mutex_critic_sec _connectorsMutex;
  Connectors _connected;
  Connectors _unconnected;
  int _pingInterval;
  int _pingTimeout;
  int _reconnectInterval;
  int _connectTimeout;
  bool _isTerminating;
  boost::thread* _pThread;
  OSS::mutex_critic_sec _threadMutex;
};

//
// Inlines
//
inline void SBCJsonRpcConnector::setPingInterval(int pingIntervalInSeconds)
{
  _pingInterval = pingIntervalInSeconds;
}

} } } // OSS::SBC

#endif // OSS_SBCJSONRPCCONNECTOR_H_INCLUDED

