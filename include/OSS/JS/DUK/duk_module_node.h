#if !defined(DUK_MODULE_NODE_H_INCLUDED)
#define DUK_MODULE_NODE_H_INCLUDED

#include "duktape.h"

#ifdef __cplusplus
extern "C" {
#endif
extern void duk_module_node_init(duk_context *ctx);
extern duk_ret_t duk_module_node_peval_file(duk_context *ctx, const char* filename, int main);
#ifdef __cplusplus
}
#endif

#endif  /* DUK_MODULE_NODE_H_INCLUDED */
