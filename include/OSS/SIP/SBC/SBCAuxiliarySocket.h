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

#ifndef SBCAUXILIARYSOCKET_H_INCLUDED
#define	SBCAUXILIARYSOCKET_H_INCLUDED

#include "OSS/ZMQ/ZMQSocket.h"
#include "OSS/UTL/Thread.h"
#include "OSS/Net/IPAddress.h"

namespace OSS {
namespace SIP {
namespace SBC {


class SBCAuxiliarySocket : protected OSS::Thread
{
public:
  
  typedef boost::function<void(const std::string&, std::string&)> Handler;
  typedef std::map<std::string, Handler> HandlerMap;
  
  struct Properties
  {
    OSS::Net::IPAddress bindAddress;
    OSS::Net::IPAddress publisherAddress;
    std::string secret;
  };
  
  SBCAuxiliarySocket();
  
  ~SBCAuxiliarySocket();

  bool run(const Properties& properties);
  
  void terminate();
  
  void addHandler(const std::string& cmd, const Handler& handler);

  void publish(const std::string& message);
  
  void setExitString(const std::string& exitString);
  
  const std::string& getExitString() const;
protected:
  virtual void main();
  
  void onHouseKeeping();
  
  void onHandleRequest(const std::string& cmd, const std::string& args);
private:
  OSS::ZMQ::ZMQSocket _socket;
  OSS::ZMQ::ZMQSocket _publisher;
  Properties _properties;
  HandlerMap _handlers;
  OSS::mutex_critic_sec _publisherMutex;
  std::string _exitString;
};


//
// Inlines
//

inline void SBCAuxiliarySocket::addHandler(const std::string& cmd, const Handler& handler)
{
  _handlers[cmd] = handler;
}

inline void SBCAuxiliarySocket::setExitString(const std::string& exitString)
{
  _exitString = exitString;
}

inline const std::string& SBCAuxiliarySocket::getExitString() const
{
  return _exitString;
}

} } } // OSS::SIP::SBC



#endif	/* SBCAUXILIARYSOCKET_H */

