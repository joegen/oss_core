
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

#include "OSS/BSON/BSONQueue.h"

#if ENABLE_FEATURE_ZMQ

namespace OSS {
namespace BSON {
 
  
using OSS::ZMQ::ZMQSocket; 

    
BSONQueue::BSONQueue(Role role, const std::string& name) :
  _role(role),
  _name(name),
  _pSocket(0)
{
  _address = "inproc://";
  _address += _name;
  initSocket();
}
    
BSONQueue::~BSONQueue()
{
  delete _pSocket;
}

bool BSONQueue::enqueue(BSONObject& msg)
{
  if (_role != PRODUCER)
  {
    return false;
  }
    
  if (msg.getDataLength() == 0)
  {
    return false;
  }
  
  std::string data((const char*)msg.getData(), msg.getDataLength());
  return _pSocket->sendRequest("BSONQueue::enqueue", data);
}

bool BSONQueue::dequeue(BSONObject& msg)
{
  if (_role != CONSUMER)
  {
    return false;
  }
  
  std::string cmd;
  std::string bson;
  if (!_pSocket->receiveRequest(cmd, bson))
  {
    return false;
  }
  
  if (cmd != "BSONQueue::enqueue" || bson.empty())
  {
    return false;
  }
  
  msg.reset((uint8_t*)bson.data(), bson.size());
  return true;
}

bool BSONQueue::enqueue(BSONDocument& msg)
{
  BSONObject bson;
  msg.toBSON(bson);
  return enqueue(bson);
}

bool BSONQueue::dequeue(BSONDocument& msg)
{
  BSONObject bson;
  if (!dequeue(bson))
  {
    return false;
  }
  msg.fromBSON(bson);
  return true;
}

bool BSONQueue::initSocket()
{
  if (_pSocket)
  {
    return true;
  }
  if (_role == PRODUCER)
  {
    _pSocket = new ZMQSocket(ZMQSocket::PUSH);
    if (_pSocket->connect(_address))
    {
      return true;
    }
  }
  else
  {
    _pSocket = new ZMQSocket(ZMQSocket::PULL);
    if (_pSocket->bind(_address))
    {
      return true;
    }
  }
 
  delete _pSocket;
  _pSocket = 0;
  return false;
}

} } // OSS::BSON

#endif // ENABLE_FEATURE_ZMQ

