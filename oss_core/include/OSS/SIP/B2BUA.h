// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSS Core SDK.
//
// Copyright (c) OSS Software Solutions
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


#ifndef SIP_B2BUA_INCLUDED
#define SIP_B2BUA_INCLUDED


#define OSS_B2BUA_MAJOR_VERSION 1
#define OSS_B2BUA_MINOR_VERSION 0
#define OSS_B2BUA_BUILD_NUMBER 0

#include "OSS/OSS.h"
#include "OSS/ABNF/ABNF.h"
#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIP.h"

//
// Automatically link SIP library.
//
#if defined(_MSC_VER)
	#if !defined(OSS_NO_AUTOMATIC_LIBS) && !defined(OSS_B2BUA_EXPORTS)
		#if defined(OSS_DLL)
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSB2BUAd.lib")
			#else
				#pragma comment(lib, "OSSB2BUA.lib")
			#endif
		#else
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSB2BUAmtd.lib")
			#else
				#pragma comment(lib, "OSSB2BUAmt.lib")
			#endif
		#endif
	#endif
#endif


#endif // SIP_B2BUA_INCLUDED


