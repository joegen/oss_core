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


#include "OSS/ABNF/ABNFSIPCSeq.h"
#include "OSS/ABNF/ABNFSIPMethod.h"


namespace OSS {

namespace ABNF {


typedef ABNFLRSequence2<ABNFStringLiteral<'C', 'S', 'e', 'q'>, ABNF_SIP_HCOLON> _pvar1; // "CSeq" HCOLON
typedef ABNFLoopUntil<ABNF_SIP_DIGIT, ABNFLoopExitIfNul, 1, 1024> _pvar2; // 1*DIGIT
typedef ABNFLRSequence4<ABNFLROptional<_pvar1>, _pvar2, ABNF_SIP_LWS, ABNF_SIP_Method> Parser; // CSeq  =  "CSeq" HCOLON 1*DIGIT LWS Method
static Parser _parser;

ABNFSIPCSeq::ABNFSIPCSeq()
{
}

char* ABNFSIPCSeq::parse(const char* _t)
{
  return _parser.parse(_t);
}

char* ABNFSIPCSeq::parseTokens(const char* _t, std::vector<std::string>& tokens)
{
  return _parser.parseTokens(_t, tokens);
}


} } //OSS::ABNF


