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


#ifndef ABNF_SIPSIPMethod_INCLUDED
#define ABNF_SIPSIPMethod_INCLUDED


#include "OSS/ABNF/ABNFSIPRules.h"
#include "OSS/ABNF/ABNFSIPToken.h"


namespace OSS {

namespace ABNF {


class OSS_API ABNFSIPMethod : public ABNFBaseRule
/// Satisfies RFC 3261 ABNF Rule for 
/// Method            =  INVITEm / ACKm / OPTIONSm / BYEm
///                     / CANCELm / REGISTERm
///                     / extension-method
{
public:
  ABNFSIPMethod();
  /// Creates a new ABNFSIPMethod Rule

  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied
};

typedef ABNFSIPMethod ABNF_SIP_Method;
typedef ABNF_SIP_token ABNF_SIP_extension_method;  /// extension-method  =  token

} } //OSS::ABNF
#endif //ABNF_SIPSIPMethod_INCLUDED
