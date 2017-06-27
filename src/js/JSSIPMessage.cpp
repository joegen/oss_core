// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "GNU Lesser General Public License (LGPL)".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/JS/JSSIPMessage.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/Net.h"
#include "OSS/UTL/Logger.h"
#include "v8.h"

#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SIPStatusLine.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/SIP/SIPContact.h"

namespace OSS {
namespace JS {

using namespace OSS::SIP;

typedef v8::Handle<v8::Value> jsval;
typedef v8::Arguments jsargs;
typedef v8::String jsstring;
typedef v8::String::Utf8Value jsstringutf8;
typedef v8::FunctionTemplate jsfunc;
#define jsvoid v8::Undefined
typedef v8::Boolean jsbool;
typedef v8::Integer jsint;
typedef v8::HandleScope jsscope;
typedef v8::Handle<v8::External> jsfield;

static OSS::SIP::SIPMessage* unwrapRequest(const jsargs& args)
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

static std::string jsvalToString(const jsval& str)
{
  if (!str->IsString())
    return "";
  jsstringutf8 value(str);
  return *value;
}

static /*size_t*/ jsval msgHdrPresent(const jsargs& args/*const char* headerName*/)
{
  if (args.Length() != 2)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string headerName = jsvalToString(args[1]);
  if (headerName.empty())
    return jsvoid();

  try
  {
    return jsbool::New(pMsg->hdrPresent(headerName.c_str()));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrPresent - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static  /*size_t*/ jsval msgHdrGetSize(const jsargs& args/*const char* headerName*/)
{
  if (args.Length() != 2)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string headerName = jsvalToString(args[1]);
  if (headerName.empty())
    return jsvoid();
  
  try
  {
    return jsint::New(pMsg->hdrGetSize(headerName.c_str()));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrGetSize - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*const std::string&*/ jsval msgHdrGet(const jsargs& args/*const char* headerName, size_t index = 0*/)
{
  if (args.Length() < 2)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string headerName = jsvalToString(args[1]);
  if (headerName.empty())
    return jsvoid();
  try
  {
    if (args.Length() > 2)
    {
      size_t index = args[2]->NumberValue();
      return jsstring::New(pMsg->hdrGet(headerName.c_str(), index).c_str());
    }

    return jsstring::New(pMsg->hdrGet(headerName.c_str()).c_str());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrGet - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*bool*/ jsval msgHdrSet(const jsargs& args/*const char* headerName, const std::string& headerValue, size_t index*/)
{
  if (args.Length() < 3)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  std::string headerName = jsvalToString(args[1]);
  if (headerName.empty())
    return jsbool::New(false);

  std::string headerValue = jsvalToString(args[2]);
  if (headerValue.empty())
    return jsbool::New(false);

  size_t index = 0;
  if (args.Length() > 3)
    index = args[3]->NumberValue();
  
  try
  {
    return jsbool::New(pMsg->hdrSet(headerName.c_str(), headerValue, index));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrSet - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*bool*/ jsval msgHdrRemove(const jsargs& args/*const char* headerName*/)
{
  if (args.Length() < 2)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  std::string headerName = jsvalToString(args[1]);
  if (headerName.empty())
    return jsbool::New(false);
  
  try
  {
    return jsbool::New(pMsg->hdrRemove(headerName.c_str()));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrRemove - " << e.message();
    OSS::log_error(msg.str());
    return jsbool::New(false);
  }
}

static /*bool*/ jsval msgHdrListAppend(const jsargs& args/*const char* name, const std::string& value*/)
{
  if (args.Length() < 3)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  std::string headerName = jsvalToString(args[1]);
  if (headerName.empty())
    return jsbool::New(false);

  std::string headerValue = jsvalToString(args[2]);
  if (headerValue.empty())
    return jsbool::New(false);

  try
  {
    return jsbool::New(pMsg->hdrListAppend(headerName.c_str(), headerValue));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrListAppend - " << e.message();
    OSS::log_error(msg.str());
    return jsbool::New(false);
  }
}

static /*bool*/ jsval msgHdrListPrepend(const jsargs& args/*const char* name, const std::string& value*/)
{
  if (args.Length() < 3)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  std::string headerName = jsvalToString(args[1]);
  if (headerName.empty())
    return jsbool::New(false);

  std::string headerValue = jsvalToString(args[2]);
  if (headerValue.empty())
    return jsbool::New(false);

  try
  {
    return jsbool::New(pMsg->hdrListPrepend(headerName.c_str(), headerValue));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrListPrepend - " << e.message();
    OSS::log_error(msg.str());
    return jsbool::New(false);
  }
}

static /*std::string*/ jsval msgHdrListPopFront(const jsargs& args/*const char* name*/)
{
  if (args.Length() < 2)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string headerName = jsvalToString(args[1]);
  if (headerName.empty())
    return jsvoid();

  try
  {
    return jsstring::New(pMsg->hdrListPopFront(headerName.c_str()).c_str());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrListPopFront - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*bool*/ jsval msgHdrListRemove(const jsargs& args/*const char* name*/)
{
  if (args.Length() < 2)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  std::string headerName = jsvalToString(args[1]);
  if (headerName.empty())
    return jsbool::New(false);

  try
  {
    return jsbool::New(pMsg->hdrListRemove(headerName.c_str()));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrListRemove - " << e.message();
    OSS::log_error(msg.str());
    return jsbool::New(false);
  }
}

static  /*boost::tribool*/ jsval msgIsRequest(const jsargs& args/*const char* method = 0*/)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string method;
  if (args.Length() >= 2)
    method = jsvalToString(args[1]);

  try
  {
    if (method.empty())
      return jsbool::New(pMsg->isRequest());
    else
      return jsbool::New(pMsg->isRequest(method.c_str()));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIsRequest - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}


static /*boost::tribool*/ jsval msgIsResponse(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  try
  {
    return jsbool::New(pMsg->isResponse());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIsResponse - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*boost::tribool*/ jsval msgIs1xx(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  try
  {
    return jsbool::New(pMsg->is1xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs1xx - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*boost::tribool*/ jsval msgIs2xx(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  try
  {
    return jsbool::New(pMsg->is2xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs2xx - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*boost::tribool*/ jsval msgIs3xx(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  try
  {
    return jsbool::New(pMsg->is3xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs3xx - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*boost::tribool*/ jsval msgIs4xx(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  try
  {
    return jsbool::New(pMsg->is4xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs4xx - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*boost::tribool*/ jsval msgIs5xx(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  try
  {
    return jsbool::New(pMsg->is5xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs5xx - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*boost::tribool*/ jsval msgIs6xx(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  try
  {
    return jsbool::New(pMsg->is6xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs6xx - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*boost::tribool*/ jsval msgIsResponseFamily(const jsargs& args/*int responseCode*/)
{
  if (args.Length() < 2)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  int responseCode = args[1]->NumberValue();

  try
  {
    return jsbool::New(pMsg->isResponseFamily(responseCode));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIsResponseFamily - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
  
}

static /*boost::tribool*/ jsval msgIsErrorResponse(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  try
  {
    return jsbool::New(pMsg->isErrorResponse());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIsErrorResponse - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*boost::tribool*/ jsval msgIsMidDialog(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  try
  {
    return jsbool::New(pMsg->isMidDialog());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrGet - " << e.message();
    OSS::log_error(msg.str());
    return jsvoid();
  }
}

static /*std::string&*/ jsval msgGetBody(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  return jsstring::New(pMsg->getBody().c_str());
}

static /*bool*/ jsval msgSetBody(const jsargs& args/*const std::string& body*/)
{
  if (args.Length() < 2)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  std::string body = jsvalToString(args[1]);
  pMsg->setBody(body);

  return jsbool::New(true);
}

static /*std::string&*/ jsval msgGetStartLine(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  return jsstring::New(pMsg->getStartLine().c_str());
}

static /*std::string&*/ jsval msgSetStartLine(const jsargs& args/*const std::string& sline*/)
{
  if (args.Length() < 2)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string sline = jsvalToString(args[1]);
  pMsg->setStartLine(sline);

  return jsvoid();
}

static jsval msgGetData(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();
  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();
  return jsstring::New(pMsg->data().c_str());
}

static jsval msgCommitData(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();
  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();
  pMsg->commitData();
  return jsstring::New(pMsg->data().c_str());
}

//
// Request-Line Processing
//

static /*bool*/ jsval requestLineGetMethod(const jsargs& args/*const std::string& rline, std::string& method*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string result;
  if (OSS::SIP::SIPRequestLine::getMethod(input, result))
    return jsstring::New(result.c_str());
  return jsvoid();
}


static /*bool*/ jsval requestLineGetURI(const jsargs& args/*const std::string& rline, std::string& uri*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string result;
  if (OSS::SIP::SIPRequestLine::getURI(input, result))
    return jsstring::New(result.c_str());
  return jsvoid();
}

static /*bool*/ jsval requestLineGetVersion(const jsargs& args/*const std::string& rline, std::string& version*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string result;
  if (OSS::SIP::SIPRequestLine::getVersion(input, result))
    return jsstring::New(result.c_str());
  return jsvoid();
}

static /*bool*/ jsval requestLineSetMethod(const jsargs& args/*std::string& rline, const char* method*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  return jsbool::New(OSS::SIP::SIPRequestLine::setMethod(input, newval.c_str()));
}

static /*bool*/ jsval requestLineSetURI(const jsargs& args/*std::string& rline, const char* uri*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  if (!OSS::SIP::SIPRequestLine::setURI(input, newval.c_str()))
    return jsvoid();
  return jsstring::New(input.c_str());
}


static /*bool*/ jsval requestLineSetVersion(const jsargs& args/*std::string& rline, const char* version*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  return jsbool::New(OSS::SIP::SIPRequestLine::setVersion(input, newval.c_str()));
}


//
// Status-Line Processing
//

static jsval/*bool*/ statusLineGetVersion(const jsargs& args/*const std::string& sline, std::string& version*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string result;
  if (OSS::SIP::SIPStatusLine::getVersion(input, result))
    return jsstring::New(result.c_str());
  return jsvoid();
}

static jsval/*bool*/ statusLineSetVersion(const jsargs& args/*std::string& sline, const char* version*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  return jsbool::New(OSS::SIP::SIPStatusLine::setVersion(input, newval.c_str()));
}


static jsval/*bool*/ statusLineGetStatusCode(const jsargs& args/*const std::string& sline,std::string& statusCode*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string result;
  if (OSS::SIP::SIPStatusLine::getStatusCode(input, result))
    return jsstring::New(result.c_str());
  return jsvoid();
}


static jsval/*bool*/ statusLineSetStatusCode(const jsargs& args/*std::string& sline, const char* statusCode*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  return jsbool::New(OSS::SIP::SIPStatusLine::setStatusCode(input, newval.c_str()));
}


static jsval/*bool*/ statusLineGetReasonPhrase(const jsargs& args/*const std::string& sline, std::string& reasonPhrase*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string result;
  if (OSS::SIP::SIPStatusLine::getReasonPhrase(input, result))
    return jsstring::New(result.c_str());
  return jsvoid();
}


static jsval/*bool*/ statusLineSetReasonPhrase(const jsargs& args/*std::string& sline, const char* reasonPhrase*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  return jsbool::New(OSS::SIP::SIPStatusLine::setReasonPhrase(input, newval.c_str()));
}

//
// URI Processing
//

static jsval/*bool*/ uriSetScheme(const jsargs& args/*std::string& uri, const char* scheme*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  if (OSS::SIP::SIPURI::setScheme(input, newval.c_str()))
    return jsstring::New(input.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriGetScheme(const jsargs& args/*const std::string& uri, std::string& value*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getScheme(input, val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriGetUser(const jsargs& args/*const std::string& uri, std::string& value*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getUser(input, val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriSetUserInfo(const jsargs& args/*std::string& uri, const char* userInfo*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  if (OSS::SIP::SIPURI::setUserInfo(input, newval.c_str()))
    return jsstring::New(input.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriGetPassword(const jsargs& args/*const std::string& uri, std::string& value*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getPassword(input, val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriGetHostPort(const jsargs& args/*const std::string& uri, std::string& value*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getHostPort(input, val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriSetHostPort(const jsargs& args/*std::string& uri, const char* hostPort*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  if (OSS::SIP::SIPURI::setHostPort(input, newval.c_str()))
    return jsstring::New(input.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriGetParams(const jsargs& args/*const std::string& uri, std::string& params*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getParams(input, val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriSetParams(const jsargs& args/*std::string& uri, const std::string& params*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  if (OSS::SIP::SIPURI::setParams(input, newval.c_str()))
    return jsstring::New(input.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriHasParam(const jsargs& args/*const std::string& uri, const char* paraName*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string val = jsvalToString(args[1]);
  return jsbool::New(OSS::SIP::SIPURI::hasParam(input, val.c_str()));
}

static jsval/*bool*/ uriGetParam(const jsargs& args/*const std::string& uri, const char* paramName, std::string& paramValue*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string name = jsvalToString(args[1]);
  std::string value;
  if (OSS::SIP::SIPURI::getParam(input, name.c_str(), value))
    return jsstring::New(value.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriGetParamEx(const jsargs& args/*const std::string& params, const char* paramName, std::string& paramValue*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string name = jsvalToString(args[1]);
  std::string value;
  if (OSS::SIP::SIPURI::getParamEx(input, name.c_str(), value))
    return jsstring::New(value.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriSetParam(const jsargs& args/*std::string& uri, const char* paramName, const char* paramValue*/)
{
  if (args.Length() < 3)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string name = jsvalToString(args[1]);
  std::string newval = jsvalToString(args[2]);
  return jsbool::New(OSS::SIP::SIPURI::setParam(input, name.c_str(), newval.c_str()));
}

static jsval/*bool*/ uriSetParamEx(const jsargs& args/*std::string& params, const char* paramName, const char* paramValue*/)
{
  if (args.Length() < 3)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string name = jsvalToString(args[1]);
  std::string newval = jsvalToString(args[2]);
  return jsbool::New(OSS::SIP::SIPURI::setParam(input, name.c_str(), newval.c_str()));
}

static jsval/*void*/ uriEscapeUser(const jsargs& args/*std::string& result, const char* user*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string result;
  OSS::SIP::SIPURI::escapeUser(result, input.c_str());
    return jsstring::New(result.c_str());
}

static jsval/*void*/ uriEscapeParam(const jsargs& args/*std::string& result, const char* param*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string result;
  OSS::SIP::SIPURI::escapeParam(result, input.c_str());
  return jsstring::New(result.c_str());
}

static jsval/*bool*/ uriGetHeaders(const jsargs& args/*const std::string& uri, std::string& headers*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getHeaders(input, val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriSetHeaders(const jsargs& args/*std::string& uri, const std::string& headers*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  if (OSS::SIP::SIPURI::setHeaders(input, newval.c_str()))
    return jsstring::New(input.c_str());
  return jsvoid();
}

static jsval/*bool*/ uriVerify(const jsargs& args/*const char* uri*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  return jsbool::New(OSS::SIP::SIPURI::verify(input.c_str()));
}

//
// From Processing
//

static jsval /*bool*/ fromGetDisplayName(const jsargs& args/*const std::string& from, std::string& displayName*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string val;
  if (OSS::SIP::SIPFrom::getDisplayName(input, val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval /*bool*/ fromSetDisplayName(const jsargs& args/*std::string& from, const char* uri*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  if (OSS::SIP::SIPFrom::setDisplayName(input, newval.c_str()))
    return jsstring::New(input.c_str());
  return jsvoid();
}

static jsval /*bool*/ fromGetURI(const jsargs& args/*const std::string& from, std::string& uri*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string val;
  if (OSS::SIP::SIPFrom::getURI(input, val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval /*bool*/ fromSetURI(const jsargs& args/*std::string& from, const char* uri*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  if (OSS::SIP::SIPFrom::setURI(input, newval.c_str()))
    return jsstring::New(input.c_str());
  return jsvoid();
}

static jsval /*bool*/ fromGetHeaderParams(const jsargs& args/*const std::string& from, std::string& headerParams*/)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string val;
  if (OSS::SIP::SIPFrom::getHeaderParams(input, val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval /*bool*/ fromSetHeaderParams(const jsargs& args/*std::string& from, const char* headerParams*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string newval = jsvalToString(args[1]);
  if (OSS::SIP::SIPFrom::setHeaderParams(input, newval.c_str()))
    return jsstring::New(input.c_str());
  return jsvoid();
}

static jsval /*bool*/ fromGetHeaderParam(const jsargs& args/*const std::string& from, const char* paramName, std::string& paramValue*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string name = jsvalToString(args[1]);
  std::string val;
  if (OSS::SIP::SIPFrom::getHeaderParam(input, name.c_str(), val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval /*bool*/ fromGetHeaderParamEx(const jsargs& args/*const std::string& headerParams, const char* paramName, std::string& paramValue*/)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string name = jsvalToString(args[1]);
  std::string val;
  if (OSS::SIP::SIPFrom::getHeaderParamEx(input, name.c_str(), val))
    return jsstring::New(val.c_str());
  return jsvoid();
}

static jsval /*bool*/ fromSetHeaderParam(const jsargs& args/*std::string& from, const char* paramName, const char* paramValue*/)
{
  if (args.Length() < 3)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string name = jsvalToString(args[1]);
  std::string newval = jsvalToString(args[2]);
  if (OSS::SIP::SIPFrom::setHeaderParam(input, name.c_str(), newval.c_str()))
    return jsstring::New(input.c_str());
  return jsvoid();
}

static jsval /*bool*/ fromSetHeaderParamEx(const jsargs& args/*std::string& headerParams, const char* paramName, const char* paramValue*/)
{
  if (args.Length() < 3)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  std::string name = jsvalToString(args[1]);
  std::string newval = jsvalToString(args[2]);
  if (OSS::SIP::SIPFrom::setHeaderParamEx(input, name.c_str(), newval.c_str()))
    return jsstring::New(input.c_str());
  return jsvoid();
}

//
// Misc functions
//

static jsval cidrVerify(const jsargs& args)
{
  if (args.Length() < 2)
    return jsvoid();
  std::string ip = jsvalToString(args[0]);
  std::string cidr = jsvalToString(args[1]);
  if (ip.empty() || cidr.empty())
    return jsvoid();
  bool verified = OSS::socket_address_cidr_verify(ip, cidr);
  return jsbool::New(verified);
}

static jsval wildCardCompare(const jsargs& args)
{
  if (args.Length() < 2)
    return jsvoid();

  std::string wild = jsvalToString(args[0]);
  std::string input = jsvalToString(args[1]);

  return jsbool::New(OSS::string_wildcard_compare(wild.c_str(), input));
}

static jsval isIpInRange(const jsargs& args)
{
  if (args.Length() < 3)
    return jsvoid();
  std::string low = jsvalToString(args[0]);
  std::string high = jsvalToString(args[1]);
  std::string strIp = jsvalToString(args[2]);

  if (low.empty() || high.empty() || strIp.empty())
    return jsvoid();
  
  bool verified = OSS::socket_address_range_verify(low, high, strIp);
  
  return jsbool::New(verified);
}

static jsval md5Hash(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();
  std::string input = jsvalToString(args[0]);
  return jsstring::New(OSS::string_md5_hash(input.c_str()).c_str());
}

static jsval msgGetRequestUri(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string requestURI;
  if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
  {
    return jsstring::New(requestURI.c_str());
  }
  return jsvoid();
}


static jsval msgSetRequestUri(const jsargs& args)
{
  if (args.Length() < 2)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  std::string uri = jsvalToString(args[1]);

  try
  {
    return jsbool::New(SIPRequestLine::setURI(pMsg->startLine(), uri.c_str()));
  }
  catch(...)
  {
    return jsbool::New(false);
  }
}

static jsval msgGetRequestUriUser(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string requestURI;
  if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
  {
    std::string user;
    if (SIPURI::getUser(requestURI, user))
      return jsstring::New(user.c_str());
  }
  return jsstring::New("");
}

static jsval msgSetRequestUriUser(const jsargs& args)
{
  if (args.Length() < 2)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  std::string user = jsvalToString(args[1]);

  try
  {
    std::string requestURI;
    if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
    {
      SIPURI::setUserInfo(requestURI, user.c_str());
      if (SIPRequestLine::setURI(pMsg->startLine(), requestURI.c_str()))
        return jsbool::New(true);
      else
        return jsbool::New(false);
    }
  }
  catch(...)
  {
    return jsbool::New(false);
  }
  return jsbool::New(false);
}

static jsval msgGetRequestUriHostPort(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string requestURI;
  if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
  {
    std::string hostPort;
    if (SIPURI::getHostPort(requestURI, hostPort))
      return jsstring::New(hostPort.c_str());
  }
  return jsvoid();
}

static jsval msgSetRequestUriHostPort(const jsargs& args)
{
  if (args.Length() < 2)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  std::string hostPort = jsvalToString(args[1]);

  try
  {
    std::string requestURI;
    if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
    {
      SIPURI::setHostPort(requestURI, hostPort.c_str());
      if (SIPRequestLine::setURI(pMsg->startLine(), requestURI.c_str()))
        return jsbool::New(true);
      else
        return jsbool::New(false);
    }
  }
  catch(...)
  {
    return jsbool::New(false);
  }
  return jsbool::New(false);
}

static jsval msgGetRequestUriHost(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string requestURI;
  if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
  {
    std::string host;
    if (SIPURI::getHost(requestURI, host))
      return jsstring::New(host.c_str());
  }
  return jsvoid();
}

jsval msgGetToUser(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
  std::string user;
  if (!SIPFrom::getUser(to, user))
    return jsvoid();
  return jsstring::New(user.c_str());
}

jsval msgSetToUser(const jsargs& args)
{
  if (args.Length() < 1)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  try
  {
    std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
    std::string user = jsvalToString(args[1]);
    if (!SIPFrom::setUser(to, user.c_str()))
      return jsbool::New(false);
    if (!pMsg->hdrSet(OSS::SIP::HDR_TO, to.c_str()))
      return jsbool::New(false);
    return jsbool::New(true);
  }
  catch(...)
  {
    return jsbool::New(false);
  }
}

jsval msgGetToHostPort(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
  std::string hostPort;
  if (!SIPFrom::getHostPort(to, hostPort))
    return jsvoid();
  return jsstring::New(hostPort.c_str());
}

jsval msgGetToHost(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
  std::string host;
  if (!SIPFrom::getHost(to, host))
    return jsvoid();
  return jsstring::New(host.c_str());
}

jsval msgSetToHostPort(const jsargs& args)
{
  if (args.Length() < 1)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  try
  {
    std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
    std::string hostPort = jsvalToString(args[1]);
    if (!SIPFrom::setHostPort(to, hostPort.c_str()))
      return jsbool::New(false);
    if (!pMsg->hdrSet(OSS::SIP::HDR_TO, to.c_str()))
      return jsbool::New(false);
    return jsbool::New(true);
  }
  catch(...)
  {
    return jsbool::New(false);
  }
}

jsval msgGetFromUser(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_FROM);
  std::string user;
  if (!SIPFrom::getUser(to, user))
    return jsvoid();
  return jsstring::New(user.c_str());
}

jsval msgSetFromUser(const jsargs& args)
{
  if (args.Length() < 1)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  try
  {
    std::string from = pMsg->hdrGet(OSS::SIP::HDR_FROM);
    std::string user = jsvalToString(args[1]);
    if (!SIPFrom::setUser(from, user.c_str()))
      return jsbool::New(false);
    if (!pMsg->hdrSet(OSS::SIP::HDR_FROM, from.c_str()))
      return jsbool::New(false);
    return jsbool::New(true);
  }
  catch(...)
  {
    return jsbool::New(false);
  }
}

jsval msgGetFromHostPort(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_FROM);
  std::string hostPort;
  if (!SIPFrom::getHostPort(to, hostPort))
    return jsvoid();
  return jsstring::New(hostPort.c_str());
}

jsval msgGetFromHost(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string from = pMsg->hdrGet(OSS::SIP::HDR_FROM);
  std::string host;
  if (!SIPFrom::getHost(from, host))
    return jsvoid();
  return jsstring::New(host.c_str());
}

jsval msgSetFromHostPort(const jsargs& args)
{
  if (args.Length() < 1)
    return jsbool::New(false);

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsbool::New(false);

  try
  {
    std::string from = pMsg->hdrGet(OSS::SIP::HDR_FROM);
    std::string hostPort = jsvalToString(args[1]);
    if (!SIPFrom::setHostPort(from, hostPort.c_str()))
      return jsbool::New(false);
    if (!pMsg->hdrSet(OSS::SIP::HDR_FROM, from.c_str()))
      return jsbool::New(false);
    return jsbool::New(true);
  }
  catch(...)
  {
    return jsbool::New(false);
  }
}

jsval msgGetContactUri(const jsargs& args)
{
  if (args.Length() < 1)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string hContactList = pMsg->hdrGet(OSS::SIP::HDR_CONTACT);
  if (hContactList.empty())
    return jsvoid();

  ContactURI contact;
  if (!hContactList.empty())
    SIPContact::getAt(hContactList, contact, 0);

  std::string contactUri = contact.getURI();

  return jsstring::New(contactUri.c_str());
}

jsval msgGetContactParameter(const jsargs& args)
{
  if (args.Length() < 2)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string param = jsvalToString(args[1]);
  if (param.empty())
    return jsvoid();

  std::string hContactList = pMsg->hdrGet(OSS::SIP::HDR_CONTACT);
  if (hContactList.empty())
    return jsvoid();

  ContactURI contact;
  if (!hContactList.empty())
    SIPContact::getAt(hContactList, contact, 0);

  std::string value = contact.getHeaderParam(param.c_str());
  if (value.empty())
    return jsvoid();

  return jsstring::New(value.c_str());
}

jsval msgGetAuthenticator(const jsargs& args)
{
  if (args.Length() < 2)
    return jsvoid();

  jsscope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return jsvoid();

  std::string realm = jsvalToString(args[1]);
  OSS::string_to_lower(realm);

  int wwwAuthSize = pMsg->hdrGetSize(OSS::SIP::HDR_AUTHORIZATION);
  int proxyAuthSize = pMsg->hdrGetSize(OSS::SIP::HDR_PROXY_AUTHORIZATION);

  std::ostringstream realmMatch;
  realmMatch << "realm=" << "\"" << realm << "\"";

  std::string authenticator = "";
  if (wwwAuthSize > 0)
  {
    for (int i = 0; i < wwwAuthSize; i++)
    {
      std::string hstr = pMsg->hdrGet(OSS::SIP::HDR_AUTHORIZATION, i);
      if (!hstr.empty())
      {
        if (realm == "*")
        {
          authenticator = hstr;
          break;
        }
        else
        {
          OSS::string_to_lower(hstr);
          if (hstr.find(realmMatch.str()) != std::string::npos)
          {
            authenticator = hstr;
            break;
          }
        }
      }
    }
  }

  if (proxyAuthSize > 0 && authenticator.empty())
  {
    for (int i = 0; i < proxyAuthSize; i++)
    {
      std::string hstr = pMsg->hdrGet(OSS::SIP::HDR_PROXY_AUTHORIZATION, i);
      if (!hstr.empty())
      {
        if (realm == "*")
        {
          authenticator = hstr;
          break;
        }
        else
        {
          OSS::string_to_lower(hstr);
          if (hstr.find(realmMatch.str()) != std::string::npos)
          {
            authenticator = hstr;
            break;
          }
        }
      }
    }
  }

  return jsstring::New(authenticator.c_str());
}


void JSSIPMessage::initGlobalFuncs(OSS_HANDLE objectTemplate)
{
  v8::Handle<v8::ObjectTemplate>& global = *(static_cast<v8::Handle<v8::ObjectTemplate>*>(objectTemplate));
  global->Set(jsstring::New("msgHdrPresent"), jsfunc::New(msgHdrPresent));
  global->Set(jsstring::New("msgHdrGetSize"), jsfunc::New(msgHdrGetSize));
  global->Set(jsstring::New("msgHdrGet"), jsfunc::New(msgHdrGet));
  global->Set(jsstring::New("msgHdrSet"), jsfunc::New(msgHdrSet));
  global->Set(jsstring::New("msgHdrRemove"), jsfunc::New(msgHdrRemove));
  global->Set(jsstring::New("msgHdrListAppend"), jsfunc::New(msgHdrListAppend));
  global->Set(jsstring::New("msgHdrListPrepend"), jsfunc::New(msgHdrListPrepend));
  global->Set(jsstring::New("msgHdrListPopFront"), jsfunc::New(msgHdrListPopFront));
  global->Set(jsstring::New("msgHdrListRemove"), jsfunc::New(msgHdrListRemove));
  global->Set(jsstring::New("msgIsRequest"), jsfunc::New(msgIsRequest));
  global->Set(jsstring::New("msgIsResponse"), jsfunc::New(msgIsResponse));
  global->Set(jsstring::New("msgIs1xx"), jsfunc::New(msgIs1xx));
  global->Set(jsstring::New("msgIs2xx"), jsfunc::New(msgIs2xx));
  global->Set(jsstring::New("msgIs3xx"), jsfunc::New(msgIs3xx));
  global->Set(jsstring::New("msgIs4xx"), jsfunc::New(msgIs4xx));
  global->Set(jsstring::New("msgIs5xx"), jsfunc::New(msgIs5xx));
  global->Set(jsstring::New("msgIs6xx"), jsfunc::New(msgIs6xx));
  global->Set(jsstring::New("msgIsResponseFamily"), jsfunc::New(msgIsResponseFamily));
  global->Set(jsstring::New("msgIsErrorResponse"), jsfunc::New(msgIsErrorResponse));
  global->Set(jsstring::New("msgIsMidDialog"), jsfunc::New(msgIsMidDialog));
  global->Set(jsstring::New("msgGetBody"), jsfunc::New(msgGetBody));
  global->Set(jsstring::New("msgSetBody"), jsfunc::New(msgSetBody));
  global->Set(jsstring::New("msgGetStartLine"), jsfunc::New(msgGetStartLine));
  global->Set(jsstring::New("msgSetStartLine"), jsfunc::New(msgSetStartLine));
  global->Set(jsstring::New("msgGetData"), jsfunc::New(msgGetData));
  global->Set(jsstring::New("msgCommitData"), jsfunc::New(msgCommitData));
  global->Set(jsstring::New("msgGetRequestUri"), jsfunc::New(msgGetRequestUri));
  global->Set(jsstring::New("msgSetRequestUri"), jsfunc::New(msgSetRequestUri));
  global->Set(jsstring::New("msgGetRequestUriUser"), jsfunc::New(msgGetRequestUriUser));
  global->Set(jsstring::New("msgSetRequestUriUser"), jsfunc::New(msgSetRequestUriUser));
  global->Set(jsstring::New("msgSetRequestUriHostPort"), jsfunc::New(msgSetRequestUriHostPort));
  global->Set(jsstring::New("msgGetRequestUriHostPort"), jsfunc::New(msgGetRequestUriHostPort));
  global->Set(jsstring::New("msgGetRequestUriHost"), jsfunc::New(msgGetRequestUriHost));
  global->Set(jsstring::New("msgGetToUser"), jsfunc::New(msgGetToUser));
  global->Set(jsstring::New("msgSetToUser"), jsfunc::New(msgSetToUser));
  global->Set(jsstring::New("msgGetToHostPort"), jsfunc::New(msgGetToHostPort));
  global->Set(jsstring::New("msgGetToHost"), jsfunc::New(msgGetToHost));
  global->Set(jsstring::New("msgSetToHostPort"), jsfunc::New(msgSetToHostPort));
  global->Set(jsstring::New("msgGetFromUser"), jsfunc::New(msgGetFromUser));
  global->Set(jsstring::New("msgSetFromUser"), jsfunc::New(msgSetFromUser));
  global->Set(jsstring::New("msgGetFromHostPort"), jsfunc::New(msgGetFromHostPort));
  global->Set(jsstring::New("msgGetFromHost"), jsfunc::New(msgGetFromHost));
  global->Set(jsstring::New("msgSetFromHostPort"), jsfunc::New(msgSetFromHostPort));
  global->Set(jsstring::New("msgGetContactUri"), jsfunc::New(msgGetContactUri));
  global->Set(jsstring::New("msgGetContactParameter"), jsfunc::New(msgGetContactParameter));
  global->Set(jsstring::New("msgGetAuthenticator"), jsfunc::New(msgGetAuthenticator));
  //
  // Request-Line
  //
  global->Set(jsstring::New("requestLineGetMethod"), jsfunc::New(requestLineGetMethod));
  global->Set(jsstring::New("requestLineGetURI"), jsfunc::New(requestLineGetURI));
  global->Set(jsstring::New("requestLineGetVersion"), jsfunc::New(requestLineGetVersion));
  global->Set(jsstring::New("requestLineSetMethod"), jsfunc::New(requestLineSetMethod));
  global->Set(jsstring::New("requestLineSetURI"), jsfunc::New(requestLineSetURI));
  global->Set(jsstring::New("requestLineSetVersion"), jsfunc::New(requestLineSetVersion));

  //
  // Status-Line
  //
  global->Set(jsstring::New("statusLineGetVersion"), jsfunc::New(statusLineGetVersion));
  global->Set(jsstring::New("statusLineSetVersion"), jsfunc::New(statusLineSetVersion));
  global->Set(jsstring::New("statusLineGetStatusCode"), jsfunc::New(statusLineGetStatusCode));
  global->Set(jsstring::New("statusLineSetStatusCode"), jsfunc::New(statusLineSetStatusCode));
  global->Set(jsstring::New("statusLineGetReasonPhrase"), jsfunc::New(statusLineGetReasonPhrase));
  global->Set(jsstring::New("statusLineSetReasonPhrase"), jsfunc::New(statusLineSetReasonPhrase));

  //
  // URI
  //
  global->Set(jsstring::New("uriSetScheme"), jsfunc::New(uriSetScheme));
  global->Set(jsstring::New("uriGetScheme"), jsfunc::New(uriGetScheme));
  global->Set(jsstring::New("uriGetUser"), jsfunc::New(uriGetUser));
  global->Set(jsstring::New("uriSetUserInfo"), jsfunc::New(uriSetUserInfo));
  global->Set(jsstring::New("uriGetPassword"), jsfunc::New(uriGetPassword));
  global->Set(jsstring::New("uriGetHostPort"), jsfunc::New(uriGetHostPort));
  global->Set(jsstring::New("uriSetHostPort"), jsfunc::New(uriSetHostPort));
  global->Set(jsstring::New("uriGetParams"), jsfunc::New(uriGetParams));
  global->Set(jsstring::New("uriSetParams"), jsfunc::New(uriSetParams));
  global->Set(jsstring::New("uriHasParam"), jsfunc::New(uriHasParam));
  global->Set(jsstring::New("uriGetParam"), jsfunc::New(uriGetParam));
  global->Set(jsstring::New("uriGetParamEx"), jsfunc::New(uriGetParamEx));
  global->Set(jsstring::New("uriSetParam"), jsfunc::New(uriSetParam));
  global->Set(jsstring::New("uriSetParamEx"), jsfunc::New(uriSetParamEx));
  global->Set(jsstring::New("uriEscapeUser"), jsfunc::New(uriEscapeUser));
  global->Set(jsstring::New("uriEscapeParam"), jsfunc::New(uriEscapeParam));
  global->Set(jsstring::New("uriGetHeaders"), jsfunc::New(uriGetHeaders));
  global->Set(jsstring::New("uriSetHeaders"), jsfunc::New(uriSetHeaders));
  global->Set(jsstring::New("uriVerify"), jsfunc::New(uriVerify));
  
  //
  // From Processing
  //
  global->Set(jsstring::New("fromGetDisplayName"), jsfunc::New(fromGetDisplayName));
  global->Set(jsstring::New("fromSetDisplayName"), jsfunc::New(fromSetDisplayName));
  global->Set(jsstring::New("fromGetURI"), jsfunc::New(fromGetURI));
  global->Set(jsstring::New("fromSetURI"), jsfunc::New(fromSetURI));
  global->Set(jsstring::New("fromGetHeaderParams"), jsfunc::New(fromGetHeaderParams));
  global->Set(jsstring::New("fromSetHeaderParams"), jsfunc::New(fromSetHeaderParams));
  global->Set(jsstring::New("fromGetHeaderParam"), jsfunc::New(fromGetHeaderParam));
  global->Set(jsstring::New("fromGetHeaderParamEx"), jsfunc::New(fromGetHeaderParamEx));
  global->Set(jsstring::New("fromSetHeaderParam"), jsfunc::New(fromSetHeaderParam));
  global->Set(jsstring::New("fromSetHeaderParamEx"), jsfunc::New(fromSetHeaderParamEx));

  //
  // To Processing
  //
  global->Set(jsstring::New("toGetDisplayName"), jsfunc::New(fromGetDisplayName));
  global->Set(jsstring::New("toSetDisplayName"), jsfunc::New(fromSetDisplayName));
  global->Set(jsstring::New("toGetURI"), jsfunc::New(fromGetURI));
  global->Set(jsstring::New("toSetURI"), jsfunc::New(fromSetURI));
  global->Set(jsstring::New("toGetHeaderParams"), jsfunc::New(fromGetHeaderParams));
  global->Set(jsstring::New("toSetHeaderParams"), jsfunc::New(fromSetHeaderParams));
  global->Set(jsstring::New("toGetHeaderParam"), jsfunc::New(fromGetHeaderParam));
  global->Set(jsstring::New("toGetHeaderParamEx"), jsfunc::New(fromGetHeaderParamEx));
  global->Set(jsstring::New("toSetHeaderParam"), jsfunc::New(fromSetHeaderParam));
  global->Set(jsstring::New("toSetHeaderParamEx"), jsfunc::New(fromSetHeaderParamEx));

  //
  // Misc functions
  //
  global->Set(jsstring::New("cidrVerify"), jsfunc::New(cidrVerify));
  global->Set(jsstring::New("isIpInRange"), jsfunc::New(isIpInRange));
  global->Set(jsstring::New("wildCardCompare"), jsfunc::New(wildCardCompare));
  global->Set(jsstring::New("md5Hash"), jsfunc::New(md5Hash));
}

} } //const JSSIPMessage& msg OSS::JS




