

// OSS Software Solutions Application Programmer Interface
// Package: SBC
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


#include "OSS/SIP/SBC/SBCSDPBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/UTL/Logger.h"
#include "OSS/SDP/SDPMedia.h"
#include "OSS/SDP/SDPSession.h"

namespace OSS {
namespace SIP {
namespace SBC {

using OSS::SIP::SIPMessage;
using namespace OSS::RTP;
using namespace OSS::SDP;

static bool patch_sdp_media_address(SDPSession& sdpSession, SDPMedia::Type type, const std::string& address, unsigned short port = 0, std::size_t index = -1)
{
  size_t count = sdpSession.getMediaCount(type);
  for (size_t i = 0; i < count; i++)
  {
    if (i < index)
    {
      continue;
    }
    SDPMedia::Ptr media = sdpSession.getMedia(type, i);
    if (media)
    {
      std::string addr = media->getAddress();
      if (OSS::Net::IPAddress::isV4Address(address))
      {
        media->setAddressV4(address);
      }
      else
      {
        media->setAddressV6(address);
      }

      if (port != 0)
      {
        media->setDataPort(port);
      }
    }
  }
  return false;
}

static void finalize_session_description(SIPB2BTransaction::Ptr pTransaction, std::string& sdp)
{
  SDPSession sdpSession(sdp.c_str());
  std::string force_sdp_global_ip;
  std::string force_sdp_audio_ip;
  std::string force_sdp_audio_port;
  bool patched = false;
  
  if (pTransaction->getProperty("force-sdp-global-ip", force_sdp_global_ip) && !force_sdp_global_ip.empty())
  {
    if (OSS::Net::IPAddress::isV4Address(force_sdp_global_ip))
    {
      sdpSession.setAddressV4(force_sdp_global_ip);
    }
    else
    {
      sdpSession.setAddressV6(force_sdp_global_ip);
    }
    patched = true;
  }
  
  if (pTransaction->getProperty("force-sdp-audio-ip", force_sdp_audio_ip) && !force_sdp_audio_ip.empty())
  {
    int port = 0;
    if (pTransaction->getProperty("force-sdp-audio-port", force_sdp_audio_port) && !force_sdp_audio_port.empty())
    {
      port = OSS::string_to_number<int>(force_sdp_audio_port);
    }
    patch_sdp_media_address(sdpSession, SDPMedia::TYPE_AUDIO, force_sdp_audio_ip, port, 0);
    patched = true;
  }
  std::string force_sdp_video_ip;
  std::string force_sdp_video_port;
  if (pTransaction->getProperty("force-sdp-video-ip", force_sdp_video_ip) && !force_sdp_video_ip.empty())
  {
    int port = 0;
    if (pTransaction->getProperty("force-sdp-video-port", force_sdp_video_port) && !force_sdp_video_port.empty())
    {
      port = OSS::string_to_number<int>(force_sdp_video_port);
    }
    patch_sdp_media_address(sdpSession, SDPMedia::TYPE_VIDEO, force_sdp_video_ip, port, 0);
    patched = true;
  }
  
  std::string sbc_sdp_audio_attributes;
  if (pTransaction->getProperty("sdp-audio-attributes", sbc_sdp_audio_attributes) && !sbc_sdp_audio_attributes.empty())
  {
    std::vector<std::string> tokens = OSS::string_tokenize(sbc_sdp_audio_attributes, "~");
    SDPMedia::Ptr media = sdpSession.getMedia(SDPMedia::TYPE_AUDIO, 0);
    if (media)
    {
      for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
      {
        media->setFlagAttribute(iter->c_str());
        patched = true;
      }
    }
  }
  
  std::string sbc_sdp_video_attributes;
  if (pTransaction->getProperty("sdp-video-attributes", sbc_sdp_video_attributes) && !sbc_sdp_video_attributes.empty())
  {
    std::vector<std::string> tokens = OSS::string_tokenize(sbc_sdp_video_attributes, "~");
    SDPMedia::Ptr media = sdpSession.getMedia(SDPMedia::TYPE_VIDEO, 0);
    if (media)
    {
      for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
      {
        media->setFlagAttribute(iter->c_str());
        patched = true;
      }
    }
  }
  
  std::string sbc_sdp_remove_audio_attributes;
  if (pTransaction->getProperty("sdp-remove-audio-attributes", sbc_sdp_remove_audio_attributes) && !sbc_sdp_remove_audio_attributes.empty())
  {
    std::vector<std::string> tokens = OSS::string_tokenize(sbc_sdp_remove_audio_attributes, "~");
    SDPMedia::Ptr media = sdpSession.getMedia(SDPMedia::TYPE_AUDIO, 0);
    if (media)
    {
      for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
      {
        media->removeCommonAttribute(iter->c_str());
        patched = true;
      }
    }
  }
  
  std::string sbc_sdp_remove_video_attributes;
  if (pTransaction->getProperty("sdp-remove-video-attributes", sbc_sdp_remove_video_attributes) && !sbc_sdp_remove_video_attributes.empty())
  {
    std::vector<std::string> tokens = OSS::string_tokenize(sbc_sdp_remove_video_attributes, "~");
    SDPMedia::Ptr media = sdpSession.getMedia(SDPMedia::TYPE_VIDEO, 0);
    if (media)
    {
      for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
      {
        media->removeCommonAttribute(iter->c_str());
        patched = true;
      }
    }
  }
  
  if (patched)
  {
    sdp = sdpSession.toString();
  }
}


static void patch_ipv6_as_rfc1918_address(SIPB2BTransaction::Ptr pTransaction, std::string& sdp)
{
  const char* replacementAddress = "192.168.250.250";
  //
  // This function froces non-ipv4 address to be treated as private IP address.
  // It's a hack for T-mobile which sends IPV6 addresses in SDP
  //
  std::string doWePatch;
  pTransaction->getProperty("patch-ipv6_as-rfc1918-address", doWePatch);
  if (doWePatch != "1")
  {
    return;
  }
  bool patched = false;
  SDPSession sdpSession(sdp.c_str());
  
  std::string globalAddress = sdpSession.getAddress();
  if (!OSS::Net::IPAddress::isV4Address(globalAddress))
  {
    OSS_LOG_DEBUG(pTransaction->getLogId() << "Forced global media address to masquerade as from " << globalAddress << " to " << replacementAddress);
    sdpSession.setAddressV4(replacementAddress);
    patched = true;
  }
  
  size_t audioCount = sdpSession.getMediaCount(SDPMedia::TYPE_AUDIO);
  for (size_t i = 0; i < audioCount; i++)
  {
    SDPMedia::Ptr media = sdpSession.getMedia(SDPMedia::TYPE_AUDIO, i);
    if (media)
    {
      std::string addr = media->getAddress();
      if (!OSS::Net::IPAddress::isV4Address(addr))
      {
        OSS_LOG_DEBUG(pTransaction->getLogId() << "Forced audio media address to masquerade as from " << addr << " to " << replacementAddress);
        media->setAddressV4(replacementAddress);
        patched = true;
      }
    }
  }
  
  size_t videoCount = sdpSession.getMediaCount(SDPMedia::TYPE_VIDEO);
  for (size_t i = 0; i < videoCount; i++)
  {
    SDPMedia::Ptr media = sdpSession.getMedia(SDPMedia::TYPE_VIDEO, i);
    if (media)
    {
      std::string addr = media->getAddress();
      if (!OSS::Net::IPAddress::isV4Address(addr))
      {
        OSS_LOG_DEBUG(pTransaction->getLogId() << "Forced video media address to masquerade as from " << addr << " to " << replacementAddress);
        media->setAddressV4(replacementAddress);
        patched = true;
      }
    }
  }
  
  size_t faxCount = sdpSession.getMediaCount(SDPMedia::TYPE_FAX);
  for (size_t i = 0; i < faxCount; i++)
  {
    SDPMedia::Ptr media = sdpSession.getMedia(SDPMedia::TYPE_FAX, i);
    if (media)
    {
      std::string addr = media->getAddress();
      if (!OSS::Net::IPAddress::isV4Address(addr))
      {
        OSS_LOG_DEBUG(pTransaction->getLogId() << "Forced fax media address to masquerade as from " << addr << " to " << replacementAddress);
        media->setAddressV4(replacementAddress);
        patched = true;
      }
    }
  }
  
  if (patched)
  {
    sdp = sdpSession.toString();
  }
  
}

SBCSDPBehavior::SBCSDPBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_SDP, "SBC SDP Request Handler")
{
  setName("SBC SDP Request Handler");
}

SBCSDPBehavior::~SBCSDPBehavior()
{
}

SIPMessage::Ptr SBCSDPBehavior::onProcessRequestBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
  /// This method allows the application to
  /// process the body of the request
  /// before it gets sent out.
  ///
  /// This method allows the upper layer to modify the body
  /// based on specific application requirements.  For
  /// example, a media proxy may modify SDP address and port
  /// so that RTP passes through the application.
  ///
  /// If the return value is an error response, the transaction
  /// will send it automatically to the sender.
  ///
  /// If the body is supported, the return value must be a null-Ptr.
{

  std::string noRTPProxy;
  if (pTransaction->getProperty("no-rtp-proxy", noRTPProxy) && noRTPProxy == "1")
    return OSS::SIP::SIPMessage::Ptr();

  std::string targetTransport;
  pRequest->getProperty(OSS::PropertyMap::PROP_TargetTransport, targetTransport);
  bool isWebRtcTarget = OSS::string_caseless_starts_with(targetTransport, "ws"); 
  
  std::string sdp = pRequest->getBody();
  patch_ipv6_as_rfc1918_address(pTransaction, sdp);
  

  bool isWebRtcSender = sdp.find("RTP/SAVPF") != std::string::npos;
  //
  // Do not handle SAVPF format that are targeted to a webrtc destination.
  // WebRTC will handle its own media using ICE
  //
  if (isWebRtcSender && isWebRtcTarget)
    return OSS::SIP::SIPMessage::Ptr();
  
  std::string sessionId;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));

  SIPMessage* pServerRequest = pTransaction->serverRequest().get();
  std::string bottomVia;
  if (!SIPVia::msgGetBottomVia(pServerRequest, bottomVia))
  {
    SIPMessage::Ptr serverError = pServerRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Bad bottom via header");
    return serverError;
  }

 std::string sentBy;
 if (!SIPVia::getSentBy(bottomVia, sentBy))
  {
    SIPMessage::Ptr serverError = pServerRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Bad bottom via header");
    return serverError;
  }

  pTransaction->setProperty("sdp-answer-route", sentBy);

  OSS::Net::IPAddress addrSentBy = OSS::Net::IPAddress::fromV4IPPort(sentBy.c_str());
  OSS::Net::IPAddress addrPacketSource = pTransaction->serverTransport()->getRemoteAddress();

  //
  // Set the local interface to be used to send to A Leg.
  // Also set the external address if one is configured.
  //
  OSS::Net::IPAddress addrLocalInterface = pTransaction->serverTransport()->getLocalAddress();
  addrLocalInterface.externalAddress() = pTransaction->serverTransport()->getExternalAddress();

  std::string targetAddress;
  std::string localAddress;
  OSS_VERIFY(pRequest->getProperty("target-address", targetAddress) &&
    pRequest->getProperty("local-address", localAddress));

  OSS::Net::IPAddress addrRoute = OSS::Net::IPAddress::fromV4IPPort(targetAddress.c_str());
  OSS::Net::IPAddress addrRouteLocalInterface = OSS::Net::IPAddress::fromV4IPPort(localAddress.c_str());

  //
  // Assign the exteral address for B-Leg if one is configured
  //
  std::string routeExternalAddress;
  if (_pManager->getExternalAddress(addrRouteLocalInterface, routeExternalAddress))
  {
    addrRouteLocalInterface.externalAddress() = routeExternalAddress;
  }

  std::string rtpProxyProp;
  bool requireRTPProxy = pTransaction->getProperty("require-rtp-proxy", rtpProxyProp) && rtpProxyProp == "1";
  OSS::string_replace(sdp, "FreeSWITCH", "KarooBridge");
  
  std::string legIndex_;
  int legIndex = 0;
  if(pTransaction->getProperty("leg-index", legIndex_))
  {
    legIndex = OSS::string_to_number<int>(legIndex_.c_str());
  }
  
  try
  {
    RTPProxy::Attributes rtpAttributes;

    rtpAttributes.verbose = false;
    std::string propXOR = "0";
    rtpAttributes.forcePEAEncryption = pRequest->getProperty("peer-xor", propXOR) && propXOR == "1";
    rtpAttributes.forceCreate = requireRTPProxy;
    rtpAttributes.legIndex = legIndex;
    rtpAttributes.callId = pRequest->hdrGet("call-id");
    rtpAttributes.from = pRequest->hdrGet("from");
    rtpAttributes.to = pRequest->hdrGet("to");
       
    std::string allowHairpin = "0";
    rtpAttributes.allowHairPin = (pTransaction->getProperty("allow-hairpin", allowHairpin) && allowHairpin == "1");

    std::string sResizerSamples;
    if (!pServerRequest->getProperty("rtp-resizer-samples", sResizerSamples))
      pRequest->getProperty("rtp-resizer-samples", sResizerSamples);

    if (!sResizerSamples.empty())
    {
      try
      {
        std::vector<std::string> tokens = OSS::string_tokenize(sResizerSamples, "/");
        if (tokens.size() == 2)
        {
          rtpAttributes.resizerSamplesLeg1 = boost::lexical_cast<int>(tokens[0]);
          rtpAttributes.resizerSamplesLeg2 = boost::lexical_cast<int>(tokens[1]);
          OSS_LOG_INFO(pTransaction->getLogId() << "RTP: Activating RTP repacketization to " << rtpAttributes.resizerSamplesLeg1 << "/" << rtpAttributes.resizerSamplesLeg2 << " resolution.")
        }
        else
        {
          rtpAttributes.resizerSamplesLeg1 = boost::lexical_cast<int>(sResizerSamples);
          rtpAttributes.resizerSamplesLeg2 = boost::lexical_cast<int>(sResizerSamples);
          OSS_LOG_INFO(pTransaction->getLogId() <<"RTP: Activating RTP repacketization to " << rtpAttributes.resizerSamplesLeg1 << "/" << rtpAttributes.resizerSamplesLeg2 << " resolution.")
        }
      }
      catch(...)
      {
      }
    }

    std::string maximumChannel;
    if (pRequest->getProperty("max-channel", maximumChannel) && !maximumChannel.empty())
      rtpAttributes.countSessions = true;


    OSS_LOG_INFO(pTransaction->getLogId() << "SBCSDPBehavior::onProcessRequestBody - Processing SDP for sessionId " << sessionId);
    
    _pManager->rtpProxy().handleSDP(pTransaction->getLogId(), sessionId, addrSentBy, addrPacketSource, addrLocalInterface,
    addrRoute, addrRouteLocalInterface, RTPProxySession::INVITE, sdp, rtpAttributes);

    if (!sdp.empty())
    {
      OSS_LOG_INFO(pTransaction->getLogId() << "SBCSDPBehavior::onProcessRequestBody - Processing SDP for sessionId " << sessionId << " HANDLED");
    }
    else
    {
      OSS_LOG_INFO(pTransaction->getLogId() << "SBCSDPBehavior::onProcessRequestBody - Processing SDP for sessionId " << sessionId << " returned empty response");
    }

    if (!pRequest->isMidDialog())
    {
      if (_pManager->rtpProxy().getSessionCount() >= _pManager->rtpProxy().getMaxSession())
      {
        OSS_LOG_WARNING(pTransaction->getLogId() << "Channels Ran Out! Used up " << _pManager->rtpProxy().getSessionCount() << " of " << _pManager->rtpProxy().getMaxSession());
        _pManager->rtpProxy().removeSession(sessionId);
        SIPMessage::Ptr serverError = pServerRequest->createResponse(SIPMessage::CODE_500_InternalServerError, "No More Available Channels");
        return serverError;
      }
    }

  }
  catch(RTPProxyTooManySession e)
  {
    SIPMessage::Ptr serverError = pServerRequest->createResponse(SIPMessage::CODE_503_ServiceUnavailable, e.message().c_str());
    return serverError;
  }
  catch(OSS::Exception e)
  {
    SIPMessage::Ptr serverError = pServerRequest->createResponse(SIPMessage::CODE_400_BadRequest, e.message().c_str());
    return serverError;
  }


  if (!sdp.empty())
  {
    finalize_session_description(pTransaction, sdp);
    pRequest->setBody(sdp);
    std::string clen = OSS::string_from_number<size_t>(sdp.size());
    pRequest->hdrSet("Content-Length", clen.c_str());
  }
  
  return OSS::SIP::SIPMessage::Ptr();
}

void SBCSDPBehavior::onProcessResponseBody(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
  /// This method allows the application to
  /// process the body of the response
  /// before it gets sent out.
  ///
  /// This method allows the upper layer to modify the body
  /// based on specific application requirements.  For
  /// example, a media proxy may modify SDP address and port
  /// so that RTP passes through the application.
  ///
{
  std::string noRTPProxy;
  if (pTransaction->getProperty("no-rtp-proxy", noRTPProxy) && noRTPProxy == "1")
    return;

  std::string responseSDP;
  if (pTransaction->getProperty("response-sdp", responseSDP))
  {
    pResponse->setBody(responseSDP);
    std::string clen = OSS::string_from_number<size_t>(responseSDP.size());
    pResponse->hdrSet("Content-Length", clen.c_str());
    return;
  }
  
  std::string sdp = pResponse->getBody();
  patch_ipv6_as_rfc1918_address(pTransaction, sdp);

  std::string hContact = pResponse->hdrGet("contact");
  std::string sentBy;

  if (!hContact.empty())
  {
    ContactURI contactURI;
    if (!SIPContact::getAt(hContact, contactURI, 0))
      return;
    sentBy = contactURI.getHostPort();
  }

  std::string sessionId;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
  
  std::string sdpAnswerRoute;
  if (!pTransaction->getProperty("sdp-answer-route", sdpAnswerRoute))
  {
    SIPMessage* pServerRequest = pTransaction->serverRequest().get();
    std::string bottomVia;
    if (!SIPVia::msgGetBottomVia(pServerRequest, bottomVia))
    {
      return;
    }
    if (!SIPVia::getSentBy(bottomVia, sdpAnswerRoute))
    {
      return;
    }
    pTransaction->setProperty("sdp-answer-route", sdpAnswerRoute);
  }

  OSS::Net::IPAddress addrSentBy;
  if (!sentBy.empty())
    addrSentBy = OSS::Net::IPAddress::fromV4IPPort(sentBy.c_str());
  else
    addrSentBy = pTransaction->clientTransport()->getRemoteAddress();

  OSS::Net::IPAddress addrPacketSource = pTransaction->clientTransport()->getRemoteAddress();
  OSS::Net::IPAddress addrLocalInterface = pTransaction->clientTransport()->getLocalAddress();
  addrLocalInterface.externalAddress() = pTransaction->clientTransport()->getExternalAddress();

  OSS::Net::IPAddress addrRoute = OSS::Net::IPAddress::fromV4IPPort(sdpAnswerRoute.c_str());
  OSS::Net::IPAddress addrRouteLocalInterface = pTransaction->serverTransport()->getLocalAddress();
  addrRouteLocalInterface.externalAddress() = pTransaction->serverTransport()->getExternalAddress();

  OSS::string_replace(sdp, "FreeSWITCH", "KarooBridge");
  std::string rtpProxyProp;
  bool requireRTPProxy = pTransaction->getProperty("require-rtp-proxy", rtpProxyProp) && rtpProxyProp == "1";

  std::string enableVerboseRTP;
  bool verboseRTP = pTransaction->getProperty("enable-verbose-rtp", enableVerboseRTP) && enableVerboseRTP == "1";
  
  std::string legIndex_;
  int legIndex = 0;
  if(pTransaction->getProperty("leg-index", legIndex_))
  {
    legIndex = OSS::string_to_number<int>(legIndex_.c_str());
  }
  
  try
  {
    RTPProxy::Attributes rtpAttributes;
    rtpAttributes.verbose = verboseRTP;
    std::string propXOR = "0";
    rtpAttributes.forcePEAEncryption = pResponse->getProperty("peer-xor", propXOR) && propXOR == "1";
    rtpAttributes.forceCreate = requireRTPProxy;
    rtpAttributes.legIndex = legIndex;
    rtpAttributes.callId = pResponse->hdrGet("call-id");
    rtpAttributes.from = pResponse->hdrGet("from");
    rtpAttributes.to = pResponse->hdrGet("to");
    
    std::string allowHairpin = "0";
    rtpAttributes.allowHairPin = (pTransaction->getProperty("allow-hairpin", allowHairpin) && allowHairpin == "1");

    OSS_LOG_INFO(pTransaction->getLogId() << "SBCSDPBehavior::onProcessResponseBody - Processing SDP for sessionId " << sessionId);
    
    _pManager->rtpProxy().handleSDP(pTransaction->getLogId(), sessionId, addrSentBy, addrPacketSource, addrLocalInterface,
        addrRoute, addrRouteLocalInterface, RTPProxySession::INVITE_RESPONSE, sdp, rtpAttributes);
    
    if (!sdp.empty())
    {
      OSS_LOG_INFO(pTransaction->getLogId() << "SBCSDPBehavior::onProcessResponseBody - Processing SDP for sessionId " << sessionId << " HANDLED");
    }
    else
    {
      OSS_LOG_INFO(pTransaction->getLogId() << "SBCSDPBehavior::onProcessResponseBody - Processing SDP for sessionId " << sessionId << " returned empty response");
    }
  }
  catch(OSS::Exception e)
  {
    std::ostringstream logMsg;
    logMsg << pTransaction->getLogId() << "Unable to process SDP in response.  Exception: " << e.message();
    OSS::log_warning(logMsg.str());
    return;
  }

  if (!sdp.empty())
  {
    pResponse->setBody(sdp);
    std::string clen = OSS::string_from_number<size_t>(sdp.size());
    pTransaction->setProperty("response-sdp", sdp.c_str());
    pResponse->hdrSet("Content-Length", clen.c_str());
  }
}

} } } // OSS::SIP::SBC



