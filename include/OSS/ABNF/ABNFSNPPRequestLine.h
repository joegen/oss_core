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


#ifndef ABNF_SNPPREQUESTLINE_H_INCLUDED
#define ABNF_SNPPREQUESTLINE_H_INCLUDED


#include "OSS/ABNF/ABNFSIPRules.h"


namespace OSS {

namespace ABNF {


class OSS_API ABNFSNPPRequestLine : public ABNFBaseRule
{
public:
  ABNFSNPPRequestLine();


  char* parse(const char* _t);
  /// Returns the next off-set if the rule is satisfied

  char* parseTokens(const char* _t, std::vector<std::string>& tokens);
  /// Fills tokens vector with parsed string.
  /// Returns the next off-set if the rule is satisfied
};

typedef ABNFSNPPRequestLine ABNF_SNPP_Request_Line;

} } //OSS::ABNF
#endif //ABNF_SIPSIPRequestLine_INCLUDED
