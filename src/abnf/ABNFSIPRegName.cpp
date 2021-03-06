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


#include "OSS/ABNF/ABNFSIPRegName.h"
#include "OSS/ABNF/ABNFSIPUserInfo.h"
#include "OSS/ABNF/ABNFSIPHostPort.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// reg-name  =  1*( unreserved / escaped / "$" / "," / ";" / ":" / "@" / "&" / "=" / "+" )
typedef ABNFAnyOfChars<'$', ',', ';', ':', '@', '&', '=', '+'> _pvar1;
typedef ABNFAnyOfMultiple3<ABNF_SIP_unreserved, ABNF_SIP_escaped, _pvar1>_pvar2;
typedef ABNFLoopUntil<_pvar2, ABNFLoopExitIfNul, 1, 1024> Parser;
static Parser _parser;

ABNFSIPRegName::ABNFSIPRegName()
{
}

char* ABNFSIPRegName::parse(const char* _t)
{
  return _parser.parse(_t);
}

} } //OSS::ABNF



