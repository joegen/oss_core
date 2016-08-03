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
    PULL
  };
  
  ZMQSocket(SocketType type);
  
  ~ZMQSocket();
  
  bool connect(const std::string& peerAddress);
  
  bool bind(const std::string& bindAddress);
  
  bool sendAndReceive(const std::string& cmd, const std::string& data, std::string& response, unsigned int timeoutms);

  bool sendRequest(const std::string& cmd, const std::string& data);
  
  bool sendReply(const std::string& data);
  
  bool receiveReply(std::string& response, unsigned int timeoutms);
  
  bool receiveRequest(std::string& cmd, std::string& data, unsigned int timeoutms);
  
  void close();
  
protected:
  void internal_close();
  bool internal_connect(const std::string& peerAddress);
  bool internal_send_reply(const std::string& data);
  bool internal_send_request(const std::string& cmd, const std::string& data);
  bool internal_receive_reply(std::string& response, unsigned int timeoutms);
  bool internal_receive_request(std::string& cmd, std::string& data, unsigned int timeoutms);
  SocketType _type;
  OSS_HANDLE _context;
  OSS_HANDLE _socket;
  std::string _peerAddress;
  std::string _bindAddress;
  OSS::mutex_critic_sec _mutex;
  bool _canReconnect;
};
    
} } // OSS::ZMQ

#endif // OSS_HAVE_ZMQ

#endif // ENABLE_FEATURE_ZMQ

#endif // OSS_ZMQSOCKET_H_INCLUDED

