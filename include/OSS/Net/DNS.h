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


#ifndef OSS_DNS_H_INCLUDED
#define OSS_DNS_H_INCLUDED

#include <string>
#include <list>
#include <set>

#include <boost/tuple/tuple.hpp>
#include "OSS/OSS.h"

#include "OSS/UDNS/udnspp/dnsnaptrrecord.h"
#include "OSS/UDNS/udnspp/dnssrvrecord.h"
#include "OSS/UDNS/udnspp/dnsptrrecord.h"
#include "OSS/UDNS/udnspp/dnsrrcommon.h"
#include "OSS/UDNS/udnspp/dnstxtrecord.h"
#include "OSS/UDNS/udnspp/dnsresolver.h"
#include "OSS/UDNS/udnspp/dnscontext.h"
#include "OSS/UDNS/udnspp/dnsmxrecord.h"
#include "OSS/UDNS/udnspp/dnsarecord.h"
#include "OSS/UDNS/udnspp/dnscache.h"

namespace OSS {

  namespace UDNS = udnspp;
  //
  // A Record Types
  //
  typedef std::string dns_host_record;
  typedef std::list<dns_host_record> dns_host_record_list;
  
  //
  // SRV Record Types
  //
  typedef boost::tuples::tuple<
    std::string, /// Host Name
    std::string, /// IP4 Address
    unsigned short, /// Port
    unsigned short, /// Priority
    unsigned short /// Weight
  > dns_srv_record;

  struct dns_lookup_srv_ltcompare
  {
    bool operator()(const dns_srv_record& d1, const dns_srv_record& d2) const
    {
      if (d1.get<3>() < d2.get<3>())
        return true;
      else if (d1.get<3>() > d2.get<3>())
        return false;
      else
      {
        if (d1.get<4>() < d2.get<4>())
          return true;
        else
          return false;
      }
    }
  };


  typedef std::set<dns_srv_record, dns_lookup_srv_ltcompare> dns_srv_record_list;
  
  
  dns_host_record_list OSS_API dns_lookup_host(const std::string& query);
    /// Lookup A record 
  
  dns_srv_record_list OSS_API dns_lookup_srv(const std::string& query);
    /// Lookup SRV Record

  #if 0
  //
  // NAPTR Record Types
  //
  typedef boost::tuples::tuple<
    unsigned short, /// Priority;
    unsigned short, /// Weight;
    std::string, /// Flags;
    std::string, /// Service;
    std::string, /// Regex;
    std::string /// Replacement;
  > dns_naptr_record;

  typedef std::list<dns_naptr_record> dns_naptr_record_list;

  dns_naptr_record_list OSS_API dns_lookup_naptr(const std::string& query);
    /// Lookup NAPTR Record

  //
  // MX Record Types
  //
  typedef boost::tuples::tuple<
    std::string, /// Host Name;
    std::string, /// IP4 Address
    unsigned short, /// Priority
    bool ///Used Flag
  > dns_mx_record;

  typedef std::list<dns_mx_record> dns_mx_record_list;


  dns_mx_record_list OSS_API dns_lookup_mx(const std::string& query);
    /// Lookup MX Record
  #endif
} //OSS


#endif // OSS_DNS_H_INCLUDED

