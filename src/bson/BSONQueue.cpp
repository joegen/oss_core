
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

const int BSONQueue::REQ = ZMQSocket::REQ;
const int BSONQueue::REP = ZMQSocket::REP;
const int BSONQueue::PUSH = ZMQSocket::PUSH;
const int BSONQueue::PULL = ZMQSocket::PULL;
    
BSONQueue::BSONQueue(int role, const std::string& name) :
  _name(name),
  _pSocket(0)
{
  _role = (Role)role,
  _address = "inproc://";
  _address += _name;
}
    
BSONQueue::~BSONQueue()
{
  delete _pSocket;
}

bool BSONQueue::enqueue(BSONObject& msg)
{
  if (!initSocket())
  {
    return false;
  }
  
  if (_role == PULL)
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
  if (!initSocket())
  {
    return false;
  }
  
  return false;
}

bool BSONQueue::initSocket()
{
  if (_pSocket)
  {
    return true;
  }
  _pSocket = new ZMQSocket(_role);
  
  if ((_role == REQ || _role == PUSH) && _pSocket->connect(_address))
  {
    return true;
  }
  else if ((_role == REP || _role == PULL) && _pSocket->bind(_address))
  {
     return true;
  }
  
  delete _pSocket;
  _pSocket = 0;
  
  return false;
}

} } // OSS::BSON

#endif // ENABLE_FEATURE_ZMQ

