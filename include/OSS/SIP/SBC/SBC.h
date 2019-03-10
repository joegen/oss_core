// OSS Software Solutions Application Programmer Interface
// 
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef SIP_SBC_INCLUDED
#define SIP_SBC_INCLUDED


#define OSS_SBC_MAJOR_VERSION 1
#define OSS_SBC_MINOR_VERSION 0
#define OSS_SBC_BUILD_NUMBER 0


#include "OSS/OSS.h"
#include "OSS/Persistent/Persistent.h"
#include "OSS/ABNF/ABNF.h"
#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/B2BUA/B2BUA.h"



//
// Automatically link SBC library.
//
#if defined(_MSC_VER)
	#if !defined(OSS_NO_AUTOMATIC_LIBS) && !defined(OSS_SBC_EXPORTS)
		#if defined(OSS_DLL)
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSSBCd.lib")
			#else
				#pragma comment(lib, "OSSSBC.lib")
			#endif
		#else
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSSBCmtd.lib")
			#else
				#pragma comment(lib, "OSSSBCmt.lib")
			#endif
		#endif
	#endif
#endif


#endif // SIP_SBC_INCLUDED



