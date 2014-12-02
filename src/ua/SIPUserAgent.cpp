
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

#include "OSS/SIP/UA/SIPUserAgent.h"
#include "OSS/UTL/Logger.h"
#include "re.h"


namespace OSS {
namespace SIP {
namespace UA {


/* called when all sip transactions are completed */
void ua_exit_handler(void *arg)
{
  SIPUserAgent* ua = (SIPUserAgent*)arg;
  if (ua && ua->_exitHandler)
  {
    ua->_exitHandler();
  }
}

SIPUserAgent::SIPUserAgent( 
  const std::string& softwareId, //Software identifier
  OSS::UInt32 ctsz, // Size of client transactions hashtable (power of 2)
  OSS::UInt32 stsz,     // Size of server transactions hashtable (power of 2)
  OSS::UInt32 tcsz     // Size of SIP transport hashtable (power of 2)
    
) :
  _isRunning(false),
  _isTransportReady(false),
  _sip_stack_handle(0),
  _dns_client_handle(0)
{
  int err = 0;
  //
  // Initialize DNS
  //
  struct dnsc* pDNSClient = 0;
  uint32_t nsc = 0;
  static struct sa nameServers[16];

  nsc = ARRAY_SIZE(nameServers);
  // fetch list of DNS server IP addresses
  err = dns_srv_get(0, 0, nameServers, &nsc);
  if (err)
  {
    OSS_LOG_ERROR("SIPUserAgent::SIPUserAgent - Unable to retrieve DNS server list.");
    return;
  }

  // Create the DNS client
  err = dnsc_alloc(&pDNSClient, NULL, nameServers, nsc);
  if (err) 
  {
    OSS_LOG_ERROR("SIPUserAgent::SIPUserAgent - Unable to create DNS client.");
    return;
  }

  _dns_client_handle = (OSS_HANDLE)pDNSClient;
  
  //
  // Create the SIP Stack instance
  //
  struct sip* pSipStack = 0;
  
  err = sip_alloc(&pSipStack, pDNSClient, ctsz, stsz, tcsz,
			softwareId.c_str(),
			ua_exit_handler, (OSS_HANDLE)this);
  
  if (err) 
  {
    OSS_LOG_ERROR("SIPUserAgent::SIPUserAgent - Unable to create SIP Stack.");
    return;
  }
  
  _sip_stack_handle = pSipStack;
}

SIPUserAgent::~SIPUserAgent()
{
  struct sip* pSipStack  = (struct sip*)_sip_stack_handle;
  if (pSipStack)
  {
    mem_deref(pSipStack);
    _sip_stack_handle = 0;
  }
  
  struct dnsc* pDNSClient = (struct dnsc*)_dns_client_handle;
  if (pDNSClient)
  {
    mem_deref(pDNSClient);
    _dns_client_handle = 0;
  }
}

bool SIPUserAgent::transportInit()
{
  int err = 0;
  struct sip* pSipStack  = (struct sip*)_sip_stack_handle;
  
  if(_isTransportReady || !pSipStack)
    return false;
  
  //
  // If transport is not explicitly specified, open UDP/TCP using a random port
  //
  if (_udpListeners.empty() && _tcpListeners.empty() && _tlsListeners.empty())
  {
    struct sa laddr;
    /* fetch local IP address */
    err = net_default_source_addr_get(AF_INET, &laddr);
    if (err) 
    {
      OSS_LOG_ERROR("SIPUserAgent::transportInit - Unable to determine local IP Address.");
      return false;
    }
    
    /* listen on random port */
    sa_set_port(&laddr, 0);
    
    /* add supported SIP transports */
    err |= sip_transp_add(pSipStack, SIP_TRANSP_UDP, &laddr);
    err |= sip_transp_add(pSipStack, SIP_TRANSP_TCP, &laddr);
    if (err) 
    {
      OSS_LOG_ERROR( "transport error: " << strerror(err));
      return false;
    }
    
    _isTransportReady = true;
  }
  
  return _isTransportReady;
}
  
bool SIPUserAgent::run()
{
  if (_isRunning)
    return false;
  
  if (!transportInit())
    return false;
  //
  // Check if DNS, Transport, and SIP Stack are properly initialized
  //
  if (!_isTransportReady || !_dns_client_handle || !_sip_stack_handle)
    return false;
  
  return true;
}

void SIPUserAgent::stop(bool force)
{
  struct sip* pSipStack  = (struct sip*)_sip_stack_handle;
  if (pSipStack)
  {
    sip_close(pSipStack, force);
  }
}
  

} } } // OSS::SIP::UA
  

