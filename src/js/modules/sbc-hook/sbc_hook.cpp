// OSS Software Solutions Application Programmer Interface
//
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

#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Net/IPAddress.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SIPStatusLine.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/SIP/SIPContact.h"
#include "v8.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SBC/SBCRegisterBehavior.h"
#include "OSS/SIP/SBC/SBCJSModuleManager.h"
#include "OSS/SIP/SBC/SBCDirectories.h"

#include "OSS/JS/JSModule.h"

namespace OSS {
namespace SIP {
namespace SBC {


typedef v8::Handle<v8::Value> jsval;
typedef v8::Arguments jsargs;
typedef v8::String jsstring;
typedef v8::String::Utf8Value jsstringutf8;
typedef v8::FunctionTemplate jsfunc;
#define jsvoid v8::Undefined
typedef v8::Boolean jsbool;
typedef v8::Integer jsint;
typedef v8::Number jsdouble;
typedef v8::HandleScope jsscope;
typedef v8::Handle<v8::External> jsfield;

static SBCManager* _pSBCManager = 0;

OSS::SIP::SIPMessage* unwrapRequest(const jsargs& args)
{
  if (args.Length() < 1)
    return 0;
  jsval obj = args[0];
  if (!obj->IsObject())
    return 0;
  jsfield field = jsfield::Cast(obj->ToObject()->GetInternalField(0));
  void* ptr = field->Value();
  return static_cast<OSS::SIP::SIPMessage*>(ptr);
}

std::string jsvalToString(const jsval& str)
{
  if (!str->IsString())
    return "";
  jsstringutf8 value(str);
  return *value;
}

bool jsvalToBoolean(const jsval& str)
{
  if (!str->IsBoolean())
    return false;
  return str->IsTrue();;
}

int jsvalToInt(const jsval& str)
{
  if (!str->IsNumber())
    return 0;
  return str->Int32Value();
}

static jsval msgRouteByAOR(const jsargs& args)
{
  if (!_pSBCManager)
  {
    return jsbool::New(false);
  }

  bool userComparisonOnly = true;
  if (args.Length() == 2)
  {
    userComparisonOnly = args[1]->IsTrue();
  }

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
  {
    return jsbool::New(false);
  }

  SIPB2BTransaction* pTrn = static_cast<SIPB2BTransaction*>(pMsg->userData());
  if (!pTrn)
  {
    return jsbool::New(false);
  }

  if (!_pSBCManager)
  {
    return jsbool::New(false);
  }
  OSS::Net::IPAddress localInterface;
  OSS::Net::IPAddress target;

  return jsbool::New(_pSBCManager->onRouteByAOR(pMsg, pTrn, userComparisonOnly, localInterface, target));
}

static jsval msgRouteByRURI(const jsargs& args)
{
  if (!_pSBCManager)
  {
    return jsbool::New(false);
  }

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
  {
    return jsbool::New(false);
  }

  SIPB2BTransaction* pTrn = static_cast<SIPB2BTransaction*>(pMsg->userData());
  if (!pTrn)
  {
    return jsbool::New(false);
  }

  if (!_pSBCManager)
  {
    return jsbool::New(false);
  }

  return jsbool::New(_pSBCManager->onRouteByRURI(pMsg, pTrn));
}


static jsval msgResetMaxForwards(const jsargs& args)
{
  if (args.Length() < 2)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  std::string maxForwards = jsvalToString(args[1]);
  if (maxForwards.empty())
    return jsbool::New(false);

  if (!_pSBCManager)
  {
    return jsbool::New(false);
  }
  //
  // First check if we are not spiraling by checking the via address
  //
  std::size_t count = pMsg->hdrGetSize("via");
  for (std::size_t i = 0; i < count; i++)
  {
    const std::string& via = pMsg->hdrGet("via", i);
    std::vector<std::string> elements;
    SIPVia::splitElements(via, elements);
    for (std::vector<std::string>::const_iterator iter = elements.begin(); iter != elements.end(); iter++)
    {
      SIPVia hVia(*iter);
      std::string sentBy = hVia.getSentBy();
      std::string transport = hVia.getTransport();
      OSS::string_to_lower(transport);
      OSS::Net::IPAddress address(OSS::Net::IPAddress::fromV4IPPort(sentBy.c_str()));

      if (_pSBCManager->isLocalTransport(transport, address))
        return jsbool::New(false);
    }
  }

  pMsg->hdrSet("Max-Forwards", maxForwards);

  return jsbool::New(true);
}

jsval sbc_jsexec_async(const jsargs& args)
{
  if (args.Length() < 2)
    return jsvoid();

  std::string bin = jsvalToString(args[0]);
  std::string commandArgs = jsvalToString(args[1]);

#if OSS_OS_FAMILY_WINDOWS
  return jsvoid();
#else
  std::ostringstream cmd;
  cmd << bin << " " << commandArgs;
  return jsint::New(system(cmd.str().c_str()));
#endif
}

jsval sbc_jsexec(const jsargs& args)
{
  if (args.Length() < 2)
    return jsvoid();

  std::string bin = jsvalToString(args[0]);
  std::string commandArgs = jsvalToString(args[1]);
  std::ostringstream cmd;
  cmd << bin << " " << commandArgs;
  std::string command = cmd.str();
#ifndef OSS_OS_FAMILY_WINDOWS
  FILE *fd = popen( command.c_str(), "r" );
  std::string result;
  result.reserve(1024);
  if (!fd)
      return jsvoid();
  while (true)
  {
    int c = fgetc(fd);
    if (c != EOF)
      result.push_back((char)c);
    else
      pclose(fd);
  }

  return jsstring::New(result.c_str());
#else
  return jsvoid();
#endif

}


jsval sbc_white_list_address(const jsargs& args)
{
  if (args.Length() < 1)
    return jsbool::New(false);

  std::string entry = jsvalToString(args[0]);

  boost::system::error_code ec;
  boost::asio::ip::address ip = boost::asio::ip::address::from_string(entry, ec);
  if (!ec)
  {
    SIPTransportSession::rateLimit().whiteListAddress(ip);
    return jsbool::New(true);
  }

  return jsbool::New(false);
}

jsval sbc_white_list_network(const jsargs& args)
{
  if (args.Length() < 1)
  {
    return jsvoid();
  }

  std::string network = jsvalToString(args[0]);
  if (!network.empty())
  {
    SIPTransportSession::rateLimit().whiteListNetwork(network);
  }
  
  return jsvoid();
}

jsval sbc_deny_all_incoming(const jsargs& args)
{
  if (args.Length() < 1)
    return jsbool::New(false);

  bool deny = jsvalToBoolean(args[0]);
  SIPTransportSession::rateLimit().denyAll(deny);
  return jsbool::New(true);
}

jsval sbc_set_transport_threshold(const jsargs& args)
{
  if (!_pSBCManager || args.Length() < 3)
  {
    return jsbool::New(false);
  }
  
  int packetsPerSecondThreshold = jsvalToInt(args[0]);
  int thresholdViolationRate = jsvalToInt(args[1]);
  int banLifeTime = jsvalToInt(args[2]);
  
  _pSBCManager->transactionManager().stack().setTransportThreshold(packetsPerSecondThreshold, thresholdViolationRate, banLifeTime);
  
  return jsbool::New(true);
}

jsval sbc_add_channel_limit(const jsargs& args)
{
  if (!_pSBCManager || args.Length() < 2)
  {
    return jsbool::New(false);
  }
  
  std::string prefix = jsvalToString(args[0]);
  unsigned int limit = jsvalToInt(args[1]);
  
  _pSBCManager->cdr().channelLimits().registerDialPrefix(prefix, limit);
  
  return jsbool::New(true);
}

jsval sbc_add_domain_channel_limit(const jsargs& args)
{
  if (!_pSBCManager || args.Length() < 2)
  {
    return jsbool::New(false);
  }
  
  std::string domain = jsvalToString(args[0]);
  unsigned int limit = jsvalToInt(args[1]);
  
  _pSBCManager->cdr().domainLimits().registerDomain(domain, limit);
  
  return jsbool::New(true);
}

jsval sbc_get_channel_count(const jsargs& args)
{
  if (!_pSBCManager || args.Length() < 1)
  {
    return jsint::New(0);
  }
  
  std::string prefix = jsvalToString(args[0]);
  return jsint::New(_pSBCManager->cdr().channelLimits().getCallCount(prefix));
}

jsval sbc_get_domain_channel_count(const jsargs& args)
{
  if (!_pSBCManager || args.Length() < 1)
  {
    return jsint::New(0);
  }
  
  std::string domain = jsvalToString(args[0]);
  return jsint::New(_pSBCManager->cdr().domainLimits().getCallCount(domain));
}

jsval sbc_set_log_level(const jsargs& args)
{
  if (args.Length() < 1)
  {
    return jsbool::New(false);
  }
  
  std::string sLogLevel = jsvalToString(args[0]);;
  if (sLogLevel.empty())
  {
    return jsbool::New(false);
  }

  if (sLogLevel == "fatal")//  A fatal error. The application will most likely terminate. This is the highest priority.
  {
    OSS::log_reset_level(OSS::PRIO_FATAL);
  }
  else if (sLogLevel == "critical")//  A critical error. The application might not be able to continue running successfully.
  {
    OSS::log_reset_level(OSS::PRIO_CRITICAL);
  }
  else if (sLogLevel == "error")//  An error. An operation did not complete successfully, but the application as a whole is not affected.
  {
    OSS::log_reset_level(OSS::PRIO_ERROR);
  }
  else if (sLogLevel == "warning")//  A warning. An operation completed with an unexpected result.
  {
    OSS::log_reset_level(OSS::PRIO_WARNING);
  }
  else if (sLogLevel == "notice")//  A notice, which is an information with just a higher priority.
  {
    OSS::log_reset_level(OSS::PRIO_NOTICE);
  }
  else if (sLogLevel == "information")//  An informational message, usually denoting the successful completion of an operation.
  {
    OSS::log_reset_level(OSS::PRIO_INFORMATION);
  }
  else if (sLogLevel == "debug")//  A debugging message.
  {
    OSS::log_reset_level(OSS::PRIO_DEBUG);
  }
  else if (sLogLevel == "trace")//  A tracing message. This is the lowest priority.
  {
    OSS::log_reset_level(OSS::PRIO_TRACE);
  }
  else
  {
    return jsbool::New(false);
  }

  return jsbool::New(true);
}


jsval sbc_ban_user_id(const jsargs& args)
{
  if (!_pSBCManager || args.Length() < 1)
  {
    return jsvoid();
  }
  std::string val = jsvalToString(args[0]);
  if (!val.empty())
  {
    _pSBCManager->autoBanRules().banUserById(val);
  }
  
  return jsvoid();
}

jsval sbc_ban_user_display(const jsargs& args)
{
  if (!_pSBCManager || args.Length() < 1)
  {
    return jsvoid();
  }
  std::string val = jsvalToString(args[0]);
  if (!val.empty())
  {
    _pSBCManager->autoBanRules().banUserByDisplayName(val);
  }
  
  return jsvoid();
}

jsval sbc_ban_user_agent(const jsargs& args)
{
  if (!_pSBCManager || args.Length() < 1)
  {
    return jsvoid();
  }
  std::string val = jsvalToString(args[0]);
  if (!val.empty())
  {
    _pSBCManager->autoBanRules().banUserAgent(val);
  }
  
  return jsvoid();
}

jsval sbc_initialize(const jsargs& args)
{
  if (!_pSBCManager || args.Length() < 1)
  {
    return jsbool::New(false);
  }
  std::string val = jsvalToString(args[0]);
  boost::filesystem::path configDir(val.c_str());
  
  if (!boost::filesystem::exists(configDir))
  {
    return jsbool::New(false);
  }
  SBCDirectories::instance()->setConfigDirectory(val);
  
  return jsbool::New(_pSBCManager->initialize());
}

jsval sbc_run(const jsargs& args)
{
  return jsbool::New(_pSBCManager->run());
}

jsval sbc_start_options_keep_alive(const jsargs& args)
{
  if (!_pSBCManager || args.Length() < 1)
  {
    return jsbool::New(false);
  }
  bool start = jsvalToBoolean(args[0]);
  _pSBCManager->registerHandler()->pauseKeepAlive(!start);
  return jsbool::New(true);
}

} } } // OSS::SIP::SBC

JS_EXPORTS_INIT()
{
  using namespace OSS::SIP::SBC;
  
  _pSBCManager = OSS::SIP::SBC::SBCManager::instance();
  
  js_export_method("msgRouteByAOR", msgRouteByAOR);
  js_export_method("msgRouteByRURI", msgRouteByRURI);
  js_export_method("msgResetMaxForwards", msgResetMaxForwards);
  
  js_export_method("sbc_initialize", sbc_initialize);
  js_export_method("sbc_run", sbc_run);
  js_export_method("sbc_jsexec", sbc_jsexec);
  js_export_method("sbc_jsexec_async", sbc_jsexec_async);
  js_export_method("sbc_white_list_address", sbc_white_list_address);
  js_export_method("sbc_white_list_network", sbc_white_list_network);
  js_export_method("sbc_deny_all_incoming", sbc_deny_all_incoming);
  js_export_method("sbc_set_transport_threshold", sbc_set_transport_threshold);
  js_export_method("sbc_add_channel_limit", sbc_add_channel_limit);
  js_export_method("sbc_get_channel_count", sbc_get_channel_count);
  js_export_method("sbc_add_domain_channel_limit", sbc_add_domain_channel_limit);
  js_export_method("sbc_get_domain_channel_count", sbc_get_domain_channel_count);
  js_export_method("sbc_set_log_level", sbc_set_log_level);
  js_export_method("sbc_start_options_keep_alive", sbc_start_options_keep_alive);
  
  //
  // Banning known attackers
  //
  js_export_method("sbc_ban_user_id", sbc_ban_user_id);
  js_export_method("sbc_ban_user_display", sbc_ban_user_display);
  js_export_method("sbc_ban_user_agent", sbc_ban_user_agent);
  
  js_export_finalize(); 
}

JS_REGISTER_MODULE(JSSBCHook);



