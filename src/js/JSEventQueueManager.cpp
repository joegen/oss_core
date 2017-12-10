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



#include "OSS/JS/JSEventLoop.h"
#include "OSS/JS/JSPlugin.h"

namespace OSS {
namespace JS {


JSEventQueueManager::JSEventQueueManager(JSEventLoop* pEventLoop) :
  _pEventLoop(pEventLoop)
{
}

JSEventQueueManager::~JSEventQueueManager()
{
}

void JSEventQueueManager::addQueue(QueueObject* pQueue)
{
  OSS::mutex_critic_sec_lock lock(_queuesMutex);
  _queues[pQueue->_queue.getFd()] = pQueue;
}

void JSEventQueueManager::removeQueue(QueueObject* pQueue)
{
  OSS::mutex_critic_sec_lock lock(_queuesMutex);
  _queues.erase(pQueue->_queue.getFd());
}

QueueObject* JSEventQueueManager::findQueue(int fd)
{
  OSS::mutex_critic_sec_lock lock(_queuesMutex);
  QueueObjectMap::iterator iter = _queues.find(fd);
  if (iter != _queues.end())
  {
    return iter->second;
  }
  return 0;
}

bool JSEventQueueManager::enqueue(int fd, const EventPtr& pEvent)
{
  OSS::mutex_critic_sec_lock lock(_queuesMutex);
  QueueObjectMap::iterator iter = _queues.find(fd);
  if (iter != _queues.end())
  {
    iter->second->_queue.enqueue(pEvent);
    return true;
  }
  return false;
}

bool JSEventQueueManager::dequeue(int fd)
{
  OSS::mutex_critic_sec_lock lock(_queuesMutex);
  QueueObjectMap::iterator iter = _queues.find(fd);
  if (iter != _queues.end())
  {
    EventPtr pEvent;
    iter->second->_queue.dequeue(pEvent);
    if (pEvent)
    {
      iter->second->_eventCallback->Call(js_get_global(), pEvent->_eventData.size(), pEvent->_eventData.data());
      return true;
    }
    return false;
  }
  return false;
}

void JSEventQueueManager::appendDescriptors(Descriptors& descriptors)
{
  OSS::mutex_critic_sec_lock lock(_queuesMutex);
  for(QueueObjectMap::iterator iter = _queues.begin(); iter != _queues.end(); iter++)
  {
    pollfd fd;
    fd.fd = iter->first;
    fd.events = POLLIN;
    fd.revents = 0;
    descriptors.push_back(fd);
  }
}



} }



