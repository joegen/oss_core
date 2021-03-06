// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef OSS_ZMQSOCKET_H_INCLUDED
#define OSS_ZMQSOCKET_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_ZMQ

#include "OSS/OSS.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/Thread.h"
#include "OSS/build.h"


#if OSS_HAVE_ZMQ

#include "OSS/ZMQ/zmq.hpp"

namespace OSS {
namespace ZMQ {
  
  
class ZMQSocket : boost::noncopyable
{
public:
  enum SocketType
  {
    REQ,
    REP,
    PUSH,
    PULL,
    PUB,
    SUB
  };
  
  typedef zmq::pollitem_t PollItem;
  typedef std::vector<PollItem> PollItems;
  
  ZMQSocket(SocketType type, zmq::context_t* pContext = 0);
  
  ~ZMQSocket();
  
  bool connect(const std::string& peerAddress);
  
  bool bind(const std::string& bindAddress);
  
  bool subscribe(const std::string& event);
  
  bool publish(const std::string& event);
  
  bool sendAndReceive(const std::string& cmd, const std::string& data, std::string& response, unsigned int timeoutms);
  bool sendAndReceive(const std::string& cmd, const std::string& data, std::string& response);
  
  bool sendAndReceive(const std::string& data, std::string& response, unsigned int timeoutms);
  bool sendAndReceive(const std::string& data, std::string& response);

  bool sendRequest(const std::string& cmd, const std::string& data);
  bool sendRequest(const std::string& data);
  
  bool sendReply(const std::string& data);
  
  bool receiveReply(std::string& reply, unsigned int timeoutms);
  bool receiveReply(std::string& reply);
  
  bool receiveRequest(std::string& cmd, std::string& data, unsigned int timeoutms);
  bool receiveRequest(std::string& cmd, std::string& data);
  
  bool receiveRequest(std::string& data);
  bool receiveRequest(std::string& data, unsigned int timeoutms);
  
  void close();
  
  zmq::socket_t* socket();
  
  static int poll(ZMQSocket::PollItems& pollItems, long timeoutMiliseconds);
  
  void initPollItem(zmq_pollitem_t& item);
  
  bool pollRead(long timeoutms = 0);
  
  int getFd() const;
  
  const std::string& getBindAddress() const;
  
  const std::string& getConnectAddress() const;
protected:
  void internal_close();
  bool internal_connect(const std::string& peerAddress);
  bool internal_send_reply(const std::string& data);
  bool internal_send_request(const std::string& cmd, const std::string& data);
  bool internal_receive_reply(std::string& reply, unsigned int timeoutms);
  bool internal_receive_request(std::string& cmd, std::string& data, unsigned int timeoutms);
  bool internal_receive_request(std::string& data, unsigned int timeoutms);
  SocketType _type;
  zmq::context_t* _context;
  zmq::socket_t* _socket;
  std::string _peerAddress;
  std::string _bindAddress;
  OSS::mutex_critic_sec _mutex;
  bool _canReconnect;
  bool _isInproc;
  bool _isExternalContext;
  static zmq::context_t* _inproc_context;
};

//
// Inlines
//

inline bool ZMQSocket::sendAndReceive(const std::string& cmd, const std::string& data, std::string& response)
{
  return sendAndReceive(cmd, data, response, 0);
}

inline bool ZMQSocket::sendAndReceive(const std::string& data, std::string& response)
{
  return sendAndReceive(data, response, 0);
}

inline bool ZMQSocket::receiveRequest(std::string& cmd, std::string& data)
{
  return receiveRequest(cmd, data, 0);
}

inline bool ZMQSocket::receiveRequest(std::string& data)
{
  return receiveRequest(data, 0);
}

inline bool ZMQSocket::receiveReply(std::string& reply)
{
  return receiveReply(reply, 0);
}

inline zmq::socket_t* ZMQSocket::socket()
{
  return _socket;
}

inline const std::string& ZMQSocket::getBindAddress() const
{
  return _bindAddress;
}
  
inline const std::string& ZMQSocket::getConnectAddress() const
{
  return _peerAddress;
}

} } // OSS::ZMQ

#endif // OSS_HAVE_ZMQ

#endif // ENABLE_FEATURE_ZMQ

#endif // OSS_ZMQSOCKET_H_INCLUDED

