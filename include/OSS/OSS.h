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


#ifndef OSS_H_INCLUDED
#define OSS_H_INCLUDED

#define OSS_API_MAJOR_VERSION 1
#define OSS_API_MINOR_VERSION 0
#define OSS_API_BUILD_NUMBER 0

// Turn off some annoying warnings
#ifdef _MSC_VER
	#pragma warning(disable:4786)  // identifier truncation warning
	#pragma warning(disable:4503)  // decorated name length exceeded - mainly a problem boost
	#pragma warning(disable:4018)  // signed/unsigned comparison
	#pragma warning(disable:4284)  // return type for operator -> is not UDT
	#pragma warning(disable:4251)  // ... needs to have dll-interface warning 
	#pragma warning(disable:4273) 
	#pragma warning(disable:4275)  // ... non dll-interface class used as base for dll-interface class
  #pragma warning(disable:4267)  // conversion from 'size_t' to 'int', possible loss of data
  #pragma warning(disable:4355)  // 'this' : used in base member initializer list
  #pragma warning(disable:1.54)  // conversion from '__w64 int' to 'unsigned short', possible loss of data
  #pragma warning(disable:4311)  // 'type cast' : pointer truncation
	#pragma warning(disable:4237)  // nonstandard extension used : 'bool' keyword is reserved for future use
  #pragma warning(disable:4800)  // 'int' : forcing value to bool 'true' or 'false' (performance warning)  
  #pragma warning(disable:4675)  // resolved overload was found by argument-dependent lookup
  #pragma warning(disable:4966) //  warning C4996: 'std::_Copy_opt' was declared deprecated
#endif

//
// The following block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SBC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OSS_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
//
#if defined(_WIN32) && defined(OSS_DLL)
	#if defined(OSS_EXPORTS)
		#define OSS_API __declspec(dllexport)
	#else
		#define OSS_API __declspec(dllimport)	
	#endif
#endif

#if !defined(OSS_API)
	#define OSS_API
#endif

//
// Automatically link DNS library.
//
#if defined(_MSC_VER)
	#if !defined(OSS_NO_AUTOMATIC_LIBS) && !defined(OSS_API_EXPORTS)
		#if defined(OSS_DLL)
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSAPId.lib")
			#else
				#pragma comment(lib, "OSSAPI.lib")
			#endif
		#else
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSAPImtd.lib")
			#else
				#pragma comment(lib, "OSSAPImt.lib")
			#endif
		#endif
	#endif
#endif

//
// Platform Identification
//
#define OSS_OS_FREE_BSD      0x0001
#define OSS_OS_AIX           0x0002
#define OSS_OS_HPUX          0x0003
#define OSS_OS_TRU64         0x0004
#define OSS_OS_LINUX         0x0005
#define OSS_OS_MAC_OS_X      0x0006
#define OSS_OS_NET_BSD       0x0007
#define OSS_OS_OPEN_BSD      0x0008
#define OSS_OS_IRIX          0x0009
#define OSS_OS_SOLARIS       0x000a
#define OSS_OS_QNX           0x000b
#define OSS_OS_VXWORKS       0x000c
#define OSS_OS_CYGWIN        0x000d
#define OSS_OS_UNKNOWN_UNIX  0x00ff
#define OSS_OS_WINDOWS_NT    0x1001
#define OSS_OS_WINDOWS_CE    0x1011
#define OSS_OS_VMS           0x2001
#define OSS_OS_MINGW         0x2011


#if defined(__FreeBSD__)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS_FAMILY_BSD 1
	#define OSS_OS OSS_OS_FREE_BSD
#elif defined(_AIX) || defined(__TOS_AIX__)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS OSS_OS_AIX
#elif defined(hpux) || defined(_hpux)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS OSS_OS_HPUX
#elif defined(__digital__) || defined(__osf__)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS OSS_OS_TRU64
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__TOS_LINUX__)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS OSS_OS_LINUX
	#define OSS_PLATFORM_LINUX 1
#elif defined(__APPLE__) || defined(__TOS_MACOS__)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS_FAMILY_BSD 1
	#define OSS_OS OSS_OS_MAC_OS_X
	#define OSS_PLATFORM_MAC_OS_X 1
#elif defined(__NetBSD__)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS_FAMILY_BSD 1
	#define OSS_OS OSS_OS_NET_BSD
#elif defined(__OpenBSD__)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS_FAMILY_BSD 1
	#define OSS_OS OSS_OS_OPEN_BSD
#elif defined(sgi) || defined(__sgi)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS OSS_OS_IRIX
#elif defined(sun) || defined(__sun)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS OSS_OS_SOLARIS
#elif defined(__QNX__)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS OSS_OS_QNX
#elif defined(unix) || defined(__unix) || defined(__unix__)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS OSS_OS_UNKNOWN_UNIX
#elif defined(_WIN32_WCE)
	#define OSS_OS_FAMILY_WINDOWS 1
	#define OSS_OS OSS_OS_WINDOWS_CE
#elif defined(_WIN32) || defined(_WIN64)
	#define OSS_OS_FAMILY_WINDOWS 1
	#define OSS_OS OSS_OS_WINDOWS_NT
#elif defined(__CYGWIN__)
	#define OSS_OS_FAMILY_UNIX 1
	#define OSS_OS OSS_OS_CYGWIN
#elif defined(__MINGW32__)
  #define OSS_OS_FAMILY_UNIX 1
  #define OSS_OS OSS_OS_MINGW 
#elif defined(__VMS)
	#define OSS_OS_FAMILY_VMS 1
	#define OSS_OS OSS_OS_VMS
#endif


//
// Hardware Architecture and Byte Order
//
#define OSS_ARCH_ALPHA   0x01
#define OSS_ARCH_IA32    0x02
#define OSS_ARCH_IA64    0x03
#define OSS_ARCH_MIPS    0x04
#define OSS_ARCH_HPPA    0x05
#define OSS_ARCH_PPC     0x06
#define OSS_ARCH_POWER   0x07
#define OSS_ARCH_SPARC   0x08
#define OSS_ARCH_AMD64   0x09
#define OSS_ARCH_ARM     0x0a
#define OSS_ARCH_M68K    0x0b
#define OSS_ARCH_S390    0x0c


#if defined(__ALPHA) || defined(__alpha) || defined(__alpha__) || defined(_M_ALPHA)
	#define OSS_ARCH OSS_ARCH_ALPHA
	#define OSS_ARCH_LITTLE_ENDIAN 1
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86)
	#define OSS_ARCH OSS_ARCH_IA32
	#define OSS_ARCH_LITTLE_ENDIAN 1
#elif defined(_IA64) || defined(__IA64__) || defined(__ia64__) || defined(__ia64) || defined(_M_IA64)
	#define OSS_ARCH OSS_ARCH_IA64
	#if defined(hpux) || defined(_hpux)
		#define OSS_ARCH_BIG_ENDIAN 1
	#else
		#define OSS_ARCH_LITTLE_ENDIAN 1
	#endif
#elif defined(__x86_64__) || defined(_M_X64)
	#define OSS_ARCH OSS_ARCH_AMD64
	#define OSS_ARCH_LITTLE_ENDIAN 1
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__) || defined(_M_MRX000)
	#define OSS_ARCH OSS_ARCH_MIPS
	#define OSS_ARCH_BIG_ENDIAN 1
#elif defined(__hppa) || defined(__hppa__)
	#define OSS_ARCH OSS_ARCH_HPPA
	#define OSS_ARCH_BIG_ENDIAN 1
#elif defined(__PPC) || defined(__POWERPC__) || defined(__powerpc) || defined(__PPC__) || \
      defined(__powerpc__) || defined(__ppc__) || defined(_ARCH_PPC) || defined(_M_PPC)
	#define OSS_ARCH OSS_ARCH_PPC
	#define OSS_ARCH_BIG_ENDIAN 1
#elif defined(_POWER) || defined(_ARCH_PWR) || defined(_ARCH_PWR2) || defined(_ARCH_PWR3) || \
      defined(_ARCH_PWR4) || defined(__THW_RS6000)
	#define OSS_ARCH OSS_ARCH_POWER
	#define OSS_ARCH_BIG_ENDIAN 1
#elif defined(__sparc__) || defined(__sparc) || defined(sparc)
	#define OSS_ARCH OSS_ARCH_SPARC
	#define OSS_ARCH_BIG_ENDIAN 1
#elif defined(__arm__) || defined(__arm) || defined(ARM) || defined(_ARM_) || defined(__ARM__) || defined(_M_ARM)
	#define OSS_ARCH OSS_ARCH_ARM
	#if defined(__ARMEB__)
		#define OSS_ARCH_BIG_ENDIAN 1
	#else
		#define OSS_ARCH_LITTLE_ENDIAN 1
	#endif
#elif defined(__m68k__)
    #define OSS_ARCH OSS_ARCH_M68K
    #define OSS_ARCH_BIG_ENDIAN 1
#elif defined(__s390__)
    #define OSS_ARCH OSS_ARCH_S390
    #define OSS_ARCH_BIG_ENDIAN 1
#endif

#include <string>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>


#if defined(OSS_OS_FAMILY_WINDOWS)
#include <Windows.h>
#endif

namespace OSS {

#if defined(_MSC_VER)
	//
	// Windows/Visual C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed __int64         Int64;
	typedef unsigned __int64       UInt64;
	#if defined(_WIN64)
		#define OSS_PTR_IS_64_BIT 1
		typedef signed __int64     IntPtr;
		typedef unsigned __int64   UIntPtr;
	#else
		typedef signed long        IntPtr;
		typedef unsigned long      UIntPtr;
	#endif
	#define OSS_HAVE_INT64 1
#elif defined(__GNUC__)
	//
	// Unix/GCC
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__LP64__)
		#define OSS_PTR_IS_64_BIT 1
		#define OSS_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define OSS_HAVE_INT64 1
#elif defined(__DECCXX)
	//
	// Compaq C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed __int64         Int64;
	typedef unsigned __int64       UInt64;
	#if defined(__VMS)
		#if defined(__32BITS)
			typedef signed long    IntPtr;
			typedef unsigned long  UIntPtr;
		#else
			typedef Int64          IntPtr;
			typedef UInt64         UIntPtr;
			#define OSS_PTR_IS_64_BIT 1
		#endif
	#else
		typedef signed long        IntPtr;
		typedef unsigned long      UIntPtr;
		#define OSS_PTR_IS_64_BIT 1
		#define OSS_LONG_IS_64_BIT 1
	#endif
	#define OSS_HAVE_INT64 1
#elif defined(__HP_aCC)
	//
	// HP Ansi C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__LP64__)
		#define OSS_PTR_IS_64_BIT 1
		#define OSS_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define OSS_HAVE_INT64 1
#elif defined(__SUNPRO_CC)
	//
	// SUN Forte C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__sparcv9)
		#define OSS_PTR_IS_64_BIT 1
		#define OSS_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define OSS_HAVE_INT64 1
#elif defined(__IBMCPP__) 
	//
	// IBM XL C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__64BIT__)
		#define OSS_PTR_IS_64_BIT 1
		#define OSS_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define OSS_HAVE_INT64 1
#elif defined(__sgi) 
	//
	// MIPSpro C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if _MIPS_SZLONG == 64
		#define OSS_PTR_IS_64_BIT 1
		#define OSS_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define OSS_HAVE_INT64 1
#endif

typedef void* OSS_HANDLE; /// Type that may safely hold a void *

void OSS_API OSS_init();
void OSS_API OSS_init(int argc, char** argv);
  /// Initialize the OSS subsystem.
  /// This must be called prior to any function call to OSS API

void OSS_argv(int* argc, char*** argv);

void OSS_API OSS_deinit();
  /// Deinitialize the OSS_API

void OSS_API OSS_register_init(boost::function<void()> func);
  /// Register a function that will be called when OSS_init is invoked

void OSS_API OSS_register_deinit(boost::function<void()> func);
  /// Register a function that will be called when OSS_deinit is invoked

//
// Debugging functions
//
namespace Private
{
  class TimedFuncTimer
  {
  public:
    TimedFuncTimer(const char* fileName, int lineNumber, const char* funcName);
    ~TimedFuncTimer();
    void flushElapsed(const char * label);
  private:
    std::string _funcName;
    std::string _fileName;
    int _lineNumber;
    OSS_HANDLE _stopWatch;
    UInt64 _pitStop;
  };
}
#define OSS_ASSERT BOOST_ASSERT
#define OSS_VERIFY BOOST_VERIFY
#define OSS_ASSERT_NULL(obj) OSS_ASSERT(obj != 0)
#define OSS_VERIFY_NULL(obj) OSS_VERIFY(obj != 0)
#ifdef _DEBUG
#define OSS_TIMED_FUNC OSS::Private::TimedFuncTimer funcTimer___(__FILE__, __LINE__, __FUNCTION__);
#define OSS_TIMED_FUNC_ELAPSED(label) funcTimer___.flushElapsed(label);
#else
#define OSS_TIMED_FUNC
#endif

} // OSS

#endif // OSS_H_INCLUDED

