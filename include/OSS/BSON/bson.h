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
