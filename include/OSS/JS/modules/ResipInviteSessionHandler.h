
#ifndef RESIPINVITESESSIONHANDLER_H_INCLUDED
#define RESIPINVITESESSIONHANDLER_H_INCLUDED


#include <resip/dum/InviteSessionHandler.hxx>
#include <rutil/SharedPtr.hxx>

class ResipInviteSessionHandler : public resip::InviteSessionHandler
{
public:
   ResipInviteSessionHandler();
   virtual ~ResipInviteSessionHandler();
   void onNewSession(resip::ClientInviteSessionHandle, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg);
   void onNewSession(resip::ServerInviteSessionHandle, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg);
   void onFailure(resip::ClientInviteSessionHandle, const resip::SipMessage& msg);
   void onProvisional(resip::ClientInviteSessionHandle, const resip::SipMessage& msg);
   void onConnected(resip::ClientInviteSessionHandle, const resip::SipMessage& msg);
   void onStaleCallTimeout(resip::ClientInviteSessionHandle handle);
   void onConnected(resip::InviteSessionHandle, const resip::SipMessage& msg);
   void onRedirected(resip::ClientInviteSessionHandle, const resip::SipMessage& msg);
   void onTerminated(resip::InviteSessionHandle, resip::InviteSessionHandler::TerminatedReason reason, const resip::SipMessage* msg);
   void onAnswer(resip::InviteSessionHandle, const resip::SipMessage& msg, const resip::SdpContents& sdp);
   void onOffer(resip::InviteSessionHandle is, const resip::SipMessage& msg, const resip::SdpContents& sdp);
   void onEarlyMedia(resip::ClientInviteSessionHandle, const resip::SipMessage& msg, const resip::SdpContents& sdp);
   void onOfferRequired(resip::InviteSessionHandle, const resip::SipMessage& msg);
   void onOfferRejected(resip::InviteSessionHandle, const resip::SipMessage* msg);
   void onRefer(resip::InviteSessionHandle, resip::ServerSubscriptionHandle, const resip::SipMessage& msg);
   void onReferAccepted(resip::InviteSessionHandle, resip::ClientSubscriptionHandle, const resip::SipMessage& msg);
   void onReferRejected(resip::InviteSessionHandle, const resip::SipMessage& msg);
   void onReferNoSub(resip::InviteSessionHandle, const resip::SipMessage& msg);
   void onInfo(resip::InviteSessionHandle, const resip::SipMessage& msg);
   void onInfoSuccess(resip::InviteSessionHandle, const resip::SipMessage& msg);
   void onInfoFailure(resip::InviteSessionHandle, const resip::SipMessage& msg);
   void onMessage(resip::InviteSessionHandle, const resip::SipMessage& msg);
   void onMessageSuccess(resip::InviteSessionHandle, const resip::SipMessage& msg);
   void onMessageFailure(resip::InviteSessionHandle, const resip::SipMessage& msg);
   void onForkDestroyed(resip::ClientInviteSessionHandle);
};

#endif /* RESIPINVITESESSIONHANDLER_H_INCLUDED */

