
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


#include "OSS/EP/EndpointConnection.h"
#include "OSS/EP/EndpointListener.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace EP {


EndpointConnection::EndpointConnection(EndpointListener* pEndpoint) :
  SIPTransportSession(),
  _pEndpoint(pEndpoint)
{
  //
  // Endpoint connections must be set reliable or else we will get retransmission events
  //
  _isReliableTransport = true;
  _isEndpoint = true;
  _endpointName = _pEndpoint->getEndpointName();
  _identifier = 0xFFFF;
  _transportScheme = _endpointName;
}
  
EndpointConnection::~EndpointConnection()
{
}
  
void EndpointConnection::writeMessage(SIPMessage::Ptr pMsg)
{
  OSS_LOG_DEBUG(pMsg->createContextId(true) << "EndpointConnection::writeMessage( " << getTransportScheme() << " ) - Handling request " << pMsg->startLine());
  _pEndpoint->postEvent(pMsg);
}

void EndpointConnection::writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port)
{
  writeMessage(msg);
}

bool EndpointConnection::writeKeepAlive()
{
  return false;
}

bool EndpointConnection::writeKeepAlive(const std::string& ip, const std::string& port)
{
  return false;
}

void EndpointConnection::start(const SIPTransportSession::Dispatch& dispatch)
{
  //
  // Not implemented
  //
}

void EndpointConnection::stop()
{
  //
  // Not implemented
  //
}

void EndpointConnection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData)
{
  //
  // Not implemented
  //
  assert(false);
}

void EndpointConnection::handleWrite(const boost::system::error_code& e)
{
  //
  // Not implemented
  //
  assert(false);
}

void EndpointConnection::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter, boost::system::error_code* out_ec, Semaphore* pSem)
{
  //
  // Not implemented
  //
  assert(false);
}

void EndpointConnection::handleClientHandshake(const boost::system::error_code& error)
{
  //
  // Not implemented
  //
  assert(false);
}

void EndpointConnection::handleServerHandshake(const boost::system::error_code& error)
{
  //
  // Not implemented
  //
  assert(false);
}

OSS::Net::IPAddress EndpointConnection::getLocalAddress() const
{
  OSS::Net::IPAddress ip(_pEndpoint->getAddress());
  ip.setPort(OSS::string_to_number<unsigned short>(_pEndpoint->getPort()));
  return ip;
}

OSS::Net::IPAddress EndpointConnection::getRemoteAddress() const
{
  //
  // Note: Endpoint connection points back to itself.  Three is no concept of remote address
  //
  OSS::Net::IPAddress ip(_pEndpoint->getAddress());
  ip.setPort(OSS::string_to_number<unsigned short>(_pEndpoint->getPort()));
  return ip;
}

void EndpointConnection::clientBind(const OSS::Net::IPAddress& ip, unsigned short portBase, unsigned short portMax)
{
  //
  // Not implemented
  //
  assert(false);
}

bool EndpointConnection::clientConnect(const OSS::Net::IPAddress& target)
{
  //
  // Not implemented
  //
  assert(false);
  return false;
}


  
 
} } 



