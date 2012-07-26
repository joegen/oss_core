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


#include "OSS/ABNF/ABNFSIPQuotedPair.h"


namespace OSS {

namespace ABNF {


typedef ABNFRange<0x00, 0x09> _pvar1; ///%x00-09
typedef ABNFRange<0x0B, 0x0C> _pvar2; ///%x0B-0C
typedef ABNFRange<0x0E, 0x7F> _pvar3; ///%x0E-7F 
typedef ABNFAnyOfMultiple3<_pvar1, _pvar2, _pvar3> _pvar4; ///(%x00-09 / %x0B-0C / %x0E-7F)
static ABNFLRSequence2<ABNFCharComparison<'\\'>, _pvar4> _parser; ///"\" (%x00-09 / %x0B-0C / %x0E-7F)

ABNFSIPQuotedPair::ABNFSIPQuotedPair()
{
}

char* ABNFSIPQuotedPair::parse(const char* _t)
{
  /// quoted-pair  =  "\" (%x00-09 / %x0B-0C / %x0E-7F)
  return _parser.parse(_t);  
}


} } //OSS::ABNF

