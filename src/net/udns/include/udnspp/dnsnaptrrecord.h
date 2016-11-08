// Library: libudnspp
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

#ifndef UDNSPP_DNSNAPTRRECORD_INCLUDED
#define UDNSPP_DNSNAPTRRECORD_INCLUDED


#include <udnspp/dnsrrcommon.h>


namespace udnspp {

  struct NAPTRRecord
  {
    int order;		            // NAPTR order 
    int preference;	          // NAPTR preference
    std::string flags;		    // NAPTR flags
    std::string service;	    // NAPTR service
    std::string regexp;		    // NAPTR regexp
    std::string replacement;	// NAPTR replacement
  };

  typedef std::vector<NAPTRRecord> DNSNAPTRRecordList;

  class DNSNAPTRRecord : public DNSRRCommon<DNSNAPTRRecordList>
  {
  public:

    DNSNAPTRRecord();

    DNSNAPTRRecord(const DNSNAPTRRecord& rr);

    DNSNAPTRRecord(dns_rr_naptr* pRr);

    ~DNSNAPTRRecord();

    void parseRR(dns_rr_naptr* pRr);

  };


} // namespace udns

#endif // UDNSPP_DNSNAPTRRECORD_INCLUDED