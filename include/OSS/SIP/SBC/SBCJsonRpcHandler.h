// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef OSS_SBCJSONRPCHANDLER_H_INCLUDED
#define OSS_SBCJSONRPCHANDLER_H_INCLUDED


#include "OSS/JSON/JsonRpcServer.h"
#include "OSS/JSON/JsonRpcClient.h"
#include "OSS/Net/ZMQSocketServer.h"
#include "OSS/Net/ZMQSocketClient.h"
#include "OSS/UTL/Logger.h"

#define SBC_JSON_RPC_PORT 40495
#define SBC_JSON_RPC_READ_TIMEOUT 5000

namespace OSS {
namespace SIP {
namespace SBC {

class SBCManager;

class SBCJsonRpcHandler
{
public:
  typedef OSS::JSON::JsonRpcServer<OSS::Net::ZMQSocketServer> JsonRpcServer;
  typedef OSS::JSON::JsonRpcClient<OSS::Net::ZMQSocketClient> JsonRpcClient;
  
  SBCJsonRpcHandler(SBCManager* pManager);
  ~SBCJsonRpcHandler();
  bool listen();
  
//
// Logging handlers
//
bool logDebug(const OSS::JSON::Object& params, OSS::JSON::Object& reply, int& error_code, std::string& error_message);
bool logInfo(const OSS::JSON::Object& params, OSS::JSON::Object& reply, int& error_code, std::string& error_message);
bool logWarning(const OSS::JSON::Object& params, OSS::JSON::Object& reply, int& error_code, std::string& error_message);
bool logError(const OSS::JSON::Object& params, OSS::JSON::Object& reply, int& error_code, std::string& error_message);
  
class Client
{
public:
  Client();
  ~Client();
  bool connect();
  bool logDebug(const std::string& contextId, const std::string& logMessage);
  bool logInfo(const std::string& contextId, const std::string& logMessage);
  bool logWarning(const std::string& contextId, const std::string& logMessage);
  bool logError(const std::string& contextId, const std::string& logMessage);
  bool log(const std::string& method, const std::string& contextId, const std::string& logMessage);
protected:
  JsonRpcClient _jsonRpcClient;
};

protected:
  SBCManager* _pManager;
  JsonRpcServer _jsonRpcServer;
};

inline SBCJsonRpcHandler::Client::Client()
{
}

inline SBCJsonRpcHandler::Client::~Client()
{
  _jsonRpcClient.close();
}

inline bool SBCJsonRpcHandler::Client::connect()
{
  std::ostringstream listenerAddress;
  listenerAddress << "tcp://127.0.0.1" << ":" << SBC_JSON_RPC_PORT;
  return _jsonRpcClient.connect(listenerAddress.str());
}

inline bool SBCJsonRpcHandler::Client::log(const std::string& method, const std::string& contextId, const std::string& logMessage)
{
  try
  {
    json::Object params;
    params["contextId"] = json::String(contextId);
    params["logMessage"] = json::String(logMessage);
    json::Object response;
    if (!_jsonRpcClient.call(method, params, response, SBC_JSON_RPC_READ_TIMEOUT))
    {
      return false;
    }
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR("SBCJsonRpcHandler::Client::logDebug Exception: " << e.what());
    return false;
  }
  catch(...)
  {
    OSS_LOG_ERROR("SBCJsonRpcHandler::Client::logDebug Exception: unknown"); 
    return false;
  }
  return true;
}

inline bool SBCJsonRpcHandler::Client::logDebug(const std::string& contextId, const std::string& logMessage)
{
  return log("logDebug", contextId, logMessage);
}

inline bool SBCJsonRpcHandler::Client::logInfo(const std::string& contextId, const std::string& logMessage)
{
  return log("logInfo", contextId, logMessage);
}

inline bool SBCJsonRpcHandler::Client::logWarning(const std::string& contextId, const std::string& logMessage)
{
  return log("logWarning", contextId, logMessage);
}

inline bool SBCJsonRpcHandler::Client::logError(const std::string& contextId, const std::string& logMessage)
{
  return log("logError", contextId, logMessage);
}

} } } // OSS::SIP::SBC


#endif // OSS_SBCJSONRPCHANDLER_H_INCLUDED

