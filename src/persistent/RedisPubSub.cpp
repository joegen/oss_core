
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


#include "OSS/Persistent/RedisPubSub.h"


namespace OSS {
namespace Persistent {

static void on_channel_message(redisAsyncContext *c, void *reply, void *privdata)
{
  redisReply* r = (redisReply*)reply;
  if (reply == NULL) return;

  if (r->type == REDIS_REPLY_ARRAY)
  {
    RedisPubSub::Event eventData;
    std::ostringstream response;
    response << "| ";
    for (std::size_t j = 0; j < r->elements; j++)
    {
        eventData.push_back(r->element[j]->str);
        response << r->element[j]->str << " | ";
    }
    OSS_LOG_DEBUG("[REDIS] RedisPubSub::on_channel_message - " << response.str());
    
    RedisPubSub* pPubSub = (RedisPubSub*)privdata;
    if (pPubSub)
    {
      pPubSub->post(eventData);
    }
  }
}

static void on_publish_response(redisAsyncContext *c, void *reply, void *privdata)
{
  redisReply* r = (redisReply*)reply;
  if (reply == NULL) return;

  if (r->type == REDIS_REPLY_ARRAY)
  {
    std::ostringstream response;
    response << "| ";
    for (std::size_t j = 0; j < r->elements; j++)
    {
      response << r->element[j]->str << " | ";
    }
    OSS_LOG_DEBUG("[REDIS] RedisPubSub::on_publish_response - " << response.str());
  }
}

RedisPubSub::RedisPubSub() :
  _context(0),
  _pEventBase(0),
  _pEventThread(0)
{
}
  
RedisPubSub::~RedisPubSub()
{
  disconnect();
}
  
bool RedisPubSub::connect(const std::string& host, int port, const std::string& password)
{
  disconnect();
  
  OSS::mutex_critic_sec_lock lock(_mutex);
  _context = redisAsyncConnect(host.c_str(), port);
  if (_context->err) 
  {
    OSS_LOG_ERROR("[REDIS] RedisPubSub::connect Error: " << _context);
    disconnect();
    return false;
  }
  
  //
  // Do we need to delete the old event base or is it owned by the context????
  //
  _pEventBase = event_base_new();
  redisLibeventAttach(_context, _pEventBase);

  if (!password.empty())
  {
    redisAsyncCommand(_context, 0, 0, "AUTH %s", password.c_str());
  }
  
  _pEventThread = new boost::thread(boost::bind(&RedisPubSub::eventLoop, this));
  return true;
}

void RedisPubSub::disconnect()
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  if (_context)
  {
    redisAsyncFree(_context);
    _context = 0;
  }
  
  if (_pEventThread)
  {
    _pEventThread->join();
    delete _pEventThread;
    _pEventThread = 0;
  }
}

void RedisPubSub::eventLoop()
{
  event_base_dispatch(_pEventBase);
}

bool RedisPubSub::subscribe(const std::string& channelName)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  if (!_context)
  {
    return false;
  }
  std::ostringstream cmd;
  cmd << "SUBSCRIBE " << channelName; 
  redisAsyncCommand(_context, on_channel_message, this, cmd.str().c_str());
  return true;
}

bool RedisPubSub::publish(const std::string& channelName, const std::string& event)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  if (!_context)
  {
    return false;
  }
  std::ostringstream cmd;
  cmd << "PUBLISH " << event; 
  redisAsyncCommand(_context, on_publish_response, this, cmd.str().c_str());
  return true;
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


