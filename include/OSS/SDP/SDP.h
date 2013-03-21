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


#ifndef OSS_SDP_INCLUDED
#define OSS_SDP_INCLUDED


#define OSS_SDP_MAJOR_VERSION 1
#define OSS_SDP_MINOR_VERSION 0
#define OSS_SDP_BUILD_NUMBER 0


#include "OSS/OSS.h"

//
// Automatically link SDP library.
//
#if defined(_MSC_VER)
	#if !defined(OSS_NO_AUTOMATIC_LIBS) && !defined(OSS_SDP_EXPORTS)
		#if defined(OSS_DLL)
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSSDPd.lib")
			#else
				#pragma comment(lib, "OSSSDP.lib")
			#endif
		#else
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSSDPmtd.lib")
			#else
				#pragma comment(lib, "OSSSDPmt.lib")
			#endif
		#endif
	#endif
#endif


#endif // OSS_SDP_INCLUDED





