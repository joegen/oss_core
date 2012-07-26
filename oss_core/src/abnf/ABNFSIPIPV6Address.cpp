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


#include "OSS/ABNF/ABNFSIPIPV6Address.h"
#include "OSS/ABNF/ABNFSIPIPV4Address.h"
#include "OSS/ABNF/ABNFSIPHexPart.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// IPv6address  =  hexpart [ ":" IPv4address ]typedef ABNFLoopUntil<ABNF_SIP_DIGIT, ABNFLoopExitIfNul, 1, 3> _pvar1; // 1*3DIGIT
typedef ABNFLoopUntil<ABNF_SIP_DIGIT, ABNFLoopExitIfNul, 1, 3> _pvar1;
typedef ABNFLRSequence2<ABNFCharComparison<'.'>,_pvar1> _pvar2;
typedef ABNFLRSequence3<_pvar2, _pvar2, _pvar2> _pvar3;
typedef ABNFLRSequence2<ABNF_SIP_hexpart, ABNFLROptional<_pvar3> > Parser;
static Parser _parser;

ABNFSIPIPV6Address::ABNFSIPIPV6Address()
{
}

char* ABNFSIPIPV6Address::parse(const char* _t)
{
  return _parser.parse(_t);
}

} } //OSS::ABNF



