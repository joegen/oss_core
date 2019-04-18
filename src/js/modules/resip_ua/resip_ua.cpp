#include "OSS/JS/JSPlugin.h"
#include "OSS/JS/JSIsolate.h"
#include "OSS/UTL/Logger.h"

#include <rutil/FdPoll.hxx>
#include <rutil/Logger.hxx>

#include "OSS/JS/modules/ResipSIPStack.h"
#include "OSS/JS/modules/ResipDialogUsageManager.h"
#include "OSS/JS/modules/ResipMasterProfile.h"
#include "OSS/JS/modules/ResipUserProfile.h"
#include "OSS/JS/modules/ResipClientSubscriptionHandler.h"
#include "OSS/JS/modules/ResipClientRegistrationHandler.h"
#include "resip/dum/ClientSubscription.hxx"


using OSS::JS::JSObjectWrap;
using namespace resip;

#define Log_Crit Log::Crit
#define Log_Err Log::Err
#define Log_Warning Log::Warning
#define Log_Info Log::Info
#define Log_Debug Log::Debug

/// GLOBAL EXPORTS ///

#define EXPORT_GLOBALS() \
  js_export_class(ResipSIPStack); \
  js_export_class(ResipDialogUsageManager); \
  js_export_class(ResipMasterProfile); \
  js_export_class(ResipUserProfile); \
  js_export_class(ResipClientSubscriptionHandler); \
  js_export_class(ResipClientRegistrationHandler); \
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
  CONST_EXPORT(UPDATE); \
  CONST_EXPORT(Log_Crit); \
  CONST_EXPORT(Log_Err); \
  CONST_EXPORT(Log_Warning); \
  CONST_EXPORT(Log_Info); \
  CONST_EXPORT(Log_Debug);

class ResipLogger : public ExternalLogger
{
public:
   virtual ~ResipLogger() {};
   /** return true to also do default logging, false to suppress default logging. */
   virtual bool operator()(Log::Level level,
      const Subsystem& subsystem, 
      const Data& appName,
      const char* file,
      int line,
      const Data& message,
      const Data& messageWithHeaders)
    {
      switch (level)
      {
      case Log::None:
        break;
      case Log::Crit:
        OSS_LOG_CRITICAL(message);
        break;
      case Log::Err:
        OSS_LOG_ERROR(message);
        break;
      case Log::Warning:
        OSS_LOG_WARNING(message);
        break;
      case Log::Info:
        OSS_LOG_INFO(message);
        break;
      case Log::Debug:
        OSS_LOG_DEBUG(message);
        break;
      default:
        OSS_LOG_TRACE(message);
      }
      return true;
    }
};

static ResipLogger _logger;

JS_METHOD_IMPL(set_log_level)
{
  js_method_arg_declare_int32(level, 0);
  Log::initialize(Log::OnlyExternalNoHeaders, (Log::Level)level, "oss_core", _logger);
  return JSUndefined();
}

JS_EXPORTS_INIT()
{
  Log::initialize(Log::OnlyExternalNoHeaders, Log::None, "oss_core", _logger);
  EXPORT_GLOBALS();
  js_export_method("setLogLevel", set_log_level);
  js_export_finalize();
}

JS_REGISTER_MODULE(JSRESIPUA);