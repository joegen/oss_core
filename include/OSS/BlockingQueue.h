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


#ifndef OSS_BLOCKINGQUEUE_H_INCLUDED
#define OSS_BLOCKINGQUEUE_H_INCLUDED

#include <queue>
#include <boost/noncopyable.hpp>
#include "OSS/Thread.h"

namespace OSS {

template <class T>
class BlockingQueue : boost::noncopyable
{
public:
  BlockingQueue(): _sem(0, 0xFFFF)
  {
  }

  void enqueue(T data)
  {
    _cs.lock();
    _queue.push(data);
    _cs.unlock();
    _sem.set();
  }

  void dequeue(T& data)
  {
    _sem.wait();
    _cs.lock();
    data = _queue.front();
    _queue.pop();
    _cs.unlock();
  }

  bool try_dequeue(T& data, long milliseconds)
  {
    if (!_sem.tryWait(milliseconds))
      return false;

    _cs.lock();
    data = _queue.front();
    _queue.pop();
    _cs.unlock();

    return true;
  }
private:
  OSS::semaphore _sem;
  OSS::mutex_critic_sec _cs;
  std::queue<T> _queue;
};

} // OSS

#endif //OSS_BLOCKINGQUEUE_H_INCLUDED


