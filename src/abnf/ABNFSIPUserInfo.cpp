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


#include "OSS/ABNF/ABNFSIPUserInfo.h"
#include "OSS/ABNF/ABNFSIPUser.h"
#include "OSS/ABNF/ABNFSIPPassword.h"


namespace OSS {

namespace ABNF {


/// RFC 3261 ABNF Rule for
/// userinfo         =  ( user / telephone-subscriber ) [ ":" password ] "@"
/// Take note that we did not include telephone-subscriber here because 
/// RFC 3261 stated
///    The BNF for telephone-subscriber can be found in RFC 2806 [9].  Note,
///    however, that any characters allowed there that are not allowed in
///    the user part of the SIP URI MUST be escaped.
/// It is therefore enough to just check for user rule and still satisfy the 
/// 'augmented' telephone-subscriber rule.

typedef ABNFLRSequence2<ABNFCharComparison<':'>, ABNF_SIP_password > _pvar1;///":" password
typedef ABNFCharComparison<'@'> _pvar2;
typedef ABNFLRSequence3<ABNF_SIP_user, ABNFLROptional<_pvar1>, _pvar2>Parser;///( user / telephone-subscriber ) [ ":" password ]
static Parser _parser;

ABNFSIPUserInfo::ABNFSIPUserInfo()
{
}

char* ABNFSIPUserInfo::parse(const char* _t)
{
  return _parser.parse(_t);
}

char* ABNFSIPUserInfo::parseTokens(const char* _t, std::vector<std::string>& tokens)
{
  return _parser.parseTokens(_t, tokens);
}


} } //OSS::ABNF
