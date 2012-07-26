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


#include "OSS/ABNF/ABNFSIPTextUtf8Char.h"
#include "OSS/ABNF/ABNFSIPUtf8NonAscii.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// TEXT-UTF8char   =  %x21-7E / UTF8-NONASCII
typedef ABNFRange<0x21, 0x7E> _pvar1;///%x21-7E
typedef ABNFAnyOf<_pvar1, ABNF_SIP_UTF8_NONASCII> Parser;/// %x21-7E / UTF8-NONASCII
static Parser _parser;


ABNFSIPTextUtf8Char::ABNFSIPTextUtf8Char()
{
}

char* ABNFSIPTextUtf8Char::parse(const char* _t)
{  
  return _parser.parse(_t);
}


} } //OSS::ABNF
