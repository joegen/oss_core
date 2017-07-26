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

#ifndef OSS_ZMQSOCKETCLIENT_H
#define OSS_ZMQSOCKETCLIENT_H

#include "OSS/build.h"
#if ENABLE_FEATURE_ZMQ

#include "OSS/ZMQ/ZMQSocket.h"
#include "OSS/Net/ClientTransport.h"
#include "OSS/UTL/BlockingQueue.h"


namespace OSS {
namespace Net {


class ZMQSocketClient : public ClientTransport
{
public:
  typedef OSS::ZMQ::ZMQSocket Socket;
  typedef OSS::BlockingQueue<ClientTransport::EventType> RequestQueue;
  
  ZMQSocketClient();
  virtual ~ZMQSocketClient();
  
  virtual bool connect(const std::string& ipPort);
    // Create a connection to the remote server
  
  virtual bool send(const std::string& data);
    // Send data to the server
  
  virtual void receive(EventData& event);
    // Receive an event from the server or local transport
  
  virtual bool receive(EventData& event, long timeout);
    // Receive an event from the server or local transport with timeout
  
  virtual bool implementsSendAndReceive();
    // Whether the client implements sendAndReceive
  
  virtual bool sendAndReceive(const std::string& data, std::string& response, long timeout);
    // Implementation for sendAndReceive
  
  virtual int getPollfd();
    // Return the file descriptor if the transport is pollable
  
  virtual void close();
    // Close the connection
  
  virtual void signal_close();
    // Signal closure but let the local implementation trigger closure 
  
  virtual bool isOpen();
    // Returns true if the transport is currently connected to the server
  
  virtual bool getRemoteAddress(IPAddress& address);
    // Return the remote address of the server if transport is a networtk transport
  
  virtual bool getLocalAddress(IPAddress& address);
    // Return the local address of the server if transport is a networtk transport
  
private:
  Socket* _pSocket;
  bool _isOpen;
  IPAddress _localAddress;
  IPAddress _remoteAddress;
  RequestQueue _eventQueue;
};

//
// Inlines
//
 inline bool ZMQSocketClient::implementsSendAndReceive()
 {
   return true;
 }

} } // OSS::Net
#endif // ENABLE_FEATURE_ZMQ
#endif // OSS_ZMQSOCKETCLIENT_PP_H

