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

#include <unistd.h>
#include "OSS/JS/DUK/InternalModules.h"
#include "OSS/JS/DUK/duk_context_helper.h"


namespace OSS {
namespace JS {
namespace DUK {
namespace MOD {

static duk_functions_t gFunctions;

duk_ret_t system_getnenv(duk_context* ctx)
{
  const char* arg = duk_require_string(ctx, 0);
  duk_push_string(ctx, getenv(arg));
  return 1;
}

duk_ret_t system_exit(duk_context* ctx)
{
  int val = duk_require_int(ctx, 0);
  _exit(val);
  return 0;
}

duk_ret_t system_mod_init(duk_context* ctx)
{
  gFunctions.push_back({ "getenv", system_getnenv, 1 });
  gFunctions.push_back({ "exit", system_exit, 1 });
  gFunctions.push_back({ 0, 0, 0 });

  duk_push_object(ctx);
  duk_export_functions(ctx, gFunctions);
  return 1;
}

} } } }
