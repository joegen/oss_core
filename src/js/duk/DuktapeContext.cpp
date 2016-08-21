
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


#include <boost/thread/pthread/mutex.hpp>

#include "OSS/JS/DUK/DuktapeContext.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace JS {
namespace DUK {



OSS::mutex_critic_sec DuktapeContext::_duk_mutex;

static void duktape_fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg) 
{ 
  OSS_LOG_FATAL("Duktape Fatal Error!  "  << "Code: " << code << " Msg:" << (const char *) (msg ? msg : "null"));
  abort();
}

DuktapeContext::DuktapeContext(const std::string& name) :
  _name(name),
  _pContext(0)
{
  _duk_mutex.lock();
  _pContext = duk_create_heap(NULL, // alloc function
      NULL, // realloc function
      NULL, // free function
      this, // user data
      duktape_fatal_handler); // fatal error handler
  _duk_mutex.unlock();
}

DuktapeContext::~DuktapeContext()
{
  _duk_mutex.lock();
  duk_destroy_heap(_pContext);
  _duk_mutex.unlock();
}


} } } // OSS::JS::DUK



