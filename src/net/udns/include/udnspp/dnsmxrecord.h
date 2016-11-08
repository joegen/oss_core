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

#ifndef UDNSPP_DNSMXRECORD_INCLUDED
#define UDNSPP_DNSMXRECORD_INCLUDED


#include <udnspp/dnsrrcommon.h>


namespace udnspp {


  struct MXRecord
  {
    int priority;		  // MX priority
    std::string name;	// MX name
  };

  typedef std::vector<MXRecord> DNSMXRecordList;

  class DNSMXRecord : public DNSRRCommon<DNSMXRecordList>
  {
  public:

    DNSMXRecord();

    DNSMXRecord(const DNSMXRecord& rr);

    DNSMXRecord(dns_rr_mx* pRr);

    ~DNSMXRecord();

    void parseRR(dns_rr_mx* pRr);

  };


} // namespace udns

#endif // UDNSPP_DNSMXRECORD_INCLUDED