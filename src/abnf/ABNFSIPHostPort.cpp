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


#include "OSS/ABNF/ABNFSIPHostPort.h"
#include "OSS/ABNF/ABNFSIPPort.h"
#include "OSS/ABNF/ABNFSIPHost.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// hostport         =  host [ ":" port ]


typedef ABNFLRSequence2<ABNFCharComparison<':'>, ABNF_SIP_port> _pvar1; // ":" port
typedef ABNFLRSequence2<ABNF_SIP_host, ABNFLROptional<_pvar1> > Parser;
static Parser _parser;


ABNFSIPHostPort::ABNFSIPHostPort()
{
}

char* ABNFSIPHostPort::parse(const char* _t)
{
  return _parser.parse(_t);
}

char* ABNFSIPHostPort::parseTokens(const char* _t, std::vector<std::string>& tokens)
{
  return _parser.parseTokens(_t, tokens);
}
  /// Fills tokens vector with parsed string.
  /// Returns the next off-set if the rule is satisfied

} } //OSS::ABNF



