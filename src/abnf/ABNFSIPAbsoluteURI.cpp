// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
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


#include "OSS/ABNF/ABNFSIPAbsoluteURI.h"
#include "OSS/ABNF/ABNFSIPHierPart.h"
#include "OSS/ABNF/ABNFSIPOpaquePart.h"
#include "OSS/ABNF/ABNFSIPScheme.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// absoluteURI    =  scheme ":" ( hier-part / opaque-part )
typedef ABNFLRSequence2<ABNF_SIP_scheme, ABNFCharComparison<':'> > _pvar1;///scheme ":" 
typedef ABNFAnyOf<ABNF_SIP_hier_part, ABNF_SIP_opaque_part> _pvar2;///( hier-part / opaque-part )
typedef ABNFLRSequence2<_pvar1, _pvar2> Parser;///scheme ":" ( hier-part / opaque-part )
static Parser _parser;

ABNFSIPAbsoluteURI::ABNFSIPAbsoluteURI()
{
}

char* ABNFSIPAbsoluteURI::parse(const char* _t)
{
  return _parser.parse(_t);
}

char* ABNFSIPAbsoluteURI::parseTokens(const char* _t, std::vector<std::string>& tokens)
{
  return _parser.parseTokens(_t, tokens);
}


} } //OSS::ABNF



