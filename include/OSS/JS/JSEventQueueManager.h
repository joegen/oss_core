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

#ifndef OSS_JSEVENTQUEUEMANAGER_H_INCLUDED
#define OSS_JSEVENTQUEUEMANAGER_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_V8


#include "OSS/JS/JS.h"
#include "OSS/JS/modules/QueueObject.h"
#include "OSS/JS/JSEventLoopComponent.h"


namespace OSS {
namespace JS {

class JSEventLoop;
  
class JSEventQueueManager : public JSEventLoopComponent
{
public:
  typedef std::map<int, QueueObject*> QueueObjectMap;
  typedef QueueObject::Event::Ptr EventPtr;
  typedef std::vector<pollfd> Descriptors;

  JSEventQueueManager(JSEventLoop* pEventLoop);
  ~JSEventQueueManager();
  
  void addQueue(QueueObject* pQueue);
  void removeQueue(QueueObject* pQueue);
  QueueObject* findQueue(int fd);
  bool enqueue(int fd, const EventPtr& pEvent);
  bool dequeue(int fd);
  std::size_t getSize();
  void appendDescriptors(Descriptors& descriptors);
  
private:
  OSS::mutex _queuesMutex;
  QueueObjectMap _queues;
};

inline std::size_t JSEventQueueManager::getSize()
{
  return _queues.size();
}


} }

#endif // ENABLE_FEATURE_V8
#endif // OSS_JSEVENTQUEUEMANAGER_H_INCLUDED

