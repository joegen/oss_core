
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


#include "OSS/SIP/SBC/SBCAuxiliarySocket.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {
namespace SBC {


#define DEFAULT_AUXILIARY_SOCKET_READ_TIMEOUT 5000
  
SBCAuxiliarySocket::SBCAuxiliarySocket() :
  _socket(OSS::ZMQ::ZMQSocket::REP),
  _publisher(OSS::ZMQ::ZMQSocket::PUB)
{
}
  
SBCAuxiliarySocket::~SBCAuxiliarySocket()
{
}

bool SBCAuxiliarySocket::run(const Properties& properties)
{
  _properties = properties;
  
  if (!_properties.bindAddress.isValid())
  {
    return false;
  }
  
  std::ostringstream bindAddress;
  bindAddress << "tcp://" << _properties.bindAddress.toIpPortString();
  if (!_socket.bind(bindAddress.str()))
  {
    OSS_LOG_ERROR("Auxiliary sender socket failed to bind to " << bindAddress.str());
    return false;
  }
  
  std::ostringstream publisherAddress;
  publisherAddress << "tcp://" << _properties.publisherAddress.toIpPortString();
  if (!_publisher.bind(publisherAddress.str()))
  {
    OSS_LOG_ERROR("Auxiliary publisher socket failed to bind to " << publisherAddress.str());
    return false;
  }
  
  dynamic_cast<OSS::Thread*>(this)->run();
  
  return true;
}

void SBCAuxiliarySocket::onHouseKeeping()
{
  if (!_terminateFlag)
  {
    return;
  }
}

void SBCAuxiliarySocket::onHandleRequest(const std::string& cmd, const std::string& args)
{
  if (!_terminateFlag)
  {    
    HandlerMap::iterator iter = _handlers.find(cmd);
    if (iter == _handlers.end())
    {
      _socket.sendReply("Invalid command");
      return;
    }
    
    std::string result;
    iter->second(args, result);
    _socket.sendReply(result);
    
    return;
  }
}

void SBCAuxiliarySocket::main()
{
  while (!_terminateFlag)
  {
    std::string cmd;
    std::string args;
    if (_socket.receiveRequest(cmd, args, DEFAULT_AUXILIARY_SOCKET_READ_TIMEOUT))
    {
      if (!_exitString.empty() && _exitString == args)
      {
        _socket.sendReply(args);
        OSS::thread_sleep(100);
        break;
      }
      onHandleRequest(cmd, args);
    }
    else
    {
      onHouseKeeping();
    }
  }
}

void SBCAuxiliarySocket::publish(const std::string& message)
{
  OSS::mutex_critic_sec_lock lock(_publisherMutex);
  _publisher.publish(message);
}

void SBCAuxiliarySocket::terminate()
{
  _socket.close();
  _terminateFlag = true;
}


} } } // OSS::SIP::SBC





