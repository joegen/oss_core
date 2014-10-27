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


#include <vector>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPUDPConnectionClone.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPException.h"


namespace OSS {
namespace SIP {


SIPUDPConnectionClone::SIPUDPConnectionClone(SIPUDPConnection::Ptr clonable):
  
  _socket(dynamic_cast<SIPUDPConnection*>(clonable.get())->_socket),
  _senderEndPoint(dynamic_cast<SIPUDPConnection*>(clonable.get())->_senderEndPoint)
{
  _pDispatch = clonable->dispatch();
  _bytesRead = clonable->getLastReadCount();
  _orginalConnection = clonable;
  _isReliableTransport = false;
  _transportScheme = "udp";
  _externalAddress = clonable->getExternalAddress();
}

SIPUDPConnectionClone::~SIPUDPConnectionClone()
{
}

void SIPUDPConnectionClone::start(SIPFSMDispatch* pDispatch)
{
  _orginalConnection->start(pDispatch);
}


void SIPUDPConnectionClone::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred, OSS_HANDLE userData)
{
  _orginalConnection->handleRead(e, bytes_transferred, userData);
}

void SIPUDPConnectionClone::writeMessage(SIPMessage::Ptr msg, const std::string& ip, const std::string& port)
{
  _orginalConnection->writeMessage(msg, ip, port);
}

void SIPUDPConnectionClone::handleWrite(const boost::system::error_code& e)
{
  // This is only significant for stream based connections (TCP/TLS)
}

void SIPUDPConnectionClone::stop()
{
  // This is only significant for stream based connections (TCP/TLS)
}

void SIPUDPConnectionClone::writeMessage(SIPMessage::Ptr msg)
{
  // This is only significant for stream based connections (TCP/TLS)
  throw OSS::SIP::SIPException("Invalid UDP Transport Operation");
}

void SIPUDPConnectionClone::handleResolve(boost::asio::ip::tcp::resolver::iterator endPointIter)
{
  // This is only significant for stream based connections (TCP/TLS)
}

void SIPUDPConnectionClone::handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endPointIter)
{
  // This is only significant for stream based connections (TCP/TLS)
}

void SIPUDPConnectionClone::handleHandshake(const boost::system::error_code& error)
{
  // This is only significant for stream based connections (TCP/TLS)
}

void SIPUDPConnectionClone::clientBind(const OSS::Net::IPAddress& listener, unsigned short portBase, unsigned short portMax)
{
  // This is only significant for stream based connections (TCP/TLS)
}

void SIPUDPConnectionClone::clientConnect(const OSS::Net::IPAddress& target)
{
  // This is only significant for stream based connections (TCP/TLS)
}

OSS::Net::IPAddress SIPUDPConnectionClone::getLocalAddress() const
{
  boost::asio::ip::address ip = _socket.local_endpoint().address();
  return OSS::Net::IPAddress(ip.to_string(), _socket.local_endpoint().port());
}

OSS::Net::IPAddress SIPUDPConnectionClone::getRemoteAddress() const
{
   boost::asio::ip::address ip = _senderEndPoint.address();
  return OSS::Net::IPAddress(ip.to_string(), _senderEndPoint.port());
}

} } // OSS::SIP

