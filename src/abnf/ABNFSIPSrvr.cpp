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


#include "OSS/ABNF/ABNFSIPSrvr.h"
#include "OSS/ABNF/ABNFSIPUserInfo.h"
#include "OSS/ABNF/ABNFSIPHostPort.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// srvr =  [ [ userinfo "@" ] hostport ]
typedef ABNFLRSequence2< ABNF_SIP_userinfo, ABNFCharComparison< '@'> >_pvar1;
typedef ABNFLRSequence2<ABNFLROptional<_pvar1>, ABNF_SIP_hostport> Parser;
static Parser _parser;

ABNFSIPSrvr::ABNFSIPSrvr()
{
}

char* ABNFSIPSrvr::parse(const char* _t)
{
  return _parser.parse(_t);
}

} } //OSS::ABNF



