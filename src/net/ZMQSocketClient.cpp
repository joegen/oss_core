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


#include "OSS/Net/ZMQSocketClient.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Net/rtnl_get_route.h"


namespace OSS {
namespace Net {

ZMQSocketClient::ZMQSocketClient() :
  _pSocket(0),
  _isOpen(false)
{
  _pSocket = new ZMQSocketClient::Socket(ZMQSocketClient::Socket::REQ);
}

ZMQSocketClient::~ZMQSocketClient()
{
  delete _pSocket;
}

bool ZMQSocketClient::connect(const std::string& ipPort)
{
  if (!OSS::string_starts_with(ipPort, "tcp://"))
  {
    _remoteAddress = IPAddress::fromV4IPPort(ipPort.c_str());
  }
  else
  {
    std::string addr;
    addr = ipPort.substr(strlen("tcp://"));
    _remoteAddress = IPAddress::fromV4IPPort(addr.c_str());
  }
  
  if (!_remoteAddress.isValid())
  {
    return false;
  }
  std::ostringstream url;
  url << "tcp://" << _remoteAddress.toIpPortString();
  _isOpen = _pSocket->connect(url.str());
  if (_isOpen)
  {
    //
    // ZeroMQ really doesn't expose addresses so we cheat here
    //
    RTNLRoutes routes;
    std::string localInterface;
    if (rtnl_get_route(routes, true) && rtnl_get_source(routes, localInterface, _remoteAddress.toString(), true))
    {
      _localAddress = IPAddress::fromV4IPPort(localInterface.c_str());
    }
    _eventQueue.enqueue(EventOpen);
  }
  else
  {
    _eventQueue.enqueue(EventFail);
  }
  return _isOpen;
}

bool ZMQSocketClient::send(const std::string& data)
{
  if (_pSocket->sendRequest(data))
  {
    _eventQueue.enqueue(EventMessage);
    return true;
  }
  return false;
}

void ZMQSocketClient::receive(EventData& event)
{
  EventType eventType = EventUnknown;
  _eventQueue.dequeue(eventType);
  event.event = eventType;
  if (eventType == EventMessage)
  {
    _pSocket->receiveReply(event.data);
  }
}

bool ZMQSocketClient::receive(EventData& event, long timeout)
{
  EventType eventType = EventUnknown;
  if (!_eventQueue.try_dequeue(eventType, timeout))
  {
    return false;
  }
  event.event = eventType;
  if (event.event == EventMessage)
  {
    return _pSocket->receiveReply(event.data, timeout);
  }
  return true;
}

bool ZMQSocketClient::sendAndReceive(const std::string& data, std::string& response, long timeout)
{
  return _pSocket->sendAndReceive(data, response, timeout);
}


int ZMQSocketClient::getPollfd()
{
  //
  // ZMQ Sockets are not pollable using normal socket polling
  //
  return -1;
}

void ZMQSocketClient::close()
{
  if (_isOpen)
  {
    OSS_LOG_DEBUG("ZMQSocketClient::close()");
    _pSocket->close();
    _isOpen = false;
    _eventQueue.enqueue(EventClose);
  }
}

void ZMQSocketClient::signal_close()
{
  close();
}

bool ZMQSocketClient::isOpen()
{
  return _isOpen;
}

bool ZMQSocketClient::getRemoteAddress(IPAddress& address)
{
  address = _remoteAddress;
  return _remoteAddress.isValid();
}

bool ZMQSocketClient::getLocalAddress(IPAddress& address)
{
  address = _localAddress;
  return _localAddress.isValid();
}

} } // OSS::Net

