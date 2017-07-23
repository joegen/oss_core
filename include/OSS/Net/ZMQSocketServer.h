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

#ifndef OSS_ZMQSOCKETSERVER_H_INCLUDED
#define OSS_ZMQSOCKETSERVER_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_ZMQ

#include "OSS/ZMQ/ZMQSocket.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/Thread.h"
#include "OSS/Net/ServerTransport.h"

namespace OSS {
namespace Net {


class ZMQSocketServer : public ServerTransport
{
public:
  typedef OSS::ZMQ::ZMQSocket Socket;
  
  ZMQSocketServer();
  virtual ~ZMQSocketServer();
  
  virtual bool listen(const std::string& bindAddress);
  
  virtual void close();
  
  //
  // Connection callbacks
  //
  
  virtual void onOpen(int connectionId);
    /// A new connection is created
  
  virtual void onClose(int connectionId);
    /// The connection was closed 
  
  virtual void onError(int connectionId, const std::string& error);
    /// Conneciton encountered an error
  
  virtual void onMessage(int connectionId, const std::string& message);
    /// Connection receveid a new message
  
  virtual void onPing(int connectionId);
    /// Connection received a ping
  
  virtual void onPong(int connectionId);
    /// Connection received a pong
  
  virtual void onPongTimeout(int connectionId);
    /// Pong not received on time
  
  virtual bool sendMessage(int connectionId, const std::string& msg);
    /// Send a message using the connection identified by connectionId
  
protected:
  void internal_read_messages();
  Socket _socket;
  
private:
  boost::thread* _pThread;
  bool _isTerminated;
  std::string _bindAddress;
  std::string _closeString;
};

} } // OSS::NET

#endif // ENABLE_FEATURE_ZMQ
#endif // OSS_WEBSOCKETSERVER_H_INCLUDED

