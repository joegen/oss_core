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

#ifndef OSS_JSFUNCTIONCALLBACK_H_INCLUDED
#define OSS_JSFUNCTIONCALLBACK_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_V8
#include "OSS/JS/JS.h"
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "OSS/JS/JSIsolateManager.h"
#include "OSS/UTL/Thread.h"


namespace OSS {
namespace JS {


class JSFunctionCallback : public boost::enable_shared_from_this<JSFunctionCallback>, private boost::noncopyable
{
public:
  typedef std::vector< v8::Persistent<v8::Value> > ArgumentVector;
  typedef boost::shared_ptr<JSFunctionCallback> Ptr;
  
  JSFunctionCallback(v8::Handle<v8::Value> func);
  JSFunctionCallback(v8::Handle<v8::Value> func, v8::Handle<v8::Value>  args);
  JSFunctionCallback(v8::Handle<v8::Value> func, v8::Handle<v8::Value>  args, v8::Handle<v8::Value> resultHandler);
  virtual ~JSFunctionCallback();
  virtual void execute();
  void handle_to_arg_vector(v8::Handle<v8::Value> input, ArgumentVector& output);
  void dispose();
  bool& autoDisposeOnExecute();
  bool autoDisposeOnExecute() const;
private:
  v8::Persistent<v8::Function> _function;
  ArgumentVector _args;
  v8::Persistent<v8::Function> _resultHandler;
  
  bool _disposed;
  bool _autoDisposeOnExecute;
  OSS::mutex_critic_sec _disposeMutex;
};

inline bool& JSFunctionCallback::autoDisposeOnExecute()
{
    return _autoDisposeOnExecute;
}

inline bool JSFunctionCallback::autoDisposeOnExecute() const
{
    return _autoDisposeOnExecute;
}
  
inline JSFunctionCallback::JSFunctionCallback(v8::Handle<v8::Value> func) :
    _disposed(false),
    _autoDisposeOnExecute(false)
{
  _function =  v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(func));
}

inline JSFunctionCallback::JSFunctionCallback(v8::Handle<v8::Value> func, v8::Handle<v8::Value>  args)  :
    _disposed(false),
    _autoDisposeOnExecute(false)
{
  _function =  v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(func));
  handle_to_arg_vector(args, _args);
}

inline JSFunctionCallback::JSFunctionCallback(v8::Handle<v8::Value> func, v8::Handle<v8::Value>  args, v8::Handle<v8::Value> resultHandler)  :
    _disposed(false),
    _autoDisposeOnExecute(false)
{
  _function =  v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(func));
  _resultHandler =  v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(resultHandler));
  handle_to_arg_vector(args, _args);
}

inline JSFunctionCallback::~JSFunctionCallback()
{
    dispose();
}

inline void JSFunctionCallback::dispose()
{
    OSS::mutex_critic_sec_lock lock(_disposeMutex);
    
    if (_disposed)
    {
        return;
    }
    
    _function.Dispose();
    if (!_resultHandler.IsEmpty())
    {
      _resultHandler.Dispose();
    }
    for (ArgumentVector::iterator iter = _args.begin(); iter != _args.end(); iter++)
    {
      iter->Dispose();
    }
    _disposed = true;
}

inline void JSFunctionCallback::handle_to_arg_vector(v8::Handle<v8::Value> input, ArgumentVector& output)
{
  if (input->IsArray())
  {
    v8::Handle<v8::Array> arrayArg = v8::Handle<v8::Array>::Cast(input);
    for (std::size_t i = 0; i <arrayArg->Length(); i++)
    {
      output.push_back(v8::Persistent<v8::Value>::New(arrayArg->Get(i)));
    }
  }
  else
  {
    output.push_back(v8::Persistent<v8::Value>::New(input));
  }
}

inline void JSFunctionCallback::execute()
{
  if (_resultHandler.IsEmpty())
  {
    _function->Call(js_get_global(), _args.size(), _args.data());
  }
  else
  {
    v8::Handle<v8::Value> result = _function->Call(js_get_global(), _args.size(), _args.data());
    ArgumentVector resultArg;
    handle_to_arg_vector(result, resultArg);
    _resultHandler->Call(js_get_global(), resultArg.size(), resultArg.data());
  }
  
  if (_autoDisposeOnExecute)
  {
      dispose();
  }
}

} } // OSS::JS


#endif // ENABLE_FEATURE_V8
#endif // OSS_JSFUNCTIONCALLBACK_H_INCLUDED

