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


#include "OSS/ABNF/ABNFSIPStatusCode.h"
#include "OSS/ABNF/ABNFSIPRequestURI.h"
#include "OSS/ABNF/ABNFSIPVersion.h"
#include "OSS/ABNF/ABNFSIPMethod.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// Status-Code     =  Informational
///               /   Redirection
///               /   Success
///               /   Client-Error
///               /   Server-Error
///               /   Global-Failure
///               /   extension-code
/// extension-code  =  3DIGIT
typedef ABNFLoopUntil<ABNF_SIP_DIGIT, ABNFLoopExitIfNul, 3, 3 > Parser;
static Parser _parser;

ABNFSIPStatusCode::ABNFSIPStatusCode()
{
}

char* ABNFSIPStatusCode::parse(const char* _t)
{
  return _parser.parse(_t);
}


} } //OSS::ABNF



