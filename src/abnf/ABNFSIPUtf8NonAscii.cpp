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


#include "OSS/ABNF/ABNFSIPUtf8NonAscii.h"


namespace OSS {

namespace ABNF {


typedef ABNFAnyOfMultiple5<
  ABNFLRSequence2<ABNFRange<0xC0, 0xDF>,ABNFLoopUntil<ABNF_SIP_UTF8_CONT, ABNFLoopExitIfNul,1,1> >,
  ABNFLRSequence2<ABNFRange<0xE0, 0xEF>,ABNFLoopUntil<ABNF_SIP_UTF8_CONT, ABNFLoopExitIfNul,2,2> >,
  ABNFLRSequence2<ABNFRange<0xF0, 0xF7>,ABNFLoopUntil<ABNF_SIP_UTF8_CONT, ABNFLoopExitIfNul,3,3> >,
  ABNFLRSequence2<ABNFRange<0xF8, 0xFb>,ABNFLoopUntil<ABNF_SIP_UTF8_CONT, ABNFLoopExitIfNul,4,4> >,
  ABNFLRSequence2<ABNFRange<0xFC, 0xFD>,ABNFLoopUntil<ABNF_SIP_UTF8_CONT, ABNFLoopExitIfNul,5,5> > >
Parser;
static Parser _parser;
/// Satisfies RFC 3261 ABNF Rule for 
/// UTF8-NONASCII   =     %xC0-DF 1UTF8-CONT
///                    /  %xE0-EF 2UTF8-CONT
///                    /  %xF0-F7 3UTF8-CONT
///                    /  %xF8-Fb 4UTF8-CONT
///                    /  %xFC-FD 5UTF8-CONT

ABNFSIPUtf8NonAscii::ABNFSIPUtf8NonAscii()
{
}

char* ABNFSIPUtf8NonAscii::parse(const char* _t)
{  
  return _parser.parse(_t);
}


} } //OSS::ABNF
