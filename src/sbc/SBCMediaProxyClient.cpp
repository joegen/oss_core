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



#include "OSS/SIP/SBC/SBCMediaProxyClient.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Net/Net.h"


namespace OSS {
namespace SIP {
namespace SBC {

//
// ZeroMQ poll timeout is in microseconds in Version 2
// and is changed to milliseconds in version 3!!!
//
#define RES_USE_POLL 1
#define ZMQ_READ_TIMEOUT 1000 * 1000 * 2


static const char * RESPONDER_BIND_ADDRESS_NODE_0 = "tcp://127.0.0.1:40590"; 
static const char * RESPONDER_BIND_ADDRESS_NODE_1 = "tcp://127.0.0.1:40591"; 
static const char * RESPONDER_BIND_ADDRESS_NODE_2 = "tcp://127.0.0.1:40592"; 
static const char * RESPONDER_BIND_ADDRESS_NODE_3 = "tcp://127.0.0.1:40593";
static const char * RESPONDER_BIND_ADDRESS_NODE_4 = "tcp://127.0.0.1:40594";   
  
static const char * RESPONDER_BIND_ADDRESS = "tcp://127.0.0.1:40597";
static const std::string& CMD_REMOVE_SESSION = "rtp.removeSession";  
static const std::string& CMD_MAX_SESSION = "rtp.rtpSessionMax";
static const std::string& CMD_HANDLE_SDP = "rtp.handleSDP";
static const std::string& CMD_GET_SDP = "rtp.getSDP";
static const unsigned int MAX_SESSION = 30;
  
void s_free (void *data, void *hint)
{
  free (data);
}
//  Convert string to 0MQ string and send to socket
static bool s_send (zmq::socket_t & socket, const std::string & data)
{
  char * buff = (char*)malloc(data.size());
  memcpy(buff, data.c_str(), data.size());
  zmq::message_t message((void*)buff, data.size(), s_free, 0);
  bool rc = socket.send(message);
  return (rc);
}

//  Sends string as 0MQ string, as multipart non-terminal
static bool s_sendmore (zmq::socket_t & socket, const std::string & data)
{
  char * buff = (char*)malloc(data.size());
  memcpy(buff, data.c_str(), data.size());
  zmq::message_t message((void*)buff, data.size(), s_free, 0);
  bool rc = socket.send(message, ZMQ_SNDMORE);
  return (rc);
}

static void s_receive (zmq::socket_t& socket, std::string& value)
{
  zmq::message_t message;
  socket.recv(&message);
  if (!message.size())
    return;
  value = std::string(static_cast<char*>(message.data()), message.size());
}

static zmq::socket_t* create_socket(zmq::context_t& context, int nodeIndex)
{
  zmq::socket_t* pSocket = new zmq::socket_t(context, ZMQ_REQ);
  int linger = 0;
  pSocket->setsockopt (ZMQ_LINGER, &linger, sizeof (linger));
  
  try
  {
    if (nodeIndex == -1)
    {
      pSocket->connect(RESPONDER_BIND_ADDRESS);
    }
    else if (nodeIndex == 0)
    {
      pSocket->connect(RESPONDER_BIND_ADDRESS_NODE_0);
    }
    else if (nodeIndex == 1)
    {
      pSocket->connect(RESPONDER_BIND_ADDRESS_NODE_1);
    }
    else if (nodeIndex == 2)
    {
      pSocket->connect(RESPONDER_BIND_ADDRESS_NODE_2);
    }
    else if (nodeIndex == 3)
    {
      pSocket->connect(RESPONDER_BIND_ADDRESS_NODE_3);
    }
    else if (nodeIndex == 4)
    {
      pSocket->connect(RESPONDER_BIND_ADDRESS_NODE_4);
    }
    else
    {
      OSS_LOG_ERROR("SBCMediaProxyClient::connect() - Invalid node index " << nodeIndex);
      delete pSocket;
      pSocket = 0;
    }
 
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR("SBCMediaProxyClient::connect() - ZMQ Exception: " << e.what());
    delete pSocket;
    pSocket = 0;
  }
  catch(...)
  {
    OSS_LOG_ERROR("SBCMediaProxyClient::connect() - Exception: ZMQ Unknown Exception");
    delete pSocket;
    pSocket = 0;
  }
  return pSocket;
}


SBCMediaProxyClient::SBCMediaProxyClient(int nodeIndex) :
  _context(1),
  _pSocket(0),
  _maxSession(MAX_SESSION),
  _sessionCount(0),
  _nodeIndex(nodeIndex)
{
}

SBCMediaProxyClient::~SBCMediaProxyClient()
{
  delete _pSocket;
}


bool SBCMediaProxyClient::initialize()
{
  //
  // Put initialization code here.  right there isn't any.
  //
  return true;
}

bool SBCMediaProxyClient::sendRequest(const std::string& logId, const std::string& cmd, const json::Object& params, json::Object& result)
{
  OSS::mutex_critic_sec_lock lock(_mutex);

  if  (!_pSocket)
    _pSocket = create_socket(_context, _nodeIndex);
  
  if (!_pSocket)
    return false;
  
  try
  {
    if (!s_sendmore(*_pSocket, cmd))
    {
      OSS_LOG_ERROR(logId << "SBCMediaProxyClient::sendRequest() - Exception: SENDMORE failed");
      delete _pSocket;
      _pSocket = 0;
      return false;
    }
    
    std::stringstream wstrm;
    json::Writer::Write(params, wstrm);
    std::string packet = wstrm.str();
    OSS_LOG_DEBUG(logId << "SBCMediaProxyClient::sendRequest() >>> Command: " << cmd << packet);
    if (!s_send(*_pSocket, packet))
    {
      OSS_LOG_ERROR(logId << "SBCMediaProxyClient::sendRequest() - Exception: SEND failed");
      delete _pSocket;
      _pSocket = 0;
      return false;
    }
    
#if RES_USE_POLL
    //
    // pool the socket for a response
    // delete it if no response is received
    //
    zmq::pollitem_t items[] = { { *_pSocket, 0, ZMQ_POLLIN, 0 } };
    zmq::poll (&items[0], 1, ZMQ_READ_TIMEOUT);
            
    if (items[0].revents & ZMQ_POLLIN) 
    {
#endif     
      std::string raw;
      s_receive(*_pSocket, raw);
      std::stringstream rstrm;
      rstrm << raw;
      OSS_LOG_DEBUG(logId << "SBCMediaProxyClient::sendRequest() <<< Command: " << cmd << raw);
      json::Reader::Read(result, rstrm);
 #if RES_USE_POLL
    }
    else
    {
      OSS_LOG_ERROR(logId << "SBCMediaProxyClient::sendRequest() - Exception: READ timeout!");
      delete _pSocket;
      _pSocket = 0;
      return false;
    }
#endif
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR(logId << "SBCMediaProxyClient::sendRequest() - Exception: " << e.what());
  }
  catch(...)
  {
    OSS_LOG_ERROR(logId << "SBCMediaProxyClient::sendRequest() - Exception: unknown");
  }
  return true;
}

bool SBCMediaProxyClient::handleSDP(
  const std::string& logId,
  const std::string& sessionId,
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  OSS::RTP::RTPProxySession::RequestType requestType,
  std::string& sdp,
  OSS::RTP::RTPProxy::Attributes& rtpAttribute)
{
  bool ok = false;
  
  try
  {
    json::Object params;
    params["logId"] = json::String(logId);
    params["sessionId"] = json::String(sessionId);
    params["sentBy"] = json::String(sentBy.toIpPortString());
    params["packetSourceIP"] = json::String(packetSourceIP.toIpPortString());
    params["packetLocalInterface"] = json::String(packetLocalInterface.toIpPortString());
    params["packetLocalInterfaceExternal"] = json::String(packetLocalInterface.externalAddress());
    params["route"] = json::String(route.toIpPortString());
    params["routeLocalInterface"] = json::String(routeLocalInterface.toIpPortString());
    params["routeLocalInterfaceExternal"] = json::String(routeLocalInterface.externalAddress());
    params["requestType"] = json::Number(requestType);
    params["maxSession"] = json::Number(_maxSession);  
    params["sdp"] = json::String(sdp);
    params["attr.verbose"] = json::Boolean(rtpAttribute.verbose);
    params["attr.forceCreate"] = json::Boolean(rtpAttribute.forceCreate);
    params["attr.legIndex"] = json::Number(rtpAttribute.legIndex);
    params["attr.forcePEAEncryption"] = json::Boolean(rtpAttribute.forcePEAEncryption);
    params["attr.callId"] = json::String(rtpAttribute.callId);
    params["attr.from"] = json::String(rtpAttribute.from);
    params["attr.to"] = json::String(rtpAttribute.to);
    params["attr.resizerSamplesLeg1"] = json::Number(rtpAttribute.resizerSamplesLeg1);
    params["attr.resizerSamplesLeg2"] = json::Number(rtpAttribute.resizerSamplesLeg2);
    params["attr.allowHairPin"] = json::Boolean(rtpAttribute.allowHairPin);
    
    json::Object result;
    ok = sendRequest(logId, CMD_HANDLE_SDP, params, result);
    if (ok)
    {
      json::Object::iterator sdpIter = result.Find("sdp");
      if (sdpIter == result.End())
      {
        ok = false;
        json::Object::iterator errorIter = result.Find("error");
        if (errorIter != result.End())
        {
          json::String& errorVal = result["error"];
          OSS_LOG_ERROR("SBCMediaProxyClient::handleSDP RPC-Exception: " << errorVal.Value());
        }
      }
      else
      {
        if (result.Find("sdp") != result.End())
        {
          json::String& sdpVal = result["sdp"];
          sdp = sdpVal.Value();
        }
        
        if (result.Find("sessionCount") != result.End())
        {
          json::Number& sessionCount = result["sessionCount"];
          _sessionCount = sessionCount.Value();
        }
      }
    }
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR("SBCMediaProxyClient::handleSDP Exception: " << e.what());
  }
  catch(...)
  {
    OSS_LOG_ERROR("SBCMediaProxyClient::handleSDP Exception: unknown"); 
  }
  
  return ok;
}

bool SBCMediaProxyClient::removeSession(const std::string& sessionId)
{
  bool ok = false;
  try
  {
    json::Object params;
    params["sessionId"] = json::String(sessionId);
    json::Object result;
    ok =  sendRequest("", CMD_REMOVE_SESSION, params, result);

    if (result.Find("sessionCount") != result.End())
    {
      json::Number& sessionCount = result["sessionCount"];
      _sessionCount = sessionCount.Value();
    }
  }
  catch(const std::exception& e)
  {
    ok = false;
    OSS_LOG_ERROR("SBCMediaProxyClient::removeSession Exception: " << e.what());
  }
  catch(...)
  {
    ok = false;
    OSS_LOG_ERROR("SBCMediaProxyClient::removeSession Exception: unknown"); 
  }
  return ok;
}

bool SBCMediaProxyClient::setMaxSession(unsigned int maxSession)
{
  _maxSession = maxSession;
  
  if (_isConnected)
  {
    try
    {
      json::Object params;
      params["maxSession"] = json::Number(maxSession);
      json::Object result;
      return sendRequest("", CMD_MAX_SESSION, params, result);
    }
    catch(const std::exception& e)
    {
      OSS_LOG_ERROR("SBCMediaProxyClient::setMaxSession Exception: " << e.what());
      return false;
    }
    catch(...)
    {
      OSS_LOG_ERROR("SBCMediaProxyClient::setMaxSession Exception: unknown");
      return false;
    }
  }
  
  return true;
}

bool SBCMediaProxyClient::getSDP(const std::string& sessionId, std::string& lastOffer, std::string& lastAnswer)
{
  bool ok = false;
  try
  {
    json::Object params;
    params["sessionId"] = json::String(sessionId);
    json::Object result;
    ok =  sendRequest("", CMD_GET_SDP, params, result);

    if (result.Find("last-offer") != result.End())
    {
      json::String& val = result["last-offer"];
      lastOffer = val.Value();
    }

    if (result.Find("last-answer") != result.End())
    {
      json::String& val = result["last-answer"];
      lastAnswer = val.Value();
    }
    
    json::Object::iterator errorIter = result.Find("error");
    if (errorIter != result.End())
    {
      ok = false;
      json::String& errorVal = result["error"];
      OSS_LOG_ERROR("SBCMediaProxyClient::getSDP RPC-Exception: " << errorVal.Value());
    }
  }
  catch(const std::exception& e)
  {
    ok = false;
    OSS_LOG_ERROR("SBCMediaProxyClient::getSDP Exception: " << e.what());
  }
  catch(...)
  {
    ok = false;
    OSS_LOG_ERROR("SBCMediaProxyClient::getSDP Exception: unknown"); 
  }
  return ok;
}
  


} } } // OSS::SIP::SBC


