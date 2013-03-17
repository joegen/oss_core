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


#ifndef ABNF_ABNF_INCLUDED
#define ABNF_ABNF_INCLUDED

#define OSS_ABNF_MAJOR_VERSION 1
#define OSS_ABNF_MINOR_VERSION 0
#define OSS_ABNF_BUILD_NUMBER 0

#include "OSS/OSS.h"

#if defined(_MSC_VER)
	#if !defined(OSS_NO_AUTOMATIC_LIBS) && !defined(OSS_ABNF_EXPORTS)
		#if defined(OSS_DLL)
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSABNFd.lib")
			#else
				#pragma comment(lib, "OSSABNF.lib")
			#endif
		#else
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSABNFmtd.lib")
			#else
				#pragma comment(lib, "OSSABNFmt.lib")
			#endif
		#endif
	#endif
#endif


#include "OSS/ABNF/ABNFBaseRule.h"
#include "OSS/ABNF/ABNFLRSequence.h"
#include "OSS/ABNF/ABNFAnyOf.h"
#include "OSS/ABNF/ABNFFindMatch.h"
#include "OSS/ABNF/ABNFLoopUntil.h"
#include "OSS/ABNF/ABNFWhileNot.h"


#endif // ABNF_ABNF_INCLUDED

