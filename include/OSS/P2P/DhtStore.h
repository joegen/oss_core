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


#ifndef P2P_DHTSTORE_H_INCLUDED
#define	P2P_DHTSTORE_H_INCLUDED

#include <boost/noncopyable.hpp>
#include "OSS/Net/IPAddress.h"
#include "OSS/P2P/Overlay.h"
#include "OSS/Persistent/KeyValueStore.h"

namespace OSS {
namespace P2P {

class DhtStore : boost::noncopyable
{
public:
  enum Operation
  {
    DHT_PUT = 128,
    DHT_GET,
    DHT_DELETE,
    DHT_GET_RESPONSE,
    DHT_REPLICATE
  };

  static DhtStore& instance();
  bool initialize(const std::string& dbPath, unsigned short localPort, const OSS::Net::IPAddress& boostrapHost);

protected:
  void handlePut(const std::string& messageKey, int messageType, const std::string& payload);
  void handleGet(const std::string& messageKey, int messageType, const std::string& payload);
  void handleDelete(const std::string& messageKey, int messageType, const std::string& payload);
  void handleGetResponse(const std::string& messageKey, int messageType, const std::string& payload);
  void handleReplicate(const std::string& messageKey, int messageType, const std::string& payload);

private:
  DhtStore();
  static DhtStore _instance;
  OSS::P2P::Overlay _overlay;
  OSS::Persistent::KeyValueStore _oplog;
  OSS::Persistent::KeyValueStore _config;
  OSS::Persistent::KeyValueStore _data;
};

} } // OSS::P2P

#endif	// P2P_DHTSTORE_H_INCLUDED

