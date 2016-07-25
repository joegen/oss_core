#define BSON_INSIDE
#define BSON_COMPILATION
static void _noop (void){}

// #include <yajl/yajl_version.h>
#ifndef YAJL_VERSION_H_
#define YAJL_VERSION_H_

// #include <yajl/yajl_common.h>
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __YAJL_COMMON_H__
#define __YAJL_COMMON_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif    

#define YAJL_MAX_DEPTH 128

/* msft dll export gunk.  To build a DLL on windows, you
 * must define WIN32, YAJL_SHARED, and YAJL_BUILD.  To use a shared
 * DLL, you must define YAJL_SHARED and WIN32 */
#if (defined(_WIN32) || defined(WIN32)) && defined(YAJL_SHARED)
#  ifdef YAJL_BUILD
#    define YAJL_API __declspec(dllexport)
#  else
#    define YAJL_API __declspec(dllimport)
#  endif
#else
#  if defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 303
#    define YAJL_API __attribute__ ((visibility("default")))
#  else
#    define YAJL_API
#  endif
#endif 

/** pointer to a malloc function, supporting client overriding memory
 *  allocation routines */
typedef void * (*yajl_malloc_func)(void *ctx, size_t sz);

/** pointer to a free function, supporting client overriding memory
 *  allocation routines */
typedef void (*yajl_free_func)(void *ctx, void * ptr);

/** pointer to a realloc function which can resize an allocation. */
typedef void * (*yajl_realloc_func)(void *ctx, void * ptr, size_t sz);

/** A structure which can be passed to yajl_*_alloc routines to allow the
 *  client to specify memory allocation functions to be used. */
typedef struct
{
    /** pointer to a function that can allocate uninitialized memory */
    yajl_malloc_func malloc;
    /** pointer to a function that can resize memory allocations */
    yajl_realloc_func realloc;
    /** pointer to a function that can free memory allocated using
     *  reallocFunction or mallocFunction */
    yajl_free_func free;
    /** a context pointer that will be passed to above allocation routines */
    void * ctx;
} yajl_alloc_funcs;

#ifdef __cplusplus
}
#endif

#endif


#define YAJL_MAJOR 2
#define YAJL_MINOR 0
#define YAJL_MICRO 4

#define YAJL_VERSION ((YAJL_MAJOR * 10000) + (YAJL_MINOR * 100) + YAJL_MICRO)

#ifdef __cplusplus
extern "C" {
#endif

extern int YAJL_API yajl_version(void);

#ifdef __cplusplus
}
#endif

#endif /* YAJL_VERSION_H_ */



int yajl_version(void)
{
	return YAJL_VERSION;
}

/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file yajl_alloc.h
 * default memory allocation routines for yajl which use malloc/realloc and
 * free
 */

// #include "yajl_alloc.h"
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file yajl_alloc.h
 * default memory allocation routines for yajl which use malloc/realloc and
 * free
 */

#ifndef __YAJL_ALLOC_H__
#define __YAJL_ALLOC_H__

// #include "yajl_common.h"


#define YA_MALLOC(afs, sz) (afs)->malloc((afs)->ctx, (sz))
#define YA_FREE(afs, ptr) (afs)->free((afs)->ctx, (ptr))
#define YA_REALLOC(afs, ptr, sz) (afs)->realloc((afs)->ctx, (ptr), (sz))

void yajl_set_default_alloc_funcs(yajl_alloc_funcs * yaf);

#endif

#include <stdlib.h>

static void * yajl_internal_malloc(void *ctx, size_t sz)
{
    return malloc(sz);
}

static void * yajl_internal_realloc(void *ctx, void * previous,
                                    size_t sz)
{
    return realloc(previous, sz);
}

static void yajl_internal_free(void *ctx, void * ptr)
{
    free(ptr);
}

void yajl_set_default_alloc_funcs(yajl_alloc_funcs * yaf)
{
    yaf->malloc = yajl_internal_malloc;
    yaf->free = yajl_internal_free;
    yaf->realloc = yajl_internal_realloc;
    yaf->ctx = NULL;
}

/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// #include "yajl_buf.h"
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __YAJL_BUF_H__
#define __YAJL_BUF_H__

// #include "yajl_common.h"

// #include "yajl_alloc.h"


/*
 * Implementation/performance notes.  If this were moved to a header
 * only implementation using #define's where possible we might be 
 * able to sqeeze a little performance out of the guy by killing function
 * call overhead.  YMMV.
 */

/**
 * yajl_buf is a buffer with exponential growth.  the buffer ensures that
 * you are always null padded.
 */
typedef struct yajl_buf_t * yajl_buf;

/* allocate a new buffer */
yajl_buf yajl_buf_alloc(yajl_alloc_funcs * alloc);

/* free the buffer */
void yajl_buf_free(yajl_buf buf);

/* append a number of bytes to the buffer */
void yajl_buf_append(yajl_buf buf, const void * data, size_t len);

/* empty the buffer */
void yajl_buf_clear(yajl_buf buf);

/* get a pointer to the beginning of the buffer */
const unsigned char * yajl_buf_data(yajl_buf buf);

/* get the length of the buffer */
size_t yajl_buf_len(yajl_buf buf);

/* truncate the buffer */
void yajl_buf_truncate(yajl_buf buf, size_t len);

#endif


#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define YAJL_BUF_INIT_SIZE 2048

struct yajl_buf_t {
    size_t len;
    size_t used;
    unsigned char * data;
    yajl_alloc_funcs * alloc;
};

static
void yajl_buf_ensure_available(yajl_buf buf, size_t want)
{
    size_t need;
    
    assert(buf != NULL);

    /* first call */
    if (buf->data == NULL) {
        buf->len = YAJL_BUF_INIT_SIZE;
        buf->data = (unsigned char *) YA_MALLOC(buf->alloc, buf->len);
        buf->data[0] = 0;
    }

    need = buf->len;

    while (want >= (need - buf->used)) need <<= 1;

    if (need != buf->len) {
        buf->data = (unsigned char *) YA_REALLOC(buf->alloc, buf->data, need);
        buf->len = need;
    }
}

yajl_buf yajl_buf_alloc(yajl_alloc_funcs * alloc)
{
    yajl_buf b = YA_MALLOC(alloc, sizeof(struct yajl_buf_t));
    memset((void *) b, 0, sizeof(struct yajl_buf_t));
    b->alloc = alloc;
    return b;
}

void yajl_buf_free(yajl_buf buf)
{
    assert(buf != NULL);
    if (buf->data) YA_FREE(buf->alloc, buf->data);
    YA_FREE(buf->alloc, buf);
}

void yajl_buf_append(yajl_buf buf, const void * data, size_t len)
{
    yajl_buf_ensure_available(buf, len);
    if (len > 0) {
        assert(data != NULL);
        memcpy(buf->data + buf->used, data, len);
        buf->used += len;
        buf->data[buf->used] = 0;
    }
}

void yajl_buf_clear(yajl_buf buf)
{
    buf->used = 0;
    if (buf->data) buf->data[buf->used] = 0;
}

const unsigned char * yajl_buf_data(yajl_buf buf)
{
    return buf->data;
}

size_t yajl_buf_len(yajl_buf buf)
{
    return buf->used;
}

void
yajl_buf_truncate(yajl_buf buf, size_t len)
{
    assert(len <= buf->used);
    buf->used = len;
}
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// #include "yajl_parse.h"
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file yajl_parse.h
 * Interface to YAJL's JSON stream parsing facilities.
 */

// #include "bson-compat.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_COMPAT_H
#define BSON_COMPAT_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


#if defined(__MINGW32__)
# if defined(__USE_MINGW_ANSI_STDIO)
#  if __USE_MINGW_ANSI_STDIO < 1
#   error "__USE_MINGW_ANSI_STDIO > 0 is required for correct PRI* macros"
#  endif
# else
#  define __USE_MINGW_ANSI_STDIO 1
# endif
#endif

// #include "bson-config.h"
#ifndef BSON_CONFIG_H
#define BSON_CONFIG_H

/*
 * Define to 1234 for Little Endian, 4321 for Big Endian.
 */
#define BSON_BYTE_ORDER 1234


/*
 * Define to 1 if you have stdbool.h
 */
#define BSON_HAVE_STDBOOL_H 1
#if BSON_HAVE_STDBOOL_H != 1
# undef BSON_HAVE_STDBOOL_H
#endif


/*
 * Define to 1 for POSIX-like systems, 2 for Windows.
 */
#define BSON_OS 1


/*
 * Define to 1 if we have access to GCC 32-bit atomic builtins.
 * While this requires GCC 4.1+ in most cases, it is also architecture
 * dependent. For example, some PPC or ARM systems may not have it even
 * if it is a recent GCC version.
 */
#define BSON_HAVE_ATOMIC_32_ADD_AND_FETCH 1
#if BSON_HAVE_ATOMIC_32_ADD_AND_FETCH != 1
# undef BSON_HAVE_ATOMIC_32_ADD_AND_FETCH
#endif

/*
 * Similarly, define to 1 if we have access to GCC 64-bit atomic builtins.
 */
#define BSON_HAVE_ATOMIC_64_ADD_AND_FETCH 1
#if BSON_HAVE_ATOMIC_64_ADD_AND_FETCH != 1
# undef BSON_HAVE_ATOMIC_64_ADD_AND_FETCH
#endif


/*
 * Define to 1 if your system requires {} around PTHREAD_ONCE_INIT.
 * This is typically just Solaris 8-10.
 */
#define BSON_PTHREAD_ONCE_INIT_NEEDS_BRACES 0
#if BSON_PTHREAD_ONCE_INIT_NEEDS_BRACES != 1
# undef BSON_PTHREAD_ONCE_INIT_NEEDS_BRACES
#endif


/*
 * Define to 1 if you have clock_gettime() available.
 */
#define BSON_HAVE_CLOCK_GETTIME 1
#if BSON_HAVE_CLOCK_GETTIME != 1
# undef BSON_HAVE_CLOCK_GETTIME
#endif


/*
 * Define to 1 if you have strnlen available on your platform.
 */
#define BSON_HAVE_STRNLEN 1
#if BSON_HAVE_STRNLEN != 1
# undef BSON_HAVE_STRNLEN
#endif


/*
 * Define to 1 if you have snprintf available on your platform.
 */
#define BSON_HAVE_SNPRINTF 1
#if BSON_HAVE_SNPRINTF != 1
# undef BSON_HAVE_SNPRINTF
#endif


/*
 * Define to 1 if you have _set_output_format (VS2013 and older).
 */
#define BSON_NEEDS_SET_OUTPUT_FORMAT 0
#if BSON_NEEDS_SET_OUTPUT_FORMAT != 1
# undef BSON_NEEDS_SET_OUTPUT_FORMAT
#endif

/*
 * Define to 1 if you have struct timespec available on your platform.
 */
#define BSON_HAVE_TIMESPEC 1
#if BSON_HAVE_TIMESPEC != 1
# undef BSON_HAVE_TIMESPEC
#endif


/*
 * Define to 1 if you want extra aligned types in libbson
 */
#define BSON_EXTRA_ALIGN 1
#if BSON_EXTRA_ALIGN != 1
# undef BSON_EXTRA_ALIGN
#endif


#endif /* BSON_CONFIG_H */

// #include "bson-macros.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_MACROS_H
#define BSON_MACROS_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
#  error "Only <bson.h> can be included directly."
#endif


#include <assert.h>
#include <stdio.h>

#ifdef __cplusplus
#  include <algorithm>
#endif

// #include "bson-config.h"



#if BSON_OS == 1
# define BSON_OS_UNIX
#elif BSON_OS == 2
# define BSON_OS_WIN32
#else
# error "Unknown operating system."
#endif


#ifdef __cplusplus
#  define BSON_BEGIN_DECLS extern "C" {
#  define BSON_END_DECLS   }
#else
#  define BSON_BEGIN_DECLS
#  define BSON_END_DECLS
#endif


#define BSON_GNUC_CHECK_VERSION(major, minor) \
    (defined(__GNUC__) && \
     ((__GNUC__ > (major)) || \
      ((__GNUC__ == (major)) && \
       (__GNUC_MINOR__ >= (minor)))))


#define BSON_GNUC_IS_VERSION(major, minor) \
    (defined(__GNUC__) && \
     (__GNUC__ == (major)) && \
     (__GNUC_MINOR__ == (minor)))


#ifdef _MSC_VER
#  ifdef BSON_COMPILATION
#    define BSON_API __declspec(dllexport)
#  else
#    define BSON_API __declspec(dllimport)
#  endif
#else
#  define BSON_API
#endif


#ifdef MIN
#  define BSON_MIN MIN
#elif defined(__cplusplus)
#  define BSON_MIN(a, b) ( (std::min)(a, b) )
#elif defined(_MSC_VER)
#  define BSON_MIN(a, b) ((a) < (b) ? (a) : (b))
#else
#  define BSON_MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif


#ifdef MAX
#  define BSON_MAX MAX
#elif defined(__cplusplus)
#  define BSON_MAX(a, b) ( (std::max)(a, b) )
#elif defined(_MSC_VER)
#  define BSON_MAX(a, b) ((a) > (b) ? (a) : (b))
#else
#  define BSON_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif


#ifdef ABS
#  define BSON_ABS ABS
#else
#  define BSON_ABS(a) (((a) < 0) ? ((a) * -1) : (a))
#endif

#ifdef _MSC_VER
#  ifdef _WIN64
#    define BSON_ALIGN_OF_PTR 8
#  else
#    define BSON_ALIGN_OF_PTR 4
#  endif
#else
#  define BSON_ALIGN_OF_PTR (sizeof(void *))
#endif

#ifdef BSON_EXTRA_ALIGN
#  if defined(_MSC_VER)
#    define BSON_ALIGNED_BEGIN(_N) __declspec (align (_N))
#    define BSON_ALIGNED_END(_N)
#  else
#    define BSON_ALIGNED_BEGIN(_N)
#    define BSON_ALIGNED_END(_N) __attribute__((aligned (_N)))
#  endif
#else
#  if defined(_MSC_VER)
#    define BSON_ALIGNED_BEGIN(_N) __declspec (align ((_N) > BSON_ALIGN_OF_PTR ? BSON_ALIGN_OF_PTR : (_N) ))
#    define BSON_ALIGNED_END(_N)
#  else
#    define BSON_ALIGNED_BEGIN(_N)
#    define BSON_ALIGNED_END(_N) __attribute__((aligned ((_N) > BSON_ALIGN_OF_PTR ? BSON_ALIGN_OF_PTR : (_N) )))
#  endif
#endif


#define bson_str_empty(s)  (!s[0])
#define bson_str_empty0(s) (!s || !s[0])


#if defined(_WIN32)
#  define BSON_FUNC __FUNCTION__
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ < 199901L
#  define BSON_FUNC __FUNCTION__
#else
#  define BSON_FUNC __func__
#endif

#define BSON_ASSERT(test) \
   do { \
      if (!(BSON_LIKELY(test))) { \
         fprintf (stderr, "%s:%d %s(): precondition failed: %s\n", \
                  __FILE__, __LINE__, BSON_FUNC, #test); \
         abort (); \
      } \
   } while (0)


#define BSON_STATIC_ASSERT(s) BSON_STATIC_ASSERT_ (s, __LINE__)
#define BSON_STATIC_ASSERT_JOIN(a, b) BSON_STATIC_ASSERT_JOIN2 (a, b)
#define BSON_STATIC_ASSERT_JOIN2(a, b) a##b
#define BSON_STATIC_ASSERT_(s, l) \
   typedef char BSON_STATIC_ASSERT_JOIN (static_assert_test_, \
                                         __LINE__)[(s) ? 1 : -1]


#if defined(__GNUC__)
#  define BSON_GNUC_CONST __attribute__((const))
#  define BSON_GNUC_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#  define BSON_GNUC_CONST
#  define BSON_GNUC_WARN_UNUSED_RESULT
#endif


#if BSON_GNUC_CHECK_VERSION(4, 0) && !defined(_WIN32)
#  define BSON_GNUC_NULL_TERMINATED __attribute__((sentinel))
#  define BSON_GNUC_INTERNAL __attribute__((visibility ("hidden")))
#else
#  define BSON_GNUC_NULL_TERMINATED
#  define BSON_GNUC_INTERNAL
#endif


#if defined(__GNUC__)
#  define BSON_LIKELY(x)    __builtin_expect (!!(x), 1)
#  define BSON_UNLIKELY(x)  __builtin_expect (!!(x), 0)
#else
#  define BSON_LIKELY(v)   v
#  define BSON_UNLIKELY(v) v
#endif


#if defined(__clang__)
#  define BSON_GNUC_PRINTF(f, v) __attribute__((format (printf, f, v)))
#elif BSON_GNUC_CHECK_VERSION(4, 4)
#  define BSON_GNUC_PRINTF(f, v) __attribute__((format (gnu_printf, f, v)))
#else
#  define BSON_GNUC_PRINTF(f, v)
#endif


#if defined(__LP64__) || defined(_LP64)
#  define BSON_WORD_SIZE 64
#else
#  define BSON_WORD_SIZE 32
#endif


#if defined(_MSC_VER)
#  define BSON_INLINE __inline
#else
#  define BSON_INLINE __inline__
#endif


#ifdef _MSC_VER
#  define BSON_ENSURE_ARRAY_PARAM_SIZE(_n)
#  define BSON_TYPEOF decltype
#else
#  define BSON_ENSURE_ARRAY_PARAM_SIZE(_n) static (_n)
#  define BSON_TYPEOF typeof
#endif


#if BSON_GNUC_CHECK_VERSION(3, 1)
# define BSON_GNUC_DEPRECATED __attribute__((__deprecated__))
#else
# define BSON_GNUC_DEPRECATED
#endif


#if BSON_GNUC_CHECK_VERSION(4, 5)
# define BSON_GNUC_DEPRECATED_FOR(f) __attribute__((deprecated("Use " #f " instead")))
#else
# define BSON_GNUC_DEPRECATED_FOR(f) BSON_GNUC_DEPRECATED
#endif


#endif /* BSON_MACROS_H */



#ifdef BSON_OS_WIN32
# if defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#  undef _WIN32_WINNT
# endif
# ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0600
# endif
# ifndef NOMINMAX
#  define NOMINMAX
# endif
# include <winsock2.h>
#  ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   undef  WIN32_LEAN_AND_MEAN
#  else
#   include <windows.h>
# endif
#include <direct.h>
#include <io.h>
#endif


#ifdef BSON_OS_UNIX
# include <unistd.h>
# include <sys/time.h>
#endif


// #include "bson-macros.h"



#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


BSON_BEGIN_DECLS


#ifdef _MSC_VER
// # include "bson-stdint-win32.h"
// ISO C9x  compliant stdint.h for Microsoft Visual Studio
// Based on ISO/IEC 9899:TC2 Committee draft (May 6, 2005) WG14/N1124
//
//  Copyright (c) 2006-2013 Alexander Chemeris
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   1. Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//   3. Neither the name of the product nor the names of its contributors may
//      be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
// EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MSC_VER // [
#error "Use this header only with Microsoft Visual C++ compilers!"
#endif // _MSC_VER ]

#ifndef _MSC_STDINT_H_ // [
#define _MSC_STDINT_H_

#if _MSC_VER > 1000

#endif

#if _MSC_VER >= 1600 // [
#include <stdint.h>
#else // ] _MSC_VER >= 1600 [

#include <limits.h>

// For Visual Studio 6 in C++ mode and for many Visual Studio versions when
// compiling for ARM we should wrap <wchar.h> include with 'extern "C++" {}'
// or compiler give many errors like this:
//   error C2733: second C linkage of overloaded function 'wmemchr' not allowed
#ifdef __cplusplus
extern "C" {
#endif
#  include <wchar.h>
#ifdef __cplusplus
}
#endif

// Define _W64 macros to mark types changing their size, like intptr_t.
#ifndef _W64
#  if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#     define _W64 __w64
#  else
#     define _W64
#  endif
#endif


// 7.18.1 Integer types

// 7.18.1.1 Exact-width integer types

// Visual Studio 6 and Embedded Visual C++ 4 doesn't
// realize that, e.g. char has the same size as __int8
// so we give up on __intX for them.
#if (_MSC_VER < 1300)
   typedef signed char       int8_t;
   typedef signed short      int16_t;
   typedef signed int        int32_t;
   typedef unsigned char     uint8_t;
   typedef unsigned short    uint16_t;
   typedef unsigned int      uint32_t;
#else
   typedef signed __int8     int8_t;
   typedef signed __int16    int16_t;
   typedef signed __int32    int32_t;
   typedef unsigned __int8   uint8_t;
   typedef unsigned __int16  uint16_t;
   typedef unsigned __int32  uint32_t;
#endif
typedef signed __int64       int64_t;
typedef unsigned __int64     uint64_t;


// 7.18.1.2 Minimum-width integer types
typedef int8_t    int_least8_t;
typedef int16_t   int_least16_t;
typedef int32_t   int_least32_t;
typedef int64_t   int_least64_t;
typedef uint8_t   uint_least8_t;
typedef uint16_t  uint_least16_t;
typedef uint32_t  uint_least32_t;
typedef uint64_t  uint_least64_t;

// 7.18.1.3 Fastest minimum-width integer types
typedef int8_t    int_fast8_t;
typedef int16_t   int_fast16_t;
typedef int32_t   int_fast32_t;
typedef int64_t   int_fast64_t;
typedef uint8_t   uint_fast8_t;
typedef uint16_t  uint_fast16_t;
typedef uint32_t  uint_fast32_t;
typedef uint64_t  uint_fast64_t;

// 7.18.1.4 Integer types capable of holding object pointers
#ifdef _WIN64 // [
   typedef signed __int64    intptr_t;
   typedef unsigned __int64  uintptr_t;
#else // _WIN64 ][
   typedef _W64 signed int   intptr_t;
   typedef _W64 unsigned int uintptr_t;
#endif // _WIN64 ]

// 7.18.1.5 Greatest-width integer types
typedef int64_t   intmax_t;
typedef uint64_t  uintmax_t;


// 7.18.2 Limits of specified-width integer types

#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS) // [   See footnote 220 at page 257 and footnote 221 at page 259

// 7.18.2.1 Limits of exact-width integer types
#define INT8_MIN     ((int8_t)_I8_MIN)
#define INT8_MAX     _I8_MAX
#define INT16_MIN    ((int16_t)_I16_MIN)
#define INT16_MAX    _I16_MAX
#define INT32_MIN    ((int32_t)_I32_MIN)
#define INT32_MAX    _I32_MAX
#define INT64_MIN    ((int64_t)_I64_MIN)
#define INT64_MAX    _I64_MAX
#define UINT8_MAX    _UI8_MAX
#define UINT16_MAX   _UI16_MAX
#define UINT32_MAX   _UI32_MAX
#define UINT64_MAX   _UI64_MAX

// 7.18.2.2 Limits of minimum-width integer types
#define INT_LEAST8_MIN    INT8_MIN
#define INT_LEAST8_MAX    INT8_MAX
#define INT_LEAST16_MIN   INT16_MIN
#define INT_LEAST16_MAX   INT16_MAX
#define INT_LEAST32_MIN   INT32_MIN
#define INT_LEAST32_MAX   INT32_MAX
#define INT_LEAST64_MIN   INT64_MIN
#define INT_LEAST64_MAX   INT64_MAX
#define UINT_LEAST8_MAX   UINT8_MAX
#define UINT_LEAST16_MAX  UINT16_MAX
#define UINT_LEAST32_MAX  UINT32_MAX
#define UINT_LEAST64_MAX  UINT64_MAX

// 7.18.2.3 Limits of fastest minimum-width integer types
#define INT_FAST8_MIN    INT8_MIN
#define INT_FAST8_MAX    INT8_MAX
#define INT_FAST16_MIN   INT16_MIN
#define INT_FAST16_MAX   INT16_MAX
#define INT_FAST32_MIN   INT32_MIN
#define INT_FAST32_MAX   INT32_MAX
#define INT_FAST64_MIN   INT64_MIN
#define INT_FAST64_MAX   INT64_MAX
#define UINT_FAST8_MAX   UINT8_MAX
#define UINT_FAST16_MAX  UINT16_MAX
#define UINT_FAST32_MAX  UINT32_MAX
#define UINT_FAST64_MAX  UINT64_MAX

// 7.18.2.4 Limits of integer types capable of holding object pointers
#ifdef _WIN64 // [
#  define INTPTR_MIN   INT64_MIN
#  define INTPTR_MAX   INT64_MAX
#  define UINTPTR_MAX  UINT64_MAX
#else // _WIN64 ][
#  define INTPTR_MIN   INT32_MIN
#  define INTPTR_MAX   INT32_MAX
#  define UINTPTR_MAX  UINT32_MAX
#endif // _WIN64 ]

// 7.18.2.5 Limits of greatest-width integer types
#define INTMAX_MIN   INT64_MIN
#define INTMAX_MAX   INT64_MAX
#define UINTMAX_MAX  UINT64_MAX

// 7.18.3 Limits of other integer types

#ifdef _WIN64 // [
#  define PTRDIFF_MIN  _I64_MIN
#  define PTRDIFF_MAX  _I64_MAX
#else  // _WIN64 ][
#  define PTRDIFF_MIN  _I32_MIN
#  define PTRDIFF_MAX  _I32_MAX
#endif  // _WIN64 ]

#define SIG_ATOMIC_MIN  INT_MIN
#define SIG_ATOMIC_MAX  INT_MAX

#ifndef SIZE_MAX // [
#  ifdef _WIN64 // [
#     define SIZE_MAX  _UI64_MAX
#  else // _WIN64 ][
#     define SIZE_MAX  _UI32_MAX
#  endif // _WIN64 ]
#endif // SIZE_MAX ]

// WCHAR_MIN and WCHAR_MAX are also defined in <wchar.h>
#ifndef WCHAR_MIN // [
#  define WCHAR_MIN  0
#endif  // WCHAR_MIN ]
#ifndef WCHAR_MAX // [
#  define WCHAR_MAX  _UI16_MAX
#endif  // WCHAR_MAX ]

#define WINT_MIN  0
#define WINT_MAX  _UI16_MAX

#endif // __STDC_LIMIT_MACROS ]


// 7.18.4 Limits of other integer types

#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS) // [   See footnote 224 at page 260

// 7.18.4.1 Macros for minimum-width integer constants

#define INT8_C(val)  val##i8
#define INT16_C(val) val##i16
#define INT32_C(val) val##i32
#define INT64_C(val) val##i64

#define UINT8_C(val)  val##ui8
#define UINT16_C(val) val##ui16
#define UINT32_C(val) val##ui32
#define UINT64_C(val) val##ui64

// 7.18.4.2 Macros for greatest-width integer constants
// These #ifndef's are needed to prevent collisions with <boost/cstdint.hpp>.
// Check out Issue 9 for the details.
#ifndef INTMAX_C //   [
#  define INTMAX_C   INT64_C
#endif // INTMAX_C    ]
#ifndef UINTMAX_C //  [
#  define UINTMAX_C  UINT64_C
#endif // UINTMAX_C   ]

#endif // __STDC_CONSTANT_MACROS ]

#endif // _MSC_VER >= 1600 ]

#endif // _MSC_STDINT_H_ ]

# ifndef __cplusplus
   /* benign redefinition of type */
#  pragma warning (disable :4142)
#  ifndef _SSIZE_T_DEFINED
#   define _SSIZE_T_DEFINED
    typedef SSIZE_T ssize_t;
#  endif
    typedef SIZE_T size_t;
#  pragma warning (default :4142)
# else
    /*
     * MSVC++ does not include ssize_t, just size_t.
     * So we need to synthesize that as well.
     */
#  pragma warning (disable :4142)
#  ifndef _SSIZE_T_DEFINED
#   define _SSIZE_T_DEFINED
    typedef SSIZE_T ssize_t;
#  endif
#  pragma warning (default :4142)
# endif
# define PRIi32 "d"
# define PRId32 "d"
# define PRIu32 "u"
# define PRIi64 "I64i"
# define PRId64 "I64i"
# define PRIu64 "I64u"
#else
// # include "bson-stdint.h"
#ifndef _SRC_BSON_BSON_STDINT_H
#define _SRC_BSON_BSON_STDINT_H 1
#ifndef _GENERATED_STDINT_H
#define _GENERATED_STDINT_H " "
/* generated using a gnu compiler version gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-4) Copyright (C) 2015 Free Software Foundation, Inc. This is free software; see the source for copying conditions. There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. */

#include <stdint.h>


/* system headers have good uint64_t */
#ifndef _HAVE_UINT64_T
#define _HAVE_UINT64_T
#endif

  /* once */
#endif
#endif

# include <inttypes.h>
#endif

#if defined(__MINGW32__) && ! defined(INIT_ONCE_STATIC_INIT)
# define INIT_ONCE_STATIC_INIT RTL_RUN_ONCE_INIT
typedef RTL_RUN_ONCE INIT_ONCE;
#endif

#ifdef BSON_HAVE_STDBOOL_H
# include <stdbool.h>
#elif !defined(__bool_true_false_are_defined)
# ifndef __cplusplus
   typedef signed char bool;
#  define false 0
#  define true 1
# endif
# define __bool_true_false_are_defined 1
#endif


#if defined(__GNUC__)
# if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)
#  define bson_sync_synchronize() __sync_synchronize()
# elif defined(__i386__ ) || defined( __i486__ ) || defined( __i586__ ) || \
          defined( __i686__ ) || defined( __x86_64__ )
#  define bson_sync_synchronize() asm volatile("mfence":::"memory")
# else
#  define bson_sync_synchronize() asm volatile("sync":::"memory")
# endif
#elif defined(_MSC_VER)
# define bson_sync_synchronize() MemoryBarrier()
#endif


#if !defined(va_copy) && defined(_MSC_VER)
# define va_copy(dst,src) ((dst) = (src))
#endif


#if !defined(va_copy) && defined(__GNUC__) && __GNUC__ < 3
# define va_copy(dst,src) __va_copy(dst, src)
#endif


BSON_END_DECLS


#endif /* BSON_COMPAT_H */


// #include <yajl/yajl_common.h>


#ifndef __YAJL_PARSE_H__
#define __YAJL_PARSE_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    /** error codes returned from this interface */
    typedef enum {
        /** no error was encountered */
        yajl_status_ok,
        /** a client callback returned zero, stopping the parse */
        yajl_status_client_canceled,
        /** An error occured during the parse.  Call yajl_get_error for
         *  more information about the encountered error */
        yajl_status_error
    } yajl_status;

    /** attain a human readable, english, string for an error */
    YAJL_API const char * yajl_status_to_string(yajl_status code);

    /** an opaque handle to a parser */
    typedef struct yajl_handle_t * yajl_handle;

    /** yajl is an event driven parser.  this means as json elements are
     *  parsed, you are called back to do something with the data.  The
     *  functions in this table indicate the various events for which
     *  you will be called back.  Each callback accepts a "context"
     *  pointer, this is a void * that is passed into the yajl_parse
     *  function which the client code may use to pass around context.
     *
     *  All callbacks return an integer.  If non-zero, the parse will
     *  continue.  If zero, the parse will be canceled and
     *  yajl_status_client_canceled will be returned from the parse.
     *
     *  \attention {
     *    A note about the handling of numbers:
     *
     *    yajl will only convert numbers that can be represented in a
     *    double or a 64 bit (long long) int.  All other numbers will
     *    be passed to the client in string form using the yajl_number
     *    callback.  Furthermore, if yajl_number is not NULL, it will
     *    always be used to return numbers, that is yajl_integer and
     *    yajl_double will be ignored.  If yajl_number is NULL but one
     *    of yajl_integer or yajl_double are defined, parsing of a
     *    number larger than is representable in a double or 64 bit
     *    integer will result in a parse error.
     *  }
     */
    typedef struct {
        int (* yajl_null)(void * ctx);
        int (* yajl_boolean)(void * ctx, int boolVal);
        int (* yajl_integer)(void * ctx, int64_t integerVal);
        int (* yajl_double)(void * ctx, double doubleVal);
        /** A callback which passes the string representation of the number
         *  back to the client.  Will be used for all numbers when present */
        int (* yajl_number)(void * ctx, const char * numberVal,
                            size_t numberLen);

        /** strings are returned as pointers into the JSON text when,
         * possible, as a result, they are _not_ null padded */
        int (* yajl_string)(void * ctx, const unsigned char * stringVal,
                            size_t stringLen);

        int (* yajl_start_map)(void * ctx);
        int (* yajl_map_key)(void * ctx, const unsigned char * key,
                             size_t stringLen);
        int (* yajl_end_map)(void * ctx);

        int (* yajl_start_array)(void * ctx);
        int (* yajl_end_array)(void * ctx);
    } yajl_callbacks;

    /** allocate a parser handle
     *  \param callbacks  a yajl callbacks structure specifying the
     *                    functions to call when different JSON entities
     *                    are encountered in the input text.  May be NULL,
     *                    which is only useful for validation.
     *  \param afs        memory allocation functions, may be NULL for to use
     *                    C runtime library routines (malloc and friends)
     *  \param ctx        a context pointer that will be passed to callbacks.
     */
    YAJL_API yajl_handle yajl_alloc(const yajl_callbacks * callbacks,
                                    yajl_alloc_funcs * afs,
                                    void * ctx);


    /** configuration parameters for the parser, these may be passed to
     *  yajl_config() along with option specific argument(s).  In general,
     *  all configuration parameters default to *off*. */
    typedef enum {
        /** Ignore javascript style comments present in
         *  JSON input.  Non-standard, but rather fun
         *  arguments: toggled off with integer zero, on otherwise.
         *
         *  example:
         *    yajl_config(h, yajl_allow_comments, 1); // turn comment support on
         */
        yajl_allow_comments = 0x01,
        /**
         * When set the parser will verify that all strings in JSON input are
         * valid UTF8 and will emit a parse error if this is not so.  When set,
         * this option makes parsing slightly more expensive (~7% depending
         * on processor and compiler in use)
         *
         * example:
         *   yajl_config(h, yajl_dont_validate_strings, 1); // disable utf8 checking
         */
        yajl_dont_validate_strings     = 0x02,
        /**
         * By default, upon calls to yajl_complete_parse(), yajl will
         * ensure the entire input text was consumed and will raise an error
         * otherwise.  Enabling this flag will cause yajl to disable this
         * check.  This can be useful when parsing json out of a that contains more
         * than a single JSON document.
         */
        yajl_allow_trailing_garbage = 0x04,
        /**
         * Allow multiple values to be parsed by a single handle.  The
         * entire text must be valid JSON, and values can be seperated
         * by any kind of whitespace.  This flag will change the
         * behavior of the parser, and cause it continue parsing after
         * a value is parsed, rather than transitioning into a
         * complete state.  This option can be useful when parsing multiple
         * values from an input stream.
         */
        yajl_allow_multiple_values = 0x08,
        /**
         * When yajl_complete_parse() is called the parser will
         * check that the top level value was completely consumed.  I.E.,
         * if called whilst in the middle of parsing a value
         * yajl will enter an error state (premature EOF).  Setting this
         * flag suppresses that check and the corresponding error.
         */
        yajl_allow_partial_values = 0x10
    } yajl_option;

    /** allow the modification of parser options subsequent to handle
     *  allocation (via yajl_alloc)
     *  \returns zero in case of errors, non-zero otherwise
     */
    YAJL_API int yajl_config(yajl_handle h, yajl_option opt, ...);

    /** free a parser handle */
    YAJL_API void yajl_free(yajl_handle handle);

    /** Parse some json!
     *  \param hand - a handle to the json parser allocated with yajl_alloc
     *  \param jsonText - a pointer to the UTF8 json text to be parsed
     *  \param jsonTextLength - the length, in bytes, of input text
     */
    YAJL_API yajl_status yajl_parse(yajl_handle hand,
                                    const unsigned char * jsonText,
                                    size_t jsonTextLength);

    /** Parse any remaining buffered json.
     *  Since yajl is a stream-based parser, without an explicit end of
     *  input, yajl sometimes can't decide if content at the end of the
     *  stream is valid or not.  For example, if "1" has been fed in,
     *  yajl can't know whether another digit is next or some character
     *  that would terminate the integer token.
     *
     *  \param hand - a handle to the json parser allocated with yajl_alloc
     */
    YAJL_API yajl_status yajl_complete_parse(yajl_handle hand);

    /** get an error string describing the state of the
     *  parse.
     *
     *  If verbose is non-zero, the message will include the JSON
     *  text where the error occured, along with an arrow pointing to
     *  the specific char.
     *
     *  \returns A dynamically allocated string will be returned which should
     *  be freed with yajl_free_error
     */
    YAJL_API unsigned char * yajl_get_error(yajl_handle hand, int verbose,
                                            const unsigned char * jsonText,
                                            size_t jsonTextLength);

    /**
     * get the amount of data consumed from the last chunk passed to YAJL.
     *
     * In the case of a successful parse this can help you understand if
     * the entire buffer was consumed (which will allow you to handle
     * "junk at end of input").
     *
     * In the event an error is encountered during parsing, this function
     * affords the client a way to get the offset into the most recent
     * chunk where the error occured.  0 will be returned if no error
     * was encountered.
     */
    YAJL_API size_t yajl_get_bytes_consumed(yajl_handle hand);

    /** free an error returned from yajl_get_error */
    YAJL_API void yajl_free_error(yajl_handle hand, unsigned char * str);

#ifdef __cplusplus
}
#endif

#endif

// #include "yajl_lex.h"
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __YAJL_LEX_H__
#define __YAJL_LEX_H__

// #include "yajl_common.h"


typedef enum {
    yajl_tok_bool,         
    yajl_tok_colon,
    yajl_tok_comma,     
    yajl_tok_eof,
    yajl_tok_error,
    yajl_tok_left_brace,     
    yajl_tok_left_bracket,
    yajl_tok_null,         
    yajl_tok_right_brace,     
    yajl_tok_right_bracket,

    /* we differentiate between integers and doubles to allow the
     * parser to interpret the number without re-scanning */
    yajl_tok_integer, 
    yajl_tok_double, 

    /* we differentiate between strings which require further processing,
     * and strings that do not */
    yajl_tok_string,
    yajl_tok_string_with_escapes,

    /* comment tokens are not currently returned to the parser, ever */
    yajl_tok_comment
} yajl_tok;

typedef struct yajl_lexer_t * yajl_lexer;

yajl_lexer yajl_lex_alloc(yajl_alloc_funcs * alloc,
                          unsigned int allowComments,
                          unsigned int validateUTF8);

void yajl_lex_free(yajl_lexer lexer);

/**
 * run/continue a lex. "offset" is an input/output parameter.
 * It should be initialized to zero for a
 * new chunk of target text, and upon subsetquent calls with the same
 * target text should passed with the value of the previous invocation.
 *
 * the client may be interested in the value of offset when an error is
 * returned from the lexer.  This allows the client to render useful
n * error messages.
 *
 * When you pass the next chunk of data, context should be reinitialized
 * to zero.
 * 
 * Finally, the output buffer is usually just a pointer into the jsonText,
 * however in cases where the entity being lexed spans multiple chunks,
 * the lexer will buffer the entity and the data returned will be
 * a pointer into that buffer.
 *
 * This behavior is abstracted from client code except for the performance
 * implications which require that the client choose a reasonable chunk
 * size to get adequate performance.
 */
yajl_tok yajl_lex_lex(yajl_lexer lexer, const unsigned char * jsonText,
                      size_t jsonTextLen, size_t * offset,
                      const unsigned char ** outBuf, size_t * outLen);

/** have a peek at the next token, but don't move the lexer forward */
yajl_tok yajl_lex_peek(yajl_lexer lexer, const unsigned char * jsonText,
                       size_t jsonTextLen, size_t offset);


typedef enum {
    yajl_lex_e_ok = 0,
    yajl_lex_string_invalid_utf8,
    yajl_lex_string_invalid_escaped_char,
    yajl_lex_string_invalid_json_char,
    yajl_lex_string_invalid_hex_char,
    yajl_lex_invalid_char,
    yajl_lex_invalid_string,
    yajl_lex_missing_integer_after_decimal,
    yajl_lex_missing_integer_after_exponent,
    yajl_lex_missing_integer_after_minus,
    yajl_lex_unallowed_comment
} yajl_lex_error;

const char * yajl_lex_error_to_string(yajl_lex_error error);

/** allows access to more specific information about the lexical
 *  error when yajl_lex_lex returns yajl_tok_error. */
yajl_lex_error yajl_lex_get_error(yajl_lexer lexer);

/** get the current offset into the most recently lexed json string. */
size_t yajl_lex_current_offset(yajl_lexer lexer);

/** get the number of lines lexed by this lexer instance */
size_t yajl_lex_current_line(yajl_lexer lexer);

/** get the number of chars lexed by this lexer instance since the last
 *  \n or \r */
size_t yajl_lex_current_char(yajl_lexer lexer);

#endif

// #include "yajl_parser.h"
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __YAJL_PARSER_H__
#define __YAJL_PARSER_H__

// #include "bson-compat.h"


// #include "yajl_parse.h"

// #include "yajl_bytestack.h"
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * A header only implementation of a simple stack of bytes, used in YAJL
 * to maintain parse state.
 */

#ifndef __YAJL_BYTESTACK_H__
#define __YAJL_BYTESTACK_H__

// #include "yajl_common.h"


#define YAJL_BS_INC 128

typedef struct yajl_bytestack_t
{
    unsigned char * stack;
    size_t size;
    size_t used;
    yajl_alloc_funcs * yaf;
} yajl_bytestack;

/* initialize a bytestack */
#define yajl_bs_init(obs, _yaf) {               \
        (obs).stack = NULL;                     \
        (obs).size = 0;                         \
        (obs).used = 0;                         \
        (obs).yaf = (_yaf);                     \
    }                                           \


/* initialize a bytestack */
#define yajl_bs_free(obs)                 \
    if ((obs).stack) (obs).yaf->free((obs).yaf->ctx, (obs).stack);

#define yajl_bs_current(obs)               \
    (assert((obs).used > 0), (obs).stack[(obs).used - 1])

#define yajl_bs_push(obs, byte) {                       \
    if (((obs).size - (obs).used) == 0) {               \
        (obs).size += YAJL_BS_INC;                      \
        (obs).stack = (obs).yaf->realloc((obs).yaf->ctx,\
                                         (void *) (obs).stack, (obs).size);\
    }                                                   \
    (obs).stack[((obs).used)++] = (byte);               \
}

/* removes the top item of the stack, returns nothing */
#define yajl_bs_pop(obs) { ((obs).used)--; }

#define yajl_bs_set(obs, byte)                          \
    (obs).stack[((obs).used) - 1] = (byte);


#endif

// #include "yajl_buf.h"

// #include "yajl_lex.h"



typedef enum {
    yajl_state_start = 0,
    yajl_state_parse_complete,
    yajl_state_parse_error,
    yajl_state_lexical_error,
    yajl_state_map_start,
    yajl_state_map_sep,
    yajl_state_map_need_val,
    yajl_state_map_got_val,
    yajl_state_map_need_key,
    yajl_state_array_start,
    yajl_state_array_got_val,
    yajl_state_array_need_val,
    yajl_state_got_value,
} yajl_state;

struct yajl_handle_t {
    const yajl_callbacks * callbacks;
    void * ctx;
    yajl_lexer lexer;
    const char * parseError;
    /* the number of bytes consumed from the last client buffer,
     * in the case of an error this will be an error offset, in the
     * case of an error this can be used as the error offset */
    size_t bytesConsumed;
    /* temporary storage for decoded strings */
    yajl_buf decodeBuf;
    /* a stack of states.  access with yajl_state_XXX routines */
    yajl_bytestack stateStack;
    /* memory allocation routines */
    yajl_alloc_funcs alloc;
    /* bitfield */
    unsigned int flags;
};

yajl_status
yajl_do_parse(yajl_handle handle, const unsigned char * jsonText,
              size_t jsonTextLen);

yajl_status
yajl_do_finish(yajl_handle handle);

unsigned char *
yajl_render_error_string(yajl_handle hand, const unsigned char * jsonText,
                         size_t jsonTextLen, int verbose);

/* A little built in integer parsing routine with the same semantics as strtol
 * that's unaffected by LOCALE. */
int64_t
yajl_parse_integer(const unsigned char *number, unsigned int length);


#endif

// #include "yajl_alloc.h"


#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

const char *
yajl_status_to_string(yajl_status stat)
{
    const char * statStr = "unknown";
    switch (stat) {
        case yajl_status_ok:
            statStr = "ok, no error";
            break;
        case yajl_status_client_canceled:
            statStr = "client canceled parse";
            break;
        case yajl_status_error:
            statStr = "parse error";
            break;
    }
    return statStr;
}

yajl_handle
yajl_alloc(const yajl_callbacks * callbacks,
           yajl_alloc_funcs * afs,
           void * ctx)
{
    yajl_handle hand = NULL;
    yajl_alloc_funcs afsBuffer;

    /* first order of business is to set up memory allocation routines */
    if (afs != NULL) {
        if (afs->malloc == NULL || afs->realloc == NULL || afs->free == NULL)
        {
            return NULL;
        }
    } else {
        yajl_set_default_alloc_funcs(&afsBuffer);
        afs = &afsBuffer;
    }

    hand = (yajl_handle) YA_MALLOC(afs, sizeof(struct yajl_handle_t));

    /* copy in pointers to allocation routines */
    memcpy((void *) &(hand->alloc), (void *) afs, sizeof(yajl_alloc_funcs));

    hand->callbacks = callbacks;
    hand->ctx = ctx;
    hand->lexer = NULL; 
    hand->bytesConsumed = 0;
    hand->decodeBuf = yajl_buf_alloc(&(hand->alloc));
    hand->flags	    = 0;
    yajl_bs_init(hand->stateStack, &(hand->alloc));
    yajl_bs_push(hand->stateStack, yajl_state_start);

    return hand;
}

int
yajl_config(yajl_handle h, yajl_option opt, ...)
{
    int rv = 1;
    va_list ap;
    va_start(ap, opt);

    switch(opt) {
        case yajl_allow_comments:
        case yajl_dont_validate_strings:
        case yajl_allow_trailing_garbage:
        case yajl_allow_multiple_values:
        case yajl_allow_partial_values:
            if (va_arg(ap, int)) h->flags |= opt;
            else h->flags &= ~opt;
            break;
        default:
            rv = 0;
    }
    va_end(ap);

    return rv;
}

void
yajl_free(yajl_handle handle)
{
    yajl_bs_free(handle->stateStack);
    yajl_buf_free(handle->decodeBuf);
    if (handle->lexer) {
        yajl_lex_free(handle->lexer);
        handle->lexer = NULL;
    }
    YA_FREE(&(handle->alloc), handle);
}

yajl_status
yajl_parse(yajl_handle hand, const unsigned char * jsonText,
           size_t jsonTextLen)
{
    yajl_status status;

    /* lazy allocation of the lexer */
    if (hand->lexer == NULL) {
        hand->lexer = yajl_lex_alloc(&(hand->alloc),
                                     hand->flags & yajl_allow_comments,
                                     !(hand->flags & yajl_dont_validate_strings));
    }

    status = yajl_do_parse(hand, jsonText, jsonTextLen);
    return status;
}


yajl_status
yajl_complete_parse(yajl_handle hand)
{
    /* The lexer is lazy allocated in the first call to parse.  if parse is
     * never called, then no data was provided to parse at all.  This is a
     * "premature EOF" error unless yajl_allow_partial_values is specified.
     * allocating the lexer now is the simplest possible way to handle this
     * case while preserving all the other semantics of the parser
     * (multiple values, partial values, etc). */
    if (hand->lexer == NULL) {
        hand->lexer = yajl_lex_alloc(&(hand->alloc),
                                     hand->flags & yajl_allow_comments,
                                     !(hand->flags & yajl_dont_validate_strings));
    }

    return yajl_do_finish(hand);
}

unsigned char *
yajl_get_error(yajl_handle hand, int verbose,
               const unsigned char * jsonText, size_t jsonTextLen)
{
    return yajl_render_error_string(hand, jsonText, jsonTextLen, verbose);
}

size_t
yajl_get_bytes_consumed(yajl_handle hand)
{
    if (!hand) return 0;
    else return hand->bytesConsumed;
}


void
yajl_free_error(yajl_handle hand, unsigned char * str)
{
    /* use memory allocation functions if set */
    YA_FREE(&(hand->alloc), str);
}

/* XXX: add utility routines to parse from file */
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// #include "yajl_encode.h"
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __YAJL_ENCODE_H__
#define __YAJL_ENCODE_H__

// #include "yajl_buf.h"

// #include "yajl_gen.h"
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file yajl_gen.h
 * Interface to YAJL's JSON generation facilities.
 */

// #include <yajl/yajl_common.h>


#ifndef __YAJL_GEN_H__
#define __YAJL_GEN_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    /** generator status codes */
    typedef enum {
        /** no error */
        yajl_gen_status_ok = 0,
        /** at a point where a map key is generated, a function other than
         *  yajl_gen_string was called */
        yajl_gen_keys_must_be_strings,
        /** YAJL's maximum generation depth was exceeded.  see
         *  YAJL_MAX_DEPTH */
        yajl_max_depth_exceeded,
        /** A generator function (yajl_gen_XXX) was called while in an error
         *  state */
        yajl_gen_in_error_state,
        /** A complete JSON document has been generated */
        yajl_gen_generation_complete,                
        /** yajl_gen_double was passed an invalid floating point value
         *  (infinity or NaN). */
        yajl_gen_invalid_number,
        /** A print callback was passed in, so there is no internal
         * buffer to get from */
        yajl_gen_no_buf,
        /** returned from yajl_gen_string() when the yajl_gen_validate_utf8
         *  option is enabled and an invalid was passed by client code.
         */
        yajl_gen_invalid_string
    } yajl_gen_status;

    /** an opaque handle to a generator */
    typedef struct yajl_gen_t * yajl_gen;

    /** a callback used for "printing" the results. */
    typedef void (*yajl_print_t)(void * ctx,
                                 const char * str,
                                 size_t len);

    /** configuration parameters for the parser, these may be passed to
     *  yajl_gen_config() along with option specific argument(s).  In general,
     *  all configuration parameters default to *off*. */
    typedef enum {
        /** generate indented (beautiful) output */
        yajl_gen_beautify = 0x01,
        /**
         * Set an indent string which is used when yajl_gen_beautify
         * is enabled.  Maybe something like \\t or some number of
         * spaces.  The default is four spaces ' '.
         */
        yajl_gen_indent_string = 0x02,
        /**
         * Set a function and context argument that should be used to
         * output generated json.  the function should conform to the
         * yajl_print_t prototype while the context argument is a
         * void * of your choosing.
         *
         * example:
         *   yajl_gen_config(g, yajl_gen_print_callback, myFunc, myVoidPtr);
         */
        yajl_gen_print_callback = 0x04,
        /**
         * Normally the generator does not validate that strings you
         * pass to it via yajl_gen_string() are valid UTF8.  Enabling
         * this option will cause it to do so.
         */
        yajl_gen_validate_utf8 = 0x08,
        /**
         * the forward solidus (slash or '/' in human) is not required to be
         * escaped in json text.  By default, YAJL will not escape it in the
         * iterest of saving bytes.  Setting this flag will cause YAJL to
         * always escape '/' in generated JSON strings.
         */
        yajl_gen_escape_solidus = 0x10
    } yajl_gen_option;

    /** allow the modification of generator options subsequent to handle
     *  allocation (via yajl_alloc)
     *  \returns zero in case of errors, non-zero otherwise
     */
    YAJL_API int yajl_gen_config(yajl_gen g, yajl_gen_option opt, ...);

    /** allocate a generator handle
     *  \param allocFuncs an optional pointer to a structure which allows
     *                    the client to overide the memory allocation
     *                    used by yajl.  May be NULL, in which case
     *                    malloc/free/realloc will be used.
     *
     *  \returns an allocated handle on success, NULL on failure (bad params)
     */
    YAJL_API yajl_gen yajl_gen_alloc(const yajl_alloc_funcs * allocFuncs);

    /** free a generator handle */
    YAJL_API void yajl_gen_free(yajl_gen handle);

    YAJL_API yajl_gen_status yajl_gen_integer(yajl_gen hand, long long int number);
    /** generate a floating point number.  number may not be infinity or
     *  NaN, as these have no representation in JSON.  In these cases the
     *  generator will return 'yajl_gen_invalid_number' */
    YAJL_API yajl_gen_status yajl_gen_double(yajl_gen hand, double number);
    YAJL_API yajl_gen_status yajl_gen_number(yajl_gen hand,
                                             const char * num,
                                             size_t len);
    YAJL_API yajl_gen_status yajl_gen_string(yajl_gen hand,
                                             const unsigned char * str,
                                             size_t len);
    YAJL_API yajl_gen_status yajl_gen_null(yajl_gen hand);
    YAJL_API yajl_gen_status yajl_gen_bool(yajl_gen hand, int boolean);
    YAJL_API yajl_gen_status yajl_gen_map_open(yajl_gen hand);
    YAJL_API yajl_gen_status yajl_gen_map_close(yajl_gen hand);
    YAJL_API yajl_gen_status yajl_gen_array_open(yajl_gen hand);
    YAJL_API yajl_gen_status yajl_gen_array_close(yajl_gen hand);

    /** access the null terminated generator buffer.  If incrementally
     *  outputing JSON, one should call yajl_gen_clear to clear the
     *  buffer.  This allows stream generation. */
    YAJL_API yajl_gen_status yajl_gen_get_buf(yajl_gen hand,
                                              const unsigned char ** buf,
                                              size_t * len);

    /** clear yajl's output buffer, but maintain all internal generation
     *  state.  This function will not "reset" the generator state, and is
     *  intended to enable incremental JSON outputing. */
    YAJL_API void yajl_gen_clear(yajl_gen hand);

#ifdef __cplusplus
}
#endif    

#endif


void yajl_string_encode(const yajl_print_t printer,
                        void * ctx,
                        const unsigned char * str,
                        size_t length,
                        int escape_solidus);

void yajl_string_decode(yajl_buf buf, const unsigned char * str,
                        size_t length);

int yajl_string_validate_utf8(const unsigned char * s, size_t len);

#endif


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void CharToHex(unsigned char c, char * hexBuf)
{
    const char * hexchar = "0123456789ABCDEF";
    hexBuf[0] = hexchar[c >> 4];
    hexBuf[1] = hexchar[c & 0x0F];
}

void
yajl_string_encode(const yajl_print_t print,
                   void * ctx,
                   const unsigned char * str,
                   size_t len,
                   int escape_solidus)
{
    size_t beg = 0;
    size_t end = 0;
    char hexBuf[7];
    hexBuf[0] = '\\'; hexBuf[1] = 'u'; hexBuf[2] = '0'; hexBuf[3] = '0';
    hexBuf[6] = 0;

    while (end < len) {
        const char * escaped = NULL;
        switch (str[end]) {
            case '\r': escaped = "\\r"; break;
            case '\n': escaped = "\\n"; break;
            case '\\': escaped = "\\\\"; break;
            /* it is not required to escape a solidus in JSON:
             * read sec. 2.5: http://www.ietf.org/rfc/rfc4627.txt
             * specifically, this production from the grammar:
             *   unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
             */
            case '/': if (escape_solidus) escaped = "\\/"; break;
            case '"': escaped = "\\\""; break;
            case '\f': escaped = "\\f"; break;
            case '\b': escaped = "\\b"; break;
            case '\t': escaped = "\\t"; break;
            default:
                if ((unsigned char) str[end] < 32) {
                    CharToHex(str[end], hexBuf + 4);
                    escaped = hexBuf;
                }
                break;
        }
        if (escaped != NULL) {
            print(ctx, (const char *) (str + beg), end - beg);
            print(ctx, escaped, (unsigned int)strlen(escaped));
            beg = ++end;
        } else {
            ++end;
        }
    }
    print(ctx, (const char *) (str + beg), end - beg);
}

static void hexToDigit(unsigned int * val, const unsigned char * hex)
{
    unsigned int i;
    for (i=0;i<4;i++) {
        unsigned char c = hex[i];
        if (c >= 'A') c = (c & ~0x20) - 7;
        c -= '0';
        assert(!(c & 0xF0));
        *val = (*val << 4) | c;
    }
}

static void Utf32toUtf8(unsigned int codepoint, char * utf8Buf) 
{
    if (codepoint < 0x80) {
        utf8Buf[0] = (char) codepoint;
        utf8Buf[1] = 0;
    } else if (codepoint < 0x0800) {
        utf8Buf[0] = (char) ((codepoint >> 6) | 0xC0);
        utf8Buf[1] = (char) ((codepoint & 0x3F) | 0x80);
        utf8Buf[2] = 0;
    } else if (codepoint < 0x10000) {
        utf8Buf[0] = (char) ((codepoint >> 12) | 0xE0);
        utf8Buf[1] = (char) (((codepoint >> 6) & 0x3F) | 0x80);
        utf8Buf[2] = (char) ((codepoint & 0x3F) | 0x80);
        utf8Buf[3] = 0;
    } else if (codepoint < 0x200000) {
        utf8Buf[0] =(char)((codepoint >> 18) | 0xF0);
        utf8Buf[1] =(char)(((codepoint >> 12) & 0x3F) | 0x80);
        utf8Buf[2] =(char)(((codepoint >> 6) & 0x3F) | 0x80);
        utf8Buf[3] =(char)((codepoint & 0x3F) | 0x80);
        utf8Buf[4] = 0;
    } else {
        utf8Buf[0] = '?';
        utf8Buf[1] = 0;
    }
}

void yajl_string_decode(yajl_buf buf, const unsigned char * str,
                        size_t len)
{
    size_t beg = 0;
    size_t end = 0;    

    while (end < len) {
        if (str[end] == '\\') {
            char utf8Buf[5];
            const char * unescaped = "?";
            yajl_buf_append(buf, str + beg, end - beg);
            switch (str[++end]) {
                case 'r': unescaped = "\r"; break;
                case 'n': unescaped = "\n"; break;
                case '\\': unescaped = "\\"; break;
                case '/': unescaped = "/"; break;
                case '"': unescaped = "\""; break;
                case 'f': unescaped = "\f"; break;
                case 'b': unescaped = "\b"; break;
                case 't': unescaped = "\t"; break;
                case 'u': {
                    unsigned int codepoint = 0;
                    hexToDigit(&codepoint, str + ++end);
                    end+=3;
                    /* check if this is a surrogate */
                    if ((codepoint & 0xFC00) == 0xD800) {
                        end++;
                        if (str[end] == '\\' && str[end + 1] == 'u') {
                            unsigned int surrogate = 0;
                            hexToDigit(&surrogate, str + end + 2);
                            codepoint =
                                (((codepoint & 0x3F) << 10) | 
                                 ((((codepoint >> 6) & 0xF) + 1) << 16) | 
                                 (surrogate & 0x3FF));
                            end += 5;
                        } else {
                            unescaped = "?";
                            break;
                        }
                    }
                    
                    Utf32toUtf8(codepoint, utf8Buf);
                    unescaped = utf8Buf;

                    if (codepoint == 0) {
                        yajl_buf_append(buf, unescaped, 1);
                        beg = ++end;
                        continue;
                    }

                    break;
                }
                default:
                    assert("this should never happen" == NULL);
            }
            yajl_buf_append(buf, unescaped, (unsigned int)strlen(unescaped));
            beg = ++end;
        } else {
            end++;
        }
    }
    yajl_buf_append(buf, str + beg, end - beg);
}

#define ADV_PTR s++; if (!(len--)) return 0;

int yajl_string_validate_utf8(const unsigned char * s, size_t len)
{
    if (!len) return 1;
    if (!s) return 0;
    
    while (len--) {
        /* single byte */
        if (*s <= 0x7f) {
            /* noop */
        }
        /* two byte */ 
        else if ((*s >> 5) == 0x6) {
            ADV_PTR;
            if (!((*s >> 6) == 0x2)) return 0;
        }
        /* three byte */
        else if ((*s >> 4) == 0x0e) {
            ADV_PTR;
            if (!((*s >> 6) == 0x2)) return 0;
            ADV_PTR;
            if (!((*s >> 6) == 0x2)) return 0;
        }
        /* four byte */        
        else if ((*s >> 3) == 0x1e) {
            ADV_PTR;
            if (!((*s >> 6) == 0x2)) return 0;
            ADV_PTR;
            if (!((*s >> 6) == 0x2)) return 0;
            ADV_PTR;
            if (!((*s >> 6) == 0x2)) return 0;
        } else {
            return 0;
        }
        
        s++;
    }
    
    return 1;
}
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// #include "yajl_gen.h"

// #include "yajl_buf.h"

// #include "yajl_encode.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

typedef enum {
    yajl_gen_start,
    yajl_gen_map_start,
    yajl_gen_map_key,
    yajl_gen_map_val,
    yajl_gen_array_start,
    yajl_gen_in_array,
    yajl_gen_complete,
    yajl_gen_error
} yajl_gen_state;

struct yajl_gen_t
{
    unsigned int flags;
    unsigned int depth;
    const char * indentString;
    yajl_gen_state state[YAJL_MAX_DEPTH];
    yajl_print_t print;
    void * ctx; /* yajl_buf */
    /* memory allocation routines */
    yajl_alloc_funcs alloc;
};

int
yajl_gen_config(yajl_gen g, yajl_gen_option opt, ...)
{
    int rv = 1;
    va_list ap;
    va_start(ap, opt);

    switch(opt) {
        case yajl_gen_beautify:
        case yajl_gen_validate_utf8:
        case yajl_gen_escape_solidus:
            if (va_arg(ap, int)) g->flags |= opt;
            else g->flags &= ~opt;
            break;
        case yajl_gen_indent_string: {
            const char *indent = va_arg(ap, const char *);
            g->indentString = indent;
            for (; *indent; indent++) {
                if (*indent != '\n'
                    && *indent != '\v'
                    && *indent != '\f'
                    && *indent != '\t'
                    && *indent != '\r'
                    && *indent != ' ')
                {
                    g->indentString = NULL;
                    rv = 0;
                }
            }
            break;
        }
        case yajl_gen_print_callback:
            yajl_buf_free(g->ctx);
            g->print = va_arg(ap, const yajl_print_t);
            g->ctx = va_arg(ap, void *);
            break;
        default:
            rv = 0;
    }

    va_end(ap);

    return rv;
}



yajl_gen
yajl_gen_alloc(const yajl_alloc_funcs * afs)
{
    yajl_gen g = NULL;
    yajl_alloc_funcs afsBuffer;

    /* first order of business is to set up memory allocation routines */
    if (afs != NULL) {
        if (afs->malloc == NULL || afs->realloc == NULL || afs->free == NULL)
        {
            return NULL;
        }
    } else {
        yajl_set_default_alloc_funcs(&afsBuffer);
        afs = &afsBuffer;
    }

    g = (yajl_gen) YA_MALLOC(afs, sizeof(struct yajl_gen_t));
    if (!g) return NULL;

    memset((void *) g, 0, sizeof(struct yajl_gen_t));
    /* copy in pointers to allocation routines */
    memcpy((void *) &(g->alloc), (void *) afs, sizeof(yajl_alloc_funcs));

    g->print = (yajl_print_t)&yajl_buf_append;
    g->ctx = yajl_buf_alloc(&(g->alloc));
    g->indentString = "    ";

    return g;
}

void
yajl_gen_free(yajl_gen g)
{
    if (g->print == (yajl_print_t)&yajl_buf_append) yajl_buf_free((yajl_buf)g->ctx);
    YA_FREE(&(g->alloc), g);
}

#define INSERT_SEP \
    if (g->state[g->depth] == yajl_gen_map_key ||               \
        g->state[g->depth] == yajl_gen_in_array) {              \
        g->print(g->ctx, ",", 1);                               \
        if ((g->flags & yajl_gen_beautify)) g->print(g->ctx, "\n", 1);               \
    } else if (g->state[g->depth] == yajl_gen_map_val) {        \
        g->print(g->ctx, ":", 1);                               \
        if ((g->flags & yajl_gen_beautify)) g->print(g->ctx, " ", 1);                \
   }

#define INSERT_WHITESPACE                                               \
    if ((g->flags & yajl_gen_beautify)) {                                                    \
        if (g->state[g->depth] != yajl_gen_map_val) {                   \
            unsigned int _i;                                            \
            for (_i=0;_i<g->depth;_i++)                                 \
                g->print(g->ctx,                                        \
                         g->indentString,                               \
                         (unsigned int)strlen(g->indentString));        \
        }                                                               \
    }

#define ENSURE_NOT_KEY \
    if (g->state[g->depth] == yajl_gen_map_key ||       \
        g->state[g->depth] == yajl_gen_map_start)  {    \
        return yajl_gen_keys_must_be_strings;           \
    }                                                   \

/* check that we're not complete, or in error state.  in a valid state
 * to be generating */
#define ENSURE_VALID_STATE \
    if (g->state[g->depth] == yajl_gen_error) {   \
        return yajl_gen_in_error_state;\
    } else if (g->state[g->depth] == yajl_gen_complete) {   \
        return yajl_gen_generation_complete;                \
    }

#define INCREMENT_DEPTH \
    if (++(g->depth) >= YAJL_MAX_DEPTH) return yajl_max_depth_exceeded;

#define DECREMENT_DEPTH \
  if (--(g->depth) >= YAJL_MAX_DEPTH) return yajl_gen_invalid_string;

#define APPENDED_ATOM \
    switch (g->state[g->depth]) {                   \
        case yajl_gen_start:                        \
            g->state[g->depth] = yajl_gen_complete; \
            break;                                  \
        case yajl_gen_map_start:                    \
        case yajl_gen_map_key:                      \
            g->state[g->depth] = yajl_gen_map_val;  \
            break;                                  \
        case yajl_gen_array_start:                  \
            g->state[g->depth] = yajl_gen_in_array; \
            break;                                  \
        case yajl_gen_map_val:                      \
            g->state[g->depth] = yajl_gen_map_key;  \
            break;                                  \
        default:                                    \
            break;                                  \
    }                                               \

#define FINAL_NEWLINE                                        \
    if ((g->flags & yajl_gen_beautify) && g->state[g->depth] == yajl_gen_complete) \
        g->print(g->ctx, "\n", 1);

yajl_gen_status
yajl_gen_integer(yajl_gen g, long long int number)
{
    char i[32];
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    sprintf(i, "%lld", number);
    g->print(g->ctx, i, (unsigned int)strlen(i));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

#if defined(_WIN32) || defined(WIN32)
# include <float.h>
# ifndef isnan
#  define isnan _isnan
# endif
# ifndef isinf
#  define isinf !_finite
# endif
#endif

#if defined(__sun) && defined(__GNUC__) && __GNUC__ < 4
# define isinf !finite
#endif

yajl_gen_status
yajl_gen_double(yajl_gen g, double number)
{
    char i[32];
    ENSURE_VALID_STATE; ENSURE_NOT_KEY;
    if (isnan(number) || isinf(number)) return yajl_gen_invalid_number;
    INSERT_SEP; INSERT_WHITESPACE;
    sprintf(i, "%.20g", number);
    if (strspn(i, "0123456789-") == strlen(i)) {
        strcat(i, ".0");
    }
    g->print(g->ctx, i, (unsigned int)strlen(i));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_number(yajl_gen g, const char * s, size_t l)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    g->print(g->ctx, s, l);
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_string(yajl_gen g, const unsigned char * str,
                size_t len)
{
    // if validation is enabled, check that the string is valid utf8
    // XXX: This checking could be done a little faster, in the same pass as
    // the string encoding
    if (g->flags & yajl_gen_validate_utf8) {
        if (!yajl_string_validate_utf8(str, len)) {
            return yajl_gen_invalid_string;
        }
    }
    ENSURE_VALID_STATE; INSERT_SEP; INSERT_WHITESPACE;
    g->print(g->ctx, "\"", 1);
    yajl_string_encode(g->print, g->ctx, str, len, g->flags & yajl_gen_escape_solidus);
    g->print(g->ctx, "\"", 1);
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_null(yajl_gen g)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    g->print(g->ctx, "null", strlen("null"));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_bool(yajl_gen g, int boolean)
{
    const char * val = boolean ? "true" : "false";

	ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    g->print(g->ctx, val, (unsigned int)strlen(val));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_map_open(yajl_gen g)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    INCREMENT_DEPTH;

    g->state[g->depth] = yajl_gen_map_start;
    g->print(g->ctx, "{", 1);
    if ((g->flags & yajl_gen_beautify)) g->print(g->ctx, "\n", 1);
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_map_close(yajl_gen g)
{
    ENSURE_VALID_STATE;
    DECREMENT_DEPTH;

    if ((g->flags & yajl_gen_beautify)) g->print(g->ctx, "\n", 1);
    APPENDED_ATOM;
    INSERT_WHITESPACE;
    g->print(g->ctx, "}", 1);
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_array_open(yajl_gen g)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    INCREMENT_DEPTH;
    g->state[g->depth] = yajl_gen_array_start;
    g->print(g->ctx, "[", 1);
    if ((g->flags & yajl_gen_beautify)) g->print(g->ctx, "\n", 1);
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_array_close(yajl_gen g)
{
    ENSURE_VALID_STATE;
    DECREMENT_DEPTH;
    if ((g->flags & yajl_gen_beautify)) g->print(g->ctx, "\n", 1);
    APPENDED_ATOM;
    INSERT_WHITESPACE;
    g->print(g->ctx, "]", 1);
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_get_buf(yajl_gen g, const unsigned char ** buf,
                 size_t * len)
{
    if (g->print != (yajl_print_t)&yajl_buf_append) return yajl_gen_no_buf;
    *buf = yajl_buf_data((yajl_buf)g->ctx);
    *len = yajl_buf_len((yajl_buf)g->ctx);
    return yajl_gen_status_ok;
}

void
yajl_gen_clear(yajl_gen g)
{
    if (g->print == (yajl_print_t)&yajl_buf_append) yajl_buf_clear((yajl_buf)g->ctx);
}
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// #include "yajl_lex.h"

// #include "yajl_buf.h"


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifdef YAJL_LEXER_DEBUG
static const char *
tokToStr(yajl_tok tok) 
{
    switch (tok) {
        case yajl_tok_bool: return "bool";
        case yajl_tok_colon: return "colon";
        case yajl_tok_comma: return "comma";
        case yajl_tok_eof: return "eof";
        case yajl_tok_error: return "error";
        case yajl_tok_left_brace: return "brace";
        case yajl_tok_left_bracket: return "bracket";
        case yajl_tok_null: return "null";
        case yajl_tok_integer: return "integer";
        case yajl_tok_double: return "double";
        case yajl_tok_right_brace: return "brace";
        case yajl_tok_right_bracket: return "bracket";
        case yajl_tok_string: return "string";
        case yajl_tok_string_with_escapes: return "string_with_escapes";
    }
    return "unknown";
}
#endif

/* Impact of the stream parsing feature on the lexer:
 *
 * YAJL support stream parsing.  That is, the ability to parse the first
 * bits of a chunk of JSON before the last bits are available (still on
 * the network or disk).  This makes the lexer more complex.  The
 * responsibility of the lexer is to handle transparently the case where
 * a chunk boundary falls in the middle of a token.  This is
 * accomplished is via a buffer and a character reading abstraction. 
 *
 * Overview of implementation
 *
 * When we lex to end of input string before end of token is hit, we
 * copy all of the input text composing the token into our lexBuf.
 * 
 * Every time we read a character, we do so through the readChar function.
 * readChar's responsibility is to handle pulling all chars from the buffer
 * before pulling chars from input text
 */

struct yajl_lexer_t {
    /* the overal line and char offset into the data */
    size_t lineOff;
    size_t charOff;

    /* error */
    yajl_lex_error error;

    /* a input buffer to handle the case where a token is spread over
     * multiple chunks */ 
    yajl_buf buf;

    /* in the case where we have data in the lexBuf, bufOff holds
     * the current offset into the lexBuf. */
    size_t bufOff;

    /* are we using the lex buf? */
    unsigned int bufInUse;

    /* shall we allow comments? */
    unsigned int allowComments;

    /* shall we validate utf8 inside strings? */
    unsigned int validateUTF8;

    yajl_alloc_funcs * alloc;
};

#define readChar(lxr, txt, off)                      \
    (((lxr)->bufInUse && yajl_buf_len((lxr)->buf) && lxr->bufOff < yajl_buf_len((lxr)->buf)) ? \
     (*((const unsigned char *) yajl_buf_data((lxr)->buf) + ((lxr)->bufOff)++)) : \
     ((txt)[(*(off))++]))

#define unreadChar(lxr, off) ((*(off) > 0) ? (*(off))-- : ((lxr)->bufOff--))

yajl_lexer
yajl_lex_alloc(yajl_alloc_funcs * alloc,
               unsigned int allowComments, unsigned int validateUTF8)
{
    yajl_lexer lxr = (yajl_lexer) YA_MALLOC(alloc, sizeof(struct yajl_lexer_t));
    memset((void *) lxr, 0, sizeof(struct yajl_lexer_t));
    lxr->buf = yajl_buf_alloc(alloc);
    lxr->allowComments = allowComments;
    lxr->validateUTF8 = validateUTF8;
    lxr->alloc = alloc;
    return lxr;
}

void
yajl_lex_free(yajl_lexer lxr)
{
    yajl_buf_free(lxr->buf);
    YA_FREE(lxr->alloc, lxr);
    return;
}

/* a lookup table which lets us quickly determine three things:
 * VEC - valid escaped control char
 * note.  the solidus '/' may be escaped or not.
 * IJC - invalid json char
 * VHC - valid hex char
 * NFP - needs further processing (from a string scanning perspective)
 * NUC - needs utf8 checking when enabled (from a string scanning perspective)
 */
#define VEC 0x01
#define IJC 0x02
#define VHC 0x04
#define NFP 0x08
#define NUC 0x10

static const char charLookupTable[256] =
{
/*00*/ IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    ,
/*08*/ IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    ,
/*10*/ IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    ,
/*18*/ IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    ,

/*20*/ 0      , 0      , NFP|VEC|IJC, 0      , 0      , 0      , 0      , 0      ,
/*28*/ 0      , 0      , 0      , 0      , 0      , 0      , 0      , VEC    ,
/*30*/ VHC    , VHC    , VHC    , VHC    , VHC    , VHC    , VHC    , VHC    ,
/*38*/ VHC    , VHC    , 0      , 0      , 0      , 0      , 0      , 0      ,

/*40*/ 0      , VHC    , VHC    , VHC    , VHC    , VHC    , VHC    , 0      ,
/*48*/ 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      ,
/*50*/ 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      ,
/*58*/ 0      , 0      , 0      , 0      , NFP|VEC|IJC, 0      , 0      , 0      ,

/*60*/ 0      , VHC    , VEC|VHC, VHC    , VHC    , VHC    , VEC|VHC, 0      ,
/*68*/ 0      , 0      , 0      , 0      , 0      , 0      , VEC    , 0      ,
/*70*/ 0      , 0      , VEC    , 0      , VEC    , 0      , 0      , 0      ,
/*78*/ 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      ,

       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,

       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,

       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,

       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    ,
       NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC    , NUC
};

/** process a variable length utf8 encoded codepoint.
 *
 *  returns:
 *    yajl_tok_string - if valid utf8 char was parsed and offset was
 *                      advanced
 *    yajl_tok_eof - if end of input was hit before validation could
 *                   complete
 *    yajl_tok_error - if invalid utf8 was encountered
 * 
 *  NOTE: on error the offset will point to the first char of the
 *  invalid utf8 */
#define UTF8_CHECK_EOF if (*offset >= jsonTextLen) { return yajl_tok_eof; }

static yajl_tok
yajl_lex_utf8_char(yajl_lexer lexer, const unsigned char * jsonText,
                   size_t jsonTextLen, size_t * offset,
                   unsigned char curChar)
{
    if (curChar <= 0x7f) {
        /* single byte */
        return yajl_tok_string;
    } else if ((curChar >> 5) == 0x6) {
        /* two byte */ 
        UTF8_CHECK_EOF;
        curChar = readChar(lexer, jsonText, offset);
        if ((curChar >> 6) == 0x2) return yajl_tok_string;
    } else if ((curChar >> 4) == 0x0e) {
        /* three byte */
        UTF8_CHECK_EOF;
        curChar = readChar(lexer, jsonText, offset);
        if ((curChar >> 6) == 0x2) {
            UTF8_CHECK_EOF;
            curChar = readChar(lexer, jsonText, offset);
            if ((curChar >> 6) == 0x2) return yajl_tok_string;
        }
    } else if ((curChar >> 3) == 0x1e) {
        /* four byte */
        UTF8_CHECK_EOF;
        curChar = readChar(lexer, jsonText, offset);
        if ((curChar >> 6) == 0x2) {
            UTF8_CHECK_EOF;
            curChar = readChar(lexer, jsonText, offset);
            if ((curChar >> 6) == 0x2) {
                UTF8_CHECK_EOF;
                curChar = readChar(lexer, jsonText, offset);
                if ((curChar >> 6) == 0x2) return yajl_tok_string;
            }
        }
    } 

    return yajl_tok_error;
}

/* lex a string.  input is the lexer, pointer to beginning of
 * json text, and start of string (offset).
 * a token is returned which has the following meanings:
 * yajl_tok_string: lex of string was successful.  offset points to
 *                  terminating '"'.
 * yajl_tok_eof: end of text was encountered before we could complete
 *               the lex.
 * yajl_tok_error: embedded in the string were unallowable chars.  offset
 *               points to the offending char
 */
#define STR_CHECK_EOF \
if (*offset >= jsonTextLen) { \
   tok = yajl_tok_eof; \
   goto finish_string_lex; \
}

/** scan a string for interesting characters that might need further
 *  review.  return the number of chars that are uninteresting and can
 *  be skipped.
 * (lth) hi world, any thoughts on how to make this routine faster? */
static size_t
yajl_string_scan(const unsigned char * buf, size_t len, int utf8check)
{
    unsigned char mask = IJC|NFP|(utf8check ? NUC : 0);
    size_t skip = 0;
    while (skip < len && !(charLookupTable[*buf] & mask))
    {
        skip++;
        buf++;
    }
    return skip;
}

static yajl_tok
yajl_lex_string(yajl_lexer lexer, const unsigned char * jsonText,
                size_t jsonTextLen, size_t * offset)
{
    yajl_tok tok = yajl_tok_error;
    int hasEscapes = 0;

    for (;;) {
        unsigned char curChar;

        /* now jump into a faster scanning routine to skip as much
         * of the buffers as possible */
        {
            const unsigned char * p;
            size_t len;
            
            if ((lexer->bufInUse && yajl_buf_len(lexer->buf) &&
                 lexer->bufOff < yajl_buf_len(lexer->buf)))
            {
                p = ((const unsigned char *) yajl_buf_data(lexer->buf) +
                     (lexer->bufOff));
                len = yajl_buf_len(lexer->buf) - lexer->bufOff;
                lexer->bufOff += yajl_string_scan(p, len, lexer->validateUTF8);
            }                
            else if (*offset < jsonTextLen) 
            {
                p = jsonText + *offset;
                len = jsonTextLen - *offset;
                *offset += yajl_string_scan(p, len, lexer->validateUTF8);
            }
        }

        STR_CHECK_EOF;

        curChar = readChar(lexer, jsonText, offset);

        /* quote terminates */
        if (curChar == '"') {
            tok = yajl_tok_string;
            break;
        }
        /* backslash escapes a set of control chars, */
        else if (curChar == '\\') {
            hasEscapes = 1;
            STR_CHECK_EOF;

            /* special case \u */
            curChar = readChar(lexer, jsonText, offset);
            if (curChar == 'u') {
                unsigned int i = 0;

                for (i=0;i<4;i++) {
                    STR_CHECK_EOF;                
                    curChar = readChar(lexer, jsonText, offset);                
                    if (!(charLookupTable[curChar] & VHC)) {
                        /* back up to offending char */
                        unreadChar(lexer, offset);
                        lexer->error = yajl_lex_string_invalid_hex_char;
                        goto finish_string_lex;
                    }
                }
            } else if (!(charLookupTable[curChar] & VEC)) {
                /* back up to offending char */
                unreadChar(lexer, offset);
                lexer->error = yajl_lex_string_invalid_escaped_char;
                goto finish_string_lex;                
            } 
        }
        /* when not validating UTF8 it's a simple table lookup to determine
         * if the present character is invalid */
        else if(charLookupTable[curChar] & IJC) {
            /* back up to offending char */
            unreadChar(lexer, offset);
            lexer->error = yajl_lex_string_invalid_json_char;
            goto finish_string_lex;                
        }
        /* when in validate UTF8 mode we need to do some extra work */
        else if (lexer->validateUTF8) {
            yajl_tok t = yajl_lex_utf8_char(lexer, jsonText, jsonTextLen,
                                            offset, curChar);
            
            if (t == yajl_tok_eof) {
                tok = yajl_tok_eof;
                goto finish_string_lex;
            } else if (t == yajl_tok_error) {
                lexer->error = yajl_lex_string_invalid_utf8;
                goto finish_string_lex;
            } 
        }
        /* accept it, and move on */ 
    }
  finish_string_lex:
    /* tell our buddy, the parser, wether he needs to process this string
     * again */
    if (hasEscapes && tok == yajl_tok_string) {
        tok = yajl_tok_string_with_escapes;
    } 

    return tok;
}

#define RETURN_IF_EOF if (*offset >= jsonTextLen) return yajl_tok_eof;

static yajl_tok
yajl_lex_number(yajl_lexer lexer, const unsigned char * jsonText,
                size_t jsonTextLen, size_t * offset)
{
    /** XXX: numbers are the only entities in json that we must lex
     *       _beyond_ in order to know that they are complete.  There
     *       is an ambiguous case for integers at EOF. */

    unsigned char c;

    yajl_tok tok = yajl_tok_integer;

    RETURN_IF_EOF;    
    c = readChar(lexer, jsonText, offset);

    /* optional leading minus */
    if (c == '-') {
        RETURN_IF_EOF;    
        c = readChar(lexer, jsonText, offset); 
    }

    /* a single zero, or a series of integers */
    if (c == '0') {
        RETURN_IF_EOF;    
        c = readChar(lexer, jsonText, offset); 
    } else if (c >= '1' && c <= '9') {
        do {
            RETURN_IF_EOF;    
            c = readChar(lexer, jsonText, offset); 
        } while (c >= '0' && c <= '9');
    } else {
        unreadChar(lexer, offset);
        lexer->error = yajl_lex_missing_integer_after_minus;
        return yajl_tok_error;
    }

    /* optional fraction (indicates this is floating point) */
    if (c == '.') {
        int numRd = 0;
        
        RETURN_IF_EOF;
        c = readChar(lexer, jsonText, offset); 

        while (c >= '0' && c <= '9') {
            numRd++;
            RETURN_IF_EOF;
            c = readChar(lexer, jsonText, offset); 
        } 

        if (!numRd) {
            unreadChar(lexer, offset);
            lexer->error = yajl_lex_missing_integer_after_decimal;
            return yajl_tok_error;
        }
        tok = yajl_tok_double;
    }

    /* optional exponent (indicates this is floating point) */
    if (c == 'e' || c == 'E') {
        RETURN_IF_EOF;
        c = readChar(lexer, jsonText, offset); 

        /* optional sign */
        if (c == '+' || c == '-') {
            RETURN_IF_EOF;
            c = readChar(lexer, jsonText, offset); 
        }

        if (c >= '0' && c <= '9') {
            do {
                RETURN_IF_EOF;
                c = readChar(lexer, jsonText, offset); 
            } while (c >= '0' && c <= '9');
        } else {
            unreadChar(lexer, offset);
            lexer->error = yajl_lex_missing_integer_after_exponent;
            return yajl_tok_error;
        }
        tok = yajl_tok_double;
    }
    
    /* we always go "one too far" */
    unreadChar(lexer, offset);
    
    return tok;
}

static yajl_tok
yajl_lex_comment(yajl_lexer lexer, const unsigned char * jsonText,
                 size_t jsonTextLen, size_t * offset)
{
    unsigned char c;

    yajl_tok tok = yajl_tok_comment;

    RETURN_IF_EOF;    
    c = readChar(lexer, jsonText, offset);

    /* either slash or star expected */
    if (c == '/') {
        /* now we throw away until end of line */
        do {
            RETURN_IF_EOF;    
            c = readChar(lexer, jsonText, offset); 
        } while (c != '\n');
    } else if (c == '*') {
        /* now we throw away until end of comment */        
        for (;;) {
            RETURN_IF_EOF;    
            c = readChar(lexer, jsonText, offset); 
            if (c == '*') {
                RETURN_IF_EOF;    
                c = readChar(lexer, jsonText, offset);                 
                if (c == '/') {
                    break;
                } else {
                    unreadChar(lexer, offset);
                }
            }
        }
    } else {
        lexer->error = yajl_lex_invalid_char;
        tok = yajl_tok_error;
    }
    
    return tok;
}

yajl_tok
yajl_lex_lex(yajl_lexer lexer, const unsigned char * jsonText,
             size_t jsonTextLen, size_t * offset,
             const unsigned char ** outBuf, size_t * outLen)
{
    yajl_tok tok = yajl_tok_error;
    unsigned char c;
    size_t startOffset = *offset;

    *outBuf = NULL;
    *outLen = 0;

    for (;;) {
        assert(*offset <= jsonTextLen);

        if (*offset >= jsonTextLen) {
            tok = yajl_tok_eof;
            goto lexed;
        }

        c = readChar(lexer, jsonText, offset);

        switch (c) {
            case '{':
                tok = yajl_tok_left_bracket;
                goto lexed;
            case '}':
                tok = yajl_tok_right_bracket;
                goto lexed;
            case '[':
                tok = yajl_tok_left_brace;
                goto lexed;
            case ']':
                tok = yajl_tok_right_brace;
                goto lexed;
            case ',':
                tok = yajl_tok_comma;
                goto lexed;
            case ':':
                tok = yajl_tok_colon;
                goto lexed;
            case '\t': case '\n': case '\v': case '\f': case '\r': case ' ':
                startOffset++;
                break;
            case 't': {
                const char * want = "rue";
                do {
                    if (*offset >= jsonTextLen) {
                        tok = yajl_tok_eof;
                        goto lexed;
                    }
                    c = readChar(lexer, jsonText, offset);
                    if (c != *want) {
                        unreadChar(lexer, offset);
                        lexer->error = yajl_lex_invalid_string;
                        tok = yajl_tok_error;
                        goto lexed;
                    }
                } while (*(++want));
                tok = yajl_tok_bool;
                goto lexed;
            }
            case 'f': {
                const char * want = "alse";
                do {
                    if (*offset >= jsonTextLen) {
                        tok = yajl_tok_eof;
                        goto lexed;
                    }
                    c = readChar(lexer, jsonText, offset);
                    if (c != *want) {
                        unreadChar(lexer, offset);
                        lexer->error = yajl_lex_invalid_string;
                        tok = yajl_tok_error;
                        goto lexed;
                    }
                } while (*(++want));
                tok = yajl_tok_bool;
                goto lexed;
            }
            case 'n': {
                const char * want = "ull";
                do {
                    if (*offset >= jsonTextLen) {
                        tok = yajl_tok_eof;
                        goto lexed;
                    }
                    c = readChar(lexer, jsonText, offset);
                    if (c != *want) {
                        unreadChar(lexer, offset);
                        lexer->error = yajl_lex_invalid_string;
                        tok = yajl_tok_error;
                        goto lexed;
                    }
                } while (*(++want));
                tok = yajl_tok_null;
                goto lexed;
            }
            case '"': {
                tok = yajl_lex_string(lexer, (const unsigned char *) jsonText,
                                      jsonTextLen, offset);
                goto lexed;
            }
            case '-':
            case '0': case '1': case '2': case '3': case '4': 
            case '5': case '6': case '7': case '8': case '9': {
                /* integer parsing wants to start from the beginning */
                unreadChar(lexer, offset);
                tok = yajl_lex_number(lexer, (const unsigned char *) jsonText,
                                      jsonTextLen, offset);
                goto lexed;
            }
            case '/':
                /* hey, look, a probable comment!  If comments are disabled
                 * it's an error. */
                if (!lexer->allowComments) {
                    unreadChar(lexer, offset);
                    lexer->error = yajl_lex_unallowed_comment;
                    tok = yajl_tok_error;
                    goto lexed;
                }
                /* if comments are enabled, then we should try to lex
                 * the thing.  possible outcomes are
                 * - successful lex (tok_comment, which means continue),
                 * - malformed comment opening (slash not followed by
                 *   '*' or '/') (tok_error)
                 * - eof hit. (tok_eof) */
                tok = yajl_lex_comment(lexer, (const unsigned char *) jsonText,
                                       jsonTextLen, offset);
                if (tok == yajl_tok_comment) {
                    /* "error" is silly, but that's the initial
                     * state of tok.  guilty until proven innocent. */  
                    tok = yajl_tok_error;
                    yajl_buf_clear(lexer->buf);
                    lexer->bufInUse = 0;
                    startOffset = *offset; 
                    break;
                }
                /* hit error or eof, bail */
                goto lexed;
            default:
                lexer->error = yajl_lex_invalid_char;
                tok = yajl_tok_error;
                goto lexed;
        }
    }


  lexed:
    /* need to append to buffer if the buffer is in use or
     * if it's an EOF token */
    if (tok == yajl_tok_eof || lexer->bufInUse) {
        if (!lexer->bufInUse) yajl_buf_clear(lexer->buf);
        lexer->bufInUse = 1;
        yajl_buf_append(lexer->buf, jsonText + startOffset, *offset - startOffset);
        lexer->bufOff = 0;
        
        if (tok != yajl_tok_eof) {
            *outBuf = yajl_buf_data(lexer->buf);
            *outLen = yajl_buf_len(lexer->buf);
            lexer->bufInUse = 0;
        }
    } else if (tok != yajl_tok_error) {
        *outBuf = jsonText + startOffset;
        *outLen = *offset - startOffset;
    }

    /* special case for strings. skip the quotes. */
    if (tok == yajl_tok_string || tok == yajl_tok_string_with_escapes)
    {
        assert(*outLen >= 2);
        (*outBuf)++;
        *outLen -= 2; 
    }


#ifdef YAJL_LEXER_DEBUG
    if (tok == yajl_tok_error) {
        printf("lexical error: %s\n",
               yajl_lex_error_to_string(yajl_lex_get_error(lexer)));
    } else if (tok == yajl_tok_eof) {
        printf("EOF hit\n");
    } else {
        printf("lexed %s: '", tokToStr(tok));
        fwrite(*outBuf, 1, *outLen, stdout);
        printf("'\n");
    }
#endif

    return tok;
}

const char *
yajl_lex_error_to_string(yajl_lex_error error)
{
    switch (error) {
        case yajl_lex_e_ok:
            return "ok, no error";
        case yajl_lex_string_invalid_utf8:
            return "invalid bytes in UTF8 string.";
        case yajl_lex_string_invalid_escaped_char:
            return "inside a string, '\\' occurs before a character "
                   "which it may not.";
        case yajl_lex_string_invalid_json_char:            
            return "invalid character inside string.";
        case yajl_lex_string_invalid_hex_char:
            return "invalid (non-hex) character occurs after '\\u' inside "
                   "string.";
        case yajl_lex_invalid_char:
            return "invalid char in json text.";
        case yajl_lex_invalid_string:
            return "invalid string in json text.";
        case yajl_lex_missing_integer_after_exponent:
            return "malformed number, a digit is required after the exponent.";
        case yajl_lex_missing_integer_after_decimal:
            return "malformed number, a digit is required after the "
                   "decimal point.";
        case yajl_lex_missing_integer_after_minus:
            return "malformed number, a digit is required after the "
                   "minus sign.";
        case yajl_lex_unallowed_comment:
            return "probable comment found in input text, comments are "
                   "not enabled.";
    }
    return "unknown error code";
}


/** allows access to more specific information about the lexical
 *  error when yajl_lex_lex returns yajl_tok_error. */
yajl_lex_error
yajl_lex_get_error(yajl_lexer lexer)
{
    if (lexer == NULL) return (yajl_lex_error) -1;
    return lexer->error;
}

size_t yajl_lex_current_line(yajl_lexer lexer)
{
    return lexer->lineOff;
}

size_t yajl_lex_current_char(yajl_lexer lexer)
{
    return lexer->charOff;
}

yajl_tok yajl_lex_peek(yajl_lexer lexer, const unsigned char * jsonText,
                       size_t jsonTextLen, size_t offset)
{
    const unsigned char * outBuf;
    size_t outLen;
    size_t bufLen = yajl_buf_len(lexer->buf);
    size_t bufOff = lexer->bufOff;
    unsigned int bufInUse = lexer->bufInUse;
    yajl_tok tok;
    
    tok = yajl_lex_lex(lexer, jsonText, jsonTextLen, &offset,
                       &outBuf, &outLen);

    lexer->bufOff = bufOff;
    lexer->bufInUse = bufInUse;
    yajl_buf_truncate(lexer->buf, bufLen);
    
    return tok;
}
/*
 * Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// #include "yajl_parse.h"

// #include "yajl_lex.h"

// #include "yajl_parser.h"

// #include "yajl_encode.h"

// #include "yajl_bytestack.h"


#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#define MAX_VALUE_TO_MULTIPLY ((INT64_MAX / 10) + (INT64_MAX % 10))

 /* same semantics as strtol */
int64_t
yajl_parse_integer(const unsigned char *number, unsigned int length)
{
    int64_t ret  = 0;
    long sign = 1;
    const unsigned char *pos = number;
    if (*pos == '-') { pos++; sign = -1; }
    if (*pos == '+') { pos++; }

    while (pos < number + length) {
        if ( ret > MAX_VALUE_TO_MULTIPLY ) {
            errno = ERANGE;
            return sign == 1 ? INT64_MAX : INT64_MIN;
        }
        ret *= 10;
        if (INT64_MAX - ret < (*pos - '0')) {
            errno = ERANGE;
            return sign == 1 ? INT64_MAX : INT64_MIN;
        }
        if (*pos < '0' || *pos > '9') {
            errno = ERANGE;
            return sign == 1 ? INT64_MAX : INT64_MIN;
        }
        ret += (*pos++ - '0');
    }

    return sign * ret;
}

unsigned char *
yajl_render_error_string(yajl_handle hand, const unsigned char * jsonText,
                         size_t jsonTextLen, int verbose)
{
    size_t offset = hand->bytesConsumed;
    unsigned char * str;
    const char * errorType = NULL;
    const char * errorText = NULL;
    char text[72];
    const char * arrow = "                     (right here) ------^\n";

    if (yajl_bs_current(hand->stateStack) == yajl_state_parse_error) {
        errorType = "parse";
        errorText = hand->parseError;
    } else if (yajl_bs_current(hand->stateStack) == yajl_state_lexical_error) {
        errorType = "lexical";
        errorText = yajl_lex_error_to_string(yajl_lex_get_error(hand->lexer));
    } else {
        errorType = "unknown";
    }

    {
        size_t memneeded = 0;
        memneeded += strlen(errorType);
        memneeded += strlen(" error");
        if (errorText != NULL) {
            memneeded += strlen(": ");
            memneeded += strlen(errorText);
        }
        str = (unsigned char *) YA_MALLOC(&(hand->alloc), memneeded + 2);
        if (!str) return NULL;
        str[0] = 0;
        strcat((char *) str, errorType);
        strcat((char *) str, " error");
        if (errorText != NULL) {
            strcat((char *) str, ": ");
            strcat((char *) str, errorText);
        }
        strcat((char *) str, "\n");
    }

    /* now we append as many spaces as needed to make sure the error
     * falls at char 41, if verbose was specified */
    if (verbose) {
        size_t start, end, i;
        size_t spacesNeeded;

        spacesNeeded = (offset < 30 ? 40 - offset : 10);
        start = (offset >= 30 ? offset - 30 : 0);
        end = (offset + 30 > jsonTextLen ? jsonTextLen : offset + 30);

        for (i=0;i<spacesNeeded;i++) text[i] = ' ';

        for (;start < end;start++, i++) {
            if (jsonText[start] != '\n' && jsonText[start] != '\r')
            {
                text[i] = jsonText[start];
            }
            else
            {
                text[i] = ' ';
            }
        }
        assert(i <= 71);
        text[i++] = '\n';
        text[i] = 0;
        {
            char * newStr = (char *)
                YA_MALLOC(&(hand->alloc), (unsigned int)(strlen((char *) str) +
                                                         strlen((char *) text) +
                                                         strlen(arrow) + 1));
            if (newStr) {
                newStr[0] = 0;
                strcat((char *) newStr, (char *) str);
                strcat((char *) newStr, text);
                strcat((char *) newStr, arrow);
            }
            YA_FREE(&(hand->alloc), str);
            str = (unsigned char *) newStr;
        }
    }
    return str;
}

/* check for client cancelation */
#define _CC_CHK(x)                                                \
    if (!(x)) {                                                   \
        yajl_bs_set(hand->stateStack, yajl_state_parse_error);    \
        hand->parseError =                                        \
            "client cancelled parse via callback return value";   \
        return yajl_status_client_canceled;                       \
    }


yajl_status
yajl_do_finish(yajl_handle hand)
{
    yajl_status stat;
    stat = yajl_do_parse(hand,(const unsigned char *) " ",1);

    if (stat != yajl_status_ok) return stat;

    switch(yajl_bs_current(hand->stateStack))
    {
        case yajl_state_parse_error:
        case yajl_state_lexical_error:
            return yajl_status_error;
        case yajl_state_got_value:
        case yajl_state_parse_complete:
            return yajl_status_ok;
        default:
            if (!(hand->flags & yajl_allow_partial_values))
            {
                yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                hand->parseError = "premature EOF";
                return yajl_status_error;
            }
            return yajl_status_ok;
    }
}

yajl_status
yajl_do_parse(yajl_handle hand, const unsigned char * jsonText,
              size_t jsonTextLen)
{
    yajl_tok tok;
    const unsigned char * buf;
    size_t bufLen;
    size_t * offset = &(hand->bytesConsumed);

    *offset = 0;

  around_again:
    switch (yajl_bs_current(hand->stateStack)) {
        case yajl_state_parse_complete:
            if (hand->flags & yajl_allow_multiple_values) {
                yajl_bs_set(hand->stateStack, yajl_state_got_value);
                goto around_again;
            }
            if (!(hand->flags & yajl_allow_trailing_garbage)) {
                if (*offset != jsonTextLen) {
                    tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                                       offset, &buf, &bufLen);
                    if (tok != yajl_tok_eof) {
                        yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                        hand->parseError = "trailing garbage";
                    }
                    goto around_again;
                }
            }
            return yajl_status_ok;
        case yajl_state_lexical_error:
        case yajl_state_parse_error:
            return yajl_status_error;
        case yajl_state_start:
        case yajl_state_got_value:
        case yajl_state_map_need_val:
        case yajl_state_array_need_val:
        case yajl_state_array_start:  {
            /* for arrays and maps, we advance the state for this
             * depth, then push the state of the next depth.
             * If an error occurs during the parsing of the nesting
             * enitity, the state at this level will not matter.
             * a state that needs pushing will be anything other
             * than state_start */

            yajl_state stateToPush = yajl_state_start;

            tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);

            switch (tok) {
                case yajl_tok_eof:
                    return yajl_status_ok;
                case yajl_tok_error:
                    yajl_bs_set(hand->stateStack, yajl_state_lexical_error);
                    goto around_again;
                case yajl_tok_string:
                    if (hand->callbacks && hand->callbacks->yajl_string) {
                        _CC_CHK(hand->callbacks->yajl_string(hand->ctx,
                                                             buf, bufLen));
                    }
                    break;
                case yajl_tok_string_with_escapes:
                    if (hand->callbacks && hand->callbacks->yajl_string) {
                        yajl_buf_clear(hand->decodeBuf);
                        yajl_string_decode(hand->decodeBuf, buf, bufLen);
                        _CC_CHK(hand->callbacks->yajl_string(
                                    hand->ctx, yajl_buf_data(hand->decodeBuf),
                                    yajl_buf_len(hand->decodeBuf)));
                    }
                    break;
                case yajl_tok_bool:
                    if (hand->callbacks && hand->callbacks->yajl_boolean) {
                        _CC_CHK(hand->callbacks->yajl_boolean(hand->ctx,
                                                              *buf == 't'));
                    }
                    break;
                case yajl_tok_null:
                    if (hand->callbacks && hand->callbacks->yajl_null) {
                        _CC_CHK(hand->callbacks->yajl_null(hand->ctx));
                    }
                    break;
                case yajl_tok_left_bracket:
                    if (hand->callbacks && hand->callbacks->yajl_start_map) {
                        _CC_CHK(hand->callbacks->yajl_start_map(hand->ctx));
                    }
                    stateToPush = yajl_state_map_start;
                    break;
                case yajl_tok_left_brace:
                    if (hand->callbacks && hand->callbacks->yajl_start_array) {
                        _CC_CHK(hand->callbacks->yajl_start_array(hand->ctx));
                    }
                    stateToPush = yajl_state_array_start;
                    break;
                case yajl_tok_integer:
                    if (hand->callbacks) {
                        if (hand->callbacks->yajl_number) {
                            _CC_CHK(hand->callbacks->yajl_number(
                                        hand->ctx,(const char *) buf, bufLen));
                        } else if (hand->callbacks->yajl_integer) {
                            int64_t i = 0;
                            errno = 0;
                            i = yajl_parse_integer(buf, (unsigned int)bufLen);
                            if ((i == INT64_MIN || i == INT64_MAX) &&
                                errno == ERANGE)
                            {
                                yajl_bs_set(hand->stateStack,
                                            yajl_state_parse_error);
                                hand->parseError = "integer overflow" ;
                                /* try to restore error offset */
                                if (*offset >= bufLen) *offset -= bufLen;
                                else *offset = 0;
                                goto around_again;
                            }
                            _CC_CHK(hand->callbacks->yajl_integer(hand->ctx,
                                                                  i));
                        }
                    }
                    break;
                case yajl_tok_double:
                    if (hand->callbacks) {
                        if (hand->callbacks->yajl_number) {
                            _CC_CHK(hand->callbacks->yajl_number(
                                        hand->ctx, (const char *) buf, bufLen));
                        } else if (hand->callbacks->yajl_double) {
                            double d = 0.0;
                            yajl_buf_clear(hand->decodeBuf);
                            yajl_buf_append(hand->decodeBuf, buf, bufLen);
                            buf = yajl_buf_data(hand->decodeBuf);
                            errno = 0;
                            d = strtod((char *) buf, NULL);
                            if ((d == HUGE_VAL || d == -HUGE_VAL) &&
                                errno == ERANGE)
                            {
                                yajl_bs_set(hand->stateStack,
                                            yajl_state_parse_error);
                                hand->parseError = "numeric (floating point) "
                                    "overflow";
                                /* try to restore error offset */
                                if (*offset >= bufLen) *offset -= bufLen;
                                else *offset = 0;
                                goto around_again;
                            }
                            _CC_CHK(hand->callbacks->yajl_double(hand->ctx,
                                                                 d));
                        }
                    }
                    break;
                case yajl_tok_right_brace: {
                    if (yajl_bs_current(hand->stateStack) ==
                        yajl_state_array_start)
                    {
                        if (hand->callbacks &&
                            hand->callbacks->yajl_end_array)
                        {
                            _CC_CHK(hand->callbacks->yajl_end_array(hand->ctx));
                        }
                        yajl_bs_pop(hand->stateStack);
                        goto around_again;
                    }
                    /* intentional fall-through */
                }
                case yajl_tok_colon:
                case yajl_tok_comma:
                case yajl_tok_right_bracket:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError =
                        "unallowed token at this point in JSON text";
                    goto around_again;
                default:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError = "invalid token, internal error";
                    goto around_again;
            }
            /* got a value.  transition depends on the state we're in. */
            {
                yajl_state s = yajl_bs_current(hand->stateStack);
                if (s == yajl_state_start || s == yajl_state_got_value) {
                    yajl_bs_set(hand->stateStack, yajl_state_parse_complete);
                } else if (s == yajl_state_map_need_val) {
                    yajl_bs_set(hand->stateStack, yajl_state_map_got_val);
                } else {
                    yajl_bs_set(hand->stateStack, yajl_state_array_got_val);
                }
            }
            if (stateToPush != yajl_state_start) {
                yajl_bs_push(hand->stateStack, stateToPush);
            }

            goto around_again;
        }
        case yajl_state_map_start:
        case yajl_state_map_need_key: {
            /* only difference between these two states is that in
             * start '}' is valid, whereas in need_key, we've parsed
             * a comma, and a string key _must_ follow */
            tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case yajl_tok_eof:
                    return yajl_status_ok;
                case yajl_tok_error:
                    yajl_bs_set(hand->stateStack, yajl_state_lexical_error);
                    goto around_again;
                case yajl_tok_string_with_escapes:
                    if (hand->callbacks && hand->callbacks->yajl_map_key) {
                        yajl_buf_clear(hand->decodeBuf);
                        yajl_string_decode(hand->decodeBuf, buf, bufLen);
                        buf = yajl_buf_data(hand->decodeBuf);
                        bufLen = yajl_buf_len(hand->decodeBuf);
                    }
                    /* intentional fall-through */
                case yajl_tok_string:
                    if (hand->callbacks && hand->callbacks->yajl_map_key) {
                        _CC_CHK(hand->callbacks->yajl_map_key(hand->ctx, buf,
                                                              bufLen));
                    }
                    yajl_bs_set(hand->stateStack, yajl_state_map_sep);
                    goto around_again;
                case yajl_tok_right_bracket:
                    if (yajl_bs_current(hand->stateStack) ==
                        yajl_state_map_start)
                    {
                        if (hand->callbacks && hand->callbacks->yajl_end_map) {
                            _CC_CHK(hand->callbacks->yajl_end_map(hand->ctx));
                        }
                        yajl_bs_pop(hand->stateStack);
                        goto around_again;
                    }
                default:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError =
                        "invalid object key (must be a string)";
                    goto around_again;
            }
        }
        case yajl_state_map_sep: {
            tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case yajl_tok_colon:
                    yajl_bs_set(hand->stateStack, yajl_state_map_need_val);
                    goto around_again;
                case yajl_tok_eof:
                    return yajl_status_ok;
                case yajl_tok_error:
                    yajl_bs_set(hand->stateStack, yajl_state_lexical_error);
                    goto around_again;
                default:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError = "object key and value must "
                        "be separated by a colon (':')";
                    goto around_again;
            }
        }
        case yajl_state_map_got_val: {
            tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case yajl_tok_right_bracket:
                    if (hand->callbacks && hand->callbacks->yajl_end_map) {
                        _CC_CHK(hand->callbacks->yajl_end_map(hand->ctx));
                    }
                    yajl_bs_pop(hand->stateStack);
                    goto around_again;
                case yajl_tok_comma:
                    yajl_bs_set(hand->stateStack, yajl_state_map_need_key);
                    goto around_again;
                case yajl_tok_eof:
                    return yajl_status_ok;
                case yajl_tok_error:
                    yajl_bs_set(hand->stateStack, yajl_state_lexical_error);
                    goto around_again;
                default:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError = "after key and value, inside map, "
                                       "I expect ',' or '}'";
                    /* try to restore error offset */
                    if (*offset >= bufLen) *offset -= bufLen;
                    else *offset = 0;
                    goto around_again;
            }
        }
        case yajl_state_array_got_val: {
            tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case yajl_tok_right_brace:
                    if (hand->callbacks && hand->callbacks->yajl_end_array) {
                        _CC_CHK(hand->callbacks->yajl_end_array(hand->ctx));
                    }
                    yajl_bs_pop(hand->stateStack);
                    goto around_again;
                case yajl_tok_comma:
                    yajl_bs_set(hand->stateStack, yajl_state_array_need_val);
                    goto around_again;
                case yajl_tok_eof:
                    return yajl_status_ok;
                case yajl_tok_error:
                    yajl_bs_set(hand->stateStack, yajl_state_lexical_error);
                    goto around_again;
                default:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError =
                        "after array element, I expect ',' or ']'";
                    goto around_again;
            }
        }
    }

    abort();
    return yajl_status_error;
}

/*
 * Copyright (c) 2010-2011  Florian Forster  <ff at octo.it>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

// #include "yajl_tree.h"
/*
 * Copyright (c) 2010-2011  Florian Forster  <ff at octo.it>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file yajl_tree.h
 *
 * Parses JSON data and returns the data in tree form.
 *
 * \author Florian Forster
 * \date August 2010
 *
 * This interface makes quick parsing and extraction of
 * smallish JSON docs trivial:
 *
 * \include example/parse_config.c
 */

#ifndef YAJL_TREE_H
#define YAJL_TREE_H 1

// #include <yajl/yajl_common.h>


#ifdef __cplusplus
extern "C" {
#endif

/** possible data types that a yajl_val_s can hold */
typedef enum {
    yajl_t_string = 1,
    yajl_t_number = 2,
    yajl_t_object = 3,
    yajl_t_array = 4,
    yajl_t_true = 5,
    yajl_t_false = 6,
    yajl_t_null = 7,
    /** The any type isn't valid for yajl_val_s.type, but can be
     *  used as an argument to routines like yajl_tree_get().
     */
    yajl_t_any = 8
} yajl_type;

#define YAJL_NUMBER_INT_VALID    0x01
#define YAJL_NUMBER_DOUBLE_VALID 0x02

/** A pointer to a node in the parse tree */
typedef struct yajl_val_s * yajl_val;

/**
 * A JSON value representation capable of holding one of the seven
 * types above. For "string", "number", "object", and "array"
 * additional data is available in the union.  The "YAJL_IS_*"
 * and "YAJL_GET_*" macros below allow type checking and convenient
 * value extraction.
 */
struct yajl_val_s
{
    /** Type of the value contained. Use the "YAJL_IS_*" macors to check for a
     * specific type. */
    yajl_type type;
    /** Type-specific data. You may use the "YAJL_GET_*" macros to access these
     * members. */
    union
    {
        char * string;
        struct {
            long long i; /*< integer value, if representable. */
            double  d;   /*< double value, if representable. */
            /** Signals whether the \em i and \em d members are
             * valid. See \c YAJL_NUMBER_INT_VALID and
             * \c YAJL_NUMBER_DOUBLE_VALID. */
            char   *r;   /*< unparsed number in string form. */
            unsigned int flags;
        } number;
        struct {
            const char **keys; /*< Array of keys */
            yajl_val *values; /*< Array of values. */
            size_t len; /*< Number of key-value-pairs. */
        } object;
        struct {
            yajl_val *values; /*< Array of elements. */
            size_t len; /*< Number of elements. */
        } array;
    } u;
};

/**
 * Parse a string.
 *
 * Parses an null-terminated string containing JSON data and returns a pointer
 * to the top-level value (root of the parse tree).
 *
 * \param input              Pointer to a null-terminated utf8 string containing
 *                           JSON data.
 * \param error_buffer       Pointer to a buffer in which an error message will
 *                           be stored if \em yajl_tree_parse fails, or
 *                           \c NULL. The buffer will be initialized before
 *                           parsing, so its content will be destroyed even if
 *                           \em yajl_tree_parse succeeds.
 * \param error_buffer_size  Size of the memory area pointed to by
 *                           \em error_buffer_size. If \em error_buffer_size is
 *                           \c NULL, this argument is ignored.
 *
 * \returns Pointer to the top-level value or \c NULL on error. The memory
 * pointed to must be freed using \em yajl_tree_free. In case of an error, a
 * null terminated message describing the error in more detail is stored in
 * \em error_buffer if it is not \c NULL.
 */
YAJL_API yajl_val yajl_tree_parse (const char *input,
                                   char *error_buffer, size_t error_buffer_size);

/**
 * Free a parse tree returned by "yajl_tree_parse".
 *
 * \param v Pointer to a JSON value returned by "yajl_tree_parse". Passing NULL
 * is valid and results in a no-op.
 */
YAJL_API void yajl_tree_free (yajl_val v);

/**
 * Access a nested value inside a tree.
 *
 * \param parent the node under which you'd like to extract values.
 * \param path A null terminated array of strings, each the name of an object key
 * \param type the yajl_type of the object you seek, or yajl_t_any if any will do.
 *
 * \returns a pointer to the found value, or NULL if we came up empty.
 * 
 * Future Ideas:  it'd be nice to move path to a string and implement support for
 * a teeny tiny micro language here, so you can extract array elements, do things
 * like .first and .last, even .length.  Inspiration from JSONPath and css selectors?
 * No it wouldn't be fast, but that's not what this API is about.
 */
YAJL_API yajl_val yajl_tree_get(yajl_val parent, const char ** path, yajl_type type);

/* Various convenience macros to check the type of a `yajl_val` */
#define YAJL_IS_STRING(v) (((v) != NULL) && ((v)->type == yajl_t_string))
#define YAJL_IS_NUMBER(v) (((v) != NULL) && ((v)->type == yajl_t_number))
#define YAJL_IS_INTEGER(v) (YAJL_IS_NUMBER(v) && ((v)->u.number.flags & YAJL_NUMBER_INT_VALID))
#define YAJL_IS_DOUBLE(v) (YAJL_IS_NUMBER(v) && ((v)->u.number.flags & YAJL_NUMBER_DOUBLE_VALID))
#define YAJL_IS_OBJECT(v) (((v) != NULL) && ((v)->type == yajl_t_object))
#define YAJL_IS_ARRAY(v)  (((v) != NULL) && ((v)->type == yajl_t_array ))
#define YAJL_IS_TRUE(v)   (((v) != NULL) && ((v)->type == yajl_t_true  ))
#define YAJL_IS_FALSE(v)  (((v) != NULL) && ((v)->type == yajl_t_false ))
#define YAJL_IS_NULL(v)   (((v) != NULL) && ((v)->type == yajl_t_null  ))

/** Given a yajl_val_string return a ptr to the bare string it contains,
 *  or NULL if the value is not a string. */
#define YAJL_GET_STRING(v) (YAJL_IS_STRING(v) ? (v)->u.string : NULL)

/** Get the string representation of a number.  You should check type first,
 *  perhaps using YAJL_IS_NUMBER */
#define YAJL_GET_NUMBER(v) ((v)->u.number.r)

/** Get the double representation of a number.  You should check type first,
 *  perhaps using YAJL_IS_DOUBLE */
#define YAJL_GET_DOUBLE(v) ((v)->u.number.d)

/** Get the 64bit (long long) integer representation of a number.  You should
 *  check type first, perhaps using YAJL_IS_INTEGER */
#define YAJL_GET_INTEGER(v) ((v)->u.number.i)

/** Get a pointer to a yajl_val_object or NULL if the value is not an object. */
#define YAJL_GET_OBJECT(v) (YAJL_IS_OBJECT(v) ? &(v)->u.object : NULL)

/** Get a pointer to a yajl_val_array or NULL if the value is not an object. */
#define YAJL_GET_ARRAY(v)  (YAJL_IS_ARRAY(v)  ? &(v)->u.array  : NULL)

#ifdef __cplusplus
}
#endif

#endif /* YAJL_TREE_H */

// #include "yajl_parse.h"


// #include "yajl_parser.h"


#if defined(_WIN32) || defined(WIN32)
#define snprintf sprintf_s
#endif

#define STATUS_CONTINUE 1
#define STATUS_ABORT    0

struct stack_elem_s;
typedef struct stack_elem_s stack_elem_t;
struct stack_elem_s
{
    char * key;
    yajl_val value;
    stack_elem_t *next;
};

struct context_s
{
    stack_elem_t *stack;
    yajl_val root;
    char *errbuf;
    size_t errbuf_size;
};
typedef struct context_s context_t;

#define RETURN_ERROR(ctx,retval,...) {                                  \
        if ((ctx)->errbuf != NULL)                                      \
            snprintf ((ctx)->errbuf, (ctx)->errbuf_size, __VA_ARGS__);  \
        return (retval);                                                \
    }

static yajl_val value_alloc (yajl_type type)
{
    yajl_val v;

    v = malloc (sizeof (*v));
    if (v == NULL) return (NULL);
    memset (v, 0, sizeof (*v));
    v->type = type;

    return (v);
}

static void yajl_object_free (yajl_val v)
{
    size_t i;

    if (!YAJL_IS_OBJECT(v)) return;

    for (i = 0; i < v->u.object.len; i++)
    {
        free((char *) v->u.object.keys[i]);
        v->u.object.keys[i] = NULL;
        yajl_tree_free (v->u.object.values[i]);
        v->u.object.values[i] = NULL;
    }

    free((void*) v->u.object.keys);
    free(v->u.object.values);
    free(v);
}

static void yajl_array_free (yajl_val v)
{
    size_t i;

    if (!YAJL_IS_ARRAY(v)) return;

    for (i = 0; i < v->u.array.len; i++)
    {
        yajl_tree_free (v->u.array.values[i]);
        v->u.array.values[i] = NULL;
    }

    free(v->u.array.values);
    free(v);
}

/*
 * Parsing nested objects and arrays is implemented using a stack. When a new
 * object or array starts (a curly or a square opening bracket is read), an
 * appropriate value is pushed on the stack. When the end of the object is
 * reached (an appropriate closing bracket has been read), the value is popped
 * off the stack and added to the enclosing object using "context_add_value".
 */
static int context_push(context_t *ctx, yajl_val v)
{
    stack_elem_t *stack;

    stack = malloc (sizeof (*stack));
    if (stack == NULL)
        RETURN_ERROR (ctx, ENOMEM, "Out of memory");
    memset (stack, 0, sizeof (*stack));

    assert ((ctx->stack == NULL)
            || YAJL_IS_OBJECT (v)
            || YAJL_IS_ARRAY (v));

    stack->value = v;
    stack->next = ctx->stack;
    ctx->stack = stack;

    return (0);
}

static yajl_val context_pop(context_t *ctx)
{
    stack_elem_t *stack;
    yajl_val v;

    if (ctx->stack == NULL)
        RETURN_ERROR (ctx, NULL, "context_pop: "
                      "Bottom of stack reached prematurely");

    stack = ctx->stack;
    ctx->stack = stack->next;

    v = stack->value;

    free (stack);

    return (v);
}

static int object_add_keyval(context_t *ctx,
                             yajl_val obj, char *key, yajl_val value)
{
    const char **tmpk;
    yajl_val *tmpv;

    /* We're checking for NULL in "context_add_value" or its callers. */
    assert (ctx != NULL);
    assert (obj != NULL);
    assert (key != NULL);
    assert (value != NULL);

    /* We're assuring that "obj" is an object in "context_add_value". */
    assert(YAJL_IS_OBJECT(obj));

    tmpk = realloc((void *) obj->u.object.keys, sizeof(*(obj->u.object.keys)) * (obj->u.object.len + 1));
    if (tmpk == NULL)
        RETURN_ERROR(ctx, ENOMEM, "Out of memory");
    obj->u.object.keys = tmpk;

    tmpv = realloc(obj->u.object.values, sizeof (*obj->u.object.values) * (obj->u.object.len + 1));
    if (tmpv == NULL)
        RETURN_ERROR(ctx, ENOMEM, "Out of memory");
    obj->u.object.values = tmpv;

    obj->u.object.keys[obj->u.object.len] = key;
    obj->u.object.values[obj->u.object.len] = value;
    obj->u.object.len++;

    return (0);
}

static int array_add_value (context_t *ctx,
                            yajl_val array, yajl_val value)
{
    yajl_val *tmp;

    /* We're checking for NULL pointers in "context_add_value" or its
     * callers. */
    assert (ctx != NULL);
    assert (array != NULL);
    assert (value != NULL);

    /* "context_add_value" will only call us with array values. */
    assert(YAJL_IS_ARRAY(array));
    
    tmp = realloc(array->u.array.values,
                  sizeof(*(array->u.array.values)) * (array->u.array.len + 1));
    if (tmp == NULL)
        RETURN_ERROR(ctx, ENOMEM, "Out of memory");
    array->u.array.values = tmp;
    array->u.array.values[array->u.array.len] = value;
    array->u.array.len++;

    return 0;
}

/*
 * Add a value to the value on top of the stack or the "root" member in the
 * context if the end of the parsing process is reached.
 */
static int context_add_value (context_t *ctx, yajl_val v)
{
    /* We're checking for NULL values in all the calling functions. */
    assert (ctx != NULL);
    assert (v != NULL);

    /*
     * There are three valid states in which this function may be called:
     *   - There is no value on the stack => This is the only value. This is the
     *     last step done when parsing a document. We assign the value to the
     *     "root" member and return.
     *   - The value on the stack is an object. In this case store the key on the
     *     stack or, if the key has already been read, add key and value to the
     *     object.
     *   - The value on the stack is an array. In this case simply add the value
     *     and return.
     */
    if (ctx->stack == NULL)
    {
        assert (ctx->root == NULL);
        ctx->root = v;
        return (0);
    }
    else if (YAJL_IS_OBJECT (ctx->stack->value))
    {
        if (ctx->stack->key == NULL)
        {
            if (!YAJL_IS_STRING (v))
                RETURN_ERROR (ctx, EINVAL, "context_add_value: "
                              "Object key is not a string (%#04x)",
                              v->type);

            ctx->stack->key = v->u.string;
            v->u.string = NULL;
            free(v);
            return (0);
        }
        else /* if (ctx->key != NULL) */
        {
            char * key;

            key = ctx->stack->key;
            ctx->stack->key = NULL;
            return (object_add_keyval (ctx, ctx->stack->value, key, v));
        }
    }
    else if (YAJL_IS_ARRAY (ctx->stack->value))
    {
        return (array_add_value (ctx, ctx->stack->value, v));
    }
    else
    {
        RETURN_ERROR (ctx, EINVAL, "context_add_value: Cannot add value to "
                      "a value of type %#04x (not a composite type)",
                      ctx->stack->value->type);
    }
}

static int handle_string (void *ctx,
                          const unsigned char *string, size_t string_length)
{
    yajl_val v;

    v = value_alloc (yajl_t_string);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.string = malloc (string_length + 1);
    if (v->u.string == NULL)
    {
        free (v);
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");
    }
    memcpy(v->u.string, string, string_length);
    v->u.string[string_length] = 0;

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_number (void *ctx, const char *string, size_t string_length)
{
    yajl_val v;
    char *endptr;

    v = value_alloc(yajl_t_number);
    if (v == NULL)
        RETURN_ERROR((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.number.r = malloc(string_length + 1);
    if (v->u.number.r == NULL)
    {
        free(v);
        RETURN_ERROR((context_t *) ctx, STATUS_ABORT, "Out of memory");
    }
    memcpy(v->u.number.r, string, string_length);
    v->u.number.r[string_length] = 0;

    v->u.number.flags = 0;

    errno = 0;
    v->u.number.i = yajl_parse_integer((const unsigned char *) v->u.number.r,
                                       (unsigned int)strlen(v->u.number.r));
    if (errno == 0)
        v->u.number.flags |= YAJL_NUMBER_INT_VALID;

    endptr = NULL;
    errno = 0;
    v->u.number.d = strtod(v->u.number.r, &endptr);
    if ((errno == 0) && (endptr != NULL) && (*endptr == 0))
        v->u.number.flags |= YAJL_NUMBER_DOUBLE_VALID;

    return ((context_add_value(ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_start_map (void *ctx)
{
    yajl_val v;

    v = value_alloc(yajl_t_object);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.object.keys = NULL;
    v->u.object.values = NULL;
    v->u.object.len = 0;

    return ((context_push (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_end_map (void *ctx)
{
    yajl_val v;

    v = context_pop (ctx);
    if (v == NULL)
        return (STATUS_ABORT);

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_start_array (void *ctx)
{
    yajl_val v;

    v = value_alloc(yajl_t_array);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.array.values = NULL;
    v->u.array.len = 0;

    return ((context_push (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_end_array (void *ctx)
{
    yajl_val v;

    v = context_pop (ctx);
    if (v == NULL)
        return (STATUS_ABORT);

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_boolean (void *ctx, int boolean_value)
{
    yajl_val v;

    v = value_alloc (boolean_value ? yajl_t_true : yajl_t_false);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_null (void *ctx)
{
    yajl_val v;

    v = value_alloc (yajl_t_null);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

/*
 * Public functions
 */
yajl_val yajl_tree_parse (const char *input,
                          char *error_buffer, size_t error_buffer_size)
{
    static const yajl_callbacks callbacks =
        {
            /* null        = */ handle_null,
            /* boolean     = */ handle_boolean,
            /* integer     = */ NULL,
            /* double      = */ NULL,
            /* number      = */ handle_number,
            /* string      = */ handle_string,
            /* start map   = */ handle_start_map,
            /* map key     = */ handle_string,
            /* end map     = */ handle_end_map,
            /* start array = */ handle_start_array,
            /* end array   = */ handle_end_array
        };

    yajl_handle handle;
    yajl_status status;
    char * internal_err_str;
	context_t ctx = { NULL, NULL, NULL, 0 };

	ctx.errbuf = error_buffer;
	ctx.errbuf_size = error_buffer_size;

    if (error_buffer != NULL)
        memset (error_buffer, 0, error_buffer_size);

    handle = yajl_alloc (&callbacks, NULL, &ctx);
    yajl_config(handle, yajl_allow_comments, 1);

    status = yajl_parse(handle,
                        (unsigned char *) input,
                        strlen (input));
    status = yajl_complete_parse (handle);
    if (status != yajl_status_ok) {
        if (error_buffer != NULL && error_buffer_size > 0) {
               internal_err_str = (char *) yajl_get_error(handle, 1,
                     (const unsigned char *) input,
                     strlen(input));
             snprintf(error_buffer, error_buffer_size, "%s", internal_err_str);
             YA_FREE(&(handle->alloc), internal_err_str);
        }
        yajl_free (handle);
        return NULL;
    }

    yajl_free (handle);
    return (ctx.root);
}

yajl_val yajl_tree_get(yajl_val n, const char ** path, yajl_type type)
{
    if (!path) return NULL;
    while (n && *path) {
        size_t i;
        size_t len;

        if (n->type != yajl_t_object) return NULL;
        len = n->u.object.len;
        for (i = 0; i < (unsigned int)len; i++) {
            if (!strcmp(*path, n->u.object.keys[i])) {
                n = n->u.object.values[i];
                break;
            }
        }
        if (i == len) return NULL;
        path++;
    }
    if (n && type != yajl_t_any && type != n->type) n = NULL;
    return n;
}

void yajl_tree_free (yajl_val v)
{
    if (v == NULL) return;

    if (YAJL_IS_STRING(v))
    {
        free(v->u.string);
        free(v);
    }
    else if (YAJL_IS_NUMBER(v))
    {
        free(v->u.number.r);
        free(v);
    }
    else if (YAJL_GET_OBJECT(v))
    {
        yajl_object_free(v);
    }
    else if (YAJL_GET_ARRAY(v))
    {
        yajl_array_free(v);
    }
    else /* if (yajl_t_true or yajl_t_false or yajl_t_null) */
    {
        free(v);
    }
}
/*
 * @file bcon.c
 * @brief BCON (BSON C Object Notation) Implementation
 */

/*    Copyright 2009-2013 MongoDB, Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */


#include <stdio.h>

// #include "bcon.h"
/*
 * @file bcon.h
 * @brief BCON (BSON C Object Notation) Declarations
 */

/*    Copyright 2009-2013 MongoDB, Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef BCON_H_
#define BCON_H_

// #include <bson.h>
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_H
#define BSON_H

#define BSON_INSIDE

// #include "bson-compat.h"


#include <string.h>
#include <time.h>

// #include "bson-macros.h"

// #include "bson-config.h"

// #include "bson-atomic.h"
/*
 * Copyright 2013-2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_ATOMIC_H
#define BSON_ATOMIC_H


// #include "bson-config.h"

// #include "bson-compat.h"

// #include "bson-macros.h"



BSON_BEGIN_DECLS


#if defined(__sun) && defined(__SVR4)
   /* Solaris */
#  include <atomic.h>
#  define bson_atomic_int_add(p,v)   atomic_add_32_nv((volatile uint32_t *)p, (v))
#  define bson_atomic_int64_add(p,v) atomic_add_64_nv((volatile uint64_t *)p, (v))
#elif defined(_WIN32)
   /* MSVC/MinGW */
#  define bson_atomic_int_add(p, v)   (InterlockedExchangeAdd((volatile LONG *)(p), (LONG)(v)) + (LONG)(v))
#  define bson_atomic_int64_add(p, v) (InterlockedExchangeAdd64((volatile LONGLONG *)(p), (LONGLONG)(v)) + (LONGLONG)(v))
#else
#  ifdef BSON_HAVE_ATOMIC_32_ADD_AND_FETCH
#    define bson_atomic_int_add(p,v) __sync_add_and_fetch((p), (v))
#  else
#    define __BSON_NEED_ATOMIC_32
#  endif
#  ifdef BSON_HAVE_ATOMIC_64_ADD_AND_FETCH
#    if BSON_GNUC_IS_VERSION(4, 1)
       /*
        * GCC 4.1 on i386 can generate buggy 64-bit atomic increment.
        * So we will work around with a fallback.
        *
        * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=40693
        */
#      define __BSON_NEED_ATOMIC_64
#    else
#      define bson_atomic_int64_add(p, v) __sync_add_and_fetch((volatile int64_t*)(p), (int64_t)(v))
#    endif
#  else
#    define __BSON_NEED_ATOMIC_64
#  endif
#endif

#ifdef __BSON_NEED_ATOMIC_32
  int32_t bson_atomic_int_add   (volatile int32_t *p, int32_t n);
#endif
#ifdef __BSON_NEED_ATOMIC_64
  int64_t bson_atomic_int64_add (volatile int64_t *p, int64_t n);
#endif


#if defined(_WIN32)
# define bson_memory_barrier() MemoryBarrier()
#elif defined(__GNUC__)
# if BSON_GNUC_CHECK_VERSION(4, 1)
#  define bson_memory_barrier() __sync_synchronize()
# else
#  warning "GCC Pre-4.1 discovered, using inline assembly for memory barrier."
#  define bson_memory_barrier() __asm__ volatile ("":::"memory")
# endif
#elif defined(__SUNPRO_C)
# include <mbarrier.h>
# define bson_memory_barrier() __machine_rw_barrier()
#elif defined(__xlC__)
# define bson_memory_barrier() __sync()
#else
# define __BSON_NEED_BARRIER 1
# warning "Unknown compiler, using lock for compiler barrier."
void bson_memory_barrier (void);
#endif


BSON_END_DECLS


#endif /* BSON_ATOMIC_H */

// #include "bson-context.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_CONTEXT_H
#define BSON_CONTEXT_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


// #include "bson-macros.h"

// #include "bson-types.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_TYPES_H
#define BSON_TYPES_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
#error "Only <bson.h> can be included directly."
#endif


#include <stdlib.h>
#include <sys/types.h>

// #include "bson-macros.h"

// #include "bson-compat.h"



BSON_BEGIN_DECLS


/*
 *--------------------------------------------------------------------------
 *
 * bson_unichar_t --
 *
 *       bson_unichar_t provides an unsigned 32-bit type for containing
 *       unicode characters. When iterating UTF-8 sequences, this should
 *       be used to avoid losing the high-bits of non-ascii characters.
 *
 * See also:
 *       bson_string_append_unichar()
 *
 *--------------------------------------------------------------------------
 */

typedef uint32_t bson_unichar_t;


/**
 * bson_context_flags_t:
 *
 * This enumeration is used to configure a bson_context_t.
 *
 * %BSON_CONTEXT_NONE: Use default options.
 * %BSON_CONTEXT_THREAD_SAFE: Context will be called from multiple threads.
 * %BSON_CONTEXT_DISABLE_PID_CACHE: Call getpid() instead of caching the
 *   result of getpid() when initializing the context.
 * %BSON_CONTEXT_DISABLE_HOST_CACHE: Call gethostname() instead of caching the
 *   result of gethostname() when initializing the context.
 */
typedef enum
{
   BSON_CONTEXT_NONE = 0,
   BSON_CONTEXT_THREAD_SAFE = (1 << 0),
   BSON_CONTEXT_DISABLE_HOST_CACHE = (1 << 1),
   BSON_CONTEXT_DISABLE_PID_CACHE = (1 << 2),
#if defined(__linux__)
   BSON_CONTEXT_USE_TASK_ID = (1 << 3),
#endif
} bson_context_flags_t;


/**
 * bson_context_t:
 *
 * This structure manages context for the bson library. It handles
 * configuration for thread-safety and other performance related requirements.
 * Consumers will create a context and may use multiple under a variety of
 * situations.
 *
 * If your program calls fork(), you should initialize a new bson_context_t
 * using bson_context_init().
 *
 * If you are using threading, it is suggested that you use a bson_context_t
 * per thread for best performance. Alternatively, you can initialize the
 * bson_context_t with BSON_CONTEXT_THREAD_SAFE, although a performance penalty
 * will be incurred.
 *
 * Many functions will require that you provide a bson_context_t such as OID
 * generation.
 *
 * This structure is oqaque in that you cannot see the contents of the
 * structure. However, it is stack allocatable in that enough padding is
 * provided in _bson_context_t to hold the structure.
 */
typedef struct _bson_context_t bson_context_t;


/**
 * bson_t:
 *
 * This structure manages a buffer whose contents are a properly formatted
 * BSON document. You may perform various transforms on the BSON documents.
 * Additionally, it can be iterated over using bson_iter_t.
 *
 * See bson_iter_init() for iterating the contents of a bson_t.
 *
 * When building a bson_t structure using the various append functions,
 * memory allocations may occur. That is performed using power of two
 * allocations and realloc().
 *
 * See http://bsonspec.org for the BSON document spec.
 *
 * This structure is meant to fit in two sequential 64-byte cachelines.
 */
BSON_ALIGNED_BEGIN (128)
typedef struct _bson_t
{
   uint32_t flags;        /* Internal flags for the bson_t. */
   uint32_t len;          /* Length of BSON data. */
   uint8_t padding[120];  /* Padding for stack allocation. */
} bson_t
BSON_ALIGNED_END (128);


/**
 * BSON_INITIALIZER:
 *
 * This macro can be used to initialize a #bson_t structure on the stack
 * without calling bson_init().
 *
 * |[
 * bson_t b = BSON_INITIALIZER;
 * ]|
 */
#define BSON_INITIALIZER { 3, 5, { 5 } }


BSON_STATIC_ASSERT (sizeof (bson_t) == 128);


/**
 * bson_oid_t:
 *
 * This structure contains the binary form of a BSON Object Id as specified
 * on http://bsonspec.org. If you would like the bson_oid_t in string form
 * see bson_oid_to_string() or bson_oid_to_string_r().
 */
typedef struct
{
   uint8_t bytes[12];
} bson_oid_t;


BSON_STATIC_ASSERT (sizeof (bson_oid_t) == 12);


/**
 * bson_validate_flags_t:
 *
 * This enumeration is used for validation of BSON documents. It allows
 * selective control on what you wish to validate.
 *
 * %BSON_VALIDATE_NONE: No additional validation occurs.
 * %BSON_VALIDATE_UTF8: Check that strings are valid UTF-8.
 * %BSON_VALIDATE_DOLLAR_KEYS: Check that keys do not start with $.
 * %BSON_VALIDATE_DOT_KEYS: Check that keys do not contain a period.
 * %BSON_VALIDATE_UTF8_ALLOW_NULL: Allow NUL bytes in UTF-8 text.
 */
typedef enum
{
   BSON_VALIDATE_NONE = 0,
   BSON_VALIDATE_UTF8 = (1 << 0),
   BSON_VALIDATE_DOLLAR_KEYS = (1 << 1),
   BSON_VALIDATE_DOT_KEYS = (1 << 2),
   BSON_VALIDATE_UTF8_ALLOW_NULL = (1 << 3),
} bson_validate_flags_t;


/**
 * bson_type_t:
 *
 * This enumeration contains all of the possible types within a BSON document.
 * Use bson_iter_type() to fetch the type of a field while iterating over it.
 */
typedef enum
{
   BSON_TYPE_EOD = 0x00,
   BSON_TYPE_DOUBLE = 0x01,
   BSON_TYPE_UTF8 = 0x02,
   BSON_TYPE_DOCUMENT = 0x03,
   BSON_TYPE_ARRAY = 0x04,
   BSON_TYPE_BINARY = 0x05,
   BSON_TYPE_UNDEFINED = 0x06,
   BSON_TYPE_OID = 0x07,
   BSON_TYPE_BOOL = 0x08,
   BSON_TYPE_DATE_TIME = 0x09,
   BSON_TYPE_NULL = 0x0A,
   BSON_TYPE_REGEX = 0x0B,
   BSON_TYPE_DBPOINTER = 0x0C,
   BSON_TYPE_CODE = 0x0D,
   BSON_TYPE_SYMBOL = 0x0E,
   BSON_TYPE_CODEWSCOPE = 0x0F,
   BSON_TYPE_INT32 = 0x10,
   BSON_TYPE_TIMESTAMP = 0x11,
   BSON_TYPE_INT64 = 0x12,
   BSON_TYPE_MAXKEY = 0x7F,
   BSON_TYPE_MINKEY = 0xFF,
} bson_type_t;


/**
 * bson_subtype_t:
 *
 * This enumeration contains the various subtypes that may be used in a binary
 * field. See http://bsonspec.org for more information.
 */
typedef enum
{
   BSON_SUBTYPE_BINARY = 0x00,
   BSON_SUBTYPE_FUNCTION = 0x01,
   BSON_SUBTYPE_BINARY_DEPRECATED = 0x02,
   BSON_SUBTYPE_UUID_DEPRECATED = 0x03,
   BSON_SUBTYPE_UUID = 0x04,
   BSON_SUBTYPE_MD5 = 0x05,
   BSON_SUBTYPE_USER = 0x80,
} bson_subtype_t;


/*
 *--------------------------------------------------------------------------
 *
 * bson_value_t --
 *
 *       A boxed type to contain various bson_type_t types.
 *
 * See also:
 *       bson_value_copy()
 *       bson_value_destroy()
 *
 *--------------------------------------------------------------------------
 */

BSON_ALIGNED_BEGIN (8)
typedef struct _bson_value_t
{
   bson_type_t           value_type;
   int32_t               padding;
   union {
      bson_oid_t         v_oid;
      int64_t            v_int64;
      int32_t            v_int32;
      int8_t             v_int8;
      double             v_double;
      bool               v_bool;
      int64_t            v_datetime;
      struct {
         uint32_t        timestamp;
         uint32_t        increment;
      } v_timestamp;
      struct {
         char           *str;
         uint32_t        len;
      } v_utf8;
      struct {
         uint8_t        *data;
         uint32_t        data_len;
      } v_doc;
      struct {
         uint8_t        *data;
         uint32_t        data_len;
         bson_subtype_t  subtype;
      } v_binary;
      struct {
         char           *regex;
         char           *options;
      } v_regex;
      struct {
         char           *collection;
         uint32_t        collection_len;
         bson_oid_t      oid;
      } v_dbpointer;
      struct {
         char           *code;
         uint32_t        code_len;
      } v_code;
      struct {
         char           *code;
         uint8_t        *scope_data;
         uint32_t        code_len;
         uint32_t        scope_len;
      } v_codewscope;
      struct {
         char           *symbol;
         uint32_t        len;
      } v_symbol;
   } value;
} bson_value_t
BSON_ALIGNED_END (8);


/**
 * bson_iter_t:
 *
 * This structure manages iteration over a bson_t structure. It keeps track
 * of the location of the current key and value within the buffer. Using the
 * various functions to get the value of the iter will read from these
 * locations.
 *
 * This structure is safe to discard on the stack. No cleanup is necessary
 * after using it.
 */
BSON_ALIGNED_BEGIN (128)
typedef struct
{
   const uint8_t *raw;      /* The raw buffer being iterated. */
   uint32_t       len;      /* The length of raw. */
   uint32_t       off;      /* The offset within the buffer. */
   uint32_t       type;     /* The offset of the type byte. */
   uint32_t       key;      /* The offset of the key byte. */
   uint32_t       d1;       /* The offset of the first data byte. */
   uint32_t       d2;       /* The offset of the second data byte. */
   uint32_t       d3;       /* The offset of the third data byte. */
   uint32_t       d4;       /* The offset of the fourth data byte. */
   uint32_t       next_off; /* The offset of the next field. */
   uint32_t       err_off;  /* The offset of the error. */
   bson_value_t   value;    /* Internal value for various state. */
} bson_iter_t
BSON_ALIGNED_END (128);


/**
 * bson_reader_t:
 *
 * This structure is used to iterate over a sequence of BSON documents. It
 * allows for them to be iterated with the possibility of no additional
 * memory allocations under certain circumstances such as reading from an
 * incoming mongo packet.
 */

BSON_ALIGNED_BEGIN (BSON_ALIGN_OF_PTR)
typedef struct
{
   uint32_t type;
   /*< private >*/
} bson_reader_t
BSON_ALIGNED_END (BSON_ALIGN_OF_PTR);


/**
 * bson_visitor_t:
 *
 * This structure contains a series of pointers that can be executed for
 * each field of a BSON document based on the field type.
 *
 * For example, if an int32 field is found, visit_int32 will be called.
 *
 * When visiting each field using bson_iter_visit_all(), you may provide a
 * data pointer that will be provided with each callback. This might be useful
 * if you are marshaling to another language.
 *
 * You may pre-maturely stop the visitation of fields by returning true in your
 * visitor. Returning false will continue visitation to further fields.
 */
BSON_ALIGNED_BEGIN (8)
typedef struct
{
   bool (*visit_before)     (const bson_iter_t *iter,
                             const char        *key,
                             void              *data);
   bool (*visit_after)      (const bson_iter_t *iter,
                             const char        *key,
                             void              *data);
   void (*visit_corrupt)    (const bson_iter_t *iter,
                             void              *data);
   bool (*visit_double)     (const bson_iter_t *iter,
                             const char        *key,
                             double             v_double,
                             void              *data);
   bool (*visit_utf8)       (const bson_iter_t *iter,
                             const char        *key,
                             size_t             v_utf8_len,
                             const char        *v_utf8,
                             void              *data);
   bool (*visit_document)   (const bson_iter_t *iter,
                             const char        *key,
                             const bson_t      *v_document,
                             void              *data);
   bool (*visit_array)      (const bson_iter_t *iter,
                             const char        *key,
                             const bson_t      *v_array,
                             void              *data);
   bool (*visit_binary)     (const bson_iter_t *iter,
                             const char        *key,
                             bson_subtype_t     v_subtype,
                             size_t             v_binary_len,
                             const uint8_t     *v_binary,
                             void              *data);
   bool (*visit_undefined)  (const bson_iter_t *iter,
                             const char        *key,
                             void              *data);
   bool (*visit_oid)        (const bson_iter_t *iter,
                             const char        *key,
                             const bson_oid_t  *v_oid,
                             void              *data);
   bool (*visit_bool)       (const bson_iter_t *iter,
                             const char        *key,
                             bool               v_bool,
                             void              *data);
   bool (*visit_date_time)  (const bson_iter_t *iter,
                             const char        *key,
                             int64_t            msec_since_epoch,
                             void              *data);
   bool (*visit_null)       (const bson_iter_t *iter,
                             const char        *key,
                             void              *data);
   bool (*visit_regex)      (const bson_iter_t *iter,
                             const char        *key,
                             const char        *v_regex,
                             const char        *v_options,
                             void              *data);
   bool (*visit_dbpointer)  (const bson_iter_t *iter,
                             const char        *key,
                             size_t             v_collection_len,
                             const char        *v_collection,
                             const bson_oid_t  *v_oid,
                             void              *data);
   bool (*visit_code)       (const bson_iter_t *iter,
                             const char        *key,
                             size_t             v_code_len,
                             const char        *v_code,
                             void              *data);
   bool (*visit_symbol)     (const bson_iter_t *iter,
                             const char        *key,
                             size_t             v_symbol_len,
                             const char        *v_symbol,
                             void              *data);
   bool (*visit_codewscope) (const bson_iter_t *iter,
                             const char        *key,
                             size_t             v_code_len,
                             const char        *v_code,
                             const bson_t      *v_scope,
                             void              *data);
   bool (*visit_int32)      (const bson_iter_t *iter,
                             const char        *key,
                             int32_t            v_int32,
                             void              *data);
   bool (*visit_timestamp)  (const bson_iter_t *iter,
                             const char        *key,
                             uint32_t           v_timestamp,
                             uint32_t           v_increment,
                             void              *data);
   bool (*visit_int64)      (const bson_iter_t *iter,
                             const char        *key,
                             int64_t            v_int64,
                             void              *data);
   bool (*visit_maxkey)     (const bson_iter_t *iter,
                             const char        *key,
                             void              *data);
   bool (*visit_minkey)     (const bson_iter_t *iter,
                             const char        *key,
                             void              *data);

   void *padding[9];
} bson_visitor_t
BSON_ALIGNED_END (8);


BSON_ALIGNED_BEGIN (8)
typedef struct _bson_error_t
{
   uint32_t domain;
   uint32_t code;
   char     message[504];
} bson_error_t
BSON_ALIGNED_END (8);


BSON_STATIC_ASSERT (sizeof (bson_error_t) == 512);


/**
 * bson_next_power_of_two:
 * @v: A 32-bit unsigned integer of required bytes.
 *
 * Determines the next larger power of two for the value of @v
 * in a constant number of operations.
 *
 * It is up to the caller to guarantee this will not overflow.
 *
 * Returns: The next power of 2 from @v.
 */
static BSON_INLINE size_t
bson_next_power_of_two (size_t v)
{
   v--;
   v |= v >> 1;
   v |= v >> 2;
   v |= v >> 4;
   v |= v >> 8;
   v |= v >> 16;
#if BSON_WORD_SIZE == 64
   v |= v >> 32;
#endif
   v++;

   return v;
}


static BSON_INLINE bool
bson_is_power_of_two (uint32_t v)
{
   return ((v != 0) && ((v & (v - 1)) == 0));
}


BSON_END_DECLS


#endif /* BSON_TYPES_H */



BSON_BEGIN_DECLS


bson_context_t *bson_context_new         (bson_context_flags_t flags);
void            bson_context_destroy     (bson_context_t *context);
bson_context_t *bson_context_get_default (void) BSON_GNUC_CONST;


BSON_END_DECLS


#endif /* BSON_CONTEXT_H */

// #include "bson-clock.h"
/*
 * Copyright 2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_CLOCK_H
#define BSON_CLOCK_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


// #include "bson-compat.h"

// #include "bson-macros.h"

// #include "bson-types.h"



BSON_BEGIN_DECLS


int64_t bson_get_monotonic_time (void);
int     bson_gettimeofday       (struct timeval *tv);


BSON_END_DECLS


#endif /* BSON_CLOCK_H */

// #include "bson-error.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_ERROR_H
#define BSON_ERROR_H


// #include "bson-compat.h"

// #include "bson-macros.h"

// #include "bson-types.h"



BSON_BEGIN_DECLS


#define BSON_ERROR_JSON   1
#define BSON_ERROR_READER 2

#define BSON_ERROR_BUFFER_SIZE      64


void  bson_set_error  (bson_error_t *error,
                       uint32_t      domain,
                       uint32_t      code,
                       const char   *format,
                       ...) BSON_GNUC_PRINTF (4, 5);
char *bson_strerror_r (int           err_code,
                       char         *buf,
                       size_t        buflen);


BSON_END_DECLS


#endif /* BSON_ERROR_H */

// #include "bson-iter.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_ITER_H
#define BSON_ITER_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


// #include "bson.h"

// #include "bson-endian.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_ENDIAN_H
#define BSON_ENDIAN_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


#if defined(__sun)
# include <sys/byteorder.h>
#endif

// #include "bson-config.h"

// #include "bson-macros.h"

// #include "bson-compat.h"



BSON_BEGIN_DECLS


#define BSON_BIG_ENDIAN    4321
#define BSON_LITTLE_ENDIAN 1234


#if defined(__sun)
# define BSON_UINT16_SWAP_LE_BE(v) BSWAP_16((uint16_t)v)
# define BSON_UINT32_SWAP_LE_BE(v) BSWAP_32((uint32_t)v)
# define BSON_UINT64_SWAP_LE_BE(v) BSWAP_64((uint64_t)v)
#elif defined(__clang__) && defined(__clang_major__) && defined(__clang_minor__) && \
  (__clang_major__ >= 3) && (__clang_minor__ >= 1)
# if __has_builtin(__builtin_bswap16)
#  define BSON_UINT16_SWAP_LE_BE(v) __builtin_bswap16(v)
# endif
# if __has_builtin(__builtin_bswap32)
#  define BSON_UINT32_SWAP_LE_BE(v) __builtin_bswap32(v)
# endif
# if __has_builtin(__builtin_bswap64)
#  define BSON_UINT64_SWAP_LE_BE(v) __builtin_bswap64(v)
# endif
#elif defined(__GNUC__) && (__GNUC__ >= 4)
# if __GNUC__ >= 4 && defined (__GNUC_MINOR__) && __GNUC_MINOR__ >= 3
#  define BSON_UINT32_SWAP_LE_BE(v) __builtin_bswap32 ((uint32_t)v)
#  define BSON_UINT64_SWAP_LE_BE(v) __builtin_bswap64 ((uint64_t)v)
# endif
# if __GNUC__ >= 4 && defined (__GNUC_MINOR__) && __GNUC_MINOR__ >= 8
#  define BSON_UINT16_SWAP_LE_BE(v) __builtin_bswap16 ((uint32_t)v)
# endif
#endif


#ifndef BSON_UINT16_SWAP_LE_BE
# define BSON_UINT16_SWAP_LE_BE(v) __bson_uint16_swap_slow ((uint16_t)v)
#endif


#ifndef BSON_UINT32_SWAP_LE_BE
# define BSON_UINT32_SWAP_LE_BE(v) __bson_uint32_swap_slow ((uint32_t)v)
#endif


#ifndef BSON_UINT64_SWAP_LE_BE
# define BSON_UINT64_SWAP_LE_BE(v) __bson_uint64_swap_slow ((uint64_t)v)
#endif


#if BSON_BYTE_ORDER == BSON_LITTLE_ENDIAN
# define BSON_UINT16_FROM_LE(v)  ((uint16_t)v)
# define BSON_UINT16_TO_LE(v)    ((uint16_t)v)
# define BSON_UINT16_FROM_BE(v)  BSON_UINT16_SWAP_LE_BE (v)
# define BSON_UINT16_TO_BE(v)    BSON_UINT16_SWAP_LE_BE (v)
# define BSON_UINT32_FROM_LE(v)  ((uint32_t)v)
# define BSON_UINT32_TO_LE(v)    ((uint32_t)v)
# define BSON_UINT32_FROM_BE(v)  BSON_UINT32_SWAP_LE_BE (v)
# define BSON_UINT32_TO_BE(v)    BSON_UINT32_SWAP_LE_BE (v)
# define BSON_UINT64_FROM_LE(v)  ((uint64_t)v)
# define BSON_UINT64_TO_LE(v)    ((uint64_t)v)
# define BSON_UINT64_FROM_BE(v)  BSON_UINT64_SWAP_LE_BE (v)
# define BSON_UINT64_TO_BE(v)    BSON_UINT64_SWAP_LE_BE (v)
# define BSON_DOUBLE_FROM_LE(v)  ((double)v)
# define BSON_DOUBLE_TO_LE(v)    ((double)v)
#elif BSON_BYTE_ORDER == BSON_BIG_ENDIAN
# define BSON_UINT16_FROM_LE(v)  BSON_UINT16_SWAP_LE_BE (v)
# define BSON_UINT16_TO_LE(v)    BSON_UINT16_SWAP_LE_BE (v)
# define BSON_UINT16_FROM_BE(v)  ((uint16_t)v)
# define BSON_UINT16_TO_BE(v)    ((uint16_t)v)
# define BSON_UINT32_FROM_LE(v)  BSON_UINT32_SWAP_LE_BE (v)
# define BSON_UINT32_TO_LE(v)    BSON_UINT32_SWAP_LE_BE (v)
# define BSON_UINT32_FROM_BE(v)  ((uint32_t)v)
# define BSON_UINT32_TO_BE(v)    ((uint32_t)v)
# define BSON_UINT64_FROM_LE(v)  BSON_UINT64_SWAP_LE_BE (v)
# define BSON_UINT64_TO_LE(v)    BSON_UINT64_SWAP_LE_BE (v)
# define BSON_UINT64_FROM_BE(v)  ((uint64_t)v)
# define BSON_UINT64_TO_BE(v)    ((uint64_t)v)
# define BSON_DOUBLE_FROM_LE(v)  (__bson_double_swap_slow (v))
# define BSON_DOUBLE_TO_LE(v)    (__bson_double_swap_slow (v))
#else
# error "The endianness of target architecture is unknown."
#endif


/*
 *--------------------------------------------------------------------------
 *
 * __bson_uint16_swap_slow --
 *
 *       Fallback endianness conversion for 16-bit integers.
 *
 * Returns:
 *       The endian swapped version.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static BSON_INLINE uint16_t
__bson_uint16_swap_slow (uint16_t v) /* IN */
{
   return ((v & 0x00FF) << 8) |
          ((v & 0xFF00) >> 8);
}


/*
 *--------------------------------------------------------------------------
 *
 * __bson_uint32_swap_slow --
 *
 *       Fallback endianness conversion for 32-bit integers.
 *
 * Returns:
 *       The endian swapped version.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static BSON_INLINE uint32_t
__bson_uint32_swap_slow (uint32_t v) /* IN */
{
   return ((v & 0x000000FFU) << 24) |
          ((v & 0x0000FF00U) <<  8) |
          ((v & 0x00FF0000U) >>  8) |
          ((v & 0xFF000000U) >> 24);
}


/*
 *--------------------------------------------------------------------------
 *
 * __bson_uint64_swap_slow --
 *
 *       Fallback endianness conversion for 64-bit integers.
 *
 * Returns:
 *       The endian swapped version.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static BSON_INLINE uint64_t
__bson_uint64_swap_slow (uint64_t v) /* IN */
{
   return ((v & 0x00000000000000FFULL) << 56) |
          ((v & 0x000000000000FF00ULL) << 40) |
          ((v & 0x0000000000FF0000ULL) << 24) |
          ((v & 0x00000000FF000000ULL) <<  8) |
          ((v & 0x000000FF00000000ULL) >>  8) |
          ((v & 0x0000FF0000000000ULL) >> 24) |
          ((v & 0x00FF000000000000ULL) >> 40) |
          ((v & 0xFF00000000000000ULL) >> 56);
}


/*
 *--------------------------------------------------------------------------
 *
 * __bson_double_swap_slow --
 *
 *       Fallback endianness conversion for double floating point.
 *
 * Returns:
 *       The endian swapped version.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

BSON_STATIC_ASSERT(sizeof(double) == sizeof(uint64_t));

static BSON_INLINE double
__bson_double_swap_slow (double v) /* IN */
{
   uint64_t uv;

   memcpy(&uv, &v, sizeof(v));
   uv = BSON_UINT64_SWAP_LE_BE(uv);
   memcpy(&v, &uv, sizeof(v));

   return v;
}

BSON_END_DECLS


#endif /* BSON_ENDIAN_H */

// #include "bson-macros.h"

// #include "bson-types.h"



BSON_BEGIN_DECLS


#define BSON_ITER_HOLDS_DOUBLE(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_DOUBLE)

#define BSON_ITER_HOLDS_UTF8(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_UTF8)

#define BSON_ITER_HOLDS_DOCUMENT(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_DOCUMENT)

#define BSON_ITER_HOLDS_ARRAY(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_ARRAY)

#define BSON_ITER_HOLDS_BINARY(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_BINARY)

#define BSON_ITER_HOLDS_UNDEFINED(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_UNDEFINED)

#define BSON_ITER_HOLDS_OID(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_OID)

#define BSON_ITER_HOLDS_BOOL(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_BOOL)

#define BSON_ITER_HOLDS_DATE_TIME(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_DATE_TIME)

#define BSON_ITER_HOLDS_NULL(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_NULL)

#define BSON_ITER_HOLDS_REGEX(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_REGEX)

#define BSON_ITER_HOLDS_DBPOINTER(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_DBPOINTER)

#define BSON_ITER_HOLDS_CODE(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_CODE)

#define BSON_ITER_HOLDS_SYMBOL(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_SYMBOL)

#define BSON_ITER_HOLDS_CODEWSCOPE(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_CODEWSCOPE)

#define BSON_ITER_HOLDS_INT32(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_INT32)

#define BSON_ITER_HOLDS_TIMESTAMP(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_TIMESTAMP)

#define BSON_ITER_HOLDS_INT64(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_INT64)

#define BSON_ITER_HOLDS_MAXKEY(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_MAXKEY)

#define BSON_ITER_HOLDS_MINKEY(iter) \
   (bson_iter_type ((iter)) == BSON_TYPE_MINKEY)

#define BSON_ITER_IS_KEY(iter, key) \
   (0 == strcmp ((key), bson_iter_key ((iter))))


const bson_value_t *
bson_iter_value (bson_iter_t *iter);


/**
 * bson_iter_utf8_len_unsafe:
 * @iter: a bson_iter_t.
 *
 * Returns the length of a string currently pointed to by @iter. This performs
 * no validation so the is responsible for knowing the BSON is valid. Calling
 * bson_validate() is one way to do this ahead of time.
 */
static BSON_INLINE uint32_t
bson_iter_utf8_len_unsafe (const bson_iter_t *iter)
{
   int32_t val;

   memcpy (&val, iter->raw + iter->d1, sizeof (val));
   val = BSON_UINT32_FROM_LE (val);
   return BSON_MAX (0, val - 1);
}


void
bson_iter_array (const bson_iter_t   *iter,
                 uint32_t            *array_len,
                 const uint8_t      **array);


void
bson_iter_binary (const bson_iter_t   *iter,
                  bson_subtype_t      *subtype,
                  uint32_t            *binary_len,
                  const uint8_t      **binary);


const char *
bson_iter_code (const bson_iter_t *iter,
                uint32_t          *length);


/**
 * bson_iter_code_unsafe:
 * @iter: A bson_iter_t.
 * @length: A location for the length of the resulting string.
 *
 * Like bson_iter_code() but performs no integrity checks.
 *
 * Returns: A string that should not be modified or freed.
 */
static BSON_INLINE const char *
bson_iter_code_unsafe (const bson_iter_t *iter,
                       uint32_t          *length)
{
   *length = bson_iter_utf8_len_unsafe (iter);
   return (const char *)(iter->raw + iter->d2);
}


const char *
bson_iter_codewscope (const bson_iter_t   *iter,
                      uint32_t            *length,
                      uint32_t            *scope_len,
                      const uint8_t      **scope);


void
bson_iter_dbpointer (const bson_iter_t *iter,
                     uint32_t          *collection_len,
                     const char       **collection,
                     const bson_oid_t **oid);


void
bson_iter_document (const bson_iter_t   *iter,
                    uint32_t            *document_len,
                    const uint8_t      **document);


double
bson_iter_double (const bson_iter_t *iter);


/**
 * bson_iter_double_unsafe:
 * @iter: A bson_iter_t.
 *
 * Similar to bson_iter_double() but does not perform an integrity checking.
 *
 * Returns: A double.
 */
static BSON_INLINE double
bson_iter_double_unsafe (const bson_iter_t *iter)
{
   double val;

   memcpy (&val, iter->raw + iter->d1, sizeof (val));
   return BSON_DOUBLE_FROM_LE (val);
}


bool
bson_iter_init (bson_iter_t  *iter,
                const bson_t *bson);


bool
bson_iter_init_find (bson_iter_t  *iter,
                     const bson_t *bson,
                     const char   *key);


bool
bson_iter_init_find_case (bson_iter_t  *iter,
                          const bson_t *bson,
                          const char   *key);


int32_t
bson_iter_int32 (const bson_iter_t *iter);


/**
 * bson_iter_int32_unsafe:
 * @iter: A bson_iter_t.
 *
 * Similar to bson_iter_int32() but with no integrity checking.
 *
 * Returns: A 32-bit signed integer.
 */
static BSON_INLINE int32_t
bson_iter_int32_unsafe (const bson_iter_t *iter)
{
   int32_t val;

   memcpy (&val, iter->raw + iter->d1, sizeof (val));
   return BSON_UINT32_FROM_LE (val);
}


int64_t
bson_iter_int64 (const bson_iter_t *iter);


int64_t
bson_iter_as_int64 (const bson_iter_t *iter);


/**
 * bson_iter_int64_unsafe:
 * @iter: a bson_iter_t.
 *
 * Similar to bson_iter_int64() but without integrity checking.
 *
 * Returns: A 64-bit signed integer.
 */
static BSON_INLINE int64_t
bson_iter_int64_unsafe (const bson_iter_t *iter)
{
   int64_t val;

   memcpy (&val, iter->raw + iter->d1, sizeof (val));
   return BSON_UINT64_FROM_LE (val);
}


bool
bson_iter_find (bson_iter_t *iter,
                const char  *key);


bool
bson_iter_find_case (bson_iter_t *iter,
                     const char  *key);


bool
bson_iter_find_descendant (bson_iter_t *iter,
                           const char  *dotkey,
                           bson_iter_t *descendant);


bool
bson_iter_next (bson_iter_t *iter);


const bson_oid_t *
bson_iter_oid (const bson_iter_t *iter);


/**
 * bson_iter_oid_unsafe:
 * @iter: A #bson_iter_t.
 *
 * Similar to bson_iter_oid() but performs no integrity checks.
 *
 * Returns: A #bson_oid_t that should not be modified or freed.
 */
static BSON_INLINE const bson_oid_t *
bson_iter_oid_unsafe (const bson_iter_t *iter)
{
   return (const bson_oid_t *)(iter->raw + iter->d1);
}


const char *
bson_iter_key (const bson_iter_t *iter);


/**
 * bson_iter_key_unsafe:
 * @iter: A bson_iter_t.
 *
 * Similar to bson_iter_key() but performs no integrity checking.
 *
 * Returns: A string that should not be modified or freed.
 */
static BSON_INLINE const char *
bson_iter_key_unsafe (const bson_iter_t *iter)
{
   return (const char *)(iter->raw + iter->key);
}


const char *
bson_iter_utf8 (const bson_iter_t *iter,
                uint32_t          *length);


/**
 * bson_iter_utf8_unsafe:
 *
 * Similar to bson_iter_utf8() but performs no integrity checking.
 *
 * Returns: A string that should not be modified or freed.
 */
static BSON_INLINE const char *
bson_iter_utf8_unsafe (const bson_iter_t *iter,
                       size_t            *length)
{
   *length = bson_iter_utf8_len_unsafe (iter);
   return (const char *)(iter->raw + iter->d2);
}


char *
bson_iter_dup_utf8 (const bson_iter_t *iter,
                    uint32_t          *length);


int64_t
bson_iter_date_time (const bson_iter_t *iter);


time_t
bson_iter_time_t (const bson_iter_t *iter);


/**
 * bson_iter_time_t_unsafe:
 * @iter: A bson_iter_t.
 *
 * Similar to bson_iter_time_t() but performs no integrity checking.
 *
 * Returns: A time_t containing the number of seconds since UNIX epoch
 *          in UTC.
 */
static BSON_INLINE time_t
bson_iter_time_t_unsafe (const bson_iter_t *iter)
{
   return (time_t)(bson_iter_int64_unsafe (iter) / 1000UL);
}


void
bson_iter_timeval (const bson_iter_t *iter,
                   struct timeval    *tv);


/**
 * bson_iter_timeval_unsafe:
 * @iter: A bson_iter_t.
 * @tv: A struct timeval.
 *
 * Similar to bson_iter_timeval() but performs no integrity checking.
 */
static BSON_INLINE void
bson_iter_timeval_unsafe (const bson_iter_t *iter,
                          struct timeval    *tv)
{
   int64_t value = bson_iter_int64_unsafe (iter);
#ifdef BSON_OS_WIN32
   tv->tv_sec = (long) (value / 1000);
#else
   tv->tv_sec = (suseconds_t) (value / 1000);
#endif
   tv->tv_usec = (value % 1000) * 1000;
}


void
bson_iter_timestamp (const bson_iter_t *iter,
                     uint32_t     *timestamp,
                     uint32_t     *increment);


bool
bson_iter_bool (const bson_iter_t *iter);


/**
 * bson_iter_bool_unsafe:
 * @iter: A bson_iter_t.
 *
 * Similar to bson_iter_bool() but performs no integrity checking.
 *
 * Returns: true or false.
 */
static BSON_INLINE bool
bson_iter_bool_unsafe (const bson_iter_t *iter)
{
   char val;

   memcpy (&val, iter->raw + iter->d1, 1);
   return !!val;
}


bool
bson_iter_as_bool (const bson_iter_t *iter);


const char *
bson_iter_regex (const bson_iter_t *iter,
                 const char       **options);


const char *
bson_iter_symbol (const bson_iter_t *iter,
                  uint32_t          *length);


bson_type_t
bson_iter_type (const bson_iter_t *iter);


/**
 * bson_iter_type_unsafe:
 * @iter: A bson_iter_t.
 *
 * Similar to bson_iter_type() but performs no integrity checking.
 *
 * Returns: A bson_type_t.
 */
static BSON_INLINE bson_type_t
bson_iter_type_unsafe (const bson_iter_t *iter)
{
   return (bson_type_t) (iter->raw + iter->type) [0];
}


bool
bson_iter_recurse (const bson_iter_t *iter,
                   bson_iter_t       *child);


void
bson_iter_overwrite_int32 (bson_iter_t *iter,
                           int32_t value);


void
bson_iter_overwrite_int64 (bson_iter_t *iter,
                           int64_t value);


void
bson_iter_overwrite_double (bson_iter_t *iter,
                            double       value);


void
bson_iter_overwrite_bool (bson_iter_t *iter,
                          bool  value);


bool
bson_iter_visit_all (bson_iter_t          *iter,
                     const bson_visitor_t *visitor,
                     void                 *data);


BSON_END_DECLS


#endif /* BSON_ITER_H */

// #include "bson-json.h"
/*
 * Copyright 2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_JSON_H
#define BSON_JSON_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


// #include "bson.h"



BSON_BEGIN_DECLS


typedef struct _bson_json_reader_t bson_json_reader_t;


typedef enum
{
   BSON_JSON_ERROR_READ_CORRUPT_JS = 1,
   BSON_JSON_ERROR_READ_INVALID_PARAM,
   BSON_JSON_ERROR_READ_CB_FAILURE,
} bson_json_error_code_t;


typedef ssize_t (*bson_json_reader_cb) (void    *handle,
                                        uint8_t *buf,
                                        size_t   count);
typedef void    (*bson_json_destroy_cb)(void    *handle);


bson_json_reader_t  *bson_json_reader_new          (void                 *data,
                                                    bson_json_reader_cb   cb,
                                                    bson_json_destroy_cb  dcb,
                                                    bool                  allow_multiple,
                                                    size_t                buf_size);
bson_json_reader_t *bson_json_reader_new_from_fd   (int                   fd,
                                                    bool                  close_on_destroy);
bson_json_reader_t *bson_json_reader_new_from_file (const char           *filename,
                                                    bson_error_t         *error);
void                bson_json_reader_destroy       (bson_json_reader_t   *reader);
int                 bson_json_reader_read          (bson_json_reader_t   *reader,
                                                    bson_t               *bson,
                                                    bson_error_t         *error);
bson_json_reader_t *bson_json_data_reader_new      (bool                  allow_multiple,
                                                    size_t                size);
void                bson_json_data_reader_ingest   (bson_json_reader_t   *reader,
                                                    const uint8_t        *data,
                                                    size_t                len);


BSON_END_DECLS


#endif /* BSON_JSON_H */

// #include "bson-keys.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_KEYS_H
#define BSON_KEYS_H


// #include "bson-macros.h"

// #include "bson-types.h"



BSON_BEGIN_DECLS


size_t bson_uint32_to_string (uint32_t     value,
                              const char **strptr,
                              char        *str,
                              size_t       size);


BSON_END_DECLS


#endif /* BSON_KEYS_H */

// #include "bson-md5.h"
/*
  Copyright (C) 1999, 2002 Aladdin Enterprises.  All rights reserved.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  L. Peter Deutsch
  ghost@aladdin.com

 */
/* $Id: md5.h,v 1.4 2002/04/13 19:20:28 lpd Exp $ */
/*
  Independent implementation of MD5 (RFC 1321).

  This code implements the MD5 Algorithm defined in RFC 1321, whose
  text is available at
    http://www.ietf.org/rfc/rfc1321.txt
  The code is derived from the text of the RFC, including the test suite
  (section A.5) but excluding the rest of Appendix A.  It does not include
  any code or documentation that is identified in the RFC as being
  copyrighted.

  The original and principal author of md5.h is L. Peter Deutsch
  <ghost@aladdin.com>.  Other authors are noted in the change history
  that follows (in reverse chronological order):

  2002-04-13 lpd Removed support for non-ANSI compilers; removed
    references to Ghostscript; clarified derivation from RFC 1321;
    now handles byte order either statically or dynamically.
  1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
  1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5);
    added conditionalization for C++ compilation from Martin
    Purschke <purschke@bnl.gov>.
  1999-05-03 lpd Original version.
 */


/*
 * The following MD5 implementation has been modified to use types as
 * specified in libbson.
 */


#ifndef BSON_MD5_H
#define BSON_MD5_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
#  error "Only <bson.h> can be included directly."
#endif


// #include "bson-endian.h"



BSON_BEGIN_DECLS


typedef struct
{
   uint32_t count[2]; /* message length in bits, lsw first */
   uint32_t abcd[4];  /* digest buffer */
   uint8_t  buf[64];  /* accumulate block */
} bson_md5_t;


void bson_md5_init   (bson_md5_t         *pms);
void bson_md5_append (bson_md5_t         *pms,
                      const uint8_t *data,
                      uint32_t       nbytes);
void bson_md5_finish (bson_md5_t         *pms,
                      uint8_t        digest[16]);


BSON_END_DECLS


#endif /* BSON_MD5_H */

// #include "bson-memory.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_MEMORY_H
#define BSON_MEMORY_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


// #include "bson-macros.h"

// #include "bson-types.h"



BSON_BEGIN_DECLS


typedef void *(*bson_realloc_func) (void  *mem,
                                    size_t num_bytes,
                                    void  *ctx);


typedef struct _bson_mem_vtable_t
{
   void *(*malloc)    (size_t  num_bytes);
   void *(*calloc)    (size_t  n_members,
                       size_t  num_bytes);
   void *(*realloc)   (void   *mem,
                       size_t  num_bytes);
   void  (*free)      (void   *mem);
   void *padding [4];
} bson_mem_vtable_t;


void  bson_mem_set_vtable (const bson_mem_vtable_t *vtable);
void  bson_mem_restore_vtable (void);
void *bson_malloc         (size_t  num_bytes);
void *bson_malloc0        (size_t  num_bytes);
void *bson_realloc        (void   *mem,
                           size_t  num_bytes);
void *bson_realloc_ctx    (void   *mem,
                           size_t  num_bytes,
                           void   *ctx);
void  bson_free           (void   *mem);
void  bson_zero_free      (void   *mem,
                           size_t  size);


BSON_END_DECLS


#endif /* BSON_MEMORY_H */

// #include "bson-oid.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_OID_H
#define BSON_OID_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


#include <time.h>

// #include "bson-context.h"

// #include "bson-macros.h"

// #include "bson-types.h"

// #include "bson-endian.h"



BSON_BEGIN_DECLS


int      bson_oid_compare          (const bson_oid_t *oid1,
                                    const bson_oid_t *oid2);
void     bson_oid_copy             (const bson_oid_t *src,
                                    bson_oid_t       *dst);
bool     bson_oid_equal            (const bson_oid_t *oid1,
                                    const bson_oid_t *oid2);
bool     bson_oid_is_valid         (const char       *str,
                                    size_t            length);
time_t   bson_oid_get_time_t       (const bson_oid_t *oid);
uint32_t bson_oid_hash             (const bson_oid_t *oid);
void     bson_oid_init             (bson_oid_t       *oid,
                                    bson_context_t   *context);
void     bson_oid_init_from_data   (bson_oid_t       *oid,
                                    const uint8_t    *data);
void     bson_oid_init_from_string (bson_oid_t       *oid,
                                    const char       *str);
void     bson_oid_init_sequence    (bson_oid_t       *oid,
                                    bson_context_t   *context);
void     bson_oid_to_string        (const bson_oid_t *oid,
                                    char              str[25]);


/**
 * bson_oid_compare_unsafe:
 * @oid1: A bson_oid_t.
 * @oid2: A bson_oid_t.
 *
 * Performs a qsort() style comparison between @oid1 and @oid2.
 *
 * This function is meant to be as fast as possible and therefore performs
 * no argument validation. That is the callers responsibility.
 *
 * Returns: An integer < 0 if @oid1 is less than @oid2. Zero if they are equal.
 *          An integer > 0 if @oid1 is greater than @oid2.
 */
static BSON_INLINE int
bson_oid_compare_unsafe (const bson_oid_t *oid1,
                         const bson_oid_t *oid2)
{
   return memcmp (oid1, oid2, sizeof *oid1);
}


/**
 * bson_oid_equal_unsafe:
 * @oid1: A bson_oid_t.
 * @oid2: A bson_oid_t.
 *
 * Checks the equality of @oid1 and @oid2.
 *
 * This function is meant to be as fast as possible and therefore performs
 * no checks for argument validity. That is the callers responsibility.
 *
 * Returns: true if @oid1 and @oid2 are equal; otherwise false.
 */
static BSON_INLINE bool
bson_oid_equal_unsafe (const bson_oid_t *oid1,
                       const bson_oid_t *oid2)
{
   return !memcmp (oid1, oid2, sizeof *oid1);
}

/**
 * bson_oid_hash_unsafe:
 * @oid: A bson_oid_t.
 *
 * This function performs a DJB style hash upon the bytes contained in @oid.
 * The result is a hash key suitable for use in a hashtable.
 *
 * This function is meant to be as fast as possible and therefore performs no
 * validation of arguments. The caller is responsible to ensure they are
 * passing valid arguments.
 *
 * Returns: A uint32_t containing a hash code.
 */
static BSON_INLINE uint32_t
bson_oid_hash_unsafe (const bson_oid_t *oid)
{
   uint32_t hash = 5381;
   uint32_t i;

   for (i = 0; i < sizeof oid->bytes; i++) {
      hash = ((hash << 5) + hash) + oid->bytes[i];
   }

   return hash;
}


/**
 * bson_oid_copy_unsafe:
 * @src: A bson_oid_t to copy from.
 * @dst: A bson_oid_t to copy into.
 *
 * Copies the contents of @src into @dst. This function is meant to be as
 * fast as possible and therefore performs no argument checking. It is the
 * callers responsibility to ensure they are passing valid data into the
 * function.
 */
static BSON_INLINE void
bson_oid_copy_unsafe (const bson_oid_t *src,
                      bson_oid_t       *dst)
{
   memcpy (dst, src, sizeof *src);
}


/**
 * bson_oid_parse_hex_char:
 * @hex: A character to parse to its integer value.
 *
 * This function contains a jump table to return the integer value for a
 * character containing a hexidecimal value (0-9, a-f, A-F). If the character
 * is not a hexidecimal character then zero is returned.
 *
 * Returns: An integer between 0 and 15.
 */
static BSON_INLINE uint8_t
bson_oid_parse_hex_char (char hex)
{
   switch (hex) {
   case '0':
      return 0;
   case '1':
      return 1;
   case '2':
      return 2;
   case '3':
      return 3;
   case '4':
      return 4;
   case '5':
      return 5;
   case '6':
      return 6;
   case '7':
      return 7;
   case '8':
      return 8;
   case '9':
      return 9;
   case 'a':
   case 'A':
      return 0xa;
   case 'b':
   case 'B':
      return 0xb;
   case 'c':
   case 'C':
      return 0xc;
   case 'd':
   case 'D':
      return 0xd;
   case 'e':
   case 'E':
      return 0xe;
   case 'f':
   case 'F':
      return 0xf;
   default:
      return 0;
   }
}


/**
 * bson_oid_init_from_string_unsafe:
 * @oid: A bson_oid_t to store the result.
 * @str: A 24-character hexidecimal encoded string.
 *
 * Parses a string containing 24 hexidecimal encoded bytes into a bson_oid_t.
 * This function is meant to be as fast as possible and inlined into your
 * code. For that purpose, the function does not perform any sort of bounds
 * checking and it is the callers responsibility to ensure they are passing
 * valid input to the function.
 */
static BSON_INLINE void
bson_oid_init_from_string_unsafe (bson_oid_t *oid,
                                  const char *str)
{
   int i;

   for (i = 0; i < 12; i++) {
      oid->bytes[i] = ((bson_oid_parse_hex_char (str[2 * i]) << 4) |
                       (bson_oid_parse_hex_char (str[2 * i + 1])));
   }
}


/**
 * bson_oid_get_time_t_unsafe:
 * @oid: A bson_oid_t.
 *
 * Fetches the time @oid was generated.
 *
 * Returns: A time_t containing the UNIX timestamp of generation.
 */
static BSON_INLINE time_t
bson_oid_get_time_t_unsafe (const bson_oid_t *oid)
{
   uint32_t t;

   memcpy (&t, oid, sizeof (t));
   return BSON_UINT32_FROM_BE (t);
}


BSON_END_DECLS


#endif /* BSON_OID_H */

// #include "bson-reader.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_READER_H
#define BSON_READER_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


// #include "bson-compat.h"

// #include "bson-oid.h"

// #include "bson-types.h"



BSON_BEGIN_DECLS


#define BSON_ERROR_READER_BADFD 1


/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_read_func_t --
 *
 *       This function is a callback used by bson_reader_t to read the
 *       next chunk of data from the underlying opaque file descriptor.
 *
 *       This function is meant to operate similar to the read() function
 *       as part of libc on UNIX-like systems.
 *
 * Parameters:
 *       @handle: The handle to read from.
 *       @buf: The buffer to read into.
 *       @count: The number of bytes to read.
 *
 * Returns:
 *       0 for end of stream.
 *       -1 for read failure.
 *       Greater than zero for number of bytes read into @buf.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

typedef ssize_t (*bson_reader_read_func_t) (void  *handle, /* IN */
                                            void  *buf,    /* IN */
                                            size_t count); /* IN */


/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_destroy_func_t --
 *
 *       Destroy callback to release any resources associated with the
 *       opaque handle.
 *
 * Parameters:
 *       @handle: the handle provided to bson_reader_new_from_handle().
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

typedef void (*bson_reader_destroy_func_t) (void *handle); /* IN */


bson_reader_t *bson_reader_new_from_handle  (void                       *handle,
                                             bson_reader_read_func_t     rf,
                                             bson_reader_destroy_func_t  df);
bson_reader_t *bson_reader_new_from_fd      (int                         fd,
                                             bool                        close_on_destroy);
bson_reader_t *bson_reader_new_from_file    (const char                 *path,
                                             bson_error_t               *error);
bson_reader_t *bson_reader_new_from_data    (const uint8_t              *data,
                                             size_t                      length);
void           bson_reader_destroy          (bson_reader_t              *reader);
void           bson_reader_set_read_func    (bson_reader_t              *reader,
                                             bson_reader_read_func_t     func);
void           bson_reader_set_destroy_func (bson_reader_t              *reader,
                                             bson_reader_destroy_func_t  func);
const bson_t  *bson_reader_read             (bson_reader_t              *reader,
                                             bool                       *reached_eof);
off_t          bson_reader_tell             (bson_reader_t              *reader);


BSON_END_DECLS


#endif /* BSON_READER_H */

// #include "bson-string.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_STRING_H
#define BSON_STRING_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


#include <stdarg.h>

// #include "bson-macros.h"

// #include "bson-types.h"



BSON_BEGIN_DECLS


typedef struct
{
   char     *str;
   uint32_t  len;
   uint32_t  alloc;
} bson_string_t;


bson_string_t *bson_string_new            (const char      *str);
char          *bson_string_free           (bson_string_t   *string,
                                           bool             free_segment);
void           bson_string_append         (bson_string_t   *string,
                                           const char      *str);
void           bson_string_append_c       (bson_string_t   *string,
                                           char             str);
void           bson_string_append_unichar (bson_string_t   *string,
                                           bson_unichar_t   unichar);
void           bson_string_append_printf  (bson_string_t   *string,
                                           const char      *format,
                                           ...) BSON_GNUC_PRINTF (2, 3);
void           bson_string_truncate       (bson_string_t  *string,
                                           uint32_t        len);
char          *bson_strdup                (const char     *str);
char          *bson_strdup_printf         (const char     *format,
                                           ...) BSON_GNUC_PRINTF (1, 2);
char          *bson_strdupv_printf        (const char     *format,
                                           va_list         args) BSON_GNUC_PRINTF (1, 0);
char          *bson_strndup               (const char     *str,
                                           size_t          n_bytes);
void           bson_strncpy               (char           *dst,
                                           const char     *src,
                                           size_t          size);
int            bson_vsnprintf             (char           *str,
                                           size_t          size,
                                           const char     *format,
                                           va_list         ap) BSON_GNUC_PRINTF (3, 0);
int            bson_snprintf              (char           *str,
                                           size_t          size,
                                           const char     *format,
                                           ...) BSON_GNUC_PRINTF (3, 4);
void           bson_strfreev              (char          **strv);
size_t         bson_strnlen               (const char     *s,
                                           size_t          maxlen);
int64_t        bson_ascii_strtoll         (const char     *str,
                                           char          **endptr,
                                           int             base);


BSON_END_DECLS


#endif /* BSON_STRING_H */

// #include "bson-types.h"

// #include "bson-utf8.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_UTF8_H
#define BSON_UTF8_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


// #include "bson-macros.h"

// #include "bson-types.h"



BSON_BEGIN_DECLS


bool            bson_utf8_validate        (const char     *utf8,
                                           size_t          utf8_len,
                                           bool            allow_null);
char           *bson_utf8_escape_for_json (const char     *utf8,
                                           ssize_t         utf8_len);
bson_unichar_t  bson_utf8_get_char        (const char     *utf8);
const char     *bson_utf8_next_char       (const char     *utf8);
void            bson_utf8_from_unichar    (bson_unichar_t  unichar,
                                           char            utf8[6],
                                           uint32_t       *len);


BSON_END_DECLS


#endif /* BSON_UTF8_H */

// #include "bson-value.h"
/*
 * Copyright 2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_VALUE_H
#define BSON_VALUE_H


// #include "bson-macros.h"

// #include "bson-types.h"



BSON_BEGIN_DECLS


void bson_value_copy    (const bson_value_t *src,
                         bson_value_t       *dst);
void bson_value_destroy (bson_value_t       *value);


BSON_END_DECLS


#endif /* BSON_VALUE_H */

// #include "bson-version.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
#error "Only <bson.h> can be included directly."
#endif


#ifndef BSON_VERSION_H
#define BSON_VERSION_H


/**
 * BSON_MAJOR_VERSION:
 *
 * BSON major version component (e.g. 1 if %BSON_VERSION is 1.2.3)
 */
#define BSON_MAJOR_VERSION (1)


/**
 * BSON_MINOR_VERSION:
 *
 * BSON minor version component (e.g. 2 if %BSON_VERSION is 1.2.3)
 */
#define BSON_MINOR_VERSION (3)


/**
 * BSON_MICRO_VERSION:
 *
 * BSON micro version component (e.g. 3 if %BSON_VERSION is 1.2.3)
 */
#define BSON_MICRO_VERSION (5)


/**
 * BSON_PRERELEASE_VERSION:
 *
 * BSON prerelease version component (e.g. rc0 if %BSON_VERSION is 1.2.3-rc0)
 */
#define BSON_PRERELEASE_VERSION ()

/**
 * BSON_VERSION:
 *
 * BSON version.
 */
#define BSON_VERSION (1.3.5)


/**
 * BSON_VERSION_S:
 *
 * BSON version, encoded as a string, useful for printing and
 * concatenation.
 */
#define BSON_VERSION_S "1.3.5"


/**
 * BSON_VERSION_HEX:
 *
 * BSON version, encoded as an hexadecimal number, useful for
 * integer comparisons.
 */
#define BSON_VERSION_HEX (BSON_MAJOR_VERSION << 24 | \
                          BSON_MINOR_VERSION << 16 | \
                          BSON_MICRO_VERSION << 8)


/**
 * BSON_CHECK_VERSION:
 * @major: required major version
 * @minor: required minor version
 * @micro: required micro version
 *
 * Compile-time version checking. Evaluates to %TRUE if the version
 * of BSON is greater than the required one.
 */
#define BSON_CHECK_VERSION(major,minor,micro)   \
        (BSON_MAJOR_VERSION > (major) || \
         (BSON_MAJOR_VERSION == (major) && BSON_MINOR_VERSION > (minor)) || \
         (BSON_MAJOR_VERSION == (major) && BSON_MINOR_VERSION == (minor) && \
          BSON_MICRO_VERSION >= (micro)))

#endif /* BSON_VERSION_H */

// #include "bson-version-functions.h"
/*
 * Copyright 2015 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
#error "Only <bson.h> can be included directly."
#endif


#ifndef BSON_VERSION_FUNCTIONS_H
#define BSON_VERSION_FUNCTIONS_H

// #include "bson-types.h"


int bson_get_major_version (void);
int bson_get_minor_version (void);
int bson_get_micro_version (void);
const char *bson_get_version (void);
bool bson_check_version (int required_major,
                           int required_minor,
                           int required_micro);


#endif /* BSON_VERSION_FUNCTIONS_H */

// #include "bson-writer.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_WRITER_H
#define BSON_WRITER_H


// #include "bson.h"



BSON_BEGIN_DECLS


/**
 * bson_writer_t:
 *
 * The bson_writer_t structure is a helper for writing a series of BSON
 * documents to a single malloc() buffer. You can provide a realloc() style
 * function to grow the buffer as you go.
 *
 * This is useful if you want to build a series of BSON documents right into
 * the target buffer for an outgoing packet. The offset parameter allows you to
 * start at an offset of the target buffer.
 */
typedef struct _bson_writer_t bson_writer_t;


bson_writer_t *bson_writer_new        (uint8_t           **buf,
                                       size_t             *buflen,
                                       size_t              offset,
                                       bson_realloc_func   realloc_func,
                                       void               *realloc_func_ctx);
void           bson_writer_destroy    (bson_writer_t      *writer);
size_t         bson_writer_get_length (bson_writer_t      *writer);
bool           bson_writer_begin      (bson_writer_t      *writer,
                                       bson_t            **bson);
void           bson_writer_end        (bson_writer_t      *writer);
void           bson_writer_rollback   (bson_writer_t      *writer);


BSON_END_DECLS


#endif /* BSON_WRITER_H */

// #include "bcon.h"


#undef BSON_INSIDE


BSON_BEGIN_DECLS


/**
 * bson_empty:
 * @b: a bson_t.
 *
 * Checks to see if @b is an empty BSON document. An empty BSON document is
 * a 5 byte document which contains the length (4 bytes) and a single NUL
 * byte indicating end of fields.
 */
#define bson_empty(b) (((b)->len == 5) || !bson_get_data ((b))[4])


/**
 * bson_empty0:
 *
 * Like bson_empty() but treats NULL the same as an empty bson_t document.
 */
#define bson_empty0(b) (!(b) || bson_empty (b))


/**
 * bson_clear:
 *
 * Easily free a bson document and set it to NULL. Use like:
 *
 * bson_t *doc = bson_new();
 * bson_clear (&doc);
 * assert (doc == NULL);
 */
#define bson_clear(bptr) \
   do { \
      if (*(bptr)) { \
         bson_destroy (*(bptr)); \
         *(bptr) = NULL; \
      } \
   } while (0)


/**
 * BSON_MAX_SIZE:
 *
 * The maximum size in bytes of a BSON document.
 */
#define BSON_MAX_SIZE ((size_t)((1U << 31) - 1))


#define BSON_APPEND_ARRAY(b,key,val) \
      bson_append_array (b, key, (int)strlen (key), val)

#define BSON_APPEND_ARRAY_BEGIN(b,key,child) \
      bson_append_array_begin (b, key, (int)strlen (key), child)

#define BSON_APPEND_BINARY(b,key,subtype,val,len) \
      bson_append_binary (b, key, (int) strlen (key), subtype, val, len)

#define BSON_APPEND_BOOL(b,key,val) \
      bson_append_bool (b, key, (int) strlen (key), val)

#define BSON_APPEND_CODE(b,key,val) \
      bson_append_code (b, key, (int) strlen (key), val)

#define BSON_APPEND_CODE_WITH_SCOPE(b,key,val,scope) \
      bson_append_code_with_scope (b, key, (int) strlen (key), val, scope)

#define BSON_APPEND_DBPOINTER(b,key,coll,oid) \
      bson_append_dbpointer (b, key, (int) strlen (key), coll, oid)

#define BSON_APPEND_DOCUMENT_BEGIN(b,key,child) \
      bson_append_document_begin (b, key, (int)strlen (key), child)

#define BSON_APPEND_DOUBLE(b,key,val) \
      bson_append_double (b, key, (int) strlen (key), val)

#define BSON_APPEND_DOCUMENT(b,key,val) \
      bson_append_document (b, key, (int) strlen (key), val)

#define BSON_APPEND_INT32(b,key,val) \
      bson_append_int32 (b, key, (int) strlen (key), val)

#define BSON_APPEND_INT64(b,key,val) \
      bson_append_int64 (b, key, (int) strlen (key), val)

#define BSON_APPEND_MINKEY(b,key) \
      bson_append_minkey (b, key, (int) strlen (key))

#define BSON_APPEND_MAXKEY(b,key) \
      bson_append_maxkey (b, key, (int) strlen (key))

#define BSON_APPEND_NULL(b,key) \
      bson_append_null (b, key, (int) strlen (key))

#define BSON_APPEND_OID(b,key,val) \
      bson_append_oid (b, key, (int) strlen (key), val)

#define BSON_APPEND_REGEX(b,key,val,opt) \
      bson_append_regex (b, key, (int) strlen (key), val, opt)

#define BSON_APPEND_UTF8(b,key,val) \
      bson_append_utf8 (b, key, (int) strlen (key), val, (int) strlen (val))

#define BSON_APPEND_SYMBOL(b,key,val) \
      bson_append_symbol (b, key, (int) strlen (key), val, (int) strlen (val))

#define BSON_APPEND_TIME_T(b,key,val) \
      bson_append_time_t (b, key, (int) strlen (key), val)

#define BSON_APPEND_TIMEVAL(b,key,val) \
      bson_append_timeval (b, key, (int) strlen (key), val)

#define BSON_APPEND_DATE_TIME(b,key,val) \
      bson_append_date_time (b, key, (int) strlen (key), val)

#define BSON_APPEND_TIMESTAMP(b,key,val,inc) \
      bson_append_timestamp (b, key, (int) strlen (key), val, inc)

#define BSON_APPEND_UNDEFINED(b,key) \
      bson_append_undefined (b, key, (int) strlen (key))

#define BSON_APPEND_VALUE(b,key,val) \
      bson_append_value (b, key, (int) strlen (key), (val))


/**
 * bson_new:
 *
 * Allocates a new bson_t structure. Call the various bson_append_*()
 * functions to add fields to the bson. You can iterate the bson_t at any
 * time using a bson_iter_t and bson_iter_init().
 *
 * Returns: A newly allocated bson_t that should be freed with bson_destroy().
 */
bson_t *
bson_new (void);


bson_t *
bson_new_from_json (const uint8_t *data,
                    ssize_t        len,
                    bson_error_t  *error);


bool
bson_init_from_json (bson_t        *bson,
                     const char    *data,
                     ssize_t        len,
                     bson_error_t  *error);


/**
 * bson_init_static:
 * @b: A pointer to a bson_t.
 * @data: The data buffer to use.
 * @length: The length of @data.
 *
 * Initializes a bson_t using @data and @length. This is ideal if you would
 * like to use a stack allocation for your bson and do not need to grow the
 * buffer. @data must be valid for the life of @b.
 *
 * Returns: true if initialized successfully; otherwise false.
 */
bool
bson_init_static (bson_t        *b,
                  const uint8_t *data,
                  size_t         length);


/**
 * bson_init:
 * @b: A pointer to a bson_t.
 *
 * Initializes a bson_t for use. This function is useful to those that want a
 * stack allocated bson_t. The usefulness of a stack allocated bson_t is
 * marginal as the target buffer for content will still require heap
 * allocations. It can help reduce heap fragmentation on allocators that do
 * not employ SLAB/magazine semantics.
 *
 * You must call bson_destroy() with @b to release resources when you are done
 * using @b.
 */
void
bson_init (bson_t *b);


/**
 * bson_reinit:
 * @b: (inout): A bson_t.
 *
 * This is equivalent to calling bson_destroy() and bson_init() on a #bson_t.
 * However, it will try to persist the existing malloc'd buffer if one exists.
 * This is useful in cases where you want to reduce malloc overhead while
 * building many documents.
 */
void
bson_reinit (bson_t *b);


/**
 * bson_new_from_data:
 * @data: A buffer containing a serialized bson document.
 * @length: The length of the document in bytes.
 *
 * Creates a new bson_t structure using the data provided. @data should contain
 * at least @length bytes that can be copied into the new bson_t structure.
 *
 * Returns: A newly allocated bson_t that should be freed with bson_destroy().
 *   If the first four bytes (little-endian) of data do not match @length,
 *   then NULL will be returned.
 */
bson_t *
bson_new_from_data (const uint8_t *data,
                    size_t         length);


/**
 * bson_new_from_buffer:
 * @buf: A pointer to a buffer containing a serialized bson document.  Or null
 * @buf_len: The length of the buffer in bytes.
 * @realloc_fun: a realloc like function
 * @realloc_fun_ctx: a context for the realloc function
 *
 * Creates a new bson_t structure using the data provided. @buf should contain
 * a bson document, or null pointer should be passed for new allocations.
 *
 * Returns: A newly allocated bson_t that should be freed with bson_destroy().
 *          The underlying buffer will be used and not be freed in destroy.
 */
bson_t *
bson_new_from_buffer (uint8_t           **buf,
                      size_t             *buf_len,
                      bson_realloc_func   realloc_func,
                      void               *realloc_func_ctx);


/**
 * bson_sized_new:
 * @size: A size_t containing the number of bytes to allocate.
 *
 * This will allocate a new bson_t with enough bytes to hold a buffer
 * sized @size. @size must be smaller than INT_MAX bytes.
 *
 * Returns: A newly allocated bson_t that should be freed with bson_destroy().
 */
bson_t *
bson_sized_new (size_t size);


/**
 * bson_copy:
 * @bson: A bson_t.
 *
 * Copies @bson into a newly allocated bson_t. You must call bson_destroy()
 * when you are done with the resulting value to free its resources.
 *
 * Returns: A newly allocated bson_t that should be free'd with bson_destroy()
 */
bson_t *
bson_copy (const bson_t *bson);


/**
 * bson_copy_to:
 * @src: The source bson_t.
 * @dst: The destination bson_t.
 *
 * Initializes @dst and copies the content from @src into @dst.
 */
void
bson_copy_to (const bson_t *src,
              bson_t       *dst);


/**
 * bson_copy_to_excluding:
 * @src: A bson_t.
 * @dst: A bson_t to initialize and copy into.
 * @first_exclude: First field name to exclude.
 *
 * Copies @src into @dst excluding any field that is provided.
 * This is handy for situations when you need to remove one or
 * more fields in a bson_t. Note that bson_init() will be called
 * on dst.
 */
void
bson_copy_to_excluding (const bson_t *src,
                        bson_t       *dst,
                        const char   *first_exclude,
                        ...) BSON_GNUC_NULL_TERMINATED BSON_GNUC_DEPRECATED_FOR(bson_copy_to_excluding_noinit);

/**
 * bson_copy_to_excluding_noinit:
 * @src: A bson_t.
 * @dst: A bson_t to initialize and copy into.
 * @first_exclude: First field name to exclude.
 *
 * The same as bson_copy_to_excluding, but does not call bson_init()
 * on the dst. This version should be preferred in new code, but the
 * old function is left for backwards compatibility.
 */
void
bson_copy_to_excluding_noinit (const bson_t *src,
                               bson_t       *dst,
                               const char   *first_exclude,
                               ...) BSON_GNUC_NULL_TERMINATED;

/**
 * bson_destroy:
 * @bson: A bson_t.
 *
 * Frees the resources associated with @bson.
 */
void
bson_destroy (bson_t *bson);


/**
 * bson_destroy_with_steal:
 * @bson: A #bson_t.
 * @steal: If ownership of the data buffer should be transfered to caller.
 * @length: (out): location for the length of the buffer.
 *
 * Destroys @bson similar to calling bson_destroy() except that the underlying
 * buffer will be returned and ownership transfered to the caller if @steal
 * is non-zero.
 *
 * If length is non-NULL, the length of @bson will be stored in @length.
 *
 * It is a programming error to call this function with any bson that has
 * been initialized static, or is being used to create a subdocument with
 * functions such as bson_append_document_begin() or bson_append_array_begin().
 *
 * Returns: a buffer owned by the caller if @steal is true. Otherwise NULL.
 *    If there was an error, NULL is returned.
 */
uint8_t *
bson_destroy_with_steal (bson_t   *bson,
                         bool      steal,
                         uint32_t *length);


/**
 * bson_get_data:
 * @bson: A bson_t.
 *
 * Fetched the data buffer for @bson of @bson->len bytes in length.
 *
 * Returns: A buffer that should not be modified or freed.
 */
const uint8_t *
bson_get_data (const bson_t *bson);


/**
 * bson_count_keys:
 * @bson: A bson_t.
 *
 * Counts the number of elements found in @bson.
 */
uint32_t
bson_count_keys (const bson_t *bson);


/**
 * bson_has_field:
 * @bson: A bson_t.
 * @key: The key to lookup.
 *
 * Checks to see if @bson contains a field named @key.
 *
 * This function is case-sensitive.
 *
 * Returns: true if @key exists in @bson; otherwise false.
 */
bool
bson_has_field (const bson_t *bson,
                const char   *key);


/**
 * bson_compare:
 * @bson: A bson_t.
 * @other: A bson_t.
 *
 * Compares @bson to @other in a qsort() style comparison.
 * See qsort() for information on how this function works.
 *
 * Returns: Less than zero, zero, or greater than zero.
 */
int
bson_compare (const bson_t *bson,
              const bson_t *other);

/*
 * bson_compare:
 * @bson: A bson_t.
 * @other: A bson_t.
 *
 * Checks to see if @bson and @other are equal.
 *
 * Returns: true if equal; otherwise false.
 */
bool
bson_equal (const bson_t *bson,
            const bson_t *other);


/**
 * bson_validate:
 * @bson: A bson_t.
 * @offset: A location for the error offset.
 *
 * Validates a BSON document by walking through the document and inspecting
 * the fields for valid content.
 *
 * Returns: true if @bson is valid; otherwise false and @offset is set.
 */
bool
bson_validate (const bson_t         *bson,
               bson_validate_flags_t flags,
               size_t               *offset);


/**
 * bson_as_json:
 * @bson: A bson_t.
 * @length: A location for the string length, or NULL.
 *
 * Creates a new string containing @bson in extended JSON format. The caller
 * is responsible for freeing the resulting string. If @length is non-NULL,
 * then the length of the resulting string will be placed in @length.
 *
 * See http://docs.mongodb.org/manual/reference/mongodb-extended-json/ for
 * more information on extended JSON.
 *
 * Returns: A newly allocated string that should be freed with bson_free().
 */
char *
bson_as_json (const bson_t *bson,
              size_t       *length);


/* like bson_as_json() but for outermost arrays. */
char *
bson_array_as_json (const bson_t *bson,
                    size_t       *length);


bool
bson_append_value (bson_t             *bson,
                   const char         *key,
                   int                 key_length,
                   const bson_value_t *value);


/**
 * bson_append_array:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @array: A bson_t containing the array.
 *
 * Appends a BSON array to @bson. BSON arrays are like documents where the
 * key is the string version of the index. For example, the first item of the
 * array would have the key "0". The second item would have the index "1".
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_array (bson_t       *bson,
                   const char   *key,
                   int           key_length,
                   const bson_t *array);


/**
 * bson_append_binary:
 * @bson: A bson_t to append.
 * @key: The key for the field.
 * @subtype: The bson_subtype_t of the binary.
 * @binary: The binary buffer to append.
 * @length: The length of @binary.
 *
 * Appends a binary buffer to the BSON document.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_binary (bson_t         *bson,
                    const char     *key,
                    int             key_length,
                    bson_subtype_t  subtype,
                    const uint8_t  *binary,
                    uint32_t        length);


/**
 * bson_append_bool:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @value: The boolean value.
 *
 * Appends a new field to @bson of type BSON_TYPE_BOOL.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_bool (bson_t     *bson,
                  const char *key,
                  int         key_length,
                  bool value);


/**
 * bson_append_code:
 * @bson: A bson_t.
 * @key: The key for the document.
 * @javascript: JavaScript code to be executed.
 *
 * Appends a field of type BSON_TYPE_CODE to the BSON document. @javascript
 * should contain a script in javascript to be executed.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_code (bson_t     *bson,
                  const char *key,
                  int         key_length,
                  const char *javascript);


/**
 * bson_append_code_with_scope:
 * @bson: A bson_t.
 * @key: The key for the document.
 * @javascript: JavaScript code to be executed.
 * @scope: A bson_t containing the scope for @javascript.
 *
 * Appends a field of type BSON_TYPE_CODEWSCOPE to the BSON document.
 * @javascript should contain a script in javascript to be executed.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_code_with_scope (bson_t       *bson,
                             const char   *key,
                             int           key_length,
                             const char   *javascript,
                             const bson_t *scope);


/**
 * bson_append_dbpointer:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @collection: The collection name.
 * @oid: The oid to the reference.
 *
 * Appends a new field of type BSON_TYPE_DBPOINTER. This datum type is
 * deprecated in the BSON spec and should not be used in new code.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_dbpointer (bson_t           *bson,
                       const char       *key,
                       int               key_length,
                       const char       *collection,
                       const bson_oid_t *oid);


/**
 * bson_append_double:
 * @bson: A bson_t.
 * @key: The key for the field.
 *
 * Appends a new field to @bson of the type BSON_TYPE_DOUBLE.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_double (bson_t     *bson,
                    const char *key,
                    int         key_length,
                    double      value);


/**
 * bson_append_document:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @value: A bson_t containing the subdocument.
 *
 * Appends a new field to @bson of the type BSON_TYPE_DOCUMENT.
 * The documents contents will be copied into @bson.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_document (bson_t       *bson,
                      const char   *key,
                      int           key_length,
                      const bson_t *value);


/**
 * bson_append_document_begin:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @key_length: The length of @key in bytes not including NUL or -1
 *    if @key_length is NUL terminated.
 * @child: A location to an uninitialized bson_t.
 *
 * Appends a new field named @key to @bson. The field is, however,
 * incomplete.  @child will be initialized so that you may add fields to the
 * child document.  Child will use a memory buffer owned by @bson and
 * therefore grow the parent buffer as additional space is used. This allows
 * a single malloc'd buffer to be used when building documents which can help
 * reduce memory fragmentation.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_document_begin (bson_t     *bson,
                            const char *key,
                            int         key_length,
                            bson_t     *child);


/**
 * bson_append_document_end:
 * @bson: A bson_t.
 * @child: A bson_t supplied to bson_append_document_begin().
 *
 * Finishes the appending of a document to a @bson. @child is considered
 * disposed after this call and should not be used any further.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_document_end (bson_t *bson,
                          bson_t *child);


/**
 * bson_append_array_begin:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @key_length: The length of @key in bytes not including NUL or -1
 *    if @key_length is NUL terminated.
 * @child: A location to an uninitialized bson_t.
 *
 * Appends a new field named @key to @bson. The field is, however,
 * incomplete. @child will be initialized so that you may add fields to the
 * child array. Child will use a memory buffer owned by @bson and
 * therefore grow the parent buffer as additional space is used. This allows
 * a single malloc'd buffer to be used when building arrays which can help
 * reduce memory fragmentation.
 *
 * The type of @child will be BSON_TYPE_ARRAY and therefore the keys inside
 * of it MUST be "0", "1", etc.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_array_begin (bson_t     *bson,
                         const char *key,
                         int         key_length,
                         bson_t     *child);


/**
 * bson_append_array_end:
 * @bson: A bson_t.
 * @child: A bson_t supplied to bson_append_array_begin().
 *
 * Finishes the appending of a array to a @bson. @child is considered
 * disposed after this call and should not be used any further.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_array_end (bson_t *bson,
                       bson_t *child);


/**
 * bson_append_int32:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @value: The int32_t 32-bit integer value.
 *
 * Appends a new field of type BSON_TYPE_INT32 to @bson.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_int32 (bson_t      *bson,
                   const char  *key,
                   int          key_length,
                   int32_t value);


/**
 * bson_append_int64:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @value: The int64_t 64-bit integer value.
 *
 * Appends a new field of type BSON_TYPE_INT64 to @bson.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_int64 (bson_t      *bson,
                   const char  *key,
                   int          key_length,
                   int64_t value);


/**
 * bson_append_iter:
 * @bson: A bson_t to append to.
 * @key: The key name or %NULL to take current key from @iter.
 * @key_length: The key length or -1 to use strlen().
 * @iter: The iter located on the position of the element to append.
 *
 * Appends a new field to @bson that is equivalent to the field currently
 * pointed to by @iter.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_iter (bson_t            *bson,
                  const char        *key,
                  int                key_length,
                  const bson_iter_t *iter);


/**
 * bson_append_minkey:
 * @bson: A bson_t.
 * @key: The key for the field.
 *
 * Appends a new field of type BSON_TYPE_MINKEY to @bson. This is a special
 * type that compares lower than all other possible BSON element values.
 *
 * See http://bsonspec.org for more information on this type.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_minkey (bson_t     *bson,
                    const char *key,
                    int         key_length);


/**
 * bson_append_maxkey:
 * @bson: A bson_t.
 * @key: The key for the field.
 *
 * Appends a new field of type BSON_TYPE_MAXKEY to @bson. This is a special
 * type that compares higher than all other possible BSON element values.
 *
 * See http://bsonspec.org for more information on this type.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_maxkey (bson_t     *bson,
                    const char *key,
                    int         key_length);


/**
 * bson_append_null:
 * @bson: A bson_t.
 * @key: The key for the field.
 *
 * Appends a new field to @bson with NULL for the value.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_null (bson_t     *bson,
                  const char *key,
                  int         key_length);


/**
 * bson_append_oid:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @oid: bson_oid_t.
 *
 * Appends a new field to the @bson of type BSON_TYPE_OID using the contents of
 * @oid.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_oid (bson_t           *bson,
                 const char       *key,
                 int               key_length,
                 const bson_oid_t *oid);


/**
 * bson_append_regex:
 * @bson: A bson_t.
 * @key: The key of the field.
 * @regex: The regex to append to the bson.
 * @options: Options for @regex.
 *
 * Appends a new field to @bson of type BSON_TYPE_REGEX. @regex should
 * be the regex string. @options should contain the options for the regex.
 *
 * Valid options for @options are:
 *
 *   'i' for case-insensitive.
 *   'm' for multiple matching.
 *   'x' for verbose mode.
 *   'l' to make \w and \W locale dependent.
 *   's' for dotall mode ('.' matches everything)
 *   'u' to make \w and \W match unicode.
 *
 * For more information on what comprimises a BSON regex, see bsonspec.org.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_regex (bson_t     *bson,
                   const char *key,
                   int         key_length,
                   const char *regex,
                   const char *options);


/**
 * bson_append_utf8:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @value: A UTF-8 encoded string.
 * @length: The length of @value or -1 if it is NUL terminated.
 *
 * Appends a new field to @bson using @key as the key and @value as the UTF-8
 * encoded value.
 *
 * It is the callers responsibility to ensure @value is valid UTF-8. You can
 * use bson_utf8_validate() to perform this check.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_utf8 (bson_t     *bson,
                  const char *key,
                  int         key_length,
                  const char *value,
                  int         length);


/**
 * bson_append_symbol:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @value: The symbol as a string.
 * @length: The length of @value or -1 if NUL-terminated.
 *
 * Appends a new field to @bson of type BSON_TYPE_SYMBOL. This BSON type is
 * deprecated and should not be used in new code.
 *
 * See http://bsonspec.org for more information on this type.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_symbol (bson_t     *bson,
                    const char *key,
                    int         key_length,
                    const char *value,
                    int         length);


/**
 * bson_append_time_t:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @value: A time_t.
 *
 * Appends a BSON_TYPE_DATE_TIME field to @bson using the time_t @value for the
 * number of seconds since UNIX epoch in UTC.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_time_t (bson_t     *bson,
                    const char *key,
                    int         key_length,
                    time_t      value);


/**
 * bson_append_timeval:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @value: A struct timeval containing the date and time.
 *
 * Appends a BSON_TYPE_DATE_TIME field to @bson using the struct timeval
 * provided. The time is persisted in milliseconds since the UNIX epoch in UTC.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_timeval (bson_t         *bson,
                     const char     *key,
                     int             key_length,
                     struct timeval *value);


/**
 * bson_append_date_time:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @key_length: The length of @key in bytes or -1 if \0 terminated.
 * @value: The number of milliseconds elapsed since UNIX epoch.
 *
 * Appends a new field to @bson of type BSON_TYPE_DATE_TIME.
 *
 * Returns: true if sucessful; otherwise false.
 */
bool
bson_append_date_time (bson_t      *bson,
                       const char  *key,
                       int          key_length,
                       int64_t value);


/**
 * bson_append_now_utc:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @key_length: The length of @key or -1 if it is NULL terminated.
 *
 * Appends a BSON_TYPE_DATE_TIME field to @bson using the current time in UTC
 * as the field value.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_now_utc (bson_t     *bson,
                     const char *key,
                     int         key_length);

/**
 * bson_append_timestamp:
 * @bson: A bson_t.
 * @key: The key for the field.
 * @timestamp: 4 byte timestamp.
 * @increment: 4 byte increment for timestamp.
 *
 * Appends a field of type BSON_TYPE_TIMESTAMP to @bson. This is a special type
 * used by MongoDB replication and sharding. If you need generic time and date
 * fields use bson_append_time_t() or bson_append_timeval().
 *
 * Setting @increment and @timestamp to zero has special semantics. See
 * http://bsonspec.org for more information on this field type.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_timestamp (bson_t       *bson,
                       const char   *key,
                       int           key_length,
                       uint32_t timestamp,
                       uint32_t increment);


/**
 * bson_append_undefined:
 * @bson: A bson_t.
 * @key: The key for the field.
 *
 * Appends a field of type BSON_TYPE_UNDEFINED. This type is deprecated in the
 * spec and should not be used for new code. However, it is provided for those
 * needing to interact with legacy systems.
 *
 * Returns: true if successful; false if append would overflow max size.
 */
bool
bson_append_undefined (bson_t     *bson,
                       const char *key,
                       int         key_length);


bool
bson_concat (bson_t       *dst,
             const bson_t *src);


BSON_END_DECLS


#endif /* BSON_H */



BSON_BEGIN_DECLS


#define BCON_STACK_MAX 100

#define BCON_ENSURE_DECLARE(fun, type) \
   static BSON_INLINE type bcon_ensure_##fun (type _t) { return _t; }

#define BCON_ENSURE(fun, val) \
   bcon_ensure_##fun (val)

#define BCON_ENSURE_STORAGE(fun, val) \
   bcon_ensure_##fun (&(val))

BCON_ENSURE_DECLARE (const_char_ptr, const char *)
BCON_ENSURE_DECLARE (const_char_ptr_ptr, const char **)
BCON_ENSURE_DECLARE (double, double)
BCON_ENSURE_DECLARE (double_ptr, double *)
BCON_ENSURE_DECLARE (const_bson_ptr, const bson_t *)
BCON_ENSURE_DECLARE (bson_ptr, bson_t *)
BCON_ENSURE_DECLARE (subtype, bson_subtype_t)
BCON_ENSURE_DECLARE (subtype_ptr, bson_subtype_t *)
BCON_ENSURE_DECLARE (const_uint8_ptr, const uint8_t *)
BCON_ENSURE_DECLARE (const_uint8_ptr_ptr, const uint8_t **)
BCON_ENSURE_DECLARE (uint32, uint32_t)
BCON_ENSURE_DECLARE (uint32_ptr, uint32_t *)
BCON_ENSURE_DECLARE (const_oid_ptr, const bson_oid_t *)
BCON_ENSURE_DECLARE (const_oid_ptr_ptr, const bson_oid_t **)
BCON_ENSURE_DECLARE (int32, int32_t)
BCON_ENSURE_DECLARE (int32_ptr, int32_t *)
BCON_ENSURE_DECLARE (int64, int64_t)
BCON_ENSURE_DECLARE (int64_ptr, int64_t *)
BCON_ENSURE_DECLARE (bool, bool)
BCON_ENSURE_DECLARE (bool_ptr, bool *)
BCON_ENSURE_DECLARE (bson_type, bson_type_t)
BCON_ENSURE_DECLARE (bson_iter_ptr, bson_iter_t *)
BCON_ENSURE_DECLARE (const_bson_iter_ptr, const bson_iter_t *)

#define BCON_UTF8(_val) \
   BCON_MAGIC, BCON_TYPE_UTF8, BCON_ENSURE (const_char_ptr, (_val))
#define BCON_DOUBLE(_val) \
   BCON_MAGIC, BCON_TYPE_DOUBLE, BCON_ENSURE (double, (_val))
#define BCON_DOCUMENT(_val) \
   BCON_MAGIC, BCON_TYPE_DOCUMENT, BCON_ENSURE (const_bson_ptr, (_val))
#define BCON_ARRAY(_val) \
   BCON_MAGIC, BCON_TYPE_ARRAY, BCON_ENSURE (const_bson_ptr, (_val))
#define BCON_BIN(_subtype, _binary, _length) \
   BCON_MAGIC, BCON_TYPE_BIN, \
   BCON_ENSURE (subtype, (_subtype)), \
   BCON_ENSURE (const_uint8_ptr, (_binary)), \
   BCON_ENSURE (uint32, (_length))
#define BCON_UNDEFINED BCON_MAGIC, BCON_TYPE_UNDEFINED
#define BCON_OID(_val) \
   BCON_MAGIC, BCON_TYPE_OID, BCON_ENSURE (const_oid_ptr, (_val))
#define BCON_BOOL(_val) \
   BCON_MAGIC, BCON_TYPE_BOOL, BCON_ENSURE (bool, (_val))
#define BCON_DATE_TIME(_val) \
   BCON_MAGIC, BCON_TYPE_DATE_TIME, BCON_ENSURE (int64, (_val))
#define BCON_NULL BCON_MAGIC, BCON_TYPE_NULL
#define BCON_REGEX(_regex, _flags) \
   BCON_MAGIC, BCON_TYPE_REGEX, \
   BCON_ENSURE (const_char_ptr, (_regex)), \
   BCON_ENSURE (const_char_ptr, (_flags))
#define BCON_DBPOINTER(_collection, _oid) \
   BCON_MAGIC, BCON_TYPE_DBPOINTER, \
   BCON_ENSURE (const_char_ptr, (_collection)), \
   BCON_ENSURE (const_oid_ptr, (_oid))
#define BCON_CODE(_val) \
   BCON_MAGIC, BCON_TYPE_CODE, BCON_ENSURE (const_char_ptr, (_val))
#define BCON_SYMBOL(_val) \
   BCON_MAGIC, BCON_TYPE_SYMBOL, BCON_ENSURE (const_char_ptr, (_val))
#define BCON_CODEWSCOPE(_js, _scope) \
   BCON_MAGIC, BCON_TYPE_CODEWSCOPE, \
   BCON_ENSURE (const_char_ptr, (_js)), \
   BCON_ENSURE (const_bson_ptr, (_scope))
#define BCON_INT32(_val) \
   BCON_MAGIC, BCON_TYPE_INT32, BCON_ENSURE (int32, (_val))
#define BCON_TIMESTAMP(_timestamp, _increment) \
   BCON_MAGIC, BCON_TYPE_TIMESTAMP, \
   BCON_ENSURE (int32, (_timestamp)), \
   BCON_ENSURE (int32, (_increment))
#define BCON_INT64(_val) \
   BCON_MAGIC, BCON_TYPE_INT64, BCON_ENSURE (int64, (_val))
#define BCON_MAXKEY BCON_MAGIC, BCON_TYPE_MAXKEY
#define BCON_MINKEY BCON_MAGIC, BCON_TYPE_MINKEY
#define BCON(_val) \
   BCON_MAGIC, BCON_TYPE_BCON, BCON_ENSURE (const_bson_ptr, (_val))
#define BCON_ITER(_val) \
   BCON_MAGIC, BCON_TYPE_ITER, BCON_ENSURE (const_bson_iter_ptr, (_val))

#define BCONE_UTF8(_val) BCONE_MAGIC, BCON_TYPE_UTF8, \
   BCON_ENSURE_STORAGE (const_char_ptr_ptr, (_val))
#define BCONE_DOUBLE(_val) BCONE_MAGIC, BCON_TYPE_DOUBLE, \
   BCON_ENSURE_STORAGE (double_ptr, (_val))
#define BCONE_DOCUMENT(_val) BCONE_MAGIC, BCON_TYPE_DOCUMENT, \
   BCON_ENSURE_STORAGE (bson_ptr, (_val))
#define BCONE_ARRAY(_val) BCONE_MAGIC, BCON_TYPE_ARRAY, \
   BCON_ENSURE_STORAGE (bson_ptr, (_val))
#define BCONE_BIN(subtype, binary, length) \
   BCONE_MAGIC, BCON_TYPE_BIN, \
   BCON_ENSURE_STORAGE (subtype_ptr, (subtype)), \
   BCON_ENSURE_STORAGE (const_uint8_ptr_ptr, (binary)), \
   BCON_ENSURE_STORAGE (uint32_ptr, (length))
#define BCONE_UNDEFINED BCONE_MAGIC, BCON_TYPE_UNDEFINED
#define BCONE_OID(_val) BCONE_MAGIC, BCON_TYPE_OID, \
   BCON_ENSURE_STORAGE (const_oid_ptr_ptr, (_val))
#define BCONE_BOOL(_val) BCONE_MAGIC, BCON_TYPE_BOOL, \
   BCON_ENSURE_STORAGE (bool_ptr, (_val))
#define BCONE_DATE_TIME(_val) BCONE_MAGIC, BCON_TYPE_DATE_TIME, \
   BCON_ENSURE_STORAGE (int64_ptr, (_val))
#define BCONE_NULL BCONE_MAGIC, BCON_TYPE_NULL
#define BCONE_REGEX(_regex, _flags) \
   BCONE_MAGIC, BCON_TYPE_REGEX, \
   BCON_ENSURE_STORAGE (const_char_ptr_ptr, (_regex)), \
   BCON_ENSURE_STORAGE (const_char_ptr_ptr, (_flags))
#define BCONE_DBPOINTER(_collection, _oid) \
   BCONE_MAGIC, BCON_TYPE_DBPOINTER, \
   BCON_ENSURE_STORAGE (const_char_ptr_ptr, (_collection)), \
   BCON_ENSURE_STORAGE (const_oid_ptr_ptr, (_oid))
#define BCONE_CODE(_val) BCONE_MAGIC, BCON_TYPE_CODE, \
   BCON_ENSURE_STORAGE (const_char_ptr_ptr, (_val))
#define BCONE_SYMBOL(_val) BCONE_MAGIC, BCON_TYPE_SYMBOL, \
   BCON_ENSURE_STORAGE (const_char_ptr_ptr, (_val))
#define BCONE_CODEWSCOPE(_js, _scope) \
   BCONE_MAGIC, BCON_TYPE_CODEWSCOPE, \
   BCON_ENSURE_STORAGE (const_char_ptr_ptr, (_js)), \
   BCON_ENSURE_STORAGE (bson_ptr, (_scope))
#define BCONE_INT32(_val) BCONE_MAGIC, BCON_TYPE_INT32, \
   BCON_ENSURE_STORAGE (int32_ptr, (_val))
#define BCONE_TIMESTAMP(_timestamp, _increment) \
   BCONE_MAGIC, BCON_TYPE_TIMESTAMP, \
   BCON_ENSURE_STORAGE (int32_ptr, (_timestamp)), \
   BCON_ENSURE_STORAGE (int32_ptr, (_increment))
#define BCONE_INT64(_val) BCONE_MAGIC, BCON_TYPE_INT64, \
   BCON_ENSURE_STORAGE (int64_ptr, (_val))
#define BCONE_MAXKEY BCONE_MAGIC, BCON_TYPE_MAXKEY
#define BCONE_MINKEY BCONE_MAGIC, BCON_TYPE_MINKEY
#define BCONE_SKIP(_val) BCONE_MAGIC, BCON_TYPE_SKIP, \
   BCON_ENSURE (bson_type, (_val))
#define BCONE_ITER(_val) BCONE_MAGIC, BCON_TYPE_ITER, \
   BCON_ENSURE_STORAGE (bson_iter_ptr, (_val))

#define BCON_MAGIC  bson_bcon_magic()
#define BCONE_MAGIC bson_bcone_magic()

typedef enum
{
   BCON_TYPE_UTF8,
   BCON_TYPE_DOUBLE,
   BCON_TYPE_DOCUMENT,
   BCON_TYPE_ARRAY,
   BCON_TYPE_BIN,
   BCON_TYPE_UNDEFINED,
   BCON_TYPE_OID,
   BCON_TYPE_BOOL,
   BCON_TYPE_DATE_TIME,
   BCON_TYPE_NULL,
   BCON_TYPE_REGEX,
   BCON_TYPE_DBPOINTER,
   BCON_TYPE_CODE,
   BCON_TYPE_SYMBOL,
   BCON_TYPE_CODEWSCOPE,
   BCON_TYPE_INT32,
   BCON_TYPE_TIMESTAMP,
   BCON_TYPE_INT64,
   BCON_TYPE_MAXKEY,
   BCON_TYPE_MINKEY,
   BCON_TYPE_BCON,
   BCON_TYPE_ARRAY_START,
   BCON_TYPE_ARRAY_END,
   BCON_TYPE_DOC_START,
   BCON_TYPE_DOC_END,
   BCON_TYPE_END,
   BCON_TYPE_RAW,
   BCON_TYPE_SKIP,
   BCON_TYPE_ITER,
   BCON_TYPE_ERROR,
} bcon_type_t;

typedef struct bcon_append_ctx_frame
{
   int         i;
   bool is_array;
   bson_t      bson;
} bcon_append_ctx_frame_t;

typedef struct bcon_extract_ctx_frame
{
   int         i;
   bool is_array;
   bson_iter_t iter;
} bcon_extract_ctx_frame_t;

typedef struct _bcon_append_ctx_t
{
   bcon_append_ctx_frame_t stack[BCON_STACK_MAX];
   int                     n;
} bcon_append_ctx_t;

typedef struct _bcon_extract_ctx_t
{
   bcon_extract_ctx_frame_t stack[BCON_STACK_MAX];
   int                      n;
} bcon_extract_ctx_t;

void
bcon_append (bson_t *bson,
             ...) BSON_GNUC_NULL_TERMINATED;
void
bcon_append_ctx (bson_t            *bson,
                 bcon_append_ctx_t *ctx,
                 ...) BSON_GNUC_NULL_TERMINATED;
void
bcon_append_ctx_va (bson_t            *bson,
                    bcon_append_ctx_t *ctx,
                    va_list           *va);
void
bcon_append_ctx_init (bcon_append_ctx_t *ctx);

void
bcon_extract_ctx_init (bcon_extract_ctx_t *ctx);

void
bcon_extract_ctx (bson_t             *bson,
                  bcon_extract_ctx_t *ctx,
                  ...) BSON_GNUC_NULL_TERMINATED;

bool
bcon_extract_ctx_va (bson_t             *bson,
                     bcon_extract_ctx_t *ctx,
                     va_list            *ap);

bool
bcon_extract (bson_t *bson,
              ...) BSON_GNUC_NULL_TERMINATED;

bool
bcon_extract_va (bson_t             *bson,
                 bcon_extract_ctx_t *ctx,
                 ...) BSON_GNUC_NULL_TERMINATED;

bson_t *
bcon_new (void *unused,
          ...) BSON_GNUC_NULL_TERMINATED;

/**
 * The bcon_..() functions are all declared with __attribute__((sentinel)).
 *
 * From GCC manual for "sentinel": "A valid NULL in this context is defined as
 * zero with any pointer type. If your system defines the NULL macro with an
 * integer type then you need to add an explicit cast."
 * Case in point: GCC on Solaris (at least)
 */
#define BCON_APPEND(_bson, ...) \
   bcon_append ((_bson), __VA_ARGS__, (void *)NULL)
#define BCON_APPEND_CTX(_bson, _ctx, ...) \
   bcon_append_ctx ((_bson), (_ctx), __VA_ARGS__, (void *)NULL)

#define BCON_EXTRACT(_bson, ...) \
   bcon_extract ((_bson), __VA_ARGS__, (void *)NULL)

#define BCON_EXTRACT_CTX(_bson, _ctx, ...) \
   bcon_extract ((_bson), (_ctx), __VA_ARGS__, (void *)NULL)

#define BCON_NEW(...) \
   bcon_new (NULL, __VA_ARGS__, (void *)NULL)

const char *bson_bcon_magic  (void) BSON_GNUC_CONST;
const char *bson_bcone_magic (void) BSON_GNUC_CONST;


BSON_END_DECLS


#endif



/* These stack manipulation macros are used to manage append recursion in
 * bcon_append_ctx_va().  They take care of some awkward dereference rules (the
 * real bson object isn't in the stack, but accessed by pointer) and add in run
 * time asserts to make sure we don't blow the stack in either direction */

#define STACK_ELE(_delta, _name) (ctx->stack[(_delta) + ctx->n]._name)

#define STACK_BSON(_delta) ( \
      ((_delta) + ctx->n) == 0 \
      ? bson \
      : &STACK_ELE (_delta, bson) \
      )

#define STACK_ITER(_delta) ( \
      ((_delta) + ctx->n) == 0 \
      ? &root_iter \
      : &STACK_ELE (_delta, iter) \
      )

#define STACK_BSON_PARENT STACK_BSON (-1)
#define STACK_BSON_CHILD STACK_BSON (0)

#define STACK_ITER_PARENT STACK_ITER (-1)
#define STACK_ITER_CHILD STACK_ITER (0)

#define STACK_I STACK_ELE (0, i)
#define STACK_IS_ARRAY STACK_ELE (0, is_array)

#define STACK_PUSH_ARRAY(statement) \
   do { \
      assert (ctx->n < (BCON_STACK_MAX - 1)); \
      ctx->n++; \
      STACK_I = 0; \
      STACK_IS_ARRAY = 1; \
      statement; \
   } while (0)

#define STACK_PUSH_DOC(statement) \
   do { \
      assert (ctx->n < (BCON_STACK_MAX - 1)); \
      ctx->n++; \
      STACK_IS_ARRAY = 0; \
      statement; \
   } while (0)

#define STACK_POP_ARRAY(statement) \
   do { \
      assert (STACK_IS_ARRAY); \
      assert (ctx->n != 0); \
      statement; \
      ctx->n--; \
   } while (0)

#define STACK_POP_DOC(statement) \
   do { \
      assert (!STACK_IS_ARRAY); \
      assert (ctx->n != 0); \
      statement; \
      ctx->n--; \
   } while (0)

/* This is a landing pad union for all of the types we can process with bcon.
 * We need actual storage for this to capture the return value of va_arg, which
 * takes multiple calls to get everything we need for some complex types */
typedef union bcon_append {
   char   *UTF8;
   double  DOUBLE;
   bson_t *DOCUMENT;
   bson_t *ARRAY;
   bson_t *BCON;

   struct
   {
      bson_subtype_t subtype;
      uint8_t  *binary;
      uint32_t  length;
   } BIN;

   bson_oid_t    *OID;
   bool BOOL;
   int64_t   DATE_TIME;

   struct
   {
      char *regex;
      char *flags;
   } REGEX;

   struct
   {
      char       *collection;
      bson_oid_t *oid;
   } DBPOINTER;

   const char *CODE;

   char *SYMBOL;

   struct
   {
      const char *js;
      bson_t     *scope;
   } CODEWSCOPE;

   int32_t INT32;

   struct
   {
      uint32_t timestamp;
      uint32_t increment;
   } TIMESTAMP;

   int64_t       INT64;
   const bson_iter_t *ITER;
} bcon_append_t;

/* same as bcon_append_t.  Some extra symbols and varying types that handle the
 * differences between bson_append and bson_iter */
typedef union bcon_extract {
   bson_type_t  TYPE;
   bson_iter_t *ITER;
   const char  *key;
   const char **UTF8;
   double      *DOUBLE;
   bson_t      *DOCUMENT;
   bson_t      *ARRAY;

   struct
   {
      bson_subtype_t      *subtype;
      const uint8_t      **binary;
      uint32_t            *length;
   } BIN;

   const bson_oid_t **OID;
   bool *BOOL;
   int64_t      *DATE_TIME;

   struct
   {
      const char **regex;
      const char **flags;
   } REGEX;

   struct
   {
      const char       **collection;
      const bson_oid_t **oid;
   } DBPOINTER;

   const char **CODE;

   const char **SYMBOL;

   struct
   {
      const char **js;
      bson_t      *scope;
   } CODEWSCOPE;

   int32_t *INT32;

   struct
   {
      uint32_t *timestamp;
      uint32_t *increment;
   } TIMESTAMP;

   int64_t *INT64;
} bcon_extract_t;

static const char *gBconMagic = "BCON_MAGIC";
static const char *gBconeMagic = "BCONE_MAGIC";

const char *
bson_bcon_magic (void)
{
   return gBconMagic;
}


const char *
bson_bcone_magic (void)
{
   return gBconeMagic;
}

/* appends val to the passed bson object.  Meant to be a super simple dispatch
 * table */
static void
_bcon_append_single (bson_t        *bson,
                     bcon_type_t    type,
                     const char    *key,
                     bcon_append_t *val)
{
   switch ((int)type) {
   case BCON_TYPE_UTF8:
      bson_append_utf8 (bson, key, -1, val->UTF8, -1);
      break;
   case BCON_TYPE_DOUBLE:
      bson_append_double (bson, key, -1, val->DOUBLE);
      break;
   case BCON_TYPE_BIN: {
         bson_append_binary (bson, key, -1, val->BIN.subtype, val->BIN.binary,
                             val->BIN.length);
         break;
      }
   case BCON_TYPE_UNDEFINED:
      bson_append_undefined (bson, key, -1);
      break;
   case BCON_TYPE_OID:
      bson_append_oid (bson, key, -1, val->OID);
      break;
   case BCON_TYPE_BOOL:
      bson_append_bool (bson, key, -1, (bool)val->BOOL);
      break;
   case BCON_TYPE_DATE_TIME:
      bson_append_date_time (bson, key, -1, val->DATE_TIME);
      break;
   case BCON_TYPE_NULL:
      bson_append_null (bson, key, -1);
      break;
   case BCON_TYPE_REGEX: {
         bson_append_regex (bson, key, -1, val->REGEX.regex, val->REGEX.flags);
         break;
      }
   case BCON_TYPE_DBPOINTER: {
         bson_append_dbpointer (bson, key, -1, val->DBPOINTER.collection,
                                val->DBPOINTER.oid);
         break;
      }
   case BCON_TYPE_CODE:
      bson_append_code (bson, key, -1, val->CODE);
      break;
   case BCON_TYPE_SYMBOL:
      bson_append_symbol (bson, key, -1, val->SYMBOL, -1);
      break;
   case BCON_TYPE_CODEWSCOPE:
      bson_append_code_with_scope (bson, key, -1, val->CODEWSCOPE.js,
                                   val->CODEWSCOPE.scope);
      break;
   case BCON_TYPE_INT32:
      bson_append_int32 (bson, key, -1, val->INT32);
      break;
   case BCON_TYPE_TIMESTAMP: {
         bson_append_timestamp (bson, key, -1, val->TIMESTAMP.timestamp,
                                val->TIMESTAMP.increment);
         break;
      }
   case BCON_TYPE_INT64:
      bson_append_int64 (bson, key, -1, val->INT64);
      break;
   case BCON_TYPE_MAXKEY:
      bson_append_maxkey (bson, key, -1);
      break;
   case BCON_TYPE_MINKEY:
      bson_append_minkey (bson, key, -1);
      break;
   case BCON_TYPE_ARRAY: {
         bson_append_array (bson, key, -1, val->ARRAY);
         break;
      }
   case BCON_TYPE_DOCUMENT: {
         bson_append_document (bson, key, -1, val->DOCUMENT);
         break;
      }
   case BCON_TYPE_ITER:
      bson_append_iter (bson, key, -1, val->ITER);
      break;
   default:
      assert (0);
      break;
   }
}

#define CHECK_TYPE(_type) \
   do { \
      if (bson_iter_type (iter) != (_type)) { return false; } \
   } while (0)

/* extracts the value under the iterator and writes it to val.  returns false
 * if the iterator type doesn't match the token type.
 *
 * There are two magic tokens:
 *
 * BCONE_SKIP -
 *    Let's us verify that a key has a type, without caring about its value.
 *    This allows for wider declarative BSON verification
 *
 * BCONE_ITER -
 *    Returns the underlying iterator.  This could allow for more complicated,
 *    procedural verification (if a parameter could have multiple types).
 * */
static bool
_bcon_extract_single (const bson_iter_t *iter,
                      bcon_type_t        type,
                      bcon_extract_t    *val)
{
   switch ((int)type) {
   case BCON_TYPE_UTF8:
      CHECK_TYPE (BSON_TYPE_UTF8);
      *val->UTF8 = bson_iter_utf8 (iter, NULL);
      break;
   case BCON_TYPE_DOUBLE:
      CHECK_TYPE (BSON_TYPE_DOUBLE);
      *val->DOUBLE = bson_iter_double (iter);
      break;
   case BCON_TYPE_BIN:
      CHECK_TYPE (BSON_TYPE_BINARY);
      bson_iter_binary (iter, val->BIN.subtype, val->BIN.length,
                        val->BIN.binary);
      break;
   case BCON_TYPE_UNDEFINED:
      CHECK_TYPE (BSON_TYPE_UNDEFINED);
      break;
   case BCON_TYPE_OID:
      CHECK_TYPE (BSON_TYPE_OID);
      *val->OID = bson_iter_oid (iter);
      break;
   case BCON_TYPE_BOOL:
      CHECK_TYPE (BSON_TYPE_BOOL);
      *val->BOOL = bson_iter_bool (iter);
      break;
   case BCON_TYPE_DATE_TIME:
      CHECK_TYPE (BSON_TYPE_DATE_TIME);
      *val->DATE_TIME = bson_iter_date_time (iter);
      break;
   case BCON_TYPE_NULL:
      CHECK_TYPE (BSON_TYPE_NULL);
      break;
   case BCON_TYPE_REGEX:
      CHECK_TYPE (BSON_TYPE_REGEX);
      *val->REGEX.regex = bson_iter_regex (iter, val->REGEX.flags);

      break;
   case BCON_TYPE_DBPOINTER:
      CHECK_TYPE (BSON_TYPE_DBPOINTER);
      bson_iter_dbpointer (iter, NULL, val->DBPOINTER.collection,
                           val->DBPOINTER.oid);
      break;
   case BCON_TYPE_CODE:
      CHECK_TYPE (BSON_TYPE_CODE);
      *val->CODE = bson_iter_code (iter, NULL);
      break;
   case BCON_TYPE_SYMBOL:
      CHECK_TYPE (BSON_TYPE_SYMBOL);
      *val->SYMBOL = bson_iter_symbol (iter, NULL);
      break;
   case BCON_TYPE_CODEWSCOPE: {
         const uint8_t *buf;
         uint32_t len;

         CHECK_TYPE (BSON_TYPE_CODEWSCOPE);

         *val->CODEWSCOPE.js = bson_iter_codewscope (iter, NULL, &len, &buf);

         bson_init_static (val->CODEWSCOPE.scope, buf, len);
         break;
      }
   case BCON_TYPE_INT32:
      CHECK_TYPE (BSON_TYPE_INT32);
      *val->INT32 = bson_iter_int32 (iter);
      break;
   case BCON_TYPE_TIMESTAMP:
      CHECK_TYPE (BSON_TYPE_TIMESTAMP);
      bson_iter_timestamp (iter, val->TIMESTAMP.timestamp,
                           val->TIMESTAMP.increment);
      break;
   case BCON_TYPE_INT64:
      CHECK_TYPE (BSON_TYPE_INT64);
      *val->INT64 = bson_iter_int64 (iter);
      break;
   case BCON_TYPE_MAXKEY:
      CHECK_TYPE (BSON_TYPE_MAXKEY);
      break;
   case BCON_TYPE_MINKEY:
      CHECK_TYPE (BSON_TYPE_MINKEY);
      break;
   case BCON_TYPE_ARRAY: {
         const uint8_t *buf;
         uint32_t len;

         CHECK_TYPE (BSON_TYPE_ARRAY);

         bson_iter_array (iter, &len, &buf);

         bson_init_static (val->ARRAY, buf, len);
         break;
      }
   case BCON_TYPE_DOCUMENT: {
         const uint8_t *buf;
         uint32_t len;

         CHECK_TYPE (BSON_TYPE_DOCUMENT);

         bson_iter_document (iter, &len, &buf);

         bson_init_static (val->DOCUMENT, buf, len);
         break;
      }
   case BCON_TYPE_SKIP:
      CHECK_TYPE (val->TYPE);
      break;
   case BCON_TYPE_ITER:
      memcpy (val->ITER, iter, sizeof *iter);
      break;
   default:
      assert (0);
      break;
   }

   return true;
}

/* Consumes ap, storing output values into u and returning the type of the
 * captured token.
 *
 * The basic workflow goes like this:
 *
 * 1. Look at the current arg.  It will be a char *
 *    a. If it's a NULL, we're done processing.
 *    b. If it's BCON_MAGIC (a symbol with storage in this module)
 *       I. The next token is the type
 *       II. The type specifies how many args to eat and their types
 *    c. Otherwise it's either recursion related or a raw string
 *       I. If the first byte is '{', '}', '[', or ']' pass back an
 *          appropriate recursion token
 *       II. If not, just call it a UTF8 token and pass that back
 */
static bcon_type_t
_bcon_append_tokenize (va_list       *ap,
                       bcon_append_t *u)
{
   char *mark;
   bcon_type_t type;

   mark = va_arg (*ap, char *);

   assert (mark != BCONE_MAGIC);

   if (mark == NULL) {
      type = BCON_TYPE_END;
   } else if (mark == BCON_MAGIC) {
      type = va_arg (*ap, bcon_type_t);

      switch ((int)type) {
      case BCON_TYPE_UTF8:
         u->UTF8 = va_arg (*ap, char *);
         break;
      case BCON_TYPE_DOUBLE:
         u->DOUBLE = va_arg (*ap, double);
         break;
      case BCON_TYPE_DOCUMENT:
         u->DOCUMENT = va_arg (*ap, bson_t *);
         break;
      case BCON_TYPE_ARRAY:
         u->ARRAY = va_arg (*ap, bson_t *);
         break;
      case BCON_TYPE_BIN:
         u->BIN.subtype = va_arg (*ap, bson_subtype_t);
         u->BIN.binary = va_arg (*ap, uint8_t *);
         u->BIN.length = va_arg (*ap, uint32_t);
         break;
      case BCON_TYPE_UNDEFINED:
         break;
      case BCON_TYPE_OID:
         u->OID = va_arg (*ap, bson_oid_t *);
         break;
      case BCON_TYPE_BOOL:
         u->BOOL = va_arg (*ap, int);
         break;
      case BCON_TYPE_DATE_TIME:
         u->DATE_TIME = va_arg (*ap, int64_t);
         break;
      case BCON_TYPE_NULL:
         break;
      case BCON_TYPE_REGEX:
         u->REGEX.regex = va_arg (*ap, char *);
         u->REGEX.flags = va_arg (*ap, char *);
         break;
      case BCON_TYPE_DBPOINTER:
         u->DBPOINTER.collection = va_arg (*ap, char *);
         u->DBPOINTER.oid = va_arg (*ap, bson_oid_t *);
         break;
      case BCON_TYPE_CODE:
         u->CODE = va_arg (*ap, char *);
         break;
      case BCON_TYPE_SYMBOL:
         u->SYMBOL = va_arg (*ap, char *);
         break;
      case BCON_TYPE_CODEWSCOPE:
         u->CODEWSCOPE.js = va_arg (*ap, char *);
         u->CODEWSCOPE.scope = va_arg (*ap, bson_t *);
         break;
      case BCON_TYPE_INT32:
         u->INT32 = va_arg (*ap, int32_t);
         break;
      case BCON_TYPE_TIMESTAMP:
         u->TIMESTAMP.timestamp = va_arg (*ap, uint32_t);
         u->TIMESTAMP.increment = va_arg (*ap, uint32_t);
         break;
      case BCON_TYPE_INT64:
         u->INT64 = va_arg (*ap, int64_t);
         break;
      case BCON_TYPE_MAXKEY:
         break;
      case BCON_TYPE_MINKEY:
         break;
      case BCON_TYPE_BCON:
         u->BCON = va_arg (*ap, bson_t *);
         break;
      case BCON_TYPE_ITER:
         u->ITER = va_arg (*ap, const bson_iter_t *);
         break;
      default:
         assert (0);
         break;
      }
   } else {
      switch (mark[0]) {
      case '{':
         type = BCON_TYPE_DOC_START;
         break;
      case '}':
         type = BCON_TYPE_DOC_END;
         break;
      case '[':
         type = BCON_TYPE_ARRAY_START;
         break;
      case ']':
         type = BCON_TYPE_ARRAY_END;
         break;

      default:
         type = BCON_TYPE_UTF8;
         u->UTF8 = mark;
         break;
      }
   }

   return type;
}


/* Consumes ap, storing output values into u and returning the type of the
 * captured token.
 *
 * The basic workflow goes like this:
 *
 * 1. Look at the current arg.  It will be a char *
 *    a. If it's a NULL, we're done processing.
 *    b. If it's BCONE_MAGIC (a symbol with storage in this module)
 *       I. The next token is the type
 *       II. The type specifies how many args to eat and their types
 *    c. Otherwise it's either recursion related or a raw string
 *       I. If the first byte is '{', '}', '[', or ']' pass back an
 *          appropriate recursion token
 *       II. If not, just call it a UTF8 token and pass that back
 */
static bcon_type_t
_bcon_extract_tokenize (va_list        *ap,
                        bcon_extract_t *u)
{
   char *mark;
   bcon_type_t type;

   mark = va_arg (*ap, char *);

   assert (mark != BCON_MAGIC);

   if (mark == NULL) {
      type = BCON_TYPE_END;
   } else if (mark == BCONE_MAGIC) {
      type = va_arg (*ap, bcon_type_t);

      switch ((int)type) {
      case BCON_TYPE_UTF8:
         u->UTF8 = va_arg (*ap, const char **);
         break;
      case BCON_TYPE_DOUBLE:
         u->DOUBLE = va_arg (*ap, double *);
         break;
      case BCON_TYPE_DOCUMENT:
         u->DOCUMENT = va_arg (*ap, bson_t *);
         break;
      case BCON_TYPE_ARRAY:
         u->ARRAY = va_arg (*ap, bson_t *);
         break;
      case BCON_TYPE_BIN:
         u->BIN.subtype = va_arg (*ap, bson_subtype_t *);
         u->BIN.binary = va_arg (*ap, const uint8_t * *);
         u->BIN.length = va_arg (*ap, uint32_t *);
         break;
      case BCON_TYPE_UNDEFINED:
         break;
      case BCON_TYPE_OID:
         u->OID = va_arg (*ap, const bson_oid_t * *);
         break;
      case BCON_TYPE_BOOL:
         u->BOOL = va_arg (*ap, bool *);
         break;
      case BCON_TYPE_DATE_TIME:
         u->DATE_TIME = va_arg (*ap, int64_t *);
         break;
      case BCON_TYPE_NULL:
         break;
      case BCON_TYPE_REGEX:
         u->REGEX.regex = va_arg (*ap, const char **);
         u->REGEX.flags = va_arg (*ap, const char **);
         break;
      case BCON_TYPE_DBPOINTER:
         u->DBPOINTER.collection = va_arg (*ap, const char **);
         u->DBPOINTER.oid = va_arg (*ap, const bson_oid_t * *);
         break;
      case BCON_TYPE_CODE:
         u->CODE = va_arg (*ap, const char **);
         break;
      case BCON_TYPE_SYMBOL:
         u->SYMBOL = va_arg (*ap, const char **);
         break;
      case BCON_TYPE_CODEWSCOPE:
         u->CODEWSCOPE.js = va_arg (*ap, const char **);
         u->CODEWSCOPE.scope = va_arg (*ap, bson_t *);
         break;
      case BCON_TYPE_INT32:
         u->INT32 = va_arg (*ap, int32_t *);
         break;
      case BCON_TYPE_TIMESTAMP:
         u->TIMESTAMP.timestamp = va_arg (*ap, uint32_t *);
         u->TIMESTAMP.increment = va_arg (*ap, uint32_t *);
         break;
      case BCON_TYPE_INT64:
         u->INT64 = va_arg (*ap, int64_t *);
         break;
      case BCON_TYPE_MAXKEY:
         break;
      case BCON_TYPE_MINKEY:
         break;
      case BCON_TYPE_SKIP:
         u->TYPE = va_arg (*ap, bson_type_t);
         break;
      case BCON_TYPE_ITER:
         u->ITER = va_arg (*ap, bson_iter_t *);
         break;
      default:
         assert (0);
         break;
      }
   } else {
      switch (mark[0]) {
      case '{':
         type = BCON_TYPE_DOC_START;
         break;
      case '}':
         type = BCON_TYPE_DOC_END;
         break;
      case '[':
         type = BCON_TYPE_ARRAY_START;
         break;
      case ']':
         type = BCON_TYPE_ARRAY_END;
         break;

      default:
         type = BCON_TYPE_RAW;
         u->key = mark;
         break;
      }
   }

   return type;
}


/* This trivial utility function is useful for concatenating a bson object onto
 * the end of another, ignoring the keys from the source bson object and
 * continuing to use and increment the keys from the source.  It's only useful
 * when called from bcon_append_ctx_va */
static void
_bson_concat_array (bson_t            *dest,
                    const bson_t      *src,
                    bcon_append_ctx_t *ctx)
{
   bson_iter_t iter;
   const char *key;
   char i_str[16];
   bool r;

   r = bson_iter_init (&iter, src);

   if (!r) {
      fprintf (stderr, "Invalid BSON document, possible memory coruption.\n");
      return;
   }

   STACK_I--;

   while (bson_iter_next (&iter)) {
      bson_uint32_to_string (STACK_I, &key, i_str, sizeof i_str);
      STACK_I++;

      bson_append_iter (dest, key, -1, &iter);
   }
}


/* Append_ctx_va consumes the va_list until NULL is found, appending into bson
 * as tokens are found.  It can receive or return an in-progress bson object
 * via the ctx param.  It can also operate on the middle of a va_list, and so
 * can be wrapped inside of another varargs function.
 *
 * Note that passing in a va_list that isn't perferectly formatted for BCON
 * ingestion will almost certainly result in undefined behavior
 *
 * The workflow relies on the passed ctx object, which holds a stack of bson
 * objects, along with metadata (if the emedded layer is an array, and which
 * element it is on if so).  We iterate, generating tokens from the va_list,
 * until we reach an END token.  If any errors occur, we just blow up (the
 * var_args stuff is already incredibly fragile to mistakes, and we have no way
 * of introspecting, so just don't screw it up).
 *
 * There are also a few STACK_* macros in here which manimpulate ctx that are
 * defined up top.
 * */
void
bcon_append_ctx_va (bson_t            *bson,
                    bcon_append_ctx_t *ctx,
                    va_list           *ap)
{
   bcon_type_t type;
   const char *key;
   char i_str[16];

   bcon_append_t u = { 0 };

   while (1) {
      if (STACK_IS_ARRAY) {
         bson_uint32_to_string (STACK_I, &key, i_str, sizeof i_str);
         STACK_I++;
      } else {
         type = _bcon_append_tokenize (ap, &u);

         if (type == BCON_TYPE_END) {
            return;
         }

         if (type == BCON_TYPE_DOC_END) {
            STACK_POP_DOC (bson_append_document_end (STACK_BSON_PARENT,
                                                     STACK_BSON_CHILD));
            continue;
         }

         if (type == BCON_TYPE_BCON) {
            bson_concat (STACK_BSON_CHILD, u.BCON);
            continue;
         }

         assert (type == BCON_TYPE_UTF8);

         key = u.UTF8;
      }

      type = _bcon_append_tokenize (ap, &u);
      assert (type != BCON_TYPE_END);

      switch ((int)type) {
      case BCON_TYPE_BCON:
         assert (STACK_IS_ARRAY);
         _bson_concat_array (STACK_BSON_CHILD, u.BCON, ctx);

         break;
      case BCON_TYPE_DOC_START:
         STACK_PUSH_DOC (bson_append_document_begin (STACK_BSON_PARENT, key, -1,
                                                     STACK_BSON_CHILD));
         break;
      case BCON_TYPE_DOC_END:
         STACK_POP_DOC (bson_append_document_end (STACK_BSON_PARENT,
                                                  STACK_BSON_CHILD));
         break;
      case BCON_TYPE_ARRAY_START:
         STACK_PUSH_ARRAY (bson_append_array_begin (STACK_BSON_PARENT, key, -1,
                                                    STACK_BSON_CHILD));
         break;
      case BCON_TYPE_ARRAY_END:
         STACK_POP_ARRAY (bson_append_array_end (STACK_BSON_PARENT,
                                                 STACK_BSON_CHILD));
         break;
      default:
         _bcon_append_single (STACK_BSON_CHILD, type, key, &u);

         break;
      }
   }
}


/* extract_ctx_va consumes the va_list until NULL is found, extracting values
 * as tokens are found.  It can receive or return an in-progress bson object
 * via the ctx param.  It can also operate on the middle of a va_list, and so
 * can be wrapped inside of another varargs function.
 *
 * Note that passing in a va_list that isn't perferectly formatted for BCON
 * ingestion will almost certainly result in undefined behavior
 *
 * The workflow relies on the passed ctx object, which holds a stack of iterator
 * objects, along with metadata (if the emedded layer is an array, and which
 * element it is on if so).  We iterate, generating tokens from the va_list,
 * until we reach an END token.  If any errors occur, we just blow up (the
 * var_args stuff is already incredibly fragile to mistakes, and we have no way
 * of introspecting, so just don't screw it up).
 *
 * There are also a few STACK_* macros in here which manimpulate ctx that are
 * defined up top.
 *
 * The function returns true if all tokens could be successfully matched, false
 * otherwise.
 * */
bool
bcon_extract_ctx_va (bson_t             *bson,
                     bcon_extract_ctx_t *ctx,
                     va_list            *ap)
{
   bcon_type_t type;
   const char *key;
   bson_iter_t root_iter;
   bson_iter_t current_iter;
   char i_str[16];

   bcon_extract_t u = { 0 };

   bson_iter_init (&root_iter, bson);

   while (1) {
      if (STACK_IS_ARRAY) {
         bson_uint32_to_string (STACK_I, &key, i_str, sizeof i_str);
         STACK_I++;
      } else {
         type = _bcon_extract_tokenize (ap, &u);

         if (type == BCON_TYPE_END) {
            return true;
         }

         if (type == BCON_TYPE_DOC_END) {
            STACK_POP_DOC (_noop ());
            continue;
         }

         assert (type == BCON_TYPE_RAW);

         key = u.key;
      }

      type = _bcon_extract_tokenize (ap, &u);
      assert (type != BCON_TYPE_END);

      if (type == BCON_TYPE_DOC_END) {
         STACK_POP_DOC (_noop ());
      } else if (type == BCON_TYPE_ARRAY_END) {
         STACK_POP_ARRAY (_noop ());
      } else {
         memcpy (&current_iter, STACK_ITER_CHILD, sizeof current_iter);

         if (!bson_iter_find (&current_iter, key)) { return false; }

         switch ((int)type) {
         case BCON_TYPE_DOC_START:

            if (bson_iter_type (&current_iter) !=
                BSON_TYPE_DOCUMENT) { return false; }

            STACK_PUSH_DOC (bson_iter_recurse (&current_iter,
                                               STACK_ITER_CHILD));
            break;
         case BCON_TYPE_ARRAY_START:

            if (bson_iter_type (&current_iter) !=
                BSON_TYPE_ARRAY) { return false; }

            STACK_PUSH_ARRAY (bson_iter_recurse (&current_iter,
                                                 STACK_ITER_CHILD));
            break;
         default:

            if (!_bcon_extract_single (&current_iter, type, &u)) {
               return false;
            }

            break;
         }
      }
   }
}

void
bcon_extract_ctx_init (bcon_extract_ctx_t *ctx)
{
   ctx->n = 0;
   ctx->stack[0].is_array = false;
}

bool
bcon_extract (bson_t *bson,
              ...)
{
   va_list ap;
   bcon_extract_ctx_t ctx;
   bool r;

   bcon_extract_ctx_init (&ctx);

   va_start (ap, bson);

   r = bcon_extract_ctx_va (bson, &ctx, &ap);

   va_end (ap);

   return r;
}


void
bcon_append (bson_t *bson,
             ...)
{
   va_list ap;
   bcon_append_ctx_t ctx;

   bcon_append_ctx_init (&ctx);

   va_start (ap, bson);

   bcon_append_ctx_va (bson, &ctx, &ap);

   va_end (ap);
}


void
bcon_append_ctx (bson_t            *bson,
                 bcon_append_ctx_t *ctx,
                 ...)
{
   va_list ap;

   va_start (ap, ctx);

   bcon_append_ctx_va (bson, ctx, &ap);

   va_end (ap);
}


void
bcon_extract_ctx (bson_t             *bson,
                  bcon_extract_ctx_t *ctx,
                  ...)
{
   va_list ap;

   va_start (ap, ctx);

   bcon_extract_ctx_va (bson, ctx, &ap);

   va_end (ap);
}

void
bcon_append_ctx_init (bcon_append_ctx_t *ctx)
{
   ctx->n = 0;
   ctx->stack[0].is_array = 0;
}


bson_t *
bcon_new (void *unused,
          ...)
{
   va_list ap;
   bcon_append_ctx_t ctx;
   bson_t *bson;

   bcon_append_ctx_init (&ctx);

   bson = bson_new ();

   va_start (ap, unused);

   bcon_append_ctx_va (bson, &ctx, &ap);

   va_end (ap);

   return bson;
}
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifdef __APPLE__
# include <mach/clock.h>
# include <mach/mach.h>
# include <mach/mach_time.h>
# include <sys/time.h>
#endif


// #include "bson-config.h"

// #include "bson-compat.h"



#if defined(BSON_HAVE_CLOCK_GETTIME)
# include <time.h>
# include <sys/time.h>
#endif

// #include "bson-clock.h"



/*
 *--------------------------------------------------------------------------
 *
 * bson_gettimeofday --
 *
 *       A wrapper around gettimeofday() with fallback support for Windows.
 *
 * Returns:
 *       0 if successful.
 *
 * Side effects:
 *       @tv is set.
 *
 *--------------------------------------------------------------------------
 */

int
bson_gettimeofday (struct timeval *tv) /* OUT */
{
#if defined(_WIN32)
# if defined(_MSC_VER)
#  define DELTA_EPOCH_IN_MICROSEC 11644473600000000Ui64
# else
#  define DELTA_EPOCH_IN_MICROSEC 11644473600000000ULL
# endif
  FILETIME ft;
  uint64_t tmp = 0;

   /*
    * The const value is shamelessy stolen from
    * http://www.boost.org/doc/libs/1_55_0/boost/chrono/detail/inlined/win/chrono.hpp
    *
    * File times are the number of 100 nanosecond intervals elapsed since
    * 12:00 am Jan 1, 1601 UTC.  I haven't check the math particularly hard
    *
    * ...  good luck
    */

   if (tv) {
      GetSystemTimeAsFileTime (&ft);

      /* pull out of the filetime into a 64 bit uint */
      tmp |= ft.dwHighDateTime;
      tmp <<= 32;
      tmp |= ft.dwLowDateTime;

      /* convert from 100's of nanosecs to microsecs */
      tmp /= 10;

      /* adjust to unix epoch */
      tmp -= DELTA_EPOCH_IN_MICROSEC;

      tv->tv_sec = (long)(tmp / 1000000UL);
      tv->tv_usec = (long)(tmp % 1000000UL);
   }

   return 0;
#else
   return gettimeofday (tv, NULL);
#endif
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_get_monotonic_time --
 *
 *       Returns the monotonic system time, if available. A best effort is
 *       made to use the monotonic clock. However, some systems may not
 *       support such a feature.
 *
 * Returns:
 *       The monotonic clock in microseconds.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

int64_t
bson_get_monotonic_time (void)
{
#if defined(BSON_HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
   struct timespec ts;
   clock_gettime (CLOCK_MONOTONIC, &ts);
   return ((ts.tv_sec * 1000000UL) + (ts.tv_nsec / 1000UL));
#elif defined(__APPLE__)
   static mach_timebase_info_data_t info = { 0 };
   static double ratio = 0.0;

   if (!info.denom) {
      // the value from mach_absolute_time () * info.numer / info.denom
      // is in nano seconds. So we have to divid by 1000.0 to get micro seconds
      mach_timebase_info (&info);
      ratio = (double)info.numer / (double)info.denom / 1000.0;
   }

   return mach_absolute_time () * ratio;
#elif defined(_WIN32)
   /* Despite it's name, this is in milliseconds! */
   int64_t ticks = GetTickCount64 ();
   return (ticks * 1000L);
#else
# warning "Monotonic clock is not yet supported on your platform."
   struct timeval tv;

   bson_gettimeofday (&tv);
   return (tv.tv_sec * 1000000UL) + tv.tv_usec;
#endif
}
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// #include "bson-compat.h"

// #include "bson-macros.h"

// #include "bson-error.h"

// #include "bson-iso8601-private.h"
/*
 * Copyright 2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_ISO8601_PRIVATE_H
#define BSON_ISO8601_PRIVATE_H


// #include "bson-compat.h"

// #include "bson-macros.h"



BSON_BEGIN_DECLS

bool
_bson_iso8601_date_parse (const char   *str,
                          int32_t       len,
                          int64_t      *out);

BSON_END_DECLS


#endif /* BSON_ISO8601_PRIVATE_H */


#ifndef _WIN32
// # include "bson-timegm-private.h"
/*
 * Copyright 2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_TIMEGM_PRIVATE_H
#define BSON_TIMEGM_PRIVATE_H


// #include "bson-compat.h"

// #include "bson-macros.h"



BSON_BEGIN_DECLS

time_t
_bson_timegm (struct tm *const tmp);

BSON_END_DECLS


#endif /* BSON_TIMEGM_PRIVATE_H */

#endif


static bool
get_tok (const char  *terminals,
         const char **ptr,
         int32_t     *remaining,
         const char **out,
         int32_t     *out_len)
{
   const char *terminal;
   bool found_terminal = false;

   if (!*remaining) {
      *out = "";
      *out_len = 0;
   }

   *out = *ptr;
   *out_len = -1;

   for (; *remaining && !found_terminal;
        (*ptr)++, (*remaining)--, (*out_len)++) {
      for (terminal = terminals; *terminal; terminal++) {
         if (**ptr == *terminal) {
            found_terminal = true;
            break;
         }
      }
   }

   if (!found_terminal) {
      (*out_len)++;
   }

   return found_terminal;
}

static bool
digits_only (const char *str,
             int32_t     len)
{
   int i;

   for (i = 0; i < len; i++) {
      if (!isdigit(str[i])) {
         return false;
      }
   }

   return true;
}

static bool
parse_num (const char *str,
           int32_t     len,
           int32_t     digits,
           int32_t     min,
           int32_t     max,
           int32_t    *out)
{
   int i;
   int magnitude = 1;
   int32_t value = 0;

   if ((digits >= 0 && len != digits) || !digits_only (str, len)) {
      return false;
   }

   for (i = 1; i <= len; i++, magnitude *= 10) {
      value += (str[len - i] - '0') * magnitude;
   }

   if (value < min || value > max) {
      return false;
   }

   *out = value;

   return true;
}

bool
_bson_iso8601_date_parse (const char *str,
                          int32_t     len,
                          int64_t    *out)
{
   const char *ptr;
   int32_t remaining = len;

   const char *year_ptr;
   const char *month_ptr;
   const char *day_ptr;
   const char *hour_ptr;
   const char *min_ptr;
   const char *sec_ptr;
   const char *millis_ptr;
   const char *tz_ptr;

   int32_t year_len = 0;
   int32_t month_len = 0;
   int32_t day_len = 0;
   int32_t hour_len = 0;
   int32_t min_len = 0;
   int32_t sec_len = 0;
   int32_t millis_len = 0;
   int32_t tz_len = 0;

   int32_t year;
   int32_t month;
   int32_t day;
   int32_t hour;
   int32_t min;
   int32_t sec = 0;
   int64_t millis = 0;
   int32_t tz_adjustment = 0;

#ifdef BSON_OS_WIN32
   SYSTEMTIME win_sys_time;
   FILETIME win_file_time;
   int64_t win_time_offset;
   int64_t win_epoch_difference;
#else
   struct tm posix_date = { 0 };
#endif

   ptr = str;

   /* we have to match at least yyyy-mm-ddThh:mm[:+-Z] */
   if (!(get_tok ("-", &ptr, &remaining, &year_ptr,
                  &year_len) &&
         get_tok ("-", &ptr, &remaining, &month_ptr,
                  &month_len) &&
         get_tok ("T", &ptr, &remaining, &day_ptr,
                  &day_len) &&
         get_tok (":", &ptr, &remaining, &hour_ptr,
                  &hour_len) &&
         get_tok (":+-Z", &ptr, &remaining, &min_ptr, &min_len))) {
      return false;
   }

   /* if the minute has a ':' at the end look for seconds */
   if (min_ptr[min_len] == ':') {
      if (remaining < 2) {
         return false;
      }

      get_tok (".+-Z", &ptr, &remaining, &sec_ptr, &sec_len);

      if (!sec_len) {
         return false;
      }
   }

   /* if we had a second and it is followed by a '.' look for milliseconds */
   if (sec_len && sec_ptr[sec_len] == '.') {
      if (remaining < 2) {
         return false;
      }

      get_tok ("+-Z", &ptr, &remaining, &millis_ptr, &millis_len);

      if (!millis_len) {
         return false;
      }
   }

   /* backtrack by 1 to put ptr on the timezone */
   ptr--;
   remaining++;

   get_tok ("", &ptr, &remaining, &tz_ptr, &tz_len);

   /* we want to include the last few hours in 1969 for timezones translate
    * across 1970 GMT.  We'll check in timegm later on to make sure we're post
    * 1970 */
   if (!parse_num (year_ptr, year_len, 4, 1969, 9999, &year)) {
      return false;
   }

   /* values are as in struct tm */
   year -= 1900;

   if (!parse_num (month_ptr, month_len, 2, 1, 12, &month)) {
      return false;
   }

   /* values are as in struct tm */
   month -= 1;

   if (!parse_num (day_ptr, day_len, 2, 1, 31, &day)) {
      return false;
   }

   if (!parse_num (hour_ptr, hour_len, 2, 0, 23, &hour)) {
      return false;
   }

   if (!parse_num (min_ptr, min_len, 2, 0, 59, &min)) {
      return false;
   }

   if (sec_len && !parse_num (sec_ptr, sec_len, 2, 0, 60, &sec)) {
      return false;
   }

   if (tz_len > 0) {
      if (tz_ptr[0] == 'Z' && tz_len == 1) {
         /* valid */
      } else if (tz_ptr[0] == '+' || tz_ptr[0] == '-') {
         int32_t tz_hour;
         int32_t tz_min;

         if (tz_len != 5 || !digits_only (tz_ptr + 1, 4)) {
            return false;
         }

         if (!parse_num (tz_ptr + 1, 2, -1, -23, 23, &tz_hour)) {
            return false;
         }

         if (!parse_num (tz_ptr + 3, 2, -1, 0, 59, &tz_min)) {
            return false;
         }

         /* we inflect the meaning of a 'positive' timezone.  Those are hours
          * we have to substract, and vice versa */
         tz_adjustment =
            (tz_ptr[0] == '-' ? 1 : -1) * ((tz_min * 60) + (tz_hour * 60 * 60));

         if (!(tz_adjustment > -86400 && tz_adjustment < 86400)) {
            return false;
         }
      } else {
         return false;
      }
   }

   if (millis_len > 0) {
      int i;
      int magnitude;
      millis = 0;

      if (millis_len > 3 || !digits_only (millis_ptr, millis_len)) {
         return false;
      }

      for (i = 1, magnitude = 1; i <= millis_len; i++, magnitude *= 10) {
         millis += (millis_ptr[millis_len - i] - '0') * magnitude;
      }

      if (millis_len == 1) {
         millis *= 100;
      } else if (millis_len == 2) {
         millis *= 10;
      }

      if (millis < 0 || millis > 1000) {
         return false;
      }
   }

#ifdef BSON_OS_WIN32
   win_sys_time.wMilliseconds = millis;
   win_sys_time.wSecond = sec;
   win_sys_time.wMinute = min;
   win_sys_time.wHour = hour;
   win_sys_time.wDay = day;
   win_sys_time.wDayOfWeek = -1;  /* ignored */
   win_sys_time.wMonth = month + 1;
   win_sys_time.wYear = year + 1900;

   /* the wDayOfWeek member of SYSTEMTIME is ignored by this function */
   if (SystemTimeToFileTime (&win_sys_time, &win_file_time) == 0) {
      return 0;
   }

   /* The Windows FILETIME structure contains two parts of a 64-bit value representing the
    * number of 100-nanosecond intervals since January 1, 1601
    */
   win_time_offset =
      (((uint64_t)win_file_time.dwHighDateTime) << 32) |
      win_file_time.dwLowDateTime;

   /* There are 11644473600 seconds between the unix epoch and the windows epoch
    * 100-nanoseconds = milliseconds * 10000
    */
   win_epoch_difference = 11644473600000 * 10000;

   /* removes the diff between 1970 and 1601 */
   win_time_offset -= win_epoch_difference;

   /* 1 milliseconds = 1000000 nanoseconds = 10000 100-nanosecond intervals */
   millis = win_time_offset / 10000;
#else
   posix_date.tm_sec = sec;
   posix_date.tm_min = min;
   posix_date.tm_hour = hour;
   posix_date.tm_mday = day;
   posix_date.tm_mon = month;
   posix_date.tm_year = year;
   posix_date.tm_wday = 0;
   posix_date.tm_yday = 0;

   millis = (1000 * ((uint64_t)_bson_timegm (&posix_date))) + millis;

#endif

   millis += tz_adjustment * 1000;

   if (millis < 0) {
      return false;
   }

   *out = millis;

   return true;
}
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h>

// #include "bson-keys.h"

// #include "bson-string.h"



static const char * gUint32Strs[] = {
   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12",
   "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
   "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34",
   "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45",
   "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56",
   "57", "58", "59", "60", "61", "62", "63", "64", "65", "66", "67",
   "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78",
   "79", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
   "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "100",
   "101", "102", "103", "104", "105", "106", "107", "108", "109", "110",
   "111", "112", "113", "114", "115", "116", "117", "118", "119", "120",
   "121", "122", "123", "124", "125", "126", "127", "128", "129", "130",
   "131", "132", "133", "134", "135", "136", "137", "138", "139", "140",
   "141", "142", "143", "144", "145", "146", "147", "148", "149", "150",
   "151", "152", "153", "154", "155", "156", "157", "158", "159", "160",
   "161", "162", "163", "164", "165", "166", "167", "168", "169", "170",
   "171", "172", "173", "174", "175", "176", "177", "178", "179", "180",
   "181", "182", "183", "184", "185", "186", "187", "188", "189", "190",
   "191", "192", "193", "194", "195", "196", "197", "198", "199", "200",
   "201", "202", "203", "204", "205", "206", "207", "208", "209", "210",
   "211", "212", "213", "214", "215", "216", "217", "218", "219", "220",
   "221", "222", "223", "224", "225", "226", "227", "228", "229", "230",
   "231", "232", "233", "234", "235", "236", "237", "238", "239", "240",
   "241", "242", "243", "244", "245", "246", "247", "248", "249", "250",
   "251", "252", "253", "254", "255", "256", "257", "258", "259", "260",
   "261", "262", "263", "264", "265", "266", "267", "268", "269", "270",
   "271", "272", "273", "274", "275", "276", "277", "278", "279", "280",
   "281", "282", "283", "284", "285", "286", "287", "288", "289", "290",
   "291", "292", "293", "294", "295", "296", "297", "298", "299", "300",
   "301", "302", "303", "304", "305", "306", "307", "308", "309", "310",
   "311", "312", "313", "314", "315", "316", "317", "318", "319", "320",
   "321", "322", "323", "324", "325", "326", "327", "328", "329", "330",
   "331", "332", "333", "334", "335", "336", "337", "338", "339", "340",
   "341", "342", "343", "344", "345", "346", "347", "348", "349", "350",
   "351", "352", "353", "354", "355", "356", "357", "358", "359", "360",
   "361", "362", "363", "364", "365", "366", "367", "368", "369", "370",
   "371", "372", "373", "374", "375", "376", "377", "378", "379", "380",
   "381", "382", "383", "384", "385", "386", "387", "388", "389", "390",
   "391", "392", "393", "394", "395", "396", "397", "398", "399", "400",
   "401", "402", "403", "404", "405", "406", "407", "408", "409", "410",
   "411", "412", "413", "414", "415", "416", "417", "418", "419", "420",
   "421", "422", "423", "424", "425", "426", "427", "428", "429", "430",
   "431", "432", "433", "434", "435", "436", "437", "438", "439", "440",
   "441", "442", "443", "444", "445", "446", "447", "448", "449", "450",
   "451", "452", "453", "454", "455", "456", "457", "458", "459", "460",
   "461", "462", "463", "464", "465", "466", "467", "468", "469", "470",
   "471", "472", "473", "474", "475", "476", "477", "478", "479", "480",
   "481", "482", "483", "484", "485", "486", "487", "488", "489", "490",
   "491", "492", "493", "494", "495", "496", "497", "498", "499", "500",
   "501", "502", "503", "504", "505", "506", "507", "508", "509", "510",
   "511", "512", "513", "514", "515", "516", "517", "518", "519", "520",
   "521", "522", "523", "524", "525", "526", "527", "528", "529", "530",
   "531", "532", "533", "534", "535", "536", "537", "538", "539", "540",
   "541", "542", "543", "544", "545", "546", "547", "548", "549", "550",
   "551", "552", "553", "554", "555", "556", "557", "558", "559", "560",
   "561", "562", "563", "564", "565", "566", "567", "568", "569", "570",
   "571", "572", "573", "574", "575", "576", "577", "578", "579", "580",
   "581", "582", "583", "584", "585", "586", "587", "588", "589", "590",
   "591", "592", "593", "594", "595", "596", "597", "598", "599", "600",
   "601", "602", "603", "604", "605", "606", "607", "608", "609", "610",
   "611", "612", "613", "614", "615", "616", "617", "618", "619", "620",
   "621", "622", "623", "624", "625", "626", "627", "628", "629", "630",
   "631", "632", "633", "634", "635", "636", "637", "638", "639", "640",
   "641", "642", "643", "644", "645", "646", "647", "648", "649", "650",
   "651", "652", "653", "654", "655", "656", "657", "658", "659", "660",
   "661", "662", "663", "664", "665", "666", "667", "668", "669", "670",
   "671", "672", "673", "674", "675", "676", "677", "678", "679", "680",
   "681", "682", "683", "684", "685", "686", "687", "688", "689", "690",
   "691", "692", "693", "694", "695", "696", "697", "698", "699", "700",
   "701", "702", "703", "704", "705", "706", "707", "708", "709", "710",
   "711", "712", "713", "714", "715", "716", "717", "718", "719", "720",
   "721", "722", "723", "724", "725", "726", "727", "728", "729", "730",
   "731", "732", "733", "734", "735", "736", "737", "738", "739", "740",
   "741", "742", "743", "744", "745", "746", "747", "748", "749", "750",
   "751", "752", "753", "754", "755", "756", "757", "758", "759", "760",
   "761", "762", "763", "764", "765", "766", "767", "768", "769", "770",
   "771", "772", "773", "774", "775", "776", "777", "778", "779", "780",
   "781", "782", "783", "784", "785", "786", "787", "788", "789", "790",
   "791", "792", "793", "794", "795", "796", "797", "798", "799", "800",
   "801", "802", "803", "804", "805", "806", "807", "808", "809", "810",
   "811", "812", "813", "814", "815", "816", "817", "818", "819", "820",
   "821", "822", "823", "824", "825", "826", "827", "828", "829", "830",
   "831", "832", "833", "834", "835", "836", "837", "838", "839", "840",
   "841", "842", "843", "844", "845", "846", "847", "848", "849", "850",
   "851", "852", "853", "854", "855", "856", "857", "858", "859", "860",
   "861", "862", "863", "864", "865", "866", "867", "868", "869", "870",
   "871", "872", "873", "874", "875", "876", "877", "878", "879", "880",
   "881", "882", "883", "884", "885", "886", "887", "888", "889", "890",
   "891", "892", "893", "894", "895", "896", "897", "898", "899", "900",
   "901", "902", "903", "904", "905", "906", "907", "908", "909", "910",
   "911", "912", "913", "914", "915", "916", "917", "918", "919", "920",
   "921", "922", "923", "924", "925", "926", "927", "928", "929", "930",
   "931", "932", "933", "934", "935", "936", "937", "938", "939", "940",
   "941", "942", "943", "944", "945", "946", "947", "948", "949", "950",
   "951", "952", "953", "954", "955", "956", "957", "958", "959", "960",
   "961", "962", "963", "964", "965", "966", "967", "968", "969", "970",
   "971", "972", "973", "974", "975", "976", "977", "978", "979", "980",
   "981", "982", "983", "984", "985", "986", "987", "988", "989", "990",
   "991", "992", "993", "994", "995", "996", "997", "998", "999"
};


/*
 *--------------------------------------------------------------------------
 *
 * bson_uint32_to_string --
 *
 *       Converts @value to a string.
 *
 *       If @value is from 0 to 1000, it will use a constant string in the
 *       data section of the library.
 *
 *       If not, a string will be formatted using @str and snprintf(). This
 *       is much slower, of course and therefore we try to optimize it out.
 *
 *       @strptr will always be set. It will either point to @str or a
 *       constant string. You will want to use this as your key.
 *
 * Parameters:
 *       @value: A #uint32_t to convert to string.
 *       @strptr: (out): A pointer to the resulting string.
 *       @str: (out): Storage for a string made with snprintf.
 *       @size: Size of @str.
 *
 * Returns:
 *       The number of bytes in the resulting string.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

size_t
bson_uint32_to_string (uint32_t     value,  /* IN */
                       const char **strptr, /* OUT */
                       char        *str,    /* OUT */
                       size_t       size)   /* IN */
{
   if (value < 1000) {
      *strptr = gUint32Strs[value];

      if (value < 10) {
         return 1;
      } else if (value < 100) {
         return 2;
      } else {
         return 3;
      }
   }

   *strptr = str;

   return bson_snprintf (str, size, "%u", value);
}
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #include "bson-compat.h"


#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// #include "bson-context-private.h"
/*
 * Copyright 2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_CONTEXT_PRIVATE_H
#define BSON_CONTEXT_PRIVATE_H


// #include "bson-context.h"

// #include "bson-thread-private.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_THREAD_PRIVATE_H
#define BSON_THREAD_PRIVATE_H


#if !defined (BSON_INSIDE) && !defined (BSON_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


// #include "bson-compat.h"

// #include "bson-config.h"

// #include "bson-macros.h"



BSON_BEGIN_DECLS


#if defined(BSON_OS_UNIX)
#  include <pthread.h>
#  define bson_mutex_t                    pthread_mutex_t
#  define bson_mutex_init(_n)             pthread_mutex_init((_n), NULL)
#  define bson_mutex_lock                 pthread_mutex_lock
#  define bson_mutex_unlock               pthread_mutex_unlock
#  define bson_mutex_destroy              pthread_mutex_destroy
#  define bson_thread_t                   pthread_t
#  define bson_thread_create(_t,_f,_d)    pthread_create((_t), NULL, (_f), (_d))
#  define bson_thread_join(_n)            pthread_join((_n), NULL)
#  define bson_once_t                     pthread_once_t
#  define bson_once                       pthread_once
#  define BSON_ONCE_FUN(n)                void n(void)
#  define BSON_ONCE_RETURN                return
#  ifdef BSON_PTHREAD_ONCE_INIT_NEEDS_BRACES
#    define BSON_ONCE_INIT                {PTHREAD_ONCE_INIT}
#  else
#    define BSON_ONCE_INIT                PTHREAD_ONCE_INIT
#  endif
#else
#  define bson_mutex_t                    CRITICAL_SECTION
#  define bson_mutex_init                 InitializeCriticalSection
#  define bson_mutex_lock                 EnterCriticalSection
#  define bson_mutex_unlock               LeaveCriticalSection
#  define bson_mutex_destroy              DeleteCriticalSection
#  define bson_thread_t                   HANDLE
#  define bson_thread_create(_t,_f,_d)    (!(*(_t) = CreateThread(NULL,0,(void*)_f,_d,0,NULL)))
#  define bson_thread_join(_n)            WaitForSingleObject((_n), INFINITE)
#  define bson_once_t                     INIT_ONCE
#  define BSON_ONCE_INIT                  INIT_ONCE_STATIC_INIT
#  define bson_once(o, c)                 InitOnceExecuteOnce(o, c, NULL, NULL)
#  define BSON_ONCE_FUN(n)                BOOL CALLBACK n(PINIT_ONCE _ignored_a, PVOID _ignored_b, PVOID *_ignored_c)
#  define BSON_ONCE_RETURN                return true
#endif


BSON_END_DECLS


#endif /* BSON_THREAD_PRIVATE_H */



BSON_BEGIN_DECLS


struct _bson_context_t
{
   bson_context_flags_t flags : 7;
   bool                 pidbe_once : 1;
   uint8_t              pidbe[2];
   uint8_t              md5[3];
   int32_t              seq32;
   int64_t              seq64;

   void (*oid_get_host)  (bson_context_t *context,
                          bson_oid_t     *oid);
   void (*oid_get_pid)   (bson_context_t *context,
                          bson_oid_t     *oid);
   void (*oid_get_seq32) (bson_context_t *context,
                          bson_oid_t     *oid);
   void (*oid_get_seq64) (bson_context_t *context,
                          bson_oid_t     *oid);
};


BSON_END_DECLS


#endif /* BSON_CONTEXT_PRIVATE_H */

// #include "bson-md5.h"

// #include "bson-oid.h"

// #include "bson-string.h"



/*
 * This table contains an array of two character pairs for every possible
 * uint8_t. It is used as a lookup table when encoding a bson_oid_t
 * to hex formatted ASCII. Performing two characters at a time roughly
 * reduces the number of operations by one-half.
 */
static const uint16_t gHexCharPairs[] = {
#if BSON_BYTE_ORDER == BSON_BIG_ENDIAN
   12336, 12337, 12338, 12339, 12340, 12341, 12342, 12343, 12344, 12345,
   12385, 12386, 12387, 12388, 12389, 12390, 12592, 12593, 12594, 12595,
   12596, 12597, 12598, 12599, 12600, 12601, 12641, 12642, 12643, 12644,
   12645, 12646, 12848, 12849, 12850, 12851, 12852, 12853, 12854, 12855,
   12856, 12857, 12897, 12898, 12899, 12900, 12901, 12902, 13104, 13105,
   13106, 13107, 13108, 13109, 13110, 13111, 13112, 13113, 13153, 13154,
   13155, 13156, 13157, 13158, 13360, 13361, 13362, 13363, 13364, 13365,
   13366, 13367, 13368, 13369, 13409, 13410, 13411, 13412, 13413, 13414,
   13616, 13617, 13618, 13619, 13620, 13621, 13622, 13623, 13624, 13625,
   13665, 13666, 13667, 13668, 13669, 13670, 13872, 13873, 13874, 13875,
   13876, 13877, 13878, 13879, 13880, 13881, 13921, 13922, 13923, 13924,
   13925, 13926, 14128, 14129, 14130, 14131, 14132, 14133, 14134, 14135,
   14136, 14137, 14177, 14178, 14179, 14180, 14181, 14182, 14384, 14385,
   14386, 14387, 14388, 14389, 14390, 14391, 14392, 14393, 14433, 14434,
   14435, 14436, 14437, 14438, 14640, 14641, 14642, 14643, 14644, 14645,
   14646, 14647, 14648, 14649, 14689, 14690, 14691, 14692, 14693, 14694,
   24880, 24881, 24882, 24883, 24884, 24885, 24886, 24887, 24888, 24889,
   24929, 24930, 24931, 24932, 24933, 24934, 25136, 25137, 25138, 25139,
   25140, 25141, 25142, 25143, 25144, 25145, 25185, 25186, 25187, 25188,
   25189, 25190, 25392, 25393, 25394, 25395, 25396, 25397, 25398, 25399,
   25400, 25401, 25441, 25442, 25443, 25444, 25445, 25446, 25648, 25649,
   25650, 25651, 25652, 25653, 25654, 25655, 25656, 25657, 25697, 25698,
   25699, 25700, 25701, 25702, 25904, 25905, 25906, 25907, 25908, 25909,
   25910, 25911, 25912, 25913, 25953, 25954, 25955, 25956, 25957, 25958,
   26160, 26161, 26162, 26163, 26164, 26165, 26166, 26167, 26168, 26169,
   26209, 26210, 26211, 26212, 26213, 26214
#else
   12336, 12592, 12848, 13104, 13360, 13616, 13872, 14128, 14384, 14640,
   24880, 25136, 25392, 25648, 25904, 26160, 12337, 12593, 12849, 13105,
   13361, 13617, 13873, 14129, 14385, 14641, 24881, 25137, 25393, 25649,
   25905, 26161, 12338, 12594, 12850, 13106, 13362, 13618, 13874, 14130,
   14386, 14642, 24882, 25138, 25394, 25650, 25906, 26162, 12339, 12595,
   12851, 13107, 13363, 13619, 13875, 14131, 14387, 14643, 24883, 25139,
   25395, 25651, 25907, 26163, 12340, 12596, 12852, 13108, 13364, 13620,
   13876, 14132, 14388, 14644, 24884, 25140, 25396, 25652, 25908, 26164,
   12341, 12597, 12853, 13109, 13365, 13621, 13877, 14133, 14389, 14645,
   24885, 25141, 25397, 25653, 25909, 26165, 12342, 12598, 12854, 13110,
   13366, 13622, 13878, 14134, 14390, 14646, 24886, 25142, 25398, 25654,
   25910, 26166, 12343, 12599, 12855, 13111, 13367, 13623, 13879, 14135,
   14391, 14647, 24887, 25143, 25399, 25655, 25911, 26167, 12344, 12600,
   12856, 13112, 13368, 13624, 13880, 14136, 14392, 14648, 24888, 25144,
   25400, 25656, 25912, 26168, 12345, 12601, 12857, 13113, 13369, 13625,
   13881, 14137, 14393, 14649, 24889, 25145, 25401, 25657, 25913, 26169,
   12385, 12641, 12897, 13153, 13409, 13665, 13921, 14177, 14433, 14689,
   24929, 25185, 25441, 25697, 25953, 26209, 12386, 12642, 12898, 13154,
   13410, 13666, 13922, 14178, 14434, 14690, 24930, 25186, 25442, 25698,
   25954, 26210, 12387, 12643, 12899, 13155, 13411, 13667, 13923, 14179,
   14435, 14691, 24931, 25187, 25443, 25699, 25955, 26211, 12388, 12644,
   12900, 13156, 13412, 13668, 13924, 14180, 14436, 14692, 24932, 25188,
   25444, 25700, 25956, 26212, 12389, 12645, 12901, 13157, 13413, 13669,
   13925, 14181, 14437, 14693, 24933, 25189, 25445, 25701, 25957, 26213,
   12390, 12646, 12902, 13158, 13414, 13670, 13926, 14182, 14438, 14694,
   24934, 25190, 25446, 25702, 25958, 26214
#endif
};


/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_init_sequence --
 *
 *       Initializes @oid with the next oid in the sequence. The first 4
 *       bytes contain the current time and the following 8 contain a 64-bit
 *       integer in big-endian format.
 *
 *       The bson_oid_t generated by this function is not guaranteed to be
 *       globally unique. Only unique within this context. It is however,
 *       guaranteed to be sequential.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is initialized.
 *
 *--------------------------------------------------------------------------
 */

void
bson_oid_init_sequence (bson_oid_t     *oid,     /* OUT */
                        bson_context_t *context) /* IN */
{
   uint32_t now = (uint32_t)(time (NULL));

   if (!context) {
      context = bson_context_get_default ();
   }

   now = BSON_UINT32_TO_BE (now);

   memcpy (&oid->bytes[0], &now, sizeof (now));
   context->oid_get_seq64 (context, oid);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_init --
 *
 *       Generates bytes for a new bson_oid_t and stores them in @oid. The
 *       bytes will be generated according to the specification and includes
 *       the current time, first 3 bytes of MD5(hostname), pid (or tid), and
 *       monotonic counter.
 *
 *       The bson_oid_t generated by this function is not guaranteed to be
 *       globally unique. Only unique within this context. It is however,
 *       guaranteed to be sequential.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is initialized.
 *
 *--------------------------------------------------------------------------
 */

void
bson_oid_init (bson_oid_t     *oid,     /* OUT */
               bson_context_t *context) /* IN */
{
   uint32_t now = (uint32_t)(time (NULL));

   BSON_ASSERT (oid);

   if (!context) {
      context = bson_context_get_default ();
   }

   now = BSON_UINT32_TO_BE (now);
   memcpy (&oid->bytes[0], &now, sizeof (now));

   context->oid_get_host (context, oid);
   context->oid_get_pid (context, oid);
   context->oid_get_seq32 (context, oid);
}


/**
 * bson_oid_init_from_data:
 * @oid: A bson_oid_t to initialize.
 * @bytes: A 12-byte buffer to copy into @oid.
 *
 */
/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_init_from_data --
 *
 *       Initializes an @oid from @data. @data MUST be a buffer of at least
 *       12 bytes. This method is analagous to memcpy()'ing data into @oid.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is initialized.
 *
 *--------------------------------------------------------------------------
 */

void
bson_oid_init_from_data (bson_oid_t    *oid,  /* OUT */
                         const uint8_t *data) /* IN */
{
   BSON_ASSERT (oid);
   BSON_ASSERT (data);

   memcpy (oid, data, 12);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_init_from_string --
 *
 *       Parses @str containing hex formatted bytes of an object id and
 *       places the bytes in @oid.
 *
 * Parameters:
 *       @oid: A bson_oid_t
 *       @str: A string containing at least 24 characters.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is initialized.
 *
 *--------------------------------------------------------------------------
 */

void
bson_oid_init_from_string (bson_oid_t *oid, /* OUT */
                           const char *str) /* IN */
{
   BSON_ASSERT (oid);
   BSON_ASSERT (str);

   bson_oid_init_from_string_unsafe (oid, str);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_get_time_t --
 *
 *       Fetches the time for which @oid was created.
 *
 * Returns:
 *       A time_t.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

time_t
bson_oid_get_time_t (const bson_oid_t *oid) /* IN */
{
   BSON_ASSERT (oid);

   return bson_oid_get_time_t_unsafe (oid);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_to_string --
 *
 *       Formats a bson_oid_t into a string. @str must contain enough bytes
 *       for the resulting string which is 25 bytes with a terminating
 *       NUL-byte.
 *
 * Parameters:
 *       @oid: A bson_oid_t.
 *       @str: A location to store the resulting string.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_oid_to_string
   (const bson_oid_t *oid,                                   /* IN */
    char              str[BSON_ENSURE_ARRAY_PARAM_SIZE(25)]) /* OUT */
{
#if !defined(__i386__) && !defined(__x86_64__)
   BSON_ASSERT (oid);
   BSON_ASSERT (str);

   bson_snprintf (str, 25,
                  "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                  oid->bytes[0],
                  oid->bytes[1],
                  oid->bytes[2],
                  oid->bytes[3],
                  oid->bytes[4],
                  oid->bytes[5],
                  oid->bytes[6],
                  oid->bytes[7],
                  oid->bytes[8],
                  oid->bytes[9],
                  oid->bytes[10],
                  oid->bytes[11]);
#else
   uint16_t *dst;
   uint8_t *id = (uint8_t *)oid;

   BSON_ASSERT (oid);
   BSON_ASSERT (str);

   dst = (uint16_t *)(void *)str;
   dst[0] = gHexCharPairs[id[0]];
   dst[1] = gHexCharPairs[id[1]];
   dst[2] = gHexCharPairs[id[2]];
   dst[3] = gHexCharPairs[id[3]];
   dst[4] = gHexCharPairs[id[4]];
   dst[5] = gHexCharPairs[id[5]];
   dst[6] = gHexCharPairs[id[6]];
   dst[7] = gHexCharPairs[id[7]];
   dst[8] = gHexCharPairs[id[8]];
   dst[9] = gHexCharPairs[id[9]];
   dst[10] = gHexCharPairs[id[10]];
   dst[11] = gHexCharPairs[id[11]];
   str[24] = '\0';
#endif
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_hash --
 *
 *       Hashes the bytes of the provided bson_oid_t using DJB hash.  This
 *       allows bson_oid_t to be used as keys in a hash table.
 *
 * Returns:
 *       A hash value corresponding to @oid.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

uint32_t
bson_oid_hash (const bson_oid_t *oid) /* IN */
{
   BSON_ASSERT (oid);

   return bson_oid_hash_unsafe (oid);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_compare --
 *
 *       A qsort() style compare function that will return less than zero if
 *       @oid1 is less than @oid2, zero if they are the same, and greater
 *       than zero if @oid2 is greater than @oid1.
 *
 * Returns:
 *       A qsort() style compare integer.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

int
bson_oid_compare (const bson_oid_t *oid1, /* IN */
                  const bson_oid_t *oid2) /* IN */
{
   BSON_ASSERT (oid1);
   BSON_ASSERT (oid2);

   return bson_oid_compare_unsafe (oid1, oid2);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_equal --
 *
 *       Compares for equality of @oid1 and @oid2. If they are equal, then
 *       true is returned, otherwise false.
 *
 * Returns:
 *       A boolean indicating the equality of @oid1 and @oid2.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_oid_equal (const bson_oid_t *oid1, /* IN */
                const bson_oid_t *oid2) /* IN */
{
   BSON_ASSERT (oid1);
   BSON_ASSERT (oid2);

   return bson_oid_equal_unsafe (oid1, oid2);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_copy --
 *
 *       Copies the contents of @src to @dst.
 *
 * Parameters:
 *       @src: A bson_oid_t to copy from.
 *       @dst: A bson_oid_t to copy to.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @dst will contain a copy of the data in @src.
 *
 *--------------------------------------------------------------------------
 */

void
bson_oid_copy (const bson_oid_t *src, /* IN */
               bson_oid_t       *dst) /* OUT */
{
   BSON_ASSERT (src);
   BSON_ASSERT (dst);

   bson_oid_copy_unsafe (src, dst);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_oid_is_valid --
 *
 *       Validates that @str is a valid OID string. @length MUST be 24, but
 *       is provided as a parameter to simplify calling code.
 *
 * Parameters:
 *       @str: A string to validate.
 *       @length: The length of @str.
 *
 * Returns:
 *       true if @str can be passed to bson_oid_init_from_string().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_oid_is_valid (const char *str,    /* IN */
                   size_t      length) /* IN */
{
   size_t i;

   BSON_ASSERT (str);

   if ((length == 25) && (str [24] == '\0')) {
      length = 24;
   }

   if (length == 24) {
      for (i = 0; i < length; i++) {
         switch (str[i]) {
         case '0': case '1': case '2': case '3': case '4': case '5': case '6':
         case '7': case '8': case '9': case 'a': case 'b': case 'c': case 'd':
         case 'e': case 'f':
            break;
         default:
            return false;
         }
      }
      return true;
   }

   return false;
}
/*
** This file is in the public domain, so clarified as of
** 1996-06-05 by Arthur David Olson.
*/

/*
** Leap second handling from Bradley White.
** POSIX-style TZ environment variable handling from Guy Harris.
*/

// #include "bson-compat.h"

// #include "bson-macros.h"

// #include "bson-timegm-private.h"


#ifndef BSON_OS_WIN32

#include "errno.h"
#include "string.h"
#include "limits.h"	/* for CHAR_BIT et al. */
#include "time.h"

/* Unlike <ctype.h>'s isdigit, this also works if c < 0 | c > UCHAR_MAX. */
#define is_digit(c) ((unsigned)(c) - '0' <= 9)

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#if 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
# define ATTRIBUTE_CONST __attribute__ ((const))
# define ATTRIBUTE_PURE __attribute__ ((__pure__))
# define ATTRIBUTE_FORMAT(spec) __attribute__ ((__format__ spec))
#else
# define ATTRIBUTE_CONST /* empty */
# define ATTRIBUTE_PURE /* empty */
# define ATTRIBUTE_FORMAT(spec) /* empty */
#endif

#if !defined _Noreturn && (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112)
# if 2 < __GNUC__ + (8 <= __GNUC_MINOR__)
#  define _Noreturn __attribute__ ((__noreturn__))
# else
#  define _Noreturn
# endif
#endif

#if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901) && !defined restrict
# define restrict /* empty */
#endif

#ifndef TYPE_BIT
#define TYPE_BIT(type)	(sizeof (type) * CHAR_BIT)
#endif /* !defined TYPE_BIT */

#ifndef TYPE_SIGNED
#define TYPE_SIGNED(type) (((type) -1) < 0)
#endif /* !defined TYPE_SIGNED */

/* The minimum and maximum finite time values.  */
static time_t const time_t_min =
    (TYPE_SIGNED(time_t)
        ? (time_t) -1 << (CHAR_BIT * sizeof (time_t) - 1)
        : 0);
static time_t const time_t_max =
    (TYPE_SIGNED(time_t)
        ? - (~ 0 < 0) - ((time_t) -1 << (CHAR_BIT * sizeof (time_t) - 1))
        : -1);


#ifndef TZ_MAX_TIMES
#define TZ_MAX_TIMES	2000
#endif /* !defined TZ_MAX_TIMES */

#ifndef TZ_MAX_TYPES
/* This must be at least 17 for Europe/Samara and Europe/Vilnius.  */
#define TZ_MAX_TYPES	256 /* Limited by what (unsigned char)'s can hold */
#endif /* !defined TZ_MAX_TYPES */

#ifndef TZ_MAX_CHARS
#define TZ_MAX_CHARS	50	/* Maximum number of abbreviation characters */
				/* (limited by what unsigned chars can hold) */
#endif /* !defined TZ_MAX_CHARS */

#ifndef TZ_MAX_LEAPS
#define TZ_MAX_LEAPS	50	/* Maximum number of leap second corrections */
#endif /* !defined TZ_MAX_LEAPS */

#define SECSPERMIN	60
#define MINSPERHOUR	60
#define HOURSPERDAY	24
#define DAYSPERWEEK	7
#define DAYSPERNYEAR	365
#define DAYSPERLYEAR	366
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	((int_fast32_t) SECSPERHOUR * HOURSPERDAY)
#define MONSPERYEAR	12

#define TM_SUNDAY	0
#define TM_MONDAY	1
#define TM_TUESDAY	2
#define TM_WEDNESDAY	3
#define TM_THURSDAY	4
#define TM_FRIDAY	5
#define TM_SATURDAY	6

#define TM_JANUARY	0
#define TM_FEBRUARY	1
#define TM_MARCH	2
#define TM_APRIL	3
#define TM_MAY		4
#define TM_JUNE		5
#define TM_JULY		6
#define TM_AUGUST	7
#define TM_SEPTEMBER	8
#define TM_OCTOBER	9
#define TM_NOVEMBER	10
#define TM_DECEMBER	11

#define TM_YEAR_BASE	1900

#define EPOCH_YEAR	1970
#define EPOCH_WDAY	TM_THURSDAY

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

/*
** Since everything in isleap is modulo 400 (or a factor of 400), we know that
**	isleap(y) == isleap(y % 400)
** and so
**	isleap(a + b) == isleap((a + b) % 400)
** or
**	isleap(a + b) == isleap(a % 400 + b % 400)
** This is true even if % means modulo rather than Fortran remainder
** (which is allowed by C89 but not C99).
** We use this to avoid addition overflow problems.
*/

#define isleap_sum(a, b)	isleap((a) % 400 + (b) % 400)

#ifndef TZ_ABBR_MAX_LEN
#define TZ_ABBR_MAX_LEN	16
#endif /* !defined TZ_ABBR_MAX_LEN */

#ifndef TZ_ABBR_CHAR_SET
#define TZ_ABBR_CHAR_SET \
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 :+-._"
#endif /* !defined TZ_ABBR_CHAR_SET */

#ifndef TZ_ABBR_ERR_CHAR
#define TZ_ABBR_ERR_CHAR	'_'
#endif /* !defined TZ_ABBR_ERR_CHAR */

#ifndef WILDABBR
/*
** Someone might make incorrect use of a time zone abbreviation:
**	1.	They might reference tzname[0] before calling tzset (explicitly
**		or implicitly).
**	2.	They might reference tzname[1] before calling tzset (explicitly
**		or implicitly).
**	3.	They might reference tzname[1] after setting to a time zone
**		in which Daylight Saving Time is never observed.
**	4.	They might reference tzname[0] after setting to a time zone
**		in which Standard Time is never observed.
**	5.	They might reference tm.TM_ZONE after calling offtime.
** What's best to do in the above cases is open to debate;
** for now, we just set things up so that in any of the five cases
** WILDABBR is used. Another possibility: initialize tzname[0] to the
** string "tzname[0] used before set", and similarly for the other cases.
** And another: initialize tzname[0] to "ERA", with an explanation in the
** manual page of what this "time zone abbreviation" means (doing this so
** that tzname[0] has the "normal" length of three characters).
*/
#define WILDABBR	"   "
#endif /* !defined WILDABBR */

#ifdef TM_ZONE
static const char	wildabbr[] = WILDABBR;
#endif

static const char	gmt[] = "GMT";

struct ttinfo {				/* time type information */
	int_fast32_t	tt_gmtoff;	/* UT offset in seconds */
	int		tt_isdst;	/* used to set tm_isdst */
	int		tt_abbrind;	/* abbreviation list index */
	int		tt_ttisstd;	/* true if transition is std time */
	int		tt_ttisgmt;	/* true if transition is UT */
};

struct lsinfo {				/* leap second information */
	time_t		ls_trans;	/* transition time */
	int_fast64_t	ls_corr;	/* correction to apply */
};

#define BIGGEST(a, b)	(((a) > (b)) ? (a) : (b))

#ifdef TZNAME_MAX
#define MY_TZNAME_MAX	TZNAME_MAX
#endif /* defined TZNAME_MAX */
#ifndef TZNAME_MAX
#define MY_TZNAME_MAX	255
#endif /* !defined TZNAME_MAX */

struct state {
	int		leapcnt;
	int		timecnt;
	int		typecnt;
	int		charcnt;
	int		goback;
	int		goahead;
	time_t		ats[TZ_MAX_TIMES];
	unsigned char	types[TZ_MAX_TIMES];
	struct ttinfo	ttis[TZ_MAX_TYPES];
	char		chars[BIGGEST(BIGGEST(TZ_MAX_CHARS + 1, sizeof gmt),
				(2 * (MY_TZNAME_MAX + 1)))];
	struct lsinfo	lsis[TZ_MAX_LEAPS];
	int		defaulttype; /* for early times or if no transitions */
};

struct rule {
	int		r_type;		/* type of rule--see below */
	int		r_day;		/* day number of rule */
	int		r_week;		/* week number of rule */
	int		r_mon;		/* month number of rule */
	int_fast32_t	r_time;		/* transition time of rule */
};

#define JULIAN_DAY		0	/* Jn - Julian day */
#define DAY_OF_YEAR		1	/* n - day of year */
#define MONTH_NTH_DAY_OF_WEEK	2	/* Mm.n.d - month, week, day of week */

/*
** Prototypes for static functions.
*/

static void		gmtload(struct state * sp);
static struct tm *	gmtsub(const time_t * timep, int_fast32_t offset,
				struct tm * tmp);
static int		increment_overflow(int * number, int delta);
static int		leaps_thru_end_of(int y) ATTRIBUTE_PURE;
static int		increment_overflow32(int_fast32_t * number, int delta);
static int		normalize_overflow32(int_fast32_t * tensptr,
				int * unitsptr, int base);
static int		normalize_overflow(int * tensptr, int * unitsptr,
				int base);
static time_t		time1(struct tm * tmp,
				struct tm * (*funcp)(const time_t *,
				int_fast32_t, struct tm *),
				int_fast32_t offset);
static time_t		time2(struct tm *tmp,
				struct tm * (*funcp)(const time_t *,
				int_fast32_t, struct tm*),
				int_fast32_t offset, int * okayp);
static time_t		time2sub(struct tm *tmp,
				struct tm * (*funcp)(const time_t *,
				int_fast32_t, struct tm*),
				int_fast32_t offset, int * okayp, int do_norm_secs);
static struct tm *	timesub(const time_t * timep, int_fast32_t offset,
				const struct state * sp, struct tm * tmp);
static int		tmcomp(const struct tm * atmp,
				const struct tm * btmp);

static struct state	gmtmem;
#define gmtptr		(&gmtmem)

static int		gmt_is_set;

static const int	mon_lengths[2][MONSPERYEAR] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const int	year_lengths[2] = {
	DAYSPERNYEAR, DAYSPERLYEAR
};

static void
gmtload(struct state *const sp)
{
    memset(sp, 0, sizeof(struct state));
    sp->typecnt = 1;
    sp->charcnt = 4;
    sp->chars[0] = 'G';
    sp->chars[1] = 'M';
    sp->chars[2] = 'T';
}

/*
** gmtsub is to gmtime as localsub is to localtime.
*/

static struct tm *
gmtsub(const time_t *const timep, const int_fast32_t offset,
       struct tm *const tmp)
{
	register struct tm *	result;

	if (!gmt_is_set) {
		gmt_is_set = true;
//		if (gmtptr != NULL)
			gmtload(gmtptr);
	}
	result = timesub(timep, offset, gmtptr, tmp);
#ifdef TM_ZONE
	/*
	** Could get fancy here and deliver something such as
	** "UT+xxxx" or "UT-xxxx" if offset is non-zero,
	** but this is no time for a treasure hunt.
	*/
	tmp->TM_ZONE = offset ? wildabbr : gmtptr ? gmtptr->chars : gmt;
#endif /* defined TM_ZONE */
	return result;
}

/*
** Return the number of leap years through the end of the given year
** where, to make the math easy, the answer for year zero is defined as zero.
*/

static int
leaps_thru_end_of(register const int y)
{
	return (y >= 0) ? (y / 4 - y / 100 + y / 400) :
		-(leaps_thru_end_of(-(y + 1)) + 1);
}

static struct tm *
timesub(const time_t *const timep, const int_fast32_t offset,
	register const struct state *const sp,
	register struct tm *const tmp)
{
	register const struct lsinfo *	lp;
	register time_t			tdays;
	register int			idays;	/* unsigned would be so 2003 */
	register int_fast64_t		rem;
	int				y;
	register const int *		ip;
	register int_fast64_t		corr;
	register int			hit;
	register int			i;

	corr = 0;
	hit = 0;
	i = (sp == NULL) ? 0 : sp->leapcnt;
	while (--i >= 0) {
		lp = &sp->lsis[i];
		if (*timep >= lp->ls_trans) {
			if (*timep == lp->ls_trans) {
				hit = ((i == 0 && lp->ls_corr > 0) ||
					lp->ls_corr > sp->lsis[i - 1].ls_corr);
				if (hit)
					while (i > 0 &&
						sp->lsis[i].ls_trans ==
						sp->lsis[i - 1].ls_trans + 1 &&
						sp->lsis[i].ls_corr ==
						sp->lsis[i - 1].ls_corr + 1) {
							++hit;
							--i;
					}
			}
			corr = lp->ls_corr;
			break;
		}
	}
	y = EPOCH_YEAR;
	tdays = *timep / SECSPERDAY;
	rem = *timep - tdays * SECSPERDAY;
	while (tdays < 0 || tdays >= year_lengths[isleap(y)]) {
		int		newy;
		register time_t	tdelta;
		register int	idelta;
		register int	leapdays;

		tdelta = tdays / DAYSPERLYEAR;
		if (! ((! TYPE_SIGNED(time_t) || INT_MIN <= tdelta)
		       && tdelta <= INT_MAX))
			return NULL;
		idelta = (int) tdelta;
		if (idelta == 0)
			idelta = (tdays < 0) ? -1 : 1;
		newy = y;
		if (increment_overflow(&newy, idelta))
			return NULL;
		leapdays = leaps_thru_end_of(newy - 1) -
			leaps_thru_end_of(y - 1);
		tdays -= ((time_t) newy - y) * DAYSPERNYEAR;
		tdays -= leapdays;
		y = newy;
	}
	{
		register int_fast32_t	seconds;

		seconds = (int_fast32_t) (tdays * SECSPERDAY);
		tdays = seconds / SECSPERDAY;
		rem += seconds - tdays * SECSPERDAY;
	}
	/*
	** Given the range, we can now fearlessly cast...
	*/
	idays = (int) tdays;
	rem += offset - corr;
	while (rem < 0) {
		rem += SECSPERDAY;
		--idays;
	}
	while (rem >= SECSPERDAY) {
		rem -= SECSPERDAY;
		++idays;
	}
	while (idays < 0) {
		if (increment_overflow(&y, -1))
			return NULL;
		idays += year_lengths[isleap(y)];
	}
	while (idays >= year_lengths[isleap(y)]) {
		idays -= year_lengths[isleap(y)];
		if (increment_overflow(&y, 1))
			return NULL;
	}
	tmp->tm_year = y;
	if (increment_overflow(&tmp->tm_year, -TM_YEAR_BASE))
		return NULL;
	tmp->tm_yday = idays;
	/*
	** The "extra" mods below avoid overflow problems.
	*/
	tmp->tm_wday = EPOCH_WDAY +
		((y - EPOCH_YEAR) % DAYSPERWEEK) *
		(DAYSPERNYEAR % DAYSPERWEEK) +
		leaps_thru_end_of(y - 1) -
		leaps_thru_end_of(EPOCH_YEAR - 1) +
		idays;
	tmp->tm_wday %= DAYSPERWEEK;
	if (tmp->tm_wday < 0)
		tmp->tm_wday += DAYSPERWEEK;
	tmp->tm_hour = (int) (rem / SECSPERHOUR);
	rem %= SECSPERHOUR;
	tmp->tm_min = (int) (rem / SECSPERMIN);
	/*
	** A positive leap second requires a special
	** representation. This uses "... ??:59:60" et seq.
	*/
	tmp->tm_sec = (int) (rem % SECSPERMIN) + hit;
	ip = mon_lengths[isleap(y)];
	for (tmp->tm_mon = 0; idays >= ip[tmp->tm_mon]; ++(tmp->tm_mon))
		idays -= ip[tmp->tm_mon];
	tmp->tm_mday = (int) (idays + 1);
	tmp->tm_isdst = 0;
#ifdef TM_GMTOFF
	tmp->TM_GMTOFF = offset;
#endif /* defined TM_GMTOFF */
	return tmp;
}

/*
** Adapted from code provided by Robert Elz, who writes:
**	The "best" way to do mktime I think is based on an idea of Bob
**	Kridle's (so its said...) from a long time ago.
**	It does a binary search of the time_t space. Since time_t's are
**	just 32 bits, its a max of 32 iterations (even at 64 bits it
**	would still be very reasonable).
*/

#ifndef WRONG
#define WRONG	(-1)
#endif /* !defined WRONG */

/*
** Normalize logic courtesy Paul Eggert.
*/

static int
increment_overflow(int *const ip, int j)
{
	register int const	i = *ip;

	/*
	** If i >= 0 there can only be overflow if i + j > INT_MAX
	** or if j > INT_MAX - i; given i >= 0, INT_MAX - i cannot overflow.
	** If i < 0 there can only be overflow if i + j < INT_MIN
	** or if j < INT_MIN - i; given i < 0, INT_MIN - i cannot overflow.
	*/
	if ((i >= 0) ? (j > INT_MAX - i) : (j < INT_MIN - i))
		return true;
	*ip += j;
	return false;
}

static int
increment_overflow32(int_fast32_t *const lp, int const m)
{
	register int_fast32_t const	l = *lp;

	if ((l >= 0) ? (m > INT_FAST32_MAX - l) : (m < INT_FAST32_MIN - l))
		return true;
	*lp += m;
	return false;
}

static int
normalize_overflow(int *const tensptr, int *const unitsptr, const int base)
{
	register int	tensdelta;

	tensdelta = (*unitsptr >= 0) ?
		(*unitsptr / base) :
		(-1 - (-1 - *unitsptr) / base);
	*unitsptr -= tensdelta * base;
	return increment_overflow(tensptr, tensdelta);
}

static int
normalize_overflow32(int_fast32_t *const tensptr, int *const unitsptr,
		     const int base)
{
	register int	tensdelta;

	tensdelta = (*unitsptr >= 0) ?
		(*unitsptr / base) :
		(-1 - (-1 - *unitsptr) / base);
	*unitsptr -= tensdelta * base;
	return increment_overflow32(tensptr, tensdelta);
}

static int
tmcomp(register const struct tm *const atmp,
       register const struct tm *const btmp)
{
	register int	result;

	if (atmp->tm_year != btmp->tm_year)
		return atmp->tm_year < btmp->tm_year ? -1 : 1;
	if ((result = (atmp->tm_mon - btmp->tm_mon)) == 0 &&
		(result = (atmp->tm_mday - btmp->tm_mday)) == 0 &&
		(result = (atmp->tm_hour - btmp->tm_hour)) == 0 &&
		(result = (atmp->tm_min - btmp->tm_min)) == 0)
			result = atmp->tm_sec - btmp->tm_sec;
	return result;
}

static time_t
time2sub(struct tm *const tmp,
	 struct tm *(*const funcp)(const time_t *, int_fast32_t, struct tm *),
	 const int_fast32_t offset,
	 int *const okayp,
	 const int do_norm_secs)
{
	register const struct state *	sp;
	register int			dir;
	register int			i, j;
	register int			saved_seconds;
	register int_fast32_t			li;
	register time_t			lo;
	register time_t			hi;
	int_fast32_t				y;
	time_t				newt;
	time_t				t;
	struct tm			yourtm, mytm;

	*okayp = false;
	yourtm = *tmp;
	if (do_norm_secs) {
		if (normalize_overflow(&yourtm.tm_min, &yourtm.tm_sec,
			SECSPERMIN))
				return WRONG;
	}
	if (normalize_overflow(&yourtm.tm_hour, &yourtm.tm_min, MINSPERHOUR))
		return WRONG;
	if (normalize_overflow(&yourtm.tm_mday, &yourtm.tm_hour, HOURSPERDAY))
		return WRONG;
	y = yourtm.tm_year;
	if (normalize_overflow32(&y, &yourtm.tm_mon, MONSPERYEAR))
		return WRONG;
	/*
	** Turn y into an actual year number for now.
	** It is converted back to an offset from TM_YEAR_BASE later.
	*/
	if (increment_overflow32(&y, TM_YEAR_BASE))
		return WRONG;
	while (yourtm.tm_mday <= 0) {
		if (increment_overflow32(&y, -1))
			return WRONG;
		li = y + (1 < yourtm.tm_mon);
		yourtm.tm_mday += year_lengths[isleap(li)];
	}
	while (yourtm.tm_mday > DAYSPERLYEAR) {
		li = y + (1 < yourtm.tm_mon);
		yourtm.tm_mday -= year_lengths[isleap(li)];
		if (increment_overflow32(&y, 1))
			return WRONG;
	}
	for ( ; ; ) {
		i = mon_lengths[isleap(y)][yourtm.tm_mon];
		if (yourtm.tm_mday <= i)
			break;
		yourtm.tm_mday -= i;
		if (++yourtm.tm_mon >= MONSPERYEAR) {
			yourtm.tm_mon = 0;
			if (increment_overflow32(&y, 1))
				return WRONG;
		}
	}
	if (increment_overflow32(&y, -TM_YEAR_BASE))
		return WRONG;
	yourtm.tm_year = y;
	if (yourtm.tm_year != y)
		return WRONG;
	if (yourtm.tm_sec >= 0 && yourtm.tm_sec < SECSPERMIN)
		saved_seconds = 0;
	else if (y + TM_YEAR_BASE < EPOCH_YEAR) {
		/*
		** We can't set tm_sec to 0, because that might push the
		** time below the minimum representable time.
		** Set tm_sec to 59 instead.
		** This assumes that the minimum representable time is
		** not in the same minute that a leap second was deleted from,
		** which is a safer assumption than using 58 would be.
		*/
		if (increment_overflow(&yourtm.tm_sec, 1 - SECSPERMIN))
			return WRONG;
		saved_seconds = yourtm.tm_sec;
		yourtm.tm_sec = SECSPERMIN - 1;
	} else {
		saved_seconds = yourtm.tm_sec;
		yourtm.tm_sec = 0;
	}
	/*
	** Do a binary search (this works whatever time_t's type is).
	*/
	if (!TYPE_SIGNED(time_t)) {
		lo = 0;
		hi = lo - 1;
	} else {
		lo = 1;
		for (i = 0; i < (int) TYPE_BIT(time_t) - 1; ++i)
			lo *= 2;
		hi = -(lo + 1);
	}
	for ( ; ; ) {
		t = lo / 2 + hi / 2;
		if (t < lo)
			t = lo;
		else if (t > hi)
			t = hi;
		if ((*funcp)(&t, offset, &mytm) == NULL) {
			/*
			** Assume that t is too extreme to be represented in
			** a struct tm; arrange things so that it is less
			** extreme on the next pass.
			*/
			dir = (t > 0) ? 1 : -1;
		} else	dir = tmcomp(&mytm, &yourtm);
		if (dir != 0) {
			if (t == lo) {
				if (t == time_t_max)
					return WRONG;
				++t;
				++lo;
			} else if (t == hi) {
				if (t == time_t_min)
					return WRONG;
				--t;
				--hi;
			}
			if (lo > hi)
				return WRONG;
			if (dir > 0)
				hi = t;
			else	lo = t;
			continue;
		}
		if (yourtm.tm_isdst < 0 || mytm.tm_isdst == yourtm.tm_isdst)
			break;
		/*
		** Right time, wrong type.
		** Hunt for right time, right type.
		** It's okay to guess wrong since the guess
		** gets checked.
		*/
		sp = (const struct state *) gmtptr;
		if (sp == NULL)
			return WRONG;
		for (i = sp->typecnt - 1; i >= 0; --i) {
			if (sp->ttis[i].tt_isdst != yourtm.tm_isdst)
				continue;
			for (j = sp->typecnt - 1; j >= 0; --j) {
				if (sp->ttis[j].tt_isdst == yourtm.tm_isdst)
					continue;
				newt = t + sp->ttis[j].tt_gmtoff -
					sp->ttis[i].tt_gmtoff;
				if ((*funcp)(&newt, offset, &mytm) == NULL)
					continue;
				if (tmcomp(&mytm, &yourtm) != 0)
					continue;
				if (mytm.tm_isdst != yourtm.tm_isdst)
					continue;
				/*
				** We have a match.
				*/
				t = newt;
				goto label;
			}
		}
		return WRONG;
	}
label:
	newt = t + saved_seconds;
	if ((newt < t) != (saved_seconds < 0))
		return WRONG;
	t = newt;
	if ((*funcp)(&t, offset, tmp))
		*okayp = true;
	return t;
}

static time_t
time2(struct tm * const	tmp,
      struct tm * (*const funcp)(const time_t *, int_fast32_t, struct tm *),
      const int_fast32_t offset,
      int *const okayp)
{
	time_t	t;

	/*
	** First try without normalization of seconds
	** (in case tm_sec contains a value associated with a leap second).
	** If that fails, try with normalization of seconds.
	*/
	t = time2sub(tmp, funcp, offset, okayp, false);
	return *okayp ? t : time2sub(tmp, funcp, offset, okayp, true);
}

static time_t
time1(struct tm *const tmp,
      struct tm *(*const funcp) (const time_t *, int_fast32_t, struct tm *),
      const int_fast32_t offset)
{
	register time_t			t;
	register const struct state *	sp;
	register int			samei, otheri;
	register int			sameind, otherind;
	register int			i;
	register int			nseen;
	int				seen[TZ_MAX_TYPES];
	int				types[TZ_MAX_TYPES];
	int				okay;

	if (tmp == NULL) {
		errno = EINVAL;
		return WRONG;
	}
	if (tmp->tm_isdst > 1)
		tmp->tm_isdst = 1;
	t = time2(tmp, funcp, offset, &okay);
	if (okay)
		return t;
	if (tmp->tm_isdst < 0)
#ifdef PCTS
		/*
		** POSIX Conformance Test Suite code courtesy Grant Sullivan.
		*/
		tmp->tm_isdst = 0;	/* reset to std and try again */
#else
		return t;
#endif /* !defined PCTS */
	/*
	** We're supposed to assume that somebody took a time of one type
	** and did some math on it that yielded a "struct tm" that's bad.
	** We try to divine the type they started from and adjust to the
	** type they need.
	*/
	sp = (const struct state *) gmtptr;
	if (sp == NULL)
		return WRONG;
	for (i = 0; i < sp->typecnt; ++i)
		seen[i] = false;
	nseen = 0;
	for (i = sp->timecnt - 1; i >= 0; --i)
		if (!seen[sp->types[i]]) {
			seen[sp->types[i]] = true;
			types[nseen++] = sp->types[i];
		}
	for (sameind = 0; sameind < nseen; ++sameind) {
		samei = types[sameind];
		if (sp->ttis[samei].tt_isdst != tmp->tm_isdst)
			continue;
		for (otherind = 0; otherind < nseen; ++otherind) {
			otheri = types[otherind];
			if (sp->ttis[otheri].tt_isdst == tmp->tm_isdst)
				continue;
			tmp->tm_sec += sp->ttis[otheri].tt_gmtoff -
					sp->ttis[samei].tt_gmtoff;
			tmp->tm_isdst = !tmp->tm_isdst;
			t = time2(tmp, funcp, offset, &okay);
			if (okay)
				return t;
			tmp->tm_sec -= sp->ttis[otheri].tt_gmtoff -
					sp->ttis[samei].tt_gmtoff;
			tmp->tm_isdst = !tmp->tm_isdst;
		}
	}
	return WRONG;
}

time_t
_bson_timegm(struct tm *const tmp)
{
	if (tmp != NULL)
		tmp->tm_isdst = 0;
	return time1(tmp, gmtsub, 0L);
}

#endif
/*
 * Copyright 2015 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// #include "bson-version.h"

// #include "bson-version-functions.h"



/**
 * bson_get_major_version:
 *
 * Helper function to return the runtime major version of the library.
 */
int
bson_get_major_version (void)
{
   return BSON_MAJOR_VERSION;
}


/**
 * bson_get_minor_version:
 *
 * Helper function to return the runtime minor version of the library.
 */
int
bson_get_minor_version (void)
{
   return BSON_MINOR_VERSION;
}

/**
 * bson_get_micro_version:
 *
 * Helper function to return the runtime micro version of the library.
 */
int
bson_get_micro_version (void)
{
   return BSON_MICRO_VERSION;
}

/**
 * bson_get_version:
 *
 * Helper function to return the runtime string version of the library.
 */
const char *
bson_get_version (void)
{
   return BSON_VERSION_S;
}

/**
 * bson_check_version:
 *
 * True if libmongoc's version is greater than or equal to the required
 * version.
 */
bool
bson_check_version (int required_major,
                    int required_minor,
                    int required_micro)
{
   return BSON_CHECK_VERSION(required_major, required_minor, required_micro);
}
/*
 * Copyright 2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// #include "bson-atomic.h"



/*
 * We should only ever hit these on non-Windows systems, for which we require
 * pthread support. Therefore, we will avoid making a threading portability
 * for threads here and just use pthreads directly.
 */


#ifdef __BSON_NEED_BARRIER
#include <pthread.h>
static pthread_mutex_t gBarrier = PTHREAD_MUTEX_INITIALIZER;
void
bson_memory_barrier (void)
{
   pthread_mutex_lock (&gBarrier);
   pthread_mutex_unlock (&gBarrier);
}
#endif


#ifdef __BSON_NEED_ATOMIC_32
#warning "Using mutex to emulate 32-bit atomics."
#include <pthread.h>
static pthread_mutex_t gSync32 = PTHREAD_MUTEX_INITIALIZER;
int32_t
bson_atomic_int_add (volatile int32_t *p,
                     int32_t           n)
{
   int ret;

   pthread_mutex_lock (&gSync32);
   *p += n;
   ret = *p;
   pthread_mutex_unlock (&gSync32);

   return ret;
}
#endif


#ifdef __BSON_NEED_ATOMIC_64
#include <pthread.h>
static pthread_mutex_t gSync64 = PTHREAD_MUTEX_INITIALIZER;
int64_t
bson_atomic_int64_add (volatile int64_t *p,
                       int64_t           n)
{
   int64_t ret;

   pthread_mutex_lock (&gSync64);
   *p += n;
   ret = *p;
   pthread_mutex_unlock (&gSync64);

   return ret;
}
#endif
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #include "bson-compat.h"


#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__linux__)
#include <sys/syscall.h>
#endif

// #include "bson-atomic.h"

// #include "bson-clock.h"

// #include "bson-context.h"

// #include "bson-context-private.h"

// #include "bson-md5.h"

// #include "bson-memory.h"

// #include "bson-thread-private.h"



#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif


/*
 * Globals.
 */
static bson_context_t gContextDefault;


#if defined(__linux__)
static uint16_t
gettid (void)
{
   return syscall (SYS_gettid);
}
#endif


/*
 *--------------------------------------------------------------------------
 *
 * _bson_context_get_oid_host --
 *
 *       Retrieves the first three bytes of MD5(hostname) and assigns them
 *       to the host portion of oid.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is modified.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_context_get_oid_host (bson_context_t *context,  /* IN */
                            bson_oid_t     *oid)      /* OUT */
{
   uint8_t *bytes = (uint8_t *)oid;
   uint8_t digest[16];
   bson_md5_t md5;
   char hostname[HOST_NAME_MAX];

   BSON_ASSERT (context);
   BSON_ASSERT (oid);

   gethostname (hostname, sizeof hostname);
   hostname[HOST_NAME_MAX - 1] = '\0';

   bson_md5_init (&md5);
   bson_md5_append (&md5, (const uint8_t *)hostname, (uint32_t)strlen (hostname));
   bson_md5_finish (&md5, &digest[0]);

   bytes[4] = digest[0];
   bytes[5] = digest[1];
   bytes[6] = digest[2];
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_context_get_oid_host_cached --
 *
 *       Fetch the cached copy of the MD5(hostname).
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is modified.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_context_get_oid_host_cached (bson_context_t *context, /* IN */
                                   bson_oid_t     *oid)     /* OUT */
{
   BSON_ASSERT (context);
   BSON_ASSERT (oid);

   oid->bytes[4] = context->md5[0];
   oid->bytes[5] = context->md5[1];
   oid->bytes[6] = context->md5[2];
}


static BSON_INLINE uint16_t
_bson_getpid (void)
{
   uint16_t pid;
#ifdef BSON_OS_WIN32
   DWORD real_pid;

   real_pid = GetCurrentProcessId ();
   pid = (real_pid & 0xFFFF) ^ ((real_pid >> 16) & 0xFFFF);
#else
   pid = getpid ();
#endif

   return pid;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_context_get_oid_pid --
 *
 *       Initialize the pid field of @oid.
 *
 *       The pid field is 2 bytes, big-endian for memcmp().
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is modified.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_context_get_oid_pid (bson_context_t *context, /* IN */
                           bson_oid_t     *oid)     /* OUT */
{
   uint16_t pid = _bson_getpid ();
   uint8_t *bytes = (uint8_t *)&pid;

   BSON_ASSERT (context);
   BSON_ASSERT (oid);

   pid = BSON_UINT16_TO_BE (pid);

   oid->bytes[7] = bytes[0];
   oid->bytes[8] = bytes[1];
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_context_get_oid_pid_cached --
 *
 *       Fetch the cached copy of the current pid.
 *       This helps avoid multiple calls to getpid() which is slower
 *       on some systems.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is modified.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_context_get_oid_pid_cached (bson_context_t *context, /* IN */
                                  bson_oid_t     *oid)     /* OUT */
{
   oid->bytes[7] = context->pidbe[0];
   oid->bytes[8] = context->pidbe[1];
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_context_get_oid_seq32 --
 *
 *       32-bit sequence generator, non-thread-safe version.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is modified.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_context_get_oid_seq32 (bson_context_t *context, /* IN */
                             bson_oid_t     *oid)     /* OUT */
{
   uint32_t seq = context->seq32++;

   seq = BSON_UINT32_TO_BE (seq);
   memcpy (&oid->bytes[9], ((uint8_t *)&seq) + 1, 3);
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_context_get_oid_seq32_threadsafe --
 *
 *       Thread-safe version of 32-bit sequence generator.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is modified.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_context_get_oid_seq32_threadsafe (bson_context_t *context, /* IN */
                                        bson_oid_t     *oid)     /* OUT */
{
   int32_t seq = bson_atomic_int_add (&context->seq32, 1);

   seq = BSON_UINT32_TO_BE (seq);
   memcpy (&oid->bytes[9], ((uint8_t *)&seq) + 1, 3);
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_context_get_oid_seq64 --
 *
 *       64-bit oid sequence generator, non-thread-safe version.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is modified.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_context_get_oid_seq64 (bson_context_t *context, /* IN */
                             bson_oid_t     *oid)     /* OUT */
{
   uint64_t seq;

   BSON_ASSERT (context);
   BSON_ASSERT (oid);

   seq = BSON_UINT64_TO_BE (context->seq64++);
   memcpy (&oid->bytes[4], &seq, sizeof (seq));
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_context_get_oid_seq64_threadsafe --
 *
 *       Thread-safe 64-bit sequence generator.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @oid is modified.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_context_get_oid_seq64_threadsafe (bson_context_t *context, /* IN */
                                        bson_oid_t     *oid)     /* OUT */
{
   int64_t seq = bson_atomic_int64_add (&context->seq64, 1);

   seq = BSON_UINT64_TO_BE (seq);
   memcpy (&oid->bytes[4], &seq, sizeof (seq));
}


static void
_bson_context_init (bson_context_t *context,    /* IN */
                    bson_context_flags_t flags) /* IN */
{
   struct timeval tv;
   uint16_t pid;
   unsigned int seed[3];
   unsigned int real_seed;
   bson_oid_t oid;

   context->flags = flags;
   context->oid_get_host = _bson_context_get_oid_host_cached;
   context->oid_get_pid = _bson_context_get_oid_pid_cached;
   context->oid_get_seq32 = _bson_context_get_oid_seq32;
   context->oid_get_seq64 = _bson_context_get_oid_seq64;

   /*
    * Generate a seed for our the random starting position of our increment
    * bytes. We mask off the last nibble so that the last digit of the OID will
    * start at zero. Just to be nice.
    *
    * The seed itself is made up of the current time in seconds, milliseconds,
    * and pid xored together. I welcome better solutions if at all necessary.
    */
   bson_gettimeofday (&tv);
   seed[0] = (unsigned int)tv.tv_sec;
   seed[1] = (unsigned int)tv.tv_usec;
   seed[2] = _bson_getpid ();
   real_seed = seed[0] ^ seed[1] ^ seed[2];

#ifdef BSON_OS_WIN32
   /* ms's runtime is multithreaded by default, so no rand_r */
   srand(real_seed);
   context->seq32 = rand() & 0x007FFFF0;
#else
   context->seq32 = rand_r (&real_seed) & 0x007FFFF0;
#endif

   if ((flags & BSON_CONTEXT_DISABLE_HOST_CACHE)) {
      context->oid_get_host = _bson_context_get_oid_host;
   } else {
      _bson_context_get_oid_host (context, &oid);
      context->md5[0] = oid.bytes[4];
      context->md5[1] = oid.bytes[5];
      context->md5[2] = oid.bytes[6];
   }

   if ((flags & BSON_CONTEXT_THREAD_SAFE)) {
      context->oid_get_seq32 = _bson_context_get_oid_seq32_threadsafe;
      context->oid_get_seq64 = _bson_context_get_oid_seq64_threadsafe;
   }

   if ((flags & BSON_CONTEXT_DISABLE_PID_CACHE)) {
      context->oid_get_pid = _bson_context_get_oid_pid;
   } else {
      pid = BSON_UINT16_TO_BE (_bson_getpid());
#if defined(__linux__)

      if ((flags & BSON_CONTEXT_USE_TASK_ID)) {
         int32_t tid;

         if ((tid = gettid ())) {
            pid = BSON_UINT16_TO_BE (tid);
         }
      }

#endif
      memcpy (&context->pidbe[0], &pid, 2);
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_context_new --
 *
 *       Initializes a new context with the flags specified.
 *
 *       In most cases, you want to call this with @flags set to
 *       BSON_CONTEXT_NONE.
 *
 *       If you are running on Linux, %BSON_CONTEXT_USE_TASK_ID can result
 *       in a healthy speedup for multi-threaded scenarios.
 *
 *       If you absolutely must have a single context for your application
 *       and use more than one thread, then %BSON_CONTEXT_THREAD_SAFE should
 *       be bitwise-or'd with your flags. This requires synchronization
 *       between threads.
 *
 *       If you expect your hostname to change often, you may consider
 *       specifying %BSON_CONTEXT_DISABLE_HOST_CACHE so that gethostname()
 *       is called for every OID generated. This is much slower.
 *
 *       If you expect your pid to change without notice, such as from an
 *       unexpected call to fork(), then specify
 *       %BSON_CONTEXT_DISABLE_PID_CACHE.
 *
 * Returns:
 *       A newly allocated bson_context_t that should be freed with
 *       bson_context_destroy().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bson_context_t *
bson_context_new (bson_context_flags_t flags)
{
   bson_context_t *context;

   context = bson_malloc0 (sizeof *context);
   _bson_context_init (context, flags);

   return context;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_context_destroy --
 *
 *       Cleans up a bson_context_t and releases any associated resources.
 *       This should be called when you are done using @context.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_context_destroy (bson_context_t *context)  /* IN */
{
   if (context != &gContextDefault) {
      memset (context, 0, sizeof *context);
      bson_free (context);
   }
}


static
BSON_ONCE_FUN(_bson_context_init_default)
{
   _bson_context_init (&gContextDefault,
                       (BSON_CONTEXT_THREAD_SAFE |
                        BSON_CONTEXT_DISABLE_PID_CACHE));
   BSON_ONCE_RETURN;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_context_get_default --
 *
 *       Fetches the default, thread-safe implementation of #bson_context_t.
 *       If you need faster generation, it is recommended you create your
 *       own #bson_context_t with bson_context_new().
 *
 * Returns:
 *       A shared instance to the default #bson_context_t. This should not
 *       be modified or freed.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bson_context_t *
bson_context_get_default (void)
{
   static bson_once_t once = BSON_ONCE_INIT;

   bson_once (&once, _bson_context_init_default);

   return &gContextDefault;
}
/*
 * Copyright 2013-2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// #include "bson-iter.h"



#define ITER_TYPE(i) ((bson_type_t) *((i)->raw + (i)->type))


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_init --
 *
 *       Initializes @iter to be used to iterate @bson.
 *
 * Returns:
 *       true if bson_iter_t was initialized. otherwise false.
 *
 * Side effects:
 *       @iter is initialized.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_init (bson_iter_t  *iter, /* OUT */
                const bson_t *bson) /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (bson);

   if (BSON_UNLIKELY (bson->len < 5)) {
      memset (iter, 0, sizeof *iter);
      return false;
   }

   iter->raw = bson_get_data (bson);
   iter->len = bson->len;
   iter->off = 0;
   iter->type = 0;
   iter->key = 0;
   iter->d1 = 0;
   iter->d2 = 0;
   iter->d3 = 0;
   iter->d4 = 0;
   iter->next_off = 4;
   iter->err_off = 0;

   return true;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_recurse --
 *
 *       Creates a new sub-iter looking at the document or array that @iter
 *       is currently pointing at.
 *
 * Returns:
 *       true if successful and @child was initialized.
 *
 * Side effects:
 *       @child is initialized.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_recurse (const bson_iter_t *iter,  /* IN */
                   bson_iter_t       *child) /* OUT */
{
   const uint8_t *data = NULL;
   uint32_t len = 0;

   BSON_ASSERT (iter);
   BSON_ASSERT (child);

   if (ITER_TYPE (iter) == BSON_TYPE_DOCUMENT) {
      bson_iter_document (iter, &len, &data);
   } else if (ITER_TYPE (iter) == BSON_TYPE_ARRAY) {
      bson_iter_array (iter, &len, &data);
   } else {
      return false;
   }

   child->raw = data;
   child->len = len;
   child->off = 0;
   child->type = 0;
   child->key = 0;
   child->d1 = 0;
   child->d2 = 0;
   child->d3 = 0;
   child->d4 = 0;
   child->next_off = 4;
   child->err_off = 0;

   return true;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_init_find --
 *
 *       Initializes a #bson_iter_t and moves the iter to the first field
 *       matching @key.
 *
 * Returns:
 *       true if the field named @key was found; otherwise false.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_init_find (bson_iter_t  *iter, /* INOUT */
                     const bson_t *bson, /* IN */
                     const char   *key)  /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   return bson_iter_init (iter, bson) && bson_iter_find (iter, key);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_init_find_case --
 *
 *       A case-insensitive version of bson_iter_init_find().
 *
 * Returns:
 *       true if the field was found and @iter is observing that field.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_init_find_case (bson_iter_t  *iter, /* INOUT */
                          const bson_t *bson, /* IN */
                          const char   *key)  /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   return bson_iter_init (iter, bson) && bson_iter_find_case (iter, key);
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_iter_find_with_len --
 *
 *       Internal helper for finding an exact key.
 *
 * Returns:
 *       true if the field @key was found.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static bool
_bson_iter_find_with_len (bson_iter_t *iter,   /* INOUT */
                          const char  *key,    /* IN */
                          int          keylen) /* IN */
{
   const char *ikey;

   if (keylen == 0) {
      return false;
   }

   if (keylen < 0) {
      keylen = (int)strlen (key);
   }

   while (bson_iter_next (iter)) {
      ikey = bson_iter_key (iter);

      if ((0 == strncmp (key, ikey, keylen)) && (ikey [keylen] == '\0')) {
         return true;
      }
   }

   return false;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_find --
 *
 *       Searches through @iter starting from the current position for a key
 *       matching @key. This is a case-sensitive search meaning "KEY" and
 *       "key" would NOT match.
 *
 * Returns:
 *       true if @key is found.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_find (bson_iter_t *iter, /* INOUT */
                const char  *key)  /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (key);

   return _bson_iter_find_with_len (iter, key, -1);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_find_case --
 *
 *       Searches through @iter starting from the current position for a key
 *       matching @key. This is a case-insensitive search meaning "KEY" and
 *       "key" would match.
 *
 * Returns:
 *       true if @key is found.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_find_case (bson_iter_t *iter, /* INOUT */
                     const char  *key)  /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (key);

   while (bson_iter_next (iter)) {
#ifdef BSON_OS_WIN32
      if (!_stricmp(key, bson_iter_key (iter))) {
#else
      if (!strcasecmp (key, bson_iter_key (iter))) {
#endif
         return true;
      }
   }

   return false;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_find_descendant --
 *
 *       Locates a descendant using the "parent.child.key" notation. This
 *       operates similar to bson_iter_find() except that it can recurse
 *       into children documents using the dot notation.
 *
 * Returns:
 *       true if the descendant was found and @descendant was initialized.
 *
 * Side effects:
 *       @descendant may be initialized.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_find_descendant (bson_iter_t *iter,       /* INOUT */
                           const char  *dotkey,     /* IN */
                           bson_iter_t *descendant) /* OUT */
{
   bson_iter_t tmp;
   const char *dot;
   size_t sublen;

   BSON_ASSERT (iter);
   BSON_ASSERT (dotkey);
   BSON_ASSERT (descendant);

   if ((dot = strchr (dotkey, '.'))) {
      sublen = dot - dotkey;
   } else {
      sublen = strlen (dotkey);
   }

   if (_bson_iter_find_with_len (iter, dotkey, (int)sublen)) {
      if (!dot) {
         *descendant = *iter;
         return true;
      }

      if (BSON_ITER_HOLDS_DOCUMENT (iter) || BSON_ITER_HOLDS_ARRAY (iter)) {
         if (bson_iter_recurse (iter, &tmp)) {
            return bson_iter_find_descendant (&tmp, dot + 1, descendant);
         }
      }
   }

   return false;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_key --
 *
 *       Retrieves the key of the current field. The resulting key is valid
 *       while @iter is valid.
 *
 * Returns:
 *       A string that should not be modified or freed.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

const char *
bson_iter_key (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   return bson_iter_key_unsafe (iter);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_type --
 *
 *       Retrieves the type of the current field.  It may be useful to check
 *       the type using the BSON_ITER_HOLDS_*() macros.
 *
 * Returns:
 *       A bson_type_t.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bson_type_t
bson_iter_type (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (iter->raw);
   BSON_ASSERT (iter->len);

   return bson_iter_type_unsafe (iter);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_next --
 *
 *       Advances @iter to the next field of the underlying BSON document.
 *       If all fields have been exhausted, then %false is returned.
 *
 *       It is a programming error to use @iter after this function has
 *       returned false.
 *
 * Returns:
 *       true if the iter was advanced to the next record.
 *       otherwise false and @iter should be considered invalid.
 *
 * Side effects:
 *       @iter may be invalidated.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_next (bson_iter_t *iter) /* INOUT */
{
   const uint8_t *data;
   uint32_t o;
   unsigned int len;

   BSON_ASSERT (iter);

   if (!iter->raw) {
      return false;
   }

   data = iter->raw;
   len = iter->len;

   iter->off = iter->next_off;
   iter->type = iter->off;
   iter->key = iter->off + 1;
   iter->d1 = 0;
   iter->d2 = 0;
   iter->d3 = 0;
   iter->d4 = 0;

   for (o = iter->off + 1; o < len; o++) {
      if (!data [o]) {
         iter->d1 = ++o;
         goto fill_data_fields;
      }
   }

   goto mark_invalid;

fill_data_fields:

   switch (ITER_TYPE (iter)) {
   case BSON_TYPE_DATE_TIME:
   case BSON_TYPE_DOUBLE:
   case BSON_TYPE_INT64:
   case BSON_TYPE_TIMESTAMP:
      iter->next_off = o + 8;
      break;
   case BSON_TYPE_CODE:
   case BSON_TYPE_SYMBOL:
   case BSON_TYPE_UTF8:
      {
         uint32_t l;

         if ((o + 4) >= len) {
            iter->err_off = o;
            goto mark_invalid;
         }

         iter->d2 = o + 4;
         memcpy (&l, iter->raw + iter->d1, sizeof (l));
         l = BSON_UINT32_FROM_LE (l);

         if (l > (len - (o + 4))) {
            iter->err_off = o;
            goto mark_invalid;
         }

         iter->next_off = o + 4 + l;

         /*
          * Make sure the string length includes the NUL byte.
          */
         if (BSON_UNLIKELY ((l == 0) || (iter->next_off >= len))) {
            iter->err_off = o;
            goto mark_invalid;
         }

         /*
          * Make sure the last byte is a NUL byte.
          */
         if (BSON_UNLIKELY ((iter->raw + iter->d2)[l - 1] != '\0')) {
            iter->err_off = o + 4 + l - 1;
            goto mark_invalid;
         }
      }
      break;
   case BSON_TYPE_BINARY:
      {
         bson_subtype_t subtype;
         uint32_t l;

         if (o >= (len - 4)) {
            iter->err_off = o;
            goto mark_invalid;
         }

         iter->d2 = o + 4;
         iter->d3 = o + 5;

         memcpy (&l, iter->raw + iter->d1, sizeof (l));
         l = BSON_UINT32_FROM_LE (l);

         if (l >= (len - o)) {
            iter->err_off = o;
            goto mark_invalid;
         }

         subtype = *(iter->raw + iter->d2);

         if (subtype == BSON_SUBTYPE_BINARY_DEPRECATED) {
            if (l < 4) {
               iter->err_off = o;
               goto mark_invalid;
            }
         }

         iter->next_off = o + 5 + l;
      }
      break;
   case BSON_TYPE_ARRAY:
   case BSON_TYPE_DOCUMENT:
      {
         uint32_t l;

         if (o >= (len - 4)) {
            iter->err_off = o;
            goto mark_invalid;
         }

         memcpy (&l, iter->raw + iter->d1, sizeof (l));
         l = BSON_UINT32_FROM_LE (l);

         if ((l > len) || (l > (len - o))) {
            iter->err_off = o;
            goto mark_invalid;
         }

         iter->next_off = o + l;
      }
      break;
   case BSON_TYPE_OID:
      iter->next_off = o + 12;
      break;
   case BSON_TYPE_BOOL:
      iter->next_off = o + 1;
      break;
   case BSON_TYPE_REGEX:
      {
         bool eor = false;
         bool eoo = false;

         for (; o < len; o++) {
            if (!data [o]) {
               iter->d2 = ++o;
               eor = true;
               break;
            }
         }

         if (!eor) {
            iter->err_off = iter->next_off;
            goto mark_invalid;
         }

         for (; o < len; o++) {
            if (!data [o]) {
               eoo = true;
               break;
            }
         }

         if (!eoo) {
            iter->err_off = iter->next_off;
            goto mark_invalid;
         }

         iter->next_off = o + 1;
      }
      break;
   case BSON_TYPE_DBPOINTER:
      {
         uint32_t l;

         if (o >= (len - 4)) {
            iter->err_off = o;
            goto mark_invalid;
         }

         iter->d2 = o + 4;
         memcpy (&l, iter->raw + iter->d1, sizeof (l));
         l = BSON_UINT32_FROM_LE (l);

         if ((l > len) || (l > (len - o))) {
            iter->err_off = o;
            goto mark_invalid;
         }

         iter->d3 = o + 4 + l;
         iter->next_off = o + 4 + l + 12;
      }
      break;
   case BSON_TYPE_CODEWSCOPE:
      {
         uint32_t l;
         uint32_t doclen;

         if ((len < 19) || (o >= (len - 14))) {
            iter->err_off = o;
            goto mark_invalid;
         }

         iter->d2 = o + 4;
         iter->d3 = o + 8;

         memcpy (&l, iter->raw + iter->d1, sizeof (l));
         l = BSON_UINT32_FROM_LE (l);

         if ((l < 14) || (l >= (len - o))) {
            iter->err_off = o;
            goto mark_invalid;
         }

         iter->next_off = o + l;

         if (iter->next_off >= len) {
            iter->err_off = o;
            goto mark_invalid;
         }

         memcpy (&l, iter->raw + iter->d2, sizeof (l));
         l = BSON_UINT32_FROM_LE (l);

         if (l >= (len - o - 4 - 4)) {
            iter->err_off = o;
            goto mark_invalid;
         }

         if ((o + 4 + 4 + l + 4) >= iter->next_off) {
            iter->err_off = o + 4;
            goto mark_invalid;
         }

         iter->d4 = o + 4 + 4 + l;
         memcpy (&doclen, iter->raw + iter->d4, sizeof (doclen));
         doclen = BSON_UINT32_FROM_LE (doclen);

         if ((o + 4 + 4 + l + doclen) != iter->next_off) {
            iter->err_off = o + 4 + 4 + l;
            goto mark_invalid;
         }
      }
      break;
   case BSON_TYPE_INT32:
      iter->next_off = o + 4;
      break;
   case BSON_TYPE_MAXKEY:
   case BSON_TYPE_MINKEY:
   case BSON_TYPE_NULL:
   case BSON_TYPE_UNDEFINED:
      iter->d1 = -1;
      iter->next_off = o;
      break;
   case BSON_TYPE_EOD:
   default:
      iter->err_off = o;
      goto mark_invalid;
   }

   /*
    * Check to see if any of the field locations would overflow the
    * current BSON buffer. If so, set the error location to the offset
    * of where the field starts.
    */
   if (iter->next_off >= len) {
      iter->err_off = o;
      goto mark_invalid;
   }

   iter->err_off = 0;

   return true;

mark_invalid:
   iter->raw = NULL;
   iter->len = 0;
   iter->next_off = 0;

   return false;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_binary --
 *
 *       Retrieves the BSON_TYPE_BINARY field. The subtype is stored in
 *       @subtype.  The length of @binary in bytes is stored in @binary_len.
 *
 *       @binary should not be modified or freed and is only valid while
 *       @iter is on the current field.
 *
 * Parameters:
 *       @iter: A bson_iter_t
 *       @subtype: A location for the binary subtype.
 *       @binary_len: A location for the length of @binary.
 *       @binary: A location for a pointer to the binary data.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_iter_binary (const bson_iter_t  *iter,        /* IN */
                  bson_subtype_t     *subtype,     /* OUT */
                  uint32_t           *binary_len,  /* OUT */
                  const uint8_t     **binary)      /* OUT */
{
   bson_subtype_t backup;

   BSON_ASSERT (iter);
   BSON_ASSERT (!binary || binary_len);

   if (ITER_TYPE (iter) == BSON_TYPE_BINARY) {
      if (!subtype) {
         subtype = &backup;
      }

      *subtype = (bson_subtype_t) *(iter->raw + iter->d2);

      if (binary) {
         memcpy (binary_len, (iter->raw + iter->d1), sizeof (*binary_len));
         *binary_len = BSON_UINT32_FROM_LE (*binary_len);
         *binary = iter->raw + iter->d3;

         if (*subtype == BSON_SUBTYPE_BINARY_DEPRECATED) {
            *binary_len -= sizeof (int32_t);
            *binary += sizeof (int32_t);
         }
      }

      return;
   }

   if (binary) {
      *binary = NULL;
   }

   if (binary_len) {
      *binary_len = 0;
   }

   if (subtype) {
      *subtype = BSON_SUBTYPE_BINARY;
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_bool --
 *
 *       Retrieves the current field of type BSON_TYPE_BOOL.
 *
 * Returns:
 *       true or false, dependent on bson document.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_bool (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_BOOL) {
      return bson_iter_bool_unsafe (iter);
   }

   return false;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_as_bool --
 *
 *       If @iter is on a boolean field, returns the boolean. If it is on a
 *       non-boolean field such as int32, int64, or double, it will convert
 *       the value to a boolean.
 *
 *       Zero is false, and non-zero is true.
 *
 * Returns:
 *       true or false, dependent on field type.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_as_bool (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   switch ((int)ITER_TYPE (iter)) {
   case BSON_TYPE_BOOL:
      return bson_iter_bool (iter);
   case BSON_TYPE_DOUBLE:
      return !(bson_iter_double (iter) == 0.0);
   case BSON_TYPE_INT64:
      return !(bson_iter_int64 (iter) == 0);
   case BSON_TYPE_INT32:
      return !(bson_iter_int32 (iter) == 0);
   case BSON_TYPE_UTF8:
      return true;
   case BSON_TYPE_NULL:
   case BSON_TYPE_UNDEFINED:
      return false;
   default:
      return true;
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_double --
 *
 *       Retrieves the current field of type BSON_TYPE_DOUBLE.
 *
 * Returns:
 *       A double.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

double
bson_iter_double (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_DOUBLE) {
      return bson_iter_double_unsafe (iter);
   }

   return 0;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_int32 --
 *
 *       Retrieves the value of the field of type BSON_TYPE_INT32.
 *
 * Returns:
 *       A 32-bit signed integer.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

int32_t
bson_iter_int32 (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_INT32) {
      return bson_iter_int32_unsafe (iter);
   }

   return 0;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_int64 --
 *
 *       Retrieves a 64-bit signed integer for the current BSON_TYPE_INT64
 *       field.
 *
 * Returns:
 *       A 64-bit signed integer.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

int64_t
bson_iter_int64 (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_INT64) {
      return bson_iter_int64_unsafe (iter);
   }

   return 0;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_as_int64 --
 *
 *       If @iter is not an int64 field, it will try to convert the value to
 *       an int64. Such field types include:
 *
 *        - bool
 *        - double
 *        - int32
 *
 * Returns:
 *       An int64_t.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

int64_t
bson_iter_as_int64 (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   switch ((int)ITER_TYPE (iter)) {
   case BSON_TYPE_BOOL:
      return (int64_t)bson_iter_bool (iter);
   case BSON_TYPE_DOUBLE:
      return (int64_t)bson_iter_double (iter);
   case BSON_TYPE_INT64:
      return bson_iter_int64 (iter);
   case BSON_TYPE_INT32:
      return (int64_t)bson_iter_int32 (iter);
   default:
      return 0;
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_oid --
 *
 *       Retrieves the current field of type %BSON_TYPE_OID. The result is
 *       valid while @iter is valid.
 *
 * Returns:
 *       A bson_oid_t that should not be modified or freed.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

const bson_oid_t *
bson_iter_oid (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_OID) {
      return bson_iter_oid_unsafe (iter);
   }

   return NULL;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_regex --
 *
 *       Fetches the current field from the iter which should be of type
 *       BSON_TYPE_REGEX.
 *
 * Returns:
 *       Regex from @iter. This should not be modified or freed.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

const char *
bson_iter_regex (const bson_iter_t *iter,    /* IN */
                 const char       **options) /* IN */
{
   const char *ret = NULL;
   const char *ret_options = NULL;

   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_REGEX) {
      ret = (const char *)(iter->raw + iter->d1);
      ret_options = (const char *)(iter->raw + iter->d2);
   }

   if (options) {
      *options = ret_options;
   }

   return ret;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_utf8 --
 *
 *       Retrieves the current field of type %BSON_TYPE_UTF8 as a UTF-8
 *       encoded string.
 *
 * Parameters:
 *       @iter: A bson_iter_t.
 *       @length: A location for the length of the string.
 *
 * Returns:
 *       A string that should not be modified or freed.
 *
 * Side effects:
 *       @length will be set to the result strings length if non-NULL.
 *
 *--------------------------------------------------------------------------
 */

const char *
bson_iter_utf8 (const bson_iter_t *iter,   /* IN */
                uint32_t          *length) /* OUT */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_UTF8) {
      if (length) {
         *length = bson_iter_utf8_len_unsafe (iter);
      }

      return (const char *)(iter->raw + iter->d2);
   }

   if (length) {
      *length = 0;
   }

   return NULL;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_dup_utf8 --
 *
 *       Copies the current UTF-8 element into a newly allocated string. The
 *       string should be freed using bson_free() when the caller is
 *       finished with it.
 *
 * Returns:
 *       A newly allocated char* that should be freed with bson_free().
 *
 * Side effects:
 *       @length will be set to the result strings length if non-NULL.
 *
 *--------------------------------------------------------------------------
 */

char *
bson_iter_dup_utf8 (const bson_iter_t *iter,   /* IN */
                    uint32_t          *length) /* OUT */
{
   uint32_t local_length = 0;
   const char *str;
   char *ret = NULL;

   BSON_ASSERT (iter);

   if ((str = bson_iter_utf8 (iter, &local_length))) {
      ret = bson_malloc0 (local_length + 1);
      memcpy (ret, str, local_length);
      ret[local_length] = '\0';
   }

   if (length) {
      *length = local_length;
   }

   return ret;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_code --
 *
 *       Retrieves the current field of type %BSON_TYPE_CODE. The length of
 *       the resulting string is stored in @length.
 *
 * Parameters:
 *       @iter: A bson_iter_t.
 *       @length: A location for the code length.
 *
 * Returns:
 *       A NUL-terminated string containing the code which should not be
 *       modified or freed.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

const char *
bson_iter_code (const bson_iter_t *iter,   /* IN */
                uint32_t          *length) /* OUT */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_CODE) {
      if (length) {
         *length = bson_iter_utf8_len_unsafe (iter);
      }

      return (const char *)(iter->raw + iter->d2);
   }

   if (length) {
      *length = 0;
   }

   return NULL;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_codewscope --
 *
 *       Similar to bson_iter_code() but with a scope associated encoded as
 *       a BSON document. @scope should not be modified or freed. It is
 *       valid while @iter is valid.
 *
 * Parameters:
 *       @iter: A #bson_iter_t.
 *       @length: A location for the length of resulting string.
 *       @scope_len: A location for the length of @scope.
 *       @scope: A location for the scope encoded as BSON.
 *
 * Returns:
 *       A NUL-terminated string that should not be modified or freed.
 *
 * Side effects:
 *       @length is set to the resulting string length in bytes.
 *       @scope_len is set to the length of @scope in bytes.
 *       @scope is set to the scope documents buffer which can be
 *       turned into a bson document with bson_init_static().
 *
 *--------------------------------------------------------------------------
 */

const char *
bson_iter_codewscope (const bson_iter_t  *iter,      /* IN */
                      uint32_t           *length,    /* OUT */
                      uint32_t           *scope_len, /* OUT */
                      const uint8_t     **scope)     /* OUT */
{
   uint32_t len;

   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_CODEWSCOPE) {
      if (length) {
         memcpy (&len, iter->raw + iter->d2, sizeof (len));
         *length = BSON_UINT32_FROM_LE (len) - 1;
      }

      memcpy (&len, iter->raw + iter->d4, sizeof (len));
      *scope_len = BSON_UINT32_FROM_LE (len);
      *scope = iter->raw + iter->d4;
      return (const char *)(iter->raw + iter->d3);
   }

   if (length) {
      *length = 0;
   }

   if (scope_len) {
      *scope_len = 0;
   }

   if (scope) {
      *scope = NULL;
   }

   return NULL;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_dbpointer --
 *
 *       Retrieves a BSON_TYPE_DBPOINTER field. @collection_len will be set
 *       to the length of the collection name. The collection name will be
 *       placed into @collection. The oid will be placed into @oid.
 *
 *       @collection and @oid should not be modified.
 *
 * Parameters:
 *       @iter: A #bson_iter_t.
 *       @collection_len: A location for the length of @collection.
 *       @collection: A location for the collection name.
 *       @oid: A location for the oid.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @collection_len is set to the length of @collection in bytes
 *       excluding the null byte.
 *       @collection is set to the collection name, including a terminating
 *       null byte.
 *       @oid is initialized with the oid.
 *
 *--------------------------------------------------------------------------
 */

void
bson_iter_dbpointer (const bson_iter_t  *iter,           /* IN */
                     uint32_t           *collection_len, /* OUT */
                     const char        **collection,     /* OUT */
                     const bson_oid_t  **oid)            /* OUT */
{
   BSON_ASSERT (iter);

   if (collection) {
      *collection = NULL;
   }

   if (oid) {
      *oid = NULL;
   }

   if (ITER_TYPE (iter) == BSON_TYPE_DBPOINTER) {
      if (collection_len) {
         memcpy (collection_len, (iter->raw + iter->d1), sizeof (*collection_len));
         *collection_len = BSON_UINT32_FROM_LE (*collection_len);

         if ((*collection_len) > 0) {
            (*collection_len)--;
         }
      }

      if (collection) {
         *collection = (const char *)(iter->raw + iter->d2);
      }

      if (oid) {
         *oid = (const bson_oid_t *)(iter->raw + iter->d3);
      }
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_symbol --
 *
 *       Retrieves the symbol of the current field of type BSON_TYPE_SYMBOL.
 *
 * Parameters:
 *       @iter: A bson_iter_t.
 *       @length: A location for the length of the symbol.
 *
 * Returns:
 *       A string containing the symbol as UTF-8. The value should not be
 *       modified or freed.
 *
 * Side effects:
 *       @length is set to the resulting strings length in bytes,
 *       excluding the null byte.
 *
 *--------------------------------------------------------------------------
 */

const char *
bson_iter_symbol (const bson_iter_t *iter,   /* IN */
                  uint32_t          *length) /* OUT */
{
   const char *ret = NULL;
   uint32_t ret_length = 0;

   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_SYMBOL) {
      ret = (const char *)(iter->raw + iter->d2);
      ret_length = bson_iter_utf8_len_unsafe (iter);
   }

   if (length) {
      *length = ret_length;
   }

   return ret;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_date_time --
 *
 *       Fetches the number of milliseconds elapsed since the UNIX epoch.
 *       This value can be negative as times before 1970 are valid.
 *
 * Returns:
 *       A signed 64-bit integer containing the number of milliseconds.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

int64_t
bson_iter_date_time (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_DATE_TIME) {
      return bson_iter_int64_unsafe (iter);
   }

   return 0;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_time_t --
 *
 *       Retrieves the current field of type BSON_TYPE_DATE_TIME as a
 *       time_t.
 *
 * Returns:
 *       A #time_t of the number of seconds since UNIX epoch in UTC.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

time_t
bson_iter_time_t (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_DATE_TIME) {
      return bson_iter_time_t_unsafe (iter);
   }

   return 0;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_timestamp --
 *
 *       Fetches the current field if it is a BSON_TYPE_TIMESTAMP.
 *
 * Parameters:
 *       @iter: A #bson_iter_t.
 *       @timestamp: a location for the timestamp.
 *       @increment: A location for the increment.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @timestamp is initialized.
 *       @increment is initialized.
 *
 *--------------------------------------------------------------------------
 */

void
bson_iter_timestamp (const bson_iter_t *iter,      /* IN */
                     uint32_t          *timestamp, /* OUT */
                     uint32_t          *increment) /* OUT */
{
   uint64_t encoded;
   uint32_t ret_timestamp = 0;
   uint32_t ret_increment = 0;

   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_TIMESTAMP) {
      memcpy (&encoded, iter->raw + iter->d1, sizeof (encoded));
      encoded = BSON_UINT64_FROM_LE (encoded);
      ret_timestamp = (encoded >> 32) & 0xFFFFFFFF;
      ret_increment = encoded & 0xFFFFFFFF;
   }

   if (timestamp) {
      *timestamp = ret_timestamp;
   }

   if (increment) {
      *increment = ret_increment;
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_timeval --
 *
 *       Retrieves the current field of type BSON_TYPE_DATE_TIME and stores
 *       it into the struct timeval provided. tv->tv_sec is set to the
 *       number of seconds since the UNIX epoch in UTC.
 *
 *       Since BSON_TYPE_DATE_TIME does not support fractions of a second,
 *       tv->tv_usec will always be set to zero.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @tv is initialized.
 *
 *--------------------------------------------------------------------------
 */

void
bson_iter_timeval (const bson_iter_t *iter,  /* IN */
                   struct timeval    *tv)    /* OUT */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_DATE_TIME) {
      bson_iter_timeval_unsafe (iter, tv);
      return;
   }

   memset (tv, 0, sizeof *tv);
}


/**
 * bson_iter_document:
 * @iter: a bson_iter_t.
 * @document_len: A location for the document length.
 * @document: A location for a pointer to the document buffer.
 *
 */
/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_document --
 *
 *       Retrieves the data to the document BSON structure and stores the
 *       length of the document buffer in @document_len and the document
 *       buffer in @document.
 *
 *       If you would like to iterate over the child contents, you might
 *       consider creating a bson_t on the stack such as the following. It
 *       allows you to call functions taking a const bson_t* only.
 *
 *          bson_t b;
 *          uint32_t len;
 *          const uint8_t *data;
 *
 *          bson_iter_document(iter, &len, &data);
 *
 *          if (bson_init_static (&b, data, len)) {
 *             ...
 *          }
 *
 *       There is no need to cleanup the bson_t structure as no data can be
 *       modified in the process of its use (as it is static/const).
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @document_len is initialized.
 *       @document is initialized.
 *
 *--------------------------------------------------------------------------
 */

void
bson_iter_document (const bson_iter_t  *iter,         /* IN */
                    uint32_t           *document_len, /* OUT */
                    const uint8_t     **document)     /* OUT */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (document_len);
   BSON_ASSERT (document);

   *document = NULL;
   *document_len = 0;

   if (ITER_TYPE (iter) == BSON_TYPE_DOCUMENT) {
      memcpy (document_len, (iter->raw + iter->d1), sizeof (*document_len));
      *document_len = BSON_UINT32_FROM_LE (*document_len);
      *document = (iter->raw + iter->d1);
   }
}


/**
 * bson_iter_array:
 * @iter: a #bson_iter_t.
 * @array_len: A location for the array length.
 * @array: A location for a pointer to the array buffer.
 */
/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_array --
 *
 *       Retrieves the data to the array BSON structure and stores the
 *       length of the array buffer in @array_len and the array buffer in
 *       @array.
 *
 *       If you would like to iterate over the child contents, you might
 *       consider creating a bson_t on the stack such as the following. It
 *       allows you to call functions taking a const bson_t* only.
 *
 *          bson_t b;
 *          uint32_t len;
 *          const uint8_t *data;
 *
 *          bson_iter_array (iter, &len, &data);
 *
 *          if (bson_init_static (&b, data, len)) {
 *             ...
 *          }
 *
 *       There is no need to cleanup the #bson_t structure as no data can be
 *       modified in the process of its use.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @array_len is initialized.
 *       @array is initialized.
 *
 *--------------------------------------------------------------------------
 */

void
bson_iter_array (const bson_iter_t  *iter,      /* IN */
                 uint32_t           *array_len, /* OUT */
                 const uint8_t     **array)     /* OUT */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (array_len);
   BSON_ASSERT (array);

   *array = NULL;
   *array_len = 0;

   if (ITER_TYPE (iter) == BSON_TYPE_ARRAY) {
      memcpy (array_len, (iter->raw + iter->d1), sizeof (*array_len));
      *array_len = BSON_UINT32_FROM_LE (*array_len);
      *array = (iter->raw + iter->d1);
   }
}


#define VISIT_FIELD(name) visitor->visit_##name && visitor->visit_##name
#define VISIT_AFTER VISIT_FIELD (after)
#define VISIT_BEFORE VISIT_FIELD (before)
#define VISIT_CORRUPT if (visitor->visit_corrupt) visitor->visit_corrupt
#define VISIT_DOUBLE VISIT_FIELD (double)
#define VISIT_UTF8 VISIT_FIELD (utf8)
#define VISIT_DOCUMENT VISIT_FIELD (document)
#define VISIT_ARRAY VISIT_FIELD (array)
#define VISIT_BINARY VISIT_FIELD (binary)
#define VISIT_UNDEFINED VISIT_FIELD (undefined)
#define VISIT_OID VISIT_FIELD (oid)
#define VISIT_BOOL VISIT_FIELD (bool)
#define VISIT_DATE_TIME VISIT_FIELD (date_time)
#define VISIT_NULL VISIT_FIELD (null)
#define VISIT_REGEX VISIT_FIELD (regex)
#define VISIT_DBPOINTER VISIT_FIELD (dbpointer)
#define VISIT_CODE VISIT_FIELD (code)
#define VISIT_SYMBOL VISIT_FIELD (symbol)
#define VISIT_CODEWSCOPE VISIT_FIELD (codewscope)
#define VISIT_INT32 VISIT_FIELD (int32)
#define VISIT_TIMESTAMP VISIT_FIELD (timestamp)
#define VISIT_INT64 VISIT_FIELD (int64)
#define VISIT_MAXKEY VISIT_FIELD (maxkey)
#define VISIT_MINKEY VISIT_FIELD (minkey)


/**
 * bson_iter_visit_all:
 * @iter: A #bson_iter_t.
 * @visitor: A #bson_visitor_t containing the visitors.
 * @data: User data for @visitor data parameters.
 *
 *
 * Returns: true if the visitor was pre-maturely ended; otherwise false.
 */
/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_visit_all --
 *
 *       Visits all fields forward from the current position of @iter. For
 *       each field found a function in @visitor will be called. Typically
 *       you will use this immediately after initializing a bson_iter_t.
 *
 *          bson_iter_init (&iter, b);
 *          bson_iter_visit_all (&iter, my_visitor, NULL);
 *
 *       @iter will no longer be valid after this function has executed and
 *       will need to be reinitialized if intending to reuse.
 *
 * Returns:
 *       true if successfully visited all fields or callback requested
 *       early termination, otherwise false.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_iter_visit_all (bson_iter_t          *iter,    /* INOUT */
                     const bson_visitor_t *visitor, /* IN */
                     void                 *data)    /* IN */
{
   const char *key;

   BSON_ASSERT (iter);
   BSON_ASSERT (visitor);

   while (bson_iter_next (iter)) {
      key = bson_iter_key_unsafe (iter);

      if (*key && !bson_utf8_validate (key, strlen (key), false)) {
         iter->err_off = iter->off;
         return true;
      }

      if (VISIT_BEFORE (iter, key, data)) {
         return true;
      }

      switch (bson_iter_type (iter)) {
      case BSON_TYPE_DOUBLE:

         if (VISIT_DOUBLE (iter, key, bson_iter_double (iter), data)) {
            return true;
         }

         break;
      case BSON_TYPE_UTF8:
         {
            uint32_t utf8_len;
            const char *utf8;

            utf8 = bson_iter_utf8 (iter, &utf8_len);

            if (!bson_utf8_validate (utf8, utf8_len, true)) {
               iter->err_off = iter->off;
               return true;
            }

            if (VISIT_UTF8 (iter, key, utf8_len, utf8, data)) {
               return true;
            }
         }
         break;
      case BSON_TYPE_DOCUMENT:
         {
            const uint8_t *docbuf = NULL;
            uint32_t doclen = 0;
            bson_t b;

            bson_iter_document (iter, &doclen, &docbuf);

            if (bson_init_static (&b, docbuf, doclen) &&
                VISIT_DOCUMENT (iter, key, &b, data)) {
               return true;
            }
         }
         break;
      case BSON_TYPE_ARRAY:
         {
            const uint8_t *docbuf = NULL;
            uint32_t doclen = 0;
            bson_t b;

            bson_iter_array (iter, &doclen, &docbuf);

            if (bson_init_static (&b, docbuf, doclen)
                && VISIT_ARRAY (iter, key, &b, data)) {
               return true;
            }
         }
         break;
      case BSON_TYPE_BINARY:
         {
            const uint8_t *binary = NULL;
            bson_subtype_t subtype = BSON_SUBTYPE_BINARY;
            uint32_t binary_len = 0;

            bson_iter_binary (iter, &subtype, &binary_len, &binary);

            if (VISIT_BINARY (iter, key, subtype, binary_len, binary, data)) {
               return true;
            }
         }
         break;
      case BSON_TYPE_UNDEFINED:

         if (VISIT_UNDEFINED (iter, key, data)) {
            return true;
         }

         break;
      case BSON_TYPE_OID:

         if (VISIT_OID (iter, key, bson_iter_oid (iter), data)) {
            return true;
         }

         break;
      case BSON_TYPE_BOOL:

         if (VISIT_BOOL (iter, key, bson_iter_bool (iter), data)) {
            return true;
         }

         break;
      case BSON_TYPE_DATE_TIME:

         if (VISIT_DATE_TIME (iter, key, bson_iter_date_time (iter), data)) {
            return true;
         }

         break;
      case BSON_TYPE_NULL:

         if (VISIT_NULL (iter, key, data)) {
            return true;
         }

         break;
      case BSON_TYPE_REGEX:
         {
            const char *regex = NULL;
            const char *options = NULL;
            regex = bson_iter_regex (iter, &options);

            if (VISIT_REGEX (iter, key, regex, options, data)) {
               return true;
            }
         }
         break;
      case BSON_TYPE_DBPOINTER:
         {
            uint32_t collection_len = 0;
            const char *collection = NULL;
            const bson_oid_t *oid = NULL;

            bson_iter_dbpointer (iter, &collection_len, &collection, &oid);

            if (VISIT_DBPOINTER (iter, key, collection_len, collection, oid,
                                 data)) {
               return true;
            }
         }
         break;
      case BSON_TYPE_CODE:
         {
            uint32_t code_len;
            const char *code;

            code = bson_iter_code (iter, &code_len);

            if (VISIT_CODE (iter, key, code_len, code, data)) {
               return true;
            }
         }
         break;
      case BSON_TYPE_SYMBOL:
         {
            uint32_t symbol_len;
            const char *symbol;

            symbol = bson_iter_symbol (iter, &symbol_len);

            if (VISIT_SYMBOL (iter, key, symbol_len, symbol, data)) {
               return true;
            }
         }
         break;
      case BSON_TYPE_CODEWSCOPE:
         {
            uint32_t length = 0;
            const char *code;
            const uint8_t *docbuf = NULL;
            uint32_t doclen = 0;
            bson_t b;

            code = bson_iter_codewscope (iter, &length, &doclen, &docbuf);

            if (bson_init_static (&b, docbuf, doclen) &&
                VISIT_CODEWSCOPE (iter, key, length, code, &b, data)) {
               return true;
            }
         }
         break;
      case BSON_TYPE_INT32:

         if (VISIT_INT32 (iter, key, bson_iter_int32 (iter), data)) {
            return true;
         }

         break;
      case BSON_TYPE_TIMESTAMP:
         {
            uint32_t timestamp;
            uint32_t increment;
            bson_iter_timestamp (iter, &timestamp, &increment);

            if (VISIT_TIMESTAMP (iter, key, timestamp, increment, data)) {
               return true;
            }
         }
         break;
      case BSON_TYPE_INT64:

         if (VISIT_INT64 (iter, key, bson_iter_int64 (iter), data)) {
            return true;
         }

         break;
      case BSON_TYPE_MAXKEY:

         if (VISIT_MAXKEY (iter, bson_iter_key_unsafe (iter), data)) {
            return true;
         }

         break;
      case BSON_TYPE_MINKEY:

         if (VISIT_MINKEY (iter, bson_iter_key_unsafe (iter), data)) {
            return true;
         }

         break;
      case BSON_TYPE_EOD:
      default:
         break;
      }

      if (VISIT_AFTER (iter, bson_iter_key_unsafe (iter), data)) {
         return true;
      }
   }

   if (iter->err_off) {
      VISIT_CORRUPT (iter, data);
   }

#undef VISIT_FIELD

   return false;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_overwrite_bool --
 *
 *       Overwrites the current BSON_TYPE_BOOLEAN field with a new value.
 *       This is performed in-place and therefore no keys are moved.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_iter_overwrite_bool (bson_iter_t *iter,  /* IN */
                          bool         value) /* IN */
{
   BSON_ASSERT (iter);
   value = !!value;

   if (ITER_TYPE (iter) == BSON_TYPE_BOOL) {
      memcpy ((void *)(iter->raw + iter->d1), &value, 1);
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_overwrite_int32 --
 *
 *       Overwrites the current BSON_TYPE_INT32 field with a new value.
 *       This is performed in-place and therefore no keys are moved.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_iter_overwrite_int32 (bson_iter_t *iter,  /* IN */
                           int32_t      value) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_INT32) {
#if BSON_BYTE_ORDER != BSON_LITTLE_ENDIAN
      value = BSON_UINT32_TO_LE (value);
#endif
      memcpy ((void *)(iter->raw + iter->d1), &value, sizeof (value));
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_overwrite_int64 --
 *
 *       Overwrites the current BSON_TYPE_INT64 field with a new value.
 *       This is performed in-place and therefore no keys are moved.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_iter_overwrite_int64 (bson_iter_t *iter,   /* IN */
                           int64_t      value)  /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_INT64) {
#if BSON_BYTE_ORDER != BSON_LITTLE_ENDIAN
      value = BSON_UINT64_TO_LE (value);
#endif
      memcpy ((void *)(iter->raw + iter->d1), &value, sizeof (value));
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_overwrite_double --
 *
 *       Overwrites the current BSON_TYPE_DOUBLE field with a new value.
 *       This is performed in-place and therefore no keys are moved.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_iter_overwrite_double (bson_iter_t *iter,  /* IN */
                            double       value) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_DOUBLE) {
      value = BSON_DOUBLE_TO_LE (value);
      memcpy ((void *)(iter->raw + iter->d1), &value, sizeof (value));
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_iter_value --
 *
 *       Retrieves a bson_value_t containing the boxed value of the current
 *       element. The result of this function valid until the state of
 *       iter has been changed (through the use of bson_iter_next()).
 *
 * Returns:
 *       A bson_value_t that should not be modified or freed. If you need
 *       to hold on to the value, use bson_value_copy().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

const bson_value_t *
bson_iter_value (bson_iter_t *iter) /* IN */
{
   bson_value_t *value;

   BSON_ASSERT (iter);

   value = &iter->value;
   value->value_type = ITER_TYPE (iter);

   switch (value->value_type) {
   case BSON_TYPE_DOUBLE:
      value->value.v_double = bson_iter_double (iter);
      break;
   case BSON_TYPE_UTF8:
      value->value.v_utf8.str =
         (char *)bson_iter_utf8 (iter, &value->value.v_utf8.len);
      break;
   case BSON_TYPE_DOCUMENT:
      bson_iter_document (iter,
                          &value->value.v_doc.data_len,
                          (const uint8_t **)&value->value.v_doc.data);
      break;
   case BSON_TYPE_ARRAY:
      bson_iter_array (iter,
                       &value->value.v_doc.data_len,
                       (const uint8_t **)&value->value.v_doc.data);
      break;
   case BSON_TYPE_BINARY:
      bson_iter_binary (iter,
                        &value->value.v_binary.subtype,
                        &value->value.v_binary.data_len,
                        (const uint8_t **)&value->value.v_binary.data);
      break;
   case BSON_TYPE_OID:
      bson_oid_copy (bson_iter_oid (iter), &value->value.v_oid);
      break;
   case BSON_TYPE_BOOL:
      value->value.v_bool = bson_iter_bool (iter);
      break;
   case BSON_TYPE_DATE_TIME:
      value->value.v_datetime = bson_iter_date_time (iter);
      break;
   case BSON_TYPE_REGEX:
      value->value.v_regex.regex = (char *)bson_iter_regex (
            iter,
            (const char **)&value->value.v_regex.options);
      break;
   case BSON_TYPE_DBPOINTER: {
      const bson_oid_t *oid;

      bson_iter_dbpointer (iter,
                           &value->value.v_dbpointer.collection_len,
                           (const char **)&value->value.v_dbpointer.collection,
                           &oid);
      bson_oid_copy (oid, &value->value.v_dbpointer.oid);
      break;
   }
   case BSON_TYPE_CODE:
      value->value.v_code.code =
         (char *)bson_iter_code (
            iter,
            &value->value.v_code.code_len);
      break;
   case BSON_TYPE_SYMBOL:
      value->value.v_symbol.symbol =
         (char *)bson_iter_symbol (
            iter,
            &value->value.v_symbol.len);
      break;
   case BSON_TYPE_CODEWSCOPE:
      value->value.v_codewscope.code =
         (char *)bson_iter_codewscope (
            iter,
            &value->value.v_codewscope.code_len,
            &value->value.v_codewscope.scope_len,
            (const uint8_t **)&value->value.v_codewscope.scope_data);
      break;
   case BSON_TYPE_INT32:
      value->value.v_int32 = bson_iter_int32 (iter);
      break;
   case BSON_TYPE_TIMESTAMP:
      bson_iter_timestamp (iter,
                           &value->value.v_timestamp.timestamp,
                           &value->value.v_timestamp.increment);
      break;
   case BSON_TYPE_INT64:
      value->value.v_int64 = bson_iter_int64 (iter);
      break;
   case BSON_TYPE_NULL:
   case BSON_TYPE_UNDEFINED:
   case BSON_TYPE_MAXKEY:
   case BSON_TYPE_MINKEY:
      break;
   case BSON_TYPE_EOD:
   default:
      return NULL;
   }

   return value;
}
/*
  Copyright (C) 1999, 2000, 2002 Aladdin Enterprises.  All rights reserved.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  L. Peter Deutsch
  ghost@aladdin.com

 */
/* $Id: md5.c,v 1.6 2002/04/13 19:20:28 lpd Exp $ */
/*
  Independent implementation of MD5 (RFC 1321).

  This code implements the MD5 Algorithm defined in RFC 1321, whose
  text is available at
    http://www.ietf.org/rfc/rfc1321.txt
  The code is derived from the text of the RFC, including the test suite
  (section A.5) but excluding the rest of Appendix A.  It does not include
  any code or documentation that is identified in the RFC as being
  copyrighted.

  The original and principal author of md5.c is L. Peter Deutsch
  <ghost@aladdin.com>.  Other authors are noted in the change history
  that follows (in reverse chronological order):

  2002-04-13 lpd Clarified derivation from RFC 1321; now handles byte order
    either statically or dynamically; added missing #include <string.h>
    in library.
  2002-03-11 lpd Corrected argument list for main(), and added int return
    type, in test program and T value program.
  2002-02-21 lpd Added missing #include <stdio.h> in test program.
  2000-07-03 lpd Patched to eliminate warnings about "constant is
    unsigned in ANSI C, signed in traditional"; made test program
    self-checking.
  1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
  1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5).
  1999-05-03 lpd Original version.
 */

/*
 * The following MD5 implementation has been modified to use types as
 * specified in libbson.
 */

// #include "bson-compat.h"


#include <string.h>

// #include "bson-md5.h"



#undef BYTE_ORDER   /* 1 = big-endian, -1 = little-endian, 0 = unknown */
#if BSON_BYTE_ORDER == BSON_BIG_ENDIAN
#  define BYTE_ORDER 1
#else
#  define BYTE_ORDER -1
#endif

#define T_MASK ((uint32_t)~0)
#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87)
#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9)
#define T3    0x242070db
#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111)
#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050)
#define T6    0x4787c62a
#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ec)
#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afe)
#define T9    0x698098d8
#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850)
#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44e)
#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841)
#define T13    0x6b901122
#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6c)
#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71)
#define T16    0x49b40821
#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9d)
#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbf)
#define T19    0x265e5a51
#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855)
#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2)
#define T22    0x02441453
#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197e)
#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437)
#define T25    0x21e1cde6
#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829)
#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278)
#define T28    0x455a14ed
#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16fa)
#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07)
#define T31    0x676f02d9
#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375)
#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bd)
#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097e)
#define T35    0x6d9d6122
#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3)
#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bb)
#define T38    0x4bdecfa9
#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49f)
#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438f)
#define T41    0x289b7ec6
#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805)
#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7a)
#define T44    0x04881d05
#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6)
#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661a)
#define T47    0x1fa27cf8
#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99a)
#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbb)
#define T50    0x432aff97
#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58)
#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6)
#define T53    0x655b59c3
#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336d)
#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82)
#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22e)
#define T57    0x6fa87e4f
#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191f)
#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebceb)
#define T60    0x4e0811a1
#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817d)
#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dca)
#define T63    0x2ad7d2bb
#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6e)


static void
bson_md5_process (bson_md5_t     *md5,
                  const uint8_t *data)
{
   uint32_t a = md5->abcd[0];
   uint32_t b = md5->abcd[1];
   uint32_t c = md5->abcd[2];
   uint32_t d = md5->abcd[3];
   uint32_t t;

#if BYTE_ORDER > 0
    /* Define storage only for big-endian CPUs. */
    uint32_t X[16];
#else
    /* Define storage for little-endian or both types of CPUs. */
    uint32_t xbuf[16];
    const uint32_t *X;
#endif

    {
#if BYTE_ORDER == 0
        /*
         * Determine dynamically whether this is a big-endian or
         * little-endian machine, since we can use a more efficient
         * algorithm on the latter.
         */
        static const int w = 1;

        if (*((const uint8_t *)&w)) /* dynamic little-endian */
#endif
#if BYTE_ORDER <= 0     /* little-endian */
        {
            /*
             * On little-endian machines, we can process properly aligned
             * data without copying it.
             */
            if (!((data - (const uint8_t *)0) & 3)) {
                /* data are properly aligned */
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
#endif
                X = (const uint32_t *)data;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
            }
            else {
                /* not aligned */
                memcpy(xbuf, data, sizeof (xbuf));
                X = xbuf;
            }
        }
#endif
#if BYTE_ORDER == 0
        else            /* dynamic big-endian */
#endif
#if BYTE_ORDER >= 0     /* big-endian */
        {
            /*
             * On big-endian machines, we must arrange the bytes in the
             * right order.
             */
            const uint8_t *xp = data;
            int i;

#  if BYTE_ORDER == 0
            X = xbuf;       /* (dynamic only) */
#  else
#    define xbuf X      /* (static only) */
#  endif
            for (i = 0; i < 16; ++i, xp += 4)
                xbuf[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);
        }
#endif
    }

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

    /* Round 1. */
    /* Let [abcd k s i] denote the operation
       a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)\
    t = a + F(b,c,d) + X[k] + Ti;\
    a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  7,  T1);
    SET(d, a, b, c,  1, 12,  T2);
    SET(c, d, a, b,  2, 17,  T3);
    SET(b, c, d, a,  3, 22,  T4);
    SET(a, b, c, d,  4,  7,  T5);
    SET(d, a, b, c,  5, 12,  T6);
    SET(c, d, a, b,  6, 17,  T7);
    SET(b, c, d, a,  7, 22,  T8);
    SET(a, b, c, d,  8,  7,  T9);
    SET(d, a, b, c,  9, 12, T10);
    SET(c, d, a, b, 10, 17, T11);
    SET(b, c, d, a, 11, 22, T12);
    SET(a, b, c, d, 12,  7, T13);
    SET(d, a, b, c, 13, 12, T14);
    SET(c, d, a, b, 14, 17, T15);
    SET(b, c, d, a, 15, 22, T16);
#undef SET

    /* Round 2. */
    /* Let [abcd k s i] denote the operation
         a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
    t = a + G(b,c,d) + X[k] + Ti;\
    a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  1,  5, T17);
    SET(d, a, b, c,  6,  9, T18);
    SET(c, d, a, b, 11, 14, T19);
    SET(b, c, d, a,  0, 20, T20);
    SET(a, b, c, d,  5,  5, T21);
    SET(d, a, b, c, 10,  9, T22);
    SET(c, d, a, b, 15, 14, T23);
    SET(b, c, d, a,  4, 20, T24);
    SET(a, b, c, d,  9,  5, T25);
    SET(d, a, b, c, 14,  9, T26);
    SET(c, d, a, b,  3, 14, T27);
    SET(b, c, d, a,  8, 20, T28);
    SET(a, b, c, d, 13,  5, T29);
    SET(d, a, b, c,  2,  9, T30);
    SET(c, d, a, b,  7, 14, T31);
    SET(b, c, d, a, 12, 20, T32);
#undef SET

    /* Round 3. */
    /* Let [abcd k s t] denote the operation
         a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)\
    t = a + H(b,c,d) + X[k] + Ti;\
    a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  5,  4, T33);
    SET(d, a, b, c,  8, 11, T34);
    SET(c, d, a, b, 11, 16, T35);
    SET(b, c, d, a, 14, 23, T36);
    SET(a, b, c, d,  1,  4, T37);
    SET(d, a, b, c,  4, 11, T38);
    SET(c, d, a, b,  7, 16, T39);
    SET(b, c, d, a, 10, 23, T40);
    SET(a, b, c, d, 13,  4, T41);
    SET(d, a, b, c,  0, 11, T42);
    SET(c, d, a, b,  3, 16, T43);
    SET(b, c, d, a,  6, 23, T44);
    SET(a, b, c, d,  9,  4, T45);
    SET(d, a, b, c, 12, 11, T46);
    SET(c, d, a, b, 15, 16, T47);
    SET(b, c, d, a,  2, 23, T48);
#undef SET

    /* Round 4. */
    /* Let [abcd k s t] denote the operation
         a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
    t = a + I(b,c,d) + X[k] + Ti;\
    a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  6, T49);
    SET(d, a, b, c,  7, 10, T50);
    SET(c, d, a, b, 14, 15, T51);
    SET(b, c, d, a,  5, 21, T52);
    SET(a, b, c, d, 12,  6, T53);
    SET(d, a, b, c,  3, 10, T54);
    SET(c, d, a, b, 10, 15, T55);
    SET(b, c, d, a,  1, 21, T56);
    SET(a, b, c, d,  8,  6, T57);
    SET(d, a, b, c, 15, 10, T58);
    SET(c, d, a, b,  6, 15, T59);
    SET(b, c, d, a, 13, 21, T60);
    SET(a, b, c, d,  4,  6, T61);
    SET(d, a, b, c, 11, 10, T62);
    SET(c, d, a, b,  2, 15, T63);
    SET(b, c, d, a,  9, 21, T64);
#undef SET

    /* Then perform the following additions. (That is increment each
       of the four registers by the value it had before this block
       was started.) */
    md5->abcd[0] += a;
    md5->abcd[1] += b;
    md5->abcd[2] += c;
    md5->abcd[3] += d;
}

void
bson_md5_init (bson_md5_t *pms)
{
    pms->count[0] = pms->count[1] = 0;
    pms->abcd[0] = 0x67452301;
    pms->abcd[1] = /*0xefcdab89*/ T_MASK ^ 0x10325476;
    pms->abcd[2] = /*0x98badcfe*/ T_MASK ^ 0x67452301;
    pms->abcd[3] = 0x10325476;
}

void
bson_md5_append (bson_md5_t         *pms,
                 const uint8_t *data,
                 uint32_t       nbytes)
{
    const uint8_t *p = data;
    int left = nbytes;
    int offset = (pms->count[0] >> 3) & 63;
    uint32_t nbits = (uint32_t)(nbytes << 3);

    if (nbytes <= 0)
        return;

    /* Update the message length. */
    pms->count[1] += nbytes >> 29;
    pms->count[0] += nbits;
    if (pms->count[0] < nbits)
        pms->count[1]++;

    /* Process an initial partial block. */
    if (offset) {
        int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

        memcpy(pms->buf + offset, p, copy);
        if (offset + copy < 64)
            return;
        p += copy;
        left -= copy;
        bson_md5_process(pms, pms->buf);
    }

    /* Process full blocks. */
    for (; left >= 64; p += 64, left -= 64)
        bson_md5_process(pms, p);

    /* Process a final partial block. */
    if (left)
        memcpy(pms->buf, p, left);
}

void
bson_md5_finish (bson_md5_t   *pms,
                 uint8_t  digest[16])
{
    static const uint8_t pad[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    uint8_t data[8];
    int i;

    /* Save the length before padding. */
    for (i = 0; i < 8; ++i)
        data[i] = (uint8_t)(pms->count[i >> 2] >> ((i & 3) << 3));
    /* Pad to 56 bytes mod 64. */
    bson_md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
    /* Append the length. */
    bson_md5_append(pms, data, sizeof (data));
    for (i = 0; i < 16; ++i)
        digest[i] = (uint8_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
}
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #include "bson.h"


#include <errno.h>
#include <fcntl.h>
#ifdef BSON_OS_WIN32
# include <io.h>
# include <share.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// #include "bson-reader.h"

// #include "bson-memory.h"



typedef enum
{
   BSON_READER_HANDLE = 1,
   BSON_READER_DATA = 2,
} bson_reader_type_t;


typedef struct
{
   bson_reader_type_t         type;
   void                      *handle;
   bool                       done   : 1;
   bool                       failed : 1;
   size_t                     end;
   size_t                     len;
   size_t                     offset;
   size_t                     bytes_read;
   bson_t                     inline_bson;
   uint8_t                   *data;
   bson_reader_read_func_t    read_func;
   bson_reader_destroy_func_t destroy_func;
} bson_reader_handle_t;


typedef struct
{
   int fd;
   bool do_close;
} bson_reader_handle_fd_t;


typedef struct
{
   bson_reader_type_t type;
   const uint8_t     *data;
   size_t             length;
   size_t             offset;
   bson_t             inline_bson;
} bson_reader_data_t;


/*
 *--------------------------------------------------------------------------
 *
 * _bson_reader_handle_fill_buffer --
 *
 *       Attempt to read as much as possible until the underlying buffer
 *       in @reader is filled or we have reached end-of-stream or
 *       read failure.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_reader_handle_fill_buffer (bson_reader_handle_t *reader) /* IN */
{
   ssize_t ret;

   /*
    * Handle first read specially.
    */
   if ((!reader->done) && (!reader->offset) && (!reader->end)) {
      ret = reader->read_func (reader->handle, &reader->data[0], reader->len);

      if (ret <= 0) {
         reader->done = true;
         return;
      }
      reader->bytes_read += ret;

      reader->end = ret;
      return;
   }

   /*
    * Move valid data to head.
    */
   memmove (&reader->data[0],
            &reader->data[reader->offset],
            reader->end - reader->offset);
   reader->end = reader->end - reader->offset;
   reader->offset = 0;

   /*
    * Read in data to fill the buffer.
    */
   ret = reader->read_func (reader->handle,
                            &reader->data[reader->end],
                            reader->len - reader->end);

   if (ret <= 0) {
      reader->done = true;
      reader->failed = (ret < 0);
   } else {
      reader->bytes_read += ret;
      reader->end += ret;
   }

   BSON_ASSERT (reader->offset == 0);
   BSON_ASSERT (reader->end <= reader->len);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_new_from_handle --
 *
 *       Allocates and initializes a new bson_reader_t using the opaque
 *       handle provided.
 *
 * Parameters:
 *       @handle: an opaque handle to use to read data.
 *       @rf: a function to perform reads on @handle.
 *       @df: a function to release @handle, or NULL.
 *
 * Returns:
 *       A newly allocated bson_reader_t if successful, otherwise NULL.
 *       Free the successful result with bson_reader_destroy().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bson_reader_t *
bson_reader_new_from_handle (void                       *handle,
                             bson_reader_read_func_t     rf,
                             bson_reader_destroy_func_t  df)
{
   bson_reader_handle_t *real;

   BSON_ASSERT (handle);
   BSON_ASSERT (rf);

   real = bson_malloc0 (sizeof *real);
   real->type = BSON_READER_HANDLE;
   real->data = bson_malloc0 (1024);
   real->handle = handle;
   real->len = 1024;
   real->offset = 0;

   bson_reader_set_read_func ((bson_reader_t *)real, rf);

   if (df) {
      bson_reader_set_destroy_func ((bson_reader_t *)real, df);
   }

   _bson_reader_handle_fill_buffer (real);

   return (bson_reader_t *)real;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_reader_handle_fd_destroy --
 *
 *       Cleanup allocations associated with state created in
 *       bson_reader_new_from_fd().
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_reader_handle_fd_destroy (void *handle) /* IN */
{
   bson_reader_handle_fd_t *fd = handle;

   if (fd) {
      if ((fd->fd != -1) && fd->do_close) {
#ifdef _WIN32
         _close (fd->fd);
#else
         close (fd->fd);
#endif
      }
      bson_free (fd);
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_reader_handle_fd_read --
 *
 *       Perform read on opaque handle created in
 *       bson_reader_new_from_fd().
 *
 *       The underlying file descriptor is read from the current position
 *       using the bson_reader_handle_fd_t allocated.
 *
 * Returns:
 *       -1 on failure.
 *       0 on end of stream.
 *       Greater than zero on success.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static ssize_t
_bson_reader_handle_fd_read (void   *handle, /* IN */
                             void   *buf,    /* IN */
                             size_t  len)    /* IN */
{
   bson_reader_handle_fd_t *fd = handle;
   ssize_t ret = -1;

   if (fd && (fd->fd != -1)) {
   again:
#ifdef BSON_OS_WIN32
      ret = _read (fd->fd, buf, (unsigned int)len);
#else
      ret = read (fd->fd, buf, len);
#endif
      if ((ret == -1) && (errno == EAGAIN)) {
         goto again;
      }
   }

   return ret;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_new_from_fd --
 *
 *       Create a new bson_reader_t using the file-descriptor provided.
 *
 * Parameters:
 *       @fd: a libc style file-descriptor.
 *       @close_on_destroy: if close() should be called on @fd when
 *          bson_reader_destroy() is called.
 *
 * Returns:
 *       A newly allocated bson_reader_t on success; otherwise NULL.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bson_reader_t *
bson_reader_new_from_fd (int  fd,               /* IN */
                         bool close_on_destroy) /* IN */
{
   bson_reader_handle_fd_t *handle;

   BSON_ASSERT (fd != -1);

   handle = bson_malloc0 (sizeof *handle);
   handle->fd = fd;
   handle->do_close = close_on_destroy;

   return bson_reader_new_from_handle (handle,
                                       _bson_reader_handle_fd_read,
                                       _bson_reader_handle_fd_destroy);
}


/**
 * bson_reader_set_read_func:
 * @reader: A bson_reader_t.
 *
 * Note that @reader must be initialized by bson_reader_init_from_handle(), or data
 * will be destroyed.
 */
/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_set_read_func --
 *
 *       Set the read func to be provided for @reader.
 *
 *       You probably want to use bson_reader_new_from_handle() or
 *       bson_reader_new_from_fd() instead.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_reader_set_read_func (bson_reader_t          *reader, /* IN */
                           bson_reader_read_func_t func)   /* IN */
{
   bson_reader_handle_t *real = (bson_reader_handle_t *)reader;

   BSON_ASSERT (reader->type == BSON_READER_HANDLE);

   real->read_func = func;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_set_destroy_func --
 *
 *       Set the function to cleanup state when @reader is destroyed.
 *
 *       You probably want bson_reader_new_from_fd() or
 *       bson_reader_new_from_handle() instead.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_reader_set_destroy_func (bson_reader_t             *reader, /* IN */
                              bson_reader_destroy_func_t func)   /* IN */
{
   bson_reader_handle_t *real = (bson_reader_handle_t *)reader;

   BSON_ASSERT (reader->type == BSON_READER_HANDLE);

   real->destroy_func = func;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_reader_handle_grow_buffer --
 *
 *       Grow the buffer to the next power of two.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static void
_bson_reader_handle_grow_buffer (bson_reader_handle_t *reader) /* IN */
{
   size_t size;

   size = reader->len * 2;
   reader->data = bson_realloc (reader->data, size);
   reader->len = size;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_reader_handle_tell --
 *
 *       Tell the current position within the underlying file-descriptor.
 *
 * Returns:
 *       An off_t containing the current offset.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static off_t
_bson_reader_handle_tell (bson_reader_handle_t *reader) /* IN */
{
   off_t off;

   off = (off_t)reader->bytes_read;
   off -= (off_t)reader->end;
   off += (off_t)reader->offset;

   return off;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_reader_handle_read --
 *
 *       Read the next chunk of data from the underlying file descriptor
 *       and return a bson_t which should not be modified.
 *
 *       There was a failure if NULL is returned and @reached_eof is
 *       not set to true.
 *
 * Returns:
 *       NULL on failure or end of stream.
 *
 * Side effects:
 *       @reached_eof is set if non-NULL.
 *
 *--------------------------------------------------------------------------
 */

static const bson_t *
_bson_reader_handle_read (bson_reader_handle_t *reader,      /* IN */
                          bool                 *reached_eof) /* IN */
{
   int32_t blen;

   if (reached_eof) {
      *reached_eof = false;
   }

   while (!reader->done) {
      if ((reader->end - reader->offset) < 4) {
         _bson_reader_handle_fill_buffer (reader);
         continue;
      }

      memcpy (&blen, &reader->data[reader->offset], sizeof blen);
      blen = BSON_UINT32_FROM_LE (blen);

      if (blen < 5) {
         return NULL;
      }

      if (blen > (int32_t)(reader->end - reader->offset)) {
         if (blen > (int32_t)reader->len) {
            _bson_reader_handle_grow_buffer (reader);
         }

         _bson_reader_handle_fill_buffer (reader);
         continue;
      }

      if (!bson_init_static (&reader->inline_bson,
                             &reader->data[reader->offset],
                             (uint32_t)blen)) {
         return NULL;
      }

      reader->offset += blen;

      return &reader->inline_bson;
   }

   if (reached_eof) {
      *reached_eof = reader->done && !reader->failed;
   }

   return NULL;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_new_from_data --
 *
 *       Allocates and initializes a new bson_reader_t that will the memory
 *       provided as a stream of BSON documents.
 *
 * Parameters:
 *       @data: A buffer to read BSON documents from.
 *       @length: The length of @data.
 *
 * Returns:
 *       A newly allocated bson_reader_t that should be freed with
 *       bson_reader_destroy().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bson_reader_t *
bson_reader_new_from_data (const uint8_t *data,   /* IN */
                           size_t         length) /* IN */
{
   bson_reader_data_t *real;

   BSON_ASSERT (data);

   real = (bson_reader_data_t*)bson_malloc0 (sizeof *real);
   real->type = BSON_READER_DATA;
   real->data = data;
   real->length = length;
   real->offset = 0;

   return (bson_reader_t *)real;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_reader_data_read --
 *
 *       Read the next document from the underlying buffer.
 *
 * Returns:
 *       NULL on failure or end of stream.
 *       a bson_t which should not be modified.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static const bson_t *
_bson_reader_data_read (bson_reader_data_t *reader,      /* IN */
                        bool               *reached_eof) /* IN */
{
   int32_t blen;

   if (reached_eof) {
      *reached_eof = false;
   }

   if ((reader->offset + 4) < reader->length) {
      memcpy (&blen, &reader->data[reader->offset], sizeof blen);
      blen = BSON_UINT32_FROM_LE (blen);

      if (blen < 5) {
         return NULL;
      }

      if (blen > (int32_t)(reader->length - reader->offset)) {
         return NULL;
      }

      if (!bson_init_static (&reader->inline_bson,
                             &reader->data[reader->offset], (uint32_t)blen)) {
         return NULL;
      }

      reader->offset += blen;

      return &reader->inline_bson;
   }

   if (reached_eof) {
      *reached_eof = (reader->offset == reader->length);
   }

   return NULL;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_reader_data_tell --
 *
 *       Tell the current position in the underlying buffer.
 *
 * Returns:
 *       An off_t of the current offset.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static off_t
_bson_reader_data_tell (bson_reader_data_t *reader) /* IN */
{
   return (off_t)reader->offset;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_destroy --
 *
 *       Release a bson_reader_t created with bson_reader_new_from_data(),
 *       bson_reader_new_from_fd(), or bson_reader_new_from_handle().
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_reader_destroy (bson_reader_t *reader) /* IN */
{
   BSON_ASSERT (reader);

   switch (reader->type) {
   case 0:
      break;
   case BSON_READER_HANDLE:
      {
         bson_reader_handle_t *handle = (bson_reader_handle_t *)reader;

         if (handle->destroy_func) {

            handle->destroy_func(handle->handle);
         }

         bson_free (handle->data);
      }
      break;
   case BSON_READER_DATA:
      break;
   default:
      fprintf (stderr, "No such reader type: %02x\n", reader->type);
      break;
   }

   reader->type = 0;

   bson_free (reader);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_read --
 *
 *       Reads the next bson_t in the underlying memory or storage.  The
 *       resulting bson_t should not be modified or freed. You may copy it
 *       and iterate over it.  Functions that take a const bson_t* are safe
 *       to use.
 *
 *       This structure does not survive calls to bson_reader_read() or
 *       bson_reader_destroy() as it uses memory allocated by the reader or
 *       underlying storage/memory.
 *
 *       If NULL is returned then @reached_eof will be set to true if the
 *       end of the file or buffer was reached. This indicates if there was
 *       an error parsing the document stream.
 *
 * Returns:
 *       A const bson_t that should not be modified or freed.
 *       NULL on failure or end of stream.
 *
 * Side effects:
 *       @reached_eof is set if non-NULL.
 *
 *--------------------------------------------------------------------------
 */

const bson_t *
bson_reader_read (bson_reader_t *reader,      /* IN */
                  bool          *reached_eof) /* OUT */
{
   BSON_ASSERT (reader);

   switch (reader->type) {
   case BSON_READER_HANDLE:
      return _bson_reader_handle_read ((bson_reader_handle_t *)reader, reached_eof);

   case BSON_READER_DATA:
      return _bson_reader_data_read ((bson_reader_data_t *)reader, reached_eof);

   default:
      fprintf (stderr, "No such reader type: %02x\n", reader->type);
      break;
   }

   return NULL;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_tell --
 *
 *       Return the current position in the underlying reader. This will
 *       always be at the beginning of a bson document or end of file.
 *
 * Returns:
 *       An off_t containing the current offset.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

off_t
bson_reader_tell (bson_reader_t *reader) /* IN */
{
   BSON_ASSERT (reader);

   switch (reader->type) {
   case BSON_READER_HANDLE:
      return _bson_reader_handle_tell ((bson_reader_handle_t *)reader);

   case BSON_READER_DATA:
      return _bson_reader_data_tell ((bson_reader_data_t *)reader);

   default:
      fprintf (stderr, "No such reader type: %02x\n", reader->type);
      return -1;
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_reader_new_from_file --
 *
 *       A convenience function to open a file containing sequential
 *       bson documents and read them using bson_reader_t.
 *
 * Returns:
 *       A new bson_reader_t if successful, otherwise NULL and
 *       @error is set. Free the non-NULL result with
 *       bson_reader_destroy().
 *
 * Side effects:
 *       @error may be set.
 *
 *--------------------------------------------------------------------------
 */

bson_reader_t *
bson_reader_new_from_file (const char   *path,  /* IN */
                           bson_error_t *error) /* OUT */
{
   char errmsg_buf[BSON_ERROR_BUFFER_SIZE];
   char *errmsg;
   int fd;

   BSON_ASSERT (path);

#ifdef BSON_OS_WIN32
   if (_sopen_s (&fd, path, (_O_RDONLY | _O_BINARY), _SH_DENYNO, 0) != 0) {
      fd = -1;
   }
#else
   fd = open (path, O_RDONLY);
#endif

   if (fd == -1) {
      errmsg = bson_strerror_r (errno, errmsg_buf, sizeof errmsg_buf);
      bson_set_error (error,
                      BSON_ERROR_READER,
                      BSON_ERROR_READER_BADFD,
                      "%s", errmsg);
      return NULL;
   }

   return bson_reader_new_from_fd (fd, true);
}
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <string.h>

// #include "bson-memory.h"

// #include "bson-string.h"

// #include "bson-utf8.h"



/*
 *--------------------------------------------------------------------------
 *
 * _bson_utf8_get_sequence --
 *
 *       Determine the sequence length of the first UTF-8 character in
 *       @utf8. The sequence length is stored in @seq_length and the mask
 *       for the first character is stored in @first_mask.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @seq_length is set.
 *       @first_mask is set.
 *
 *--------------------------------------------------------------------------
 */

static BSON_INLINE void
_bson_utf8_get_sequence (const char *utf8,       /* IN */
                         uint8_t    *seq_length, /* OUT */
                         uint8_t    *first_mask) /* OUT */
{
   unsigned char c = *(const unsigned char *)utf8;
   uint8_t m;
   uint8_t n;

   /*
    * See the following[1] for a description of what the given multi-byte
    * sequences will be based on the bits set of the first byte. We also need
    * to mask the first byte based on that.  All subsequent bytes are masked
    * against 0x3F.
    *
    * [1] http://www.joelonsoftware.com/articles/Unicode.html
    */

   if ((c & 0x80) == 0) {
      n = 1;
      m = 0x7F;
   } else if ((c & 0xE0) == 0xC0) {
      n = 2;
      m = 0x1F;
   } else if ((c & 0xF0) == 0xE0) {
      n = 3;
      m = 0x0F;
   } else if ((c & 0xF8) == 0xF0) {
      n = 4;
      m = 0x07;
   } else if ((c & 0xFC) == 0xF8) {
      n = 5;
      m = 0x03;
   } else if ((c & 0xFE) == 0xFC) {
      n = 6;
      m = 0x01;
   } else {
      n = 0;
      m = 0;
   }

   *seq_length = n;
   *first_mask = m;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_utf8_validate --
 *
 *       Validates that @utf8 is a valid UTF-8 string.
 *
 *       If @allow_null is true, then \0 is allowed within @utf8_len bytes
 *       of @utf8.  Generally, this is bad practice since the main point of
 *       UTF-8 strings is that they can be used with strlen() and friends.
 *       However, some languages such as Python can send UTF-8 encoded
 *       strings with NUL's in them.
 *
 * Parameters:
 *       @utf8: A UTF-8 encoded string.
 *       @utf8_len: The length of @utf8 in bytes.
 *       @allow_null: If \0 is allowed within @utf8, exclusing trailing \0.
 *
 * Returns:
 *       true if @utf8 is valid UTF-8. otherwise false.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_utf8_validate (const char *utf8,       /* IN */
                    size_t      utf8_len,   /* IN */
                    bool        allow_null) /* IN */
{
   bson_unichar_t c;
   uint8_t first_mask;
   uint8_t seq_length;
   unsigned i;
   unsigned j;

   BSON_ASSERT (utf8);

   for (i = 0; i < utf8_len; i += seq_length) {
      _bson_utf8_get_sequence (&utf8[i], &seq_length, &first_mask);

      /*
       * Ensure we have a valid multi-byte sequence length.
       */
      if (!seq_length) {
         return false;
      }

      /*
       * Ensure we have enough bytes left.
       */
      if ((utf8_len - i) < seq_length) {
         return false;
      }

      /*
       * Also calculate the next char as a unichar so we can
       * check code ranges for non-shortest form.
       */
      c = utf8 [i] & first_mask;

      /*
       * Check the high-bits for each additional sequence byte.
       */
      for (j = i + 1; j < (i + seq_length); j++) {
         c = (c << 6) | (utf8 [j] & 0x3F);
         if ((utf8[j] & 0xC0) != 0x80) {
            return false;
         }
      }

      /*
       * Check for NULL bytes afterwards.
       *
       * Hint: if you want to optimize this function, starting here to do
       * this in the same pass as the data above would probably be a good
       * idea. You would add a branch into the inner loop, but save possibly
       * on cache-line bouncing on larger strings. Just a thought.
       */
      if (!allow_null) {
         for (j = 0; j < seq_length; j++) {
            if (((i + j) > utf8_len) || !utf8[i + j]) {
               return false;
            }
         }
      }

      /*
       * Code point wont fit in utf-16, not allowed.
       */
      if (c > 0x0010FFFF) {
         return false;
      }

      /*
       * Byte is in reserved range for UTF-16 high-marks
       * for surrogate pairs.
       */
      if ((c & 0xFFFFF800) == 0xD800) {
         return false;
      }

      /*
       * Check non-shortest form unicode.
       */
      switch (seq_length) {
      case 1:
         if (c <= 0x007F) {
            continue;
         }
         return false;

      case 2:
         if ((c >= 0x0080) && (c <= 0x07FF)) {
            continue;
         } else if (c == 0) {
            /* Two-byte representation for NULL. */
            continue;
         }
         return false;

      case 3:
         if (((c >= 0x0800) && (c <= 0x0FFF)) ||
             ((c >= 0x1000) && (c <= 0xFFFF))) {
            continue;
         }
         return false;

      case 4:
         if (((c >= 0x10000) && (c <= 0x3FFFF)) ||
             ((c >= 0x40000) && (c <= 0xFFFFF)) ||
             ((c >= 0x100000) && (c <= 0x10FFFF))) {
            continue;
         }
         return false;

      default:
         return false;
      }
   }

   return true;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_utf8_escape_for_json --
 *
 *       Allocates a new string matching @utf8 except that special
 *       characters in JSON will be escaped. The resulting string is also
 *       UTF-8 encoded.
 *
 *       Both " and \ characters will be escaped. Additionally, if a NUL
 *       byte is found before @utf8_len bytes, it will be converted to the
 *       two byte UTF-8 sequence.
 *
 * Parameters:
 *       @utf8: A UTF-8 encoded string.
 *       @utf8_len: The length of @utf8 in bytes or -1 if NUL terminated.
 *
 * Returns:
 *       A newly allocated string that should be freed with bson_free().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

char *
bson_utf8_escape_for_json (const char *utf8,     /* IN */
                           ssize_t     utf8_len) /* IN */
{
   bson_unichar_t c;
   bson_string_t *str;
   bool length_provided = true;
   const char *end;

   BSON_ASSERT (utf8);

   str = bson_string_new (NULL);

   if (utf8_len < 0) {
      length_provided = false;
      utf8_len = strlen (utf8);
   }

   end = utf8 + utf8_len;

   while (utf8 < end) {
      c = bson_utf8_get_char (utf8);

      switch (c) {
      case '\\':
      case '"':
      case '/':
         bson_string_append_c (str, '\\');
         bson_string_append_unichar (str, c);
         break;
      case '\b':
         bson_string_append (str, "\\b");
         break;
      case '\f':
         bson_string_append (str, "\\f");
         break;
      case '\n':
         bson_string_append (str, "\\n");
         break;
      case '\r':
         bson_string_append (str, "\\r");
         break;
      case '\t':
         bson_string_append (str, "\\t");
         break;
      default:
         if (c < ' ') {
            bson_string_append_printf (str, "\\u%04u", (unsigned)c);
         } else {
            bson_string_append_unichar (str, c);
         }
         break;
      }

      if (c) {
         utf8 = bson_utf8_next_char (utf8);
      } else {
         if (length_provided && !*utf8) {
            /* we escaped nil as '\u0000', now advance past it */
            utf8++;
         } else {
            /* invalid UTF-8 */
            bson_string_free (str, true);
            return NULL;
         }
      }
   }

   return bson_string_free (str, false);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_utf8_get_char --
 *
 *       Fetches the next UTF-8 character from the UTF-8 sequence.
 *
 * Parameters:
 *       @utf8: A string containing validated UTF-8.
 *
 * Returns:
 *       A 32-bit bson_unichar_t reprsenting the multi-byte sequence.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bson_unichar_t
bson_utf8_get_char (const char *utf8) /* IN */
{
   bson_unichar_t c;
   uint8_t mask;
   uint8_t num;
   int i;

   BSON_ASSERT (utf8);

   _bson_utf8_get_sequence (utf8, &num, &mask);
   c = (*utf8) & mask;

   for (i = 1; i < num; i++) {
      c = (c << 6) | (utf8[i] & 0x3F);
   }

   return c;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_utf8_next_char --
 *
 *       Returns an incremented pointer to the beginning of the next
 *       multi-byte sequence in @utf8.
 *
 * Parameters:
 *       @utf8: A string containing validated UTF-8.
 *
 * Returns:
 *       An incremented pointer in @utf8.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

const char *
bson_utf8_next_char (const char *utf8) /* IN */
{
   uint8_t mask;
   uint8_t num;

   BSON_ASSERT (utf8);

   _bson_utf8_get_sequence (utf8, &num, &mask);

   return utf8 + num;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_utf8_from_unichar --
 *
 *       Converts the unichar to a sequence of utf8 bytes and stores those
 *       in @utf8. The number of bytes in the sequence are stored in @len.
 *
 * Parameters:
 *       @unichar: A bson_unichar_t.
 *       @utf8: A location for the multi-byte sequence.
 *       @len: A location for number of bytes stored in @utf8.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @utf8 is set.
 *       @len is set.
 *
 *--------------------------------------------------------------------------
 */

void
bson_utf8_from_unichar (
      bson_unichar_t  unichar,                               /* IN */
      char            utf8[BSON_ENSURE_ARRAY_PARAM_SIZE(6)], /* OUT */
      uint32_t       *len)                                   /* OUT */
{
   BSON_ASSERT (utf8);
   BSON_ASSERT (len);

   if (unichar <= 0x7F) {
      utf8[0] = unichar;
      *len = 1;
   } else if (unichar <= 0x7FF) {
      *len = 2;
      utf8[0] = 0xC0 | ((unichar >> 6) & 0x3F);
      utf8[1] = 0x80 | ((unichar) & 0x3F);
   } else if (unichar <= 0xFFFF) {
      *len = 3;
      utf8[0] = 0xE0 | ((unichar >> 12) & 0xF);
      utf8[1] = 0x80 | ((unichar >> 6) & 0x3F);
      utf8[2] = 0x80 | ((unichar) & 0x3F);
   } else if (unichar <= 0x1FFFFF) {
      *len = 4;
      utf8[0] = 0xF0 | ((unichar >> 18) & 0x7);
      utf8[1] = 0x80 | ((unichar >> 12) & 0x3F);
      utf8[2] = 0x80 | ((unichar >> 6) & 0x3F);
      utf8[3] = 0x80 | ((unichar) & 0x3F);
   } else if (unichar <= 0x3FFFFFF) {
      *len = 5;
      utf8[0] = 0xF8 | ((unichar >> 24) & 0x3);
      utf8[1] = 0x80 | ((unichar >> 18) & 0x3F);
      utf8[2] = 0x80 | ((unichar >> 12) & 0x3F);
      utf8[3] = 0x80 | ((unichar >> 6) & 0x3F);
      utf8[4] = 0x80 | ((unichar) & 0x3F);
   } else if (unichar <= 0x7FFFFFFF) {
      *len = 6;
      utf8[0] = 0xFC | ((unichar >> 31) & 0x1);
      utf8[1] = 0x80 | ((unichar >> 25) & 0x3F);
      utf8[2] = 0x80 | ((unichar >> 19) & 0x3F);
      utf8[3] = 0x80 | ((unichar >> 13) & 0x3F);
      utf8[4] = 0x80 | ((unichar >> 7) & 0x3F);
      utf8[5] = 0x80 | ((unichar) & 0x1);
   } else {
      *len = 0;
   }
}
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// #include "bson-private.h"
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef BSON_PRIVATE_H
#define BSON_PRIVATE_H


// #include "bson-macros.h"

// #include "bson-memory.h"

// #include "bson-types.h"



BSON_BEGIN_DECLS


typedef enum
{
   BSON_FLAG_NONE            = 0,
   BSON_FLAG_INLINE          = (1 << 0),
   BSON_FLAG_STATIC          = (1 << 1),
   BSON_FLAG_RDONLY          = (1 << 2),
   BSON_FLAG_CHILD           = (1 << 3),
   BSON_FLAG_IN_CHILD        = (1 << 4),
   BSON_FLAG_NO_FREE         = (1 << 5),
} bson_flags_t;


BSON_ALIGNED_BEGIN (128)
typedef struct
{
   bson_flags_t flags;
   uint32_t     len;
   uint8_t      data [120];
} bson_impl_inline_t
BSON_ALIGNED_END (128);


BSON_STATIC_ASSERT (sizeof (bson_impl_inline_t) == 128);


BSON_ALIGNED_BEGIN (128)
typedef struct
{
   bson_flags_t        flags;            /* flags describing the bson_t */
   uint32_t            len;              /* length of bson document in bytes */
   bson_t             *parent;           /* parent bson if a child */
   uint32_t            depth;            /* Subdocument depth. */
   uint8_t           **buf;              /* pointer to buffer pointer */
   size_t             *buflen;           /* pointer to buffer length */
   size_t              offset;           /* our offset inside *buf  */
   uint8_t            *alloc;            /* buffer that we own. */
   size_t              alloclen;         /* length of buffer that we own. */
   bson_realloc_func   realloc;          /* our realloc implementation */
   void               *realloc_func_ctx; /* context for our realloc func */
} bson_impl_alloc_t
BSON_ALIGNED_END (128);


BSON_STATIC_ASSERT (sizeof (bson_impl_alloc_t) <= 128);


BSON_END_DECLS


#endif /* BSON_PRIVATE_H */

// #include "bson-writer.h"



struct _bson_writer_t
{
   bool                ready;
   uint8_t           **buf;
   size_t             *buflen;
   size_t              offset;
   bson_realloc_func   realloc_func;
   void               *realloc_func_ctx;
   bson_t              b;
};


/*
 *--------------------------------------------------------------------------
 *
 * bson_writer_new --
 *
 *       Creates a new instance of bson_writer_t using the buffer, length,
 *       offset, and realloc() function supplied.
 *
 *       The caller is expected to clean up the structure when finished
 *       using bson_writer_destroy().
 *
 * Parameters:
 *       @buf: (inout): A pointer to a target buffer.
 *       @buflen: (inout): A pointer to the buffer length.
 *       @offset: The offset in the target buffer to start from.
 *       @realloc_func: A realloc() style function or NULL.
 *
 * Returns:
 *       A newly allocated bson_writer_t that should be freed with
 *       bson_writer_destroy().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bson_writer_t *
bson_writer_new (uint8_t           **buf,              /* IN */
                 size_t             *buflen,           /* IN */
                 size_t              offset,           /* IN */
                 bson_realloc_func   realloc_func,     /* IN */
                 void               *realloc_func_ctx) /* IN */
{
   bson_writer_t *writer;

   writer = bson_malloc0 (sizeof *writer);
   writer->buf = buf;
   writer->buflen = buflen;
   writer->offset = offset;
   writer->realloc_func = realloc_func;
   writer->realloc_func_ctx = realloc_func_ctx;
   writer->ready = true;

   return writer;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_writer_destroy --
 *
 *       Cleanup after @writer and release any allocated memory. Note that
 *       the buffer supplied to bson_writer_new() is NOT freed from this
 *       method.  The caller is responsible for that.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_writer_destroy (bson_writer_t *writer) /* IN */
{
   bson_free (writer);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_writer_get_length --
 *
 *       Fetches the current length of the content written by the buffer
 *       (including the initial offset). This includes a partly written
 *       document currently being written.
 *
 *       This is useful if you want to check to see if you've passed a given
 *       memory boundry that cannot be sent in a packet. See
 *       bson_writer_rollback() to abort the current document being written.
 *
 * Returns:
 *       The number of bytes written plus initial offset.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

size_t
bson_writer_get_length (bson_writer_t *writer) /* IN */
{
   return writer->offset + writer->b.len;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_writer_begin --
 *
 *       Begins writing a new document. The caller may use the bson
 *       structure to write out a new BSON document. When completed, the
 *       caller must call either bson_writer_end() or
 *       bson_writer_rollback().
 *
 * Parameters:
 *       @writer: A bson_writer_t.
 *       @bson: (out): A location for a bson_t*.
 *
 * Returns:
 *       true if the underlying realloc was successful; otherwise false.
 *
 * Side effects:
 *       @bson is initialized if true is returned.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_writer_begin (bson_writer_t  *writer, /* IN */
                   bson_t        **bson)   /* OUT */
{
   bson_impl_alloc_t *b;
   bool grown = false;

   BSON_ASSERT (writer);
   BSON_ASSERT (writer->ready);
   BSON_ASSERT (bson);

   writer->ready = false;

   memset (&writer->b, 0, sizeof (bson_t));

   b = (bson_impl_alloc_t *)&writer->b;
   b->flags = BSON_FLAG_STATIC | BSON_FLAG_NO_FREE;
   b->len = 5;
   b->parent = NULL;
   b->buf = writer->buf;
   b->buflen = writer->buflen;
   b->offset = writer->offset;
   b->alloc = NULL;
   b->alloclen = 0;
   b->realloc = writer->realloc_func;
   b->realloc_func_ctx = writer->realloc_func_ctx;

   while ((writer->offset + writer->b.len) > *writer->buflen) {
      if (!writer->realloc_func) {
         memset (&writer->b, 0, sizeof (bson_t));
         writer->ready = true;
         return false;
      }
      grown = true;

      if (!*writer->buflen) {
         *writer->buflen = 64;
      } else {
         (*writer->buflen) *= 2;
      }
   }

   if (grown) {
      *writer->buf = writer->realloc_func (*writer->buf, *writer->buflen, writer->realloc_func_ctx);
   }

   memset ((*writer->buf) + writer->offset + 1, 0, 5);
   (*writer->buf)[writer->offset] = 5;

   *bson = &writer->b;

   return true;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_writer_end --
 *
 *       Complete writing of a bson_writer_t to the buffer supplied.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_writer_end (bson_writer_t *writer) /* IN */
{
   BSON_ASSERT (writer);
   BSON_ASSERT (!writer->ready);

   writer->offset += writer->b.len;
   memset (&writer->b, 0, sizeof (bson_t));
   writer->ready = true;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_writer_rollback --
 *
 *       Abort the appending of the current bson_t to the memory region
 *       managed by @writer.  This is useful if you detected that you went
 *       past a particular memory limit.  For example, MongoDB has 48MB
 *       message limits.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_writer_rollback (bson_writer_t *writer) /* IN */
{
   BSON_ASSERT (writer);

   if (writer->b.len) {
      memset (&writer->b, 0, sizeof (bson_t));
   }

   writer->ready = true;
}
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// #include "bson.h"

// #include "b64_ntop.h"
/*
 * Copyright (c) 1996, 1998 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*
 * Portions Copyright (c) 1995 by International Business Machines, Inc.
 *
 * International Business Machines, Inc. (hereinafter called IBM) grants
 * permission under its copyrights to use, copy, modify, and distribute this
 * Software with or without fee, provided that the above copyright notice and
 * all paragraphs of this notice appear in all copies, and that the name of IBM
 * not be used in connection with the marketing of any product incorporating
 * the Software or modifications thereof, without specific, written prior
 * permission.
 *
 * To the extent it has a right to do so, IBM grants an immunity from suit
 * under its patents, if any, for the use, sale or manufacture of products to
 * the extent that such products are used for performing Domain Name System
 * dynamic updates in TCP/IP networks by means of the Software.  No immunity is
 * granted for any product per se or for any other function of any product.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", AND IBM DISCLAIMS ALL WARRANTIES,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL,
 * DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE, EVEN
 * IF IBM IS APPRISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */

// #include "bson-compat.h"

// #include "bson-macros.h"

// #include "bson-types.h"


#define Assert(Cond) if (!(Cond)) abort ()

static const char Base64[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char Pad64 = '=';

/* (From RFC1521 and draft-ietf-dnssec-secext-03.txt)
 * The following encoding technique is taken from RFC 1521 by Borenstein
 * and Freed.  It is reproduced here in a slightly edited form for
 * convenience.
 *
 * A 65-character subset of US-ASCII is used, enabling 6 bits to be
 * represented per printable character. (The extra 65th character, "=",
 * is used to signify a special processing function.)
 *
 * The encoding process represents 24-bit groups of input bits as output
 * strings of 4 encoded characters. Proceeding from left to right, a
 * 24-bit input group is formed by concatenating 3 8-bit input groups.
 * These 24 bits are then treated as 4 concatenated 6-bit groups, each
 * of which is translated into a single digit in the base64 alphabet.
 *
 * Each 6-bit group is used as an index into an array of 64 printable
 * characters. The character referenced by the index is placed in the
 * output string.
 *
 *                       Table 1: The Base64 Alphabet
 *
 *    Value Encoding  Value Encoding  Value Encoding  Value Encoding
 *        0 A            17 R            34 i            51 z
 *        1 B            18 S            35 j            52 0
 *        2 C            19 T            36 k            53 1
 *        3 D            20 U            37 l            54 2
 *        4 E            21 V            38 m            55 3
 *        5 F            22 W            39 n            56 4
 *        6 G            23 X            40 o            57 5
 *        7 H            24 Y            41 p            58 6
 *        8 I            25 Z            42 q            59 7
 *        9 J            26 a            43 r            60 8
 *       10 K            27 b            44 s            61 9
 *       11 L            28 c            45 t            62 +
 *       12 M            29 d            46 u            63 /
 *       13 N            30 e            47 v
 *       14 O            31 f            48 w         (pad) =
 *       15 P            32 g            49 x
 *       16 Q            33 h            50 y
 *
 * Special processing is performed if fewer than 24 bits are available
 * at the end of the data being encoded.  A full encoding quantum is
 * always completed at the end of a quantity.  When fewer than 24 input
 * bits are available in an input group, zero bits are added (on the
 * right) to form an integral number of 6-bit groups.  Padding at the
 * end of the data is performed using the '=' character.
 *
 * Since all base64 input is an integral number of octets, only the
 * following cases can arise:
 *
 *     (1) the final quantum of encoding input is an integral
 *         multiple of 24 bits; here, the final unit of encoded
 *    output will be an integral multiple of 4 characters
 *    with no "=" padding,
 *     (2) the final quantum of encoding input is exactly 8 bits;
 *         here, the final unit of encoded output will be two
 *    characters followed by two "=" padding characters, or
 *     (3) the final quantum of encoding input is exactly 16 bits;
 *         here, the final unit of encoded output will be three
 *    characters followed by one "=" padding character.
 */

static ssize_t
b64_ntop (uint8_t const *src,
          size_t         srclength,
          char          *target,
          size_t         targsize)
{
   size_t datalength = 0;
   uint8_t input[3];
   uint8_t output[4];
   size_t i;

   while (2 < srclength) {
      input[0] = *src++;
      input[1] = *src++;
      input[2] = *src++;
      srclength -= 3;

      output[0] = input[0] >> 2;
      output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
      output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
      output[3] = input[2] & 0x3f;
      Assert (output[0] < 64);
      Assert (output[1] < 64);
      Assert (output[2] < 64);
      Assert (output[3] < 64);

      if (datalength + 4 > targsize) {
         return -1;
      }
      target[datalength++] = Base64[output[0]];
      target[datalength++] = Base64[output[1]];
      target[datalength++] = Base64[output[2]];
      target[datalength++] = Base64[output[3]];
   }

   /* Now we worry about padding. */
   if (0 != srclength) {
      /* Get what's left. */
      input[0] = input[1] = input[2] = '\0';

      for (i = 0; i < srclength; i++) {
         input[i] = *src++;
      }
      output[0] = input[0] >> 2;
      output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
      output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
      Assert (output[0] < 64);
      Assert (output[1] < 64);
      Assert (output[2] < 64);

      if (datalength + 4 > targsize) {
         return -1;
      }
      target[datalength++] = Base64[output[0]];
      target[datalength++] = Base64[output[1]];

      if (srclength == 1) {
         target[datalength++] = Pad64;
      } else{
         target[datalength++] = Base64[output[2]];
      }
      target[datalength++] = Pad64;
   }

   if (datalength >= targsize) {
      return -1;
   }
   target[datalength] = '\0'; /* Returned value doesn't count \0. */
   return datalength;
}

// #include "bson-private.h"

// #include "bson-string.h"


#include <stdarg.h>
#include <string.h>
#include <math.h>


#ifndef BSON_MAX_RECURSION
# define BSON_MAX_RECURSION 100
#endif


typedef enum {
   BSON_VALIDATE_PHASE_START,
   BSON_VALIDATE_PHASE_TOP,
   BSON_VALIDATE_PHASE_LF_REF_KEY,
   BSON_VALIDATE_PHASE_LF_REF_UTF8,
   BSON_VALIDATE_PHASE_LF_ID_KEY,
   BSON_VALIDATE_PHASE_LF_DB_KEY,
   BSON_VALIDATE_PHASE_LF_DB_UTF8,
   BSON_VALIDATE_PHASE_NOT_DBREF,
} bson_validate_phase_t;


/*
 * Structures.
 */
typedef struct
{
   bson_validate_flags_t flags;
   ssize_t               err_offset;
   bson_validate_phase_t phase;
} bson_validate_state_t;


typedef struct
{
   uint32_t       count;
   bool           keys;
   uint32_t       depth;
   bson_string_t *str;
} bson_json_state_t;


/*
 * Forward declarations.
 */
static bool _bson_as_json_visit_array    (const bson_iter_t *iter,
                                          const char        *key,
                                          const bson_t      *v_array,
                                          void              *data);
static bool _bson_as_json_visit_document (const bson_iter_t *iter,
                                          const char        *key,
                                          const bson_t      *v_document,
                                          void              *data);


/*
 * Globals.
 */
static const uint8_t gZero;


/*
 *--------------------------------------------------------------------------
 *
 * _bson_impl_inline_grow --
 *
 *       Document growth implementation for documents that currently
 *       contain stack based buffers. The document may be switched to
 *       a malloc based buffer.
 *
 * Returns:
 *       true if successful; otherwise false indicating INT_MAX overflow.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static bool
_bson_impl_inline_grow (bson_impl_inline_t *impl, /* IN */
                        size_t              size) /* IN */
{
   bson_impl_alloc_t *alloc = (bson_impl_alloc_t *)impl;
   uint8_t *data;
   size_t req;

   if (((size_t)impl->len + size) <= sizeof impl->data) {
      return true;
   }

   req = bson_next_power_of_two (impl->len + size);

   if (req <= INT32_MAX) {
      data = bson_malloc (req);

      memcpy (data, impl->data, impl->len);
      alloc->flags &= ~BSON_FLAG_INLINE;
      alloc->parent = NULL;
      alloc->depth = 0;
      alloc->buf = &alloc->alloc;
      alloc->buflen = &alloc->alloclen;
      alloc->offset = 0;
      alloc->alloc = data;
      alloc->alloclen = req;
      alloc->realloc = bson_realloc_ctx;
      alloc->realloc_func_ctx = NULL;

      return true;
   }

   return false;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_impl_alloc_grow --
 *
 *       Document growth implementation for documents containing malloc
 *       based buffers.
 *
 * Returns:
 *       true if successful; otherwise false indicating INT_MAX overflow.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static bool
_bson_impl_alloc_grow (bson_impl_alloc_t *impl, /* IN */
                       size_t             size) /* IN */
{
   size_t req;

   /*
    * Determine how many bytes we need for this document in the buffer
    * including necessary trailing bytes for parent documents.
    */
   req = (impl->offset + impl->len + size + impl->depth);

   if (req <= *impl->buflen) {
      return true;
   }

   req = bson_next_power_of_two (req);

   if ((req <= INT32_MAX) && impl->realloc) {
      *impl->buf = impl->realloc (*impl->buf, req, impl->realloc_func_ctx);
      *impl->buflen = req;
      return true;
   }

   return false;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_grow --
 *
 *       Grows the bson_t structure to be large enough to contain @size
 *       bytes.
 *
 * Returns:
 *       true if successful, false if the size would overflow.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static bool
_bson_grow (bson_t   *bson, /* IN */
            uint32_t  size) /* IN */
{
   if ((bson->flags & BSON_FLAG_INLINE)) {
      return _bson_impl_inline_grow ((bson_impl_inline_t *)bson, size);
   }

   return _bson_impl_alloc_grow ((bson_impl_alloc_t *)bson, size);
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_data --
 *
 *       A helper function to return the contents of the bson document
 *       taking into account the polymorphic nature of bson_t.
 *
 * Returns:
 *       A buffer which should not be modified or freed.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static BSON_INLINE uint8_t *
_bson_data (const bson_t *bson) /* IN */
{
   if ((bson->flags & BSON_FLAG_INLINE)) {
      return ((bson_impl_inline_t *)bson)->data;
   } else {
      bson_impl_alloc_t *impl = (bson_impl_alloc_t *)bson;
      return (*impl->buf) + impl->offset;
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_encode_length --
 *
 *       Helper to encode the length of the bson_t in the first 4 bytes
 *       of the bson document. Little endian format is used as specified
 *       by bsonspec.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static BSON_INLINE void
_bson_encode_length (bson_t *bson) /* IN */
{
#if BSON_BYTE_ORDER == BSON_LITTLE_ENDIAN
   memcpy (_bson_data (bson), &bson->len, sizeof (bson->len));
#else
   uint32_t length_le = BSON_UINT32_TO_LE (bson->len);
   memcpy (_bson_data (bson), &length_le, sizeof (length_le));
#endif
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_append_va --
 *
 *       Appends the length,buffer pairs to the bson_t. @n_bytes is an
 *       optimization to perform one array growth rather than many small
 *       growths.
 *
 *       @bson: A bson_t
 *       @n_bytes: The number of bytes to append to the document.
 *       @n_pairs: The number of length,buffer pairs.
 *       @first_len: Length of first buffer.
 *       @first_data: First buffer.
 *       @args: va_list of additional tuples.
 *
 * Returns:
 *       true if the bytes were appended successfully.
 *       false if it bson would overflow INT_MAX.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static BSON_INLINE bool
_bson_append_va (bson_t        *bson,        /* IN */
                 uint32_t       n_bytes,     /* IN */
                 uint32_t       n_pairs,     /* IN */
                 uint32_t       first_len,   /* IN */
                 const uint8_t *first_data,  /* IN */
                 va_list        args)        /* IN */
{
   const uint8_t *data;
   uint32_t data_len;
   uint8_t *buf;

   BSON_ASSERT (!(bson->flags & BSON_FLAG_IN_CHILD));
   BSON_ASSERT (!(bson->flags & BSON_FLAG_RDONLY));

   if (BSON_UNLIKELY (!_bson_grow (bson, n_bytes))) {
      return false;
   }

   data = first_data;
   data_len = first_len;

   buf = _bson_data (bson) + bson->len - 1;

   do {
      n_pairs--;
      memcpy (buf, data, data_len);
      bson->len += data_len;
      buf += data_len;

      if (n_pairs) {
         data_len = va_arg (args, uint32_t);
         data = va_arg (args, const uint8_t *);
      }
   } while (n_pairs);

   _bson_encode_length (bson);

   *buf = '\0';

   return true;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_append --
 *
 *       Variadic function to append length,buffer pairs to a bson_t. If the
 *       append would cause the bson_t to overflow a 32-bit length, it will
 *       return false and no append will have occurred.
 *
 * Parameters:
 *       @bson: A bson_t.
 *       @n_pairs: Number of length,buffer pairs.
 *       @n_bytes: the total number of bytes being appended.
 *       @first_len: Length of first buffer.
 *       @first_data: First buffer.
 *
 * Returns:
 *       true if successful; otherwise false indicating INT_MAX overflow.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

static bool
_bson_append (bson_t        *bson,        /* IN */
              uint32_t       n_pairs,     /* IN */
              uint32_t       n_bytes,     /* IN */
              uint32_t       first_len,   /* IN */
              const uint8_t *first_data,  /* IN */
              ...)
{
   va_list args;
   bool ok;

   BSON_ASSERT (n_pairs);
   BSON_ASSERT (first_len);
   BSON_ASSERT (first_data);

   /*
    * Check to see if this append would overflow 32-bit signed integer. I know
    * what you're thinking. BSON uses a signed 32-bit length field? Yeah. It
    * does.
    */
   if (BSON_UNLIKELY (n_bytes > (BSON_MAX_SIZE - bson->len))) {
      return false;
   }

   va_start (args, first_data);
   ok = _bson_append_va (bson, n_bytes, n_pairs, first_len, first_data, args);
   va_end (args);

   return ok;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_append_bson_begin --
 *
 *       Begin appending a subdocument or subarray to the document using
 *       the key provided by @key.
 *
 *       If @key_length is < 0, then strlen() will be called on @key
 *       to determine the length.
 *
 *       @key_type MUST be either BSON_TYPE_DOCUMENT or BSON_TYPE_ARRAY.
 *
 * Returns:
 *       true if successful; otherwise false indicating INT_MAX overflow.
 *
 * Side effects:
 *       @child is initialized if true is returned.
 *
 *--------------------------------------------------------------------------
 */

static bool
_bson_append_bson_begin (bson_t      *bson,        /* IN */
                         const char  *key,         /* IN */
                         int          key_length,  /* IN */
                         bson_type_t  child_type,  /* IN */
                         bson_t      *child)       /* OUT */
{
   const uint8_t type = child_type;
   const uint8_t empty[5] = { 5 };
   bson_impl_alloc_t *aparent = (bson_impl_alloc_t *)bson;
   bson_impl_alloc_t *achild = (bson_impl_alloc_t *)child;

   BSON_ASSERT (!(bson->flags & BSON_FLAG_RDONLY));
   BSON_ASSERT (!(bson->flags & BSON_FLAG_IN_CHILD));
   BSON_ASSERT (key);
   BSON_ASSERT ((child_type == BSON_TYPE_DOCUMENT) ||
                (child_type == BSON_TYPE_ARRAY));
   BSON_ASSERT (child);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   /*
    * If the parent is an inline bson_t, then we need to convert
    * it to a heap allocated buffer. This makes extending buffers
    * of child bson documents much simpler logic, as they can just
    * realloc the *buf pointer.
    */
   if ((bson->flags & BSON_FLAG_INLINE)) {
      BSON_ASSERT (bson->len <= 120);
      if (!_bson_grow (bson, 128 - bson->len)) {
         return false;
      }
      BSON_ASSERT (!(bson->flags & BSON_FLAG_INLINE));
   }

   /*
    * Append the type and key for the field.
    */
   if (!_bson_append (bson, 4,
                      (1 + key_length + 1 + 5),
                      1, &type,
                      key_length, key,
                      1, &gZero,
                      5, empty)) {
      return false;
   }

   /*
    * Mark the document as working on a child document so that no
    * further modifications can happen until the caller has called
    * bson_append_{document,array}_end().
    */
   bson->flags |= BSON_FLAG_IN_CHILD;

   /*
    * Initialize the child bson_t structure and point it at the parents
    * buffers. This allows us to realloc directly from the child without
    * walking up to the parent bson_t.
    */
   achild->flags = (BSON_FLAG_CHILD | BSON_FLAG_NO_FREE | BSON_FLAG_STATIC);

   if ((bson->flags & BSON_FLAG_CHILD)) {
      achild->depth = ((bson_impl_alloc_t *)bson)->depth + 1;
   } else {
      achild->depth = 1;
   }

   achild->parent = bson;
   achild->buf = aparent->buf;
   achild->buflen = aparent->buflen;
   achild->offset = aparent->offset + aparent->len - 1 - 5;
   achild->len = 5;
   achild->alloc = NULL;
   achild->alloclen = 0;
   achild->realloc = aparent->realloc;
   achild->realloc_func_ctx = aparent->realloc_func_ctx;

   return true;
}


/*
 *--------------------------------------------------------------------------
 *
 * _bson_append_bson_end --
 *
 *       Complete a call to _bson_append_bson_begin.
 *
 * Returns:
 *       true if successful; otherwise false indicating INT_MAX overflow.
 *
 * Side effects:
 *       @child is destroyed and no longer valid after calling this
 *       function.
 *
 *--------------------------------------------------------------------------
 */

static bool
_bson_append_bson_end (bson_t *bson,   /* IN */
                       bson_t *child)  /* IN */
{
   BSON_ASSERT (bson);
   BSON_ASSERT ((bson->flags & BSON_FLAG_IN_CHILD));
   BSON_ASSERT (!(child->flags & BSON_FLAG_IN_CHILD));

   /*
    * Unmark the IN_CHILD flag.
    */
   bson->flags &= ~BSON_FLAG_IN_CHILD;

   /*
    * Now that we are done building the sub-document, add the size to the
    * parent, not including the default 5 byte empty document already added.
    */
   bson->len = (bson->len + child->len - 5);

   /*
    * Ensure we have a \0 byte at the end and proper length encoded at
    * the beginning of the document.
    */
   _bson_data (bson)[bson->len - 1] = '\0';
   _bson_encode_length (bson);

   return true;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_array_begin --
 *
 *       Start appending a new array.
 *
 *       Use @child to append to the data area for the given field.
 *
 *       It is a programming error to call any other bson function on
 *       @bson until bson_append_array_end() has been called. It is
 *       valid to call bson_append*() functions on @child.
 *
 *       This function is useful to allow building nested documents using
 *       a single buffer owned by the top-level bson document.
 *
 * Returns:
 *       true if successful; otherwise false and @child is invalid.
 *
 * Side effects:
 *       @child is initialized if true is returned.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_append_array_begin (bson_t     *bson,         /* IN */
                         const char *key,          /* IN */
                         int         key_length,   /* IN */
                         bson_t     *child)        /* IN */
{
   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (child);

   return _bson_append_bson_begin (bson, key, key_length, BSON_TYPE_ARRAY,
                                   child);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_array_end --
 *
 *       Complete a call to bson_append_array_begin().
 *
 *       It is safe to append other fields to @bson after calling this
 *       function.
 *
 * Returns:
 *       true if successful; otherwise false indicating INT_MAX overflow.
 *
 * Side effects:
 *       @child is invalid after calling this function.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_append_array_end (bson_t *bson,   /* IN */
                       bson_t *child)  /* IN */
{
   BSON_ASSERT (bson);
   BSON_ASSERT (child);

   return _bson_append_bson_end (bson, child);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_document_begin --
 *
 *       Start appending a new document.
 *
 *       Use @child to append to the data area for the given field.
 *
 *       It is a programming error to call any other bson function on
 *       @bson until bson_append_document_end() has been called. It is
 *       valid to call bson_append*() functions on @child.
 *
 *       This function is useful to allow building nested documents using
 *       a single buffer owned by the top-level bson document.
 *
 * Returns:
 *       true if successful; otherwise false and @child is invalid.
 *
 * Side effects:
 *       @child is initialized if true is returned.
 *
 *--------------------------------------------------------------------------
 */
bool
bson_append_document_begin (bson_t     *bson,         /* IN */
                            const char *key,          /* IN */
                            int         key_length,   /* IN */
                            bson_t     *child)        /* IN */
{
   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (child);

   return _bson_append_bson_begin (bson, key, key_length, BSON_TYPE_DOCUMENT,
                                   child);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_document_end --
 *
 *       Complete a call to bson_append_document_begin().
 *
 *       It is safe to append new fields to @bson after calling this
 *       function, if true is returned.
 *
 * Returns:
 *       true if successful; otherwise false indicating INT_MAX overflow.
 *
 * Side effects:
 *       @child is destroyed and invalid after calling this function.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_append_document_end (bson_t *bson,   /* IN */
                          bson_t *child)  /* IN */
{
   BSON_ASSERT (bson);
   BSON_ASSERT (child);

   return _bson_append_bson_end (bson, child);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_array --
 *
 *       Append an array to @bson.
 *
 *       Generally, bson_append_array_begin() will result in faster code
 *       since few buffers need to be malloced.
 *
 * Returns:
 *       true if successful; otherwise false indicating INT_MAX overflow.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_append_array (bson_t       *bson,       /* IN */
                   const char   *key,        /* IN */
                   int           key_length, /* IN */
                   const bson_t *array)      /* IN */
{
   static const uint8_t type = BSON_TYPE_ARRAY;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (array);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   /*
    * Let's be a bit pedantic and ensure the array has properly formatted key
    * names.  We will verify this simply by checking the first element for "0"
    * if the array is non-empty.
    */
   if (array && !bson_empty (array)) {
      bson_iter_t iter;

      if (bson_iter_init (&iter, array) && bson_iter_next (&iter)) {
         if (0 != strcmp ("0", bson_iter_key (&iter))) {
            fprintf (stderr,
                     "%s(): invalid array detected. first element of array "
                     "parameter is not \"0\".\n",
                     BSON_FUNC);
         }
      }
   }

   return _bson_append (bson, 4,
                        (1 + key_length + 1 + array->len),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        array->len, _bson_data (array));
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_binary --
 *
 *       Append binary data to @bson. The field will have the
 *       BSON_TYPE_BINARY type.
 *
 * Parameters:
 *       @subtype: the BSON Binary Subtype. See bsonspec.org for more
 *                 information.
 *       @binary: a pointer to the raw binary data.
 *       @length: the size of @binary in bytes.
 *
 * Returns:
 *       true if successful; otherwise false.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_append_binary (bson_t         *bson,       /* IN */
                    const char     *key,        /* IN */
                    int             key_length, /* IN */
                    bson_subtype_t  subtype,    /* IN */
                    const uint8_t  *binary,     /* IN */
                    uint32_t        length)     /* IN */
{
   static const uint8_t type = BSON_TYPE_BINARY;
   uint32_t length_le;
   uint32_t deprecated_length_le;
   uint8_t subtype8 = 0;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (binary);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   subtype8 = subtype;

   if (subtype == BSON_SUBTYPE_BINARY_DEPRECATED) {
      length_le = BSON_UINT32_TO_LE (length + 4);
      deprecated_length_le = BSON_UINT32_TO_LE (length);

      return _bson_append (bson, 7,
                           (1 + key_length + 1 + 4 + 1 + 4 + length),
                           1, &type,
                           key_length, key,
                           1, &gZero,
                           4, &length_le,
                           1, &subtype8,
                           4, &deprecated_length_le,
                           length, binary);
   } else {
      length_le = BSON_UINT32_TO_LE (length);

      return _bson_append (bson, 6,
                           (1 + key_length + 1 + 4 + 1 + length),
                           1, &type,
                           key_length, key,
                           1, &gZero,
                           4, &length_le,
                           1, &subtype8,
                           length, binary);
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_bool --
 *
 *       Append a new field to @bson with the name @key. The value is
 *       a boolean indicated by @value.
 *
 * Returns:
 *       true if succesful; otherwise false.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_append_bool (bson_t     *bson,       /* IN */
                  const char *key,        /* IN */
                  int         key_length, /* IN */
                  bool        value)      /* IN */
{
   static const uint8_t type = BSON_TYPE_BOOL;
   uint8_t abyte = !!value;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   return _bson_append (bson, 4,
                        (1 + key_length + 1 + 1),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        1, &abyte);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_code --
 *
 *       Append a new field to @bson containing javascript code.
 *
 *       @javascript MUST be a zero terminated UTF-8 string. It MUST NOT
 *       containing embedded \0 characters.
 *
 * Returns:
 *       true if successful; otherwise false.
 *
 * Side effects:
 *       None.
 *
 * See also:
 *       bson_append_code_with_scope().
 *
 *--------------------------------------------------------------------------
 */

bool
bson_append_code (bson_t     *bson,       /* IN */
                  const char *key,        /* IN */
                  int         key_length, /* IN */
                  const char *javascript) /* IN */
{
   static const uint8_t type = BSON_TYPE_CODE;
   uint32_t length;
   uint32_t length_le;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (javascript);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   length = (int)strlen (javascript) + 1;
   length_le = BSON_UINT32_TO_LE (length);

   return _bson_append (bson, 5,
                        (1 + key_length + 1 + 4 + length),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        4, &length_le,
                        length, javascript);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_code_with_scope --
 *
 *       Append a new field to @bson containing javascript code with
 *       supplied scope.
 *
 * Returns:
 *       true if successful; otherwise false.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_append_code_with_scope (bson_t       *bson,         /* IN */
                             const char   *key,          /* IN */
                             int           key_length,   /* IN */
                             const char   *javascript,   /* IN */
                             const bson_t *scope)        /* IN */
{
   static const uint8_t type = BSON_TYPE_CODEWSCOPE;
   uint32_t codews_length_le;
   uint32_t codews_length;
   uint32_t js_length_le;
   uint32_t js_length;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (javascript);

   if (bson_empty0 (scope)) {
      return bson_append_code (bson, key, key_length, javascript);
   }

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   js_length = (int)strlen (javascript) + 1;
   js_length_le = BSON_UINT32_TO_LE (js_length);

   codews_length = 4 + 4 + js_length + scope->len;
   codews_length_le = BSON_UINT32_TO_LE (codews_length);

   return _bson_append (bson, 7,
                        (1 + key_length + 1 + 4 + 4 + js_length + scope->len),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        4, &codews_length_le,
                        4, &js_length_le,
                        js_length, javascript,
                        scope->len, _bson_data (scope));
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_dbpointer --
 *
 *       This BSON data type is DEPRECATED.
 *
 *       Append a BSON dbpointer field to @bson.
 *
 * Returns:
 *       true if successful; otherwise false.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bool
bson_append_dbpointer (bson_t           *bson,       /* IN */
                       const char       *key,        /* IN */
                       int               key_length, /* IN */
                       const char       *collection, /* IN */
                       const bson_oid_t *oid)
{
   static const uint8_t type = BSON_TYPE_DBPOINTER;
   uint32_t length;
   uint32_t length_le;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (collection);
   BSON_ASSERT (oid);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   length = (int)strlen (collection) + 1;
   length_le = BSON_UINT32_TO_LE (length);

   return _bson_append (bson, 6,
                        (1 + key_length + 1 + 4 + length + 12),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        4, &length_le,
                        length, collection,
                        12, oid);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_append_document --
 *
 *       Append a new field to @bson containing a BSON document.
 *
 *       In general, using bson_append_document_begin() results in faster
 *       code and less memory fragmentation.
 *
 * Returns:
 *       true if successful; otherwise false.
 *
 * Side effects:
 *       None.
 *
 * See also:
 *       bson_append_document_begin().
 *
 *--------------------------------------------------------------------------
 */

bool
bson_append_document (bson_t       *bson,       /* IN */
                      const char   *key,        /* IN */
                      int           key_length, /* IN */
                      const bson_t *value)      /* IN */
{
   static const uint8_t type = BSON_TYPE_DOCUMENT;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (value);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   return _bson_append (bson, 4,
                        (1 + key_length + 1 + value->len),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        value->len, _bson_data (value));
}


bool
bson_append_double (bson_t     *bson,
                    const char *key,
                    int         key_length,
                    double      value)
{
   static const uint8_t type = BSON_TYPE_DOUBLE;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

#if BSON_BYTE_ORDER == BSON_BIG_ENDIAN
   value = BSON_DOUBLE_TO_LE (value);
#endif

   return _bson_append (bson, 4,
                        (1 + key_length + 1 + 8),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        8, &value);
}


bool
bson_append_int32 (bson_t      *bson,
                   const char  *key,
                   int          key_length,
                   int32_t value)
{
   static const uint8_t type = BSON_TYPE_INT32;
   uint32_t value_le;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   value_le = BSON_UINT32_TO_LE (value);

   return _bson_append (bson, 4,
                        (1 + key_length + 1 + 4),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        4, &value_le);
}


bool
bson_append_int64 (bson_t      *bson,
                   const char  *key,
                   int          key_length,
                   int64_t value)
{
   static const uint8_t type = BSON_TYPE_INT64;
   uint64_t value_le;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   value_le = BSON_UINT64_TO_LE (value);

   return _bson_append (bson, 4,
                        (1 + key_length + 1 + 8),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        8, &value_le);
}


bool
bson_append_iter (bson_t            *bson,
                  const char        *key,
                  int                key_length,
                  const bson_iter_t *iter)
{
   bool ret = false;

   BSON_ASSERT (bson);
   BSON_ASSERT (iter);

   if (!key) {
      key = bson_iter_key (iter);
      key_length = -1;
   }

   switch (bson_iter_type_unsafe (iter)) {
   case BSON_TYPE_EOD:
      return false;
   case BSON_TYPE_DOUBLE:
      ret = bson_append_double (bson, key, key_length, bson_iter_double (iter));
      break;
   case BSON_TYPE_UTF8:
      {
         uint32_t len = 0;
         const char *str;

         str = bson_iter_utf8 (iter, &len);
         ret = bson_append_utf8 (bson, key, key_length, str, len);
      }
      break;
   case BSON_TYPE_DOCUMENT:
      {
         const uint8_t *buf = NULL;
         uint32_t len = 0;
         bson_t doc;

         bson_iter_document (iter, &len, &buf);

         if (bson_init_static (&doc, buf, len)) {
            ret = bson_append_document (bson, key, key_length, &doc);
            bson_destroy (&doc);
         }
      }
      break;
   case BSON_TYPE_ARRAY:
      {
         const uint8_t *buf = NULL;
         uint32_t len = 0;
         bson_t doc;

         bson_iter_array (iter, &len, &buf);

         if (bson_init_static (&doc, buf, len)) {
            ret = bson_append_array (bson, key, key_length, &doc);
            bson_destroy (&doc);
         }
      }
      break;
   case BSON_TYPE_BINARY:
      {
         const uint8_t *binary = NULL;
         bson_subtype_t subtype = BSON_SUBTYPE_BINARY;
         uint32_t len = 0;

         bson_iter_binary (iter, &subtype, &len, &binary);
         ret = bson_append_binary (bson, key, key_length,
                                   subtype, binary, len);
      }
      break;
   case BSON_TYPE_UNDEFINED:
      ret = bson_append_undefined (bson, key, key_length);
      break;
   case BSON_TYPE_OID:
      ret = bson_append_oid (bson, key, key_length, bson_iter_oid (iter));
      break;
   case BSON_TYPE_BOOL:
      ret = bson_append_bool (bson, key, key_length, bson_iter_bool (iter));
      break;
   case BSON_TYPE_DATE_TIME:
      ret = bson_append_date_time (bson, key, key_length,
                                   bson_iter_date_time (iter));
      break;
   case BSON_TYPE_NULL:
      ret = bson_append_null (bson, key, key_length);
      break;
   case BSON_TYPE_REGEX:
      {
         const char *regex;
         const char *options;

         regex = bson_iter_regex (iter, &options);
         ret = bson_append_regex (bson, key, key_length, regex, options);
      }
      break;
   case BSON_TYPE_DBPOINTER:
      {
         const bson_oid_t *oid;
         uint32_t len;
         const char *collection;

         bson_iter_dbpointer (iter, &len, &collection, &oid);
         ret = bson_append_dbpointer (bson, key, key_length, collection, oid);
      }
      break;
   case BSON_TYPE_CODE:
      {
         uint32_t len;
         const char *code;

         code = bson_iter_code (iter, &len);
         ret = bson_append_code (bson, key, key_length, code);
      }
      break;
   case BSON_TYPE_SYMBOL:
      {
         uint32_t len;
         const char *symbol;

         symbol = bson_iter_symbol (iter, &len);
         ret = bson_append_symbol (bson, key, key_length, symbol, len);
      }
      break;
   case BSON_TYPE_CODEWSCOPE:
      {
         const uint8_t *scope = NULL;
         uint32_t scope_len = 0;
         uint32_t len = 0;
         const char *javascript = NULL;
         bson_t doc;

         javascript = bson_iter_codewscope (iter, &len, &scope_len, &scope);

         if (bson_init_static (&doc, scope, scope_len)) {
            ret = bson_append_code_with_scope (bson, key, key_length,
                                               javascript, &doc);
            bson_destroy (&doc);
         }
      }
      break;
   case BSON_TYPE_INT32:
      ret = bson_append_int32 (bson, key, key_length, bson_iter_int32 (iter));
      break;
   case BSON_TYPE_TIMESTAMP:
      {
         uint32_t ts;
         uint32_t inc;

         bson_iter_timestamp (iter, &ts, &inc);
         ret = bson_append_timestamp (bson, key, key_length, ts, inc);
      }
      break;
   case BSON_TYPE_INT64:
      ret = bson_append_int64 (bson, key, key_length, bson_iter_int64 (iter));
      break;
   case BSON_TYPE_MAXKEY:
      ret = bson_append_maxkey (bson, key, key_length);
      break;
   case BSON_TYPE_MINKEY:
      ret = bson_append_minkey (bson, key, key_length);
      break;
   default:
      break;
   }

   return ret;
}


bool
bson_append_maxkey (bson_t     *bson,
                    const char *key,
                    int         key_length)
{
   static const uint8_t type = BSON_TYPE_MAXKEY;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   return _bson_append (bson, 3,
                        (1 + key_length + 1),
                        1, &type,
                        key_length, key,
                        1, &gZero);
}


bool
bson_append_minkey (bson_t     *bson,
                    const char *key,
                    int         key_length)
{
   static const uint8_t type = BSON_TYPE_MINKEY;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   return _bson_append (bson, 3,
                        (1 + key_length + 1),
                        1, &type,
                        key_length, key,
                        1, &gZero);
}


bool
bson_append_null (bson_t     *bson,
                  const char *key,
                  int         key_length)
{
   static const uint8_t type = BSON_TYPE_NULL;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   return _bson_append (bson, 3,
                        (1 + key_length + 1),
                        1, &type,
                        key_length, key,
                        1, &gZero);
}


bool
bson_append_oid (bson_t           *bson,
                 const char       *key,
                 int               key_length,
                 const bson_oid_t *value)
{
   static const uint8_t type = BSON_TYPE_OID;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (value);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   return _bson_append (bson, 4,
                        (1 + key_length + 1 + 12),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        12, value);
}


bool
bson_append_regex (bson_t     *bson,
                   const char *key,
                   int         key_length,
                   const char *regex,
                   const char *options)
{
   static const uint8_t type = BSON_TYPE_REGEX;
   uint32_t regex_len;
   uint32_t options_len;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   if (!regex) {
      regex = "";
   }

   if (!options) {
      options = "";
   }

   regex_len = (int)strlen (regex) + 1;
   options_len = (int)strlen (options) + 1;

   return _bson_append (bson, 5,
                        (1 + key_length + 1 + regex_len + options_len),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        regex_len, regex,
                        options_len, options);
}


bool
bson_append_utf8 (bson_t     *bson,
                  const char *key,
                  int         key_length,
                  const char *value,
                  int         length)
{
   static const uint8_t type = BSON_TYPE_UTF8;
   uint32_t length_le;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (BSON_UNLIKELY (!value)) {
      return bson_append_null (bson, key, key_length);
   }

   if (BSON_UNLIKELY (key_length < 0)) {
      key_length = (int)strlen (key);
   }

   if (BSON_UNLIKELY (length < 0)) {
      length = (int)strlen (value);
   }

   length_le = BSON_UINT32_TO_LE (length + 1);

   return _bson_append (bson, 6,
                        (1 + key_length + 1 + 4 + length + 1),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        4, &length_le,
                        length, value,
                        1, &gZero);
}


bool
bson_append_symbol (bson_t     *bson,
                    const char *key,
                    int         key_length,
                    const char *value,
                    int         length)
{
   static const uint8_t type = BSON_TYPE_SYMBOL;
   uint32_t length_le;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (!value) {
      return bson_append_null (bson, key, key_length);
   }

   if (key_length < 0) {
      key_length = (int)strlen (key);
   }

   if (length < 0) {
      length =(int)strlen (value);
   }

   length_le = BSON_UINT32_TO_LE (length + 1);

   return _bson_append (bson, 6,
                        (1 + key_length + 1 + 4 + length + 1),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        4, &length_le,
                        length, value,
                        1, &gZero);
}


bool
bson_append_time_t (bson_t     *bson,
                    const char *key,
                    int         key_length,
                    time_t      value)
{
#ifdef BSON_OS_WIN32
   struct timeval tv = { (long)value, 0 };
#else
   struct timeval tv = { value, 0 };
#endif

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   return bson_append_timeval (bson, key, key_length, &tv);
}


bool
bson_append_timestamp (bson_t       *bson,
                       const char   *key,
                       int           key_length,
                       uint32_t timestamp,
                       uint32_t increment)
{
   static const uint8_t type = BSON_TYPE_TIMESTAMP;
   uint64_t value;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length =(int)strlen (key);
   }

   value = ((((uint64_t)timestamp) << 32) | ((uint64_t)increment));
   value = BSON_UINT64_TO_LE (value);

   return _bson_append (bson, 4,
                        (1 + key_length + 1 + 8),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        8, &value);
}


bool
bson_append_now_utc (bson_t     *bson,
                     const char *key,
                     int         key_length)
{
   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (key_length >= -1);

   return bson_append_time_t (bson, key, key_length, time (NULL));
}


bool
bson_append_date_time (bson_t      *bson,
                       const char  *key,
                       int          key_length,
                       int64_t value)
{
   static const uint8_t type = BSON_TYPE_DATE_TIME;
   uint64_t value_le;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length =(int)strlen (key);
   }

   value_le = BSON_UINT64_TO_LE (value);

   return _bson_append (bson, 4,
                        (1 + key_length + 1 + 8),
                        1, &type,
                        key_length, key,
                        1, &gZero,
                        8, &value_le);
}


bool
bson_append_timeval (bson_t         *bson,
                     const char     *key,
                     int             key_length,
                     struct timeval *value)
{
   uint64_t unix_msec;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (value);

   unix_msec = (((uint64_t)value->tv_sec) * 1000UL) +
                                  (value->tv_usec / 1000UL);
   return bson_append_date_time (bson, key, key_length, unix_msec);
}


bool
bson_append_undefined (bson_t     *bson,
                       const char *key,
                       int         key_length)
{
   static const uint8_t type = BSON_TYPE_UNDEFINED;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (key_length < 0) {
      key_length =(int)strlen (key);
   }

   return _bson_append (bson, 3,
                        (1 + key_length + 1),
                        1, &type,
                        key_length, key,
                        1, &gZero);
}


bool
bson_append_value (bson_t             *bson,
                   const char         *key,
                   int                 key_length,
                   const bson_value_t *value)
{
   bson_t local;
   bool ret = false;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);
   BSON_ASSERT (value);

   switch (value->value_type) {
   case BSON_TYPE_DOUBLE:
      ret = bson_append_double (bson, key, key_length,
                                value->value.v_double);
      break;
   case BSON_TYPE_UTF8:
      ret = bson_append_utf8 (bson, key, key_length,
                              value->value.v_utf8.str,
                              value->value.v_utf8.len);
      break;
   case BSON_TYPE_DOCUMENT:
      if (bson_init_static (&local,
                            value->value.v_doc.data,
                            value->value.v_doc.data_len)) {
         ret = bson_append_document (bson, key, key_length, &local);
         bson_destroy (&local);
      }
      break;
   case BSON_TYPE_ARRAY:
      if (bson_init_static (&local,
                            value->value.v_doc.data,
                            value->value.v_doc.data_len)) {
         ret = bson_append_array (bson, key, key_length, &local);
         bson_destroy (&local);
      }
      break;
   case BSON_TYPE_BINARY:
      ret = bson_append_binary (bson, key, key_length,
                                value->value.v_binary.subtype,
                                value->value.v_binary.data,
                                value->value.v_binary.data_len);
      break;
   case BSON_TYPE_UNDEFINED:
      ret = bson_append_undefined (bson, key, key_length);
      break;
   case BSON_TYPE_OID:
      ret = bson_append_oid (bson, key, key_length, &value->value.v_oid);
      break;
   case BSON_TYPE_BOOL:
      ret = bson_append_bool (bson, key, key_length, value->value.v_bool);
      break;
   case BSON_TYPE_DATE_TIME:
      ret = bson_append_date_time (bson, key, key_length,
                                   value->value.v_datetime);
      break;
   case BSON_TYPE_NULL:
      ret = bson_append_null (bson, key, key_length);
      break;
   case BSON_TYPE_REGEX:
      ret = bson_append_regex (bson, key, key_length,
                               value->value.v_regex.regex,
                               value->value.v_regex.options);
      break;
   case BSON_TYPE_DBPOINTER:
      ret = bson_append_dbpointer (bson, key, key_length,
                                   value->value.v_dbpointer.collection,
                                   &value->value.v_dbpointer.oid);
      break;
   case BSON_TYPE_CODE:
      ret = bson_append_code (bson, key, key_length,
                              value->value.v_code.code);
      break;
   case BSON_TYPE_SYMBOL:
      ret = bson_append_symbol (bson, key, key_length,
                                value->value.v_symbol.symbol,
                                value->value.v_symbol.len);
      break;
   case BSON_TYPE_CODEWSCOPE:
      if (bson_init_static (&local,
                            value->value.v_codewscope.scope_data,
                            value->value.v_codewscope.scope_len)) {
         ret = bson_append_code_with_scope (bson, key, key_length,
                                            value->value.v_codewscope.code,
                                            &local);
         bson_destroy (&local);
      }
      break;
   case BSON_TYPE_INT32:
      ret = bson_append_int32 (bson, key, key_length, value->value.v_int32);
      break;
   case BSON_TYPE_TIMESTAMP:
      ret = bson_append_timestamp (bson, key, key_length,
                                   value->value.v_timestamp.timestamp,
                                   value->value.v_timestamp.increment);
      break;
   case BSON_TYPE_INT64:
      ret = bson_append_int64 (bson, key, key_length, value->value.v_int64);
      break;
   case BSON_TYPE_MAXKEY:
      ret = bson_append_maxkey (bson, key, key_length);
      break;
   case BSON_TYPE_MINKEY:
      ret = bson_append_minkey (bson, key, key_length);
      break;
   case BSON_TYPE_EOD:
   default:
      break;
   }

   return ret;
}


void
bson_init (bson_t *bson)
{
   bson_impl_inline_t *impl = (bson_impl_inline_t *)bson;

   BSON_ASSERT (bson);

   impl->flags = BSON_FLAG_INLINE | BSON_FLAG_STATIC;
   impl->len = 5;
   impl->data[0] = 5;
   impl->data[1] = 0;
   impl->data[2] = 0;
   impl->data[3] = 0;
   impl->data[4] = 0;
}


void
bson_reinit (bson_t *bson)
{
   uint8_t *data;

   BSON_ASSERT (bson);

   data = _bson_data (bson);

   bson->len = 5;

   data [0] = 5;
   data [1] = 0;
   data [2] = 0;
   data [3] = 0;
   data [4] = 0;
}


bool
bson_init_static (bson_t        *bson,
                  const uint8_t *data,
                  size_t         length)
{
   bson_impl_alloc_t *impl = (bson_impl_alloc_t *)bson;
   uint32_t len_le;

   BSON_ASSERT (bson);
   BSON_ASSERT (data);

   if ((length < 5) || (length > INT_MAX)) {
      return false;
   }

   memcpy (&len_le, data, sizeof (len_le));

   if ((size_t)BSON_UINT32_FROM_LE (len_le) != length) {
      return false;
   }

   if (data[length - 1]) {
      return false;
   }

   impl->flags = BSON_FLAG_STATIC | BSON_FLAG_RDONLY;
   impl->len = (uint32_t)length;
   impl->parent = NULL;
   impl->depth = 0;
   impl->buf = &impl->alloc;
   impl->buflen = &impl->alloclen;
   impl->offset = 0;
   impl->alloc = (uint8_t *)data;
   impl->alloclen = length;
   impl->realloc = NULL;
   impl->realloc_func_ctx = NULL;

   return true;
}


bson_t *
bson_new (void)
{
   bson_impl_inline_t *impl;
   bson_t *bson;

   bson = bson_malloc (sizeof *bson);

   impl = (bson_impl_inline_t *)bson;
   impl->flags = BSON_FLAG_INLINE;
   impl->len = 5;
   impl->data[0] = 5;
   impl->data[1] = 0;
   impl->data[2] = 0;
   impl->data[3] = 0;
   impl->data[4] = 0;

   return bson;
}


bson_t *
bson_sized_new (size_t size)
{
   bson_impl_alloc_t *impl_a;
   bson_impl_inline_t *impl_i;
   bson_t *b;

   BSON_ASSERT (size <= INT32_MAX);

   b = bson_malloc (sizeof *b);
   impl_a = (bson_impl_alloc_t *)b;
   impl_i = (bson_impl_inline_t *)b;

   if (size <= sizeof impl_i->data) {
      bson_init (b);
      b->flags &= ~BSON_FLAG_STATIC;
   } else {
      impl_a->flags = BSON_FLAG_NONE;
      impl_a->len = 5;
      impl_a->parent = NULL;
      impl_a->depth = 0;
      impl_a->buf = &impl_a->alloc;
      impl_a->buflen = &impl_a->alloclen;
      impl_a->offset = 0;
      impl_a->alloclen = BSON_MAX (5, size);
      impl_a->alloc = bson_malloc (impl_a->alloclen);
      impl_a->alloc[0] = 5;
      impl_a->alloc[1] = 0;
      impl_a->alloc[2] = 0;
      impl_a->alloc[3] = 0;
      impl_a->alloc[4] = 0;
      impl_a->realloc = bson_realloc_ctx;
      impl_a->realloc_func_ctx = NULL;
   }

   return b;
}


bson_t *
bson_new_from_data (const uint8_t *data,
                    size_t         length)
{
   uint32_t len_le;
   bson_t *bson;

   BSON_ASSERT (data);

   if ((length < 5) || (length > INT_MAX) || data [length - 1]) {
      return NULL;
   }

   memcpy (&len_le, data, sizeof (len_le));

   if (length != (size_t)BSON_UINT32_FROM_LE (len_le)) {
      return NULL;
   }

   bson = bson_sized_new (length);
   memcpy (_bson_data (bson), data, length);
   bson->len = (uint32_t)length;

   return bson;
}


bson_t *
bson_new_from_buffer (uint8_t           **buf,
                      size_t             *buf_len,
                      bson_realloc_func   realloc_func,
                      void               *realloc_func_ctx)
{
   bson_impl_alloc_t *impl;
   uint32_t len_le;
   uint32_t length;
   bson_t *bson;

   BSON_ASSERT (buf);
   BSON_ASSERT (buf_len);

   if (!realloc_func) {
      realloc_func = bson_realloc_ctx;
   }

   bson = bson_malloc0 (sizeof *bson);
   impl = (bson_impl_alloc_t *)bson;

   if (!*buf) {
      length = 5;
      len_le = BSON_UINT32_TO_LE (length);
      *buf_len = 5;
      *buf = realloc_func (*buf, *buf_len, realloc_func_ctx);
      memcpy (*buf, &len_le, sizeof (len_le));
      (*buf) [4] = '\0';
   } else {
      if ((*buf_len < 5) || (*buf_len > INT_MAX)) {
         bson_free (bson);
         return NULL;
      }

      memcpy (&len_le, *buf, sizeof (len_le));
      length = BSON_UINT32_FROM_LE(len_le);
   }

   if ((*buf)[length - 1]) {
      bson_free (bson);
      return NULL;
   }

   impl->flags = BSON_FLAG_NO_FREE;
   impl->len = length;
   impl->buf = buf;
   impl->buflen = buf_len;
   impl->realloc = realloc_func;
   impl->realloc_func_ctx = realloc_func_ctx;

   return bson;
}


bson_t *
bson_copy (const bson_t *bson)
{
   const uint8_t *data;

   BSON_ASSERT (bson);

   data = _bson_data (bson);
   return bson_new_from_data (data, bson->len);
}


void
bson_copy_to (const bson_t *src,
              bson_t       *dst)
{
   const uint8_t *data;
   bson_impl_alloc_t *adst;
   size_t len;

   BSON_ASSERT (src);
   BSON_ASSERT (dst);

   if ((src->flags & BSON_FLAG_INLINE)) {
      memcpy (dst, src, sizeof *dst);
      dst->flags = (BSON_FLAG_STATIC | BSON_FLAG_INLINE);
      return;
   }

   data = _bson_data (src);
   len = bson_next_power_of_two ((size_t)src->len);

   adst = (bson_impl_alloc_t *)dst;
   adst->flags = BSON_FLAG_STATIC;
   adst->len = src->len;
   adst->parent = NULL;
   adst->depth = 0;
   adst->buf = &adst->alloc;
   adst->buflen = &adst->alloclen;
   adst->offset = 0;
   adst->alloc = bson_malloc (len);
   adst->alloclen = len;
   adst->realloc = bson_realloc_ctx;
   adst->realloc_func_ctx = NULL;
   memcpy (adst->alloc, data, src->len);
}


static bool
should_ignore (const char *first_exclude,
               va_list     args,
               const char *name)
{
   bool ret = false;
   const char *exclude = first_exclude;
   va_list args_copy;

   va_copy (args_copy, args);

   do {
      if (!strcmp (name, exclude)) {
         ret = true;
         break;
      }
   } while ((exclude = va_arg (args_copy, const char *)));

   va_end (args_copy);

   return ret;
}


static void
_bson_copy_to_excluding_va (const bson_t *src,
                            bson_t       *dst,
                            const char   *first_exclude,
                            va_list       args)
{
   bson_iter_t iter;

   if (bson_iter_init (&iter, src)) {
      while (bson_iter_next (&iter)) {
         if (!should_ignore (first_exclude, args, bson_iter_key (&iter))) {
            if (!bson_append_iter (dst, NULL, 0, &iter)) {
               /*
                * This should not be able to happen since we are copying
                * from within a valid bson_t.
                */
               BSON_ASSERT (false);
               return;
            }
         }
      }
   }
}


void
bson_copy_to_excluding (const bson_t *src,
                        bson_t       *dst,
                        const char   *first_exclude,
                        ...)
{
   va_list args;

   BSON_ASSERT (src);
   BSON_ASSERT (dst);
   BSON_ASSERT (first_exclude);

   bson_init (dst);

   va_start (args, first_exclude);
   _bson_copy_to_excluding_va (src, dst, first_exclude, args);
   va_end (args);
}

void
bson_copy_to_excluding_noinit (const bson_t *src,
                               bson_t       *dst,
                               const char   *first_exclude,
                               ...)
{
    va_list args;

    BSON_ASSERT (src);
    BSON_ASSERT (dst);
    BSON_ASSERT (first_exclude);

    va_start (args, first_exclude);
    _bson_copy_to_excluding_va (src, dst, first_exclude, args);
    va_end (args);
}


void
bson_destroy (bson_t *bson)
{
   BSON_ASSERT (bson);

   if (!(bson->flags &
         (BSON_FLAG_RDONLY | BSON_FLAG_INLINE | BSON_FLAG_NO_FREE))) {
      bson_free (*((bson_impl_alloc_t *)bson)->buf);
   }

   if (!(bson->flags & BSON_FLAG_STATIC)) {
      bson_free (bson);
   }
}


uint8_t *
bson_destroy_with_steal (bson_t   *bson,
                         bool      steal,
                         uint32_t *length)
{
   uint8_t *ret = NULL;

   BSON_ASSERT (bson);

   if (length) {
      *length = bson->len;
   }

   if (!steal) {
      bson_destroy (bson);
      return NULL;
   }

   if ((bson->flags & (BSON_FLAG_CHILD |
                       BSON_FLAG_IN_CHILD |
                       BSON_FLAG_RDONLY))) {
      /* Do nothing */
   } else if ((bson->flags & BSON_FLAG_INLINE)) {
      bson_impl_inline_t *inl;

      inl = (bson_impl_inline_t *)bson;
      ret = bson_malloc (bson->len);
      memcpy (ret, inl->data, bson->len);
   } else {
      bson_impl_alloc_t *alloc;

      alloc = (bson_impl_alloc_t *)bson;
      ret = *alloc->buf;
      *alloc->buf = NULL;
   }

   bson_destroy (bson);

   return ret;
}


const uint8_t *
bson_get_data (const bson_t *bson)
{
   BSON_ASSERT (bson);

   return _bson_data (bson);
}


uint32_t
bson_count_keys (const bson_t *bson)
{
   uint32_t count = 0;
   bson_iter_t iter;

   BSON_ASSERT (bson);

   if (bson_iter_init (&iter, bson)) {
      while (bson_iter_next (&iter)) {
         count++;
      }
   }

   return count;
}


bool
bson_has_field (const bson_t *bson,
                const char   *key)
{
   bson_iter_t iter;
   bson_iter_t child;

   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   if (NULL != strchr (key, '.')) {
      return (bson_iter_init (&iter, bson) &&
              bson_iter_find_descendant (&iter, key, &child));
   }

   return bson_iter_init_find (&iter, bson, key);
}


int
bson_compare (const bson_t *bson,
              const bson_t *other)
{
   const uint8_t *data1;
   const uint8_t *data2;
   size_t len1;
   size_t len2;
   int64_t ret;

   data1 = _bson_data (bson) + 4;
   len1 = bson->len - 4;

   data2 = _bson_data (other) + 4;
   len2 = other->len - 4;

   if (len1 == len2) {
      return memcmp (data1, data2, len1);
   }

   ret = memcmp (data1, data2, BSON_MIN (len1, len2));

   if (ret == 0) {
      ret = (int64_t) (len1 - len2);
   }

   return (ret < 0) ? -1 : (ret > 0);
}


bool
bson_equal (const bson_t *bson,
            const bson_t *other)
{
   return !bson_compare (bson, other);
}


static bool
_bson_as_json_visit_utf8 (const bson_iter_t *iter,
                          const char        *key,
                          size_t             v_utf8_len,
                          const char        *v_utf8,
                          void              *data)
{
   bson_json_state_t *state = data;
   char *escaped;

   escaped = bson_utf8_escape_for_json (v_utf8, v_utf8_len);

   if (escaped) {
      bson_string_append (state->str, "\"");
      bson_string_append (state->str, escaped);
      bson_string_append (state->str, "\"");
      bson_free (escaped);
      return false;
   }

   return true;
}


static bool
_bson_as_json_visit_int32 (const bson_iter_t *iter,
                           const char        *key,
                           int32_t       v_int32,
                           void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append_printf (state->str, "%" PRId32, v_int32);

   return false;
}


static bool
_bson_as_json_visit_int64 (const bson_iter_t *iter,
                           const char        *key,
                           int64_t       v_int64,
                           void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append_printf (state->str, "%" PRId64, v_int64);

   return false;
}


static bool
_bson_as_json_visit_double (const bson_iter_t *iter,
                            const char        *key,
                            double             v_double,
                            void              *data)
{
   bson_json_state_t *state = data;

#ifdef BSON_NEEDS_SET_OUTPUT_FORMAT
   unsigned int current_format = _set_output_format(_TWO_DIGIT_EXPONENT);
#endif

   bson_string_append_printf (state->str, "%.15g", v_double);

#ifdef BSON_NEEDS_SET_OUTPUT_FORMAT
   _set_output_format(current_format);
#endif

   return false;
}


static bool
_bson_as_json_visit_undefined (const bson_iter_t *iter,
                               const char        *key,
                               void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append (state->str, "{ \"$undefined\" : true }");

   return false;
}


static bool
_bson_as_json_visit_null (const bson_iter_t *iter,
                          const char        *key,
                          void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append (state->str, "null");

   return false;
}


static bool
_bson_as_json_visit_oid (const bson_iter_t *iter,
                         const char        *key,
                         const bson_oid_t  *oid,
                         void              *data)
{
   bson_json_state_t *state = data;
   char str[25];

   bson_oid_to_string (oid, str);
   bson_string_append (state->str, "{ \"$oid\" : \"");
   bson_string_append (state->str, str);
   bson_string_append (state->str, "\" }");

   return false;
}


static bool
_bson_as_json_visit_binary (const bson_iter_t  *iter,
                            const char         *key,
                            bson_subtype_t      v_subtype,
                            size_t              v_binary_len,
                            const uint8_t *v_binary,
                            void               *data)
{
   bson_json_state_t *state = data;
   size_t b64_len;
   char *b64;

   b64_len = (v_binary_len / 3 + 1) * 4 + 1;
   b64 = bson_malloc0 (b64_len);
   b64_ntop (v_binary, v_binary_len, b64, b64_len);

   bson_string_append (state->str, "{ \"$type\" : \"");
   bson_string_append_printf (state->str, "%02x", v_subtype);
   bson_string_append (state->str, "\", \"$binary\" : \"");
   bson_string_append (state->str, b64);
   bson_string_append (state->str, "\" }");
   bson_free (b64);

   return false;
}


static bool
_bson_as_json_visit_bool (const bson_iter_t *iter,
                          const char        *key,
                          bool        v_bool,
                          void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append (state->str, v_bool ? "true" : "false");

   return false;
}


static bool
_bson_as_json_visit_date_time (const bson_iter_t *iter,
                               const char        *key,
                               int64_t       msec_since_epoch,
                               void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append (state->str, "{ \"$date\" : ");
   bson_string_append_printf (state->str, "%" PRId64, msec_since_epoch);
   bson_string_append (state->str, " }");

   return false;
}


static bool
_bson_as_json_visit_regex (const bson_iter_t *iter,
                           const char        *key,
                           const char        *v_regex,
                           const char        *v_options,
                           void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append (state->str, "{ \"$regex\" : \"");
   bson_string_append (state->str, v_regex);
   bson_string_append (state->str, "\", \"$options\" : \"");
   bson_string_append (state->str, v_options);
   bson_string_append (state->str, "\" }");

   return false;
}


static bool
_bson_as_json_visit_timestamp (const bson_iter_t *iter,
                               const char        *key,
                               uint32_t      v_timestamp,
                               uint32_t      v_increment,
                               void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append (state->str, "{ \"$timestamp\" : { \"t\" : ");
   bson_string_append_printf (state->str, "%u", v_timestamp);
   bson_string_append (state->str, ", \"i\" : ");
   bson_string_append_printf (state->str, "%u", v_increment);
   bson_string_append (state->str, " } }");

   return false;
}


static bool
_bson_as_json_visit_dbpointer (const bson_iter_t *iter,
                               const char        *key,
                               size_t             v_collection_len,
                               const char        *v_collection,
                               const bson_oid_t  *v_oid,
                               void              *data)
{
   bson_json_state_t *state = data;
   char str[25];

   bson_string_append (state->str, "{ \"$ref\" : \"");
   bson_string_append (state->str, v_collection);
   bson_string_append (state->str, "\"");

   if (v_oid) {
      bson_oid_to_string (v_oid, str);
      bson_string_append (state->str, ", \"$id\" : \"");
      bson_string_append (state->str, str);
      bson_string_append (state->str, "\"");
   }

   bson_string_append (state->str, " }");

   return false;
}


static bool
_bson_as_json_visit_minkey (const bson_iter_t *iter,
                            const char        *key,
                            void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append (state->str, "{ \"$minKey\" : 1 }");

   return false;
}


static bool
_bson_as_json_visit_maxkey (const bson_iter_t *iter,
                            const char        *key,
                            void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append (state->str, "{ \"$maxKey\" : 1 }");

   return false;
}




static bool
_bson_as_json_visit_before (const bson_iter_t *iter,
                            const char        *key,
                            void              *data)
{
   bson_json_state_t *state = data;
   char *escaped;

   if (state->count) {
      bson_string_append (state->str, ", ");
   }

   if (state->keys) {
      escaped = bson_utf8_escape_for_json (key, -1);
      if (escaped) {
         bson_string_append (state->str, "\"");
         bson_string_append (state->str, escaped);
         bson_string_append (state->str, "\" : ");
         bson_free (escaped);
      } else {
         return true;
      }
   }

   state->count++;

   return false;
}


static bool
_bson_as_json_visit_code (const bson_iter_t *iter,
                          const char        *key,
                          size_t             v_code_len,
                          const char        *v_code,
                          void              *data)
{
   bson_json_state_t *state = data;
   char *escaped;

   escaped = bson_utf8_escape_for_json (v_code, v_code_len);

   if (escaped) {
      bson_string_append (state->str, "\"");
      bson_string_append (state->str, escaped);
      bson_string_append (state->str, "\"");
      bson_free (escaped);
      return false;
   }

   return true;
}


static bool
_bson_as_json_visit_symbol (const bson_iter_t *iter,
                            const char        *key,
                            size_t             v_symbol_len,
                            const char        *v_symbol,
                            void              *data)
{
   bson_json_state_t *state = data;

   bson_string_append (state->str, "\"");
   bson_string_append (state->str, v_symbol);
   bson_string_append (state->str, "\"");

   return false;
}


static bool
_bson_as_json_visit_codewscope (const bson_iter_t *iter,
                                const char        *key,
                                size_t             v_code_len,
                                const char        *v_code,
                                const bson_t      *v_scope,
                                void              *data)
{
   bson_json_state_t *state = data;
   char *escaped;

   escaped = bson_utf8_escape_for_json (v_code, v_code_len);

   if (escaped) {
      bson_string_append (state->str, "\"");
      bson_string_append (state->str, escaped);
      bson_string_append (state->str, "\"");
      bson_free (escaped);
      return false;
   }

   return true;
}


static const bson_visitor_t bson_as_json_visitors = {
   _bson_as_json_visit_before,
   NULL, /* visit_after */
   NULL, /* visit_corrupt */
   _bson_as_json_visit_double,
   _bson_as_json_visit_utf8,
   _bson_as_json_visit_document,
   _bson_as_json_visit_array,
   _bson_as_json_visit_binary,
   _bson_as_json_visit_undefined,
   _bson_as_json_visit_oid,
   _bson_as_json_visit_bool,
   _bson_as_json_visit_date_time,
   _bson_as_json_visit_null,
   _bson_as_json_visit_regex,
   _bson_as_json_visit_dbpointer,
   _bson_as_json_visit_code,
   _bson_as_json_visit_symbol,
   _bson_as_json_visit_codewscope,
   _bson_as_json_visit_int32,
   _bson_as_json_visit_timestamp,
   _bson_as_json_visit_int64,
   _bson_as_json_visit_maxkey,
   _bson_as_json_visit_minkey,
};


static bool
_bson_as_json_visit_document (const bson_iter_t *iter,
                              const char        *key,
                              const bson_t      *v_document,
                              void              *data)
{
   bson_json_state_t *state = data;
   bson_json_state_t child_state = { 0, true };
   bson_iter_t child;

   if (state->depth >= BSON_MAX_RECURSION) {
      bson_string_append (state->str, "{ ... }");
      return false;
   }

   if (bson_iter_init (&child, v_document)) {
      child_state.str = bson_string_new ("{ ");
      child_state.depth = state->depth + 1;
      bson_iter_visit_all (&child, &bson_as_json_visitors, &child_state);
      bson_string_append (child_state.str, " }");
      bson_string_append (state->str, child_state.str->str);
      bson_string_free (child_state.str, true);
   }

   return false;
}


static bool
_bson_as_json_visit_array (const bson_iter_t *iter,
                           const char        *key,
                           const bson_t      *v_array,
                           void              *data)
{
   bson_json_state_t *state = data;
   bson_json_state_t child_state = { 0, false };
   bson_iter_t child;

   if (state->depth >= BSON_MAX_RECURSION) {
      bson_string_append (state->str, "{ ... }");
      return false;
   }

   if (bson_iter_init (&child, v_array)) {
      child_state.str = bson_string_new ("[ ");
      child_state.depth = state->depth + 1;
      bson_iter_visit_all (&child, &bson_as_json_visitors, &child_state);
      bson_string_append (child_state.str, " ]");
      bson_string_append (state->str, child_state.str->str);
      bson_string_free (child_state.str, true);
   }

   return false;
}


char *
bson_as_json (const bson_t *bson,
              size_t       *length)
{
   bson_json_state_t state;
   bson_iter_t iter;

   BSON_ASSERT (bson);

   if (length) {
      *length = 0;
   }

   if (bson_empty0 (bson)) {
      if (length) {
         *length = 3;
      }

      return bson_strdup ("{ }");
   }

   if (!bson_iter_init (&iter, bson)) {
      return NULL;
   }

   state.count = 0;
   state.keys = true;
   state.str = bson_string_new ("{ ");
   state.depth = 0;

   if (bson_iter_visit_all (&iter, &bson_as_json_visitors, &state) ||
       iter.err_off) {
      /*
       * We were prematurely exited due to corruption or failed visitor.
       */
      bson_string_free (state.str, true);
      if (length) {
         *length = 0;
      }
      return NULL;
   }

   bson_string_append (state.str, " }");

   if (length) {
      *length = state.str->len;
   }

   return bson_string_free (state.str, false);
}


char *
bson_array_as_json (const bson_t *bson,
                    size_t       *length)
{
   bson_json_state_t state;
   bson_iter_t iter;

   BSON_ASSERT (bson);

   if (length) {
      *length = 0;
   }

   if (bson_empty0 (bson)) {
      if (length) {
         *length = 3;
      }

      return bson_strdup ("[ ]");
   }

   if (!bson_iter_init (&iter, bson)) {
      return NULL;
   }

   state.count = 0;
   state.keys = false;
   state.str = bson_string_new ("[ ");
   state.depth = 0;
   bson_iter_visit_all (&iter, &bson_as_json_visitors, &state);

   if (bson_iter_visit_all (&iter, &bson_as_json_visitors, &state) ||
       iter.err_off) {
      /*
       * We were prematurely exited due to corruption or failed visitor.
       */
      bson_string_free (state.str, true);
      if (length) {
         *length = 0;
      }
      return NULL;
   }

   bson_string_append (state.str, " ]");

   if (length) {
      *length = state.str->len;
   }

   return bson_string_free (state.str, false);
}


static bool
_bson_iter_validate_utf8 (const bson_iter_t *iter,
                          const char        *key,
                          size_t             v_utf8_len,
                          const char        *v_utf8,
                          void              *data)
{
   bson_validate_state_t *state = data;
   bool allow_null;

   if ((state->flags & BSON_VALIDATE_UTF8)) {
      allow_null = !!(state->flags & BSON_VALIDATE_UTF8_ALLOW_NULL);

      if (!bson_utf8_validate (v_utf8, v_utf8_len, allow_null)) {
         state->err_offset = iter->off;
         return true;
      }
   }

   if ((state->flags & BSON_VALIDATE_DOLLAR_KEYS)) {
      if (state->phase == BSON_VALIDATE_PHASE_LF_REF_UTF8) {
         state->phase = BSON_VALIDATE_PHASE_LF_ID_KEY;
      } else if (state->phase == BSON_VALIDATE_PHASE_LF_DB_UTF8) {
         state->phase = BSON_VALIDATE_PHASE_NOT_DBREF;
      }
   }

   return false;
}


static void
_bson_iter_validate_corrupt (const bson_iter_t *iter,
                             void              *data)
{
   bson_validate_state_t *state = data;

   state->err_offset = iter->err_off;
}


static bool
_bson_iter_validate_before (const bson_iter_t *iter,
                            const char        *key,
                            void              *data)
{
   bson_validate_state_t *state = data;

   if ((state->flags & BSON_VALIDATE_DOLLAR_KEYS)) {
      if (key[0] == '$') {
         if (state->phase == BSON_VALIDATE_PHASE_LF_REF_KEY &&
             strcmp (key, "$ref") == 0) {
            state->phase = BSON_VALIDATE_PHASE_LF_REF_UTF8;
         } else if (state->phase == BSON_VALIDATE_PHASE_LF_ID_KEY &&
                    strcmp (key, "$id") == 0) {
            state->phase = BSON_VALIDATE_PHASE_LF_DB_KEY;
         } else if (state->phase == BSON_VALIDATE_PHASE_LF_DB_KEY &&
                    strcmp (key, "$db") == 0) {
            state->phase = BSON_VALIDATE_PHASE_LF_DB_UTF8;
         } else {
            state->err_offset = iter->off;
            return true;
         }
      } else if (state->phase == BSON_VALIDATE_PHASE_LF_ID_KEY ||
                 state->phase == BSON_VALIDATE_PHASE_LF_REF_UTF8 ||
                 state->phase == BSON_VALIDATE_PHASE_LF_DB_UTF8) {
         state->err_offset = iter->off;
         return true;
      } else {
         state->phase = BSON_VALIDATE_PHASE_NOT_DBREF;
      }
   }

   if ((state->flags & BSON_VALIDATE_DOT_KEYS)) {
      if (strstr (key, ".")) {
         state->err_offset = iter->off;
         return true;
      }
   }

   return false;
}


static bool
_bson_iter_validate_codewscope (const bson_iter_t *iter,
                                const char        *key,
                                size_t             v_code_len,
                                const char        *v_code,
                                const bson_t      *v_scope,
                                void              *data)
{
   bson_validate_state_t *state = data;
   size_t offset;

   if (!bson_validate (v_scope, state->flags, &offset)) {
      state->err_offset = iter->off + offset;
      return false;
   }

   return true;
}


static bool
_bson_iter_validate_document (const bson_iter_t *iter,
                              const char        *key,
                              const bson_t      *v_document,
                              void              *data);


static const bson_visitor_t bson_validate_funcs = {
   _bson_iter_validate_before,
   NULL, /* visit_after */
   _bson_iter_validate_corrupt,
   NULL, /* visit_double */
   _bson_iter_validate_utf8,
   _bson_iter_validate_document,
   _bson_iter_validate_document, /* visit_array */
   NULL, /* visit_binary */
   NULL, /* visit_undefined */
   NULL, /* visit_oid */
   NULL, /* visit_bool */
   NULL, /* visit_date_time */
   NULL, /* visit_null */
   NULL, /* visit_regex */
   NULL, /* visit_dbpoint */
   NULL, /* visit_code */
   NULL, /* visit_symbol */
   _bson_iter_validate_codewscope,
};


static bool
_bson_iter_validate_document (const bson_iter_t *iter,
                              const char        *key,
                              const bson_t      *v_document,
                              void              *data)
{
   bson_validate_state_t *state = data;
   bson_iter_t child;
   bson_validate_phase_t phase = state->phase;

   if (!bson_iter_init (&child, v_document)) {
      state->err_offset = iter->off;
      return true;
   }

   if (state->phase == BSON_VALIDATE_PHASE_START) {
      state->phase = BSON_VALIDATE_PHASE_TOP;
   } else {
      state->phase = BSON_VALIDATE_PHASE_LF_REF_KEY;
   }

   bson_iter_visit_all (&child, &bson_validate_funcs, state);

   if (state->phase == BSON_VALIDATE_PHASE_LF_ID_KEY ||
       state->phase == BSON_VALIDATE_PHASE_LF_REF_UTF8 ||
       state->phase == BSON_VALIDATE_PHASE_LF_DB_UTF8) {
       state->err_offset = iter->off;
       return true;
   }

   state->phase = phase;

   return false;
}


bool
bson_validate (const bson_t         *bson,
               bson_validate_flags_t flags,
               size_t               *offset)
{
   bson_validate_state_t state = { flags, -1, BSON_VALIDATE_PHASE_START };
   bson_iter_t iter;

   if (!bson_iter_init (&iter, bson)) {
      state.err_offset = 0;
      goto failure;
   }

   _bson_iter_validate_document (&iter, NULL, bson, &state);

failure:

   if (offset) {
      *offset = state.err_offset;
   }

   return state.err_offset < 0;
}


bool
bson_concat (bson_t       *dst,
             const bson_t *src)
{
   BSON_ASSERT (dst);
   BSON_ASSERT (src);

   if (!bson_empty (src)) {
      return _bson_append (dst, 1, src->len - 5,
                           src->len - 5, _bson_data (src) + 4);
   }

   return true;
}
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h>
#include <stdarg.h>

// #include "bson-compat.h"

// #include "bson-config.h"

// #include "bson-error.h"

// #include "bson-memory.h"

// #include "bson-string.h"

// #include "bson-types.h"



/*
 *--------------------------------------------------------------------------
 *
 * bson_set_error --
 *
 *       Initializes @error using the parameters specified.
 *
 *       @domain is an application specific error domain which should
 *       describe which module initiated the error. Think of this as the
 *       exception type.
 *
 *       @code is the @domain specific error code.
 *
 *       @format is used to generate the format string. It uses vsnprintf()
 *       internally so the format should match what you would use there.
 *
 * Parameters:
 *       @error: A #bson_error_t.
 *       @domain: The error domain.
 *       @code: The error code.
 *       @format: A printf style format string.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       @error is initialized.
 *
 *--------------------------------------------------------------------------
 */

void
bson_set_error (bson_error_t *error,  /* OUT */
                uint32_t      domain, /* IN */
                uint32_t      code,   /* IN */
                const char   *format, /* IN */
                ...)                  /* IN */
{
   va_list args;

   if (error) {
      error->domain = domain;
      error->code = code;

      va_start (args, format);
      bson_vsnprintf (error->message, sizeof error->message, format, args);
      va_end (args);

      error->message[sizeof error->message - 1] = '\0';
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_strerror_r --
 *
 *       This is a reentrant safe macro for strerror.
 *
 *       The resulting string may be stored in @buf.
 *
 * Returns:
 *       A pointer to a static string or @buf.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

char *
bson_strerror_r (int     err_code,  /* IN */
                 char   *buf,       /* IN */
                 size_t  buflen)    /* IN */
{
   static const char *unknown_msg = "Unknown error";
   char *ret = NULL;

#if defined(__GNUC__) && defined(_GNU_SOURCE)
   ret = strerror_r (err_code, buf, buflen);
#elif defined(_WIN32)
   if (strerror_s (buf, buflen, err_code) != 0) {
      ret = buf;
   }
#else /* XSI strerror_r */
   if (strerror_r (err_code, buf, buflen) == 0) {
      ret = buf;
   }
#endif

   if (!ret) {
      bson_strncpy (buf, unknown_msg, buflen);
      ret = buf;
   }

   return ret;
}

/*
 * Copyright 2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// #include "bson.h"

// #include "bson-config.h"

// #include "bson-json.h"

// #include "bson-iso8601-private.h"

// #include "b64_pton.h"
/*
 * Copyright (c) 1996, 1998 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*
 * Portions Copyright (c) 1995 by International Business Machines, Inc.
 *
 * International Business Machines, Inc. (hereinafter called IBM) grants
 * permission under its copyrights to use, copy, modify, and distribute this
 * Software with or without fee, provided that the above copyright notice and
 * all paragraphs of this notice appear in all copies, and that the name of IBM
 * not be used in connection with the marketing of any product incorporating
 * the Software or modifications thereof, without specific, written prior
 * permission.
 *
 * To the extent it has a right to do so, IBM grants an immunity from suit
 * under its patents, if any, for the use, sale or manufacture of products to
 * the extent that such products are used for performing Domain Name System
 * dynamic updates in TCP/IP networks by means of the Software.  No immunity is
 * granted for any product per se or for any other function of any product.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", AND IBM DISCLAIMS ALL WARRANTIES,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL,
 * DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE, EVEN
 * IF IBM IS APPRISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */

// #include "bson-compat.h"


//#define Assert(Cond) if (!(Cond)) abort()

//static const char Base64[] =
//	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//static const char Pad64 = '=';

/* (From RFC1521 and draft-ietf-dnssec-secext-03.txt)
   The following encoding technique is taken from RFC 1521 by Borenstein
   and Freed.  It is reproduced here in a slightly edited form for
   convenience.

   A 65-character subset of US-ASCII is used, enabling 6 bits to be
   represented per printable character. (The extra 65th character, "=",
   is used to signify a special processing function.)

   The encoding process represents 24-bit groups of input bits as output
   strings of 4 encoded characters. Proceeding from left to right, a
   24-bit input group is formed by concatenating 3 8-bit input groups.
   These 24 bits are then treated as 4 concatenated 6-bit groups, each
   of which is translated into a single digit in the base64 alphabet.

   Each 6-bit group is used as an index into an array of 64 printable
   characters. The character referenced by the index is placed in the
   output string.

                         Table 1: The Base64 Alphabet

      Value Encoding  Value Encoding  Value Encoding  Value Encoding
          0 A            17 R            34 i            51 z
          1 B            18 S            35 j            52 0
          2 C            19 T            36 k            53 1
          3 D            20 U            37 l            54 2
          4 E            21 V            38 m            55 3
          5 F            22 W            39 n            56 4
          6 G            23 X            40 o            57 5
          7 H            24 Y            41 p            58 6
          8 I            25 Z            42 q            59 7
          9 J            26 a            43 r            60 8
         10 K            27 b            44 s            61 9
         11 L            28 c            45 t            62 +
         12 M            29 d            46 u            63 /
         13 N            30 e            47 v
         14 O            31 f            48 w         (pad) =
         15 P            32 g            49 x
         16 Q            33 h            50 y

   Special processing is performed if fewer than 24 bits are available
   at the end of the data being encoded.  A full encoding quantum is
   always completed at the end of a quantity.  When fewer than 24 input
   bits are available in an input group, zero bits are added (on the
   right) to form an integral number of 6-bit groups.  Padding at the
   end of the data is performed using the '=' character.

   Since all base64 input is an integral number of octets, only the
   following cases can arise:

       (1) the final quantum of encoding input is an integral
           multiple of 24 bits; here, the final unit of encoded
	   output will be an integral multiple of 4 characters
	   with no "=" padding,
       (2) the final quantum of encoding input is exactly 8 bits;
           here, the final unit of encoded output will be two
	   characters followed by two "=" padding characters, or
       (3) the final quantum of encoding input is exactly 16 bits;
           here, the final unit of encoded output will be three
	   characters followed by one "=" padding character.
   */

/* skips all whitespace anywhere.
   converts characters, four at a time, starting at (or after)
   src from base - 64 numbers into three 8 bit bytes in the target area.
   it returns the number of data bytes stored at the target, or -1 on error.
 */

static int b64rmap_initialized = 0;
static uint8_t b64rmap[256];

static const uint8_t b64rmap_special = 0xf0;
static const uint8_t b64rmap_end = 0xfd;
static const uint8_t b64rmap_space = 0xfe;
static const uint8_t b64rmap_invalid = 0xff;

/**
 * Initializing the reverse map is not thread safe.
 * Which is fine for NSD. For now...
 **/
static void
b64_initialize_rmap ()
{
	int i;
	unsigned char ch;

	/* Null: end of string, stop parsing */
	b64rmap[0] = b64rmap_end;

	for (i = 1; i < 256; ++i) {
		ch = (unsigned char)i;
		/* Whitespaces */
		if (isspace(ch))
			b64rmap[i] = b64rmap_space;
		/* Padding: stop parsing */
		else if (ch == Pad64)
			b64rmap[i] = b64rmap_end;
		/* Non-base64 char */
		else
			b64rmap[i] = b64rmap_invalid;
	}

	/* Fill reverse mapping for base64 chars */
	for (i = 0; Base64[i] != '\0'; ++i)
		b64rmap[(uint8_t)Base64[i]] = i;

	b64rmap_initialized = 1;
}

static int
b64_pton_do(char const *src, uint8_t *target, size_t targsize)
{
	int tarindex, state, ch;
	uint8_t ofs;

	state = 0;
	tarindex = 0;

	while (1)
	{
		ch = *src++;
		ofs = b64rmap[ch];

		if (ofs >= b64rmap_special) {
			/* Ignore whitespaces */
			if (ofs == b64rmap_space)
				continue;
			/* End of base64 characters */
			if (ofs == b64rmap_end)
				break;
			/* A non-base64 character. */
			return (-1);
		}

		switch (state) {
		case 0:
			if ((size_t)tarindex >= targsize)
				return (-1);
			target[tarindex] = ofs << 2;
			state = 1;
			break;
		case 1:
			if ((size_t)tarindex + 1 >= targsize)
				return (-1);
			target[tarindex]   |=  ofs >> 4;
			target[tarindex+1]  = (ofs & 0x0f)
						<< 4 ;
			tarindex++;
			state = 2;
			break;
		case 2:
			if ((size_t)tarindex + 1 >= targsize)
				return (-1);
			target[tarindex]   |=  ofs >> 2;
			target[tarindex+1]  = (ofs & 0x03)
						<< 6;
			tarindex++;
			state = 3;
			break;
		case 3:
			if ((size_t)tarindex >= targsize)
				return (-1);
			target[tarindex] |= ofs;
			tarindex++;
			state = 0;
			break;
		default:
			abort();
		}
	}

	/*
	 * We are done decoding Base-64 chars.  Let's see if we ended
	 * on a byte boundary, and/or with erroneous trailing characters.
	 */

	if (ch == Pad64) {		/* We got a pad char. */
		ch = *src++;		/* Skip it, get next. */
		switch (state) {
		case 0:		/* Invalid = in first position */
		case 1:		/* Invalid = in second position */
			return (-1);

		case 2:		/* Valid, means one byte of info */
			/* Skip any number of spaces. */
			for ((void)NULL; ch != '\0'; ch = *src++)
				if (b64rmap[ch] != b64rmap_space)
					break;
			/* Make sure there is another trailing = sign. */
			if (ch != Pad64)
				return (-1);
			ch = *src++;		/* Skip the = */
			/* Fall through to "single trailing =" case. */
			/* FALLTHROUGH */

		case 3:		/* Valid, means two bytes of info */
			/*
			 * We know this char is an =.  Is there anything but
			 * whitespace after it?
			 */
			for ((void)NULL; ch != '\0'; ch = *src++)
				if (b64rmap[ch] != b64rmap_space)
					return (-1);

			/*
			 * Now make sure for cases 2 and 3 that the "extra"
			 * bits that slopped past the last full byte were
			 * zeros.  If we don't check them, they become a
			 * subliminal channel.
			 */
			if (target[tarindex] != 0)
				return (-1);
		default:
			break;
		}
	} else {
		/*
		 * We ended by seeing the end of the string.  Make sure we
		 * have no partial bytes lying around.
		 */
		if (state != 0)
			return (-1);
	}

	return (tarindex);
}


static int
b64_pton_len(char const *src)
{
	int tarindex, state, ch;
	uint8_t ofs;

	state = 0;
	tarindex = 0;

	while (1)
	{
		ch = *src++;
		ofs = b64rmap[ch];

		if (ofs >= b64rmap_special) {
			/* Ignore whitespaces */
			if (ofs == b64rmap_space)
				continue;
			/* End of base64 characters */
			if (ofs == b64rmap_end)
				break;
			/* A non-base64 character. */
			return (-1);
		}

		switch (state) {
		case 0:
			state = 1;
			break;
		case 1:
			tarindex++;
			state = 2;
			break;
		case 2:
			tarindex++;
			state = 3;
			break;
		case 3:
			tarindex++;
			state = 0;
			break;
		default:
			abort();
		}
	}

	/*
	 * We are done decoding Base-64 chars.  Let's see if we ended
	 * on a byte boundary, and/or with erroneous trailing characters.
	 */

	if (ch == Pad64) {		/* We got a pad char. */
		ch = *src++;		/* Skip it, get next. */
		switch (state) {
		case 0:		/* Invalid = in first position */
		case 1:		/* Invalid = in second position */
			return (-1);

		case 2:		/* Valid, means one byte of info */
			/* Skip any number of spaces. */
			for ((void)NULL; ch != '\0'; ch = *src++)
				if (b64rmap[ch] != b64rmap_space)
					break;
			/* Make sure there is another trailing = sign. */
			if (ch != Pad64)
				return (-1);
			ch = *src++;		/* Skip the = */
			/* Fall through to "single trailing =" case. */
			/* FALLTHROUGH */

		case 3:		/* Valid, means two bytes of info */
			/*
			 * We know this char is an =.  Is there anything but
			 * whitespace after it?
			 */
			for ((void)NULL; ch != '\0'; ch = *src++)
				if (b64rmap[ch] != b64rmap_space)
					return (-1);

		default:
			break;
		}
	} else {
		/*
		 * We ended by seeing the end of the string.  Make sure we
		 * have no partial bytes lying around.
		 */
		if (state != 0)
			return (-1);
	}

	return (tarindex);
}


static int
b64_pton(char const *src, uint8_t *target, size_t targsize)
{
	if (!b64rmap_initialized)
		b64_initialize_rmap ();

	if (target)
		return b64_pton_do (src, target, targsize);
	else
		return b64_pton_len (src);
}


// #include <yajl/yajl_parser.h>

// #include <yajl/yajl_bytestack.h>


#ifdef _WIN32
# include <io.h>
# include <share.h>
#endif


#define STACK_MAX 100
#define BSON_JSON_DEFAULT_BUF_SIZE (1 << 14)


typedef enum
{
   BSON_JSON_REGULAR,
   BSON_JSON_DONE,
   BSON_JSON_ERROR,
   BSON_JSON_IN_START_MAP,
   BSON_JSON_IN_BSON_TYPE,
   BSON_JSON_IN_BSON_TYPE_DATE_NUMBERLONG,
   BSON_JSON_IN_BSON_TYPE_DATE_ENDMAP,
   BSON_JSON_IN_BSON_TYPE_TIMESTAMP_STARTMAP,
   BSON_JSON_IN_BSON_TYPE_TIMESTAMP_VALUES,
   BSON_JSON_IN_BSON_TYPE_TIMESTAMP_ENDMAP,
} bson_json_read_state_t;


typedef enum
{
   BSON_JSON_LF_REGEX,
   BSON_JSON_LF_OPTIONS,
   BSON_JSON_LF_OID,
   BSON_JSON_LF_BINARY,
   BSON_JSON_LF_TYPE,
   BSON_JSON_LF_DATE,
   BSON_JSON_LF_TIMESTAMP_T,
   BSON_JSON_LF_TIMESTAMP_I,
   BSON_JSON_LF_UNDEFINED,
   BSON_JSON_LF_MINKEY,
   BSON_JSON_LF_MAXKEY,
   BSON_JSON_LF_INT64,
} bson_json_read_bson_state_t;


typedef struct
{
   uint8_t *buf;
   size_t   n_bytes;
   size_t   len;
} bson_json_buf_t;


typedef struct
{
   int    i;
   bool   is_array;
   bson_t bson;
} bson_json_stack_frame_t;


typedef union
{
   struct {
      bool has_regex;
      bool has_options;
   } regex;
   struct {
      bool       has_oid;
      bson_oid_t oid;
   } oid;
   struct {
      bool           has_binary;
      bool           has_subtype;
      bson_subtype_t type;
   } binary;
   struct {
      bool    has_date;
      int64_t date;
   } date;
   struct {
      bool     has_t;
      bool     has_i;
      uint32_t t;
      uint32_t i;
   } timestamp;
   struct {
      bool has_undefined;
   } undefined;
   struct {
      bool has_minkey;
   } minkey;
   struct {
      bool has_maxkey;
   } maxkey;
   struct {
      int64_t value;
   } v_int64;
} bson_json_bson_data_t;


typedef struct
{
   bson_t                      *bson;
   bson_json_stack_frame_t      stack[STACK_MAX];
   int                          n;
   const char                  *key;
   bson_json_buf_t              key_buf;
   bson_json_read_state_t       read_state;
   bson_json_read_bson_state_t  bson_state;
   bson_type_t                  bson_type;
   bson_json_buf_t              bson_type_buf [3];
   bson_json_bson_data_t        bson_type_data;
   bool                         known_bson_type;
} bson_json_reader_bson_t;


typedef struct
{
   void                 *data;
   bson_json_reader_cb   cb;
   bson_json_destroy_cb  dcb;
   uint8_t              *buf;
   size_t                buf_size;
   size_t                bytes_read;
   size_t                bytes_parsed;
   bool                  all_whitespace;
} bson_json_reader_producer_t;


struct _bson_json_reader_t
{
   bson_json_reader_producer_t  producer;
   bson_json_reader_bson_t      bson;
   yajl_handle                  yh;
   bson_error_t                *error;
};


typedef struct
{
   int fd;
   bool do_close;
} bson_json_reader_handle_fd_t;


static void *
bson_yajl_malloc_func (void   *ctx,
                       size_t  sz)
{
   return bson_malloc (sz);
}


static void
bson_yajl_free_func (void *ctx,
                     void *ptr)
{
   bson_free (ptr);
}


static void *
bson_yajl_realloc_func (void   *ctx,
                        void   *ptr,
                        size_t  sz)
{
   return bson_realloc (ptr, sz);
}


static yajl_alloc_funcs gYajlAllocFuncs = {
   bson_yajl_malloc_func,
   bson_yajl_realloc_func,
   bson_yajl_free_func,
};

#undef STACK_ELE
#undef STACK_BSON
#undef STACK_PUSH_ARRAY
#undef STACK_PUSH_DOC
#undef STACK_POP_ARRAY
#undef STACK_POP_DOC

#define STACK_ELE(_delta, _name) (bson->stack[(_delta) + bson->n]._name)
#define STACK_BSON(_delta) \
      (((_delta) + bson->n) == 0 ? bson->bson : &STACK_ELE (_delta, bson))
#define STACK_BSON_PARENT STACK_BSON (-1)
#define STACK_BSON_CHILD STACK_BSON (0)
#define STACK_I STACK_ELE (0, i)
#define STACK_IS_ARRAY STACK_ELE (0, is_array)
#define STACK_PUSH_ARRAY(statement) \
   do { \
      if (bson->n >= (STACK_MAX - 1)) { return 0; } \
      bson->n++; \
      STACK_I = 0; \
      STACK_IS_ARRAY = 1; \
      if (bson->n != 0) { \
         statement; \
      } \
   } while (0)
#define STACK_PUSH_DOC(statement) \
   do { \
      if (bson->n >= (STACK_MAX - 1)) { return 0; } \
      bson->n++; \
      STACK_IS_ARRAY = 0; \
      if (bson->n != 0) { \
         statement; \
      } \
   } while (0)
#define STACK_POP_ARRAY(statement) \
   do { \
      if (!STACK_IS_ARRAY) { return 0; } \
      if (bson->n < 0) { return 0; } \
      if (bson->n > 0) { \
         statement; \
      } \
      bson->n--; \
   } while (0)
#define STACK_POP_DOC(statement) \
   do { \
      if (STACK_IS_ARRAY) { return 0; } \
      if (bson->n < 0) { return 0; } \
      if (bson->n > 0) { \
         statement; \
      } \
      bson->n--; \
   } while (0)
#define BASIC_YAJL_CB_PREAMBLE \
   const char *key; \
   size_t len; \
   bson_json_reader_t *reader = (bson_json_reader_t *)_ctx; \
   bson_json_reader_bson_t *bson = &reader->bson; \
   _bson_json_read_fixup_key (bson); \
   key = bson->key; \
   len = bson->key_buf.len;
#define BASIC_YAJL_CB_BAIL_IF_NOT_NORMAL(_type) \
   if (bson->read_state != BSON_JSON_REGULAR) { \
      _bson_json_read_set_error (reader, "Invalid read of %s in state %d", \
                                 (_type), bson->read_state); \
      return 0; \
   } else if (! key) { \
      _bson_json_read_set_error (reader, "Invalid read of %s without key in state %d", \
                                 (_type), bson->read_state); \
      return 0; \
   }
#define HANDLE_OPTION(_key, _type, _state) \
   (len == strlen (_key) && strncmp ((const char *)val, (_key), len) == 0) { \
      if (bson->known_bson_type && bson->bson_type != (_type)) { \
         _bson_json_read_set_error (reader, \
                                    "Invalid key %s.  Looking for values for %d", \
                                    (_key), bson->bson_type); \
         return 0; \
      } \
      bson->bson_type = (_type); \
      bson->bson_state = (_state); \
   }

static bool
_bson_json_all_whitespace (const char *utf8)
{
   bool all_whitespace = true;

   if (utf8) {
      for (; *utf8; utf8 = bson_utf8_next_char (utf8)) {
         if (!isspace (bson_utf8_get_char (utf8))) {
            all_whitespace = false;
            break;
         }
      }
   }

   return all_whitespace;
}

static void
_bson_json_read_set_error (bson_json_reader_t *reader,
                           const char         *fmt,
                           ...)
   BSON_GNUC_PRINTF (2, 3);


static void
_bson_json_read_set_error (bson_json_reader_t *reader, /* IN */
                           const char         *fmt,    /* IN */
                           ...)
{
   va_list ap;

   if (reader->error) {
      reader->error->domain = BSON_ERROR_JSON;
      reader->error->code = BSON_JSON_ERROR_READ_INVALID_PARAM;
      va_start (ap, fmt);
      bson_vsnprintf (reader->error->message, sizeof reader->error->message,
                      fmt, ap);
      va_end (ap);
      reader->error->message [sizeof reader->error->message - 1] = '\0';
   }

   reader->bson.read_state = BSON_JSON_ERROR;
}


static void
_bson_json_buf_ensure (bson_json_buf_t *buf, /* IN */
                       size_t           len) /* IN */
{
   if (buf->n_bytes < len) {
      bson_free (buf->buf);

      buf->n_bytes = bson_next_power_of_two (len);
      buf->buf = bson_malloc (buf->n_bytes);
   }
}


static void
_bson_json_read_fixup_key (bson_json_reader_bson_t *bson) /* IN */
{
   if (bson->n >= 0 && STACK_IS_ARRAY) {
      _bson_json_buf_ensure (&bson->key_buf, 12);
      bson->key_buf.len = bson_uint32_to_string (STACK_I, &bson->key,
                                                 (char *)bson->key_buf.buf, 12);
      STACK_I++;
   }
}


static void
_bson_json_buf_set (bson_json_buf_t *buf,            /* IN */
                    const void       *from,          /* IN */
                    size_t            len,           /* IN */
                    bool              trailing_null) /* IN */
{
   if (trailing_null) {
      _bson_json_buf_ensure (buf, len + 1);
   } else {
      _bson_json_buf_ensure (buf, len);
   }

   memcpy (buf->buf, from, len);

   if (trailing_null) {
      buf->buf[len] = '\0';
   }

   buf->len = len;
}


static int
_bson_json_read_null (void *_ctx)
{
   BASIC_YAJL_CB_PREAMBLE;
   BASIC_YAJL_CB_BAIL_IF_NOT_NORMAL ("null");

   bson_append_null (STACK_BSON_CHILD, key, (int)len);

   return 1;
}


static int
_bson_json_read_boolean (void *_ctx, /* IN */
                         int   val)  /* IN */
{
   BASIC_YAJL_CB_PREAMBLE;

   if (bson->read_state == BSON_JSON_IN_BSON_TYPE && bson->bson_state ==
       BSON_JSON_LF_UNDEFINED) {
      bson->bson_type_data.undefined.has_undefined = true;
      return 1;
   }

   BASIC_YAJL_CB_BAIL_IF_NOT_NORMAL ("boolean");

   bson_append_bool (STACK_BSON_CHILD, key, (int)len, val);

   return 1;
}


static int
_bson_json_read_integer (void    *_ctx, /* IN */
                         int64_t  val)  /* IN */
{
   bson_json_read_state_t rs;
   bson_json_read_bson_state_t bs;

   BASIC_YAJL_CB_PREAMBLE;

   rs = bson->read_state;
   bs = bson->bson_state;

   if (rs == BSON_JSON_REGULAR) {
      BASIC_YAJL_CB_BAIL_IF_NOT_NORMAL ("integer");

      if (val <= INT32_MAX) {
         bson_append_int32 (STACK_BSON_CHILD, key, (int)len, (int)val);
      } else {
         bson_append_int64 (STACK_BSON_CHILD, key, (int)len, val);
      }
   } else if (rs == BSON_JSON_IN_BSON_TYPE || rs ==
              BSON_JSON_IN_BSON_TYPE_TIMESTAMP_VALUES) {
      switch (bs) {
      case BSON_JSON_LF_DATE:
         bson->bson_type_data.date.has_date = true;
         bson->bson_type_data.date.date = val;
         break;
      case BSON_JSON_LF_TIMESTAMP_T:
         bson->bson_type_data.timestamp.has_t = true;
         bson->bson_type_data.timestamp.t = (uint32_t)val;
         break;
      case BSON_JSON_LF_TIMESTAMP_I:
         bson->bson_type_data.timestamp.has_i = true;
         bson->bson_type_data.timestamp.i = (uint32_t)val;
         break;
      case BSON_JSON_LF_MINKEY:
         bson->bson_type_data.minkey.has_minkey = true;
         break;
      case BSON_JSON_LF_MAXKEY:
         bson->bson_type_data.maxkey.has_maxkey = true;
         break;
      case BSON_JSON_LF_REGEX:
      case BSON_JSON_LF_OPTIONS:
      case BSON_JSON_LF_OID:
      case BSON_JSON_LF_BINARY:
      case BSON_JSON_LF_TYPE:
      case BSON_JSON_LF_UNDEFINED:
      case BSON_JSON_LF_INT64:
      default:
         _bson_json_read_set_error (reader,
                                    "Invalid special type for integer read %d",
                                    bs);
         return 0;
      }
   } else {
      _bson_json_read_set_error (reader, "Invalid state for integer read %d",
                                 rs);
      return 0;
   }

   return 1;
}


static int
_bson_json_read_double (void   *_ctx, /* IN */
                        double  val)  /* IN */
{
   BASIC_YAJL_CB_PREAMBLE;
   BASIC_YAJL_CB_BAIL_IF_NOT_NORMAL ("double");

   bson_append_double (STACK_BSON_CHILD, key, (int)len, val);

   return 1;
}


static int
_bson_json_read_string (void                *_ctx, /* IN */
                        const unsigned char *val,  /* IN */
                        size_t               vlen) /* IN */
{
   bson_json_read_state_t rs;
   bson_json_read_bson_state_t bs;

   BASIC_YAJL_CB_PREAMBLE;

   rs = bson->read_state;
   bs = bson->bson_state;

   if (rs == BSON_JSON_REGULAR) {
      BASIC_YAJL_CB_BAIL_IF_NOT_NORMAL ("string");
      bson_append_utf8 (STACK_BSON_CHILD, key, (int)len, (const char *)val, (int)vlen);
   } else if (rs == BSON_JSON_IN_BSON_TYPE || rs ==
              BSON_JSON_IN_BSON_TYPE_TIMESTAMP_VALUES || rs == BSON_JSON_IN_BSON_TYPE_DATE_NUMBERLONG) {
      const char *val_w_null;
      _bson_json_buf_set (&bson->bson_type_buf[2], val, vlen, true);
      val_w_null = (const char *)bson->bson_type_buf[2].buf;

      switch (bs) {
      case BSON_JSON_LF_REGEX:
         bson->bson_type_data.regex.has_regex = true;
         _bson_json_buf_set (&bson->bson_type_buf[0], val, vlen, true);
         break;
      case BSON_JSON_LF_OPTIONS:
         bson->bson_type_data.regex.has_options = true;
         _bson_json_buf_set (&bson->bson_type_buf[1], val, vlen, true);
         break;
      case BSON_JSON_LF_OID:

         if (vlen != 24) {
            goto BAD_PARSE;
         }

         bson->bson_type_data.oid.has_oid = true;
         bson_oid_init_from_string (&bson->bson_type_data.oid.oid, val_w_null);
         break;
      case BSON_JSON_LF_TYPE:
         bson->bson_type_data.binary.has_subtype = true;

#ifdef _MSC_VER
# define SSCANF sscanf_s
#else
# define SSCANF sscanf
#endif

         if (SSCANF (val_w_null, "%02x",
                     &bson->bson_type_data.binary.type) != 1) {
            goto BAD_PARSE;
         }

#undef SSCANF

         break;
      case BSON_JSON_LF_BINARY: {
            /* TODO: error handling for pton */
            int binary_len;
            bson->bson_type_data.binary.has_binary = true;
            binary_len = b64_pton (val_w_null, NULL, 0);
            _bson_json_buf_ensure (&bson->bson_type_buf[0], binary_len + 1);
            b64_pton ((char *)bson->bson_type_buf[2].buf,
                      bson->bson_type_buf[0].buf, binary_len + 1);
            bson->bson_type_buf[0].len = binary_len;
            break;
         }
      case BSON_JSON_LF_INT64:
         {
            int64_t v64;
            char *endptr = NULL;

            errno = 0;
            v64 = bson_ascii_strtoll ((const char *)val, &endptr, 10);

            if (((v64 == INT64_MIN) || (v64 == INT64_MAX)) && (errno == ERANGE)) {
               goto BAD_PARSE;
            }

            if (endptr != ((const char *)val + vlen)) {
               goto BAD_PARSE;
            }

            if (bson->read_state == BSON_JSON_IN_BSON_TYPE) {
                bson->bson_type_data.v_int64.value = v64;
            } else if (bson->read_state == BSON_JSON_IN_BSON_TYPE_DATE_NUMBERLONG) {
                bson->bson_type_data.date.has_date = true;
                bson->bson_type_data.date.date = v64;
            } else {
                goto BAD_PARSE;
            }
         }
         break;
      case BSON_JSON_LF_DATE:
         {
            int64_t v64;

            if (!_bson_iso8601_date_parse ((char *)val, (int)vlen, &v64)) {
               _bson_json_read_set_error (reader, "Could not parse \"%s\" as a date",
                                          val_w_null);
               return 0;
            } else {
               bson->bson_type_data.date.has_date = true;
               bson->bson_type_data.date.date = v64;
            }
         }
         break;
      case BSON_JSON_LF_TIMESTAMP_T:
      case BSON_JSON_LF_TIMESTAMP_I:
      case BSON_JSON_LF_UNDEFINED:
      case BSON_JSON_LF_MINKEY:
      case BSON_JSON_LF_MAXKEY:
      default:
         goto BAD_PARSE;
      }

      return 1;

   BAD_PARSE:
      _bson_json_read_set_error (reader,
                                 "Invalid input string %s, looking for %d",
                                 val_w_null, bs);
      return 0;
   } else {
      _bson_json_read_set_error (reader, "Invalid state to look for string %d",
                                 rs);
      return 0;
   }

   return 1;
}


static int
_bson_json_read_start_map (void *_ctx) /* IN */
{
   BASIC_YAJL_CB_PREAMBLE;

   if (bson->read_state == BSON_JSON_IN_BSON_TYPE && bson->bson_state == BSON_JSON_LF_DATE) {
      bson->read_state = BSON_JSON_IN_BSON_TYPE_DATE_NUMBERLONG;
   } else if (bson->read_state == BSON_JSON_IN_BSON_TYPE_TIMESTAMP_STARTMAP) {
      bson->read_state = BSON_JSON_IN_BSON_TYPE_TIMESTAMP_VALUES;
   } else {
      bson->read_state = BSON_JSON_IN_START_MAP;
   }

   /* silence some warnings */
   (void)len;
   (void)key;

   return 1;
}


static bool
_is_known_key (const char *key, size_t len)
{
   bool ret;

#define IS_KEY(k) (len == strlen(k) && (0 == memcmp (k, key, len)))

   ret = (IS_KEY ("$regex") ||
          IS_KEY ("$options") ||
          IS_KEY ("$oid") ||
          IS_KEY ("$binary") ||
          IS_KEY ("$type") ||
          IS_KEY ("$date") ||
          IS_KEY ("$undefined") ||
          IS_KEY ("$maxKey") ||
          IS_KEY ("$minKey") ||
          IS_KEY ("$timestamp") ||
          IS_KEY ("$numberLong"));

#undef IS_KEY

   return ret;
}


static int
_bson_json_read_map_key (void          *_ctx, /* IN */
                         const uint8_t *val,  /* IN */
                         size_t         len)  /* IN */
{
   bson_json_reader_t *reader = (bson_json_reader_t *)_ctx;
   bson_json_reader_bson_t *bson = &reader->bson;

   if (bson->read_state == BSON_JSON_IN_START_MAP) {
      if (len > 0 && val[0] == '$' && _is_known_key ((const char *)val, len)) {
         bson->read_state = BSON_JSON_IN_BSON_TYPE;
         bson->bson_type = (bson_type_t) 0;
         memset (&bson->bson_type_data, 0, sizeof bson->bson_type_data);
      } else {
         bson->read_state = BSON_JSON_REGULAR;
         STACK_PUSH_DOC (bson_append_document_begin (STACK_BSON_PARENT,
                                                     bson->key,
                                                     (int)bson->key_buf.len,
                                                     STACK_BSON_CHILD));
      }
   }

   if (bson->read_state == BSON_JSON_IN_BSON_TYPE) {
      if HANDLE_OPTION ("$regex", BSON_TYPE_REGEX, BSON_JSON_LF_REGEX) else
      if HANDLE_OPTION ("$options", BSON_TYPE_REGEX, BSON_JSON_LF_OPTIONS) else
      if HANDLE_OPTION ("$oid", BSON_TYPE_OID, BSON_JSON_LF_OID) else
      if HANDLE_OPTION ("$binary", BSON_TYPE_BINARY, BSON_JSON_LF_BINARY) else
      if HANDLE_OPTION ("$type", BSON_TYPE_BINARY, BSON_JSON_LF_TYPE) else
      if HANDLE_OPTION ("$date", BSON_TYPE_DATE_TIME, BSON_JSON_LF_DATE) else
      if HANDLE_OPTION ("$undefined", BSON_TYPE_UNDEFINED,
                        BSON_JSON_LF_UNDEFINED) else
      if HANDLE_OPTION ("$minKey", BSON_TYPE_MINKEY, BSON_JSON_LF_MINKEY) else
      if HANDLE_OPTION ("$maxKey", BSON_TYPE_MAXKEY, BSON_JSON_LF_MAXKEY) else
      if HANDLE_OPTION ("$numberLong", BSON_TYPE_INT64, BSON_JSON_LF_INT64) else
      if (len == strlen ("$timestamp") &&
          memcmp (val, "$timestamp", len) == 0) {
         bson->bson_type = BSON_TYPE_TIMESTAMP;
         bson->read_state = BSON_JSON_IN_BSON_TYPE_TIMESTAMP_STARTMAP;
      } else {
         _bson_json_read_set_error (reader,
                                    "Invalid key %s.  Looking for values for %d",
                                    val, bson->bson_type);
         return 0;
      }
   } else if (bson->read_state == BSON_JSON_IN_BSON_TYPE_DATE_NUMBERLONG) {
      if HANDLE_OPTION ("$numberLong", BSON_TYPE_DATE_TIME, BSON_JSON_LF_INT64) else {
         _bson_json_read_set_error (reader,
                                    "Invalid key %s.  Looking for values for %d",
                                    val, bson->bson_type);
         return 0;
      }
   } else if (bson->read_state == BSON_JSON_IN_BSON_TYPE_TIMESTAMP_VALUES) {
      if HANDLE_OPTION ("t", BSON_TYPE_TIMESTAMP, BSON_JSON_LF_TIMESTAMP_T) else
      if HANDLE_OPTION ("i", BSON_TYPE_TIMESTAMP,
                        BSON_JSON_LF_TIMESTAMP_I) else {
         _bson_json_read_set_error (reader,
                                    "Invalid key %s.  Looking for values for %d",
                                    val, bson->bson_type);
         return 0;
      }
   } else {
      _bson_json_buf_set (&bson->key_buf, val, len, true);
      bson->key = (const char *)bson->key_buf.buf;
   }

   return 1;
}


static int
_bson_json_read_append_binary (bson_json_reader_t      *reader, /* IN */
                               bson_json_reader_bson_t *bson)   /* IN */
{
   if (!bson->bson_type_data.binary.has_binary) {
      _bson_json_read_set_error (reader,
                                 "Missing $binary after $type in BSON_TYPE_BINARY");
   } else if (!bson->bson_type_data.binary.has_subtype) {
      _bson_json_read_set_error (reader,
                                 "Missing $type after $binary in BSON_TYPE_BINARY");
   } else {
      return bson_append_binary (STACK_BSON_CHILD, bson->key, (int)bson->key_buf.len,
                                 bson->bson_type_data.binary.type,
                                 bson->bson_type_buf[0].buf,
                                 (uint32_t)bson->bson_type_buf[0].len);
   }

   return 0;
}


static int
_bson_json_read_append_regex (bson_json_reader_t      *reader, /* IN */
                              bson_json_reader_bson_t *bson)   /* IN */
{
   char *regex = NULL;
   char *options = NULL;

   if (!bson->bson_type_data.regex.has_regex) {
      _bson_json_read_set_error (reader,
                                 "Missing $regex after $options in BSON_TYPE_REGEX");
      return 0;
   }

   regex = (char *)bson->bson_type_buf[0].buf;

   if (bson->bson_type_data.regex.has_options) {
      options = (char *)bson->bson_type_buf[1].buf;
   }

   return bson_append_regex (STACK_BSON_CHILD, bson->key, (int)bson->key_buf.len,
                             regex, options);
}


static int
_bson_json_read_append_oid (bson_json_reader_t      *reader, /* IN */
                            bson_json_reader_bson_t *bson)   /* IN */
{
   return bson_append_oid (STACK_BSON_CHILD, bson->key, (int)bson->key_buf.len,
                           &bson->bson_type_data.oid.oid);
}


static int
_bson_json_read_append_date_time (bson_json_reader_t      *reader, /* IN */
                                  bson_json_reader_bson_t *bson)   /* IN */
{
   return bson_append_date_time (STACK_BSON_CHILD, bson->key, (int)bson->key_buf.len,
                                 bson->bson_type_data.date.date);
}


static int
_bson_json_read_append_timestamp (bson_json_reader_t      *reader, /* IN */
                                  bson_json_reader_bson_t *bson)   /* IN */
{
   if (!bson->bson_type_data.timestamp.has_t) {
      _bson_json_read_set_error (reader,
                                 "Missing t after $timestamp in BSON_TYPE_TIMESTAMP");
      return 0;
   }

   if (!bson->bson_type_data.timestamp.has_i) {
      _bson_json_read_set_error (reader,
                                 "Missing i after $timestamp in BSON_TYPE_TIMESTAMP");
      return 0;
   }

   return bson_append_timestamp (STACK_BSON_CHILD, bson->key, (int)bson->key_buf.len,
                                 bson->bson_type_data.timestamp.t,
                                 bson->bson_type_data.timestamp.i);
}


static int
_bson_json_read_end_map (void *_ctx) /* IN */
{
   bson_json_reader_t *reader = (bson_json_reader_t *)_ctx;
   bson_json_reader_bson_t *bson = &reader->bson;

   if (bson->read_state == BSON_JSON_IN_START_MAP) {
      bson->read_state = BSON_JSON_REGULAR;
      STACK_PUSH_DOC (bson_append_document_begin (STACK_BSON_PARENT, bson->key,
                                                  (int)bson->key_buf.len,
                                                  STACK_BSON_CHILD));
   }

   if (bson->read_state == BSON_JSON_IN_BSON_TYPE) {
      bson->read_state = BSON_JSON_REGULAR;
      switch (bson->bson_type) {
      case BSON_TYPE_REGEX:
         return _bson_json_read_append_regex (reader, bson);
      case BSON_TYPE_OID:
         return _bson_json_read_append_oid (reader, bson);
      case BSON_TYPE_BINARY:
         return _bson_json_read_append_binary (reader, bson);
      case BSON_TYPE_DATE_TIME:
         return _bson_json_read_append_date_time (reader, bson);
      case BSON_TYPE_UNDEFINED:
         return bson_append_undefined (STACK_BSON_CHILD, bson->key,
                                       (int)bson->key_buf.len);
      case BSON_TYPE_MINKEY:
         return bson_append_minkey (STACK_BSON_CHILD, bson->key,
                                    (int)bson->key_buf.len);
      case BSON_TYPE_MAXKEY:
         return bson_append_maxkey (STACK_BSON_CHILD, bson->key,
                                    (int)bson->key_buf.len);
      case BSON_TYPE_INT64:
         return bson_append_int64 (STACK_BSON_CHILD, bson->key,
                                   (int)bson->key_buf.len,
                                   bson->bson_type_data.v_int64.value);
      case BSON_TYPE_EOD:
      case BSON_TYPE_DOUBLE:
      case BSON_TYPE_UTF8:
      case BSON_TYPE_DOCUMENT:
      case BSON_TYPE_ARRAY:
      case BSON_TYPE_BOOL:
      case BSON_TYPE_NULL:
      case BSON_TYPE_CODE:
      case BSON_TYPE_SYMBOL:
      case BSON_TYPE_CODEWSCOPE:
      case BSON_TYPE_INT32:
      case BSON_TYPE_TIMESTAMP:
      case BSON_TYPE_DBPOINTER:
      default:
         _bson_json_read_set_error (reader, "Unknown type %d", bson->bson_type);
         return 0;
         break;
      }
   } else if (bson->read_state == BSON_JSON_IN_BSON_TYPE_TIMESTAMP_VALUES) {
      bson->read_state = BSON_JSON_IN_BSON_TYPE_TIMESTAMP_ENDMAP;

      return _bson_json_read_append_timestamp (reader, bson);
   } else if (bson->read_state == BSON_JSON_IN_BSON_TYPE_TIMESTAMP_ENDMAP) {
      bson->read_state = BSON_JSON_REGULAR;
   } else if (bson->read_state == BSON_JSON_IN_BSON_TYPE_DATE_NUMBERLONG) {
      bson->read_state = BSON_JSON_IN_BSON_TYPE_DATE_ENDMAP;

      return _bson_json_read_append_date_time(reader, bson);
   } else if (bson->read_state == BSON_JSON_IN_BSON_TYPE_DATE_ENDMAP) {
      bson->read_state = BSON_JSON_REGULAR;
   } else if (bson->read_state == BSON_JSON_REGULAR) {
      STACK_POP_DOC (bson_append_document_end (STACK_BSON_PARENT,
                                               STACK_BSON_CHILD));

      if (bson->n == -1) {
         bson->read_state = BSON_JSON_DONE;
         return 0;
      }
   } else {
      _bson_json_read_set_error (reader, "Invalid state %d", bson->read_state);
      return 0;
   }

   return 1;
}


static int
_bson_json_read_start_array (void *_ctx) /* IN */
{
   const char *key;
   size_t len;
   bson_json_reader_t *reader = (bson_json_reader_t *)_ctx;
   bson_json_reader_bson_t *bson = &reader->bson;

   if (bson->n < 0) {
      STACK_PUSH_ARRAY (_noop ());
   } else {
      _bson_json_read_fixup_key (bson);
      key = bson->key;
      len = bson->key_buf.len;

      BASIC_YAJL_CB_BAIL_IF_NOT_NORMAL ("[");

      STACK_PUSH_ARRAY (bson_append_array_begin (STACK_BSON_PARENT, key, (int)len,
                                                 STACK_BSON_CHILD));
   }

   return 1;
}


static int
_bson_json_read_end_array (void *_ctx) /* IN */
{
   bson_json_reader_t *reader = (bson_json_reader_t *)_ctx;
   bson_json_reader_bson_t *bson = &reader->bson;

   if (bson->read_state != BSON_JSON_REGULAR) {
      _bson_json_read_set_error (reader, "Invalid read of %s in state %d",
                                 "]", bson->read_state);
      return 0;
   }

   STACK_POP_ARRAY (bson_append_array_end (STACK_BSON_PARENT,
                                           STACK_BSON_CHILD));
   if (bson->n == -1) {
      bson->read_state = BSON_JSON_DONE;
      return 0;
   }

   return 1;
}


static yajl_callbacks read_cbs = {
   _bson_json_read_null,
   _bson_json_read_boolean,
   _bson_json_read_integer,
   _bson_json_read_double,
   NULL,
   _bson_json_read_string,
   _bson_json_read_start_map,
   _bson_json_read_map_key,
   _bson_json_read_end_map,
   _bson_json_read_start_array,
   _bson_json_read_end_array
};


static int
_bson_json_read_parse_error (bson_json_reader_t *reader, /* IN */
                             yajl_status         ys,     /* IN */
                             bson_error_t       *error)  /* OUT */
{
   unsigned char *str;
   int r;
   yajl_handle yh = reader->yh;
   bson_json_reader_bson_t *bson = &reader->bson;
   bson_json_reader_producer_t *p = &reader->producer;

   if (ys == yajl_status_client_canceled) {
      if (bson->read_state == BSON_JSON_DONE) {
         r = 1;
      } else {
         r = -1;
      }
   } else if (p->all_whitespace) {
      r = 0;
   } else {
      if (error) {
         str = yajl_get_error (yh, 1, p->buf + p->bytes_parsed,
                               p->bytes_read - p->bytes_parsed);
         bson_set_error (error,
                         BSON_ERROR_JSON,
                         BSON_JSON_ERROR_READ_CORRUPT_JS,
                         "%s", str);
         yajl_free_error (yh, str);
      }

      r = -1;
   }

   p->bytes_parsed += yajl_get_bytes_consumed (yh);

   yh->stateStack.used = 0;
   yajl_bs_push (yh->stateStack, yajl_state_start);

   return r;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_json_reader_read --
 *
 *       Read the next json document from @reader and write its value
 *       into @bson. @bson will be allocated as part of this process.
 *
 *       @bson MUST be initialized before calling this function as it
 *       will not be initialized automatically. The reasoning for this
 *       is so that you can chain together bson_json_reader_t with
 *       other components like bson_writer_t.
 *
 * Returns:
 *       1 if successful and data was read.
 *       0 if successful and no data was read.
 *       -1 if there was an error and @error is set.
 *
 * Side effects:
 *       @error may be set.
 *
 *--------------------------------------------------------------------------
 */

int
bson_json_reader_read (bson_json_reader_t *reader, /* IN */
                       bson_t             *bson,   /* IN */
                       bson_error_t       *error)  /* OUT */
{
   bson_json_reader_producer_t *p;
   yajl_status ys;
   yajl_handle yh;
   ssize_t r;
   bool read_something = false;
   int ret = 0;

   BSON_ASSERT (reader);
   BSON_ASSERT (bson);

   p = &reader->producer;
   yh = reader->yh;

   reader->bson.bson = bson;
   reader->bson.n = -1;
   reader->bson.read_state = BSON_JSON_REGULAR;
   reader->error = error;
   reader->producer.all_whitespace = true;

   for (;; ) {
      if (!read_something &&
          p->bytes_parsed &&
          (p->bytes_read > p->bytes_parsed)) {
         r = p->bytes_read - p->bytes_parsed;
      } else {
         r = p->cb (p->data, p->buf, p->buf_size - 1);

         if (r > 0) {
            p->bytes_read = r;
            p->bytes_parsed = 0;
            p->buf[r] = '\0';
         }
      }

      if (r < 0) {
         if (error) {
            bson_set_error (error,
                            BSON_ERROR_JSON,
                            BSON_JSON_ERROR_READ_CB_FAILURE,
                            "reader cb failed");
         }
         ret = -1;
         goto cleanup;
      } else if (r == 0) {
         break;
      } else {
         read_something = true;

         if (p->all_whitespace) {
            p->all_whitespace = _bson_json_all_whitespace (
               (char *)(p->buf + p->bytes_parsed));
         }

         ys = yajl_parse (yh, p->buf + p->bytes_parsed, r);

         if (ys != yajl_status_ok) {
            ret = _bson_json_read_parse_error (reader, ys, error);
            goto cleanup;
         }
      }
   }

   if (read_something) {
      ys = yajl_complete_parse (yh);

      if (ys != yajl_status_ok) {
         ret = _bson_json_read_parse_error (reader, ys, error);
         goto cleanup;
      }
   }

cleanup:

   return ret;
}


bson_json_reader_t *
bson_json_reader_new (void                 *data,           /* IN */
                      bson_json_reader_cb   cb,             /* IN */
                      bson_json_destroy_cb  dcb,            /* IN */
                      bool                  allow_multiple, /* IN */
                      size_t                buf_size)       /* IN */
{
   bson_json_reader_t *r;
   bson_json_reader_producer_t *p;

   r = bson_malloc0 (sizeof *r);

   p = &r->producer;

   p->data = data;
   p->cb = cb;
   p->dcb = dcb;
   p->buf_size = buf_size ? buf_size : BSON_JSON_DEFAULT_BUF_SIZE;
   p->buf = bson_malloc (p->buf_size);

   r->yh = yajl_alloc (&read_cbs, &gYajlAllocFuncs, r);

   yajl_config (r->yh,
                yajl_dont_validate_strings |
                (allow_multiple ?  yajl_allow_multiple_values : 0)
                , 1);

   return r;
}


void
bson_json_reader_destroy (bson_json_reader_t *reader) /* IN */
{
   int i;
   bson_json_reader_producer_t *p = &reader->producer;
   bson_json_reader_bson_t *b = &reader->bson;

   if (reader->producer.dcb) {
      reader->producer.dcb (reader->producer.data);
   }

   bson_free (p->buf);
   bson_free (b->key_buf.buf);

   for (i = 0; i < 3; i++) {
      bson_free (b->bson_type_buf[i].buf);
   }

   yajl_free (reader->yh);

   bson_free (reader);
}


typedef struct
{
   const uint8_t *data;
   size_t         len;
   size_t         bytes_parsed;
} bson_json_data_reader_t;


static ssize_t
_bson_json_data_reader_cb (void    *_ctx,
                           uint8_t *buf,
                           size_t   len)
{
   size_t bytes;
   bson_json_data_reader_t *ctx = (bson_json_data_reader_t *)_ctx;

   if (!ctx->data) {
      return -1;
   }

   bytes = BSON_MIN (len, ctx->len - ctx->bytes_parsed);

   memcpy (buf, ctx->data + ctx->bytes_parsed, bytes);

   ctx->bytes_parsed += bytes;

   return bytes;
}


bson_json_reader_t *
bson_json_data_reader_new (bool   allow_multiple, /* IN */
                           size_t size)           /* IN */
{
   bson_json_data_reader_t *dr = bson_malloc0 (sizeof *dr);

   return bson_json_reader_new (dr, &_bson_json_data_reader_cb, &bson_free,
                                allow_multiple, size);
}


void
bson_json_data_reader_ingest (bson_json_reader_t *reader, /* IN */
                              const uint8_t      *data,   /* IN */
                              size_t              len)    /* IN */
{
   bson_json_data_reader_t *ctx =
      (bson_json_data_reader_t *)reader->producer.data;

   ctx->data = data;
   ctx->len = len;
   ctx->bytes_parsed = 0;
}


bson_t *
bson_new_from_json (const uint8_t *data,  /* IN */
                    ssize_t        len,   /* IN */
                    bson_error_t  *error) /* OUT */
{
   bson_json_reader_t *reader;
   bson_t *bson;
   int r;

   BSON_ASSERT (data);

   if (len < 0) {
      len = (ssize_t)strlen ((const char *)data);
   }

   bson = bson_new ();
   reader = bson_json_data_reader_new (false, BSON_JSON_DEFAULT_BUF_SIZE);
   bson_json_data_reader_ingest (reader, data, len);
   r = bson_json_reader_read (reader, bson, error);
   bson_json_reader_destroy (reader);

   if (r != 1) {
      bson_destroy (bson);
      return NULL;
   }

   return bson;
}


bool
bson_init_from_json (bson_t       *bson,  /* OUT */
                     const char   *data,  /* IN */
                     ssize_t       len,   /* IN */
                     bson_error_t *error) /* OUT */
{
   bson_json_reader_t *reader;
   int r;

   BSON_ASSERT (bson);
   BSON_ASSERT (data);

   if (len < 0) {
      len = strlen (data);
   }

   bson_init (bson);

   reader = bson_json_data_reader_new (false, BSON_JSON_DEFAULT_BUF_SIZE);
   bson_json_data_reader_ingest (reader, (const uint8_t *)data, len);
   r = bson_json_reader_read (reader, bson, error);
   bson_json_reader_destroy (reader);

   if (r != 1) {
      bson_destroy (bson);
      return false;
   }

   return true;
}


static void
_bson_json_reader_handle_fd_destroy (void *handle) /* IN */
{
   bson_json_reader_handle_fd_t *fd = handle;

   if (fd) {
      if ((fd->fd != -1) && fd->do_close) {
#ifdef _WIN32
		 _close(fd->fd);
#else
         close (fd->fd);
#endif
      }
      bson_free (fd);
   }
}


static ssize_t
_bson_json_reader_handle_fd_read (void    *handle, /* IN */
                                  uint8_t *buf,    /* IN */
                                  size_t   len)   /* IN */
{
   bson_json_reader_handle_fd_t *fd = handle;
   ssize_t ret = -1;

   if (fd && (fd->fd != -1)) {
   again:
#ifdef BSON_OS_WIN32
      ret = _read (fd->fd, buf, (unsigned int)len);
#else
      ret = read (fd->fd, buf, len);
#endif
      if ((ret == -1) && (errno == EAGAIN)) {
         goto again;
      }
   }

   return ret;
}


bson_json_reader_t *
bson_json_reader_new_from_fd (int fd,                /* IN */
                              bool close_on_destroy) /* IN */
{
   bson_json_reader_handle_fd_t *handle;

   BSON_ASSERT (fd != -1);

   handle = bson_malloc0 (sizeof *handle);
   handle->fd = fd;
   handle->do_close = close_on_destroy;

   return bson_json_reader_new (handle,
                                _bson_json_reader_handle_fd_read,
                                _bson_json_reader_handle_fd_destroy,
                                true,
                                BSON_JSON_DEFAULT_BUF_SIZE);
}


bson_json_reader_t *
bson_json_reader_new_from_file (const char   *path,  /* IN */
                                bson_error_t *error) /* OUT */
{
   char errmsg_buf[BSON_ERROR_BUFFER_SIZE];
   char *errmsg;
   int fd = -1;

   BSON_ASSERT (path);

#ifdef BSON_OS_WIN32
   _sopen_s (&fd, path, (_O_RDONLY | _O_BINARY), _SH_DENYNO, _S_IREAD);
#else
   fd = open (path, O_RDONLY);
#endif

   if (fd == -1) {
      errmsg = bson_strerror_r (errno, errmsg_buf, sizeof errmsg_buf);
      bson_set_error (error,
                      BSON_ERROR_READER,
                      BSON_ERROR_READER_BADFD,
                      "%s", errmsg);
      return NULL;
   }

   return bson_json_reader_new_from_fd (fd, true);
}
/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdlib.h>
#include <string.h>

// #include "bson-atomic.h"

// #include "bson-config.h"

// #include "bson-memory.h"



static bson_mem_vtable_t gMemVtable = {
   malloc,
   calloc,
#ifdef __APPLE__
   reallocf,
#else
   realloc,
#endif
   free,
};


/*
 *--------------------------------------------------------------------------
 *
 * bson_malloc --
 *
 *       Allocates @num_bytes of memory and returns a pointer to it.  If
 *       malloc failed to allocate the memory, abort() is called.
 *
 *       Libbson does not try to handle OOM conditions as it is beyond the
 *       scope of this library to handle so appropriately.
 *
 * Parameters:
 *       @num_bytes: The number of bytes to allocate.
 *
 * Returns:
 *       A pointer if successful; otherwise abort() is called and this
 *       function will never return.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void *
bson_malloc (size_t num_bytes) /* IN */
{
   void *mem;

   if (!(mem = gMemVtable.malloc (num_bytes))) {
      abort ();
   }

   return mem;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_malloc0 --
 *
 *       Like bson_malloc() except the memory is zeroed first. This is
 *       similar to calloc() except that abort() is called in case of
 *       failure to allocate memory.
 *
 * Parameters:
 *       @num_bytes: The number of bytes to allocate.
 *
 * Returns:
 *       A pointer if successful; otherwise abort() is called and this
 *       function will never return.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void *
bson_malloc0 (size_t num_bytes) /* IN */
{
   void *mem = NULL;

   if (BSON_LIKELY (num_bytes)) {
      if (BSON_UNLIKELY (!(mem = gMemVtable.calloc (1, num_bytes)))) {
         abort ();
      }
   }

   return mem;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_realloc --
 *
 *       This function behaves similar to realloc() except that if there is
 *       a failure abort() is called.
 *
 * Parameters:
 *       @mem: The memory to realloc, or NULL.
 *       @num_bytes: The size of the new allocation or 0 to free.
 *
 * Returns:
 *       The new allocation if successful; otherwise abort() is called and
 *       this function never returns.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void *
bson_realloc (void   *mem,        /* IN */
              size_t  num_bytes)  /* IN */
{
   /*
    * Not all platforms are guaranteed to free() the memory if a call to
    * realloc() with a size of zero occurs. Windows, Linux, and FreeBSD do,
    * however, OS X does not.
    */
   if (BSON_UNLIKELY (num_bytes == 0)) {
      gMemVtable.free (mem);
      return NULL;
   }

   mem = gMemVtable.realloc (mem, num_bytes);

   if (BSON_UNLIKELY (!mem)) {
      abort ();
   }

   return mem;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_realloc_ctx --
 *
 *       This wraps bson_realloc and provides a compatible api for similar
 *       functions with a context
 *
 * Parameters:
 *       @mem: The memory to realloc, or NULL.
 *       @num_bytes: The size of the new allocation or 0 to free.
 *       @ctx: Ignored
 *
 * Returns:
 *       The new allocation if successful; otherwise abort() is called and
 *       this function never returns.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */


void *
bson_realloc_ctx (void   *mem,        /* IN */
                  size_t  num_bytes,  /* IN */
                  void   *ctx)        /* IN */
{
   return bson_realloc (mem, num_bytes);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_free --
 *
 *       Frees @mem using the underlying allocator.
 *
 *       Currently, this only calls free() directly, but that is subject to
 *       change.
 *
 * Parameters:
 *       @mem: An allocation to free.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_free (void *mem) /* IN */
{
   gMemVtable.free (mem);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_zero_free --
 *
 *       Frees @mem using the underlying allocator. @size bytes of @mem will
 *       be zeroed before freeing the memory. This is useful in scenarios
 *       where @mem contains passwords or other sensitive information.
 *
 * Parameters:
 *       @mem: An allocation to free.
 *       @size: The number of bytes in @mem.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_zero_free (void  *mem,  /* IN */
                size_t size) /* IN */
{
   if (BSON_LIKELY (mem)) {
      memset (mem, 0, size);
      gMemVtable.free (mem);
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_mem_set_vtable --
 *
 *       This function will change our allocationt vtable.
 *
 *       It is imperitive that this is called at the beginning of the
 *       process before any memory has been allocated by the default
 *       allocator.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_mem_set_vtable (const bson_mem_vtable_t *vtable)
{
   BSON_ASSERT (vtable);

   if (!vtable->malloc ||
       !vtable->calloc ||
       !vtable->realloc ||
       !vtable->free) {
      fprintf (stderr, "Failure to install BSON vtable, "
                       "missing functions.\n");
      return;
   }

   gMemVtable = *vtable;
}

void
bson_mem_restore_vtable (void)
{
   bson_mem_vtable_t vtable = {
      malloc,
      calloc,
#ifdef __APPLE__
      reallocf,
#else
      realloc,
#endif
      free,
   };

   bson_mem_set_vtable(&vtable);
}

/*
 * Copyright 2013 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <limits.h>
#include <stdarg.h>
#include <string.h>

// #include "bson-compat.h"

// #include "bson-config.h"

// #include "bson-string.h"

// #include "bson-memory.h"

// #include "bson-utf8.h"



/*
 *--------------------------------------------------------------------------
 *
 * bson_string_new --
 *
 *       Create a new bson_string_t.
 *
 *       bson_string_t is a power-of-2 allocation growing string. Every
 *       time data is appended the next power of two size is chosen for
 *       the allocation. Pretty standard stuff.
 *
 *       It is UTF-8 aware through the use of bson_string_append_unichar().
 *       The proper UTF-8 character sequence will be used.
 *
 * Parameters:
 *       @str: a string to copy or NULL.
 *
 * Returns:
 *       A newly allocated bson_string_t that should be freed with
 *       bson_string_free().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

bson_string_t *
bson_string_new (const char *str) /* IN */
{
   bson_string_t *ret;

   ret = bson_malloc0 (sizeof *ret);
   ret->len = str ? (int)strlen (str) : 0;
   ret->alloc = ret->len + 1;

   if (!bson_is_power_of_two (ret->alloc)) {
      ret->alloc = (uint32_t)bson_next_power_of_two ((size_t)ret->alloc);
   }

   BSON_ASSERT (ret->alloc >= 1);

   ret->str = bson_malloc (ret->alloc);

   if (str) {
      memcpy (ret->str, str, ret->len);
   }
   ret->str [ret->len] = '\0';

   ret->str [ret->len] = '\0';

   return ret;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_string_free --
 *
 *       Free the bson_string_t @string and related allocations.
 *
 *       If @free_segment is false, then the strings buffer will be
 *       returned and is not freed. Otherwise, NULL is returned.
 *
 * Returns:
 *       The string->str if free_segment is false.
 *       Otherwise NULL.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

char *
bson_string_free (bson_string_t *string,       /* IN */
                  bool           free_segment) /* IN */
{
   char *ret = NULL;

   BSON_ASSERT (string);

   if (!free_segment) {
      ret = string->str;
   } else {
      bson_free (string->str);
   }

   bson_free (string);

   return ret;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_string_append --
 *
 *       Append the UTF-8 string @str to @string.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_string_append (bson_string_t *string, /* IN */
                    const char    *str)    /* IN */
{
   uint32_t len;

   BSON_ASSERT (string);
   BSON_ASSERT (str);

   len = (uint32_t)strlen (str);

   if ((string->alloc - string->len - 1) < len) {
      string->alloc += len;
      if (!bson_is_power_of_two (string->alloc)) {
         string->alloc = (uint32_t)bson_next_power_of_two ((size_t)string->alloc);
      }
      string->str = bson_realloc (string->str, string->alloc);
   }

   memcpy (string->str + string->len, str, len);
   string->len += len;
   string->str [string->len] = '\0';
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_string_append_c --
 *
 *       Append the ASCII character @c to @string.
 *
 *       Do not use this if you are working with UTF-8 sequences,
 *       use bson_string_append_unichar().
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_string_append_c (bson_string_t *string, /* IN */
                      char           c)      /* IN */
{
   char cc[2];

   BSON_ASSERT (string);

   if (BSON_UNLIKELY (string->alloc == (string->len + 1))) {
      cc [0] = c;
      cc [1] = '\0';
      bson_string_append (string, cc);
      return;
   }

   string->str [string->len++] = c;
   string->str [string->len] = '\0';
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_string_append_unichar --
 *
 *       Append the bson_unichar_t @unichar to the string @string.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_string_append_unichar (bson_string_t  *string,  /* IN */
                            bson_unichar_t  unichar) /* IN */
{
   uint32_t len;
   char str [8];

   BSON_ASSERT (string);
   BSON_ASSERT (unichar);

   bson_utf8_from_unichar (unichar, str, &len);

   if (len <= 6) {
      str [len] = '\0';
      bson_string_append (string, str);
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_string_append_printf --
 *
 *       Format a string according to @format and append it to @string.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_string_append_printf (bson_string_t *string,
                           const char    *format,
                           ...)
{
   va_list args;
   char *ret;

   BSON_ASSERT (string);
   BSON_ASSERT (format);

   va_start (args, format);
   ret = bson_strdupv_printf (format, args);
   va_end (args);
   bson_string_append (string, ret);
   bson_free (ret);
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_string_truncate --
 *
 *       Truncate the string @string to @len bytes.
 *
 *       The underlying memory will be released via realloc() down to
 *       the minimum required size specified by @len.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_string_truncate (bson_string_t *string, /* IN */
                      uint32_t       len)    /* IN */
{
   uint32_t alloc;

   BSON_ASSERT (string);
   BSON_ASSERT (len < INT_MAX);

   alloc = len + 1;

   if (alloc < 16) {
      alloc = 16;
   }

   if (!bson_is_power_of_two (alloc)) {
      alloc = (uint32_t)bson_next_power_of_two ((size_t)alloc);
   }

   string->str = bson_realloc (string->str, alloc);
   string->alloc = alloc;
   string->len = len;

   string->str [string->len] = '\0';
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_strdup --
 *
 *       Portable strdup().
 *
 * Returns:
 *       A newly allocated string that should be freed with bson_free().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

char *
bson_strdup (const char *str) /* IN */
{
   long len;
   char *out;

   if (!str) {
      return NULL;
   }

   len = (long)strlen (str);
   out = bson_malloc (len + 1);

   if (!out) {
      return NULL;
   }

   memcpy (out, str, len + 1);

   return out;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_strdupv_printf --
 *
 *       Like bson_strdup_printf() but takes a va_list.
 *
 * Returns:
 *       A newly allocated string that should be freed with bson_free().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

char *
bson_strdupv_printf (const char *format, /* IN */
                     va_list     args)   /* IN */
{
   va_list my_args;
   char *buf;
   int len = 32;
   int n;

   BSON_ASSERT (format);

   buf = bson_malloc0 (len);

   while (true) {
      va_copy (my_args, args);
      n = bson_vsnprintf (buf, len, format, my_args);
      va_end (my_args);

      if (n > -1 && n < len) {
         return buf;
      }

      if (n > -1) {
         len = n + 1;
      } else {
         len *= 2;
      }

      buf = bson_realloc (buf, len);
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_strdup_printf --
 *
 *       Convenience function that formats a string according to @format
 *       and returns a copy of it.
 *
 * Returns:
 *       A newly created string that should be freed with bson_free().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

char *
bson_strdup_printf (const char *format, /* IN */
                    ...)                /* IN */
{
   va_list args;
   char *ret;

   BSON_ASSERT (format);

   va_start (args, format);
   ret = bson_strdupv_printf (format, args);
   va_end (args);

   return ret;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_strndup --
 *
 *       A portable strndup().
 *
 * Returns:
 *       A newly allocated string that should be freed with bson_free().
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

char *
bson_strndup (const char *str,     /* IN */
              size_t      n_bytes) /* IN */
{
   char *ret;

   BSON_ASSERT (str);

   ret = bson_malloc (n_bytes + 1);
   memcpy (ret, str, n_bytes);
   ret[n_bytes] = '\0';

   return ret;
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_strfreev --
 *
 *       Frees each string in a NULL terminated array of strings.
 *       This also frees the underlying array.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_strfreev (char **str) /* IN */
{
   int i;

   if (str) {
      for (i = 0; str [i]; i++)
         bson_free (str [i]);
      bson_free (str);
   }
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_strnlen --
 *
 *       A portable strnlen().
 *
 * Returns:
 *       The length of @s up to @maxlen.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

size_t
bson_strnlen (const char *s,      /* IN */
              size_t      maxlen) /* IN */
{
#ifdef BSON_HAVE_STRNLEN
   return strnlen (s, maxlen);
#else
   size_t i;

   for (i = 0; i < maxlen; i++) {
      if (s [i] == '\0') {
         return i;
      }
   }

   return maxlen;
#endif
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_strncpy --
 *
 *       A portable strncpy.
 *
 *       Copies @src into @dst, which must be @size bytes or larger.
 *       The result is guaranteed to be \0 terminated.
 *
 * Returns:
 *       None.
 *
 * Side effects:
 *       None.
 *
 *--------------------------------------------------------------------------
 */

void
bson_strncpy (char       *dst,  /* IN */
              const char *src,  /* IN */
              size_t      size) /* IN */
{
#ifdef _MSC_VER
   strncpy_s (dst, size, src, _TRUNCATE);
#else
   strncpy (dst, src, size);
   dst[size - 1] = '\0';
#endif
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_vsnprintf --
 *
 *       A portable vsnprintf.
 *
 *       If more than @size bytes are required (exluding the null byte),
 *       then @size bytes will be written to @string and the return value
 *       is the number of bytes required.
 *
 *       This function will always return a NULL terminated string.
 *
 * Returns:
 *       The number of bytes required for @format excluding the null byte.
 *
 * Side effects:
 *       @str is initialized with the formatted string.
 *
 *--------------------------------------------------------------------------
 */

int
bson_vsnprintf (char       *str,    /* IN */
                size_t      size,   /* IN */
                const char *format, /* IN */
                va_list     ap)     /* IN */
{
#ifdef BSON_OS_WIN32
   int r = -1;

   BSON_ASSERT (str);

   if (size != 0) {
       r = _vsnprintf_s (str, size, _TRUNCATE, format, ap);
   }

   if (r == -1) {
      r = _vscprintf (format, ap);
   }

   str [size - 1] = '\0';

   return r;
#else
   int r;

   r = vsnprintf (str, size, format, ap);
   str [size - 1] = '\0';
   return r;
#endif
}


/*
 *--------------------------------------------------------------------------
 *
 * bson_snprintf --
 *
 *       A portable snprintf.
 *
 *       If @format requires more than @size bytes, then @size bytes are
 *       written and the result is the number of bytes required (excluding
 *       the null byte).
 *
 *       This function will always return a NULL terminated string.
 *
 * Returns:
 *       The number of bytes required for @format.
 *
 * Side effects:
 *       @str is initialized.
 *
 *--------------------------------------------------------------------------
 */

int
bson_snprintf (char       *str,    /* IN */
               size_t      size,   /* IN */
               const char *format, /* IN */
               ...)
{
   int r;
   va_list ap;

   BSON_ASSERT (str);

   va_start (ap, format);
   r = bson_vsnprintf (str, size, format, ap);
   va_end (ap);

   return r;
}


int64_t
bson_ascii_strtoll (const char  *s,
                    char       **e,
                    int          base)
{
    char *tok = (char *)s;
    char c;
    int64_t number = 0;
    int64_t sign = 1;

    if (!s) {
       errno = EINVAL;
       return 0;
    }

    c = *tok;

    while (isspace (c)) {
        c = *++tok;
    }

    if (!isdigit (c) && (c != '+') && (c != '-')) {
        *e = tok - 1;
        errno = EINVAL;
        return 0;
    }

    if (c == '-') {
        sign = -1;
        c = *++tok;
    }

    if (c == '+') {
        c = *++tok;
    }

    if (c == '0' && tok[1] != '\0') {
        /* Hex, octal or binary -- maybe. */

        c = *++tok;

        if (c == 'x' || c == 'X') { /* Hex */
            if (base != 16) {
                *e = (char *)(s);
                errno = EINVAL;
                return 0;
            }

            c = *++tok;
            if (!isxdigit (c)) {
                *e = tok;
                errno = EINVAL;
                return 0;
            }
            do {
                number = (number << 4) + (c - '0');
                c = *(++tok);
            } while (isxdigit (c));
        }
        else { /* Octal */
            if (base != 8) {
                *e = (char *)(s);
                errno = EINVAL;
                return 0;
            }

            if (c < '0' || c >= '8') {
                *e = tok;
                errno = EINVAL;
                return 0;
            }
            do {
                number = (number << 3) + (c - '0');
                c = *(++tok);
            } while (('0' <= c) && (c < '8'));
        }

        while (c == 'l' || c == 'L' || c == 'u' || c == 'U') {
            c = *++tok;
        }
    }
    else {
        /* Decimal */
        if (base != 10) {
            *e = (char *)(s);
            errno = EINVAL;
            return 0;
        }

        do {
            number = (number * 10) + (c - '0');
            c = *(++tok);
        } while (isdigit (c));

        while (c == 'l' || c == 'L' || c == 'u' || c == 'U') {
            c = *(++tok);
        }
    }

    *e = tok;
    errno = 0;
    return (sign * number);
}

/*
 * Copyright 2014 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// #include "bson-memory.h"

// #include "bson-string.h"

// #include "bson-value.h"

// #include "bson-oid.h"



void
bson_value_copy (const bson_value_t *src, /* IN */
                 bson_value_t       *dst) /* OUT */
{
   BSON_ASSERT (src);
   BSON_ASSERT (dst);

   dst->value_type = src->value_type;

   switch (src->value_type) {
   case BSON_TYPE_DOUBLE:
      dst->value.v_double = src->value.v_double;
      break;
   case BSON_TYPE_UTF8:
      dst->value.v_utf8.len = src->value.v_utf8.len;
      dst->value.v_utf8.str = bson_malloc (src->value.v_utf8.len + 1);
      memcpy (dst->value.v_utf8.str,
              src->value.v_utf8.str,
              dst->value.v_utf8.len);
      dst->value.v_utf8.str [dst->value.v_utf8.len] = '\0';
      break;
   case BSON_TYPE_DOCUMENT:
   case BSON_TYPE_ARRAY:
      dst->value.v_doc.data_len = src->value.v_doc.data_len;
      dst->value.v_doc.data = bson_malloc (src->value.v_doc.data_len);
      memcpy (dst->value.v_doc.data,
              src->value.v_doc.data,
              dst->value.v_doc.data_len);
      break;
   case BSON_TYPE_BINARY:
      dst->value.v_binary.subtype = src->value.v_binary.subtype;
      dst->value.v_binary.data_len = src->value.v_binary.data_len;
      dst->value.v_binary.data = bson_malloc (src->value.v_binary.data_len);
      memcpy (dst->value.v_binary.data,
              src->value.v_binary.data,
              dst->value.v_binary.data_len);
      break;
   case BSON_TYPE_OID:
      bson_oid_copy (&src->value.v_oid, &dst->value.v_oid);
      break;
   case BSON_TYPE_BOOL:
      dst->value.v_bool = src->value.v_bool;
      break;
   case BSON_TYPE_DATE_TIME:
      dst->value.v_datetime = src->value.v_datetime;
      break;
   case BSON_TYPE_REGEX:
      dst->value.v_regex.regex = bson_strdup (src->value.v_regex.regex);
      dst->value.v_regex.options = bson_strdup (src->value.v_regex.options);
      break;
   case BSON_TYPE_DBPOINTER:
      dst->value.v_dbpointer.collection_len = src->value.v_dbpointer.collection_len;
      dst->value.v_dbpointer.collection = bson_malloc (src->value.v_dbpointer.collection_len + 1);
      memcpy (dst->value.v_dbpointer.collection,
              src->value.v_dbpointer.collection,
              dst->value.v_dbpointer.collection_len);
      dst->value.v_dbpointer.collection [dst->value.v_dbpointer.collection_len] = '\0';
      bson_oid_copy (&src->value.v_dbpointer.oid, &dst->value.v_dbpointer.oid);
      break;
   case BSON_TYPE_CODE:
      dst->value.v_code.code_len = src->value.v_code.code_len;
      dst->value.v_code.code = bson_malloc (src->value.v_code.code_len + 1);
      memcpy (dst->value.v_code.code,
              src->value.v_code.code,
              dst->value.v_code.code_len);
      dst->value.v_code.code [dst->value.v_code.code_len] = '\0';
      break;
   case BSON_TYPE_SYMBOL:
      dst->value.v_symbol.len = src->value.v_symbol.len;
      dst->value.v_symbol.symbol = bson_malloc (src->value.v_symbol.len + 1);
      memcpy (dst->value.v_symbol.symbol,
              src->value.v_symbol.symbol,
              dst->value.v_symbol.len);
      dst->value.v_symbol.symbol [dst->value.v_symbol.len] = '\0';
      break;
   case BSON_TYPE_CODEWSCOPE:
      dst->value.v_codewscope.code_len = src->value.v_codewscope.code_len;
      dst->value.v_codewscope.code = bson_malloc (src->value.v_codewscope.code_len + 1);
      memcpy (dst->value.v_codewscope.code,
              src->value.v_codewscope.code,
              dst->value.v_codewscope.code_len);
      dst->value.v_codewscope.code [dst->value.v_codewscope.code_len] = '\0';
      dst->value.v_codewscope.scope_len = src->value.v_codewscope.scope_len;
      dst->value.v_codewscope.scope_data = bson_malloc (src->value.v_codewscope.scope_len);
      memcpy (dst->value.v_codewscope.scope_data,
              src->value.v_codewscope.scope_data,
              dst->value.v_codewscope.scope_len);
      break;
   case BSON_TYPE_INT32:
      dst->value.v_int32 = src->value.v_int32;
      break;
   case BSON_TYPE_TIMESTAMP:
      dst->value.v_timestamp.timestamp = src->value.v_timestamp.timestamp;
      dst->value.v_timestamp.increment = src->value.v_timestamp.increment;
      break;
   case BSON_TYPE_INT64:
      dst->value.v_int64 = src->value.v_int64;
      break;
   case BSON_TYPE_UNDEFINED:
   case BSON_TYPE_NULL:
   case BSON_TYPE_MAXKEY:
   case BSON_TYPE_MINKEY:
      break;
   case BSON_TYPE_EOD:
   default:
      BSON_ASSERT (false);
      return;
   }
}


void
bson_value_destroy (bson_value_t *value) /* IN */
{
   switch (value->value_type) {
   case BSON_TYPE_UTF8:
      bson_free (value->value.v_utf8.str);
      break;
   case BSON_TYPE_DOCUMENT:
   case BSON_TYPE_ARRAY:
      bson_free (value->value.v_doc.data);
      break;
   case BSON_TYPE_BINARY:
      bson_free (value->value.v_binary.data);
      break;
   case BSON_TYPE_REGEX:
      bson_free (value->value.v_regex.regex);
      bson_free (value->value.v_regex.options);
      break;
   case BSON_TYPE_DBPOINTER:
      bson_free (value->value.v_dbpointer.collection);
      break;
   case BSON_TYPE_CODE:
      bson_free (value->value.v_code.code);
      break;
   case BSON_TYPE_SYMBOL:
      bson_free (value->value.v_symbol.symbol);
      break;
   case BSON_TYPE_CODEWSCOPE:
      bson_free (value->value.v_codewscope.code);
      bson_free (value->value.v_codewscope.scope_data);
      break;
   case BSON_TYPE_DOUBLE:
   case BSON_TYPE_UNDEFINED:
   case BSON_TYPE_OID:
   case BSON_TYPE_BOOL:
   case BSON_TYPE_DATE_TIME:
   case BSON_TYPE_NULL:
   case BSON_TYPE_INT32:
   case BSON_TYPE_TIMESTAMP:
   case BSON_TYPE_INT64:
   case BSON_TYPE_MAXKEY:
   case BSON_TYPE_MINKEY:
   case BSON_TYPE_EOD:
   default:
      break;
   }
}
