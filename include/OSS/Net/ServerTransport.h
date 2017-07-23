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

#ifndef OSS_NET_SERVERTRANSPORT_H_INCLUDED
#define OSS_NET_SERVERTRANSPORT_H_INCLUDED


#include "OSS/OSS.h"


namespace OSS {
namespace Net {
  

class ServerTransport
{
public:
  ServerTransport();
  virtual ~ServerTransport();
  
  virtual bool listen(const std::string& bindAddress) = 0;
  
  virtual void close() = 0;
  
  //
  // Connection callbacks
  //
  
  virtual void onOpen(int connectionId) = 0;
    /// A new connection is created
  
  virtual void onClose(int connectionId) = 0;
    /// The connection was closed 
  
  virtual void onError(int connectionId, const std::string& error) = 0;
    /// Conneciton encountered an error
  
  virtual void onMessage(int connectionId, const std::string& message) = 0;
    /// Connection receveid a new message
  
  virtual void onPing(int connectionId) = 0;
    /// Connection received a ping (if protocol supports keep-alive)
  
  virtual void onPong(int connectionId) = 0;
    /// Connection received a pong (if protocol supports keep-alive)
  
  virtual void onPongTimeout(int connectionId) = 0;
    /// Pong not received on time (if protocol supports keep-alive)
  
  virtual bool sendMessage(int connectionId, const std::string& msg) = 0;
    /// Send a message using the connection identified by connectionId
};

//
// Inlines
//

inline ServerTransport::ServerTransport()
{
}

inline ServerTransport::~ServerTransport()
{
}

} } // OSS::Net

#endif // OSS_NET_SERVERTRANSPORT_H_INCLUDED

