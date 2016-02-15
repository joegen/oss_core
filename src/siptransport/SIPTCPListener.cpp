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
#include <boost/bind.hpp>
#include "OSS/SIP/SIPTCPListener.h"
#include "OSS/SIP/SIPTransportService.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {


SIPTCPListener::SIPTCPListener(
  SIPTransportService* pTransportService,
  const SIPTransportSession::Dispatch& dispatch,
  const std::string& address, 
  const std::string& port,
  SIPStreamedConnectionManager& connectionManager):
  SIPListener(pTransportService, address, port),
  _acceptor(pTransportService->ioService()),
  _resolver(pTransportService->ioService()),
  _connectionManager(connectionManager),
  _pNewConnection(new SIPStreamedConnection(pTransportService->ioService(), _connectionManager, this)),
  _dispatch(dispatch)
{
}

SIPTCPListener::~SIPTCPListener()
{
}

void SIPTCPListener::run()
{
  if (!_hasStarted)
  {
    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver::query query(getAddress(), getPort());
    boost::asio::ip::tcp::endpoint endpoint = *_resolver.resolve(query);
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();
    _acceptor.async_accept(dynamic_cast<SIPStreamedConnection*>(_pNewConnection.get())->socket(),
        boost::bind(&SIPTCPListener::handleAccept, this,
          boost::asio::placeholders::error, (void*)0));
    _hasStarted = true;
  }
}

void SIPTCPListener::restart(boost::system::error_code& e)
{
  if (!canBeRestarted())
  {
    OSS_LOG_ERROR("SIPTCPListener::restart() - Exception: canBeRestarted returned FALSE!");
    return;
  }
  
  try
  {
    _hasStarted = false;
    run();
    OSS_LOG_NOTICE("SIPTCPListener::restart() address: " << _address << ":" << _port << " Ok");
  }
  catch(const boost::system::error_code& err)
  {
    e = err;
  }
  catch(...)
  {
    OSS_LOG_ERROR("SIPTCPListener::restart() " << _address << ":" << _port << " UNKNOWN EXCEPTION");
  }
}
  
void SIPTCPListener::closeTemporarily(boost::system::error_code& e)
{
  _acceptor.close(e);
  OSS_LOG_NOTICE("SIPTLSListener::closeTemporarily INVOKED");
}
  
bool SIPTCPListener::canBeRestarted() const
{
  return _hasStarted && !_acceptor.is_open();
}

void SIPTCPListener::handleAccept(const boost::system::error_code& e, OSS_HANDLE userData)
{
  if (!e)
  {
    OSS_LOG_DEBUG("SIPTCPListener::handleAccept STARTING new connection");
    _pNewConnection->setExternalAddress(_externalAddress);
    _connectionManager.start(_pNewConnection);

    if (_acceptor.is_open())
    {
      OSS_LOG_DEBUG("SIPTCPListener::handleAccept RESTARTING async accept loop");
      _pNewConnection.reset(new SIPStreamedConnection(*_pIoService, _connectionManager, this));
      _acceptor.async_accept(dynamic_cast<SIPStreamedConnection*>(_pNewConnection.get())->socket(),
        boost::bind(&SIPTCPListener::handleAccept, this,
          boost::asio::placeholders::error, userData));
    }
    else
    {
      OSS_LOG_DEBUG("SIPTCPListener::handleAccept ABORTING async accept loop");
    }
  }
  else
  {
    OSS_LOG_DEBUG("SIPTCPListener::handleAccept INVOKED with exception " << e.message());
  }
}

void SIPTCPListener::handleStart()
{
}

void SIPTCPListener::handleStop()
{
  // The server is stopped by cancelling all outstanding asynchronous
  // operations. Once all operations have finished the io_service::run() call
  // will exit.
  _acceptor.close();
  _connectionManager.stopAll();
}


} } // OSS::SIP

