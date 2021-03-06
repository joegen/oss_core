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
#include "OSS/JS/JSTaskManager.h"



namespace OSS {
namespace JS {


JSTaskManager::JSTaskManager(JSEventLoop* pEventLoop) :
  JSEventLoopComponent(pEventLoop)
{
}

JSTaskManager::~JSTaskManager()
{
  
}

void JSTaskManager::queueTask(const JSTask::Task& task, void* userData, const JSTask::Task& completionCallback)
{
  JSTask::Ptr pTask(new JSTask(task, userData));
  if (completionCallback)
  {
    pTask->setCompletionCallback(completionCallback);
  }
  queueTask(pTask);
}

void JSTaskManager::queueTask(const JSTask::Ptr& pTask)
{
  _mutex.lock();
  push(pTask);
  _mutex.unlock();
  _pEventLoop->wakeup();
}

bool JSTaskManager::doOneWork()
{
  _mutex.lock();
  if (!empty())
  {
    JSTask::Ptr pTask = front();
    pop();
    _mutex.unlock();
    pTask->execute();
    return true;
  }
  _mutex.unlock();
  return false;
}
  

} } // OSS::JS

