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


#ifndef RTP_RTPProxySession_INCLUDED
#define RTP_RTPProxySession_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_RTP

#include "OSS/UTL/Thread.h"
#include "OSS/RTP/RTPProxyTuple.h"


namespace OSS {
namespace RTP {

class OSS_API RTPProxySession
{
public:
  typedef boost::shared_ptr<RTPProxySession> Ptr;

  enum RequestType
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
    OFFER_WAITING_AUTHENTICATION,
    ANSWER_REQUIRED_AUTHENTICATION,
    UPDATE_WAITING_ANSWER,
    PRACK_WAITING_ANSWER,
    NEGOTIATED
  };

  RTPProxySession(RTPProxyManager* pManager, const std::string& identifier);
    /// Creates a new RTPProxySession

  ~RTPProxySession();
    /// Destroys RTPProxySession

  void stop();
    /// Stop the RTP sessions

  void handleSDP(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);
    /// handles the incoming SDP offer or answer.
    /// The will automatically rewrite the SDP
    /// to represent the new cleated RTP session
    /// attributes.  Right now only the first occurence
    /// of audio, video and fax m lines are supported.
    /// New stream types may be added in the future.
    /// This will throw and RTPProxyException if
    /// the SDP cannot be processed

  RTPProxyManager*& manager();
    /// Returns a direct pointer to the manager

  const std::string& getIdentifier() const;
    /// Returns a reference to the identifier

  State getState() const;
    /// Returns the STATE of the session

  void setState(State state);
    /// Changes the state of the session

  bool isVoiceInactive() const;
    /// Returns true if media is stopped due to inactivity

  bool isVideoInactive() const;
    /// Returns true if media is stopped due to inactivity

  bool isFaxInactive() const;
    /// Returns true if media is stopped due to inactivity

  bool isAuthTimeout() const;
    /// This flag indicates that the state has remained in authenticating state
    /// longer than the designated timeout

  std::string& logId();
    /// Reference to the log identifier if it was set by the application

  bool& verbose();
    /// True if verbose logging is set by the application.
    /// Use this only to debug the rtp stream and not for production
    /// environment.
#if ENABLE_FEATURE_CONFIG
  void dumpStateFile();
    /// This method will save session information to a state-file to allow the
    /// manager to reconstruct during retarts
#endif
  
#if ENABLE_FEATURE_REDIS
  void dumpStateToRedis();
    /// This method will save session information to a redis to allow the
    /// manager to reconstruct during retarts
#endif

#if ENABLE_FEATURE_CONFIG
  static RTPProxySession::Ptr reconstructFromStateFile(RTPProxyManager* pManager,
    const boost::filesystem::path& stateFile);
    /// Reconstruct session-state from a state file.  Will return false
    /// if the session can't be reconstructed
#endif  
  
#if ENABLE_FEATURE_REDIS
  static RTPProxySession::Ptr reconstructFromRedis(RTPProxyManager* pManager,
    const std::string& identifier);
    /// Reconstruct session-state from redis.  Will return false
    /// if the session can't be reconstructed
#endif

  std::string& from();
    /// Return the from header of the transaction that created the session

  std::string& to();
    /// Return the to header of the transaction that created the session

  std::string& callId();
    /// Return the call-id of the transaction that created the session

  void setResizerSamples(int leg1, int leg2);
    /// Enable resizing of RTP packets

  int getResizerSamplesLeg1() const;
  int getResizerSamplesLeg2() const;

  void setMonitoredRoute(const std::string& route);
  const std::string& getMonitoredRoute() const;
  
  const std::string& getLastOffer() const;
  const std::string& getLastAnswer() const;
protected:
  void handleStateIdle(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void handleStateInviteWaitingAnswerResponse(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void handleStateInviteWaitingAnswerAckOrPrack(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void handleStateOfferWaitingAuthentication(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void handleStateAnswerRequiredAuthentication(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void handleStateUpdateWaitingAnswer(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void handleStatePrackWaitingAnswer(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void handleStateNegotiated(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);


  void handleInitialSDPOffer(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void handleInitialSDPAnswer(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void handleSDPOffer(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void handleSDPAnswer(
    const OSS::Net::IPAddress& sentBy,
    const OSS::Net::IPAddress& packetSourceIP,
    const OSS::Net::IPAddress& packetLocalInterface,
    const OSS::Net::IPAddress& route,
    const OSS::Net::IPAddress& routeLocalInterface,
    RequestType requestType,
    std::string& sdp,
    RTPProxy::Attributes& rtpAttribute);

  void setInactive();
private:
  void handleAuthStateTimeout(const boost::system::error_code& e);
    /// signals a timeout on leg-2 read operation
  State _state;
  std::string _identifier;
  std::string _logId;
  boost::filesystem::path _stateFile;
  bool _verbose;
  RTPProxyManager* _pManager;
  OSS::mutex_critic_sec _csStateMutex;

  mutable RTPProxyTuple _audio;
  mutable RTPProxyTuple _video;
  mutable RTPProxyTuple _fax;

  std::string _leg1Identifier;
  std::string _leg2Identifier;
  
  std::string _leg1OriginAddress;
  std::string _leg2OriginAddress;

  bool _isExpectingInitialAnswer;
  bool _hasOfferedAudioProxy;
  bool _hasOfferedVideoProxy;
  bool _hasOfferedFaxProxy;
  bool _isAudioProxyNegotiated;
  bool _isVideoProxyNegotiated;
  bool _isFaxProxyNegotiated;
  int _lastOfferIndex;
  std::string _lastSDPInAck;
  std::string _callId;
  std::string _from;
  std::string _to;
  std::string _lastOffer;
  std::string _lastAnswer;

  boost::asio::deadline_timer _authStateTimer;
  bool _isAuthTimeout;
  int _resizerSamplesLeg1;
  int _resizerSamplesLeg2;

  std::string _monitoredRoute;
};

//
// Inlines
//

inline const std::string& RTPProxySession::getIdentifier() const
{
  return _identifier;
}

inline RTPProxyManager*& RTPProxySession::manager()
{
  return _pManager;
}

inline RTPProxySession::State RTPProxySession::getState() const
{
  return _state;
}

inline bool RTPProxySession::isVoiceInactive() const
{
  return _audio.data().isInactive();
}

inline bool RTPProxySession::isVideoInactive() const
{
  return _video.data().isInactive();
}

inline bool RTPProxySession::isFaxInactive() const
{
  return _fax.data().isInactive();
}

inline void RTPProxySession::setInactive()
{
  _audio.data().setInactive();
  _video.data().setInactive();
  _fax.data().setInactive();
}

inline bool RTPProxySession::isAuthTimeout() const
{
  return _isAuthTimeout;
}

inline std::string& RTPProxySession::logId()
{
  return _logId;
}

inline bool& RTPProxySession::verbose()
{
  return _verbose;
}

inline std::string& RTPProxySession::from()
{
  return _from;
}

inline std::string& RTPProxySession::to()
{
  return _to;
}

inline std::string& RTPProxySession::callId()
{
  return _callId;
}

inline void RTPProxySession::setResizerSamples(int leg1, int leg2)
{
  _resizerSamplesLeg1 = leg1;
  _resizerSamplesLeg2 = leg2;
  _audio.setResizerSamples(_resizerSamplesLeg1, _resizerSamplesLeg2);
  //TODO: What about resizing video or fax?
}

inline int RTPProxySession::getResizerSamplesLeg1() const
{
  return _resizerSamplesLeg1;
}

inline int RTPProxySession::getResizerSamplesLeg2() const
{
  return _resizerSamplesLeg2;
}
    /// Enable resizing of RTP packets

inline void RTPProxySession::setMonitoredRoute(const std::string& route)
{
  _monitoredRoute = route;
}
  
inline const std::string& RTPProxySession::getMonitoredRoute() const
{
  return _monitoredRoute;
}

inline const std::string& RTPProxySession::getLastOffer() const
{
  return _lastOffer;
}

inline const std::string& RTPProxySession::getLastAnswer() const
{
  return _lastAnswer;
}


} } // OSS::RTP

#endif // ENABLE_FEATURE_RTP

#endif // RTP_RTPProxySession_INCLUDED

