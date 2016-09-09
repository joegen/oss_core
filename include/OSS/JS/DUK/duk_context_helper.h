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

#ifndef OSS_DUK_CONTEXT_HELPER_H_INCLUDED
#define	OSS_DUK_CONTEXT_HELPER_H_INCLUDED

#include <stdlib.h>
#include <vector>
#include "OSS/JS/DUK/duktape.h"


typedef std::vector<duk_function_list_entry> duk_functions_t;
typedef std::vector<duk_number_list_entry> duk_numeric_constants_t;

void duktape_fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg);

duk_ret_t duktape_resolve_module(duk_context* ctx);

duk_ret_t duktape_load_module(duk_context* ctx);

int duktape_get_error_stack(duk_context *ctx);

void duktape_dump_result(duk_context* ctx, int r, FILE* foutput, FILE* ferror);

void duk_export_functions(duk_context* ctx, duk_functions_t& funcs);

void duk_export_constants(duk_context* ctx, duk_numeric_constants_t& constants);

void duk_init_common_js(duk_context* ctx);

#endif	// OSS_DUK_CONTEXT_HELPER_H_INCLUDED

