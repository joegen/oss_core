// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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
    _pTransportService(pTransportService),
    _ioService(pTransportService->ioService()),
    _address(address),
    _port(port)
{
}

SIPListener::~SIPListener()
{
}

SIPListener::SIPListener(const SIPListener& copy) :
  _ioService(copy._ioService)
{
}

SIPListener& SIPListener::operator = (const SIPListener&)
{
  return *this;
}


} } // OSS::SIP

