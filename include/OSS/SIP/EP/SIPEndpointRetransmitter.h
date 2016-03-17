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


#ifndef SIPENDPOINTRETRANSMITTER_H_INCLUDED
#define SIPENDPOINTRETRANSMITTER_H_INCLUDED


#include "OSS/SIP/SIPFsm.h"


namespace OSS {
namespace SIP {
namespace EP {
  

class SIPEndpoint;  
  
class SIPEndpointRetransmitter : public boost::enable_shared_from_this<SIPEndpointRetransmitter>, private boost::noncopyable
{
public:
  
  typedef boost::shared_ptr<SIPEndpointRetransmitter> Ptr;
  
  SIPEndpointRetransmitter(
    SIPEndpoint* pEndpoint,
    const SIPMessage::Ptr& pRequest,
    SIPTransportSession::Ptr& pTransport,
    const Net::IPAddress& target
  );
  
  ~SIPEndpointRetransmitter();
  
  void start(unsigned t1);
  
  void stop();
  
  const SIPMessage::Ptr& getRequest() const;
  
  const Net::IPAddress& getTargetAddress() const;
  
  const SIPTransportSession::Ptr& getTransport() const;
  
  SIPEndpoint& endpoint();
  
protected:
  
  void startTimer(unsigned long expire);
  
  void cancelTimer();
  
  void handleTimer(const boost::system::error_code& e);
  
  boost::asio::deadline_timer _timer;
  SIPMessage::Ptr _pRequest;
  Net::IPAddress _target;
  SIPTransportSession::Ptr _pTransport;
  SIPEndpoint* _pEndpoint;
  unsigned _currentExpire;
  unsigned _t1;
  unsigned _t2;
  int _tb;
  std::string _logId;
};


//
// Inlines
//

inline const SIPMessage::Ptr& SIPEndpointRetransmitter::getRequest() const
{
  return _pRequest;
}

inline SIPEndpoint& SIPEndpointRetransmitter::endpoint()
{
  return *_pEndpoint;
}

inline const Net::IPAddress& SIPEndpointRetransmitter::getTargetAddress() const
{
  return _target;
}

inline const SIPTransportSession::Ptr& SIPEndpointRetransmitter::getTransport() const
{
  return _pTransport;
}


} } } // OSS::SIP::EP

#endif // SIPENDPOINTRETRANSMITTER_H_INCLUDED

