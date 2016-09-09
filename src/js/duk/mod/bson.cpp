
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


#include "OSS/JS/DUK/InternalModules.h"
#include "OSS/BSON/BSONParser.h"
#include "OSS/JS/DUK/duk_context_helper.h"


namespace OSS {
namespace JS {
namespace DUK {
namespace MOD {

using namespace OSS::BSON;

static duk_functions_t gFunctions;

duk_ret_t bson_create(duk_context* ctx)
{
  duk_push_pointer(ctx, new BSONParser());
  return 1;
}

duk_ret_t bson_mod_init(duk_context* ctx)
{
  gFunctions.push_back({ "bson_create", bson_create, 0 });
  gFunctions.push_back({ 0, 0, 0 });
  duk_export_functions(ctx, gFunctions);
  return 1;
}

} } } }
