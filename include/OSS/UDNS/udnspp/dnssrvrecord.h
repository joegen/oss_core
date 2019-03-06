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

#ifndef UDNSPP_DNSSRVRECORD_INCLUDED
#define UDNSPP_DNSSRVRECORD_INCLUDED


#include <udnspp/dnsrrcommon.h>



namespace udnspp {

struct SRVRecord
{
  int priority;		  // SRV priority
  int weight;		    // SRV weight
  int port;		      // SRV port
  std::string name;	// SRV name
};

typedef std::vector<SRVRecord> DNSSRVRecordList;

class DNSSRVRecord : public DNSRRCommon<DNSSRVRecordList>
{
public:

  DNSSRVRecord();

  DNSSRVRecord(const DNSSRVRecord& rr);

  DNSSRVRecord(dns_rr_srv* pRr);

  ~DNSSRVRecord();

  void parseRR(dns_rr_srv* pRr);

};


} // namespace udns

#endif // UDNSPP_DNSSRVRECORD_INCLUDED