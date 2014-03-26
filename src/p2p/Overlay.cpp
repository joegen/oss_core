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
    userData->instance->onMessage(pMsg->source.keystr, pKey->keystr, pMsg->type, payload);
}

void Overlay::onMessage(const std::string& senderKey, const std::string& messageKey, int messageType, const std::string& payload)
{
  MessageCallbackList::iterator iter = _messageCbList.find(messageType);
  if (iter != _messageCbList.end())
  {
    (iter->second)(senderKey, messageKey, messageType, payload);
  }
}

void overlay_update_upcall(ChimeraState* pState, Key* pKey, ChimeraHost* pHost, int joined)
{
  Overlay::Node node;
  node.key = pKey->keystr;
  node.name = pHost->name;
  node.hostPort = OSS::IPAddress(pHost->address);
  node.hostPort.setPort(pHost->port);

  Overlay::UserData* userData = (Overlay::UserData*)pState->userData;
  if (userData)
    userData->instance->onUpdate(node, joined ? true : false);
}

void Overlay::onUpdate(const Node& node, bool joined)
{
  if (joined)
  {
    _nodeList[node.key] = node;
  }
  else
  {
    _nodeList.erase(node.key);
  }

  if (_updateCb)
    _updateCb(node, joined);
}



bool Overlay::init(unsigned short port)
{
  ChimeraState* pState = chimera_init (port);
  if (pState)
  {
    pState->userData = (void*)&_userData;
    _state = pState;
    chimera_deliver(pState, overlay_deliver_upcall);
    chimera_update(pState, overlay_update_upcall);
  }
  return _state != 0;
}
  // Create an overlay listening on a the specified port
  //
bool Overlay::join(const OSS::IPAddress& bootstrapHost)
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
  // Join an overlay with bootstrap host
  //
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

bool Overlay::registerMessageType(int messageType, bool sendAck, MessageCallback cb)
{
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


} }



