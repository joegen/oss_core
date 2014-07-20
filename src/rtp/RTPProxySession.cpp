/*
 * Copyright (C) 2012  OSS Software Solutions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */


#include "OSS/RTP/RTPProxySession.h"
#include "OSS/SDP/SDPSession.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/RTP/RTPProxyManager.h"
#include "OSS/RTP/RTPProxyRecord.h"


namespace OSS {
namespace RTP {


using namespace OSS::SDP;
using namespace OSS::Persistent;
using namespace OSS::Net;


RTPProxySession::RTPProxySession(RTPProxyManager* pManager, const std::string& identifier) :
  _state(IDLE),
  _identifier(identifier),
  _verbose(false),
  _pManager(pManager),
  _audio(pManager, this, identifier + "-audio", false),
  _video(pManager, this, identifier + "-video", false),
  _fax(pManager, this, identifier + "-fax", true),
  _isExpectingInitialAnswer(false),
  _hasOfferedAudioProxy(false),
  _hasOfferedVideoProxy(false),
  _hasOfferedFaxProxy(false),
  _isAudioProxyNegotiated(false),
  _isVideoProxyNegotiated(false),
  _isFaxProxyNegotiated(false),
  _lastOfferIndex(0),
  _authStateTimer(_pManager->_ioService, boost::posix_time::milliseconds(_pManager->_readTimeout)),
  _isAuthTimeout(false),
  _resizerSamplesLeg1(0),
  _resizerSamplesLeg2(0)
{
}

RTPProxySession::~RTPProxySession()
{
  stop();
  if (!_pManager->hasRtpDb() && _pManager->persistStateFiles())
    ClassType::remove(_stateFile);
  else if (_pManager->hasRtpDb())
    _pManager->redisClient().del(_identifier);
}

void RTPProxySession::stop()
{
  _audio.stop();
  _video.stop();
  _fax.stop();
}

/*
 * enum RequestType
  {
    INVITE,
    INVITE_RESPONSE,
    INVITE_ACK,
    UPDATE,
    UPDATE_RESPONSE,
    PRACK,
    PRACK_RESPONSE
  };

  enum State
  {
    IDLE,
    INVITE_WAITING_ANSWER_RESPONSE,
    INVITE_WAITING_ANSWER_ACK_OR_PRACK,
    UPDATE_WAITING_ANSWER,
    PRACK_WAITING_ANSWER,
    NEGOTIATED
  };
 */

void RTPProxySession::handleSDP(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  OSS::mutex_critic_sec_lock stateLock(_csStateMutex);

  int rtpCount = _pManager->sessionList().size();
  std::string newState;

  if (!_resizerSamplesLeg1 && rtpAttribute.resizerSamplesLeg1)
    _resizerSamplesLeg1 = rtpAttribute.resizerSamplesLeg1;

  if (!_resizerSamplesLeg2 && rtpAttribute.resizerSamplesLeg2)
    _resizerSamplesLeg2 = rtpAttribute.resizerSamplesLeg2;

  if (_resizerSamplesLeg1 || _resizerSamplesLeg2)
  {

    //
    // Force RTP proxy if this session is configured with resizing
    //
    rtpAttribute.forceCreate = true;
    OSS::SDP::SDPSession sdpSession(sdp.c_str());
    //
    // Remove the ptime attribute for audio
    //
    SDPMedia::Ptr audio = sdpSession.getMedia(SDPMedia::TYPE_AUDIO);
    if (audio)
    {
      audio->removePtime();
      sdp = sdpSession.toString();
    }
  }
  
  switch(_state)
  {
  case IDLE:
    if (requestType != INVITE && requestType != INVITE_RESPONSE)
    {
      OSS_LOG_WARNING(_logId << "RTP: Ignoring request type=" << requestType << " for state " << "IDLE");
      return;
    }
    handleStateIdle(sentBy, packetSourceIP, packetLocalInterface, 
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
    
    if (requestType == INVITE)
    {
      _state = INVITE_WAITING_ANSWER_RESPONSE;
      newState = "INVITE_WAITING_ANSWER_RESPONSE";
    }
    else
    {
      _state = INVITE_WAITING_ANSWER_ACK_OR_PRACK;
      newState = "INVITE_WAITING_ANSWER_ACK_OR_PRACK";
    }
    break;



    OSS_LOG_INFO(_logId << "RTP: Changing state from IDLE to " << newState << " Count: " << rtpCount);

  case INVITE_WAITING_ANSWER_RESPONSE:
    if (requestType != INVITE_RESPONSE)
    {
      OSS_LOG_WARNING(_logId << "RTP: Ignoring request type=" << requestType << " for state " << "INVITE_WAITING_ANSWER_RESPONSE");
      return;
    }
    handleStateInviteWaitingAnswerResponse(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
    _state = NEGOTIATED;

    OSS_LOG_INFO(_logId << "RTP: Changing state from INVITE_WAITING_ANSWER_RESPONSE to " << "NEGOTIATED" << " Count: " << rtpCount);

    break;
  case INVITE_WAITING_ANSWER_ACK_OR_PRACK:
    if (requestType != INVITE_ACK && requestType != PRACK)
    {
      OSS_LOG_WARNING(_logId << "RTP: Ignoring request type=" << requestType << " for state " << "INVITE_WAITING_ANSWER_ACK_OR_PRACK");
      return;
    }

    handleStateInviteWaitingAnswerAckOrPrack(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);

    if (requestType == INVITE_ACK)
      this->_lastSDPInAck = sdp;
   
    _state = NEGOTIATED;

    OSS_LOG_INFO(_logId << "RTP: Changing state from INVITE_WAITING_ANSWER_ACK_OR_PRACK to " << "NEGOTIATED" << " Count: " << rtpCount);

    break;

  case OFFER_WAITING_AUTHENTICATION:
    if (requestType != INVITE)
    {
      OSS_LOG_WARNING(_logId << "RTP: Ignoring request type=" << requestType << " for state " << "OFFER_WAITING_AUTHENTICATION");
      return;
    }

    handleStateOfferWaitingAuthentication(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);

    _state = ANSWER_REQUIRED_AUTHENTICATION;

    OSS_LOG_INFO(_logId << "RTP: Changing state from OFFER_WAITING_AUTHENTICATION to " << "ANSWER_REQUIRED_AUTHENTICATION" << " Count: " << rtpCount);

    break;

  case ANSWER_REQUIRED_AUTHENTICATION:
    if (requestType != INVITE_RESPONSE)
    {
      OSS_LOG_WARNING(_logId << "RTP: Ignoring request type=" << requestType << " for state " << "ANSWER_REQUIRED_AUTHENTICATION");
      return;
    }

    handleStateAnswerRequiredAuthentication(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);

    _state = NEGOTIATED;

    OSS_LOG_INFO(_logId << "RTP: Changing state from ANSWER_REQUIRED_AUTHENTICATION to " << "NEGOTIATED" << " Count: " << rtpCount);

    break;

  case UPDATE_WAITING_ANSWER:
    if (requestType != UPDATE_RESPONSE)
    {
      OSS_LOG_WARNING(_logId << "RTP: Ignoring request type=" << requestType << " for state " << "UPDATE_WAITING_ANSWER");
      return;
    }

    handleStateUpdateWaitingAnswer(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
    _state = NEGOTIATED;

    OSS_LOG_INFO(_logId << "RTP: Changing state from UPDATE_WAITING_ANSWER to " << "NEGOTIATED" << " Count: " << rtpCount);

    break;
  case PRACK_WAITING_ANSWER:
    if (requestType != PRACK_RESPONSE)
    {
      OSS_LOG_WARNING(_logId << "RTP: Ignoring request type=" << requestType << " for state " << "PRACK_WAITING_ANSWER");
      return;
    }
    handleStatePrackWaitingAnswer(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
    _state = NEGOTIATED;

     OSS_LOG_INFO(_logId << "RTP: Changing state from PRACK_WAITING_ANSWER to " << "NEGOTIATED" << " Count: " << rtpCount);

    break;
  case NEGOTIATED:
    if (requestType != INVITE && requestType != INVITE_RESPONSE && requestType != PRACK)
    {
      if (requestType == INVITE_ACK && !_lastSDPInAck.empty())
        sdp = _lastSDPInAck;
      OSS_LOG_WARNING(_logId << "RTP: Ignoring request type=" << requestType << " for state " << "NEGOTIATED");
      return;
    }

    handleStateNegotiated(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);

    if (requestType == INVITE)
    {
      _state = INVITE_WAITING_ANSWER_RESPONSE;
      newState = "INVITE_WAITING_ANSWER_RESPONSE";
    }
    else if (requestType == INVITE_RESPONSE)
    {
      _state = INVITE_WAITING_ANSWER_ACK_OR_PRACK;
       _lastSDPInAck = "";
       newState = "INVITE_WAITING_ANSWER_ACK_OR_PRACK";
    }
    else if (requestType == UPDATE)
    {
      _state = UPDATE_WAITING_ANSWER;
      newState = "UPDATE_WAITING_ANSWER";
    }
    else if (requestType == PRACK)
    {
      _state = PRACK_WAITING_ANSWER;
      newState = "PRACK_WAITING_ANSWER";
    }

    OSS_LOG_INFO(_logId << "RTP: Changing state from NEGOTIATED to " << newState << " Count: " << rtpCount);
    
    break;
  }
}

void RTPProxySession::handleStateIdle(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  handleInitialSDPOffer(sentBy, packetSourceIP, packetLocalInterface, 
    route, routeLocalInterface, requestType, sdp, rtpAttribute);
}

void RTPProxySession::handleStateOfferWaitingAuthentication(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute)
{
  handleSDPOffer(sentBy, packetSourceIP, packetLocalInterface,
    route, routeLocalInterface, requestType, sdp, rtpAttribute);
}

void RTPProxySession::handleStateAnswerRequiredAuthentication(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute)
{
  if (requestType != INVITE_RESPONSE)
    return;
  if (_isExpectingInitialAnswer)
    handleInitialSDPAnswer(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
  else
    handleSDPAnswer(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
  _isExpectingInitialAnswer = false;
}

void RTPProxySession::handleStateInviteWaitingAnswerResponse(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  if (requestType != INVITE_RESPONSE)
    return;
  if (_isExpectingInitialAnswer)
    handleInitialSDPAnswer(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
  else
    handleSDPAnswer(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
  _isExpectingInitialAnswer = false;
}

void RTPProxySession::handleStateInviteWaitingAnswerAckOrPrack(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  if (_isExpectingInitialAnswer)
    handleInitialSDPAnswer(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
  else
    handleSDPAnswer(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
  _isExpectingInitialAnswer = false;
}

void RTPProxySession::handleStateUpdateWaitingAnswer(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  handleSDPAnswer(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
  _isExpectingInitialAnswer = false;
}

void RTPProxySession::handleStatePrackWaitingAnswer(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  handleSDPAnswer(sentBy, packetSourceIP, packetLocalInterface,
      route, routeLocalInterface, requestType, sdp, rtpAttribute);
  _isExpectingInitialAnswer = false;
}

void RTPProxySession::handleStateNegotiated(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  //
  //  RTP proxy is already proxied prior.  force it.  
  //
  rtpAttribute.forceCreate = _hasOfferedAudioProxy || _hasOfferedVideoProxy || _hasOfferedFaxProxy;
  handleSDPOffer(sentBy, packetSourceIP, packetLocalInterface,
    route, routeLocalInterface, requestType, sdp, rtpAttribute);
}

void RTPProxySession::handleInitialSDPOffer(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  _hasOfferedAudioProxy = false;
  _hasOfferedVideoProxy = false;
  _hasOfferedFaxProxy = false;
  _leg1Identifier = "";
  _leg2Identifier = "";
  _lastOfferIndex = 0;

  bool forceCreateProxy = rtpAttribute.forceCreate || rtpAttribute.forcePEAEncryption;

  _isExpectingInitialAnswer = true;
  SDPSession offer(sdp.c_str());
  std::string sessionAddress = offer.getAddress();
  bool isBlackhole = (sessionAddress == "0.0.0.0");
  bool hasSessionAddress = (!sessionAddress.empty());
  bool hasChangedSessionAddress = false;
  //
  // Determine the session ID of the offerer and preserve it as leg1
  // Take note that the concept of legs in the RTP proxy does not necessarily
  // corespond to who the caller is in signalling
  //
  std::string oLine = offer.findHeader('o');
  if (oLine.empty())
  {
    throw SDPException("Unable to parse session-id from SDP offer.");
  }
  std::vector<std::string> oLineTokens = OSS::string_tokenize(oLine, " ");
  if (oLineTokens.size() != 6 || oLineTokens[1].empty() || oLineTokens[5].empty())
  {
    throw SDPException("Unable to parse session-id from SDP offer.");
  }

  _leg1Identifier = oLineTokens[1];
  //
  // Process audio session
  //
  SDPMedia::Ptr audio = offer.getMedia(SDPMedia::TYPE_AUDIO);
  if (audio)
  {
    std::string address = audio->getAddress();
    OSS::Net::IPAddress mediaAddress;
    bool hasMediaLevelAddress = !address.empty();
    if (hasMediaLevelAddress)
      mediaAddress = address;
    else if (!sessionAddress.empty())
      mediaAddress = sessionAddress;
    else
      throw SDPException("Unable to parse media address from SDP offer.");

    bool createProxy = false;
    if (mediaAddress.compare(routeLocalInterface, false /*don't include port*/))
      createProxy = false;
    else if (!forceCreateProxy && mediaAddress.isValid()
      && mediaAddress.isPrivate() && mediaAddress != packetSourceIP)
      createProxy = true;
    else if(forceCreateProxy)
      createProxy = true;

    if (createProxy)
    {
      //
      // Initialize the rtp proxy
      //
      OSS::Net::IPAddress leg1DataListener = packetLocalInterface;
      OSS::Net::IPAddress leg2DataListener = routeLocalInterface;
      OSS::Net::IPAddress leg1ControlListener = packetLocalInterface;
      OSS::Net::IPAddress leg2ControlListener = routeLocalInterface;
      if (_audio.open(leg1DataListener, leg2DataListener, leg1ControlListener, leg2ControlListener))
      {
        unsigned short dataPort = audio->getDataPort();
        unsigned short controlPort = audio->getControlPort();

        _audio.data().isLeg1XOREncrypted() = rtpAttribute.forcePEAEncryption;
        _audio.control().isLeg1XOREncrypted() = rtpAttribute.forcePEAEncryption;

        if (!mediaAddress.isPrivate())
        {
          _audio.data().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
          _audio.control().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        }
        else
        {
          _audio.data().leg1Destination() = boost::asio::ip::udp::endpoint(const_cast<IPAddress&>(packetSourceIP).address(), dataPort);
          _audio.control().leg1Destination() = boost::asio::ip::udp::endpoint(const_cast<IPAddress&>(packetSourceIP).address(), controlPort);
        }
        //
        // rewrite the SDP to be presented to leg2
        //
        OSS::Net::IPAddress dataAddress = _audio.data().getLeg2Address();
        OSS::Net::IPAddress controlAddress = _audio.control().getLeg2Address();

        if (hasSessionAddress)
        {
          if (!hasChangedSessionAddress && !isBlackhole)
          {
            if (dataAddress.externalAddress().empty())
              offer.changeAddress(dataAddress.toString(), "IP4");
            else
              offer.changeAddress(dataAddress.externalAddress(), "IP4");

            hasChangedSessionAddress = true;
          }

          if (hasMediaLevelAddress)
          {
            if (dataAddress.externalAddress().empty())
              audio->setAddressV4(dataAddress.toString());
            else
              audio->setAddressV4(dataAddress.externalAddress());
          }
        }
        else
        {
          if (dataAddress.externalAddress().empty())
            audio->setAddressV4(dataAddress.toString());
          else
            audio->setAddressV4(dataAddress.externalAddress());
        }

        if (!isBlackhole)
        {
          audio->setDataPort(dataAddress.getPort());
          audio->setControlPort(controlAddress.getPort());
        }
        _hasOfferedAudioProxy = true;
      }
    }
  }
  //
  // Process video session
  //
  SDPMedia::Ptr video = offer.getMedia(SDPMedia::TYPE_VIDEO);
  if (video)
  {
    std::string address = video->getAddress();
    OSS::Net::IPAddress mediaAddress;
    bool hasMediaLevelAddress = !address.empty();
    if (hasMediaLevelAddress)
      mediaAddress = address;
    else if (!sessionAddress.empty())
      mediaAddress = sessionAddress;
    else
      throw SDPException("Unable to parse media address from SDP offer.");

    bool createProxy = false;
    if (mediaAddress.compare(routeLocalInterface, false /*don't include port*/))
      createProxy = false;
    else if (!forceCreateProxy && mediaAddress.isValid()
      && mediaAddress.isPrivate() && mediaAddress != packetSourceIP)
      createProxy = true;
    else if(forceCreateProxy)
      createProxy = true;

    if (createProxy)
    {
      //
      // Initialize the rtp proxy
      //
      OSS::Net::IPAddress leg1DataListener = packetLocalInterface;
      OSS::Net::IPAddress leg2DataListener = routeLocalInterface;
      OSS::Net::IPAddress leg1ControlListener = packetLocalInterface;
      OSS::Net::IPAddress leg2ControlListener = routeLocalInterface;
      if (_video.open(leg1DataListener, leg2DataListener, leg1ControlListener, leg2ControlListener))
      {
        unsigned short dataPort = video->getDataPort();
        unsigned short controlPort = video->getControlPort();

        _video.data().isLeg1XOREncrypted() = rtpAttribute.forcePEAEncryption;
        _video.control().isLeg1XOREncrypted() = rtpAttribute.forcePEAEncryption;

        if (!mediaAddress.isPrivate())
        {
          _video.data().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
          _video.control().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        }
        else
        {
          _video.data().leg1Destination() = boost::asio::ip::udp::endpoint(const_cast<IPAddress&>(packetSourceIP).address(), dataPort);
          _video.control().leg1Destination() = boost::asio::ip::udp::endpoint(const_cast<IPAddress&>(packetSourceIP).address(), controlPort);
        }

        //
        // rewrite the SDP to be presented to leg2
        //
        OSS::Net::IPAddress dataAddress = _video.data().getLeg2Address();
        OSS::Net::IPAddress controlAddress = _video.control().getLeg2Address();

        if (hasSessionAddress)
        {
          if (!hasChangedSessionAddress && !isBlackhole)
          {
            if (dataAddress.externalAddress().empty())
              offer.changeAddress(dataAddress.toString(), "IP4");
            else
              offer.changeAddress(dataAddress.externalAddress(), "IP4");

            hasChangedSessionAddress = true;
          }

          if (hasMediaLevelAddress)
          {
            if (dataAddress.externalAddress().empty())
              video->setAddressV4(dataAddress.toString());
            else
              video->setAddressV4(dataAddress.externalAddress());
          }
        }
        else
        {
          if (dataAddress.externalAddress().empty())
            video->setAddressV4(dataAddress.toString());
          else
            video->setAddressV4(dataAddress.externalAddress());
        }

        if (!isBlackhole)
        {
          video->setDataPort(dataAddress.getPort());
          video->setControlPort(controlAddress.getPort());
        }

        _hasOfferedVideoProxy = true;
      }
    }
  }
  //
  // Process video session
  //
  SDPMedia::Ptr fax = offer.getMedia(SDPMedia::TYPE_FAX);
  if (fax)
  {
    std::string address = fax->getAddress();
    OSS::Net::IPAddress mediaAddress;
    bool hasMediaLevelAddress = !address.empty();
    if (hasMediaLevelAddress)
      mediaAddress = address;
    else if (!sessionAddress.empty())
      mediaAddress = sessionAddress;
    else
      throw SDPException("Unable to parse media address from SDP offer.");

    bool createProxy = false;
    if (mediaAddress.compare(routeLocalInterface, false /*don't include port*/))
      createProxy = false;
    else if (!forceCreateProxy && mediaAddress.isValid()
      && mediaAddress.isPrivate() && mediaAddress != packetSourceIP)
      createProxy = true;
    else if(forceCreateProxy)
      createProxy = true;

    if (createProxy)
    {
      //
      // Initialize the rtp proxy
      //
      OSS::Net::IPAddress leg1DataListener = packetLocalInterface;
      OSS::Net::IPAddress leg2DataListener = routeLocalInterface;
      OSS::Net::IPAddress leg1ControlListener = packetLocalInterface;
      OSS::Net::IPAddress leg2ControlListener = routeLocalInterface;
      if (_fax.open(leg1DataListener, leg2DataListener, leg1ControlListener, leg2ControlListener))
      {
        unsigned short dataPort = fax->getDataPort();
        unsigned short controlPort = fax->getControlPort();

        _fax.data().isLeg1XOREncrypted() = rtpAttribute.forcePEAEncryption;
        _fax.control().isLeg1XOREncrypted() = rtpAttribute.forcePEAEncryption;

        if (!mediaAddress.isPrivate())
        {
          _fax.data().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
          _fax.control().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        }
        else
        {
          _fax.data().leg1Destination() = boost::asio::ip::udp::endpoint(const_cast<IPAddress&>(packetSourceIP).address(), dataPort);
          _fax.control().leg1Destination() = boost::asio::ip::udp::endpoint(const_cast<IPAddress&>(packetSourceIP).address(), controlPort);
        }

        //
        // rewrite the SDP to be presented to leg2
        //
        OSS::Net::IPAddress dataAddress = _fax.data().getLeg2Address();
        OSS::Net::IPAddress controlAddress = _fax.control().getLeg2Address();

        if (hasSessionAddress)
        {
          if (!hasChangedSessionAddress && !isBlackhole)
          {
            if (dataAddress.externalAddress().empty())
              offer.changeAddress(dataAddress.toString(), "IP4");
            else
              offer.changeAddress(dataAddress.externalAddress(), "IP4");

            hasChangedSessionAddress = true;
          }

          if (hasMediaLevelAddress)
          {
            if (dataAddress.externalAddress().empty())
              fax->setAddressV4(dataAddress.toString());
            else
              fax->setAddressV4(dataAddress.externalAddress());
          }
        }
        else
        {
          if (dataAddress.externalAddress().empty())
            fax->setAddressV4(dataAddress.toString());
          else
            fax->setAddressV4(dataAddress.externalAddress());
        }

        if (!isBlackhole)
        {
          fax->setDataPort(dataAddress.getPort());
          fax->setControlPort(controlAddress.getPort());
        }

        _hasOfferedFaxProxy = true;
      }
    }
  }


  if (_hasOfferedAudioProxy || _hasOfferedVideoProxy || _hasOfferedFaxProxy)
  {
    sdp = offer.toString();
  }

}

void RTPProxySession::handleInitialSDPAnswer(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  OSS_VERIFY(_isExpectingInitialAnswer);
  _isExpectingInitialAnswer = false;

  SDPSession offer(sdp.c_str());
  std::string sessionAddress = offer.getAddress();
  bool isBlackhole = sessionAddress == "0.0.0.0"; //TODO: magic values
  bool hasSessionAddress = (!sessionAddress.empty());
  bool hasChangedSessionAddress = false;
  //
  // Determine the session ID of the offerer and preserve it as leg2
  // Take note that the concept of legs in the RTP proxy does not necessarily
  // corespond to who the caller is in signalling
  //
  std::string oLine = offer.findHeader('o');
  if (oLine.empty())
  {
    throw SDPException("Unable to parse session-id from SDP offer.");
  }
  std::vector<std::string> oLineTokens = OSS::string_tokenize(oLine, " ");
  if (oLineTokens.size() != 6 || oLineTokens[1].empty() || oLineTokens[5].empty())//TODO: magic values
  {
    throw SDPException("Unable to parse session-id from SDP offer.");
  }

  _leg2Identifier = oLineTokens[1];
  //
  // Process audio session
  //
  if (_hasOfferedAudioProxy)
  {
    SDPMedia::Ptr audio = offer.getMedia(SDPMedia::TYPE_AUDIO);
    if (!audio)
    {
      _audio.stop();
      _isAudioProxyNegotiated = false;
    }
    else
    {
      OSS::Net::IPAddress mediaAddress;
      bool hasMediaLevelAddress = !audio->getAddress().empty();
      if (hasMediaLevelAddress)
        mediaAddress = audio->getAddress();
      else if (!sessionAddress.empty())
        mediaAddress = sessionAddress;

      unsigned short dataPort = audio->getDataPort();
      unsigned short controlPort = audio->getControlPort();
      _audio.data().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
      _audio.control().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);

      _audio.data().isLeg2XOREncrypted() = rtpAttribute.forcePEAEncryption;
      _audio.control().isLeg2XOREncrypted() = rtpAttribute.forcePEAEncryption;

      _isAudioProxyNegotiated = true;
      _audio.start();
      //
      // rewrite the SDP to be presented to leg1
      //
      OSS::Net::IPAddress dataAddress = _audio.data().getLeg1Address();
      OSS::Net::IPAddress controlAddress = _audio.control().getLeg1Address();
      if (hasSessionAddress)
      {
        if (!hasChangedSessionAddress && !isBlackhole)
        {
          if (dataAddress.externalAddress().empty())
            offer.changeAddress(dataAddress.toString(), "IP4");
          else
            offer.changeAddress(dataAddress.externalAddress(), "IP4");

          hasChangedSessionAddress = true;
        }

        if (hasMediaLevelAddress)
        {
          if (dataAddress.externalAddress().empty())
            audio->setAddressV4(dataAddress.toString());
          else
            audio->setAddressV4(dataAddress.externalAddress());
        }
      }
      else
      {
        if (dataAddress.externalAddress().empty())
          audio->setAddressV4(dataAddress.toString());
        else
          audio->setAddressV4(dataAddress.externalAddress());
      }

      if (!isBlackhole)
      {
        audio->setDataPort(dataAddress.getPort());
        audio->setControlPort(controlAddress.getPort());
      }
    }
  }
  //
  // Process video session
  //
  if (_hasOfferedVideoProxy)
  {
    SDPMedia::Ptr video = offer.getMedia(SDPMedia::TYPE_VIDEO);
    if (!video)
    {
      _video.stop();
      _isVideoProxyNegotiated = false;
    }
    else
    {
      OSS::Net::IPAddress mediaAddress;
      bool hasMediaLevelAddress = !video->getAddress().empty();
      if (hasMediaLevelAddress)
        mediaAddress = video->getAddress();
      else if (!sessionAddress.empty())
        mediaAddress = sessionAddress;

      unsigned short dataPort = video->getDataPort();
      unsigned short controlPort = video->getControlPort();
      _video.data().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
      _video.control().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);

      _video.data().isLeg2XOREncrypted() = rtpAttribute.forcePEAEncryption;
      _video.control().isLeg2XOREncrypted() = rtpAttribute.forcePEAEncryption;

      _isVideoProxyNegotiated = true;
      _video.start();
      //
      // rewrite the SDP to be presented to leg1
      //
      OSS::Net::IPAddress dataAddress = _video.data().getLeg1Address();
      OSS::Net::IPAddress controlAddress = _video.control().getLeg1Address();

      if (hasSessionAddress)
      {
        if (!hasChangedSessionAddress && !isBlackhole)
        {
          if (dataAddress.externalAddress().empty())
            offer.changeAddress(dataAddress.toString(), "IP4");
          else
            offer.changeAddress(dataAddress.externalAddress(), "IP4");

          hasChangedSessionAddress = true;
        }

        if (hasMediaLevelAddress)
        {
          if (dataAddress.externalAddress().empty())
            video->setAddressV4(dataAddress.toString());
          else
            video->setAddressV4(dataAddress.externalAddress());
        }
      }
      else
      {
        if (dataAddress.externalAddress().empty())
          video->setAddressV4(dataAddress.toString());
        else
          video->setAddressV4(dataAddress.externalAddress());
      }

      if (!isBlackhole)
      {
        video->setDataPort(dataAddress.getPort());
        video->setControlPort(controlAddress.getPort());
      }
    }
  }
  //
  // Process fax session
  //
  if (_hasOfferedFaxProxy)
  {
    SDPMedia::Ptr fax = offer.getMedia(SDPMedia::TYPE_FAX);
    if (!fax)
    {
      _fax.stop();
      _isFaxProxyNegotiated = false;
    }
    else
    {
      OSS::Net::IPAddress mediaAddress;
      bool hasMediaLevelAddress = !fax->getAddress().empty();
      if (hasMediaLevelAddress)
        mediaAddress = fax->getAddress();
      else if (!sessionAddress.empty())
        mediaAddress = sessionAddress;

      unsigned short dataPort = fax->getDataPort();
      unsigned short controlPort = fax->getControlPort();
      _fax.data().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
      _fax.control().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);

      _fax.data().isLeg2XOREncrypted() = rtpAttribute.forcePEAEncryption;
      _fax.control().isLeg2XOREncrypted() = rtpAttribute.forcePEAEncryption;

      _isFaxProxyNegotiated = true;
      _fax.start();
      //
      // rewrite the SDP to be presented to leg1
      //
      OSS::Net::IPAddress dataAddress = _fax.data().getLeg1Address();
      OSS::Net::IPAddress controlAddress = _fax.control().getLeg1Address();

      if (hasSessionAddress)
      {
        if (!hasChangedSessionAddress && !isBlackhole)
        {
          if (dataAddress.externalAddress().empty())
            offer.changeAddress(dataAddress.toString(), "IP4");
          else
            offer.changeAddress(dataAddress.externalAddress(), "IP4");

          hasChangedSessionAddress = true;
        }

        if (hasMediaLevelAddress)
        {
          if (dataAddress.externalAddress().empty())
            fax->setAddressV4(dataAddress.toString());
          else
            fax->setAddressV4(dataAddress.externalAddress());
        }
      }
      else
      {
        if (dataAddress.externalAddress().empty())
          fax->setAddressV4(dataAddress.toString());
        else
          fax->setAddressV4(dataAddress.externalAddress());
      }

      if (!isBlackhole)
      {
        fax->setDataPort(dataAddress.getPort());
        fax->setControlPort(controlAddress.getPort());
      }
    }
  }

  if (_hasOfferedAudioProxy || _hasOfferedVideoProxy || _hasOfferedFaxProxy)
  {
    sdp = offer.toString();
    dumpStateFile();
  }
  else
  {
    setInactive();
  }
}

void RTPProxySession::handleSDPOffer(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  SDPSession offer(sdp.c_str());
  std::string sessionAddress = offer.getAddress();
  bool isBlackhole = (sessionAddress == "0.0.0.0");
  bool hasSessionAddress = (!sessionAddress.empty());
  bool hasChangedSessionAddress = false;
  //
  // Determine the session ID of the offerer and preserve it as leg2
  // Take note that the concept of legs in the RTP proxy does not necessarily
  // corespond to who the caller is in signalling
  //
  std::string oLine = offer.findHeader('o');
  if (oLine.empty())
  {
    throw SDPException("Unable to parse session-id from SDP offer.");
  }
  std::vector<std::string> oLineTokens = OSS::string_tokenize(oLine, " ");
  if (oLineTokens.size() != 6 || oLineTokens[1].empty() || oLineTokens[5].empty())
  {
    throw SDPException("Unable to parse session-id from SDP offer.");
  }

  int legIndex = 0;
  if (_leg1Identifier == oLineTokens[1])
  {
    _lastOfferIndex = legIndex = 1;
  }
  else if (_leg2Identifier == oLineTokens[1])
  {
    _lastOfferIndex = legIndex = 2;
  }
  else
  {
    //
    // This is a new session. Close the current stream and treat as a preliminary offer
    //
    stop();
    handleInitialSDPOffer(sentBy, packetSourceIP, packetLocalInterface, route,
      routeLocalInterface, requestType, sdp, rtpAttribute);
    return;
  }

  OSS_LOG_DEBUG(_logId << " SDP Session " << _identifier << " processing session update (OFFER) from leg " << legIndex);
  //
  // Process audio session
  //
  SDPMedia::Ptr audio = offer.getMedia(SDPMedia::TYPE_AUDIO);
  SDPMedia::Ptr video = offer.getMedia(SDPMedia::TYPE_VIDEO);
  SDPMedia::Ptr fax = offer.getMedia(SDPMedia::TYPE_FAX);
  if ((audio && !_isAudioProxyNegotiated) ||
    (video && !_isVideoProxyNegotiated) ||
    (fax && !_isFaxProxyNegotiated))
  {
    //
    // Although this is not really a new session
    // treat it as one since media is being modified.
    // Close the current stream and treat as a preliminary offer
    //
    stop();
    handleInitialSDPOffer(sentBy, packetSourceIP, packetLocalInterface, route,
      routeLocalInterface, requestType, sdp, rtpAttribute);
    return;
  }

  if (!audio && _isAudioProxyNegotiated)
  {
    //
    // The previous session has audio but it is no longer present in the new offer
    //
    _audio.stop();
    _hasOfferedAudioProxy = false;
    _isAudioProxyNegotiated = false;
  }
  else if (audio && _isAudioProxyNegotiated)
  {
    //
    // Audio is existing from previous session and is now being reoffered.
    // We need to reset the audio session states
    //
    std::string address = audio->getAddress();
    OSS::Net::IPAddress mediaAddress;
    bool hasMediaLevelAddress = !audio->getAddress().empty();
    if (hasMediaLevelAddress)
      mediaAddress = audio->getAddress();
    else if (!sessionAddress.empty())
      mediaAddress = sessionAddress;
    else
      throw SDPException("Unable to parse media address from SDP offer.");

    if (mediaAddress.isValid())
    {
      unsigned short dataPort = audio->getDataPort();
      unsigned short controlPort = audio->getControlPort();
      OSS::Net::IPAddress dataAddress;
      OSS::Net::IPAddress controlAddress;
      if (legIndex == 1)
      {
        _audio.data().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _audio.data().resetLeg1();
        _audio.control().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _audio.control().resetLeg1();
        dataAddress = _audio.data().getLeg2Address();
        controlAddress = _audio.control().getLeg2Address();
      }
      else
      {
        _audio.data().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _audio.data().resetLeg2();
        _audio.control().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _audio.control().resetLeg2();
        dataAddress = _audio.data().getLeg1Address();
        controlAddress = _audio.control().getLeg1Address();
      }
      //
      // rewrite the SDP to be presented to the other leg
      //
      if (hasSessionAddress)
      {
        if (!hasChangedSessionAddress && !isBlackhole)
        {
          if (dataAddress.externalAddress().empty())
            offer.changeAddress(dataAddress.toString(), "IP4");
          else
            offer.changeAddress(dataAddress.externalAddress(), "IP4");

          hasChangedSessionAddress = true;
        }

        if (hasMediaLevelAddress)
        {
          if (dataAddress.externalAddress().empty())
            audio->setAddressV4(dataAddress.toString());
          else
            audio->setAddressV4(dataAddress.externalAddress());
        }
      }
      else
      {
        if (dataAddress.externalAddress().empty())
          audio->setAddressV4(dataAddress.toString());
        else
          audio->setAddressV4(dataAddress.externalAddress());
      }

      if (!isBlackhole)
      {
        audio->setDataPort(dataAddress.getPort());
        audio->setControlPort(controlAddress.getPort());
      }
    }

    _hasOfferedAudioProxy = true;

  }

  //
  // Process video session
  //
  
  if (!video && _isVideoProxyNegotiated)
  {
    //
    // The previous session has video but it is no longer present in the new offer
    //
    _video.stop();
    _hasOfferedVideoProxy = false;
    _isVideoProxyNegotiated = false;
  }
  else if (video && _isVideoProxyNegotiated)
  {
    //
    // Video is existing from previous session and is now being reoffered.
    // We need to reset the video session states
    //
    std::string address = video->getAddress();
    OSS::Net::IPAddress mediaAddress;
    bool hasMediaLevelAddress = !video->getAddress().empty();
    if (hasMediaLevelAddress)
      mediaAddress = video->getAddress();
    else if (!sessionAddress.empty())
      mediaAddress = sessionAddress;
    else
      throw SDPException("Unable to parse media address from SDP offer.");

    if (mediaAddress.isValid())
    {
      unsigned short dataPort = video->getDataPort();
      unsigned short controlPort = video->getControlPort();
      OSS::Net::IPAddress dataAddress;
      OSS::Net::IPAddress controlAddress;
      if (legIndex == 1)
      {
        _video.data().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _video.data().resetLeg1();
        _video.control().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _video.control().resetLeg1();
        dataAddress = _video.data().getLeg2Address();
        controlAddress = _video.control().getLeg2Address();
      }
      else
      {
        _video.data().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _video.data().resetLeg2();
        _video.control().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _video.control().resetLeg2();
        dataAddress = _video.data().getLeg1Address();
        controlAddress = _video.control().getLeg1Address();
      }
      //
      // rewrite the SDP to be presented to the other leg
      //
      if (hasSessionAddress)
      {
        if (!hasChangedSessionAddress && !isBlackhole)
        {
          if (dataAddress.externalAddress().empty())
            offer.changeAddress(dataAddress.toString(), "IP4");
          else
            offer.changeAddress(dataAddress.externalAddress(), "IP4");

          hasChangedSessionAddress = true;
        }

        if (hasMediaLevelAddress)
        {
          if (dataAddress.externalAddress().empty())
            video->setAddressV4(dataAddress.toString());
          else
            video->setAddressV4(dataAddress.externalAddress());
        }
      }
      else
      {
        if (dataAddress.externalAddress().empty())
          video->setAddressV4(dataAddress.toString());
        else
          video->setAddressV4(dataAddress.externalAddress());
      }

      if (!isBlackhole)
      {
        video->setDataPort(dataAddress.getPort());
        video->setControlPort(controlAddress.getPort());
      }
    }

    _hasOfferedVideoProxy = true;

  }
  
  //
  // Process fax session
  //
  
  if (!fax && _isFaxProxyNegotiated)
  {
    //
    // The previous session has fax but it is no longer present in the new offer
    //
    _fax.stop();
    _hasOfferedFaxProxy = false;
    _isFaxProxyNegotiated = false;
  }
  else if (fax && _isFaxProxyNegotiated)
  {
    //
    // Fax is existing from previous session and is now being reoffered.
    // We need to reset the fax session states
    //
    std::string address = fax->getAddress();
    OSS::Net::IPAddress mediaAddress;
    bool hasMediaLevelAddress = !fax->getAddress().empty();
    if (hasMediaLevelAddress)
      mediaAddress = fax->getAddress();
    else if (!sessionAddress.empty())
      mediaAddress = sessionAddress;
    else
      throw SDPException("Unable to parse media address from SDP offer.");

    if (mediaAddress.isValid())
    {
      unsigned short dataPort = fax->getDataPort();
      unsigned short controlPort = fax->getControlPort();
      OSS::Net::IPAddress dataAddress;
      OSS::Net::IPAddress controlAddress;
      if (legIndex == 1)
      {
        _fax.data().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _fax.data().resetLeg1();
        _fax.control().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _fax.control().resetLeg1();
        dataAddress = _fax.data().getLeg2Address();
        controlAddress = _fax.control().getLeg2Address();
      }
      else
      {
        _fax.data().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _fax.data().resetLeg2();
        _fax.control().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _fax.control().resetLeg2();
        dataAddress = _fax.data().getLeg1Address();
        controlAddress = _fax.control().getLeg1Address();
      }
      //
      // rewrite the SDP to be presented to the other leg
      //
      if (hasSessionAddress)
      {
        if (!hasChangedSessionAddress && !isBlackhole)
        {
          if (dataAddress.externalAddress().empty())
            offer.changeAddress(dataAddress.toString(), "IP4");
          else
            offer.changeAddress(dataAddress.externalAddress(), "IP4");

          hasChangedSessionAddress = true;
        }

        if (hasMediaLevelAddress)
        {
          if (dataAddress.externalAddress().empty())
            fax->setAddressV4(dataAddress.toString());
          else
            fax->setAddressV4(dataAddress.externalAddress());
        }
      }
      else
      {
        if (dataAddress.externalAddress().empty())
          fax->setAddressV4(dataAddress.toString());
        else
          fax->setAddressV4(dataAddress.externalAddress());
      }

      if (!isBlackhole)
      {
        fax->setDataPort(dataAddress.getPort());
        fax->setControlPort(controlAddress.getPort());
      }
    }

    _hasOfferedFaxProxy = true;

  }
  
  if (_hasOfferedAudioProxy || _hasOfferedVideoProxy || _hasOfferedFaxProxy)
  {
    sdp = offer.toString();
  }

}



void RTPProxySession::handleSDPAnswer(
  const OSS::Net::IPAddress& sentBy,
  const OSS::Net::IPAddress& packetSourceIP,
  const OSS::Net::IPAddress& packetLocalInterface,
  const OSS::Net::IPAddress& route,
  const OSS::Net::IPAddress& routeLocalInterface,
  RequestType requestType,
  std::string& sdp,
  RTPProxy::Attributes& rtpAttribute)
{
  SDPSession offer(sdp.c_str());
  std::string sessionAddress = offer.getAddress();
  bool isBlackhole = (sessionAddress == "0.0.0.0");
  bool hasSessionAddress = (!sessionAddress.empty());
  bool hasChangedSessionAddress = false;
  //
  // Determine the session ID of the offerer and preserve it as leg2
  // Take note that the concept of legs in the RTP proxy does not necessarily
  // corespond to who the caller is in signalling
  //
  std::string oLine = offer.findHeader('o');
  if (oLine.empty())
  {
    throw SDPException("Unable to parse session-id from SDP offer.");
  }
  std::vector<std::string> oLineTokens = OSS::string_tokenize(oLine, " ");
  if (oLineTokens.size() != 6 || oLineTokens[1].empty() || oLineTokens[5].empty()) //TODO:magic values
  {
    throw SDPException("Unable to parse session-id from SDP offer.");
  }

  int legIndex = 0;
  if (_leg1Identifier == oLineTokens[1])
  {
    legIndex = 1;
  }
  else if (_leg2Identifier == oLineTokens[1])
  {
    legIndex = 2;
  }
  else
  {
    if (_lastOfferIndex == 0)
    {
      throw SDPException("Illegal session-id change");
    }
    else if (_lastOfferIndex == 1)
    {
      legIndex = 2;
      _leg2Identifier = oLineTokens[1];
    }
    else 
    {
      legIndex = 1;
      _leg1Identifier = oLineTokens[1];
    }
  }

  _lastOfferIndex = 0;

  OSS_LOG_DEBUG(_logId << " SDP Session " << _identifier << " processing session update (ANSWER) from leg " << legIndex);

  //
  // Process audio session
  //
  if (_hasOfferedAudioProxy)
  {
    SDPMedia::Ptr audio = offer.getMedia(SDPMedia::TYPE_AUDIO);
    if (!audio)
    {
      _audio.stop();
      _isAudioProxyNegotiated = false;
    }
    else
    {
      OSS::Net::IPAddress mediaAddress;
      std::string address = audio->getAddress();
      bool hasMediaLevelAddress = !address.empty();
      if (hasMediaLevelAddress)
        mediaAddress = audio->getAddress();
      else if (!sessionAddress.empty())
        mediaAddress = sessionAddress;

      unsigned short dataPort = audio->getDataPort();
      unsigned short controlPort = audio->getControlPort();
      OSS::Net::IPAddress dataAddress;
      OSS::Net::IPAddress controlAddress;
      if (legIndex == 1)
      {
        _audio.data().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _audio.control().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _isAudioProxyNegotiated = true;
        _audio.data().resetLeg1();
        _audio.control().resetLeg1();
        dataAddress = _audio.data().getLeg2Address();
        controlAddress = _audio.control().getLeg2Address();
      }
      else
      {
        _audio.data().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _audio.control().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _isAudioProxyNegotiated = true;
        _audio.data().resetLeg2();
        _audio.control().resetLeg2();
        dataAddress = _audio.data().getLeg1Address();
        controlAddress = _audio.control().getLeg1Address();
      }

      if (hasSessionAddress)
      {
        if (!hasChangedSessionAddress && !isBlackhole)
        {
          if (dataAddress.externalAddress().empty())
            offer.changeAddress(dataAddress.toString(), "IP4");
          else
            offer.changeAddress(dataAddress.externalAddress(), "IP4");

          hasChangedSessionAddress = true;
        }

        if (hasMediaLevelAddress)
        {
          if (dataAddress.externalAddress().empty())
            audio->setAddressV4(dataAddress.toString());
          else
            audio->setAddressV4(dataAddress.externalAddress());
        }
      }
      else
      {
        if (dataAddress.externalAddress().empty())
          audio->setAddressV4(dataAddress.toString());
        else
          audio->setAddressV4(dataAddress.externalAddress());
      }

      if (!isBlackhole)
      {
        audio->setDataPort(dataAddress.getPort());
        audio->setControlPort(controlAddress.getPort());
      }
    }
  }

  //
  // Process video session
  //
  if (_hasOfferedVideoProxy)
  {
    SDPMedia::Ptr video = offer.getMedia(SDPMedia::TYPE_VIDEO);
    if (!video)
    {
      _video.stop();
      _isVideoProxyNegotiated = false;
    }
    else
    {
      OSS::Net::IPAddress mediaAddress;
      std::string address = video->getAddress();
      bool hasMediaLevelAddress = !address.empty();
      if (hasMediaLevelAddress)
        mediaAddress = video->getAddress();
      else if (!sessionAddress.empty())
        mediaAddress = sessionAddress;

      unsigned short dataPort = video->getDataPort();
      unsigned short controlPort = video->getControlPort();
      OSS::Net::IPAddress dataAddress;
      OSS::Net::IPAddress controlAddress;
      if (legIndex == 1)
      {
        _video.data().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _video.control().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _isVideoProxyNegotiated = true;
        _video.data().resetLeg1();
        _video.control().resetLeg1();
        dataAddress = _video.data().getLeg2Address();
        controlAddress = _video.control().getLeg2Address();
      }
      else
      {
        _video.data().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _video.control().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _isVideoProxyNegotiated = true;
        _video.data().resetLeg2();
        _video.control().resetLeg2();
        dataAddress = _video.data().getLeg1Address();
        controlAddress = _video.control().getLeg1Address();
      }

      if (hasSessionAddress)
      {
        if (!hasChangedSessionAddress && !isBlackhole)
        {
          if (dataAddress.externalAddress().empty())
            offer.changeAddress(dataAddress.toString(), "IP4");
          else
            offer.changeAddress(dataAddress.externalAddress(), "IP4");

          hasChangedSessionAddress = true;
        }

        if (hasMediaLevelAddress)
        {
          if (dataAddress.externalAddress().empty())
            video->setAddressV4(dataAddress.toString());
          else
            video->setAddressV4(dataAddress.externalAddress());
        }
      }
      else
      {
        if (dataAddress.externalAddress().empty())
          video->setAddressV4(dataAddress.toString());
        else
          video->setAddressV4(dataAddress.externalAddress());
      }

      if (!isBlackhole)
      {
        video->setDataPort(dataAddress.getPort());
        video->setControlPort(controlAddress.getPort());
      }
    }
  }

  //
  // Process fax session
  //
  if (_hasOfferedFaxProxy)
  {
    SDPMedia::Ptr fax = offer.getMedia(SDPMedia::TYPE_FAX);
    if (!fax)
    {
      _fax.stop();
      _isFaxProxyNegotiated = false;
    }
    else
    {
      OSS::Net::IPAddress mediaAddress;
      std::string address = fax->getAddress();
      bool hasMediaLevelAddress = !address.empty();
      if (hasMediaLevelAddress)
        mediaAddress = fax->getAddress();
      else if (!sessionAddress.empty())
        mediaAddress = sessionAddress;

      unsigned short dataPort = fax->getDataPort();
      unsigned short controlPort = fax->getControlPort();
      OSS::Net::IPAddress dataAddress;
      OSS::Net::IPAddress controlAddress;
      if (legIndex == 1)
      {
        _fax.data().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _fax.control().leg1Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _isFaxProxyNegotiated = true;
        _fax.data().resetLeg1();
        _fax.control().resetLeg1();
        dataAddress = _fax.data().getLeg2Address();
        controlAddress = _fax.control().getLeg2Address();
      }
      else
      {
        _fax.data().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), dataPort);
        _fax.control().leg2Destination() = boost::asio::ip::udp::endpoint(mediaAddress.address(), controlPort);
        _isFaxProxyNegotiated = true;
        _fax.data().resetLeg2();
        _fax.control().resetLeg2();
        dataAddress = _fax.data().getLeg1Address();
        controlAddress = _fax.control().getLeg1Address();
      }

      if (hasSessionAddress)
      {
        if (!hasChangedSessionAddress && !isBlackhole)
        {
          if (dataAddress.externalAddress().empty())
            offer.changeAddress(dataAddress.toString(), "IP4");
          else
            offer.changeAddress(dataAddress.externalAddress(), "IP4");

          hasChangedSessionAddress = true;
        }

        if (hasMediaLevelAddress)
        {
          if (dataAddress.externalAddress().empty())
            fax->setAddressV4(dataAddress.toString());
          else
            fax->setAddressV4(dataAddress.externalAddress());
        }
      }
      else
      {
        if (dataAddress.externalAddress().empty())
          fax->setAddressV4(dataAddress.toString());
        else
          fax->setAddressV4(dataAddress.externalAddress());
      }

      if (!isBlackhole)
      {
        fax->setDataPort(dataAddress.getPort());
        fax->setControlPort(controlAddress.getPort());
      }
    }
  }
  
  if (_hasOfferedAudioProxy || _hasOfferedVideoProxy || _hasOfferedFaxProxy)
  {
    sdp = offer.toString();
    dumpStateFile();
  }
}

void RTPProxySession::dumpStateToRedis()
{
  RTPProxyRecord record;
  
  record.identifier = _identifier.c_str();
  record.logId = _logId.c_str();
  record.leg1Identifier = _leg1Identifier.c_str();
  record.leg2Identifier = _leg2Identifier.c_str();
  record.lastSDPInAck = _lastSDPInAck.c_str();
  record.isExpectingInitialAnswer = _isExpectingInitialAnswer;
  record.hasOfferedAudioProxy = _hasOfferedAudioProxy;
  record.hasOfferedVideoProxy = _hasOfferedVideoProxy;
  record.hasOfferedFaxProxy = _hasOfferedFaxProxy;
  record.isAudioProxyNegotiated = _isAudioProxyNegotiated;
  record.isVideoProxyNegotiated = _isVideoProxyNegotiated;
  record.isFaxProxyNegotiated = _isFaxProxyNegotiated;
  record.verbose = _verbose;
  record.state = _state;
  record.lastOfferIndex = _lastOfferIndex;

  if (_hasOfferedAudioProxy)
  {

    RTPProxy& audio_data = _audio.data();
    {
      std::ostringstream localEp1Strm;
      localEp1Strm << audio_data._localEndPointLeg1.address().to_string() << ":" << audio_data._localEndPointLeg1.port();

      std::ostringstream localEp2Strm;
      localEp2Strm << audio_data._localEndPointLeg2.address().to_string() << ":" << audio_data._localEndPointLeg2.port();

      std::ostringstream senderEp1Strm;
      senderEp1Strm << audio_data._senderEndPointLeg1.address().to_string() << ":" << audio_data._senderEndPointLeg1.port();

      std::ostringstream senderEp2Strm;
      senderEp2Strm << audio_data._senderEndPointLeg2.address().to_string() << ":" << audio_data._senderEndPointLeg2.port();

      std::ostringstream lastSenderEp1Strm;
      lastSenderEp1Strm << audio_data._lastSenderEndPointLeg1.address().to_string() << ":" << audio_data._lastSenderEndPointLeg1.port();

      std::ostringstream lastSenderEp2Strm;
      lastSenderEp2Strm << audio_data._lastSenderEndPointLeg2.address().to_string() << ":" << audio_data._lastSenderEndPointLeg2.port();

      record.audio.data.identifier = audio_data._identifier.c_str();
      record.audio.data.localEndPointLeg1 = localEp1Strm.str().c_str();
      record.audio.data.localEndPointLeg2 = localEp2Strm.str().c_str();
      record.audio.data.senderEndPointLeg1 = senderEp1Strm.str().c_str();
      record.audio.data.senderEndPointLeg2 = senderEp2Strm.str().c_str();
      record.audio.data.lastSenderEndPointLeg1 = lastSenderEp1Strm.str().c_str();
      record.audio.data.lastSenderEndPointLeg2 = lastSenderEp2Strm.str().c_str();
      record.audio.data.adjustSenderFromPacketSource = audio_data._adjustSenderFromPacketSource;
      record.audio.data.leg1Reset = audio_data._leg1Reset;
      record.audio.data.leg2Reset = audio_data._leg2Reset;
      record.audio.data.isStarted = audio_data._isStarted;
      record.audio.data.isInactive = audio_data._isInactive;
      record.audio.data.isLeg1XOREncrypted = audio_data._isLeg1XOREncrypted;
      record.audio.data.isLeg2XOREncrypted = audio_data._isLeg2XOREncrypted;
    }

    RTPProxy& audio_control = _audio.control();
    {
      std::ostringstream localEp1Strm;
      localEp1Strm << audio_control._localEndPointLeg1.address().to_string() << ":" << audio_control._localEndPointLeg1.port();

      std::ostringstream localEp2Strm;
      localEp2Strm << audio_control._localEndPointLeg2.address().to_string() << ":" << audio_control._localEndPointLeg2.port();

      std::ostringstream senderEp1Strm;
      senderEp1Strm << audio_control._senderEndPointLeg1.address().to_string() << ":" << audio_control._senderEndPointLeg1.port();

      std::ostringstream senderEp2Strm;
      senderEp2Strm << audio_control._senderEndPointLeg2.address().to_string() << ":" << audio_control._senderEndPointLeg2.port();

      std::ostringstream lastSenderEp1Strm;
      lastSenderEp1Strm << audio_control._lastSenderEndPointLeg1.address().to_string() << ":" << audio_control._lastSenderEndPointLeg1.port();

      std::ostringstream lastSenderEp2Strm;
      lastSenderEp2Strm << audio_control._lastSenderEndPointLeg2.address().to_string() << ":" << audio_control._lastSenderEndPointLeg2.port();

      record.audio.control.identifier = audio_control._identifier.c_str();
      record.audio.control.localEndPointLeg1 = localEp1Strm.str().c_str();
      record.audio.control.localEndPointLeg2 = localEp2Strm.str().c_str();
      record.audio.control.senderEndPointLeg1 = senderEp1Strm.str().c_str();
      record.audio.control.senderEndPointLeg2 = senderEp2Strm.str().c_str();
      record.audio.control.lastSenderEndPointLeg1 = lastSenderEp1Strm.str().c_str();
      record.audio.control.lastSenderEndPointLeg2 = lastSenderEp2Strm.str().c_str();
      record.audio.control.adjustSenderFromPacketSource = audio_control._adjustSenderFromPacketSource;
      record.audio.control.leg1Reset = audio_control._leg1Reset;
      record.audio.control.leg2Reset = audio_control._leg2Reset;
      record.audio.control.isStarted = audio_control._isStarted;
      record.audio.control.isInactive = audio_control._isInactive;
      record.audio.control.isLeg1XOREncrypted = audio_control._isLeg1XOREncrypted;
      record.audio.control.isLeg2XOREncrypted = audio_control._isLeg2XOREncrypted;
    }
  }

  if (_hasOfferedVideoProxy)
  {

    RTPProxy& video_data = _video.data();
    {
      std::ostringstream localEp1Strm;
      localEp1Strm << video_data._localEndPointLeg1.address().to_string() << ":" << video_data._localEndPointLeg1.port();

      std::ostringstream localEp2Strm;
      localEp2Strm << video_data._localEndPointLeg2.address().to_string() << ":" << video_data._localEndPointLeg2.port();

      std::ostringstream senderEp1Strm;
      senderEp1Strm << video_data._senderEndPointLeg1.address().to_string() << ":" << video_data._senderEndPointLeg1.port();

      std::ostringstream senderEp2Strm;
      senderEp2Strm << video_data._senderEndPointLeg2.address().to_string() << ":" << video_data._senderEndPointLeg2.port();

      std::ostringstream lastSenderEp1Strm;
      lastSenderEp1Strm << video_data._lastSenderEndPointLeg1.address().to_string() << ":" << video_data._lastSenderEndPointLeg1.port();

      std::ostringstream lastSenderEp2Strm;
      lastSenderEp2Strm << video_data._lastSenderEndPointLeg2.address().to_string() << ":" << video_data._lastSenderEndPointLeg2.port();

      record.video.data.identifier = video_data._identifier.c_str();
      record.video.data.localEndPointLeg1 = localEp1Strm.str().c_str();
      record.video.data.localEndPointLeg2 = localEp2Strm.str().c_str();
      record.video.data.senderEndPointLeg1 = senderEp1Strm.str().c_str();
      record.video.data.senderEndPointLeg2 = senderEp2Strm.str().c_str();
      record.video.data.lastSenderEndPointLeg1 = lastSenderEp1Strm.str().c_str();
      record.video.data.lastSenderEndPointLeg2 = lastSenderEp2Strm.str().c_str();
      record.video.data.adjustSenderFromPacketSource = video_data._adjustSenderFromPacketSource;
      record.video.data.leg1Reset = video_data._leg1Reset;
      record.video.data.leg2Reset = video_data._leg2Reset;
      record.video.data.isStarted = video_data._isStarted;
      record.video.data.isInactive = video_data._isInactive;
      record.video.data.isLeg1XOREncrypted = video_data._isLeg1XOREncrypted;
      record.video.data.isLeg2XOREncrypted = video_data._isLeg2XOREncrypted;
    }

    RTPProxy& video_control = _video.control();
    {
      std::ostringstream localEp1Strm;
      localEp1Strm << video_control._localEndPointLeg1.address().to_string() << ":" << video_control._localEndPointLeg1.port();

      std::ostringstream localEp2Strm;
      localEp2Strm << video_control._localEndPointLeg2.address().to_string() << ":" << video_control._localEndPointLeg2.port();

      std::ostringstream senderEp1Strm;
      senderEp1Strm << video_control._senderEndPointLeg1.address().to_string() << ":" << video_control._senderEndPointLeg1.port();

      std::ostringstream senderEp2Strm;
      senderEp2Strm << video_control._senderEndPointLeg2.address().to_string() << ":" << video_control._senderEndPointLeg2.port();

      std::ostringstream lastSenderEp1Strm;
      lastSenderEp1Strm << video_control._lastSenderEndPointLeg1.address().to_string() << ":" << video_control._lastSenderEndPointLeg1.port();

      std::ostringstream lastSenderEp2Strm;
      lastSenderEp2Strm << video_control._lastSenderEndPointLeg2.address().to_string() << ":" << video_control._lastSenderEndPointLeg2.port();

      record.video.control.identifier = video_control._identifier.c_str();
      record.video.control.localEndPointLeg1 = localEp1Strm.str().c_str();
      record.video.control.localEndPointLeg2 = localEp2Strm.str().c_str();
      record.video.control.senderEndPointLeg1 = senderEp1Strm.str().c_str();
      record.video.control.senderEndPointLeg2 = senderEp2Strm.str().c_str();
      record.video.control.lastSenderEndPointLeg1 = lastSenderEp1Strm.str().c_str();
      record.video.control.lastSenderEndPointLeg2 = lastSenderEp2Strm.str().c_str();
      record.video.control.adjustSenderFromPacketSource = video_control._adjustSenderFromPacketSource;
      record.video.control.leg1Reset = video_control._leg1Reset;
      record.video.control.leg2Reset = video_control._leg2Reset;
      record.video.control.isStarted = video_control._isStarted;
      record.video.control.isInactive = video_control._isInactive;
      record.video.control.isLeg1XOREncrypted = video_control._isLeg1XOREncrypted;
      record.video.control.isLeg2XOREncrypted = video_control._isLeg2XOREncrypted;
    }
  }
 
  if (_hasOfferedFaxProxy)
  {

    RTPProxy& fax_data = _fax.data();
    {
      std::ostringstream localEp1Strm;
      localEp1Strm << fax_data._localEndPointLeg1.address().to_string() << ":" << fax_data._localEndPointLeg1.port();

      std::ostringstream localEp2Strm;
      localEp2Strm << fax_data._localEndPointLeg2.address().to_string() << ":" << fax_data._localEndPointLeg2.port();

      std::ostringstream senderEp1Strm;
      senderEp1Strm << fax_data._senderEndPointLeg1.address().to_string() << ":" << fax_data._senderEndPointLeg1.port();

      std::ostringstream senderEp2Strm;
      senderEp2Strm << fax_data._senderEndPointLeg2.address().to_string() << ":" << fax_data._senderEndPointLeg2.port();

      std::ostringstream lastSenderEp1Strm;
      lastSenderEp1Strm << fax_data._lastSenderEndPointLeg1.address().to_string() << ":" << fax_data._lastSenderEndPointLeg1.port();

      std::ostringstream lastSenderEp2Strm;
      lastSenderEp2Strm << fax_data._lastSenderEndPointLeg2.address().to_string() << ":" << fax_data._lastSenderEndPointLeg2.port();

      record.fax.data.identifier = fax_data._identifier.c_str();
      record.fax.data.localEndPointLeg1 = localEp1Strm.str().c_str();
      record.fax.data.localEndPointLeg2 = localEp2Strm.str().c_str();
      record.fax.data.senderEndPointLeg1 = senderEp1Strm.str().c_str();
      record.fax.data.senderEndPointLeg2 = senderEp2Strm.str().c_str();
      record.fax.data.lastSenderEndPointLeg1 = lastSenderEp1Strm.str().c_str();
      record.fax.data.lastSenderEndPointLeg2 = lastSenderEp2Strm.str().c_str();
      record.fax.data.adjustSenderFromPacketSource = fax_data._adjustSenderFromPacketSource;
      record.fax.data.leg1Reset = fax_data._leg1Reset;
      record.fax.data.leg2Reset = fax_data._leg2Reset;
      record.fax.data.isStarted = fax_data._isStarted;
      record.fax.data.isInactive = fax_data._isInactive;
      record.fax.data.isLeg1XOREncrypted = fax_data._isLeg1XOREncrypted;
      record.fax.data.isLeg2XOREncrypted = fax_data._isLeg2XOREncrypted;
    }

    RTPProxy& fax_control = _fax.control();
    {
      std::ostringstream localEp1Strm;
      localEp1Strm << fax_control._localEndPointLeg1.address().to_string() << ":" << fax_control._localEndPointLeg1.port();

      std::ostringstream localEp2Strm;
      localEp2Strm << fax_control._localEndPointLeg2.address().to_string() << ":" << fax_control._localEndPointLeg2.port();

      std::ostringstream senderEp1Strm;
      senderEp1Strm << fax_control._senderEndPointLeg1.address().to_string() << ":" << fax_control._senderEndPointLeg1.port();

      std::ostringstream senderEp2Strm;
      senderEp2Strm << fax_control._senderEndPointLeg2.address().to_string() << ":" << fax_control._senderEndPointLeg2.port();

      std::ostringstream lastSenderEp1Strm;
      lastSenderEp1Strm << fax_control._lastSenderEndPointLeg1.address().to_string() << ":" << fax_control._lastSenderEndPointLeg1.port();

      std::ostringstream lastSenderEp2Strm;
      lastSenderEp2Strm << fax_control._lastSenderEndPointLeg2.address().to_string() << ":" << fax_control._lastSenderEndPointLeg2.port();

      record.fax.control.identifier = fax_control._identifier.c_str();
      record.fax.control.localEndPointLeg1 = localEp1Strm.str().c_str();
      record.fax.control.localEndPointLeg2 = localEp2Strm.str().c_str();
      record.fax.control.senderEndPointLeg1 = senderEp1Strm.str().c_str();
      record.fax.control.senderEndPointLeg2 = senderEp2Strm.str().c_str();
      record.fax.control.lastSenderEndPointLeg1 = lastSenderEp1Strm.str().c_str();
      record.fax.control.lastSenderEndPointLeg2 = lastSenderEp2Strm.str().c_str();
      record.fax.control.adjustSenderFromPacketSource = fax_control._adjustSenderFromPacketSource;
      record.fax.control.leg1Reset = fax_control._leg1Reset;
      record.fax.control.leg2Reset = fax_control._leg2Reset;
      record.fax.control.isStarted = fax_control._isStarted;
      record.fax.control.isInactive = fax_control._isInactive;
      record.fax.control.isLeg1XOREncrypted = fax_control._isLeg1XOREncrypted;
      record.fax.control.isLeg2XOREncrypted = fax_control._isLeg2XOREncrypted;
    }
  }
  
  record.writeToRedis(_pManager->redisClient(), _identifier);
}

void RTPProxySession::dumpStateFile()
{
  //
  // Check if we will be using redis
  //
  if (_pManager->hasRtpDb())
  {
    dumpStateToRedis();
    return;
  }

  //
  // Check if the manager allows persistence of state files to the disc
  //
  if (!_pManager->persistStateFiles())
    return;

  ClassType persistent;
  DataType root = persistent.self();

  DataType identifier = root.addGroupElement("identifier", DataType::TypeString);
  identifier = _identifier.c_str();

  DataType logId = root.addGroupElement("logId", DataType::TypeString);
  logId = _logId.c_str();

  DataType leg1Identifier = root.addGroupElement("leg1Identifier", DataType::TypeString);
  leg1Identifier = _leg1Identifier.c_str();

  DataType leg2Identifier = root.addGroupElement("leg2Identifier", DataType::TypeString);
  leg2Identifier = _leg2Identifier.c_str();

  DataType lastSDPInAck = root.addGroupElement("lastSDPInAck", DataType::TypeString);
  lastSDPInAck = _lastSDPInAck.c_str();

  DataType isExpectingInitialAnswer = root.addGroupElement("isExpectingInitialAnswer", DataType::TypeBoolean);
  isExpectingInitialAnswer = _isExpectingInitialAnswer;

  DataType hasOfferedAudioProxy = root.addGroupElement("hasOfferedAudioProxy", DataType::TypeBoolean);
  hasOfferedAudioProxy = _hasOfferedAudioProxy;

  DataType hasOfferedVideoProxy = root.addGroupElement("hasOfferedVideoProxy", DataType::TypeBoolean);
  hasOfferedVideoProxy = _hasOfferedVideoProxy;

  DataType hasOfferedFaxProxy = root.addGroupElement("hasOfferedFaxProxy", DataType::TypeBoolean);
  hasOfferedFaxProxy = _hasOfferedFaxProxy;

  DataType isAudioProxyNegotiated = root.addGroupElement("isAudioProxyNegotiated", DataType::TypeBoolean);
  isAudioProxyNegotiated = _isAudioProxyNegotiated;

  DataType isVideoProxyNegotiated = root.addGroupElement("isVideoProxyNegotiated", DataType::TypeBoolean);
  isVideoProxyNegotiated = _isVideoProxyNegotiated;

  DataType isFaxProxyNegotiated = root.addGroupElement("isFaxProxyNegotiated", DataType::TypeBoolean);
  isFaxProxyNegotiated = _isFaxProxyNegotiated;

  DataType verbose = root.addGroupElement("verbose", DataType::TypeBoolean);
  verbose = _verbose;

  DataType state = root.addGroupElement("state", DataType::TypeInt);
  state = _state;

  DataType lastOfferIndex  = root.addGroupElement("lastOfferIndex", DataType::TypeInt);
  lastOfferIndex = _lastOfferIndex;

  if (_hasOfferedAudioProxy)
  {
    DataType audio = root.addGroupElement("audio", DataType::TypeGroup);
    DataType data = audio.addGroupElement("data", DataType::TypeGroup);
    {
      RTPProxy& proxy = _audio.data();
      DataType identifier = data.addGroupElement("identifier", DataType::TypeString);
      identifier = proxy._identifier.c_str();

      DataType localEndPointLeg1 = data.addGroupElement("localEndPointLeg1", DataType::TypeString);
      std::ostringstream localEp1Strm;
      localEp1Strm << proxy._localEndPointLeg1.address().to_string() << ":" << proxy._localEndPointLeg1.port();
      localEndPointLeg1 = localEp1Strm.str().c_str();

      DataType localEndPointLeg2 = data.addGroupElement("localEndPointLeg2", DataType::TypeString);
      std::ostringstream localEp2Strm;
      localEp2Strm << proxy._localEndPointLeg2.address().to_string() << ":" << proxy._localEndPointLeg2.port();
      localEndPointLeg2 = localEp2Strm.str().c_str();

      DataType senderEndPointLeg1 = data.addGroupElement("senderEndPointLeg1", DataType::TypeString);
      std::ostringstream senderEp1Strm;
      senderEp1Strm << proxy._senderEndPointLeg1.address().to_string() << ":" << proxy._senderEndPointLeg1.port();
      senderEndPointLeg1 = senderEp1Strm.str().c_str();

      DataType senderEndPointLeg2 = data.addGroupElement("senderEndPointLeg2", DataType::TypeString);
      std::ostringstream senderEp2Strm;
      senderEp2Strm << proxy._senderEndPointLeg2.address().to_string() << ":" << proxy._senderEndPointLeg2.port();
      senderEndPointLeg2 = senderEp2Strm.str().c_str();

      DataType lastSenderEndPointLeg1 = data.addGroupElement("lastSenderEndPointLeg1", DataType::TypeString);
      std::ostringstream lastSenderEp1Strm;
      lastSenderEp1Strm << proxy._lastSenderEndPointLeg1.address().to_string() << ":" << proxy._lastSenderEndPointLeg1.port();
      lastSenderEndPointLeg1 = lastSenderEp1Strm.str().c_str();

      DataType lastSenderEndPointLeg2 = data.addGroupElement("lastSenderEndPointLeg2", DataType::TypeString);
      std::ostringstream lastSenderEp2Strm;
      lastSenderEp2Strm << proxy._lastSenderEndPointLeg2.address().to_string() << ":" << proxy._lastSenderEndPointLeg2.port();
      lastSenderEndPointLeg2 = lastSenderEp2Strm.str().c_str();


      DataType adjustSenderFromPacketSource = data.addGroupElement("adjustSenderFromPacketSource", DataType::TypeBoolean);
      adjustSenderFromPacketSource = proxy._adjustSenderFromPacketSource;

      DataType leg1Reset = data.addGroupElement("leg1Reset", DataType::TypeBoolean);
      leg1Reset = proxy._leg1Reset;

      DataType leg2Reset = data.addGroupElement("leg2Reset", DataType::TypeBoolean);
      leg2Reset = proxy._leg2Reset;

      DataType isStarted = data.addGroupElement("isStarted", DataType::TypeBoolean);
      isStarted = proxy._isStarted;

      DataType isInactive = data.addGroupElement("isInactive", DataType::TypeBoolean);
      isInactive = proxy._isInactive;

      DataType isLeg1XOREncrypted = data.addGroupElement("isLeg1XOREncrypted", DataType::TypeBoolean);
      isLeg1XOREncrypted = proxy._isLeg1XOREncrypted;

      DataType isLeg2XOREncrypted = data.addGroupElement("isLeg2XOREncrypted", DataType::TypeBoolean);
      isLeg2XOREncrypted = proxy._isLeg2XOREncrypted;
    }

    DataType control = audio.addGroupElement("control", DataType::TypeGroup);
    {
      RTPProxy& proxy = _audio.control();
      DataType identifier = control.addGroupElement("identifier", DataType::TypeString);
      identifier = proxy._identifier.c_str();

      DataType localEndPointLeg1 = control.addGroupElement("localEndPointLeg1", DataType::TypeString);
      std::ostringstream localEp1Strm;
      localEp1Strm << proxy._localEndPointLeg1.address().to_string() << ":" << proxy._localEndPointLeg1.port();
      localEndPointLeg1 = localEp1Strm.str().c_str();

      DataType localEndPointLeg2 = control.addGroupElement("localEndPointLeg2", DataType::TypeString);
      std::ostringstream localEp2Strm;
      localEp2Strm << proxy._localEndPointLeg2.address().to_string() << ":" << proxy._localEndPointLeg2.port();
      localEndPointLeg2 = localEp2Strm.str().c_str();

      DataType senderEndPointLeg1 = control.addGroupElement("senderEndPointLeg1", DataType::TypeString);
      std::ostringstream senderEp1Strm;
      senderEp1Strm << proxy._senderEndPointLeg1.address().to_string() << ":" << proxy._senderEndPointLeg1.port();
      senderEndPointLeg1 = senderEp1Strm.str().c_str();

      DataType senderEndPointLeg2 = control.addGroupElement("senderEndPointLeg2", DataType::TypeString);
      std::ostringstream senderEp2Strm;
      senderEp2Strm << proxy._senderEndPointLeg2.address().to_string() << ":" << proxy._senderEndPointLeg2.port();
      senderEndPointLeg2 = senderEp2Strm.str().c_str();

      DataType lastSenderEndPointLeg1 = control.addGroupElement("lastSenderEndPointLeg1", DataType::TypeString);
      std::ostringstream lastSenderEp1Strm;
      lastSenderEp1Strm << proxy._lastSenderEndPointLeg1.address().to_string() << ":" << proxy._lastSenderEndPointLeg1.port();
      lastSenderEndPointLeg1 = lastSenderEp1Strm.str().c_str();

      DataType lastSenderEndPointLeg2 = control.addGroupElement("lastSenderEndPointLeg2", DataType::TypeString);
      std::ostringstream lastSenderEp2Strm;
      lastSenderEp2Strm << proxy._lastSenderEndPointLeg2.address().to_string() << ":" << proxy._lastSenderEndPointLeg2.port();
      lastSenderEndPointLeg2 = lastSenderEp2Strm.str().c_str();


      DataType adjustSenderFromPacketSource = control.addGroupElement("adjustSenderFromPacketSource", DataType::TypeBoolean);
      adjustSenderFromPacketSource = proxy._adjustSenderFromPacketSource;

      DataType leg1Reset = control.addGroupElement("leg1Reset", DataType::TypeBoolean);
      leg1Reset = proxy._leg1Reset;

      DataType leg2Reset = control.addGroupElement("leg2Reset", DataType::TypeBoolean);
      leg2Reset = proxy._leg2Reset;

      DataType isStarted = control.addGroupElement("isStarted", DataType::TypeBoolean);
      isStarted = proxy._isStarted;

      DataType isInactive = control.addGroupElement("isInactive", DataType::TypeBoolean);
      isInactive = proxy._isInactive;

      DataType isLeg1XOREncrypted = control.addGroupElement("isLeg1XOREncrypted", DataType::TypeBoolean);
      isLeg1XOREncrypted = proxy._isLeg1XOREncrypted;

      DataType isLeg2XOREncrypted = control.addGroupElement("isLeg2XOREncrypted", DataType::TypeBoolean);
      isLeg2XOREncrypted = proxy._isLeg2XOREncrypted;
    }
  }

  if (_hasOfferedVideoProxy)
  {
    DataType video = root.addGroupElement("video", DataType::TypeGroup);
    DataType data = video.addGroupElement("data", DataType::TypeGroup);
    {
      RTPProxy& proxy = _video.data();
      DataType identifier = data.addGroupElement("identifier", DataType::TypeString);
      identifier = proxy._identifier.c_str();

      DataType localEndPointLeg1 = data.addGroupElement("localEndPointLeg1", DataType::TypeString);
      std::ostringstream localEp1Strm;
      localEp1Strm << proxy._localEndPointLeg1.address().to_string() << ":" << proxy._localEndPointLeg1.port();
      localEndPointLeg1 = localEp1Strm.str().c_str();

      DataType localEndPointLeg2 = data.addGroupElement("localEndPointLeg2", DataType::TypeString);
      std::ostringstream localEp2Strm;
      localEp2Strm << proxy._localEndPointLeg2.address().to_string() << ":" << proxy._localEndPointLeg2.port();
      localEndPointLeg2 = localEp2Strm.str().c_str();

      DataType senderEndPointLeg1 = data.addGroupElement("senderEndPointLeg1", DataType::TypeString);
      std::ostringstream senderEp1Strm;
      senderEp1Strm << proxy._senderEndPointLeg1.address().to_string() << ":" << proxy._senderEndPointLeg1.port();
      senderEndPointLeg1 = senderEp1Strm.str().c_str();

      DataType senderEndPointLeg2 = data.addGroupElement("senderEndPointLeg2", DataType::TypeString);
      std::ostringstream senderEp2Strm;
      senderEp2Strm << proxy._senderEndPointLeg2.address().to_string() << ":" << proxy._senderEndPointLeg2.port();
      senderEndPointLeg2 = senderEp2Strm.str().c_str();

      DataType lastSenderEndPointLeg1 = data.addGroupElement("lastSenderEndPointLeg1", DataType::TypeString);
      std::ostringstream lastSenderEp1Strm;
      lastSenderEp1Strm << proxy._lastSenderEndPointLeg1.address().to_string() << ":" << proxy._lastSenderEndPointLeg1.port();
      lastSenderEndPointLeg1 = lastSenderEp1Strm.str().c_str();

      DataType lastSenderEndPointLeg2 = data.addGroupElement("lastSenderEndPointLeg2", DataType::TypeString);
      std::ostringstream lastSenderEp2Strm;
      lastSenderEp2Strm << proxy._lastSenderEndPointLeg2.address().to_string() << ":" << proxy._lastSenderEndPointLeg2.port();
      lastSenderEndPointLeg2 = lastSenderEp2Strm.str().c_str();


      DataType adjustSenderFromPacketSource = data.addGroupElement("adjustSenderFromPacketSource", DataType::TypeBoolean);
      adjustSenderFromPacketSource = proxy._adjustSenderFromPacketSource;

      DataType leg1Reset = data.addGroupElement("leg1Reset", DataType::TypeBoolean);
      leg1Reset = proxy._leg1Reset;

      DataType leg2Reset = data.addGroupElement("leg2Reset", DataType::TypeBoolean);
      leg2Reset = proxy._leg2Reset;

      DataType isStarted = data.addGroupElement("isStarted", DataType::TypeBoolean);
      isStarted = proxy._isStarted;

      DataType isInactive = data.addGroupElement("isInactive", DataType::TypeBoolean);
      isInactive = proxy._isInactive;

      DataType isLeg1XOREncrypted = data.addGroupElement("isLeg1XOREncrypted", DataType::TypeBoolean);
      isLeg1XOREncrypted = proxy._isLeg1XOREncrypted;

      DataType isLeg2XOREncrypted = data.addGroupElement("isLeg2XOREncrypted", DataType::TypeBoolean);
      isLeg2XOREncrypted = proxy._isLeg2XOREncrypted;
    }

    DataType control = video.addGroupElement("control", DataType::TypeGroup);
    {
      RTPProxy& proxy = _video.control();
      DataType identifier = control.addGroupElement("identifier", DataType::TypeString);
      identifier = proxy._identifier.c_str();

      DataType localEndPointLeg1 = control.addGroupElement("localEndPointLeg1", DataType::TypeString);
      std::ostringstream localEp1Strm;
      localEp1Strm << proxy._localEndPointLeg1.address().to_string() << ":" << proxy._localEndPointLeg1.port();
      localEndPointLeg1 = localEp1Strm.str().c_str();

      DataType localEndPointLeg2 = control.addGroupElement("localEndPointLeg2", DataType::TypeString);
      std::ostringstream localEp2Strm;
      localEp2Strm << proxy._localEndPointLeg2.address().to_string() << ":" << proxy._localEndPointLeg2.port();
      localEndPointLeg2 = localEp2Strm.str().c_str();

      DataType senderEndPointLeg1 = control.addGroupElement("senderEndPointLeg1", DataType::TypeString);
      std::ostringstream senderEp1Strm;
      senderEp1Strm << proxy._senderEndPointLeg1.address().to_string() << ":" << proxy._senderEndPointLeg1.port();
      senderEndPointLeg1 = senderEp1Strm.str().c_str();

      DataType senderEndPointLeg2 = control.addGroupElement("senderEndPointLeg2", DataType::TypeString);
      std::ostringstream senderEp2Strm;
      senderEp2Strm << proxy._senderEndPointLeg2.address().to_string() << ":" << proxy._senderEndPointLeg2.port();
      senderEndPointLeg2 = senderEp2Strm.str().c_str();

      DataType lastSenderEndPointLeg1 = control.addGroupElement("lastSenderEndPointLeg1", DataType::TypeString);
      std::ostringstream lastSenderEp1Strm;
      lastSenderEp1Strm << proxy._lastSenderEndPointLeg1.address().to_string() << ":" << proxy._lastSenderEndPointLeg1.port();
      lastSenderEndPointLeg1 = lastSenderEp1Strm.str().c_str();

      DataType lastSenderEndPointLeg2 = control.addGroupElement("lastSenderEndPointLeg2", DataType::TypeString);
      std::ostringstream lastSenderEp2Strm;
      lastSenderEp2Strm << proxy._lastSenderEndPointLeg2.address().to_string() << ":" << proxy._lastSenderEndPointLeg2.port();
      lastSenderEndPointLeg2 = lastSenderEp2Strm.str().c_str();


      DataType adjustSenderFromPacketSource = control.addGroupElement("adjustSenderFromPacketSource", DataType::TypeBoolean);
      adjustSenderFromPacketSource = proxy._adjustSenderFromPacketSource;

      DataType leg1Reset = control.addGroupElement("leg1Reset", DataType::TypeBoolean);
      leg1Reset = proxy._leg1Reset;

      DataType leg2Reset = control.addGroupElement("leg2Reset", DataType::TypeBoolean);
      leg2Reset = proxy._leg2Reset;

      DataType isStarted = control.addGroupElement("isStarted", DataType::TypeBoolean);
      isStarted = proxy._isStarted;

      DataType isInactive = control.addGroupElement("isInactive", DataType::TypeBoolean);
      isInactive = proxy._isInactive;

      DataType isLeg1XOREncrypted = control.addGroupElement("isLeg1XOREncrypted", DataType::TypeBoolean);
      isLeg1XOREncrypted = proxy._isLeg1XOREncrypted;

      DataType isLeg2XOREncrypted = control.addGroupElement("isLeg2XOREncrypted", DataType::TypeBoolean);
      isLeg2XOREncrypted = proxy._isLeg2XOREncrypted;
    }
  }

  _stateFile = operator/(this->_pManager->getStateDirectory(), ClassType::createSafeFileName(this->_identifier.c_str()));
  persistent.persist(_stateFile);

}

RTPProxySession::Ptr RTPProxySession::reconstructFromRedis(RTPProxyManager* pManager, const std::string& identifier)
{
  RTPProxySession* pSession = 0;
  if (pManager->hasRtpDb())
  {
    RTPProxyRecord record;
    if (record.readFromRedis(pManager->redisClient(), identifier))
    {
      pSession = new RTPProxySession(pManager, record.identifier);
      pSession->_logId = record.logId;
      pSession->_leg1Identifier = record.leg1Identifier;
      pSession->_leg2Identifier = record.leg2Identifier;
      pSession->_lastSDPInAck = record.lastSDPInAck;
      pSession->_isExpectingInitialAnswer = record.isExpectingInitialAnswer;
      pSession->_hasOfferedAudioProxy = record.hasOfferedAudioProxy;
      pSession->_hasOfferedVideoProxy = record.hasOfferedVideoProxy;
      pSession->_hasOfferedFaxProxy = record.hasOfferedFaxProxy;
      pSession->_isAudioProxyNegotiated = record.isAudioProxyNegotiated;
      pSession->_isVideoProxyNegotiated = record.isVideoProxyNegotiated;
      pSession->_isFaxProxyNegotiated = record.isFaxProxyNegotiated;
      pSession->_verbose = record.verbose;
      pSession->_state = (State)record.state;
      pSession->_lastOfferIndex  = record.lastOfferIndex;
    }
    else
    {
      return RTPProxySession::Ptr();
    }
    
    if (pSession->_hasOfferedAudioProxy)
    {
      pSession->_audio.data()._identifier = record.audio.data.identifier;
      {
        OSS::Net::IPAddress localEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.audio.data.localEndPointLeg1.c_str());
        OSS::Net::IPAddress localEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.audio.data.localEndPointLeg2.c_str());
        OSS::Net::IPAddress senderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.audio.data.senderEndPointLeg1.c_str());
        OSS::Net::IPAddress senderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.audio.data.senderEndPointLeg2.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.audio.data.lastSenderEndPointLeg1.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.audio.data.lastSenderEndPointLeg2.c_str());

        pSession->_audio.data()._senderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg1.address(), senderEndPointLeg1.getPort());
        pSession->_audio.data()._senderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg2.address(), senderEndPointLeg2.getPort());

        pSession->_audio.data()._lastSenderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg1.address(), lastSenderEndPointLeg1.getPort());
        pSession->_audio.data()._lastSenderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg2.address(), lastSenderEndPointLeg2.getPort());

        pSession->_audio.data()._adjustSenderFromPacketSource = record.audio.data.adjustSenderFromPacketSource;
        pSession->_audio.data()._leg1Reset = record.audio.data.leg1Reset;
        pSession->_audio.data()._leg2Reset = record.audio.data.leg2Reset;
        pSession->_audio.data()._isStarted = record.audio.data.isStarted;
        pSession->_audio.data()._isInactive = record.audio.data.isInactive;
        pSession->_audio.data()._isLeg1XOREncrypted = record.audio.data.isLeg1XOREncrypted;
        pSession->_audio.data()._isLeg2XOREncrypted = record.audio.data.isLeg2XOREncrypted;
        if (!pSession->_audio.data().open(localEndPointLeg1, localEndPointLeg2))
        {
          delete pSession;
          return RTPProxySession::Ptr();
        }
        pSession->_audio.data().start();
      }

      pSession->_audio.control()._identifier = record.audio.control.identifier;
      {
        OSS::Net::IPAddress localEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.audio.control.localEndPointLeg1.c_str());
        OSS::Net::IPAddress localEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.audio.control.localEndPointLeg2.c_str());
        OSS::Net::IPAddress senderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.audio.control.senderEndPointLeg1.c_str());
        OSS::Net::IPAddress senderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.audio.control.senderEndPointLeg2.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.audio.control.lastSenderEndPointLeg1.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.audio.control.lastSenderEndPointLeg2.c_str());

        pSession->_audio.control()._senderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg1.address(), senderEndPointLeg1.getPort());
        pSession->_audio.control()._senderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg2.address(), senderEndPointLeg2.getPort());

        pSession->_audio.control()._lastSenderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg1.address(), lastSenderEndPointLeg1.getPort());
        pSession->_audio.control()._lastSenderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg2.address(), lastSenderEndPointLeg2.getPort());

        pSession->_audio.control()._adjustSenderFromPacketSource = record.audio.control.adjustSenderFromPacketSource;
        pSession->_audio.control()._leg1Reset = record.audio.control.leg1Reset;
        pSession->_audio.control()._leg2Reset = record.audio.control.leg2Reset;
        pSession->_audio.control()._isStarted = record.audio.control.isStarted;
        pSession->_audio.control()._isInactive = record.audio.control.isInactive;
        pSession->_audio.control()._isLeg1XOREncrypted = record.audio.control.isLeg1XOREncrypted;
        pSession->_audio.control()._isLeg2XOREncrypted = record.audio.control.isLeg2XOREncrypted;
        if (!pSession->_audio.control().open(localEndPointLeg1, localEndPointLeg2))
        {
          delete pSession;
          return RTPProxySession::Ptr();
        }
        pSession->_audio.control().start();
      }
    }

    if (pSession->_hasOfferedVideoProxy)
    {
      pSession->_video.data()._identifier = record.video.data.identifier;
      {
        OSS::Net::IPAddress localEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.video.data.localEndPointLeg1.c_str());
        OSS::Net::IPAddress localEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.video.data.localEndPointLeg2.c_str());
        OSS::Net::IPAddress senderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.video.data.senderEndPointLeg1.c_str());
        OSS::Net::IPAddress senderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.video.data.senderEndPointLeg2.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.video.data.lastSenderEndPointLeg1.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.video.data.lastSenderEndPointLeg2.c_str());

        pSession->_video.data()._senderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg1.address(), senderEndPointLeg1.getPort());
        pSession->_video.data()._senderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg2.address(), senderEndPointLeg2.getPort());

        pSession->_video.data()._lastSenderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg1.address(), lastSenderEndPointLeg1.getPort());
        pSession->_video.data()._lastSenderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg2.address(), lastSenderEndPointLeg2.getPort());

        pSession->_video.data()._adjustSenderFromPacketSource = record.video.data.adjustSenderFromPacketSource;
        pSession->_video.data()._leg1Reset = record.video.data.leg1Reset;
        pSession->_video.data()._leg2Reset = record.video.data.leg2Reset;
        pSession->_video.data()._isStarted = record.video.data.isStarted;
        pSession->_video.data()._isInactive = record.video.data.isInactive;
        pSession->_video.data()._isLeg1XOREncrypted = record.video.data.isLeg1XOREncrypted;
        pSession->_video.data()._isLeg2XOREncrypted = record.video.data.isLeg2XOREncrypted;
        if (!pSession->_video.data().open(localEndPointLeg1, localEndPointLeg2))
        {
          delete pSession;
          return RTPProxySession::Ptr();
        }
        pSession->_video.data().start();
      }

      pSession->_video.control()._identifier = record.video.control.identifier;
      {
        OSS::Net::IPAddress localEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.video.control.localEndPointLeg1.c_str());
        OSS::Net::IPAddress localEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.video.control.localEndPointLeg2.c_str());
        OSS::Net::IPAddress senderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.video.control.senderEndPointLeg1.c_str());
        OSS::Net::IPAddress senderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.video.control.senderEndPointLeg2.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.video.control.lastSenderEndPointLeg1.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.video.control.lastSenderEndPointLeg2.c_str());

        pSession->_video.control()._senderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg1.address(), senderEndPointLeg1.getPort());
        pSession->_video.control()._senderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg2.address(), senderEndPointLeg2.getPort());

        pSession->_video.control()._lastSenderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg1.address(), lastSenderEndPointLeg1.getPort());
        pSession->_video.control()._lastSenderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg2.address(), lastSenderEndPointLeg2.getPort());

        pSession->_video.control()._adjustSenderFromPacketSource = record.video.control.adjustSenderFromPacketSource;
        pSession->_video.control()._leg1Reset = record.video.control.leg1Reset;
        pSession->_video.control()._leg2Reset = record.video.control.leg2Reset;
        pSession->_video.control()._isStarted = record.video.control.isStarted;
        pSession->_video.control()._isInactive = record.video.control.isInactive;
        pSession->_video.control()._isLeg1XOREncrypted = record.video.control.isLeg1XOREncrypted;
        pSession->_video.control()._isLeg2XOREncrypted = record.video.control.isLeg2XOREncrypted;
        if (!pSession->_video.control().open(localEndPointLeg1, localEndPointLeg2))
        {
          delete pSession;
          return RTPProxySession::Ptr();
        }
        pSession->_video.control().start();
      }
    }

    if (pSession->_hasOfferedFaxProxy)
    {
      pSession->_fax.data()._identifier = record.fax.data.identifier;
      {
        OSS::Net::IPAddress localEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.fax.data.localEndPointLeg1.c_str());
        OSS::Net::IPAddress localEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.fax.data.localEndPointLeg2.c_str());
        OSS::Net::IPAddress senderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.fax.data.senderEndPointLeg1.c_str());
        OSS::Net::IPAddress senderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.fax.data.senderEndPointLeg2.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.fax.data.lastSenderEndPointLeg1.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.fax.data.lastSenderEndPointLeg2.c_str());

        pSession->_fax.data()._senderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg1.address(), senderEndPointLeg1.getPort());
        pSession->_fax.data()._senderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg2.address(), senderEndPointLeg2.getPort());

        pSession->_fax.data()._lastSenderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg1.address(), lastSenderEndPointLeg1.getPort());
        pSession->_fax.data()._lastSenderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg2.address(), lastSenderEndPointLeg2.getPort());

        pSession->_fax.data()._adjustSenderFromPacketSource = record.fax.data.adjustSenderFromPacketSource;
        pSession->_fax.data()._leg1Reset = record.fax.data.leg1Reset;
        pSession->_fax.data()._leg2Reset = record.fax.data.leg2Reset;
        pSession->_fax.data()._isStarted = record.fax.data.isStarted;
        pSession->_fax.data()._isInactive = record.fax.data.isInactive;
        pSession->_fax.data()._isLeg1XOREncrypted = record.fax.data.isLeg1XOREncrypted;
        pSession->_fax.data()._isLeg2XOREncrypted = record.fax.data.isLeg2XOREncrypted;
        if (!pSession->_fax.data().open(localEndPointLeg1, localEndPointLeg2))
        {
          delete pSession;
          return RTPProxySession::Ptr();
        }
        pSession->_fax.data().start();
      }

      pSession->_fax.control()._identifier = record.fax.control.identifier;
      {
        OSS::Net::IPAddress localEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.fax.control.localEndPointLeg1.c_str());
        OSS::Net::IPAddress localEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.fax.control.localEndPointLeg2.c_str());
        OSS::Net::IPAddress senderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.fax.control.senderEndPointLeg1.c_str());
        OSS::Net::IPAddress senderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.fax.control.senderEndPointLeg2.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort(record.fax.control.lastSenderEndPointLeg1.c_str());
        OSS::Net::IPAddress lastSenderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort(record.fax.control.lastSenderEndPointLeg2.c_str());

        pSession->_fax.control()._senderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg1.address(), senderEndPointLeg1.getPort());
        pSession->_fax.control()._senderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg2.address(), senderEndPointLeg2.getPort());

        pSession->_fax.control()._lastSenderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg1.address(), lastSenderEndPointLeg1.getPort());
        pSession->_fax.control()._lastSenderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg2.address(), lastSenderEndPointLeg2.getPort());

        pSession->_fax.control()._adjustSenderFromPacketSource = record.fax.control.adjustSenderFromPacketSource;
        pSession->_fax.control()._leg1Reset = record.fax.control.leg1Reset;
        pSession->_fax.control()._leg2Reset = record.fax.control.leg2Reset;
        pSession->_fax.control()._isStarted = record.fax.control.isStarted;
        pSession->_fax.control()._isInactive = record.fax.control.isInactive;
        pSession->_fax.control()._isLeg1XOREncrypted = record.fax.control.isLeg1XOREncrypted;
        pSession->_fax.control()._isLeg2XOREncrypted = record.fax.control.isLeg2XOREncrypted;
        if (!pSession->_fax.control().open(localEndPointLeg1, localEndPointLeg2))
        {
          delete pSession;
          return RTPProxySession::Ptr();
        }
        pSession->_fax.control().start();
      }
    }
  }
  return RTPProxySession::Ptr(pSession);
}


RTPProxySession::Ptr RTPProxySession::reconstructFromStateFile(
  RTPProxyManager* pManager, const boost::filesystem::path& stateFile)
{
  if (!pManager->persistStateFiles())
  {
    return RTPProxySession::Ptr();
  }

  RTPProxySession* pSession = 0;
  try
  {
    ClassType persistent;
    if (!persistent.load(stateFile))
    {
      return RTPProxySession::Ptr();
    }
    DataType root = persistent.self();

    pSession = new RTPProxySession(pManager, (const char*)root["identifier"]);
    pSession->_stateFile = stateFile;
    pSession->_logId = (const char*)root["logId"];
    pSession->_leg1Identifier = (const char*)root["leg1Identifier"];
    pSession->_leg2Identifier = (const char*)root["leg2Identifier"];
    pSession->_lastSDPInAck = (const char*)root["lastSDPInAck"];
    pSession->_isExpectingInitialAnswer = (bool)root["isExpectingInitialAnswer"];
    pSession->_hasOfferedAudioProxy = (bool)root["hasOfferedAudioProxy"];
    pSession->_hasOfferedVideoProxy = (bool)root["hasOfferedVideoProxy"];
    pSession->_hasOfferedFaxProxy = (bool)root["hasOfferedFaxProxy"];
    pSession->_isAudioProxyNegotiated = (bool)root["isAudioProxyNegotiated"];
    pSession->_isVideoProxyNegotiated = (bool)root["isVideoProxyNegotiated"];
    pSession->_isFaxProxyNegotiated = (bool)root["isFaxProxyNegotiated"];
    pSession->_verbose = (bool)root["verbose"];
    pSession->_state = (State)(int)root["state"];
    pSession->_lastOfferIndex  = (int)root["lastOfferIndex"];

    if (pSession->_hasOfferedAudioProxy)
    {
      DataType audio = root["audio"];
      DataType data = audio["data"];
      {
        pSession->_audio.data()._identifier = (const char*)data["identifier"];

        OSS::Net::IPAddress localEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["localEndPointLeg1"]);
        OSS::Net::IPAddress localEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["localEndPointLeg2"]);
        OSS::Net::IPAddress senderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["senderEndPointLeg1"]);
        OSS::Net::IPAddress senderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["senderEndPointLeg2"]);
        OSS::Net::IPAddress lastSenderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["lastSenderEndPointLeg1"]);
        OSS::Net::IPAddress lastSenderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["lastSenderEndPointLeg2"]);
        
        pSession->_audio.data()._senderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg1.address(), senderEndPointLeg1.getPort());
        pSession->_audio.data()._senderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg2.address(), senderEndPointLeg2.getPort());
        
        pSession->_audio.data()._lastSenderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg1.address(), lastSenderEndPointLeg1.getPort());
        pSession->_audio.data()._lastSenderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg2.address(), lastSenderEndPointLeg2.getPort());

        pSession->_audio.data()._adjustSenderFromPacketSource = (bool)data["adjustSenderFromPacketSource"];
        pSession->_audio.data()._leg1Reset = (bool)data["leg1Reset"];
        pSession->_audio.data()._leg2Reset = (bool)data["leg2Reset"];
        pSession->_audio.data()._isStarted = (bool)data["isStarted"];
        pSession->_audio.data()._isInactive = (bool)data["isInactive"];
        pSession->_audio.data()._isLeg1XOREncrypted = (bool)data["isLeg1XOREncrypted"];
        pSession->_audio.data()._isLeg2XOREncrypted = (bool)data["isLeg2XOREncrypted"];

        if (!pSession->_audio.data().open(localEndPointLeg1, localEndPointLeg2))
        {
          delete pSession;
          return RTPProxySession::Ptr();
        }
        pSession->_audio.data().start();
      }

      DataType control = audio["control"];
      {
        pSession->_audio.control()._identifier = (const char*)control["identifier"];

        OSS::Net::IPAddress localEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["localEndPointLeg1"]);
        OSS::Net::IPAddress localEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["localEndPointLeg2"]);
        OSS::Net::IPAddress senderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["senderEndPointLeg1"]);
        OSS::Net::IPAddress senderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["senderEndPointLeg2"]);
        OSS::Net::IPAddress lastSenderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["lastSenderEndPointLeg1"]);
        OSS::Net::IPAddress lastSenderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["lastSenderEndPointLeg2"]);

        pSession->_audio.control()._senderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg1.address(), senderEndPointLeg1.getPort());
        pSession->_audio.control()._senderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg2.address(), senderEndPointLeg2.getPort());

        pSession->_audio.control()._lastSenderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg1.address(), lastSenderEndPointLeg1.getPort());
        pSession->_audio.control()._lastSenderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg2.address(), lastSenderEndPointLeg2.getPort());

        pSession->_audio.control()._adjustSenderFromPacketSource = (bool)control["adjustSenderFromPacketSource"];
        pSession->_audio.control()._leg1Reset = (bool)control["leg1Reset"];
        pSession->_audio.control()._leg2Reset = (bool)control["leg2Reset"];
        pSession->_audio.control()._isStarted = (bool)control["isStarted"];
        pSession->_audio.control()._isInactive = (bool)control["isInactive"];
        pSession->_audio.control()._isLeg1XOREncrypted = (bool)control["isLeg1XOREncrypted"];
        pSession->_audio.control()._isLeg2XOREncrypted = (bool)control["isLeg2XOREncrypted"];

        if (!pSession->_audio.control().open(localEndPointLeg1, localEndPointLeg2))
        {
          delete pSession;
          return RTPProxySession::Ptr();
        }
        pSession->_audio.control().start();
      }
    }

    if (pSession->_hasOfferedVideoProxy)
    {
      DataType video = root["video"];
      DataType data = video["data"];
      {
        pSession->_video.data()._identifier = (const char*)data["identifier"];

        OSS::Net::IPAddress localEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["localEndPointLeg1"]);
        OSS::Net::IPAddress localEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["localEndPointLeg2"]);
        OSS::Net::IPAddress senderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["senderEndPointLeg1"]);
        OSS::Net::IPAddress senderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["senderEndPointLeg2"]);
        OSS::Net::IPAddress lastSenderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["lastSenderEndPointLeg1"]);
        OSS::Net::IPAddress lastSenderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)data["lastSenderEndPointLeg2"]);

        pSession->_video.data()._senderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg1.address(), senderEndPointLeg1.getPort());
        pSession->_video.data()._senderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg2.address(), senderEndPointLeg2.getPort());

        pSession->_video.data()._lastSenderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg1.address(), lastSenderEndPointLeg1.getPort());
        pSession->_video.data()._lastSenderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg2.address(), lastSenderEndPointLeg2.getPort());

        pSession->_video.data()._adjustSenderFromPacketSource = (bool)data["adjustSenderFromPacketSource"];
        pSession->_video.data()._leg1Reset = (bool)data["leg1Reset"];
        pSession->_video.data()._leg2Reset = (bool)data["leg2Reset"];
        pSession->_video.data()._isStarted = (bool)data["isStarted"];
        pSession->_video.data()._isInactive = (bool)data["isInactive"];
        pSession->_video.data()._isLeg1XOREncrypted = (bool)data["isLeg1XOREncrypted"];
        pSession->_video.data()._isLeg2XOREncrypted = (bool)data["isLeg2XOREncrypted"];

        if (!pSession->_video.data().open(localEndPointLeg1, localEndPointLeg2))
        {
          delete pSession;
          return RTPProxySession::Ptr();
        }
        pSession->_video.data().start();
      }

      DataType control = video["control"];
      {
        pSession->_video.control()._identifier = (const char*)control["identifier"];

        OSS::Net::IPAddress localEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["localEndPointLeg1"]);
        OSS::Net::IPAddress localEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["localEndPointLeg2"]);
        OSS::Net::IPAddress senderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["senderEndPointLeg1"]);
        OSS::Net::IPAddress senderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["senderEndPointLeg2"]);
        OSS::Net::IPAddress lastSenderEndPointLeg1 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["lastSenderEndPointLeg1"]);
        OSS::Net::IPAddress lastSenderEndPointLeg2 = OSS::Net::IPAddress::fromV4IPPort((const char*)control["lastSenderEndPointLeg2"]);

        pSession->_video.control()._senderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg1.address(), senderEndPointLeg1.getPort());
        pSession->_video.control()._senderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(senderEndPointLeg2.address(), senderEndPointLeg2.getPort());

        pSession->_video.control()._lastSenderEndPointLeg1 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg1.address(), lastSenderEndPointLeg1.getPort());
        pSession->_video.control()._lastSenderEndPointLeg2 =
          boost::asio::ip::udp::endpoint(lastSenderEndPointLeg2.address(), lastSenderEndPointLeg2.getPort());

        pSession->_video.control()._adjustSenderFromPacketSource = (bool)control["adjustSenderFromPacketSource"];
        pSession->_video.control()._leg1Reset = (bool)control["leg1Reset"];
        pSession->_video.control()._leg2Reset = (bool)control["leg2Reset"];
        pSession->_video.control()._isStarted = (bool)control["isStarted"];
        pSession->_video.control()._isInactive = (bool)control["isInactive"];
        pSession->_video.control()._isLeg1XOREncrypted = (bool)control["isLeg1XOREncrypted"];
        pSession->_video.control()._isLeg2XOREncrypted = (bool)control["isLeg2XOREncrypted"];

        if (!pSession->_video.control().open(localEndPointLeg1, localEndPointLeg2))
        {
          delete pSession;
          return RTPProxySession::Ptr();
        }
        pSession->_video.control().start();
      }
    }
  }
  catch(...)
  {
    delete pSession;
    return RTPProxySession::Ptr();
  }

  return RTPProxySession::Ptr(pSession);
}

void RTPProxySession::handleAuthStateTimeout(const boost::system::error_code& e)
{
  if (!e)
  {
    _isAuthTimeout = true;
  }
}

void RTPProxySession::setState(RTPProxySession::State state)
{
  if (state == OFFER_WAITING_AUTHENTICATION)
  {
    //
    // start the timer here because there is no guaranty that a new
    // request with a credential would be sent
    //
    _authStateTimer.async_wait(boost::bind(&RTPProxySession::handleAuthStateTimeout, this, boost::asio::placeholders::error));
  }
  else
  {
    _authStateTimer.cancel();
  }
  _state = state;
}

} } // OSS::RTP

