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



#include "OSS/SIP/EP/SIPEndpoint.h"
#include "OSS/SIP/EP/SIPEndpointRetransmitter.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {
namespace EP {
  

SIPEndpointRetransmitter::SIPEndpointRetransmitter(
  SIPEndpoint* pEndpoint,
  const SIPMessage::Ptr& pRequest,
  SIPTransportSession::Ptr& pTransport,
  const Net::IPAddress& target
) : _timer(pEndpoint->stack().transport().ioService(), boost::posix_time::milliseconds(0)),
    _pRequest(pRequest),
    _target(target),
    _pTransport(pTransport),
    _pEndpoint(pEndpoint),
    _currentExpire(0),
    _t1(500),
    _t2(_t1 * 8),
    _tb(_t1 * 64)
{
  _logId = pRequest->createContextId(true);
  OSS_LOG_DEBUG(_logId << "SIPEndpointRetransmitter CREATED");
}

SIPEndpointRetransmitter::~SIPEndpointRetransmitter()
{
  stop();
  
  OSS_LOG_DEBUG(_logId << "SIPEndpointRetransmitter DESTROYED");
}

void SIPEndpointRetransmitter::start(unsigned t1)
{
  _t1 = t1,
  _t2 = _t1 * 8;
  _tb = _t1 * 64;
  _currentExpire = _t1;
  startTimer(_currentExpire);
}

void SIPEndpointRetransmitter::stop()
{
  cancelTimer();
}

void SIPEndpointRetransmitter::startTimer(unsigned long expire)
{
  cancelTimer();
  _timer.expires_from_now(boost::posix_time::milliseconds(expire));
  _timer.async_wait(boost::bind(&SIPEndpointRetransmitter::handleTimer, shared_from_this(), boost::asio::placeholders::error));
}

void SIPEndpointRetransmitter::cancelTimer()
{
  _timer.cancel();
}

void SIPEndpointRetransmitter::handleTimer(const boost::system::error_code& e)
{
  if (e)
  {
    return;
  }
  
  try
  {
    std::string target = _target.toString();
    std::string port = OSS::string_from_number<unsigned short>(_target.getPort());
    if (_pTransport)
    {
      _pTransport->writeMessage(_pRequest, target, port);
    }
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR(_logId << "SIPEndpointRetransmitter::handleTimer - Exception: " << e.what());
  }
  
  //
  // Recompute next expire
  //
  _currentExpire = (_currentExpire * 2 > _t2) ? _t2 : _currentExpire * 2;
  _tb -=  _currentExpire;
  if (_tb > 0)
  {
    startTimer(_currentExpire);
  }
  else
  {
    //
    // We are done.  Report and ACK timeout
    //
    if (_pEndpoint)
    {
      _pEndpoint->onHandleAckTimeout(_pRequest);
    }
  }
}
  

} } } // OSS::SIP::EP



