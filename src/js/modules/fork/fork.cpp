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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

JS_METHOD_IMPL(__fork)
{
  return JSInt32(fork());
}

JS_METHOD_IMPL(__wait)
{
  int status = 0;
  pid_t pid = ::wait(&status);
  JSObjectHandle result = JSObject();
  result->Set(JSLiteral("pid"), JSUInt32(pid));
  result->Set(JSLiteral("status"), JSUInt32(status));
  return result;
}

JS_EXPORTS_INIT()
{
  js_export_method("fork", __fork);
  js_export_method("wait", __wait);
  js_export_finalize();
}

JS_REGISTER_MODULE(JSFork);
