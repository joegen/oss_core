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


#include "OSS/SIP/SIPListener.h"
#include "OSS/SIP/SIPTransportService.h"


namespace OSS {
namespace SIP {

  
SIPListener::SIPListener(
  SIPTransportService* pTransportService,
  const std::string& address,
  const std::string& port) :
    _pIoService(0),
    _pTransportService(pTransportService),
    _address(address),
    _port(port),
    _isVirtual(false),
    _hasStarted(false),
    _isEndpoint(false)
{
  if (_pTransportService)
    _pIoService = (&(_pTransportService->ioService()));
}

SIPListener::SIPListener(
  SIPTransportService* pTransportService,
  const std::string& address,
  const std::string& port,
  const std::string& alias) :
    _pIoService(0),
    _pTransportService(pTransportService),
    _address(address),
    _port(port),
    _isVirtual(false),
    _hasStarted(false),
    _isEndpoint(false),
    _alias(alias)
{
  if (_pTransportService)
    _pIoService = (&(_pTransportService->ioService()));
}


SIPListener::~SIPListener()
{
}

SIPListener::SIPListener(const SIPListener& copy) :
  _pIoService(copy._pIoService),
  _pTransportService(0),
  _address(),
  _port(0)
{
}

SIPListener& SIPListener::operator = (const SIPListener&)
{
  return *this;
}

bool SIPListener::isAcceptableDestination(const std::string& address) const
{
  for (SubNets::const_iterator iter = _subNets.begin(); iter != _subNets.end(); iter++)
  {
    if (OSS::socket_address_cidr_verify(address, *iter))
    {
      return true;
    }
  }
  return false;
}

void SIPListener::restart(boost::system::error_code& e)
{
  //
  // Unimplemented
  //
  assert(true);
}
  
void SIPListener::closeTemporarily(boost::system::error_code& e)
{
  //
  // Unimplemented
  //
  assert(true);
}

bool SIPListener::canBeRestarted() const
{
  return false;
}


} } // OSS::SIP

