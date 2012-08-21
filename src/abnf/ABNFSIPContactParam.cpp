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


#include "OSS/ABNF/ABNFSIPContactParam.h"
#include "OSS/ABNF/ABNFSIPGenericParams.h"
#include "OSS/ABNF/ABNFSIPNameAddr.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// contact-param  = (name-addr / addr-spec) generic-params
static ABNFLRSequence2< 
  ABNFAnyOf< ABNF_SIP_name_addr, ABNF_SIP_addr_spec >, 
  ABNFLROptional< ABNF_SIP_generic_params > > _parser;

ABNFSIPContactParam::ABNFSIPContactParam()
{
}

char* ABNFSIPContactParam::parse(const char* _t)
{
  return _parser.parse(_t);
}

char* ABNFSIPContactParam::parseTokens(const char* _t, std::vector<std::string>& tokens)
{
  return _parser.parseTokens(_t, tokens);
}

} } //OSS::ABNF


