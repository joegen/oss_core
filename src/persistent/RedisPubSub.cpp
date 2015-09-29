
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


#include <event.h>
#include "OSS/Persistent/RedisPubSub.h"


namespace OSS {
namespace Persistent {

static void on_channel_message(redisAsyncContext *c, void *reply, void *privdata)
{
  redisReply* r = (redisReply*)reply;
  if (!reply)
  {
    return;
  }

  if (r->type == REDIS_REPLY_ARRAY)
  {
    RedisPubSub::Event eventData;
    std::ostringstream response;
    response << "| ";
    for (std::size_t j = 0; j < r->elements; j++)
    {
      if (r->element[j]->type == REDIS_REPLY_STRING)
      {
        eventData.push_back(r->element[j]->str);
        response << r->element[j]->str << " | ";
      }
    }
    OSS_LOG_DEBUG("[REDIS] RedisPubSub::on_channel_message - " << response.str());
    
    RedisPubSub* pPubSub = (RedisPubSub*)privdata;
    if (pPubSub)
    {
      if (!eventData.empty() && eventData.front() != "subscribe")
      {
        pPubSub->post(eventData);
      }
    }
  }
  
  
  //
  // Note: Redis frees the reply object so do not free it here!
  //
}

void exit_func(int fd, short flags, void* privdata) 
{
  RedisPubSub* pPubSub = (RedisPubSub*)privdata;
  if (pPubSub)
  {
    event_base_loopbreak(pPubSub->getEventBase());
  }
}
  
RedisPubSub::RedisPubSub() :
  _context(0),
  _pEventBase(0),
  _pEventThread(0),
  _exiting(false),
  _pReconnectThread(0)
{
}
  
RedisPubSub::~RedisPubSub()
{
  _exiting = true;
  
  if (_pReconnectThread)
  {
    _pReconnectThread->join();
    delete _pReconnectThread;
    _pReconnectThread = 0;
  }
  
  disconnect();
}
  
bool RedisPubSub::connect(const std::string& host, int port, const std::string& password)
{
  _host = host;
  _port = port;
  _password = password;
  
  disconnect();
  
  OSS::mutex_critic_sec_lock lock(_mutex);
  _context = redisAsyncConnect(host.c_str(), port);
  if (_context->err) 
  {
    OSS_LOG_ERROR("[REDIS] RedisPubSub::connect Error: " << _context);
    disconnect();
    return false;
  }
  
  _pEventBase = event_base_new();
  redisLibeventAttach(_context, _pEventBase);

  if (!password.empty())
  {
    redisAsyncCommand(_context, 0, 0, "AUTH %s", password.c_str());
  }
   
  return true;
}

void RedisPubSub::disconnect()
{
  OSS::mutex_critic_sec_lock lock(_mutex); 

  if (_pEventBase)
  {
    event_base_once(_pEventBase, 0, EV_WRITE, exit_func, this, 0);
  }

  if (_pEventThread)
  {
    _pEventThread->join();
    delete _pEventThread;
    _pEventThread = 0;
  }

  if (_context)
  {
    _context->c.flags |= REDIS_CONNECTED;
    redisAsyncFree(_context);
    _context = 0;
  }

  if (_pEventBase)
  {
    event_base_free(_pEventBase);
    _pEventBase = 0;
  }

  
}

void RedisPubSub::eventLoop()
{
  event_base_dispatch(_pEventBase);
  
  Event eventData;
  eventData.push_back("event");
  eventData.push_back("RedisPubSub");
  eventData.push_back("terminated");
  post(eventData);
  
  if (!_exiting && _pReconnectThread)
  {
    _pReconnectThread->join();
    delete _pReconnectThread;
    _pReconnectThread = 0;
  }
  
  if (!_host.empty() && !_channelName.empty())
  {
    _pReconnectThread = new boost::thread(boost::bind(&RedisPubSub::reconnect, this));
  }
}

void RedisPubSub::reconnect()
{
  std::cout << "RECONNECTING" << std::endl;
  OSS::thread_sleep(1000);
  if (!_host.empty() && connect(_host, _port, _password))
  {
    if (!_channelName.empty())
    {
      subscribe(_channelName);
    }
  }
}

bool RedisPubSub::subscribe(const std::string& channelName)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  if (!_context)
  {
    return false;
  }
  _channelName = channelName;
  std::ostringstream cmd;
  cmd << "SUBSCRIBE " << channelName; 
  bool ret = redisAsyncCommand(_context, on_channel_message, this, cmd.str().c_str()) == 0;
  
  if (ret && !_pEventThread)
  {
    _pEventThread = new boost::thread(boost::bind(&RedisPubSub::eventLoop, this));
  }
  
  return ret;
}


void RedisPubSub::receive(Event& event)
{
  _eventQueue.dequeue(event);
}
  
void RedisPubSub::post(const Event& event)
{
  _eventQueue.enqueue(event);
}

} } // OSS::Presistent  


