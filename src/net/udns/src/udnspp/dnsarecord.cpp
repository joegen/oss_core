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


#include <cassert>
#include <udnspp/dnsarecord.h>


#ifdef WINDOWS
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

namespace udnspp {


DNSARecordV4::DNSARecordV4()
{
}

DNSARecordV4::DNSARecordV4(dns_rr_a4* pRr)
{
  parseRR(pRr);
}


DNSARecordV4& DNSARecordV4::operator=(dns_rr_a4* pRr)
{
  parseRR(pRr);
  return *this;
}

void DNSARecordV4::parseRR(dns_rr_a4* pRr)
{
  assert(pRr);
  _cname = pRr->dnsa4_cname;
  _qname = pRr->dnsa4_qname;
  _ttl = pRr->dnsa4_ttl;

  _records.clear();
  for (int i = 0; i < pRr->dnsa4_nrr; i++)
  {
    char buf[6 * 5 + 4 * 4];
    _records.push_back(dns_ntop(AF_INET, (void*)&pRr->dnsa4_addr[i], buf, sizeof(buf)));
  }
}



DNSARecordV6::DNSARecordV6()
{
}

DNSARecordV6::DNSARecordV6(dns_rr_a6* pRr)
{
  parseRR(pRr);
}


DNSARecordV6& DNSARecordV6::operator=(dns_rr_a6* pRr)
{
  parseRR(pRr);
  return *this;
}

void DNSARecordV6::parseRR(dns_rr_a6* pRr)
{
  assert(pRr);
  _cname = pRr->dnsa6_cname;
  _qname = pRr->dnsa6_qname;
  _ttl = pRr->dnsa6_ttl;

  _records.clear();
  for (int i = 0; i < pRr->dnsa6_nrr; i++)
  {
    char buf[6 * 5 + 4 * 4];
    _records.push_back(dns_ntop(AF_INET6, (void*)&pRr->dnsa6_addr[i], buf, sizeof(buf)));
  }
}


} // namespace udns

