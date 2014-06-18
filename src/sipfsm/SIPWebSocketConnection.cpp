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

#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPWebSocketConnection.h"
#include "OSS/SIP/SIPWebSocketConnectionManager.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/Logger.h"


namespace OSS {
namespace SIP {

SIPWebSocketConnection::ServerReadWriteHandler::ServerReadWriteHandler(SIPWebSocketConnection& rConnection):
  	_rConnection(rConnection)
{
}

void SIPWebSocketConnection::ServerReadWriteHandler::on_close(websocketpp::server::connection_ptr pConnection)
{
	OSS_LOG_DEBUG("SIPWebSocketConnection::ServerReadWriteHandler::on_close INVOKED");
}

void SIPWebSocketConnection::ServerReadWriteHandler::on_message(websocketpp::server::connection_ptr pConnection, websocketpp::server::handler::message_ptr pMsg)
{
  	boost::system::error_code ec;
  	std::string payload = pMsg->get_payload();
  	_rConnection.handleRead(ec, payload.size(), &payload);
}

void SIPWebSocketConnection::ServerReadWriteHandler::on_error(websocketpp::server::connection_ptr pConnection)
{
	//TODO: do proper error check here
	OSS_LOG_DEBUG("SIPWebSocketConnection::ServerReadWriteHandler::on_error reason:" << pConnection->get_fail_reason());

	std::string empty;
	_rConnection.handleRead(boost::asio::error::connection_aborted, 0, &empty);
}

bool SIPWebSocketConnection::ServerReadWriteHandler::on_ping(websocketpp::server::connection_ptr pConnection)
{
	OSS_LOG_DEBUG("SIPWebSocketConnection::ServerReadWriteHandler::on_ping INVOKED");
	return true;
}
void SIPWebSocketConnection::ServerReadWriteHandler::on_pong(websocketpp::server::connection_ptr pConnection)
{
	OSS_LOG_DEBUG("SIPWebSocketConnection::ServerReadWriteHandler::on_pong INVOKED");
}
void SIPWebSocketConnection::ServerReadWriteHandler::on_pong_timeout(websocketpp::server::connection_ptr pConnection,std::string)
{
	OSS_LOG_DEBUG("SIPWebSocketConnection::ServerReadWriteHandler::on_pong_timeout INVOKED");
}


SIPWebSocketConnection::SIPWebSocketConnection(SIPWebSocketConnectionManager& manager):
		_connectionManager(manager),
    _readExceptionCount(0)
{
	_transportScheme = "ws";
}

SIPWebSocketConnection::SIPWebSocketConnection(const websocketpp::server::connection_ptr& pConnection, SIPWebSocketConnectionManager& manager) :
  _pServerConnection(pConnection),
  _connectionManager(manager),
  _readExceptionCount(0)
{
	_transportScheme = "ws";

	_pServerConnectionHandler = websocketpp::server::handler::ptr(new ServerReadWriteHandler(*this));
	pConnection->set_handler(_pServerConnectionHandler);
}

SIPWebSocketConnection::~SIPWebSocketConnection()
{
	stop();
}

void SIPWebSocketConnection::start(SIPFSMDispatch* pDispatch)
    /// Start the first asynchronous operation for the connection.
{
	  _pDispatch = pDispatch;
}

void SIPWebSocketConnection::stop()
    /// Stop all asynchronous operations associated with the connection.
{
	if (_pServerConnection)
	{
		_pServerConnection->close(websocketpp::close::status::NORMAL);
	}
}

void SIPWebSocketConnection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData)
    /// Handle completion of a read operation.
{
	if (e || bytes_transferred <=0)
	{
		OSS_LOG_DEBUG("SIPWebSocketConnection::handleRead Exception " << e.message());

		if (++_readExceptionCount >= 5)
		{
			OSS_LOG_ERROR("SIPWebSocketConnection::handleRead has reached maximum exception count.  Bailing out.");
			boost::system::error_code ignored_ec;

			_connectionManager.stop(shared_from_this());
		}
	}

	OSS_LOG_DEBUG("SIPWebSocketConnection::handleRead STARTING new connection");
	std::string* buffer = reinterpret_cast<std::string*>(userData);

	//
	// set the last read address
	//
	if (!_lastReadAddress.isValid())
	{
		boost::system::error_code ec;

		EndPoint ep = _pServerConnection->get_raw_socket().remote_endpoint(ec);
		if (!ec)
		{
			boost::asio::ip::address ip = ep.address();
			_lastReadAddress = OSS::IPAddress(ip.to_string(), ep.port());
		}
		else
		{
			OSS_LOG_WARNING("SIPWebSocketConnection::handleRead() Exception " << ec.message());
		}
	}

	//
	// Reset the read exception count
	//
	_readExceptionCount = 0;

	_bytesRead =  bytes_transferred;
	if (!_pRequest)
	{
		_pRequest = SIPMessage::Ptr(new SIPMessage());
	}

	boost::tribool result;
	const char* begin = buffer->data();
	const char* end = buffer->data() + bytes_transferred;

	boost::tuple<boost::tribool, const char*> ret =  _pRequest->consume(begin, end);
	result = ret.get<0>();
	const char* tail = ret.get<1>();

	if (result)
	{
		//
		// Message has been read in full
		//
		_pDispatch->onReceivedMessage(_pRequest->shared_from_this(), shared_from_this());
		if (tail >= end)
		{
			//
			// The end of the SIPMessage is reached so we can simply reset the
			// request buffer and start the read operation all over again
			//
			_pRequest.reset();

			//
			// We are done
			//
			return;
		}
		else
		{
			//
			// This should not happen as there is one full message per read.
			// The tail is within the range of the end of the read buffer.
			//
			OSS_ASSERT(false);
		}
	}
	else if (!result)
	{
		_pRequest.reset();
	}
	else
	{
		//
		// This should not happen as there is one full message per read.
		// Partial message?
		//
		OSS_ASSERT(false);
	}
}

void SIPWebSocketConnection::handleWrite(const boost::system::error_code& e)
/// Handle completion of a write operation.
{
	// Not implemented for ws
	OSS_ASSERT(false);
}

void SIPWebSocketConnection::writeMessage(SIPMessage::Ptr msg)
/// Send a SIP message using this session.
{
	if (_pServerConnection)
	{
		_pServerConnection->send(msg->data(), websocketpp::frame::opcode::BINARY);
	}
}

bool SIPWebSocketConnection::writeKeepAlive()
    /// Send a connection specific keep-alive
    /// This is normally invoked by the application layer to
    /// keep NAT port bindings open as well as to poke
    /// reliability of the transport for stream based connections.
    /// The default behavior sends nothing
{
	return true;
}

bool SIPWebSocketConnection::writeKeepAlive(const std::string& ip, const std::string& port)
{
	// Not implemented for ws
	OSS_ASSERT(false);
}

void SIPWebSocketConnection::writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port)
    /// Send a SIP message using this session.  This is used by the UDP tranport
{
	  //
	  // This is connection oriented so ignore the remote target
	  //
	  writeMessage(msg);
}

void SIPWebSocketConnection::handleResolve(boost::asio::ip::tcp::resolver::iterator endPointIter)
{
	// Client side not implemented yet
	OSS_ASSERT(false);
}

void SIPWebSocketConnection::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter)
{
	// Client side not implemented yet
	OSS_ASSERT(false);
}

void SIPWebSocketConnection::handleHandshake(const boost::system::error_code& error)
{
	  // this is only significant for TLS
	OSS_ASSERT(false);
}

OSS::IPAddress SIPWebSocketConnection::getLocalAddress() const
    /// Returns the local address binding for this transport
{
	  if (_localAddress.isValid())
	    return _localAddress;

	  if (_pServerConnection)
	  {
	    boost::system::error_code ec;
	    EndPoint ep = _pServerConnection->get_raw_socket().local_endpoint(ec);
	    if (!ec)
	    {
	      boost::asio::ip::address ip = ep.address();
	      _localAddress = OSS::IPAddress(ip.to_string(), ep.port());
	      return _localAddress;
	    }
	    else
	    {
	      OSS_LOG_WARNING("SIPWebSocketConnection::getLocalAddress() Exception " << ec.message());
	    }
	  }

	  return OSS::IPAddress();
}

OSS::IPAddress SIPWebSocketConnection::getRemoteAddress() const
    /// Returns the last read source address
{
     if (_lastReadAddress.isValid())
       return _lastReadAddress;

     if (_pServerConnection)
     {
         boost::system::error_code ec;
         EndPoint ep = _pServerConnection->get_raw_socket().remote_endpoint(ec);
         if (!ec)
         {
           boost::asio::ip::address ip = ep.address();
           _lastReadAddress = OSS::IPAddress(ip.to_string(), ep.port());
           return _lastReadAddress;
         }
         else
         {
           OSS_LOG_WARNING("SIPWebSocketConnection::getRemoteAddress() Exception " << ec.message());
           return _connectAddress;
         }
     }

     return OSS::IPAddress();

}

void SIPWebSocketConnection::clientBind(const OSS::IPAddress& ip, unsigned short portBase, unsigned short portMax)
    /// Bind the local client
{
	OSS_ASSERT(false);
}

void SIPWebSocketConnection::clientConnect(const OSS::IPAddress& target)
    /// Connect to a remote host
{
	OSS_ASSERT(false);
}


} } /// OSS::SIP



