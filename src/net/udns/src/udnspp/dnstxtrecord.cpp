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


#include <udnspp/dnstxtrecord.h>

#ifdef WINDOWS
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif


namespace udnspp {


  DNSTXTRecord::DNSTXTRecord()
  {
  }

  DNSTXTRecord::DNSTXTRecord(const DNSTXTRecord& rr) :
    DNSRRCommon<DNSTXTRecordList>(rr)
  {
  }

  DNSTXTRecord::DNSTXTRecord(dns_rr_txt* pRr)
  {
    parseRR(pRr);
  }

  DNSTXTRecord::~DNSTXTRecord()
  {
  }

  void DNSTXTRecord::parseRR(dns_rr_txt* pRr)
  {
    assert(pRr);
    _cname = pRr->dnstxt_cname;
    _qname = pRr->dnstxt_qname;
    _ttl = pRr->dnstxt_ttl;

    _records.clear();
    for (int i = 0; i < pRr->dnstxt_nrr; i++)
    {

      if (pRr->dnstxt_txt[i].len)
      {
        std::string txt(pRr->dnstxt_txt[i].txt, pRr->dnstxt_txt[i].txt + pRr->dnstxt_txt[i].len);
        _records.push_back(txt);
      } 
    }
  }

} // namespace udns
