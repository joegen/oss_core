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


#ifndef OSS_BLOCKINGQUEUE_H_INCLUDED
#define OSS_BLOCKINGQUEUE_H_INCLUDED

#include <queue>
#include <boost/noncopyable.hpp>
#include "OSS/UTL/Thread.h"
#include <unistd.h>

namespace OSS {

template <class T>
class BlockingQueue : boost::noncopyable
{
public:
  BlockingQueue(bool usePipe = false) :
    _sem(0, 0xFFFF),
    _usePipe(usePipe)
  {
    if (_usePipe)
    {
      pipe(_pipe);
    }
  }
  
  ~BlockingQueue()
  {
    if (_usePipe)
    {
      close(_pipe[0]);
      close(_pipe[1]);
    }
  }

  void enqueue(T data)
  {
    _cs.lock();
    _queue.push(data);
    if (_usePipe)
    {
      write(_pipe[1], " ", 1);
    }
    _cs.unlock();
    _sem.set();
  }

  void dequeue(T& data)
  {
    _sem.wait();
    _cs.lock();
    if (_usePipe)
    {
      char buf[1];
      read(_pipe[0], buf, 1);
    }
    data = _queue.front();
    _queue.pop();
    _cs.unlock();
  }

  bool try_dequeue(T& data, long milliseconds)
  {
    if (!_sem.tryWait(milliseconds))
      return false;

    _cs.lock();
    if (_usePipe)
    {
      char buf[1];
      read(_pipe[0], buf, 1);
    }
    data = _queue.front();
    _queue.pop();
    _cs.unlock();

    return true;
  }
  
  std::size_t size() const
  {
    std::size_t ret = 0;
    _cs.lock();
    ret = _queue.size();
    _cs.unlock();
    return ret;
  }
  
  int getFd() const
  {
    return _usePipe ? _pipe[0] : 0;
  }
  
private:
  OSS::semaphore _sem;
  mutable OSS::mutex_critic_sec _cs;
  std::queue<T> _queue;
  int _pipe[2];
  bool _usePipe;
};

} // OSS

#endif //OSS_BLOCKINGQUEUE_H_INCLUDED


