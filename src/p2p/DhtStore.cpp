
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

#include <sstream>
#include "OSS/P2P/DhtStore.h"

namespace OSS {
namespace P2P {

DhtStore DhtStore::_instance;

DhtStore& DhtStore::instance()
{
  return DhtStore::_instance;
}

DhtStore::DhtStore()
{
}

bool DhtStore::initialize(const std::string& dbPath, unsigned short localPort, const OSS::IPAddress& boostrapHost)
{
  std::ostringstream oplogPath;
  oplogPath << dbPath << ".oplog";

  std::ostringstream configPath;
  configPath << dbPath << ".oplog";

  if (!_oplog.open(oplogPath.str()))
    return false;

  if (!_config.open(configPath.str()))
    return false;

  if (!_data.open(dbPath))
    return false;

  if (!_overlay.init(localPort))
    return false;

  Overlay::MessageCallback putcb = boost::bind(&DhtStore::handlePut, this, _1, _2, _3);
  Overlay::MessageCallback getcb = boost::bind(&DhtStore::handleGet, this, _1, _2, _3);
  Overlay::MessageCallback delcb = boost::bind(&DhtStore::handleDelete, this, _1, _2, _3);
  Overlay::MessageCallback getrcb = boost::bind(&DhtStore::handleGetResponse, this, _1, _2, _3);
  Overlay::MessageCallback replcb = boost::bind(&DhtStore::handleReplicate, this, _1, _2, _3);

  _overlay.registerMessageType(DHT_PUT, true, putcb);
  _overlay.registerMessageType(DHT_GET, true, getcb);
  _overlay.registerMessageType(DHT_DELETE, true, delcb);
  _overlay.registerMessageType(DHT_GET_RESPONSE, true, getrcb);
  _overlay.registerMessageType(DHT_REPLICATE, true, replcb);

  return _overlay.join(boostrapHost);
}

void DhtStore::handlePut(const std::string& messageKey, int messageType, const std::string& payload)
{
}

void DhtStore::handleGet(const std::string& messageKey, int messageType, const std::string& payload)
{
}

void DhtStore::handleDelete(const std::string& messageKey, int messageType, const std::string& payload)
{
}

void DhtStore::handleGetResponse(const std::string& messageKey, int messageType, const std::string& payload)
{
}

void DhtStore::handleReplicate(const std::string& messageKey, int messageType, const std::string& payload)
{
}



} } // OSS::P2P



