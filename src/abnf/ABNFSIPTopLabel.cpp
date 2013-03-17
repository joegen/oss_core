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


#include "OSS/ABNF/ABNFSIPTopLabel.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// toplabel  =  ALPHA / ALPHA *( alphanum / "-" ) alphanum

typedef ABNFAnyOf<ABNF_SIP_alphanum, ABNFCharComparison<'-'> > _pvar1; //alphanum / "-"
typedef ABNFLoopUntil<_pvar1, ABNFLoopExitIfNul, 0, 1024> _pvar2; //*( alphanum / "-" )
typedef ABNFLRSequence2<ABNF_SIP_ALPHA, _pvar2> _pvar3;// ALPHA *( alphanum / "-" ) alphanum
typedef ABNFAnyOf<_pvar3, ABNF_SIP_ALPHA> Parser;// toplabel  =  ALPHA / ALPHA *( alphanum / "-" ) alphanum
static Parser _parser;

ABNFSIPTopLabel::ABNFSIPTopLabel()
{
}

char* ABNFSIPTopLabel::parse(const char* _t)
{
  return _parser.parse(_t);
}

} } //OSS::ABNF



