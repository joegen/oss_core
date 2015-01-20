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
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USvoidE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef PROPERTYMAP_H_INCLUDED
#define	PROPERTYMAP_H_INCLUDED

namespace OSS {

struct PropertyMap
{
  #define PROP_MAP_STRINGS \
  { \
    "target-address", \
    "target-port", \
    "target-transport", \
    "target-host", \
    "local-address", \
    "transport-id", \
    "session-id", \
    "xor", \
    "peer-xor", \
    "auth-method", \
    "auth-action", \
    "auth-response", \
    "route-action", \
    "reject-code", \
    "reject-reason", \
    "interface-address", \
    "interface-port", \
    "transaction-timeout", \
    "rtp-resizer-samples", \
    "max-channel", \
    "response-target", \
    "response-interface", \
    "response-sdp", \
    "no-rtp-proxy", \
    "require-rtp-proxy", \
    "enable-verbose-rtp", \
    "leg1-contact", \
    "leg2-contact", \
    "leg1-rr", \
    "leg2-rr", \
    "leg-identifier", \
    "leg-index", \
    "invoke-local-handler", \
    "disable-nat-compensation", \
    "respond-to-packet-source", \
    "sdp-answer-route", \
    "reinvite", \
    "generate-local-response", \
    "inbound-contact," \
    "is-early-dialog-persisted", \
    "is-out-of-dialog-refer", \
    "subnets", \
    "property-undefined" \
  };

  enum Enum
  {
    PROP_TargetAddress,
    PROP_TargetPort,
    PROP_TargetTransport,
    PROP_TargetHost,
    PROP_LocalAddress,
    PROP_TransportId,
    PROP_SessionId,
    PROP_XOR,
    PROP_PeerXOR,
    PROP_AuthMethod,
    PROP_AuthAction,
    PROP_AuthResponse,
    PROP_RouteAction,
    PROP_RejectCode,
    PROP_RejectReason,
    PROP_InterfaceAddress,
    PROP_InterfacePort,
    PROP_TransactionTimeout,
    PROP_RTPResizerSamples,
    PROP_MaxChannel,
    PROP_ResponseTarget,
    PROP_ResponseInterface,
    PROP_ResponseSDP,
    PROP_NoRTPProxy,
    PROP_RequireRTPProxy,
    PROP_EnableVerboseRTP,
    PROP_Leg1Contact,
    PROP_Leg2Contact,
    PROP_Leg1RR,
    PROP_Leg2RR,
    PROP_LegIdentifier,
    PROP_LegIndex,
    PROP_InvokeLocalHandler,
    PROP_DisableNATCompensation,
    PROP_RespondToPacketSource,
    PROP_SDPAnswerRoute,
    PROP_IsReinvite,
    PROP_GenerateLocalResponse,
    PROP_InboundContact,
    PROP_IsEarlyDialogPersisted,
    PROP_IsOutOfDialogRefer,
    PROP_Subnets,
    PROP_Max
  };
  
  static const char* propertyString(PropertyMap::Enum prop)
    /// returns the string representation of a custom property
  {
    char* ret = 0;
    
    if (prop < PROP_Max)
    {
      static const char* prop_map[] = PROP_MAP_STRINGS;

      
      ret = (char*)prop_map[prop];
    }
    
    return ret;
  }
};

} // namespace OSS


#endif	// PROPERTYMAP_H_INCLUDED

