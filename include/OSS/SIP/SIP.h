// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
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


#ifndef SIP_SIP_INCLUDED
#define SIP_SIP_INCLUDED


#define OSS_SIP_MAJOR_VERSION 1
#define OSS_SIP_MINOR_VERSION 0
#define OSS_SIP_BUILD_NUMBER 0


#include "OSS/OSS.h"
#include "OSS/ABNF/ABNF.h"
#include "OSS/SIP/Parser.h"


//
// Automatically link SIP library.
//
#if defined(_MSC_VER)
	#if !defined(OSS_NO_AUTOMATIC_LIBS) && !defined(OSS_SIP_EXPORTS)
		#if defined(OSS_DLL)
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSSIPFSMd.lib")
			#else
				#pragma comment(lib, "OSSSIPFSM.lib")
			#endif
		#else
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSSIPFSMmtd.lib")
			#else
				#pragma comment(lib, "OSSSIPFSMmt.lib")
			#endif
		#endif
	#endif
#endif

#define OSS_SIP_MAX_PACKET_SIZE 1024 * 1000

#endif // SIP_SIP_INCLUDED

