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


#include "OSS/Net/WebSocketClient.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace Net {
  
  
WebSocketClient::WebSocketClient() :
  _eventQueue(true)
{
  _isOpen = false;
  _pClientThread = 0;
  _pHandler = Handler::ptr(new Handler(this));
  _pClient = new websocketpp::client(_pHandler);
  _pClient->alog().unset_level(websocketpp::log::alevel::ALL);
  _pClient->elog().unset_level(websocketpp::log::elevel::ALL);
}
  
WebSocketClient::~WebSocketClient()
{
  close();
  delete _pClient;
}

void WebSocketClient::close()
{
  OSS::mutex_critic_sec_lock lock(_ioMutex);
  if (_pConnection && _isOpen)
  {
    _pConnection->close(websocketpp::close::status::GOING_AWAY, "");
  }
  
  if (_pClientThread)
  {
    _pClientThread->join();
    delete _pClientThread;
    _pClientThread = 0;
  }
  _isOpen = false;
 
}

bool WebSocketClient::connect(const std::string& url)
{
  OSS::mutex_critic_sec_lock lock(_ioMutex);
  if (_pClientThread)
  {
    return false;
  }
  
  try
  {
    try
    {
      _pConnection = _pClient->get_connection(url);
    }
    catch(...)
    {
      _pClient->reset();
      _pConnection = _pClient->get_connection(url);
    }
    if (!_pConnection)
    {
      OSS_LOG_ERROR("Client connection is null while calling WebSocketClient::connect");
      return false;
    }
    _pClient->connect(_pConnection);
    _pClientThread = new boost::thread(boost::bind(&websocketpp::client::run, _pClient, false));
  }
  catch(std::exception& e)
  {
    OSS_LOG_ERROR("WebSocketClient::connect Exception: "  << e.what());
    return false;
  }
  return true;
}
  
WebSocketClient::Handler::Handler(WebSocketClient* pClient) :
  _pClient(pClient)
{  
}

WebSocketClient::Handler::~Handler()
{
}
    
void WebSocketClient::Handler::on_message(websocketpp::client::connection_ptr pConnection, websocketpp::message::data_ptr pMsg)
{
  WebSocketClient::EventData event;
  event.event = "on_message";
  event.data = pMsg->get_payload();
  _pClient->_eventQueue.enqueue(event);
}

void WebSocketClient::Handler::on_open(websocketpp::client::connection_ptr pConnection)
{
  _pClient->_isOpen = true;
  WebSocketClient::EventData event;
  event.event = "on_open";
  event.data = "Connection opened";
  _pClient->_eventQueue.enqueue(event);  
}

void WebSocketClient::Handler::on_close(websocketpp::client::connection_ptr pConnection)
{
  _pClient->_isOpen = false;
  WebSocketClient::EventData event;
  event.event = "on_close";
  event.data = "Connection closed";
  _pClient->_eventQueue.enqueue(event);
}

void WebSocketClient::Handler::on_fail(websocketpp::client::connection_ptr pConnection)
{
  WebSocketClient::EventData event;
  event.event = "on_fail";
  event.data = "Connection failed";
  _pClient->_eventQueue.enqueue(event);  
}

bool WebSocketClient::Handler::on_ping(websocketpp::client::connection_ptr pConnection, std::string data)
{
  WebSocketClient::EventData event;
  event.event = "on_ping";
  event.data = data;
  _pClient->_eventQueue.enqueue(event);
  return true;  
}

void WebSocketClient::Handler::on_pong(websocketpp::client::connection_ptr pConnection, std::string data)
{
  WebSocketClient::EventData event;
  event.event = "on_pong";
  event.data = data;
  _pClient->_eventQueue.enqueue(event);
}

void WebSocketClient::Handler::on_pong_timeout(websocketpp::client::connection_ptr pConnection, std::string data)
{
  WebSocketClient::EventData event;
  event.event = "on_pong_timeout";
  event.data = data;
  _pClient->_eventQueue.enqueue(event);
}

bool WebSocketClient::getRemoteAddress(IPAddress& address)
{
  OSS::mutex_critic_sec_lock lock(_ioMutex);
  if (!_pConnection)
  {
    return false;
  }
  
  boost::system::error_code ec;
  boost::asio::ip::tcp::endpoint ep = _pConnection->get_socket().remote_endpoint(ec);
  if (ec)
  {
    return false;
  }
  
  address = ep.address();
  address.setPort(ep.port());
  return true;
}

bool WebSocketClient::getLocalAddress(IPAddress& address)
{
  OSS::mutex_critic_sec_lock lock(_ioMutex);
  if (!_pConnection)
  {
    return false;
  }
  
  boost::system::error_code ec;
  boost::asio::ip::tcp::endpoint ep = _pConnection->get_socket().local_endpoint(ec);
  if (ec)
  {
    return false;
  }
  
  address = ep.address();
  address.setPort(ep.port());
  return true;
}

bool WebSocketClient::send(const std::string& data)
{
  OSS::mutex_critic_sec_lock lock(_ioMutex);
  if (!_pClient || !_pClientThread || !_pConnection)
  {
    return false;
  }
  
  try
  {
    _pConnection->send(data);
  }
  catch(std::exception& e)
  {
    return false;
  }
  return true;
}

  
} } // OSS::Net


