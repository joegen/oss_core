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


#include "OSS/ABNF/ABNFSIPHexSeq.h"
#include "OSS/ABNF/ABNFSIPHex4.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// hexseq  =  hex4 *( ":" hex4)

typedef ABNFLRSequence2<ABNFCharComparison<':'>, ABNF_SIP_hex4> _pvar1;///":" hex4
typedef ABNFLoopUntil<_pvar1, ABNFLoopExitIfNul, 0, 1024> _pvar2;  ///*( ":" hex4)
typedef ABNFLRSequence2<ABNF_SIP_hex4, _pvar2> Parser;/// hexseq  =  hex4 *( ":" hex4)
static Parser _parser;

ABNFSIPHexSeq::ABNFSIPHexSeq()
{
}

char* ABNFSIPHexSeq::parse(const char* _t)
{
  return _parser.parse(_t);
}

} } //OSS::ABNF



