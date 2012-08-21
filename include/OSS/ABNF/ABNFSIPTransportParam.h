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


#ifndef ABNF_SIPSIPTransportParam_INCLUDED
#define ABNF_SIPSIPTransportParam_INCLUDED


#include "OSS/ABNF/ABNFSIPRules.h"
#include "OSS/ABNF/ABNFSIPToken.h"


namespace OSS {

namespace ABNF {


class OSS_API ABNFSIPTransportParam : public ABNFBaseRule
/// Satisfies RFC 3261 ABNF Rule for 
/// transport-param   =  "transport="
///                     ( "udp" / "tcp" / "sctp" / "tls"
///                     / other-transport)
{
public:
  ABNFSIPTransportParam();
  /// Creates a new ABNFSIPTransportParam Rule

  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied
};

typedef ABNFSIPTransportParam ABNF_SIP_transport_param;
typedef ABNF_SIP_token ABNF_SIP_other_transport;  /// other-transport   =  token

} } //OSS::ABNF
#endif //ABNF_SIPSIPTransportParam_INCLUDED
