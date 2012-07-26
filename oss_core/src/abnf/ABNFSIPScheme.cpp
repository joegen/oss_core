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


#include "OSS/ABNF/ABNFSIPScheme.h"


namespace OSS {

namespace ABNF {

/// Satisfies RFC 3261 ABNF Rule for 
/// scheme =  ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
typedef ABNFAnyOfChars<'+', '-', '.'> _pvar1;
typedef ABNFAnyOfMultiple3<ABNF_SIP_ALPHA, ABNF_SIP_DIGIT, _pvar1>_pvar2;///ALPHA / DIGIT / "+" / "-" / "."
typedef ABNFLoopUntil<_pvar2, ABNFLoopExitIfNul, 0, 1024> _pvar3;///*( ALPHA / DIGIT / "+" / "-" / "." )
typedef ABNFLRSequence2<ABNF_SIP_ALPHA, _pvar3> Parser;///ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )

static Parser _parser;

ABNFSIPScheme::ABNFSIPScheme()
{
}

char* ABNFSIPScheme::parse(const char* _t)
{
  return _parser.parse(_t);
}

} } //OSS::ABNF



