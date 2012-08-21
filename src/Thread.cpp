// OSS Software Solutions Application Programmer Interface
// Package: OSSAPI
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSS Core SDK.
//
// Copyright (c) OSS Software Solutions
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
#include "OSS/Thread.h"
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

bool thread_pool::schedule(boost::function<void()> task)
{
  thread_pool_runnable* runnable = new thread_pool_runnable();
  runnable->_task = task;
  try
  {
    static_cast<Poco::ThreadPool*>(_threadPool)->start(*runnable);
  }
  catch(...)
  {
    delete runnable;
    return false;
  }
  return true;
}

void thread_pool::schedule_with_arg(boost::function<void(argument_place_holder)> task, argument_place_holder arg)
{
  thread_pool_runnable* runnable = new thread_pool_runnable();
  runnable->_arg = arg;
  runnable->_argTask = task;
  static_cast<Poco::ThreadPool*>(_threadPool)->start(*runnable);
}


void thread_pool::static_schedule(boost::function<void()> task)
{
  thread_pool_runnable* runnable = new thread_pool_runnable();
  runnable->_task = task;
  Poco::ThreadPool::defaultPool().start(*runnable);
}

void thread_pool::static_schedule_with_arg(boost::function<void(argument_place_holder)> task, argument_place_holder arg)
{
  thread_pool_runnable* runnable = new thread_pool_runnable();
  runnable->_arg = arg;
  runnable->_argTask = task;
  Poco::ThreadPool::defaultPool().start(*runnable);
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
	timeval sTimeout = { milliseconds / 1000, ( milliseconds % 1000 ) * 1000 };
	select( 0, 0, 0, 0, &sTimeout );
#endif
}

} // OSS
