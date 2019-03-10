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


#include "OSS/SIP/SBC/SBCMediaProxy.h"
#include "OSS/SIP/SBC/SBCManager.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
static int get_node(const std::string& sessionId, bool& spillOver)
{
  if (sessionId.size() < 5)
    return 0;
  
  spillOver = false;
  char c = sessionId.at(4);
  
  switch(c)
  {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      spillOver = true;
      return 0;
    case '6':
      spillOver = true;
      return 1;
    case '7':
      spillOver = true;
      return 2;
    case '8':
      spillOver = true;
      return 3;
    case '9':
      spillOver = true;
      return 4;
  }
  
  return 0;
}
  
SBCMediaProxy::SBCMediaProxy(SBCManager* pManager) :
  _node0(0),
  _node1(1),
  _node2(2),
  _node3(3),
  _node4(4),
  _pManager(pManager)
{
}
  
SBCMediaProxy::~SBCMediaProxy()
{
}

bool SBCMediaProxy::initialize()
{
  if (!_node0.initialize())
    return false;
  if (!_node1.initialize())
    return false;
  if (!_node2.initialize())
    return false;
  if (!_node3.initialize())
    return false;
  if (!_node4.initialize())
    return false;
  
  return true;
}

SBCMediaProxyClient* SBCMediaProxy::getNode(const std::string& sessionId, bool& spillOver)
{
  int node = get_node(sessionId, spillOver);
  if (node == 0)
    return &_node0;
  else if (node == 1)
    return &_node1;
  else if (node == 2)
    return &_node2;
  else if (node == 3)
    return &_node3;
  else if (node == 4)
    return &_node4;
  return 0;
}
   
bool SBCMediaProxy::handleSDP(
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
  OSS_LOG_DEBUG("SBCMediaProxy handling SDP");
  bool spillOver = false;
  SBCMediaProxyClient* pNode = getNode(sessionId, spillOver);
  if (pNode)
  {
    return pNode->handleSDP(logId, sessionId, sentBy, packetSourceIP, packetLocalInterface, route, routeLocalInterface, requestType, sdp, rtpAttribute);
  }
  return false;
}

bool SBCMediaProxy::getSDP(const std::string& sessionId, std::string& lastOffer, std::string& lastAnswer)
{
  bool spillOver = false;
  SBCMediaProxyClient* pNode = getNode(sessionId, spillOver);
  if (pNode)
  {
    return pNode->getSDP( sessionId, lastOffer, lastAnswer);
  }
  return false;
}


bool SBCMediaProxy::removeSession(const std::string& sessionId)
{
  bool spillOver = false;
  SBCMediaProxyClient* pNode = getNode(sessionId, spillOver);
  if (pNode)
  {
    return pNode->removeSession(sessionId);
  }
  return false;
}

bool SBCMediaProxy::setMaxSession(unsigned int maxSession)
{
  _node0.setMaxSession(maxSession);
  _node1.setMaxSession(maxSession);
  _node2.setMaxSession(maxSession);
  _node3.setMaxSession(maxSession);
  _node4.setMaxSession(maxSession);
  return true;
}

unsigned int SBCMediaProxy::getMaxSession() const
{
  return _node0.getMaxSession();
}
  
unsigned int SBCMediaProxy::getSessionCount() const
{
  return _node0.getSessionCount() +
    _node1.getSessionCount() +
    _node2.getSessionCount() +
    _node3.getSessionCount() +
    _node4.getSessionCount();
}

} } } //OSS::SIP::SBC