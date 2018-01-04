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

#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include <errno.h>
#include <poll.h>

typedef std::vector<pollfd> PollFD;

static void array_to_pollfd_vector(v8::Handle<v8::Array>& input, PollFD& output)
{
  v8::HandleScope scope;
  for(uint32_t i = 0; i < input->Length(); i++)
  {
    v8::Handle<v8::Object> item = input->Get(i)->ToObject();
    v8::Handle<v8::Integer> fd = item->Get(v8::String::New("fd"))->ToInteger();
    v8::Handle<v8::Integer> events = item->Get(v8::String::New("events"))->ToInteger();

    pollfd pfd;
    pfd.fd = fd->Value();
    pfd.events = events->Value();
    pfd.revents = 0;
    output.push_back(pfd);
  }
}

static void pollfd_vector_to_array(PollFD& input, v8::Handle<v8::Array>& output)
{
  uint32_t index = 0;
  for (PollFD::iterator iter = input.begin(); iter != input.end(); iter++)
  {
    v8::Handle<v8::Object> item = v8::Object::New();
    v8::Handle<v8::Integer> fd = v8::Integer::New(iter->fd);
    v8::Handle<v8::Integer> events = v8::Integer::New(iter->events);
    v8::Handle<v8::Integer> revents = v8::Integer::New(iter->revents);
    
    item->Set(v8::String::New("fd"), fd);
    item->Set(v8::String::New("events"), events);
    item->Set(v8::String::New("revents"), revents);
    
    output->Set(index++, item);
  }
}

static v8::Handle<v8::Value> __poll(const v8::Arguments& args)
{
  if (args.Length() < 1 || !args[0]->IsArray())
  {
    return v8::Undefined();
  }
  v8::HandleScope scope;
  
  PollFD pfds;
  v8::Handle<v8::Array> args0 = v8::Handle<v8::Array>::Cast(args[0]);
  int timeout = -1;
  array_to_pollfd_vector(args0, pfds);
  
  if (args.Length() >= 2 && args[1]->IsNumber())
  {
    timeout = args[1]->IntegerValue();
  }
  
  v8::Handle<v8::Array> result = v8::Array::New(2);
  int ret = ::poll(pfds.data(), pfds.size(), timeout);
  result->Set(0, v8::Integer::New(ret));
  
  if (ret == 0 || ret == -1)
  {
    return result;
  }
  
  v8::Handle<v8::Array> pfdsout = v8::Array::New(pfds.size());
  pollfd_vector_to_array(pfds, pfdsout);
  result->Set(1, pfdsout);
  return result;
}

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  
  //
  // Methods
  //
  exports->Set(v8::String::New("poll"), v8::FunctionTemplate::New(__poll)->GetFunction());
  //
  // Mutable Properties
  //

  //
  // Constants
  //
  CONST_EXPORT(POLLIN);  /* There is data to read.  */
  CONST_EXPORT(POLLPRI);  /* There is urgent data to read.  */
  CONST_EXPORT(POLLOUT);  /* Writing now will not block.  */

  #if defined __USE_XOPEN || defined __USE_XOPEN2K8
    CONST_EXPORT(POLLRDNORM);  /* Normal data may be read.  */
    CONST_EXPORT(POLLRDBAND);  /* Priority data may be read.  */
    CONST_EXPORT(POLLWRNORM);  /* Writing now will not block.  */
    CONST_EXPORT(POLLWRBAND);  /* Priority data may be written.  */
  #endif

  #ifdef __USE_GNU
    CONST_EXPORT(POLLMSG);
    CONST_EXPORT(POLLREMOVE);
    CONST_EXPORT(POLLRDHUP);
  #endif

  CONST_EXPORT(POLLERR);    /* Error condition.  */
  CONST_EXPORT(POLLHUP);    /* Hung up.  */
  CONST_EXPORT(POLLNVAL);   /* Invalid polling request.  */

  return exports;
}

JS_REGISTER_MODULE(Poll);
