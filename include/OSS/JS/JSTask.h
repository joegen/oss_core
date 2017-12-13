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

#ifndef OSS_JSTASK_H_INCLUDED
#define OSS_JSTASK_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_V8

#include "v8.h"


#include <boost/function.hpp>
#include <boost/core/addressof.hpp>
#include <boost/shared_ptr.hpp>


namespace OSS {
namespace JS {



class JSTask
{
public:
  typedef boost::function<void(void*)> Task;
  typedef boost::shared_ptr<JSTask> Ptr;
  
  JSTask();
  JSTask(const JSTask& task);
  JSTask(const JSTask::Task& task);
  JSTask(const JSTask::Task& task, void* userData);
  ~JSTask();
  JSTask& operator = (const JSTask& task);
  JSTask& operator = (const JSTask::Task& task);
  void setUserData(void* userData);
  void* getUserData() const;
  void setCompletionCallback(const JSTask::Task& callback);
  
protected:
  void execute();
private:
  void* _userData;
  JSTask::Task _func;
  JSTask::Task _callback;
  friend class JSTaskManager;
};

//
// Inlines
//

inline JSTask::JSTask() :
  _userData(0)
{
}

inline JSTask::JSTask(const JSTask& task)
{
  _userData = task._userData;
  _func = task._func;
  _callback = task._callback;
}

inline JSTask::JSTask(const JSTask::Task& task)
{
  _userData = 0;
  _func = task;
}

inline JSTask::JSTask(const JSTask::Task& task, void* userData)
{
  _func = task;
  _userData = userData;
}

inline JSTask::~JSTask()
{
}

inline JSTask& JSTask::operator = (const JSTask& task)
{
  if (boost::addressof(task) == this)
  {
    return *this;
  }
  _func = task._func;
  _userData = task._userData;
  _callback = task._callback;
  return *this;
}

inline JSTask& JSTask::operator = (const JSTask::Task& task)
{
  _userData = 0;
  _func = task;
  return *this;
}

inline void JSTask::execute()
{
  assert(_func);
  _func(_userData);
  if (_callback)
  {
    _callback(_userData);
  }
}

inline void JSTask::setUserData(void* userData)
{
  _userData = userData;
}

inline void* JSTask::getUserData() const
{
  return _userData;
}

inline void JSTask::setCompletionCallback(const JSTask::Task& callback)
{
  _callback = callback;
}

} } // OSS::JS


#endif // ENABLE_FEATURE_V8
#endif // OSS_JSTASK_H_INCLUDED

