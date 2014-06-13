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

#include <list>
#include <sstream>
#include "OSS/DNS.h"
#include "OSS/Net.h"
#include "OSS/Core.h"
#include "Poco/ExpireCache.h"

#if OSS_OS_FAMILY_WINDOWS
  #include <windns.h>
#else
  #include <arpa/nameser.h>
  #include <resolv.h>
  #include <arpa/nameser_compat.h>
  #define OSS_HAS_RESOLV_H
#endif  // OSS_OS_FAMILY_WINDOWS



namespace OSS {
namespace Private {

#ifdef OSS_HAS_RESOLV_H
#ifndef T_SRV
#define T_SRV   33
#endif

#ifndef T_NAPTR
#define T_NAPTR   35
#endif


#define DNS_STATUS  int
#define DNS_TYPE_SRV  T_SRV
#define DNS_TYPE_MX  T_MX
#define DNS_TYPE_A  T_A
#define DNS_TYPE_NAPTR  T_NAPTR
#define DnsFreeRecordList 0
#define DNS_QUERY_STANDARD 0
#define DNS_QUERY_BYPASS_CACHE 0

typedef Poco::ExpireCache<std::string, dns_srv_record_list> DnsSrvExpireCache;
typedef Poco::ExpireCache<std::string, dns_host_record_list> DnsHostExpireCache;
typedef Poco::SharedPtr<dns_host_record_list> DnsHostPtr;
typedef Poco::SharedPtr<dns_srv_record_list> DnsSrvPtr;

typedef struct _DnsAData {
  unsigned long IpAddress;
} DNS_A_DATA;

typedef struct {
  char   pNameExchange[MAXDNAME];
  unsigned short   wPreference;
} DNS_MX_DATA;

typedef struct {
  char pNameHost[MAXDNAME];
} DNS_PTR_DATA;

typedef struct _DnsSRVData {
  char   pNameTarget[MAXDNAME];
  unsigned short   wPriority;
  unsigned short   wWeight;
  unsigned short   wPort;
} DNS_SRV_DATA;

typedef struct _DnsNULLData {
  unsigned long  dwByteCount;
  char   data[1];
} DNS_NULL_DATA;

typedef struct _DnsRecordFlags
{
  unsigned   Section     : 2;
  unsigned   Delete      : 1;
  unsigned   CharSet     : 2;
  unsigned   Unused      : 3;
  unsigned   Reserved    : 24;
} DNS_RECORD_FLAGS;

typedef enum _DnsSection
{
  DnsSectionQuestion,
  DnsSectionAnswer,
  DnsSectionAuthority,
  DnsSectionAddtional,
} DNS_SECTION;


class DnsRecord {
  public:
    DnsRecord * pNext;
    char        pName[MAXDNAME];
    unsigned short        wType;
    unsigned short        wDataLength;

    union {
      unsigned long               DW;     ///< flags as unsigned long
      DNS_RECORD_FLAGS    S;      ///< flags as structure
    } Flags;

    union {
      DNS_A_DATA     A;
      DNS_MX_DATA    MX;
      DNS_PTR_DATA   NS;
      DNS_SRV_DATA   SRV;
      DNS_NULL_DATA  Null;
    } Data;
};

typedef DnsRecord * DNS_RECORD;

static bool GetDN(const unsigned char * reply, const unsigned char * replyEnd, unsigned char * & cp, char * buff)
{
  int len = dn_expand(reply, replyEnd, cp, buff, MAXDNAME);
  if (len < 0)
    return false;
  cp += len;
  return true;
}

static bool ProcessDNSRecords(
        const unsigned char * reply,
        const unsigned char * replyEnd,
              unsigned char * cp,
            size_t anCount,
            size_t nsCount,
            size_t arCount,
     DNS_RECORD * results)
{
  DNS_RECORD lastRecord = 0;

  size_t rrCount = anCount + nsCount + arCount;
  nsCount += anCount;
  arCount += nsCount;

  size_t i;
  for (i = 0; i < rrCount; i++) {

    int section;
    if (i < anCount)
      section = DnsSectionAnswer;
    else if (i < nsCount)
      section = DnsSectionAuthority;
    else // if (i < arCount)
      section = DnsSectionAddtional;

    // get the name
    char pName[MAXDNAME];
    if (!GetDN(reply, replyEnd, cp, pName))
      return false;

    // get other common parts of the record
    unsigned short  type;
    unsigned short  dlen;

    GETSHORT(type,     cp);
    GETSHORT(dlen,     cp);

    unsigned char * data = cp;
    cp += dlen;

    DNS_RECORD newRecord  = 0;

    switch (type) {
      default:
        newRecord = (DNS_RECORD)malloc(sizeof(DnsRecord) + sizeof(unsigned long) + dlen);
        newRecord->Data.Null.dwByteCount = dlen;
        memcpy(&newRecord->Data, data, dlen);
        break;

      case T_SRV:
        newRecord = (DNS_RECORD)malloc(sizeof(DnsRecord));
        memset(newRecord, 0, sizeof(DnsRecord));
        GETSHORT(newRecord->Data.SRV.wPriority, data);
        GETSHORT(newRecord->Data.SRV.wWeight, data);
        GETSHORT(newRecord->Data.SRV.wPort, data);
        if (!GetDN(reply, replyEnd, data, newRecord->Data.SRV.pNameTarget)) {
          free(newRecord);
          return false;
        }
        break;

      case T_MX:
        newRecord = (DNS_RECORD)malloc(sizeof(DnsRecord));
        memset(newRecord, 0, sizeof(DnsRecord));
        GETSHORT(newRecord->Data.MX.wPreference,  data);
        if (!GetDN(reply, replyEnd, data, newRecord->Data.MX.pNameExchange)) {
          free(newRecord);
          return false;
        }
        break;

      case T_A:
        newRecord = (DNS_RECORD)malloc(sizeof(DnsRecord));
        memset(newRecord, 0, sizeof(DnsRecord));
        GETLONG(newRecord->Data.A.IpAddress, data);
        break;

      case T_NS:
        newRecord = (DNS_RECORD)malloc(sizeof(DnsRecord));
        memset(newRecord, 0, sizeof(DnsRecord));
        if (!GetDN(reply, replyEnd, data, newRecord->Data.NS.pNameHost)) {
          delete newRecord;
          return false;
        }
        break;
    }

    // initialise the new record
    if (newRecord != 0) {
      newRecord->wType = type;
      newRecord->Flags.S.Section = section;
      newRecord->pNext = 0;
      strcpy(newRecord->pName, pName);

      bool hasPlaceHolder = false;
      if (*results == 0)
      {
        *results = newRecord;
        hasPlaceHolder = true;
      }
      else if (lastRecord != 0)
      {
        lastRecord->pNext = newRecord;
        hasPlaceHolder = true;
      }

      if (hasPlaceHolder)
      {
        lastRecord = newRecord;
        newRecord = 0;
      }
      else
      {
        //
        // This can never happen but we need to satisfy coverity scan.
        //
        free(newRecord);
        newRecord = 0;
      }
    }
  }

  return true;
}

void DnsRecordListFree(DNS_RECORD rec, int /* FreeType */)
{
  while (rec != 0) {
    DNS_RECORD next = rec->pNext;
    free(rec);
    rec = next;
  }
}

DNS_STATUS DnsQuery_A(const char * service,
                              unsigned short requestType,
                             unsigned long options,
                            void *,
                     DNS_RECORD * results,
                            void *)
{
  if (results == 0)
    return -1;

  *results = 0;

  struct __res_state statbuf;
  res_ninit(&statbuf);

  union {
    HEADER hdr;
    unsigned char buf[PACKETSZ];
  } reply;

  int replyLen = res_nsearch(&statbuf, service, C_IN, requestType, (unsigned char *)&reply, sizeof(reply));

  if (replyLen < 1)
    return -1;

  unsigned char * replyStart = reply.buf;
  unsigned char * replyEnd   = reply.buf + replyLen;
  unsigned char * cp         = reply.buf + sizeof(HEADER);

  // ignore questions in response
  uint16_t i;
  for (i = 0; i < ntohs(reply.hdr.qdcount); i++) {
    char qName[MAXDNAME];
    if (!GetDN(replyStart, replyEnd, cp, qName))
      return -1;
    cp += QFIXEDSZ;
  }

  if (!ProcessDNSRecords(
       replyStart,
       replyEnd,
       cp,
       ntohs(reply.hdr.ancount),
       ntohs(reply.hdr.nscount),
       ntohs(reply.hdr.arcount),
       results)) {
    DnsRecordListFree(*results, 0);
    res_nclose(&statbuf);
    return -1;
  }

  res_nclose(&statbuf);
  return 0;
}


#endif // OSS_HAS_RESOLV_H
} }  // OSS::Private

namespace OSS {

//
// Exposed Functions
//
dns_host_record_list dns_lookup_host(const std::string& host)
{
  static OSS::Private::DnsHostExpireCache hostCache(3600 * 1000);

  dns_host_record_list results;
  IPAddress addr(host);
  if (addr.isValid())
  {
    results.push_back(host);
    return results;
  }

  if (host.empty())
  {
    //
    // If the host is empty, treat as a flush request
    //
    hostCache.clear();
    return results;
  }

  //
  // Check if it is cached
  //
  OSS::Private::DnsHostPtr cached = hostCache.get(host);
  if (cached)
    return *cached;


  boost::asio::ip::tcp::resolver::query query(host, "0");
  boost::asio::ip::tcp::resolver::iterator endpoint_iterator = OSS::net_resolver().resolve(query);
  boost::asio::ip::tcp::resolver::iterator end;
  while (endpoint_iterator != end)
  {
    boost::asio::ip::tcp::endpoint ep = *endpoint_iterator;
    results.push_back(ep.address().to_string());
    endpoint_iterator++;
  }

  if (!results.empty())
  {
    //
    // Cache it
    //
    hostCache.add(host, OSS::Private::DnsHostPtr(new dns_host_record_list(results)));
  }
  return results;
}

bool handle_dns_srv_record(OSS::Private::DNS_RECORD dnsRecord, OSS::Private::DNS_RECORD results, dns_srv_record& record)
{
  if (
      (dnsRecord->Flags.S.Section == OSS::Private::DnsSectionAnswer) &&
      (dnsRecord->wType == DNS_TYPE_SRV) &&
      (strlen(dnsRecord->Data.SRV.pNameTarget) > 0) &&
      (strcmp(dnsRecord->Data.SRV.pNameTarget, ".") != 0)
      ) {
    record.get<0>() = std::string(dnsRecord->Data.SRV.pNameTarget);
    record.get<2>()     = dnsRecord->Data.SRV.wPort;
    record.get<3>() = dnsRecord->Data.SRV.wPriority;
    record.get<4>()   = dnsRecord->Data.SRV.wWeight;

    // see if any A records match this hostname
    OSS::Private::DNS_RECORD aRecord = results;
    while (aRecord != 0) {
      if ((dnsRecord->Flags.S.Section == OSS::Private::DnsSectionAddtional) && (dnsRecord->wType == DNS_TYPE_A)) {
        record.get<1>() = dnsRecord->Data.A.IpAddress;
        break;
      }
      aRecord = aRecord->pNext;
    }
    // if no A record found, then get address the hard way
    boost::asio::ip::tcp::resolver::query query(record.get<0>(), "0");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = OSS::net_resolver().resolve(query);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator_end;
    if (endpoint_iterator != endpoint_iterator_end)
    {
      boost::asio::ip::tcp::endpoint ep = *endpoint_iterator;
      record.get<1>() = ep.address().to_string();
    }
    return true;
  }
  return false;
}

bool dns_srv_lookup(const std::string& name, dns_srv_record_list& recordList)
{
  static OSS::Private::DnsSrvExpireCache srvCache(3600 * 1000);

  if (name.empty())
  {
    //
    // Treat an empty name as a flush request
    //
    srvCache.clear();
    //
    // Return false so that it does not break any accidental call to flush with empty name.
    // A real request to flush wont care about the return value
    //
    return false;
  }

  recordList.clear();

  //
  // Check if we have something in cache
  //
  OSS::Private::DnsSrvPtr cached = srvCache.get(name);
  if (cached)
  {
    recordList = *cached;
    return !recordList.empty();
  }


  OSS::Private::DNS_RECORD results = 0;
  DNS_STATUS status = DnsQuery_A(name.c_str(),
                                 DNS_TYPE_SRV,
                                 DNS_QUERY_STANDARD,
                                 0,
                                 &results,
                                 0);
  if (status != 0)
    return false;

  // find records matching the correct type
  OSS::Private::DNS_RECORD dnsRecord = results;
  while (dnsRecord != 0)
  {
    dns_srv_record record;
    if (!handle_dns_srv_record(dnsRecord, results, record) )
      break;
    recordList.insert(record);
    dnsRecord = dnsRecord->pNext;
  }

  if (results != 0)
    DnsRecordListFree(results, DnsFreeRecordList);

  if (!recordList.empty())
  {
    //
    // cache it
    //
    srvCache.add(name, OSS::Private::DnsSrvPtr(new dns_srv_record_list(recordList)));
    return true;
  }

  return !recordList.empty();
}

dns_srv_record_list dns_lookup_srv(const std::string& query)
{
  dns_srv_record_list answer;
  dns_srv_lookup(query, answer);
  return answer;
}

#if 0
dns_naptr_record_list dns_lookup_naptr(const std::string& query)
{
  dns_naptr_record_list answer;

  return answer;
}

dns_mx_record_list dns_lookup_mx(const std::string& query)
{
  dns_mx_record_list answer;

  return answer;
}


#endif





////////////////////////////////////////////////////////////////////////////////








} //OSS

