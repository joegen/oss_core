// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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
#include "OSS/Logger.h"


namespace OSS {
namespace SIP {


SIPTCPListener::SIPTCPListener(
  SIPTransportService* pTransportService,
  SIPFSMDispatch* dispatch,
  const std::string& address, 
  const std::string& port,
  SIPTCPConnectionManager& connectionManager):
  SIPListener(pTransportService, address, port),
  _acceptor(pTransportService->ioService()),
  _resolver(pTransportService->ioService()),
  _connectionManager(connectionManager),
  _pNewConnection(new SIPTCPConnection(pTransportService->ioService(), _connectionManager)),
  _dispatch(dispatch)
{
}

SIPTCPListener::~SIPTCPListener()
{
}

void SIPTCPListener::run()
{
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver::query query(getAddress(), getPort());
  boost::asio::ip::tcp::endpoint endpoint = *_resolver.resolve(query);
  _acceptor.open(endpoint.protocol());
  _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  _acceptor.bind(endpoint);
  _acceptor.listen();
  _acceptor.async_accept(dynamic_cast<SIPTCPConnection*>(_pNewConnection.get())->socket(),
      boost::bind(&SIPTCPListener::handleAccept, this,
        boost::asio::placeholders::error, 0));
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
      _pNewConnection.reset(new SIPTCPConnection(*_pIoService, _connectionManager));
      _acceptor.async_accept(dynamic_cast<SIPTCPConnection*>(_pNewConnection.get())->socket(),
        boost::bind(&SIPTCPListener::handleAccept, this,
          boost::asio::placeholders::error, 0));
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

void SIPTCPListener::connect(const std::string& address, const std::string& port)
{
  boost::asio::ip::tcp::resolver::query query(address, port);   
   _resolver.async_resolve(query,
        boost::bind(&SIPTCPListener::handleConnect, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::iterator));
}

void SIPTCPListener::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter)
{
  if (!e)
  {
    SIPTCPConnection::Ptr conn(new SIPTCPConnection(*_pIoService, _connectionManager));
    _connectionManager.add(conn);
    conn->handleResolve(endPointIter);
  }
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

