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


#include "OSS/ABNF/ABNFSIPQuotedString.h"
#include "OSS/ABNF/ABNFSIPQdText.h"
#include "OSS/ABNF/ABNFSIPQuotedPair.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// quoted-string  =  SWS DQUOTE *(qdtext / quoted-pair ) DQUOTE
typedef ABNFAnyOf<ABNF_SIP_qdtext, ABNF_SIP_quoted_pair> _pvar1;///qdtext / quoted-pair
typedef ABNFLoopUntil<_pvar1, ABNFLoopExitChars<'\"'>, 0, 1024> _pvar2;///*(qdtext / quoted-pair )
typedef ABNFLRSequence4<ABNF_SIP_SWS, ABNF_SIP_DQUOTE, _pvar2, ABNF_SIP_DQUOTE> Parser;///SWS DQUOTE *(qdtext / quoted-pair ) DQUOTE
static Parser _parser;

ABNFSIPQuotedString::ABNFSIPQuotedString()
{
}

char* ABNFSIPQuotedString::parse(const char* _t)
{  
  return _parser.parse(_t);
}


} } //OSS::ABNF

