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
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "OSS/UTL/Thread.h"
#include <unistd.h>

namespace OSS {

template <class T>
class BlockingQueue : boost::noncopyable
{
public:
  typedef boost::function<bool(BlockingQueue<T>&, const T&)> QueueObserver;
  
  BlockingQueue(bool usePipe = false, std::size_t maxSize = 0) :
    _sem(0, SEM_VALUE_MAX),
    _usePipe(usePipe),
    _maxSize(maxSize)
  {
    if (_usePipe)
    {
      assert(pipe(_pipe) == 0);
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

  bool enqueue(T data)
  {
    _cs.lock();
    
    if (_maxSize && _queue.size() >= _maxSize)
    {
      _cs.unlock();
      return false;
    }
    
    if (_enqueueObserver && !_enqueueObserver(*this, data))
    {
      _cs.unlock();
      return false;
    }
    _queue.push_back(data);
    if (_usePipe)
    {
      std::size_t w = 0;
      w = write(_pipe[1], " ", 1);
      (void)w;
    }
    _cs.unlock();
    _sem.set();
    return true;
  }

  void dequeue(T& data)
  {
    _sem.wait();
    _cs.lock();
    if (_usePipe)
    {
      std::size_t r = 0;
      char buf[1];
      r = read(_pipe[0], buf, 1);
      (void)r;
    }
    if (!_queue.empty())
    {
      data = _queue.front();
      _queue.pop_front();
      if (_dequeueObserver)
      {
        _dequeueObserver(*this, data);
      }
    }
    _cs.unlock();
  }

  bool try_dequeue(T& data, long milliseconds)
  {
    if (!_sem.tryWait(milliseconds))
      return false;

    _cs.lock();
    if (_usePipe)
    {
      std::size_t r = 0;
      char buf[1];
      r = read(_pipe[0], buf, 1);
      (void)r;
    }
    if (!_queue.empty())
    {
      data = _queue.front();
      _queue.pop_front();
      if (_dequeueObserver)
      {
        _dequeueObserver(*this, data);
      }
    }
    else
    {
      _cs.unlock();
      return false;
    }
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
  
  void setEnqueueObserver(const QueueObserver& observer)
  {
    _enqueueObserver = observer;
  }
  
  void setDequeueObserver(const QueueObserver& observer)
  {
    _dequeueObserver = observer;
  }
  
  void copy(std::deque<T>& content)
  {
    _cs.lock();
    std::copy(_queue.begin(), _queue.end(),  std::back_inserter(content));
    _cs.unlock();
  }
  
  void clear()
  {
    while(size() != 0)
    {
      T data;
      dequeue(data);
    }
  }
  
  void setMaxSize(std::size_t maxSize)
  {
    _cs.lock();
    _maxSize = maxSize;
    _cs.unlock();
  }
  
  std::size_t getMaxSize() const
  {
    return _maxSize;
  }
private:
  OSS::semaphore _sem;
  mutable OSS::mutex_critic_sec _cs;
  std::deque<T> _queue;
  int _pipe[2];
  bool _usePipe;
  QueueObserver _enqueueObserver;
  QueueObserver _dequeueObserver;
  std::size_t _maxSize;
};

} // OSS

#endif //OSS_BLOCKINGQUEUE_H_INCLUDED


