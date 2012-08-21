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


#ifndef SIP_SIPARSER_INCLUDED
#define SIP_SIPARSER_INCLUDED


#define OSS_PARSER_MAJOR_VERSION 1
#define OSS_PARSER_MINOR_VERSION 0
#define OSS_PARSER_BUILD_NUMBER 0


#include "OSS/OSS.h"
#include "OSS/ABNF/ABNF.h"


//
// Automatically link Parser library.
//
#if defined(_MSC_VER)
	#if !defined(OSS_NO_AUTOMATIC_LIBS) && !defined(OSS_SIP_PARSER_EXPORTS)
		#if defined(OSS_DLL)
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSSIPParserd.lib")
			#else
				#pragma comment(lib, "OSSSIPParser.lib")
			#endif
		#else
			#if defined(_DEBUG)
				#pragma comment(lib, "OSSSIPParsermtd.lib")
			#else
				#pragma comment(lib, "OSSSIPParsermt.lib")
			#endif
		#endif
	#endif
#endif


#endif // SIP_SIPARSER_INCLUDED

