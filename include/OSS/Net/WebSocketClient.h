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


#ifndef OSS_WEBSOCKETCLIENT_PP_H
#define OSS_WEBSOCKETCLIENT_PP_H

#include "OSS/build.h"
#if ENABLE_FEATURE_WEBSOCKETS

#include "OSS/UTL/BlockingQueue.h"
#include "OSS/UTL/Thread.h"
#include "OSS/Net/WebSocket.h"
#include "OSS/Net/IPAddress.h"


namespace OSS {
namespace Net {
  
  
class WebSocketClient
{
public:
  
  struct EventData
  {
    std::string event;
    std::string data;
  };
  
  WebSocketClient();
  ~WebSocketClient();
  
  bool connect(const std::string& url);
  bool send(const std::string& data);
  void receive(EventData& event);
  bool receive(EventData& event, long timeout);
  int getPollfd();
  void close();
  void signal_close();
  bool isOpen();
  bool getRemoteAddress(IPAddress& address);
  bool getLocalAddress(IPAddress& address);
protected:
  void on_close();
  void on_fail();
  class Handler : public websocketpp::client::handler
  {
  public:
    Handler(WebSocketClient* pClient);
    ~Handler();
    virtual void on_message(websocketpp::client::connection_ptr pConnection, websocketpp::message::data_ptr pMsg);
    virtual void on_open(websocketpp::client::connection_ptr pConnection);
    virtual void on_close(websocketpp::client::connection_ptr pConnection);
    virtual void on_fail(websocketpp::client::connection_ptr pConnection);
    virtual bool on_ping(websocketpp::client::connection_ptr pConnection, std::string data);
    virtual void on_pong(websocketpp::client::connection_ptr pConnection, std::string data);
    virtual void on_pong_timeout(websocketpp::client::connection_ptr pConnection, std::string data);
  private:
    WebSocketClient* _pClient;
  };
  
  Handler::ptr _pHandler;
  websocketpp::client* _pClient;
  websocketpp::client::connection_ptr _pConnection;
  boost::thread* _pClientThread;
  OSS::BlockingQueue<EventData> _eventQueue;
  OSS::mutex_critic_sec _ioMutex;
  bool _isOpen;
  friend class Handler;
};

//
// Inlines
//

inline int WebSocketClient::getPollfd()
{
  return _eventQueue.getFd();
}

inline void WebSocketClient::receive(EventData& event)
{
  _eventQueue.dequeue(event);
}

inline bool WebSocketClient::receive(EventData& event, long timeout)
{
  return _eventQueue.try_dequeue(event, timeout);
}

inline bool WebSocketClient::isOpen()
{
  return _isOpen;
}
  
  
} } // OSS::Net


#endif // ENABLE_FEATURE_WEBSOCKETS
#endif // OSS_WEBSOCKETCLIENT_PP_H

