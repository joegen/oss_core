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

#ifndef RTPPROXYCLIENT_H_INCLUDED
#define	RTPPROXYCLIENT_H_INCLUDED


#include <string.h>
#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client.hpp>

#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/JSON/elements.h"
#include "OSS/UTL/BlockingQueue.h"
#include "OSS/RTP/RTPProxy.h"
#include "OSS/RTP/RTPProxySession.h"


namespace OSS {
namespace RTP {


class RTPProxyClient
{
public:
  struct Connection
  {
    xmlrpc_c::clientXmlTransport_curl transport;
    xmlrpc_c::client_xml client;

    Connection() :
      transport(
            xmlrpc_c::clientXmlTransport_curl::constrOpt()
            .no_ssl_verifyhost(true)
            .user_agent("RTPProxyClient/1.0")),
      client(&transport)
    {
    }
  };

  RTPProxyClient(const OSS::Net::IPAddress& rpcHost, int poolSize);
  ~RTPProxyClient();
  void handleSDP(
    const std::string& logId,
    const std::string& sessionId,
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RTPProxySession::RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void removeSession(const std::string& sessionId);
protected:
  int _poolSize;
  BlockingQueue<Connection*> _connections;
  OSS::Net::IPAddress _rpcHost;
};
    
} } // OSS::RTP


#endif	// RTPPROXYCLIENT_H_INCLUDED

