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


#include "OSS/ABNF/ABNFSIPHexPart.h"
#include "OSS/ABNF/ABNFSIPHexSeq.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// hexpart  =  hexseq / hexseq "::" [ hexseq ] / "::" [ hexseq ]
typedef ABNFStrictStringLiteral<':', ':'> _pvar1; /// "::"
typedef ABNFLROptional<ABNF_SIP_hexseq> _pvar2; /// [ hexseq ]
typedef ABNFLRSequence3<ABNF_SIP_hexseq, _pvar1, _pvar2> _pvar3; /// hexseq "::" [ hexseq ]
typedef ABNFLRSequence2<_pvar1, _pvar2> _pvar4;/// "::" [ hexseq ]
typedef ABNFAnyOfMultiple3<_pvar4, _pvar3, ABNF_SIP_hexseq> Parser; /// hexpart  =  hexseq / hexseq "::" [ hexseq ] / "::" [ hexseq ]
static Parser _parser;

ABNFSIPHexPart::ABNFSIPHexPart()
{
}

char* ABNFSIPHexPart::parse(const char* _t)
{
  return _parser.parse(_t);
}

} } //OSS::ABNF



