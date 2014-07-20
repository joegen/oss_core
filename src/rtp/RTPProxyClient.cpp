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


#include "OSS/RTP/RTPProxyClient.h"
#include "OSS/UTL/Logger.h"

namespace OSS {
namespace RTP {


RTPProxyClient::RTPProxyClient(const OSS::Net::IPAddress& rpcHost, int poolSize) :
  _poolSize(poolSize),
  _rpcHost(rpcHost)
{
  for (int i = 0; i < _poolSize; i++)
    _connections.enqueue(new Connection());
}

RTPProxyClient::~RTPProxyClient()
{
  for (int i = 0; i < _poolSize; i++)
  {
    Connection* pConnection;
    _connections.dequeue(pConnection);
    delete pConnection;
  }
}

void RTPProxyClient::handleSDP(
  const std::string& logId,
  const std::string& sessionId,
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RTPProxySession::RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  Connection* pConnection;
  _connections.dequeue(pConnection);

  if (pConnection)
  {
    try
    {
      //
      //  Call the RPC
      //
    	//TODO: Refactor this piece to have translation from params to json and viceversa
    	// in one place.
      json::Object params;
      params["logId"] = json::String(logId);
      params["sessionId"] = json::String(sessionId);
      params["sentBy"] = json::String(sentBy.toIpPortString());
      params["packetSourceIP"] = json::String(packetSourceIP.toIpPortString());
      params["packetLocalInterface"] = json::String(packetLocalInterface.toIpPortString());
      params["route"] = json::String(route.toIpPortString());
      params["routeLocalInterface"] = json::String(routeLocalInterface.toIpPortString());
      params["requestType"] = json::Number(requestType);
      params["sdp"] = json::String(sdp);

      params["attr.verbose"] = json::Boolean(rtpAttribute.verbose);
      params["attr.forceCreate"] = json::Boolean(rtpAttribute.forceCreate);
      params["attr.forcePEAEncryption"] = json::Boolean(rtpAttribute.forcePEAEncryption);
      params["attr.callId"] = json::String(rtpAttribute.callId);
      params["attr.from"] = json::String(rtpAttribute.from);
      params["attr.to"] = json::String(rtpAttribute.to);
      params["attr.resizerSamplesLeg1"] = json::Number(rtpAttribute.resizerSamplesLeg1);
      params["attr.resizerSamplesLeg2"] = json::Number(rtpAttribute.resizerSamplesLeg2);


      std::ostringstream jsonParams;
      json::Writer::Write(params, jsonParams);

      std::string methodName("rtp.handleSDP");
      xmlrpc_c::paramList rpcParams;
      rpcParams.add(xmlrpc_c::value_string(jsonParams.str()));
      xmlrpc_c::rpcPtr proc(methodName, rpcParams);
      std::ostringstream rpcUrl;
      rpcUrl << "http://" << _rpcHost.toIpPortString() << "/RPC2"; //TODO: magic value
      xmlrpc_c::carriageParm_curl0 requestUri(rpcUrl.str());

      OSS_LOG_DEBUG("[RPC] RTPProxyClient::handleSDP POST: " << rpcUrl.str());

      proc->call(&(pConnection->client), &requestUri);



      std::stringstream jsonResponse;
      json::Object response;
      std::string responseString(xmlrpc_c::value_string(proc->getResult()));
      jsonResponse << responseString;
      json::Reader::Read(response, jsonResponse);
      json::String responseSdp = response["sdp"];
      if (!responseSdp.Value().empty())
        sdp = responseSdp.Value();
    }
    catch(std::exception& e)
    {
      OSS_LOG_ERROR("[RPC] RTPProxyClient::handleSDP Exception: " << e.what());
    }
    catch(...)
    {
      OSS_LOG_ERROR("[RPC] RTPProxyClient::handleSDP: Unknown exception.");
    }
    _connections.enqueue(pConnection);
  }
}

void RTPProxyClient::removeSession(const std::string& sessionId)
{
  Connection* pConnection;
  _connections.dequeue(pConnection);

  if (pConnection)
  {
    try
    {
      //
      //  Call the RPC
      //
      json::Object params;
      params["sessionId"] = json::String(sessionId);
      std::ostringstream jsonParams;
      json::Writer::Write(params, jsonParams);

      std::string methodName("rtp.removeSession");
      xmlrpc_c::paramList rpcParams;
      rpcParams.add(xmlrpc_c::value_string(jsonParams.str()));
      xmlrpc_c::rpcPtr proc(methodName, rpcParams);
      std::ostringstream rpcUrl;
      rpcUrl << "http://" << _rpcHost.toIpPortString() << "/RPC2"; //TODO: magic value
      xmlrpc_c::carriageParm_curl0 requestUri(rpcUrl.str());

      OSS_LOG_DEBUG("[RPC] RTPProxyClient::removeSession POST: " << rpcUrl.str());

      proc->call(&(pConnection->client), &requestUri);
    }
    catch(std::exception& e)
    {
      OSS_LOG_ERROR("[RPC] RTPProxyClient::removeSession Exception: " << e.what());
    }
    catch(...)
    {
      OSS_LOG_ERROR("[RPC] RTPProxyClient::removeSession: Unknown exception.");
    }
    _connections.enqueue(pConnection);
  }
}


} } // OSS::RTP




