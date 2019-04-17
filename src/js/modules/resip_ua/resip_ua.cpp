#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"

#include <rutil/FdPoll.hxx>
#include <rutil/Logger.hxx>

#include "OSS/JS/modules/ResipSIPStack.h"
#include "OSS/JS/modules/ResipDialogUsageManager.h"
#include "OSS/JS/modules/ResipMasterProfile.h"
#include "OSS/JS/modules/ResipUserProfile.h"
#include "OSS/JS/modules/ResipClientSubscriptionHandler.h"
#include "resip/dum/ClientSubscription.hxx"


using OSS::JS::JSObjectWrap;
using namespace resip;

/// GLOBAL EXPORTS ///

#define EXPORT_GLOBALS() \
  js_export_class(ResipSIPStack); \
  js_export_class(ResipDialogUsageManager); \
  js_export_class(ResipMasterProfile); \
  js_export_class(ResipUserProfile); \
  js_export_class(ResipClientSubscriptionHandler); \
  CONST_EXPORT(UNKNOWN_TRANSPORT); \
  CONST_EXPORT(TLS); \
  CONST_EXPORT(TCP); \
  CONST_EXPORT(UDP); \
  CONST_EXPORT(SCTP); \
  CONST_EXPORT(DCCP); \
  CONST_EXPORT(DTLS); \
  CONST_EXPORT(WS); \
  CONST_EXPORT(WSS); \
  CONST_EXPORT(MAX_TRANSPORT); \
  CONST_EXPORT(UNKNOWN); \
  CONST_EXPORT(ACK); \
  CONST_EXPORT(BYE); \
  CONST_EXPORT(CANCEL); \
  CONST_EXPORT(INVITE); \
  CONST_EXPORT(NOTIFY); \
  CONST_EXPORT(OPTIONS); \
  CONST_EXPORT(REFER); \
  CONST_EXPORT(REGISTER); \
  CONST_EXPORT(SUBSCRIBE); \
  CONST_EXPORT(RESPONSE); \
  CONST_EXPORT(MESSAGE); \
  CONST_EXPORT(INFO); \
  CONST_EXPORT(PRACK); \
  CONST_EXPORT(PUBLISH); \
  CONST_EXPORT(SERVICE); \
  CONST_EXPORT(UPDATE);


JS_EXPORTS_INIT()
{
  EXPORT_GLOBALS();
  Log::initialize(Log::Cout, Log::Debug, "oss_core");
  js_export_finalize();
}

JS_REGISTER_MODULE(JSRESIPUA);