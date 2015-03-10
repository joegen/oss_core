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


#include "OSS/OSS.h"
#include "OSS/UTL/Thread.h"
#include "Poco/ThreadPool.h"
#include "Poco/Semaphore.h"


namespace OSS {


//
// Semaphore
//

semaphore::semaphore()
{
  _sem = new Poco::Semaphore(0, 1);
}

semaphore::semaphore(int n)
{
  _sem = new Poco::Semaphore(n);
}

semaphore::semaphore(int n, int max)
{
  _sem = new Poco::Semaphore(n, max);
}

semaphore::~semaphore()
{
  delete static_cast<Poco::Semaphore*>(_sem);
}

void semaphore::set()
{
  static_cast<Poco::Semaphore*>(_sem)->set();
}

void semaphore::wait()
{
  static_cast<Poco::Semaphore*>(_sem)->wait();
}

bool semaphore::wait(long milliseconds)
{
  try
  {
    static_cast<Poco::Semaphore*>(_sem)->wait(milliseconds);
  }
  catch(const Poco::Exception& e)
  {
    return false;
  }
  return true;
}

bool semaphore::tryWait(long milliseconds)
{
  return static_cast<Poco::Semaphore*>(_sem)->tryWait(milliseconds);
}


//
// Threadpool
//

class thread_pool_runnable : public Poco::Runnable
{
public:
  thread_pool_runnable()
  {
  }
  void run()
  {
    if (_task)
      _task();
    else if (_argTask)
      _argTask(_arg);

    delete this;
  }
  boost::function<void()> _task;
  thread_pool::argument_place_holder _arg;
  boost::function<void(thread_pool::argument_place_holder)> _argTask;
};

thread_pool::thread_pool(
  int minCapacity,
	int maxCapacity,
	int idleTime,
  int stackSize) : _threadPool(0)
{
  _threadPool = new Poco::ThreadPool(minCapacity, maxCapacity, idleTime, stackSize);
}

thread_pool::~thread_pool()
{
  delete static_cast<Poco::ThreadPool*>(_threadPool);
}

void thread_pool::join()
{
  static_cast<Poco::ThreadPool*>(_threadPool)->joinAll();
}

int thread_pool::schedule(boost::function<void()> task)
{
  thread_pool_runnable* runnable = new thread_pool_runnable();
  runnable->_task = task;
  try
  {
    static_cast<Poco::ThreadPool*>(_threadPool)->start(*runnable);
    return static_cast<Poco::ThreadPool*>(_threadPool)->used();
  }
  catch(...)
  {
    delete runnable;
    return -1;
  }
  return 0;
}

int thread_pool::schedule_with_arg(boost::function<void(argument_place_holder)> task, argument_place_holder arg)
{
  thread_pool_runnable* runnable = new thread_pool_runnable();
  runnable->_argTask = task;
  runnable->_arg = arg;
  try
  {
    static_cast<Poco::ThreadPool*>(_threadPool)->start(*runnable);
    return static_cast<Poco::ThreadPool*>(_threadPool)->used();
  }
  catch(...)
  {
    delete runnable;
    return -1;
  }
  return 0;
}


int thread_pool::static_schedule(boost::function<void()> task)
{
  thread_pool_runnable* runnable = new thread_pool_runnable();
  runnable->_task = task;
  try
  {
    Poco::ThreadPool::defaultPool().start(*runnable);
    return Poco::ThreadPool::defaultPool().used();
  }
  catch(...)
  {
    delete runnable;
    return -1;
  }
  return 0;

}

int thread_pool::static_schedule_with_arg(boost::function<void(argument_place_holder)> task, argument_place_holder arg)
{
  thread_pool_runnable* runnable = new thread_pool_runnable();
  runnable->_argTask = task;
  runnable->_arg = arg;
  try
  {
    Poco::ThreadPool::defaultPool().start(*runnable);
    return Poco::ThreadPool::defaultPool().used();
  }
  catch(...)
  {
    delete runnable;
    return -1;
  }
  return 0;
}

void thread_pool::static_join()
{
  Poco::ThreadPool::defaultPool().joinAll();
}


#if defined ( WIN32 ) || defined ( WIN64 )
	#include <winsock2.h>
	#include <windows.h>
#else	// defined ( WIN32 ) || defined ( WIN64 )
	#include <sys/select.h>
#endif	// defined ( WIN32 ) || defined ( WIN64 )


void thread_sleep( unsigned long milliseconds )
  /// Pause thread execution for certain time expressed in milliseconds
{
#if defined ( WIN32 )
	::Sleep( milliseconds );
#else
	timeval sTimeout = { (long int)(milliseconds / 1000), (long int)(( milliseconds % 1000 ) * 1000) };
	select( 0, 0, 0, 0, &sTimeout );
#endif
}

Thread::Thread() :
  _task(),
  _pThread(0),
  _terminateFlag(true)
{
}

Thread::Thread(const Task& task) :
  _task(task),
  _pThread(0),
  _terminateFlag(true)
{
}

Thread::~Thread()
{
  stop();
}

void Thread::run()
{
  {
  mutex_critic_sec_lock lock(_terminateFlagMutex);
  if (!isTerminated())
    stop();
  _terminateFlag = false;
  }
  
  {
    mutex_critic_sec_lock lock(_threadMutex);
    assert(!_pThread);
    _pThread = new boost::thread(boost::bind(&Thread::runTask, this));
  }
}

void Thread::stop()
{
  {
    mutex_critic_sec_lock lock(_terminateFlagMutex);
    if (_terminateFlag)
    {
      assert(!_pThread);
      return; // we are already terminated
    }
  }
    
  {
    mutex_critic_sec_lock lock(_terminateFlagMutex);
    _terminateFlag = true;
  }
  
  {
    mutex_critic_sec_lock lock(_threadMutex);
    assert(_pThread);
    
    onTerminate();
    
    _pThread->join();
    delete _pThread;
    _pThread = 0;
  }
}

void Thread::setTask(const Task& task)
{
  assert(isTerminated());
  _task = task;
}

bool Thread::isTerminated()
{
  mutex_critic_sec_lock lock(_terminateFlagMutex);
  return _terminateFlag;
}
  
void Thread::runTask()
{
  if (_task)
  {
    _task();
  }
}

void Thread::onTerminate()
{
}


} // OSS
