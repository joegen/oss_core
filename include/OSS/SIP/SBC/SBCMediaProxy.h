// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef SBCMEDIAPROXY_H_INCLUDED
#define	SBCMEDIAPROXY_H_INCLUDED


#include "OSS/SIP/SBC/SBCMediaProxyClient.h"
#include "OSS/RTP/RTPProxyManager.h"


namespace OSS {
namespace SIP {
namespace SBC {

class SBCManager;

class SBCMediaProxy
{
public:
  SBCMediaProxy(SBCManager* pManager);
  
  ~SBCMediaProxy();
   
  bool handleSDP(
    const std::string& logId,
    const std::string& sessionId,
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    OSS::RTP::RTPProxySession::RequestType requestType,
    std::string& sdp,
    OSS::RTP::RTPProxy::Attributes& rtpAttribute);
  
  bool getSDP(const std::string& sessionId, std::string& lastOffer, std::string& lastAnswer);
  
  bool initialize(bool remoteRtpEnabled = false);
  
  bool removeSession(const std::string& sessionId);
  
  bool setMaxSession(unsigned int maxSession);
  
  unsigned int getMaxSession() const;
  
  unsigned int getSessionCount() const;
  
protected:
  SBCMediaProxyClient* getNode(const std::string& sessionId, bool& spillOver);
  SBCMediaProxyClient _node0;
  SBCMediaProxyClient _node1;
  SBCMediaProxyClient _node2;
  SBCMediaProxyClient _node3;
  SBCMediaProxyClient _node4;
  SBCManager* _pManager;
  OSS::RTP::RTPProxyManager _rtp;
  bool _remoteRtpEnabled;
};

//
// Inlines
//


} } } // OSS::SIP::SBC



#endif	// SBCMEDIAPROXY_H_INCLUDED
