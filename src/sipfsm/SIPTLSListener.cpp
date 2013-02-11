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
#include "OSS/SIP/SIPTLSListener.h"
#include "OSS/SIP/SIPTransportService.h"


namespace OSS {
namespace SIP {


SIPTLSListener::SIPTLSListener(
  SIPTransportService* pTransportService,
  SIPFSMDispatch* dispatch,
  const std::string& address, 
  const std::string& port,
  const std::string& tlsCertFile,
  const std::string& diffieHellmanParamFile,
  const std::string& tlsPassword):
  SIPListener(pTransportService, address, port),
  _tlsContext(pTransportService->ioService(), boost::asio::ssl::context::tlsv1),
  _acceptor(pTransportService->ioService()),
  _resolver(pTransportService->ioService()),
  _connectionManager(dispatch),
  _pNewConnection(new SIPTLSConnection(pTransportService->ioService(), _tlsContext, _connectionManager)),
  _dispatch(dispatch),
  _tlsCertFile(tlsCertFile),
  _diffieHellmanParamFile(diffieHellmanParamFile),
  _tlsPassword(tlsPassword)
{
}

SIPTLSListener::~SIPTLSListener()
{
}

void SIPTLSListener::run()
{
  // Prepare the TLS context
  _tlsContext.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
  _tlsContext.use_certificate_chain_file(_tlsCertFile.c_str());
  _tlsContext.use_private_key_file(_tlsCertFile.c_str(), boost::asio::ssl::context::pem);
  _tlsContext.use_tmp_dh_file(_diffieHellmanParamFile.c_str());
  _tlsContext.set_password_callback(boost::bind(&SIPTLSListener::getTLSPassword, this));

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver::query query(getAddress(), getPort());
  boost::asio::ip::tcp::endpoint endpoint = *_resolver.resolve(query);
  _acceptor.open(endpoint.protocol());
  _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  _acceptor.bind(endpoint);
  _acceptor.listen();
  _acceptor.async_accept(dynamic_cast<SIPTLSConnection*>(_pNewConnection.get())->socket().lowest_layer(),
      boost::bind(&SIPTLSListener::handleAccept, this,
        boost::asio::placeholders::error, (void*)0));
}

void SIPTLSListener::handleAccept(const boost::system::error_code& e, OSS_HANDLE userData)
{
  if (!e)
  {
    _pNewConnection->setExternalAddress(_externalAddress);
    _connectionManager.start(_pNewConnection);
    _pNewConnection.reset(new SIPTLSConnection(*_pIoService, _tlsContext, _connectionManager));
    _acceptor.async_accept(dynamic_cast<SIPTLSConnection*>(_pNewConnection.get())->socket().lowest_layer(),
      boost::bind(&SIPTLSListener::handleAccept, this,
        boost::asio::placeholders::error, userData));
  }
}

void SIPTLSListener::connect(const std::string& address, const std::string& port)
{
  boost::asio::ip::tcp::resolver::query query(address, port);   
   _resolver.async_resolve(query,
        boost::bind(&SIPTLSListener::handleConnect, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::iterator));
}

void SIPTLSListener::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter)
{
  if (!e)
  {
    SIPTLSConnection::Ptr conn(new SIPTLSConnection(*_pIoService, _tlsContext, _connectionManager));
    _connectionManager.add(conn);
    conn->handleResolve(endPointIter);
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

