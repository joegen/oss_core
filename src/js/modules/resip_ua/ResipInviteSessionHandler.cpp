#include "OSS/JS/modules/ResipInviteSessionHandler.h"


ResipInviteSessionHandler::ResipInviteSessionHandler()
{
}

ResipInviteSessionHandler::~ResipInviteSessionHandler()
{
}

void ResipInviteSessionHandler::onNewSession(resip::ClientInviteSessionHandle, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onNewSession(resip::ServerInviteSessionHandle, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onFailure(resip::ClientInviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onProvisional(resip::ClientInviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onConnected(resip::ClientInviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onStaleCallTimeout(resip::ClientInviteSessionHandle handle)
{
}

void ResipInviteSessionHandler::onConnected(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onRedirected(resip::ClientInviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onTerminated(resip::InviteSessionHandle, resip::InviteSessionHandler::TerminatedReason reason, const resip::SipMessage* msg)
{
}

void ResipInviteSessionHandler::onAnswer(resip::InviteSessionHandle, const resip::SipMessage& msg, const resip::SdpContents& sdp)
{
}

void ResipInviteSessionHandler::onOffer(resip::InviteSessionHandle is, const resip::SipMessage& msg, const resip::SdpContents& sdp)      
{
}

void ResipInviteSessionHandler::onEarlyMedia(resip::ClientInviteSessionHandle, const resip::SipMessage& msg, const resip::SdpContents& sdp)
{
}

void ResipInviteSessionHandler::onOfferRequired(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onOfferRejected(resip::InviteSessionHandle, const resip::SipMessage* msg)
{
}

void ResipInviteSessionHandler::onRefer(resip::InviteSessionHandle, resip::ServerSubscriptionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onReferAccepted(resip::InviteSessionHandle, resip::ClientSubscriptionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onReferRejected(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onReferNoSub(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onInfo(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onInfoSuccess(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onInfoFailure(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onMessage(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onMessageSuccess(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onMessageFailure(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipInviteSessionHandler::onForkDestroyed(resip::ClientInviteSessionHandle)
{
}



