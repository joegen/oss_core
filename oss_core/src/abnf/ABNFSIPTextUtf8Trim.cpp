// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSABNF
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


#include "OSS/ABNF/ABNFSIPTextUtf8Trim.h"
#include "OSS/ABNF/ABNFSIPTextUtf8Char.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// TEXT-UTF8-TRIM  =  1*TEXT-UTF8char *(*LWS TEXT-UTF8char)
typedef ABNFLoopUntil<ABNF_SIP_TEXT_UTF8char, ABNFLoopExitIfNul, 1, 1024> _pvar1; ///1*TEXT-UTF8char
typedef ABNFLoopUntil<ABNF_SIP_LWS, ABNFLoopExitIfNul, 0, 1024> _pvar2; ///*LWS
typedef ABNFLRSequence2<_pvar2, ABNF_SIP_TEXT_UTF8char> _pvar3;///*LWS TEXT-UTF8char
typedef ABNFLoopUntil<_pvar3, ABNFLoopExitIfNul, 0, 1024> _pvar4;///*(*LWS TEXT-UTF8char)
typedef ABNFLRSequence2<_pvar1, _pvar4> Parser;///1*TEXT-UTF8char *(*LWS TEXT-UTF8char)
static Parser _parser;

ABNFSIPTextUtf8Trim::ABNFSIPTextUtf8Trim()
{
}

char* ABNFSIPTextUtf8Trim::parse(const char* _t)
{  
  return _parser.parse(_t);
}


} } //OSS::ABNF
