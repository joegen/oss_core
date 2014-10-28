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


#ifndef SIP_SIPTLSListener_INCLUDED
#define SIP_SIPTLSListener_INCLUDED


#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "OSS/SIP/SIPListener.h"
#include "OSS/SIP/SIPStreamedConnection.h"
#include "OSS/SIP/SIPStreamedConnectionManager.h"


namespace OSS {
namespace SIP {


class SIPFSMDispatch;

class OSS_API SIPTLSListener: 
  public SIPListener,
  private boost::noncopyable
{
public:
  typedef boost::shared_ptr<SIPTLSListener> Ptr;
  
  SIPTLSListener(
    SIPTransportService* pTransportService,
    const SIPTransportSession::Dispatch& dispatch,
    const std::string& address, 
    const std::string& port
  );
    /// Construct the server to listen on the specified TCP address and port.

  virtual ~SIPTLSListener();
    /// Destroys the server.

  virtual void run();
    /// Run the server's io_service loop.

  virtual void handleAccept(const boost::system::error_code& e, OSS_HANDLE userData = 0);
    /// Handle completion of an asynchronous accept operation.

  virtual void handleStop();
    /// Handle a request to stop the server.

private:
  boost::asio::ip::tcp::acceptor _acceptor;
    /// Acceptor used to listen for incoming connections.

  boost::asio::ip::tcp::resolver _resolver;
    /// The resolver service;
  
  boost::asio::ssl::context& _tlsContext;
    /// The TLS Context we inherited from the transport service

  SIPStreamedConnectionManager _connectionManager;
    /// The connection manager which owns all live connections.

  SIPStreamedConnection::Ptr _pNewConnection;
    /// The next connection to be accepted.

  SIPTransportSession::Dispatch _dispatch;
    /// The handler for all incoming requests.

  
};

//
// Inlines
//



} } // OSS::SIP
#endif // SIP_SIPTLSListener_INCLUDED

