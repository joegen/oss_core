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

#ifndef OSS_JSFILEDESCRIPTOR_H_INCLUDED
#define OSS_JSFILEDESCRIPTOR_H_INCLUDED


#include "OSS/build.h"
#if ENABLE_FEATURE_V8

#include "v8.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JS/JSPlugin.h"
#include <poll.h>


namespace OSS {
namespace JS {

  
class JSFileDescriptor
{
public:
  typedef boost::shared_ptr<JSFileDescriptor> Ptr;
  JSFileDescriptor();
  JSFileDescriptor(v8::Handle<v8::Value> ioHandler, int fd, int events);
  ~JSFileDescriptor();
  void setFileDescriptor(int fd);
  int getFileDescriptor() const;
  void setIoHandler(v8::Handle<v8::Value> ioHandler);
  const v8::Persistent<v8::Function>& getIoHandler() const;
  pollfd& pollFd();
  void signalIO(pollfd pfd);
protected:
  v8::Persistent<v8::Function> _ioHandler;
  pollfd _pollfd;
};
  
//
// Inlines
//
inline JSFileDescriptor::JSFileDescriptor()
{
}

inline JSFileDescriptor::JSFileDescriptor(v8::Handle<v8::Value> ioHandler, int fd, int events)
{
  _ioHandler = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(ioHandler));
  _pollfd.fd = fd;
  _pollfd.events = events;
}

inline JSFileDescriptor::~JSFileDescriptor()
{
  if (!_ioHandler.IsEmpty())
  {
    _ioHandler.Dispose();
  }
}

inline void JSFileDescriptor::setFileDescriptor(int fd)
{
  _pollfd.fd = fd;
}

inline int JSFileDescriptor::getFileDescriptor() const
{
  return _pollfd.fd;
}

inline void JSFileDescriptor::setIoHandler(v8::Handle<v8::Value> ioHandler)
{
  _ioHandler = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(ioHandler));
}

inline const v8::Persistent<v8::Function>& JSFileDescriptor::getIoHandler() const
{
  return _ioHandler;
}

inline pollfd& JSFileDescriptor::pollFd()
{
  return _pollfd;
}

inline void JSFileDescriptor::signalIO(pollfd pfd)
{
  std::vector< v8::Local<v8::Value> > args(2);
  args[0] = v8::Int32::New(pfd.fd);
  args[1] = v8::Int32::New(pfd.revents);
  v8::Local<v8::Context> context = v8::Context::GetCalling();
  assert(!context.IsEmpty());
  assert(!_ioHandler.IsEmpty());
  _ioHandler->Call(context->Global(), args.size(), args.data());
}


} }

#endif // ENABLE_FEATURE_V8
#endif // OSS_JSFILEDESCRIPTOR_H_INCLUDED

