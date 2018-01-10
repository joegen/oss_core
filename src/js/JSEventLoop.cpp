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
#include "OSS/JS/JSIsolate.h"

namespace OSS {
namespace JS {


JSEventLoop::JSEventLoop(JSIsolate* pIsolate) :
  _isTerminated(false),
  _pIsolate(pIsolate),
  _fdManager(this),
  _queueManager(this),
  _eventEmitter(this),
  _taskManager(this),
  _functionCallback(this),
  _timerManager(this),
  _interIsolate(this),
  _garbageCollectionFrequency(30)
{
}

JSEventLoop::~JSEventLoop()
{
}

JSIsolate* JSEventLoop::getIsolate()
{
  return _pIsolate;
}

void JSEventLoop::processEvents()
{
  OSS::UInt64 lastGarbageCollectionTime = 0;
  
  OSS::JS::JSIsolate::Ptr pIsolate = OSS::JS::JSIsolate::getIsolate();

  std::vector<pollfd> descriptors;
  bool reconstructFdSet = true;
  
  //
  // Static Descriptors
  //
  pollfd pfds[3];
  pfds[0].fd = this->getFd();
  pfds[0].events = POLLIN;
  pfds[0].revents = 0;
  pfds[1].fd = _functionCallback.getFd();
  pfds[1].events = POLLIN;
  pfds[1].revents = 0;
  pfds[2].fd = _timerManager.getFd();
  pfds[2].events = POLLIN;
  pfds[2].revents = 0;
  while (!_isTerminated)
  {
    if (reconstructFdSet)
    {
      descriptors.clear();
      descriptors.reserve(_queueManager.getSize() + 3);
      descriptors.push_back(pfds[0]);
      descriptors.push_back(pfds[1]);
      descriptors.push_back(pfds[2]);
      
      _queueManager.appendDescriptors(descriptors);
      _fdManager.appendDescriptors(descriptors);
    }
    int ret = ::poll(descriptors.data(), descriptors.size(), 100);
    v8::HandleScope scope;
    if (ret == -1 || _isTerminated)
    {
      break;
    }
    else if (ret == 0)
    {
      //
      // Use the timeout to let V8 perform internal garbage collection tasks
      //
      if (_pIsolate->isRoot())
      {
        OSS::UInt64 now = OSS::getTime();
        if (now - lastGarbageCollectionTime > _garbageCollectionFrequency * 1000)
        {
          v8::V8::LowMemoryNotification();
          lastGarbageCollectionTime = now;
        }
        while(!v8::V8::IdleNotification());
        continue;
      }
    }
    
    //
    // Perform garbage collection every 30 seconds
    //
    if (_pIsolate->isRoot())
    {
      OSS::UInt64 now = OSS::getTime();
      if (now - lastGarbageCollectionTime > _garbageCollectionFrequency * 1000)
      {
        v8::V8::LowMemoryNotification();
        lastGarbageCollectionTime = now;
      }
    }
    
    //
    // Check if C++ just wants to execute a task in the event loop
    //
    if (_taskManager.doOneWork() || _interIsolate.doOneWork())
    {
      //
      // Do not reconstruct fdset if we did some tasks
      //
      reconstructFdSet = false;
      continue;
    }
    
    //
    // From this point onwards, we only reconstruct the FD set if we are waken up via the wakeup pipe
    //
    reconstructFdSet = false;
    if (descriptors[0].revents & POLLIN)
    {
      this->clearOne();
      if (_isTerminated)
      {
        break;
      }
      else
      {
        reconstructFdSet = true;
        continue;
      }
    }
    else if (descriptors[1].revents & POLLIN)
    {
      _functionCallback.doOneWork();
    }
    else if (descriptors[2].revents & POLLIN)
    {
      _timerManager.doOneWork();
    }
    else
    {
      bool found = false;
      std::size_t fdsize = descriptors.size();

      for (std::size_t i = 3; i < fdsize; i++)
      {
        pollfd& pfd = descriptors[i];
        int revents = pfd.revents;
        if (revents & POLLIN)
        {

          found = _queueManager.dequeue(pfd.fd);
          if (!found)
          {
            found = _fdManager.signalIO(pfd);
          }
        }
        
        if (found)
        {
          break;
        }
      }
    }
  }
}

void JSEventLoop::terminate()
{
  _isTerminated = true;
  wakeup();
}

} } 



