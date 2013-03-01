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


#include "OSS/ABNF/ABNFSIPQdText.h"
#include "OSS/ABNF/ABNFSIPUtf8NonAscii.h"


namespace OSS {

namespace ABNF {


/// RFC 3261 ABNF Rule for 
/// qdtext	=  	LWS    /   %x21   /   %x23-5B   /   %x5D-7E   /   UTF8-NONASCII
typedef ABNFAnyOfMultiple5<
  ABNF_SIP_LWS,
  ABNFCharComparison<0x21>,
  ABNFRange<0x23, 0x5B>,
  ABNFRange<0x5D, 0x7E>,
  ABNF_SIP_UTF8_NONASCII >
Parser;
static Parser _parser; 

ABNFSIPQdText::ABNFSIPQdText()
{
}

char* ABNFSIPQdText::parse(const char* _t)
{  
  return _parser.parse(_t);
}

} } //OSS::ABNF
