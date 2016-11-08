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


#include <udnspp/dnssrvrecord.h>

#ifdef WINDOWS
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif


namespace udnspp {


DNSSRVRecord::DNSSRVRecord()
{
}

DNSSRVRecord::DNSSRVRecord(const DNSSRVRecord& rr) :
  DNSRRCommon<DNSSRVRecordList>(rr)
{
}

DNSSRVRecord::DNSSRVRecord(dns_rr_srv* pRr)
{
  parseRR(pRr);
}

DNSSRVRecord::~DNSSRVRecord()
{
}

void DNSSRVRecord::parseRR(dns_rr_srv* pRr)
{
  assert(pRr);
  _cname = pRr->dnssrv_cname;
  _qname = pRr->dnssrv_qname;
  _ttl = pRr->dnssrv_ttl;

  _records.clear();
  for (int i = 0; i < pRr->dnssrv_nrr; i++)
  {
    SRVRecord rec;
    rec.name = pRr->dnssrv_srv[i].name;
    rec.port = pRr->dnssrv_srv[i].port;
    rec.priority = pRr->dnssrv_srv[i].priority;
    rec.weight = pRr->dnssrv_srv[i].weight;
    _records.push_back(rec);
  }
}

} // namespace udns
