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



#include "OSS/ZMQ/ZMQSocket.h"
#include "OSS/UTL/Logger.h"


#if OSS_HAVE_ZMQ

#include "OSS/ZMQ/zmq.hpp"

namespace OSS {
namespace ZMQ {
  
static void zeromq_free (void *data, void *hint)
{
  free (data);
}
//  Convert string to 0MQ string and send to socket
static bool zeromq_send (zmq::socket_t & socket, const std::string & data)
{
  char * buff = (char*)malloc(data.size());
  memcpy(buff, data.c_str(), data.size());
  zmq::message_t message((void*)buff, data.size(), zeromq_free, 0);
  bool rc = socket.send(message);
  return (rc);
}

//  Sends string as 0MQ string, as multipart non-terminal
static bool zeromq_sendmore (zmq::socket_t & socket, const std::string & data)
{
  char * buff = (char*)malloc(data.size());
  memcpy(buff, data.c_str(), data.size());
  zmq::message_t message((void*)buff, data.size(), zeromq_free, 0);
  bool rc = socket.send(message, ZMQ_SNDMORE);
  return (rc);
}

static void zeromq_receive (zmq::socket_t& socket, std::string& value)
{
  zmq::message_t message;
  socket.recv(&message);
  if (!message.size())
    return;
  value = std::string(static_cast<char*>(message.data()), message.size());
} 

static zmq::context_t* zeromq_context(OSS_HANDLE context)
{
  return reinterpret_cast<zmq::context_t*>(context);
}

static zmq::socket_t* zeromq_socket(OSS_HANDLE sock)
{
  return reinterpret_cast<zmq::socket_t*>(sock);
}

static zmq::socket_t* zeromq_create_socket(OSS_HANDLE context, int type)
{
  zmq::socket_t* socket = new zmq::socket_t(*zeromq_context(context), type);
  int linger = 0;
  socket->setsockopt (ZMQ_LINGER, &linger, sizeof (linger));
  return socket;
}

static bool zeromq_poll_read(zmq::socket_t* sock, int timeoutms)
{
  int timeoutnano = timeoutms * 1000; // convert to nanoseconds
  zmq::pollitem_t items[] = { { *sock, 0, ZMQ_POLLIN, 0 } };
  int rc = zmq::poll (&items[0], 1, timeoutnano);
  switch (rc)
  {
    case ETERM:
      OSS_LOG_ERROR("zeromq_poll_read - At least one of the members of the items array refers to a socket whose associated ØMQ context was terminated.");
      return false;
    case EFAULT:
      OSS_LOG_ERROR("zeromq_poll_read - The provided items was not valid (NULL).");
      return false;
    case EINTR:
      OSS_LOG_ERROR("zeromq_poll_read - The operation was interrupted by delivery of a signal before any events were available.");
      return false;
  }
  
  return items[0].revents & ZMQ_POLLIN;
}
  
ZMQSocket::ZMQSocket(SocketType type) :
  _type(type),
  _context(0),
  _socket(0),
  _canReconnect(false)
{
  _context = new zmq::context_t(1);
}
  
ZMQSocket::~ZMQSocket()
{
  close();
  delete zeromq_context(_context);
}

bool ZMQSocket::bind(const std::string& bindAddress)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  
  if ( _socket || !_context)
  {
    return false;
  }
  
  switch(_type)
  {
  case REP:
     _socket = zeromq_create_socket(_context, ZMQ_REP);
    break;
  case REQ:
    return false;
  }
  
  if (!_socket)
  {
    return false;
  }
  
  try
  {
    zeromq_socket(_socket)->bind(bindAddress.c_str());
  }
  catch(zmq::error_t& error_)
  {
    OSS_LOG_ERROR("ZMQSocket::bind() - ZMQ Exception:  " << error_.what());
    internal_close();
    return false;
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR("ZMQSocket::bind() - ZMQ Exception: " << e.what());
    internal_close();
    return false;
  }
  catch(...)
  {
    OSS_LOG_ERROR("ZMQSocket::bind() - Exception: ZMQ Unknown Exception");
    internal_close();
    return false;
  }

  _bindAddress = bindAddress;
  
  return true;
}

bool ZMQSocket::connect(const std::string& peerAddress)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  return internal_connect(peerAddress);
}

bool ZMQSocket::internal_connect(const std::string& peerAddress)
{
  if (_socket)
  {
    return false;
  }
  
  switch(_type)
  {
  case REQ:
     _socket = zeromq_create_socket(_context, ZMQ_REQ);
    break;
  case REP:
    return false;
  }
  
  if (!_socket)
  {
    return false;
  }
  
  try
  {
    zeromq_socket(_socket)->connect(peerAddress.c_str());
  }
  catch(zmq::error_t& error_)
  {
    OSS_LOG_ERROR("ZMQSocket::internal_connect() - ZMQ Exception:  " << error_.what());
    internal_close();
    return false;
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR("ZMQSocket::internal_connect() - ZMQ Exception: " << e.what());
    internal_close();
    return false;
  }
  catch(...)
  {
    OSS_LOG_ERROR("ZMQSocket::internal_connect() - Exception: ZMQ Unknown Exception");
    internal_close();
    return false;
  }
  _peerAddress = peerAddress;
  return true;
}

void ZMQSocket::close()
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  _canReconnect = false;
  internal_close();
}

void ZMQSocket::internal_close()
{
  delete zeromq_socket(_socket);
  _socket = 0;
  
  if (!_canReconnect)
  {
    _peerAddress = "";
  }
}

bool ZMQSocket::sendAndReceive(const std::string& cmd, const std::string& data, std::string& response, unsigned int timeoutms)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  
  if (!internal_send_request(cmd, data))
  {
    return false;
  }
  
  return internal_receive_reply(response, timeoutms);
}

bool ZMQSocket::sendRequest(const std::string& cmd, const std::string& data)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  return internal_send_request(cmd, data);
}

bool ZMQSocket::internal_send_request(const std::string& cmd, const std::string& data)
{  
  //
  // reconnect the socket 
  //
  if (!_socket && _peerAddress.empty())
  {
    return false;
  }
  else if (!_socket && _canReconnect && !internal_connect(_peerAddress))
  {
    return false;
  }
  
  if (!zeromq_sendmore(*zeromq_socket(_socket), cmd))
  {
    OSS_LOG_ERROR("ZMQSocket::send() - Exception: zeromq_sendmore(cmd) failed");
    _canReconnect = true;
    internal_close();    
    return false;
  }
  
  if (!zeromq_send(*zeromq_socket(_socket), data))
  {
    OSS_LOG_ERROR("ZMQSocket::send() - Exception: zeromq_send(data) failed");
    _canReconnect = true;
    internal_close();
    return false;
  }
  
  return true;
}

bool ZMQSocket::sendReply(const std::string& data)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  return internal_send_reply(data);
}


bool ZMQSocket::internal_send_reply(const std::string& data)
{  
  if (!_socket || !zeromq_send(*zeromq_socket(_socket), data))
  {
    OSS_LOG_ERROR("ZMQSocket::send() - Exception: zeromq_send(data) failed");
    return false;
  }
  return true;
}

bool ZMQSocket::receiveReply(std::string& data, unsigned int timeoutms)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  return internal_receive_reply(data, timeoutms);
}

 
bool ZMQSocket::internal_receive_reply(std::string& response, unsigned int timeoutms)
{
  if (!_socket)
  {
    return false;
  }
  
  if (!zeromq_poll_read(zeromq_socket(_socket), timeoutms))
  {
    OSS_LOG_ERROR("ZMQSocket::internal_receive() - Exception: zeromq_poll_read() failed");
    _canReconnect = true;
    internal_close();
    return false;
  }
  
  zeromq_receive(*zeromq_socket(_socket), response);  
  return true;
}

bool ZMQSocket::receiveRequest(std::string& cmd, std::string& data, unsigned int timeoutms)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  return internal_receive_request(cmd, data, timeoutms);
}

bool ZMQSocket::internal_receive_request(std::string& cmd, std::string& data, unsigned int timeoutms)
{
  if (!_socket)
  {
    return false;
  }
  
  if (!zeromq_poll_read(zeromq_socket(_socket), timeoutms))
  {
    OSS_LOG_ERROR("ZMQSocket::internal_receive() - Exception: zeromq_poll_read() failed");
    return false;
  }
  
  zeromq_receive(*zeromq_socket(_socket), cmd); 
  zeromq_receive(*zeromq_socket(_socket), data);  
  return true;
}


} } // OSS::ZMQ

#endif // OSS_HAVE_ZMQ
