/*
 * Copyright (C) OSS Software Solutions
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

#ifndef OSS_HTTPSERVER_H_INCLUDED
#define	OSS_HTTPSERVER_H_INCLUDED


#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/Net.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "OSS/Net/AccessControl.h"
#include "OSS/Net/TLSManager.h"

namespace OSS {
namespace Net {

class HTTPServer : boost::noncopyable
{
public:
  typedef Poco::Net::HTTPServerRequest Request;
  typedef Poco::Net::HTTPServerResponse Response;
  typedef boost::function<void(Request&, Response&)> Handler;
  
  HTTPServer();
  
  HTTPServer(int maxQueuedConnections, int maxThreads);
  
  ~HTTPServer();
  
  bool start(const std::string& address, unsigned short port, bool secure);
  
  bool start(unsigned short port, bool secure);
  
  void stop();
  
  const std::string& getAddress() const;
  
  unsigned short getPort() const;

  void setHandler(const Handler& handler);
  
  bool isAuthorizedAddress(const std::string& host);

  AccessControl& accessControl();
private:
  bool _isSecure;
  OSS_HANDLE _socketHandle;
  OSS_HANDLE _secureSocketHandle;
  OSS_HANDLE _serverHandle;
  OSS_HANDLE _serverParamsHandle;
  int _maxQueuedConnections; 
  int _maxThreads;
  std::string _address;
  unsigned short _port;
  Handler _handler;
  AccessControl _accessControl;
}; 

//
// Inlines
//

inline bool HTTPServer::start(unsigned short port, bool secure)
{
  return start("", port, secure);
}

inline const std::string& HTTPServer::getAddress() const
{
  return _address;
}
  
inline unsigned short HTTPServer::getPort() const
{
  return _port;
}

inline void HTTPServer::setHandler(const Handler& handler)
{
  _handler = handler;
}

inline AccessControl& HTTPServer::accessControl()
{
  return _accessControl;
}
  
} } // OSS::Net




#endif	// OSS_HTTPSERVER_H_INCLUDED

