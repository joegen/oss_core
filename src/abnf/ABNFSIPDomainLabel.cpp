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


#include "OSS/ABNF/ABNFSIPDomainLabel.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// domainlabel =  alphanum / alphanum *( alphanum / "-" ) alphanum

#if 0
typedef ABNFAnyOf<ABNF_SIP_alphanum, ABNFCharComparison<'-'> > _pvar1; // alphanum / "-"
typedef ABNFLoopUntil<_pvar1, ABNFLoopExitIfNul, 0, 1024> _pvar2; // *( alphanum / "-" )
typedef ABNFLRSequence3<ABNF_SIP_alphanum, _pvar2, ABNF_SIP_alphanum> _pvar3;// alphanum *( alphanum / "-" ) alphanum
typedef ABNFAnyOf<ABNF_SIP_alphanum, _pvar3> Parser; // domainlabel =  alphanum / alphanum *( alphanum / "-" ) alphanum
static Parser _parser;
#endif

typedef ABNFLoopUntil< ABNFAnyOf< ABNF_SIP_alphanum, ABNFCharDash >, ABNFLoopExitIfNul, 0, 1024> Parser;
static Parser _parser;

ABNFSIPDomainLabel::ABNFSIPDomainLabel()
{
}

char* ABNFSIPDomainLabel::parse(const char* _t)
{
  return _parser.parse(_t);
}

} } // OSS::ABNF



