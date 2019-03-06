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

#ifndef UDNSPP_RRCOMMON_INCLUDED
#define UDNSPP_RRCOMMON_INCLUDED


#include <udns.h>
#include <string>
#include <vector>
#include <cassert>


#ifdef WINDOWS
#include <Windows.h>
#endif

namespace udnspp {

template <typename T>
class DNSRRCommon
{
public:

  DNSRRCommon()
  {
  }

  DNSRRCommon(const DNSRRCommon& rr)
  {
    _cname = rr._cname;
    _qname = rr._qname;
    _ttl = rr._ttl;
    _records = rr._records;
  }

  ~DNSRRCommon()
  {
  }

  void swap(DNSRRCommon& rr)
  {
    std::swap(_cname, rr._cname);
    std::swap(_qname, rr._qname);
    std::swap(_ttl, rr._ttl);
    std::swap(_records, rr._records);
  }

  DNSRRCommon& operator=(const DNSRRCommon& rr)
  {
    DNSRRCommon clonable(rr);
    swap(clonable);
    return *this;
  }

  const std::string& getCName() const
  {
    return _cname;
  }

  const std::string& getQName() const
  {
    return _qname;
  }

  unsigned int getTTL() const
  {
    return _ttl;
  }

  const T& getRecords() const
  {
    return _records;
  }

  T& getRecords()
  {
    return _records;
  }

protected:
  std::string _cname;		// canonical name
  std::string _qname;		// original query name
  unsigned int _ttl;	  // TTL value
  T _records;
};

} // namespace udns

#endif // UDNSPP_RRCOMMON_INCLUDED