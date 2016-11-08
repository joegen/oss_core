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


#include <udnspp/dnsnaptrrecord.h>


namespace udnspp {

  
DNSNAPTRRecord::DNSNAPTRRecord()
{
}

DNSNAPTRRecord::DNSNAPTRRecord(const DNSNAPTRRecord& rr) :
  DNSRRCommon<DNSNAPTRRecordList>(rr)
{
}

DNSNAPTRRecord::DNSNAPTRRecord(dns_rr_naptr* pRr)
{
  parseRR(pRr);
}

DNSNAPTRRecord::~DNSNAPTRRecord()
{
}

void DNSNAPTRRecord::parseRR(dns_rr_naptr* pRr)
{
  assert(pRr);
  _cname = pRr->dnsnaptr_cname;
  _qname = pRr->dnsnaptr_qname;
  _ttl = pRr->dnsnaptr_ttl;

  _records.clear();
  for (int i = 0; i < pRr->dnsnaptr_nrr; i++)
  {
    NAPTRRecord rec;
    rec.order = pRr->dnsnaptr_naptr[i].order;
    rec.preference = pRr->dnsnaptr_naptr[i].preference;
    rec.flags = pRr->dnsnaptr_naptr[i].flags;
    rec.service = pRr->dnsnaptr_naptr[i].service;
    rec.regexp = pRr->dnsnaptr_naptr[i].regexp;
    rec.replacement = pRr->dnsnaptr_naptr[i].replacement;
    _records.push_back(rec);
  }
}


} // namespace udns

