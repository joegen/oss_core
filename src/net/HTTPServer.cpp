/*
 * Copyright (C) 2013  OSS Software Solutions
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


#include "OSS/Net/HTTPServer.h"
#include "OSS/Logger.h"
#include "OSS/IPAddress.h"

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SecureStreamSocket.h"
#include "Poco/Net/SecureServerSocket.h"
#include "Poco/Net/X509Certificate.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include <iostream>


using Poco::Net::ServerSocket;
using Poco::Net::SocketAddress;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Net::X509Certificate;
using Poco::Net::SecureServerSocket;
using Poco::Net::SecureStreamSocket;
using Poco::ThreadPool;


namespace OSS {
namespace Net {

#define HTTP_SERVER_MAX_QUEUED_CONNECTIONS 100
#define HTTP_SERVER_MAX_THREADS 8

class HTTPServerRequestHandler: public HTTPRequestHandler
{
public:
	HTTPServerRequestHandler(HTTPServer::Handler& handler) :
    _handler(handler)
	{
	}
	
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
    if (_handler)
      _handler(request, response);
	}
  
  HTTPServer::Handler& _handler;
};
  
class HTTPServerHandlerFactory: public HTTPRequestHandlerFactory
{
public:
	HTTPServerHandlerFactory(HTTPServer* pServer, HTTPServer::Handler& handler) :
    _handler(handler),
    _pServer(pServer)
	{
	}

	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
	{
    if (!_handler)
      return 0;
    
    std::string src = request.clientAddress().host().toString();
    //
    // Check if this IP is allowed by the server
    //
    if (!_pServer->isAuthorizedAddress(src))
      return 0;
    
		return new HTTPServerRequestHandler(_handler);
	}
  
  HTTPServer::Handler& _handler;
  HTTPServer* _pServer;
};

HTTPServer::HTTPServer() :
  _socketHandle(0),
  _secureSocketHandle(0),
  _serverHandle(0),
  _serverParamsHandle(0),
  _maxQueuedConnections(HTTP_SERVER_MAX_QUEUED_CONNECTIONS),
  _maxThreads(HTTP_SERVER_MAX_THREADS)
{
}

HTTPServer::HTTPServer(int maxQueuedConnections, int maxThreads) :
  _socketHandle(0),
  _secureSocketHandle(0),
  _serverHandle(0),
  _serverParamsHandle(0),
  _maxQueuedConnections(maxQueuedConnections),
  _maxThreads(maxThreads)
{
}

HTTPServer::~HTTPServer()
{
  stop();
}
  
bool HTTPServer::start(const std::string& address, unsigned short port, bool secure)
{
  if (!_handler)
  {
    OSS_LOG_ERROR("HTTPServer::start - Handler not set.");
    return false;  
  }
  
  _address = address;
  _port = port;
  _isSecure = secure;
    
  
  try
  {
    ServerSocket* pSocket = 0;
    SecureServerSocket* pSecureSocket = 0;

    if (!_isSecure)
    {
      if (_address.empty())
      {
        pSocket = new ServerSocket(port);
      }
      else
      {
        SocketAddress sockAddress(_address, port);
        pSocket = new ServerSocket(sockAddress);
        
      }
      _socketHandle = (OSS_HANDLE)pSocket;
    }
    else
    {
      Poco::Net::initializeSSL();
      if (_address.empty())
      {
        pSecureSocket = new SecureServerSocket(port);
      }
      else
      {
        SocketAddress sockAddress(_address, port);
        pSecureSocket = new SecureServerSocket(sockAddress);
      }
      _secureSocketHandle = (OSS_HANDLE)pSecureSocket;
    }

    

    HTTPServerParams* pParams = new HTTPServerParams();
    _serverParamsHandle = (OSS_HANDLE)pParams;

    //
    // Grow the default pool so it has enough to accommodate this servers requirement 
    //
    ThreadPool::defaultPool().addCapacity(_maxThreads);

    pParams->setMaxQueued(_maxQueuedConnections);
    pParams->setMaxThreads(_maxThreads);

    Poco::Net::HTTPServer* pHTTPServer = new Poco::Net::HTTPServer(new HTTPServerHandlerFactory(this, _handler), _isSecure ? *pSecureSocket : *pSocket, pParams);
    _serverHandle = (OSS_HANDLE)pHTTPServer;

    pHTTPServer->start();
       
    return true;
  }
  catch(Poco::Exception e)
  {
    OSS_LOG_ERROR("HTTPServer::start - Exception: " << e.message());
    return false;
  }
}


void HTTPServer::stop()
{
  if (_serverHandle)
  {
    Poco::Net::HTTPServer* pHTTPServer = (Poco::Net::HTTPServer*)_serverHandle;
    pHTTPServer->stop();
  }
  
  delete (ServerSocket*)_socketHandle;
  delete (SecureServerSocket*)_secureSocketHandle;
  delete (Poco::Net::HTTPServer*)_serverHandle;
  _socketHandle = 0;
  _serverHandle = 0;
  _secureSocketHandle = 0;
  
  if (_isSecure)
  {
    Poco::Net::uninitializeSSL();
  }
}

bool HTTPServer::isAuthorizedAddress(const std::string& host)
{
  OSS::IPAddress addr(host);
  _accessControl.logPacket(addr.address(), -1);
  return _accessControl.isBannedAddress(addr.address());
}
  

} } // OSS::Net
