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


#ifndef ABNF_SIPSIPStatusCode_INCLUDED
#define ABNF_SIPSIPStatusCode_INCLUDED


#include "OSS/ABNF/ABNFSIPRules.h"


namespace OSS {

namespace ABNF {


class OSS_API ABNFSIPStatusCode : public ABNFBaseRule
/// Satisfies RFC 3261 ABNF Rule for 
/// Status-Code     =  Informational
///               /   Redirection
///               /   Success
///               /   Client-Error
///               /   Server-Error
///               /   Global-Failure
///               /   extension-code
/// extension-code  =  3DIGIT
{
public:
  ABNFSIPStatusCode();
  /// Creates a new ABNFSIPStatusCode Rule

  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied
};

typedef ABNFSIPStatusCode ABNF_SIP_Status_Code;
typedef ABNFSIPStatusCode ABNF_SIP_extension_code;

} } //OSS::ABNF
#endif //ABNF_SIPSIPStatusCode_INCLUDED
