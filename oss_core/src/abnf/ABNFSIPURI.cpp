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


#include "OSS/ABNF/ABNFSIPURI.h"
#include "OSS/ABNF/ABNFSIPUserInfo.h"
#include "OSS/ABNF/ABNFSIPHostPort.h"
#include "OSS/ABNF/ABNFSIPURIParameters.h"
#include "OSS/ABNF/ABNFSIPURIHeaders.h"
#include "OSS/ABNF/ABNFSIPToken.h"
#include "OSS/ABNF/ABNFSIPScheme.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// SIP-URI  =  ("sip:" / "sips:" / token ":") [ userinfo ] hostport uri-parameters [ headers ]
typedef ABNFLRSequence2<ABNF_SIP_scheme, ABNFCharComparison<':'> > _pvar1; /// "sip:"
typedef ABNFLROptional<ABNF_SIP_userinfo> _pvar2;     /// [ userinfo ]
typedef ABNFLROptional<ABNF_SIP_headers> _pvar3;      /// [ headers ]
typedef ABNFLRSequence5<_pvar1, _pvar2, ABNF_SIP_hostport, ABNFLROptional<ABNF_SIP_uri_parameters>, _pvar3> Parser;
static Parser _parser;


ABNFSIPURI::ABNFSIPURI()
{
}

char* ABNFSIPURI::parse(const char* _t)
{
  return _parser.parse(_t);
}

char* ABNFSIPURI::parseTokens(const char* _t, std::vector<std::string>& tokens)
{
  return _parser.parseTokens(_t, tokens);
}

} } //OSS::ABNF



