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

#ifndef UDNSPP_DNSARECORD_INCLUDED
#define UDNSPP_DNSARECORD_INCLUDED


#include <OSS/UDNS/dnsrrcommon.h>


namespace OSS {
namespace UDNS {

typedef std::vector<std::string> DNSAddressList;
typedef DNSRRCommon<DNSAddressList> DNSARecord;

class DNSARecordV4 : public DNSARecord
{
public:
  DNSARecordV4();

  DNSARecordV4(dns_rr_a4* pRr);

  DNSARecordV4& operator=(dns_rr_a4* pRr);

  void parseRR(dns_rr_a4* pRr);
};

class DNSARecordV6 : public DNSARecord
{
public:
  DNSARecordV6();

  DNSARecordV6(dns_rr_a6* pRr);

  DNSARecordV6& operator=(dns_rr_a6* pRr);

  void parseRR(dns_rr_a6* pRr);
};


} } // namespace udns

#endif // UDNSPP_DNSARECORD_INCLUDED