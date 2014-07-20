// Library: OSS_CORE - Foundation API for SIP B2BUA
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
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USvoidE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/P2P/Overlay.h"

extern "C"
{
  #include "chimera/chimera.h"
  #include "chimera/route.h"
}

namespace OSS {
namespace P2P {

Overlay::Overlay() :
  _state(0)
{
  _userData.instance = this;
}

Overlay::~Overlay()
{
}

void overlay_deliver_upcall(ChimeraState* pState, Key* pKey, Message* pMsg)
{
  std::string payload(pMsg->payload, pMsg->size);

  Overlay::UserData* userData = (Overlay::UserData*)pState->userData;
  if (userData)
    userData->instance->onMessage(pKey->keystr, pMsg->type, payload);
}

void Overlay::onMessage(const std::string& messageKey, int messageType, const std::string& payload)
{
  MessageCallbackList::iterator iter = _messageCbList.find(messageType);
  if (iter != _messageCbList.end())
  {
    (iter->second)(messageKey, messageType, payload);
  }
}

void overlay_update_upcall(ChimeraState* pState, Key* pKey, ChimeraHost* pHost, int joined)
{
  Overlay::Node node;
  node.key = pKey->keystr;
  node.name = pHost->name;

  unsigned char *ip = (unsigned char *)&(pHost->address);
  std::ostringstream ipaddr;
  ipaddr << (int)ip[0] << "." << (int)ip[1] << "." << (int)ip[2] << "." << (int)ip[3];
  node.hostPort = OSS::Net::IPAddress(ipaddr.str());
  node.hostPort.setPort(pHost->port);

  Overlay::UserData* userData = (Overlay::UserData*)pState->userData;
  if (userData)
    userData->instance->onUpdate(node, joined ? true : false);
}

void Overlay::onUpdate(const Node& node, bool joined)
{
  if (joined)
  {
    {
      OSS::mutex_write_lock lock(_nodeListMutex);
      _nodeList[node.key] = node;
    }
    
    {
      OSS::mutex_write_lock lock(_hostListMutex);
      _hostList[node.hostPort.toIpPortString()] = node;
    }
  }
  else
  {
    {
      OSS::mutex_write_lock lock(_nodeListMutex);
      _nodeList.erase(node.key);
    }
    
    {
      OSS::mutex_write_lock lock(_hostListMutex);
      _hostList.erase(node.hostPort.toIpPortString());
    }
  }

  if (_updateCb)
    _updateCb(node, joined);
}

bool Overlay::init(unsigned short port, int gracePeriod, int pingInterval)
{
  chimera_set_periods(pingInterval, gracePeriod);
  return init(port);
}

bool Overlay::init(unsigned short port)
{
  ChimeraState* pState = chimera_init (port);
  if (pState)
  {
    pState->userData = (void*)&_userData;
    _state = pState;
    ChimeraGlobal *chglob = (ChimeraGlobal *) pState->chimera;
    _nodeId = get_key_string(&chglob->me->key);
  
    chimera_deliver(pState, overlay_deliver_upcall);
    chimera_update(pState, overlay_update_upcall);
  }
  return _state != 0;
}
  // Create an overlay listening on a the specified port
  //
bool Overlay::join(const OSS::Net::IPAddress& bootstrapHost)
{
  ChimeraState* pState = static_cast<ChimeraState*>(_state);
  if (!pState)
    return false;

  ChimeraHost* pHost = host_get(pState, const_cast<char*>(bootstrapHost.toString().c_str()), bootstrapHost.getPort());

  if (!pHost)
    return false;

  chimera_join(pState, pHost);

  return true;
}

#if 0
bool Overlay::setLocalKey(const std::string& key)
{
  ChimeraState* pState = static_cast<ChimeraState*>(_state);
  if (!pState)
    return false;

  Key localKey;
  str_to_key (const_cast<char*>(key.c_str()), &localKey);

  key_makehash (pState->log, &localKey, const_cast<char*>(key.c_str()));

  chimera_setkey(pState, localKey);
  return true;
}
#endif

bool Overlay::registerMessageType(int messageType, bool sendAck, MessageCallback cb)
{
  //
  // Make sure we do not collide with internal chimera message types
  //
  OSS_ASSERT (messageType >= OVERLAY_MIN_MSG_TYPE);

  ChimeraState* pState = static_cast<ChimeraState*>(_state);
  if (!pState)
    return false;

  if (_messageCbList.find(messageType) != _messageCbList.end())
    return false;

  chimera_register(pState, messageType, sendAck ? 1 : 2);

  _messageCbList[messageType] = cb;

  return true;
}

bool Overlay::sendMessage(int messageType, const std::string& key, const std::string& payload)
{
  ChimeraState* pState = static_cast<ChimeraState*>(_state);
  if (!pState)
    return false;
  Key mykey;
  str_to_key(const_cast<char*>(key.c_str()), &mykey);
  chimera_send(pState, mykey, messageType, payload.size(), const_cast<char*>(payload.c_str()));
  return true;
}

void Overlay::getNodeList( Overlay::NodeList& nodes) const
{
  OSS::mutex_read_lock lock(_nodeListMutex);
  nodes = _nodeList;
}

void Overlay::getHostList( Overlay::NodeList& nodes) const
{
  OSS::mutex_read_lock lock(_hostListMutex);
  nodes = _hostList;
}

bool Overlay::getNode(const std::string& nodeId, Node& node) const
{
  OSS::mutex_read_lock lock(_nodeListMutex);
  NodeList::const_iterator iter = _nodeList.find(nodeId);
  if (iter == _nodeList.end())
    return false;
  node = iter->second;
  return true;
}

bool Overlay::getHost(const OSS::Net::IPAddress& hostPort, Node& node) const
{
  OSS::mutex_read_lock lock(_hostListMutex);
  NodeList::const_iterator iter = _hostList.find(hostPort.toIpPortString());
  if (iter == _hostList.end())
    return false;
  node = iter->second;
  return true;
}

int leafset_size (ChimeraHost ** arr)
{
    int i = 0;
    for (i = 0; arr[i] != NULL; i++);
    return i;
}

bool Overlay::getRightLeafSet(LeafSet& leafset, int maxNodes) const
{
  ChimeraState* pState = static_cast<ChimeraState*>(_state);
  if (!pState)
    return false;

  RouteGlobal *routeglob = (RouteGlobal *) pState->route;
  OSS_ASSERT(routeglob);

  pthread_mutex_lock (&routeglob->lock);

  int rsize = 0;
  for (rsize = 0; routeglob->rightleafset[rsize] != NULL; rsize++);

  if (maxNodes < rsize)
    rsize = maxNodes;

  leafset.clear();

  for (int i = 0; i < rsize; i++)
  {
    leafset.push_back(get_key_string(&routeglob->rightleafset[i]->key));
  }

  pthread_mutex_unlock (&routeglob->lock);

  return !leafset.empty();
}



} }



