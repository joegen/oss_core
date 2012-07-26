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


#include "OSS/ABNF/ABNFSIPTransportParam.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// transport-param   =  "transport="
///                     ( "udp" / "tcp" / "sctp" / "tls"
///                     / other-transport)
typedef ABNFStringLiteral<'u', 'd', 'p'> _pvar1;
typedef ABNFStringLiteral<'t', 'c', 'p'> _pvar2;
typedef ABNFStringLiteral<'s', 'c', 't', 'p'> _pvar3;
typedef ABNFStringLiteral<'t', 'l', 's'> _pvar4;
typedef ABNFAnyOfMultiple5<_pvar1, _pvar2, _pvar3, 
                           _pvar4, ABNF_SIP_other_transport > Parser;
static Parser _parser;

ABNFSIPTransportParam::ABNFSIPTransportParam()
{
}

char* ABNFSIPTransportParam::parse(const char* _t)
{
  return _parser.parse(_t);
}

} } //OSS::ABNF



