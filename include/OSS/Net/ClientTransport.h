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

#ifndef OSS_NET_CLIENTTRANSPORT_H_INCLUDED
#define OSS_NET_CLIENTTRANSPORT_H_INCLUDED


#include "OSS/Net/IPAddress.h"


namespace OSS {
namespace Net {
  

class ClientTransport
{
public:
  
  enum EventType
  {
    EventMessage,
    EventOpen,
    EventClose,
    EventFail,
    EventPing,
    EventPong,
    EventPongTimeout,
    EventUnknown
  };
  
  struct EventData
  {
    EventType event;
    std::string data;
  };
  
  ClientTransport();
  virtual ~ClientTransport();
  
  virtual bool connect(const std::string& url) = 0;
    // Create a connection to the remote server
  
  virtual bool send(const std::string& data) = 0;
    // Send data to the server
  
  virtual void receive(EventData& event) = 0;
    // Receive an event from the server or local transport
  
  virtual bool receive(EventData& event, long timeout) = 0;
    // Receive an event from the server or local transport with timeout
  
  virtual int getPollfd() = 0;
    // Return the file descriptor if the transport is pollable
  
  virtual void close() = 0;
    // Close the connection
  
  virtual void signal_close() = 0;
    // Signal closure but let the local implementation trigger closure 
  
  virtual bool isOpen() = 0;
    // Returns true if the transport is currently connected to the server
  
  virtual bool getRemoteAddress(IPAddress& address) = 0;
    // Return the remote address of the server if transport is a networtk transport
  
  virtual bool getLocalAddress(IPAddress& address) = 0;
    // Return the local address of the server if transport is a networtk transport
};

//
// Inlines
//

inline ClientTransport::ClientTransport()
{
}

inline ClientTransport::~ClientTransport()
{
}

} } // OSS::Net

#endif // OSS_NET_CLIENTTRANSPORT_H_INCLUDED

