
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


#include "OSS/JS/DUK/DuktapeContext.h"
#include "OSS/JS/DUK/duk_context_helper.h"
#include "OSS/UTL/Logger.h"


using OSS::JS::DUK::DuktapeContext;
using OSS::JS::DUK::DuktapeModule;


void duktape_fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg) 
{ 
  OSS_LOG_FATAL("Duktape Fatal Error!  "  << "Code: " << code << " Msg:" << (const char *) (msg ? msg : "null"));
  abort();
}

duk_ret_t duktape_resolve_module(duk_context* ctx) 
{
  std::string moduleId = duk_get_string(ctx, 0);
  std::string parentId = duk_get_string(ctx, 1);
  std::string resolvedId;
  
  DuktapeContext* pContext = DuktapeContext::getContext(ctx);
  if (pContext)
  {
    if (pContext->resolveModule(parentId, moduleId, resolvedId))
    {
      duk_push_string(ctx, resolvedId.c_str());
      return 1;
    }
  }
  duk_error(ctx, DUK_ERR_ERROR, "Module not found");
  return -42;
}

duk_ret_t duktape_load_module(duk_context* ctx)
{
  std::string resolvedId = duk_require_string(ctx, 0);
  DuktapeContext* pContext = DuktapeContext::getContext(ctx);
  if (pContext)
  {
    return pContext->loadModule(resolvedId);
  }
  return -1;
}

int duktape_get_error_stack(duk_context *ctx) 
{
    if (!duk_is_object(ctx, -1)) 
    {
      return 1;
    }

    if (!duk_has_prop_string(ctx, -1, "stack")) 
    {
        return 1;
    }

    if (!duk_is_error(ctx, -1)) 
    {
        /* Not an Error instance, don't read "stack" */
        return 1;
    }

    duk_get_prop_string(ctx, -1, "stack");  /* caller coerces */
    duk_remove(ctx, -2);
    return 1;
}


void duktape_dump_result(duk_context* ctx, int r, FILE* foutput, FILE* ferror) 
{
    if (r != DUK_EXEC_SUCCESS) 
    {
      if (ferror) 
      {
        duk_safe_call(ctx, duktape_get_error_stack, 1 /*nargs*/, 1 /*nrets*/);
        fprintf(ferror, "%s\n", duk_safe_to_string(ctx, -1));
        fflush(ferror);
      }
    } 
    else 
    {
      if (foutput) 
      {
        /* TODO: make this optional with a parameter? */
        /* beautify output */
        duk_eval_string(ctx, "(function (v) {\n"
                             "    try {\n"
                             "        return Duktape.enc('jx', v, null, 4);\n"
                             "    } catch (e) {\n"
                             "        return ''+v;\n"
                             "    }\n"
                             "})");
        duk_insert(ctx, -2);
        duk_call(ctx, 1);

        fprintf(foutput, "= %s\n", duk_safe_to_string(ctx, -1));
        fflush(foutput);
      }
    }
    duk_pop(ctx);
}

void duk_export_functions(duk_context* ctx, duk_functions_t& funcs)
{
  duk_put_function_list(ctx, -1, funcs.data());
}

void duk_export_constants(duk_context* ctx, duk_numeric_constants_t& constants)
{
  duk_put_number_list(ctx, -1, constants.data());
  duk_put_prop_string(ctx, -2, "defines");
}

void duk_init_common_js(duk_context* ctx)
{
  duk_push_object(ctx);
  duk_push_c_function(ctx, duktape_resolve_module, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "resolve");
  duk_push_c_function(ctx, duktape_load_module, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "load");
  duk_module_node_init(ctx);
}
