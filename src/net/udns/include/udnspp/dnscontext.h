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

#ifndef UDNSPP_DNSCONTEXT_INCLUDED
#define UDNSPP_DNSCONTEXT_INCLUDED

#include <udns.h> 
#include <string>

namespace udnspp {

class DNSContext
{
public:
  DNSContext();

  DNSContext(const DNSContext& context);

  ~DNSContext();

  int open();

  bool isOpen() const;

  int getSocketFd() const;

  bool setOption(dns_opt option, int value);

  bool setFlags(int flags);

  dns_ctx* context();

  dns_ctx* context(bool autoOpen);

  static DNSContext* defaultContext();
protected:
  dns_ctx* _pCtx;
  bool _canFreeCtx;
  int _socketFd;
};

//
// Inlines
//

inline int DNSContext::open()
{
  return (_socketFd = dns_open(_pCtx));
}

inline bool DNSContext::isOpen() const
{
  return _socketFd >= 0;
}

inline int DNSContext::getSocketFd() const
{
  return _socketFd;
}

inline bool DNSContext::setOption(dns_opt option, int value)
{
  return dns_set_opt(_pCtx, option, value) == 0;
}

inline bool DNSContext::setFlags(int flags)
{
  return setOption(DNS_OPT_FLAGS, flags);
}

inline dns_ctx* DNSContext::context()
{
  return context(true);
}


} // namespace udnspp

#endif // UDNSPP_DNSCONTEXT_INCLUDED