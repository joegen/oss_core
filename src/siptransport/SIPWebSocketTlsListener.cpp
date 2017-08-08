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
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#include <boost/asio.hpp>
#include "OSS/UTL/Logger.h"
#include "OSS/SIP/SIPTransportService.h"
#include "OSS/SIP/SIPWebSocketTlsListener.h"

//#include <boost/asio/ip/tcp.hpp>

namespace OSS {
namespace SIP {


SIPWebSocketTlsListener::ServerAcceptHandler::ServerAcceptHandler(SIPWebSocketTlsListener& listener):
		_rListener(listener)
{
}

boost::shared_ptr<boost::asio::ssl::context> SIPWebSocketTlsListener::ServerAcceptHandler::on_tls_init() 
{
  return _rListener.getTransportService()->tlsServerContextPtr();
}

void SIPWebSocketTlsListener::ServerAcceptHandler::on_fail(websocketpp::server_tls::connection_ptr pConnection)
{
  	//TODO: do proper error check here
  	// Call get_fail_reason() for a human readable error.
      /**
       * Returns the internal WS++ fail code. This code starts at a value of
       * websocketpp::fail::status::GOOD and will be set to other values as errors
       * occur. Some values are direct errors, others point to locations where
       * more specific error information can be found. Key values:
       *
       * (all in websocketpp::fail::status:: ) namespace
       * GOOD - No error has occurred yet
       * SYSTEM - A system call failed, the system error code is avaliable via
       *          get_fail_system_code()
       * WEBSOCKET - The WebSocket connection close handshake was performed, more
       *             information is avaliable via get_local_close_code()/reason()
       * UNKNOWN - terminate was called without a more specific error being set
       *
       * Refer to common.hpp for the rest of the individual codes.
       */
  	OSS_LOG_DEBUG("SIPWebSocketTlsListener::ServerAcceptHandler::on_fail reason: " << pConnection->get_fail_reason());
}

void SIPWebSocketTlsListener::ServerAcceptHandler::on_open(websocketpp::server_tls::connection_ptr pConnection)
{
  	OSS_LOG_DEBUG("SIPWebSocketTlsListener::ServerAcceptHandler::on_open");
  	boost::system::error_code ec;

  	//accept only sip websockets connections
//  	if (pConnection->get_resource() == "/sip")
//  	{
  		_rListener.handleAccept(ec, &pConnection);
//  	}
}

void SIPWebSocketTlsListener::ServerAcceptHandler::validate(websocketpp::server_tls::connection_ptr pConnection)
{
  pConnection->select_subprotocol("sip");
}

SIPWebSocketTlsListener::SIPWebSocketTlsListener(
  SIPTransportService* pTransportService,
  const std::string& address,
  const std::string& port,
  SIPWebSocketTlsConnectionManager& connectionManager) :
    SIPListener(pTransportService, address, port),
    _pServerEndPoint(0),
    _connectionManager(connectionManager),
    _resolver(pTransportService->ioService())
{
	_pServerThread = 0;
	_pClientThread = 0;
}

SIPWebSocketTlsListener::~SIPWebSocketTlsListener()
{
  handleStop();
}

void SIPWebSocketTlsListener::run()
{
  if (!_hasStarted)
  {
    assert(!_pServerThread);
    _pServerThread = new boost::thread(boost::bind(&SIPWebSocketTlsListener::run_server, this));
    _hasStarted = true;
  }
}

void SIPWebSocketTlsListener::run_server()
{
  assert(!_pServerEndPoint);
  try
  {
    _pServerAcceptHandler = websocketpp::server_tls::handler::ptr(new ServerAcceptHandler(*this));
    _pServerEndPoint = new websocketpp::server_tls(_pServerAcceptHandler);

    if (PRIO_DEBUG == log_get_level())
    {
      _pServerEndPoint->alog().set_level(websocketpp::log::alevel::ALL);
      _pServerEndPoint->elog().set_level(websocketpp::log::elevel::ALL);
    }
  
    boost::asio::ip::tcp::resolver::query query(getAddress(), getPort());
    boost::asio::ip::tcp::endpoint endpoint = *_resolver.resolve(query);
    _hasStarted = true;
    _pServerEndPoint->listen(endpoint);
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR("SIPWebSocketTlsListener::run_server " << _address << ":" << _port << " Exception: " << e.what());
    handleStop();
  }
  catch(...)
  {
    OSS_LOG_ERROR("SIPWebSocketTlsListener::run_server " << _address << ":" << _port << " UNKNOWN EXCEPTION");
    handleStop();
  }
}

void SIPWebSocketTlsListener::run_client()
{
	//TODO: Not yet implemented for websocket
	OSS_ASSERT(false);
}

void SIPWebSocketTlsListener::handleAccept(const boost::system::error_code& e, OSS_HANDLE connectionPtr)
{
  if (!e)
  {
    OSS_LOG_DEBUG("SIPWebSocketTlsListener::handleAccept STARTING new connection");

    websocketpp::server_tls::connection_ptr* pWsConnection =
    		reinterpret_cast<websocketpp::server_tls::connection_ptr*>(connectionPtr);
    OSS_VERIFY_NULL(pWsConnection);

    SIPWebSocketTlsConnection::Ptr pNewConnection(new SIPWebSocketTlsConnection(*pWsConnection, _connectionManager, this));

    pNewConnection->setExternalAddress(_externalAddress);
    _connectionManager.start(pNewConnection);
  }
  else
  {
    OSS_LOG_DEBUG("SIPWebSocketTlsListener::handleAccept INVOKED with exception " << e.message());
  }
}

void SIPWebSocketTlsListener::handleStart()
{
}


void SIPWebSocketTlsListener::handleStop()
{
  _connectionManager.stopAll();
  
  if (_pServerEndPoint)
  {
    _pServerEndPoint->stop_listen(true);
  }

  if (_pServerThread)
  {
	  _pServerThread->join();
	  delete _pServerThread;
	  _pServerThread = 0;
  }

  if (_pServerEndPoint)
  {
     delete _pServerEndPoint;
    _pServerEndPoint = 0;
  }
}

void SIPWebSocketTlsListener::restart(boost::system::error_code& e)
{
  if (canBeRestarted())
  {
    run();
    OSS_LOG_NOTICE("SIPWebSocketTlsListener::restart() address: " << _address << ":" << _port << " Ok");
  }
}
  
void SIPWebSocketTlsListener::closeTemporarily(boost::system::error_code& e)
{
  handleStop();
  OSS_LOG_NOTICE("SIPWebSocketTlsListener::closeTemporarily INVOKED");
}
  
bool SIPWebSocketTlsListener::canBeRestarted() const
{
  return !_pServerThread && !_pServerEndPoint;
}



} } // OSS::SIP



