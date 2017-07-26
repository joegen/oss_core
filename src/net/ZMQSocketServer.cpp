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


#include <boost/functional.hpp>

#include "OSS/Net/ZMQSocketServer.h"
#include "OSS/Net/ZMQSocketClient.h"
#include "OSS/UTL/Logger.h"

const unsigned int SOCKET_READ_TIMEOUT = 500;

namespace OSS {
namespace Net {


ZMQSocketServer::ZMQSocketServer() :
  _socket(Socket::REP),
  _pThread(0),
  _isTerminated(false)
{
  _closeString = "~~close-" + OSS::string_from_number<intptr_t>((intptr_t)this);
}

ZMQSocketServer::~ZMQSocketServer()
{
  close();
}

bool ZMQSocketServer::listen(const std::string& bindAddress)
{
  if (_pThread || !_socket.bind(bindAddress))
  {
    return false;
  }
  
  _bindAddress = bindAddress;
  _pThread = new boost::thread(boost::bind(&ZMQSocketServer::internal_read_messages, this));
  return true;
}

void ZMQSocketServer::internal_read_messages()
{
  _isTerminated = false;
  while(!_isTerminated)
  {
    std::string message;
    if (_socket.receiveRequest(message, SOCKET_READ_TIMEOUT))
    {
      if (message == _closeString)
      {
        break;
      }
      onMessage(0, message);
    }
  }
  _socket.close();
  _isTerminated = true;
  OSS_LOG_DEBUG("ZMQSocketServer thread TERMINATED");
}

void ZMQSocketServer::close()
{
  if (_isTerminated || !_pThread)
  {
    return;
  }
  
  OSS_LOG_DEBUG("ZMQSocketServer::close()");
  
  ZMQSocketClient client;
  if (!client.connect(_bindAddress))
  {
    return;
  }
  client.send(_closeString);

  if (_pThread)
  {
    _pThread->join();
    delete _pThread;
    _pThread = 0;
  }
}

bool ZMQSocketServer::sendMessage(int connectionId, const std::string& msg)
{
  return _socket.sendReply(msg);
}

void ZMQSocketServer::onOpen(int connectionId)
{
}

void ZMQSocketServer::onClose(int connectionId)
{
}

void ZMQSocketServer::onError(int connectionId, const std::string& error)
{
}

void ZMQSocketServer::onMessage(int connectionId, const std::string& message)
{
}

void  ZMQSocketServer::onPing(int connectionId)
{
}

void  ZMQSocketServer::onPong(int connectionId)
{
}

void  ZMQSocketServer::onPongTimeout(int connectionId)
{
}


} } // OSS::NET



