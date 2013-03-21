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


#ifndef ABNF_SIPSIPUserParam_INCLUDED
#define ABNF_SIPSIPUserParam_INCLUDED


#include "OSS/ABNF/ABNFSIPRules.h"
#include "OSS/ABNF/ABNFSIPToken.h"


namespace OSS {

namespace ABNF {


class OSS_API ABNFSIPUserParam : public ABNFBaseRule
/// Satisfies RFC 3261 ABNF Rule for 
/// user-param        =  "user=" ( "phone" / "ip" / other-user)
{
public:
  ABNFSIPUserParam();
  /// Creates a new ABNFSIPUserParam Rule

  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied
};

typedef ABNFSIPUserParam ABNF_SIP_user_param;
typedef ABNF_SIP_token ABNF_SIP_other_user;  /// other-user =  token

} } //OSS::ABNF
#endif //ABNF_SIPSIPUserParam_INCLUDED
