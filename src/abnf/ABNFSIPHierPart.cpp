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


#include "OSS/ABNF/ABNFSIPHierPart.h"
#include "OSS/ABNF/ABNFSIPPathSegments.h"
#include "OSS/ABNF/ABNFSIPNetPath.h"
#include "OSS/ABNF/ABNFSIPAuthority.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// hier-part   =  ( net-path / abs-path ) [ "?" query ]
typedef ABNFAnyOf<ABNF_SIP_net_path, ABNF_SIP_abs_path> _pvar1;//( net-path / abs-path )
typedef ABNFLRSequence2<ABNFCharComparison<'?'>, ABNF_SIP_query> _pvar2;
typedef ABNFLRSequence2<_pvar1, ABNFLROptional<_pvar2> > Parser;//( net-path / abs-path ) [ "?" query ]
static Parser _parser;

ABNFSIPHierPart::ABNFSIPHierPart()
{
}

char* ABNFSIPHierPart::parse(const char* _t)
{
  return _parser.parse(_t);
}

char* ABNFSIPHierPart::parseTokens(const char* _t, std::vector<std::string>& tokens)
{
  return _parser.parseTokens(_t, tokens);
}


} } //OSS::ABNF



