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
#include "OSS/SIP/SIPTLSListener.h"
#include "OSS/SIP/SIPTransportService.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {


SIPTLSListener::SIPTLSListener(
  SIPTransportService* pTransportService,
  const SIPTransportSession::Dispatch& dispatch,
  const std::string& address, 
  const std::string& port) :
  SIPListener(pTransportService, address, port),
  _acceptor(pTransportService->ioService()),
  _resolver(pTransportService->ioService()),
  _tlsContext(pTransportService->tlsServerContext()),
  _connectionManager(dispatch),
  _pNewConnection(new SIPStreamedConnection(pTransportService->ioService(), &_tlsContext, _connectionManager)),
  _dispatch(dispatch)
{
}

SIPTLSListener::~SIPTLSListener()
{
}

void SIPTLSListener::run()
{
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver::query query(getAddress(), getPort());
  boost::asio::ip::tcp::endpoint endpoint = *_resolver.resolve(query);
  _acceptor.open(endpoint.protocol());
  _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  _acceptor.bind(endpoint);
  _acceptor.listen();
  _acceptor.async_accept(dynamic_cast<SIPStreamedConnection*>(_pNewConnection.get())->socket().lowest_layer(),
      boost::bind(&SIPTLSListener::handleAccept, this,
        boost::asio::placeholders::error, (void*)0));
}

void SIPTLSListener::handleAccept(const boost::system::error_code& e, OSS_HANDLE userData)
{
  if (!e)
  {
    OSS_LOG_DEBUG("SIPTLSListener::handleAccept STARTING new connection");
    _pNewConnection->setExternalAddress(_externalAddress);
    _connectionManager.start(_pNewConnection);

    if (_acceptor.is_open())
    {
      OSS_LOG_DEBUG("SIPTLSListener::handleAccept RESTARTING async accept loop");
      _pNewConnection.reset(new SIPStreamedConnection(*_pIoService, &_tlsContext, _connectionManager));
      _acceptor.async_accept(dynamic_cast<SIPStreamedConnection*>(_pNewConnection.get())->socket().lowest_layer(),
        boost::bind(&SIPTLSListener::handleAccept, this,
          boost::asio::placeholders::error, userData));
    }
    else
    {
      OSS_LOG_DEBUG("SIPTLSListener::handleAccept ABORTING async accept loop");
    }
  }
  else
  {
    OSS_LOG_DEBUG("SIPTLSListener::handleAccept INVOKED with exception " << e.message());
  }
}

void SIPTLSListener::handleStop()
{
  // The server is stopped by cancelling all outstanding asynchronous
  // operations. Once all operations have finished the io_service::run() call
  // will exit.
  _acceptor.close();
  _connectionManager.stopAll();
}


} } // OSS::SIP

