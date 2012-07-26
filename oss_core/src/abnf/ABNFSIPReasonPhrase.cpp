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


#include "OSS/ABNF/ABNFSIPReasonPhrase.h"
#include "OSS/ABNF/ABNFSIPUtf8NonAscii.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// Reason-Phrase   =  *(reserved / unreserved / escaped / UTF8-NONASCII / UTF8-CONT / SP / HTAB)
typedef ABNFAnyOfMultiple5<
  ABNF_SIP_reserved,
  ABNF_SIP_unreserved,
  ABNF_SIP_escaped,
  ABNF_SIP_UTF8_NONASCII,
  ABNF_SIP_UTF8_CONT> _pvar1;
typedef ABNFAnyOf<ABNF_SIP_SP, ABNF_SIP_HTAB> _pvar2;
typedef ABNFAnyOf<_pvar1, _pvar2> _pvar3;
typedef ABNFLoopUntil<_pvar3, ABNFLoopExitChars<'\r', '\n'>, 0, 1024> Parser;
static Parser _parser;

ABNFSIPReasonPhrase::ABNFSIPReasonPhrase()
{
}

char* ABNFSIPReasonPhrase::parse(const char* _t)
{
  return _parser.parse(_t);
}


} } //OSS::ABNF



