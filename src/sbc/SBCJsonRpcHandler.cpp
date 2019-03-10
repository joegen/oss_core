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


#include "OSS/SIP/SBC/SBCJsonRpcHandler.h"
#include "OSS/SIP/SBC/SBCManager.h"


namespace OSS {
namespace SIP {
namespace SBC {

SBCJsonRpcHandler::SBCJsonRpcHandler(SBCManager* pManager) :
  _pManager(pManager)
{
  _jsonRpcServer.registerProcedure("logInfo", boost::bind(&SBCJsonRpcHandler::logInfo, this, _1, _2, _3, _4));
  _jsonRpcServer.registerProcedure("logDebug", boost::bind(&SBCJsonRpcHandler::logDebug, this, _1, _2, _3, _4));
  _jsonRpcServer.registerProcedure("logWarning", boost::bind(&SBCJsonRpcHandler::logWarning, this, _1, _2, _3, _4));
  _jsonRpcServer.registerProcedure("logError", boost::bind(&SBCJsonRpcHandler::logError, this, _1, _2, _3, _4));
}
  
SBCJsonRpcHandler::~SBCJsonRpcHandler()
{
  _jsonRpcServer.close();
}

bool SBCJsonRpcHandler::listen()
{
  std::ostringstream listenerAddress;
  listenerAddress << "tcp://127.0.0.1" << ":" << SBC_JSON_RPC_PORT;
  OSS_LOG_DEBUG("SBC JSON-RPC binding to address " << listenerAddress.str());
  return _jsonRpcServer.listen(listenerAddress.str());
}

bool SBCJsonRpcHandler::logDebug(const OSS::JSON::Object& params, OSS::JSON::Object& reply, int& error_code, std::string& error_message)
{
  OSS::JSON::String contextId = params["contextId"];
  OSS::JSON::String logMessage = params["logMessage"];
  OSS_LOG_DEBUG(contextId.Value() << "\t" << logMessage.Value());
  return true;
}

bool SBCJsonRpcHandler::logInfo(const OSS::JSON::Object& params, OSS::JSON::Object& reply, int& error_code, std::string& error_message)
{
  OSS::JSON::String contextId = params["contextId"];
  OSS::JSON::String logMessage = params["logMessage"];
  OSS_LOG_INFO(contextId.Value() << "\t" << logMessage.Value());
  return true;
}

bool SBCJsonRpcHandler::logWarning(const OSS::JSON::Object& params, OSS::JSON::Object& reply, int& error_code, std::string& error_message)
{
  OSS::JSON::String contextId = params["contextId"];
  OSS::JSON::String logMessage = params["logMessage"];
  OSS_LOG_WARNING(contextId.Value() << "\t" << logMessage.Value());
  return true;
}

bool SBCJsonRpcHandler::logError(const OSS::JSON::Object& params, OSS::JSON::Object& reply, int& error_code, std::string& error_message)
{
  OSS::JSON::String contextId = params["contextId"];
  OSS::JSON::String logMessage = params["logMessage"];
  OSS_LOG_ERROR(contextId.Value() << "\t" << logMessage.Value());
  return true;
}


} } } // OSS::SIP::SBC



