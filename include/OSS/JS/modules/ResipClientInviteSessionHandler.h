#ifndef RESIPCLIENTINVITESESSIONHANDLER_H_INCLUDED
#define RESIPCLIENTINVITESESSIONHANDLER_H_INCLUDED


#include <v8.h>
#include <vector>
#include "OSS/JS/JSPlugin.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/JSEventArgument.h"

#include "OSS/JS/modules/ResipInviteSessionHandler.h"

class ResipClientInviteSessionHandler : public OSS::JS::JSObjectWrap
{
public:
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(handleOnNewSession);
  JS_METHOD_DECLARE(handleOnFailure);
  JS_METHOD_DECLARE(handleOnProvisional);
  JS_METHOD_DECLARE(handleOnConnected);
  JS_METHOD_DECLARE(handleOnStaleCallTimeout);
  JS_METHOD_DECLARE(handleOnRedirected);
  JS_METHOD_DECLARE(handleOnTerminated);
  JS_METHOD_DECLARE(handleOnAnswer);
  JS_METHOD_DECLARE(handleOnOffer);
  JS_METHOD_DECLARE(handleOnEarlyMedia);
  JS_METHOD_DECLARE(handleOnOfferRequired);
  JS_METHOD_DECLARE(handleOnOfferRejected);
  JS_METHOD_DECLARE(handleOnRefer);
  JS_METHOD_DECLARE(handleOnReferAccepted);
  JS_METHOD_DECLARE(handleOnReferRejected);
  JS_METHOD_DECLARE(handleOnReferNoSub);
  JS_METHOD_DECLARE(handleOnInfo);
  JS_METHOD_DECLARE(handleOnInfoSuccess);
  JS_METHOD_DECLARE(handleOnInfoFailure);
  JS_METHOD_DECLARE(handleOnMessage);
  JS_METHOD_DECLARE(handleOnMessageSuccess);
  JS_METHOD_DECLARE(handleOnMessageFailure);
  JS_METHOD_DECLARE(handleOnForkDestroyed);

  void onNewSession(resip::ClientInviteSessionHandle, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg);
  void onFailure(resip::ClientInviteSessionHandle, const resip::SipMessage& msg);
  void onProvisional(resip::ClientInviteSessionHandle, const resip::SipMessage& msg);
  void onConnected(resip::ClientInviteSessionHandle, const resip::SipMessage& msg);
  void onStaleCallTimeout(resip::ClientInviteSessionHandle handle);
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

protected:
  void onNewSessionIsolated(void* user_data);
  void onFailureIsolated(void* user_data);
  void onProvisionalIsolated(void* user_data);
  void onConnectedIsolated(void* user_data);
  void onStaleCallTimeoutIsolated(void* user_data);
  void onRedirectedIsolated(void* user_data);
  void onTerminatedIsolated(void* user_data);
  void onAnswerIsolated(void* user_data);
  void onOfferIsolated(void* user_data);
  void onEarlyMediaIsolated(void* user_data);
  void onOfferRequiredIsolated(void* user_data);
  void onOfferRejectedIsolated(void* user_data);
  void onReferIsolated(void* user_data);
  void onReferAcceptedIsolated(void* user_data);
  void onReferRejectedIsolated(void* user_data);
  void onReferNoSubIsolated(void* user_data);
  void onInfoIsolated(void* user_data);
  void onInfoSuccessIsolated(void* user_data);
  void onInfoFailureIsolated(void* user_data);
  void onMessageIsolated(void* user_data);
  void onMessageSuccessIsolated(void* user_data);
  void onMessageFailureIsolated(void* user_data);
  void onForkDestroyedIsolated(void* user_data);

  private:
    ResipClientInviteSessionHandler();
    ~ResipClientInviteSessionHandler();
};
#endif /* RESIPCLIENTINVITESESSIONHANDLER_H */

