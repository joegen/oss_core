#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"
#include "OSS/UTL/Thread.h"
#include <rutil/Logger.hxx>

#include "OSS/JS/modules/ResipClientInviteSessionHandler.h"

JS_CLASS_INTERFACE(ResipClientInviteSessionHandler, "ClientInviteSessionHandler")
{ 
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onNewSession", handleOnNewSession);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onFailure", handleOnFailure);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onProvisional", handleOnProvisional);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onConnected", handleOnConnected);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onStaleCallTimeout", handleOnStaleCallTimeout);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onRedirected", handleOnRedirected);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onTerminated", handleOnTerminated);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onAnswer", handleOnAnswer);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onOffer", handleOnOffer);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onEarlyMedia", handleOnEarlyMedia);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onOfferRequired", handleOnOfferRequired);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onOfferRejected", handleOnOfferRejected);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onRefer", handleOnRefer);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onReferAccepted", handleOnReferAccepted);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onReferRejected", handleOnReferRejected);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onReferNoSub", handleOnReferNoSub);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onInfo", handleOnInfo);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onInfoSuccess", handleOnInfoSuccess);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onInfoFailure", handleOnInfoFailure);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onMessage", handleOnMessage);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onMessageSuccess", handleOnMessageSuccess);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onMessageFailure", handleOnMessageFailure);
  JS_CLASS_METHOD_DEFINE(ResipClientInviteSessionHandler, "onForkDestroyed", handleOnForkDestroyed);
  JS_CLASS_INTERFACE_END(ResipClientInviteSessionHandler); 
}

JS_CONSTRUCTOR_IMPL(ResipClientInviteSessionHandler)
{
  return js_method_arg_self();
}

ResipClientInviteSessionHandler::ResipClientInviteSessionHandler()
{
}

ResipClientInviteSessionHandler::~ResipClientInviteSessionHandler()
{
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnNewSession)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnFailure)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnProvisional)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnConnected)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnStaleCallTimeout)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnRedirected)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnTerminated)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnAnswer)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnOffer)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnEarlyMedia)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnOfferRequired)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnOfferRejected)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnRefer)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnReferAccepted)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnReferRejected)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnReferNoSub)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnInfo)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnInfoSuccess)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnInfoFailure)
{
  return JSUndefined();
}
JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnMessage)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnMessageSuccess)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnMessageFailure)
{
  return JSUndefined();
}

JS_METHOD_IMPL(ResipClientInviteSessionHandler::handleOnForkDestroyed)
{
  return JSUndefined();
}

void ResipClientInviteSessionHandler::onNewSession(resip::ClientInviteSessionHandle, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onFailure(resip::ClientInviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onProvisional(resip::ClientInviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onConnected(resip::ClientInviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onStaleCallTimeout(resip::ClientInviteSessionHandle handle)
{
}

void ResipClientInviteSessionHandler::onRedirected(resip::ClientInviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onTerminated(resip::InviteSessionHandle, resip::InviteSessionHandler::TerminatedReason reason, const resip::SipMessage* msg)
{
}

void ResipClientInviteSessionHandler::onAnswer(resip::InviteSessionHandle, const resip::SipMessage& msg, const resip::SdpContents& sdp)
{
}

void ResipClientInviteSessionHandler::onOffer(resip::InviteSessionHandle is, const resip::SipMessage& msg, const resip::SdpContents& sdp)
{
}

void ResipClientInviteSessionHandler::onEarlyMedia(resip::ClientInviteSessionHandle, const resip::SipMessage& msg, const resip::SdpContents& sdp)
{
}

void ResipClientInviteSessionHandler::onOfferRequired(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onOfferRejected(resip::InviteSessionHandle, const resip::SipMessage* msg)
{
}

void ResipClientInviteSessionHandler::onRefer(resip::InviteSessionHandle, resip::ServerSubscriptionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onReferAccepted(resip::InviteSessionHandle, resip::ClientSubscriptionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onReferRejected(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onReferNoSub(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onInfo(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onInfoSuccess(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onInfoFailure(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onMessage(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onMessageSuccess(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onMessageFailure(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
}

void ResipClientInviteSessionHandler::onForkDestroyed(resip::ClientInviteSessionHandle)
{
}

void ResipClientInviteSessionHandler::onNewSessionIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onFailureIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onProvisionalIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onConnectedIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onStaleCallTimeoutIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onRedirectedIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onTerminatedIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onAnswerIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onOfferIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onEarlyMediaIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onOfferRequiredIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onOfferRejectedIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onReferIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onReferAcceptedIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onReferRejectedIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onReferNoSubIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onInfoIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onInfoSuccessIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onInfoFailureIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onMessageIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onMessageSuccessIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onMessageFailureIsolated(void* user_data)
{
}

void ResipClientInviteSessionHandler::onForkDestroyedIsolated(void* user_data)
{
}



