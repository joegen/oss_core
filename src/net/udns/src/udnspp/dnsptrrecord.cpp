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


#include <udnspp/dnsptrrecord.h>


namespace udnspp {


DNSPTRRecord::DNSPTRRecord()
{
}

DNSPTRRecord::DNSPTRRecord(const DNSPTRRecord& rr) :
  DNSRRCommon<DNSPTRRecordList>(rr)
{
}

DNSPTRRecord::DNSPTRRecord(dns_rr_ptr* pRr)
{
  parseRR(pRr);
}

DNSPTRRecord::~DNSPTRRecord()
{
}

void DNSPTRRecord::parseRR(dns_rr_ptr* pRr)
{
  assert(pRr);
  _cname = pRr->dnsptr_cname;
  _qname = pRr->dnsptr_qname;
  _ttl = pRr->dnsptr_ttl;

  _records.clear();
  for (int i = 0; i < pRr->dnsptr_nrr; i++)
  {
    std::string rec = pRr->dnsptr_ptr[i];
  }
}

 
} // namespace udns

