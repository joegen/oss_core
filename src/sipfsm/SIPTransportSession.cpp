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


#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPFSMDispatch.h"


namespace OSS {
namespace SIP {



SIPTransportRateLimitStrategy SIPTransportSession::_rateLimit;
SIPTransportRateLimitStrategy& SIPTransportSession::rateLimit()
{
  return SIPTransportSession::_rateLimit;
}

SIPTransportSession::SIPTransportSession() :
  _isReliableTransport(true),
  _pDispatch(0),
  _bytesTransferred(0),
  _bytesRead(0),
  _identifier(0)
{
}

SIPTransportSession::~SIPTransportSession()
{
}

SIPTransportSession::SIPTransportSession(const SIPTransportSession&)
{
}

SIPTransportSession& SIPTransportSession::operator = (const SIPTransportSession&)
{
  return *this;
}

} } // OSS::SIP
