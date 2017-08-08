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

//
// WebSocket++ has the following license :
//

/*
 * Copyright (c) 2011, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef OSS_SIPWEBSOCKETTLSLISTENER_H_INCLUDED
#define	OSS_SIPWEBSOCKETTLSLISTENER_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_WEBSOCKETS

#include <boost/noncopyable.hpp>
#include "OSS/SIP/SIPListener.h"
#include "OSS/SIP/SIPWebSocketTlsConnection.h"
#include "OSS/SIP/SIPWebSocketTlsConnectionManager.h"

#include "OSS/UTL/Thread.h"


namespace OSS {
namespace SIP {


class SIPFSMDispatch;

class SIPWebSocketTlsListener :
		public OSS::SIP::SIPListener,
		private boost::noncopyable
{
public:

	typedef boost::shared_ptr<SIPWebSocketTlsListener> Ptr;

	// Handler for new websocket connections. This will just accept the connection
	// and pass it to the connection manager
	class ServerAcceptHandler : public websocketpp::server_tls::handler
	{
	public:
		ServerAcceptHandler(SIPWebSocketTlsListener& listener);

		void on_fail(websocketpp::server_tls::connection_ptr pConnection);
		void on_open(websocketpp::server_tls::connection_ptr pConnection);
    void validate(websocketpp::server_tls::connection_ptr pConnection);
    boost::shared_ptr<boost::asio::ssl::context> on_tls_init();
		SIPWebSocketTlsListener& _rListener;
	};


public:

  SIPWebSocketTlsListener(
    SIPTransportService* pTransportService,
    const std::string& address,
    const std::string& port,
    SIPWebSocketTlsConnectionManager& connectionManager);

  ~SIPWebSocketTlsListener();

  virtual void run();
    /// Run the server's io_service loop.

  virtual void handleAccept(const boost::system::error_code& e, OSS_HANDLE userData = 0);
    /// Handle completion of an asynchronous accept operation.

  virtual void handleStart();
    /// Handle a request to start the server.
  
  virtual void handleStop();
    /// Handle a request to stop the server.
  
  virtual void restart(boost::system::error_code& e);
    /// Restart a temporarily closed listener.  
  
  virtual void closeTemporarily(boost::system::error_code& e);
    /// Temporarily close the transport with a intention to restart it later on
  
  virtual bool canBeRestarted() const;
    /// returns true if the listener can safely be restarted
 
protected:
  void run_server();
  void run_client();

  websocketpp::server_tls::handler::ptr _pServerAcceptHandler;
  websocketpp::server_tls* _pServerEndPoint;

  SIPWebSocketTlsConnectionManager& _connectionManager;
  boost::asio::ip::tcp::resolver _resolver;
  /// The resolver service;

  boost::thread* _pServerThread;
  boost::thread* _pClientThread;
};


//
// Inlines
//

} } // OSS::SIP

#endif // ENABLE_FEATURE_WEBSOCKETS

#endif	/// OSS_SIPWEBSOCKETTLSLISTENER_H_INCLUDED

