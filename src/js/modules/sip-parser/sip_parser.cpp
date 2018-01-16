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


#include <OSS/SIP/SIPHeaderTokens.h>

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
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"

#include "OSS/JS/JSModule.h"


using namespace OSS::SIP;
using B2BUA::SIPB2BTransaction;

static /*size_t*/ v8::Handle<v8::Value> msgGetMethod(const v8::Arguments& args/*const char* headerName*/)
{
  if (args.Length() != 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  try
  {
    std::string cseq = pMsg->hdrGet(OSS::SIP::HDR_CSEQ);
    OSS::SIP::SIPCSeq hCSeq(cseq.c_str());
    return v8::String::New(hCSeq.getMethod().c_str());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgGetMethod - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*size_t*/ v8::Handle<v8::Value> msgHdrPresent(const v8::Arguments& args/*const char* headerName*/)
{
  if (args.Length() != 2)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string headerName = OSS::JS::string_from_js_string(args[1]);
  if (headerName.empty())
    return v8::Undefined();

  try
  {
    return v8::Boolean::New(pMsg->hdrPresent(headerName.c_str()));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrPresent - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static  /*size_t*/ v8::Handle<v8::Value> msgHdrGetSize(const v8::Arguments& args/*const char* headerName*/)
{
  if (args.Length() != 2)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string headerName = OSS::JS::string_from_js_string(args[1]);
  if (headerName.empty())
    return v8::Undefined();
  
  try
  {
    return v8::Integer::New(pMsg->hdrGetSize(headerName.c_str()));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrGetSize - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*const std::string&*/ v8::Handle<v8::Value> msgHdrGet(const v8::Arguments& args/*const char* headerName, size_t index = 0*/)
{
  if (args.Length() < 2)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string headerName = OSS::JS::string_from_js_string(args[1]);
  if (headerName.empty())
    return v8::Undefined();
  try
  {
    if (args.Length() > 2)
    {
      size_t index = args[2]->NumberValue();
      return v8::String::New(pMsg->hdrGet(headerName.c_str(), index).c_str());
    }

    return v8::String::New(pMsg->hdrGet(headerName.c_str()).c_str());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrGet - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*bool*/ v8::Handle<v8::Value> msgHdrSet(const v8::Arguments& args/*const char* headerName, const std::string& headerValue, size_t index*/)
{
  if (args.Length() < 3)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  std::string headerName = OSS::JS::string_from_js_string(args[1]);
  if (headerName.empty())
    return v8::Boolean::New(false);

  std::string headerValue = OSS::JS::string_from_js_string(args[2]);
  if (headerValue.empty())
    return v8::Boolean::New(false);

  size_t index = 0;
  if (args.Length() > 3)
    index = args[3]->NumberValue();
  
  try
  {
    return v8::Boolean::New(pMsg->hdrSet(headerName.c_str(), headerValue, index));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrSet - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*bool*/ v8::Handle<v8::Value> msgHdrRemove(const v8::Arguments& args/*const char* headerName*/)
{
  if (args.Length() < 2)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  std::string headerName = OSS::JS::string_from_js_string(args[1]);
  if (headerName.empty())
    return v8::Boolean::New(false);
  
  try
  {
    return v8::Boolean::New(pMsg->hdrRemove(headerName.c_str()));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrRemove - " << e.message();
    OSS::log_error(msg.str());
    return v8::Boolean::New(false);
  }
}

static /*bool*/ v8::Handle<v8::Value> msgHdrListAppend(const v8::Arguments& args/*const char* name, const std::string& value*/)
{
  if (args.Length() < 3)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  std::string headerName = OSS::JS::string_from_js_string(args[1]);
  if (headerName.empty())
    return v8::Boolean::New(false);

  std::string headerValue = OSS::JS::string_from_js_string(args[2]);
  if (headerValue.empty())
    return v8::Boolean::New(false);

  try
  {
    return v8::Boolean::New(pMsg->hdrListAppend(headerName.c_str(), headerValue));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrListAppend - " << e.message();
    OSS::log_error(msg.str());
    return v8::Boolean::New(false);
  }
}

static /*bool*/ v8::Handle<v8::Value> msgHdrListPrepend(const v8::Arguments& args/*const char* name, const std::string& value*/)
{
  if (args.Length() < 3)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  std::string headerName = OSS::JS::string_from_js_string(args[1]);
  if (headerName.empty())
    return v8::Boolean::New(false);

  std::string headerValue = OSS::JS::string_from_js_string(args[2]);
  if (headerValue.empty())
    return v8::Boolean::New(false);

  try
  {
    return v8::Boolean::New(pMsg->hdrListPrepend(headerName.c_str(), headerValue));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrListPrepend - " << e.message();
    OSS::log_error(msg.str());
    return v8::Boolean::New(false);
  }
}

static /*std::string*/ v8::Handle<v8::Value> msgHdrListPopFront(const v8::Arguments& args/*const char* name*/)
{
  if (args.Length() < 2)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string headerName = OSS::JS::string_from_js_string(args[1]);
  if (headerName.empty())
    return v8::Undefined();

  try
  {
    return v8::String::New(pMsg->hdrListPopFront(headerName.c_str()).c_str());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrListPopFront - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*bool*/ v8::Handle<v8::Value> msgHdrListRemove(const v8::Arguments& args/*const char* name*/)
{
  if (args.Length() < 2)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  std::string headerName = OSS::JS::string_from_js_string(args[1]);
  if (headerName.empty())
    return v8::Boolean::New(false);

  try
  {
    return v8::Boolean::New(pMsg->hdrListRemove(headerName.c_str()));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrListRemove - " << e.message();
    OSS::log_error(msg.str());
    return v8::Boolean::New(false);
  }
}

static  /*boost::tribool*/ v8::Handle<v8::Value> msgIsRequest(const v8::Arguments& args/*const char* method = 0*/)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string method;
  if (args.Length() >= 2)
    method = OSS::JS::string_from_js_string(args[1]);

  try
  {
    if (method.empty())
      return v8::Boolean::New(pMsg->isRequest());
    else
      return v8::Boolean::New(pMsg->isRequest(method.c_str()));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIsRequest - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}


static /*boost::tribool*/ v8::Handle<v8::Value> msgIsResponse(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  try
  {
    return v8::Boolean::New(pMsg->isResponse());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIsResponse - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*boost::tribool*/ v8::Handle<v8::Value> msgIs1xx(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  try
  {
    return v8::Boolean::New(pMsg->is1xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs1xx - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*boost::tribool*/ v8::Handle<v8::Value> msgIs2xx(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  try
  {
    return v8::Boolean::New(pMsg->is2xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs2xx - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*boost::tribool*/ v8::Handle<v8::Value> msgIs3xx(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  try
  {
    return v8::Boolean::New(pMsg->is3xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs3xx - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*boost::tribool*/ v8::Handle<v8::Value> msgIs4xx(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  try
  {
    return v8::Boolean::New(pMsg->is4xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs4xx - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*boost::tribool*/ v8::Handle<v8::Value> msgIs5xx(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  try
  {
    return v8::Boolean::New(pMsg->is5xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs5xx - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*boost::tribool*/ v8::Handle<v8::Value> msgIs6xx(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  try
  {
    return v8::Boolean::New(pMsg->is6xx());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIs6xx - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*boost::tribool*/ v8::Handle<v8::Value> msgIsResponseFamily(const v8::Arguments& args/*int responseCode*/)
{
  if (args.Length() < 2)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  int responseCode = args[1]->NumberValue();

  try
  {
    return v8::Boolean::New(pMsg->isResponseFamily(responseCode));
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIsResponseFamily - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
  
}

static /*boost::tribool*/ v8::Handle<v8::Value> msgIsErrorResponse(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  try
  {
    return v8::Boolean::New(pMsg->isErrorResponse());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgIsErrorResponse - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*boost::tribool*/ v8::Handle<v8::Value> msgIsMidDialog(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  try
  {
    return v8::Boolean::New(pMsg->isMidDialog());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream msg;
    msg << "JavaScript->C++ Exception: msgHdrGet - " << e.message();
    OSS::log_error(msg.str());
    return v8::Undefined();
  }
}

static /*std::string&*/ v8::Handle<v8::Value> msgGetBody(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  return v8::String::New(pMsg->getBody().c_str());
}

static /*bool*/ v8::Handle<v8::Value> msgSetBody(const v8::Arguments& args/*const std::string& body*/)
{
  if (args.Length() < 2)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  std::string body = OSS::JS::string_from_js_string(args[1]);
  pMsg->setBody(body);

  return v8::Boolean::New(true);
}

static /*std::string&*/ v8::Handle<v8::Value> msgGetStartLine(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  return v8::String::New(pMsg->getStartLine().c_str());
}

static /*std::string&*/ v8::Handle<v8::Value> msgSetStartLine(const v8::Arguments& args/*const std::string& sline*/)
{
  if (args.Length() < 2)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string sline = OSS::JS::string_from_js_string(args[1]);
  pMsg->setStartLine(sline);

  return v8::Undefined();
}

static v8::Handle<v8::Value> msgGetData(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();
  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();
  return v8::String::New(pMsg->data().c_str());
}

static v8::Handle<v8::Value> msgCommitData(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();
  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();
  pMsg->commitData();
  return v8::String::New(pMsg->data().c_str());
}

//
// Request-Line Processing
//

static /*bool*/ v8::Handle<v8::Value> requestLineGetMethod(const v8::Arguments& args/*const std::string& rline, std::string& method*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string result;
  if (OSS::SIP::SIPRequestLine::getMethod(input, result))
    return v8::String::New(result.c_str());
  return v8::Undefined();
}


static /*bool*/ v8::Handle<v8::Value> requestLineGetURI(const v8::Arguments& args/*const std::string& rline, std::string& uri*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string result;
  if (OSS::SIP::SIPRequestLine::getURI(input, result))
    return v8::String::New(result.c_str());
  return v8::Undefined();
}

static /*bool*/ v8::Handle<v8::Value> requestLineGetVersion(const v8::Arguments& args/*const std::string& rline, std::string& version*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string result;
  if (OSS::SIP::SIPRequestLine::getVersion(input, result))
    return v8::String::New(result.c_str());
  return v8::Undefined();
}

static /*bool*/ v8::Handle<v8::Value> requestLineSetMethod(const v8::Arguments& args/*std::string& rline, const char* method*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  return v8::Boolean::New(OSS::SIP::SIPRequestLine::setMethod(input, newval.c_str()));
}

static /*bool*/ v8::Handle<v8::Value> requestLineSetURI(const v8::Arguments& args/*std::string& rline, const char* uri*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  if (!OSS::SIP::SIPRequestLine::setURI(input, newval.c_str()))
    return v8::Undefined();
  return v8::String::New(input.c_str());
}


static /*bool*/ v8::Handle<v8::Value> requestLineSetVersion(const v8::Arguments& args/*std::string& rline, const char* version*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  return v8::Boolean::New(OSS::SIP::SIPRequestLine::setVersion(input, newval.c_str()));
}


//
// Status-Line Processing
//

static v8::Handle<v8::Value>/*bool*/ statusLineGetVersion(const v8::Arguments& args/*const std::string& sline, std::string& version*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string result;
  if (OSS::SIP::SIPStatusLine::getVersion(input, result))
    return v8::String::New(result.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ statusLineSetVersion(const v8::Arguments& args/*std::string& sline, const char* version*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  return v8::Boolean::New(OSS::SIP::SIPStatusLine::setVersion(input, newval.c_str()));
}


static v8::Handle<v8::Value>/*bool*/ statusLineGetStatusCode(const v8::Arguments& args/*const std::string& sline,std::string& statusCode*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string result;
  if (OSS::SIP::SIPStatusLine::getStatusCode(input, result))
    return v8::String::New(result.c_str());
  return v8::Undefined();
}


static v8::Handle<v8::Value>/*bool*/ statusLineSetStatusCode(const v8::Arguments& args/*std::string& sline, const char* statusCode*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  return v8::Boolean::New(OSS::SIP::SIPStatusLine::setStatusCode(input, newval.c_str()));
}


static v8::Handle<v8::Value>/*bool*/ statusLineGetReasonPhrase(const v8::Arguments& args/*const std::string& sline, std::string& reasonPhrase*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string result;
  if (OSS::SIP::SIPStatusLine::getReasonPhrase(input, result))
    return v8::String::New(result.c_str());
  return v8::Undefined();
}


static v8::Handle<v8::Value>/*bool*/ statusLineSetReasonPhrase(const v8::Arguments& args/*std::string& sline, const char* reasonPhrase*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  return v8::Boolean::New(OSS::SIP::SIPStatusLine::setReasonPhrase(input, newval.c_str()));
}

//
// URI Processing
//

static v8::Handle<v8::Value>/*bool*/ uriSetScheme(const v8::Arguments& args/*std::string& uri, const char* scheme*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  if (OSS::SIP::SIPURI::setScheme(input, newval.c_str()))
    return v8::String::New(input.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriGetScheme(const v8::Arguments& args/*const std::string& uri, std::string& value*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getScheme(input, val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriGetUser(const v8::Arguments& args/*const std::string& uri, std::string& value*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getUser(input, val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriSetUserInfo(const v8::Arguments& args/*std::string& uri, const char* userInfo*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  if (OSS::SIP::SIPURI::setUserInfo(input, newval.c_str()))
    return v8::String::New(input.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriGetPassword(const v8::Arguments& args/*const std::string& uri, std::string& value*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getPassword(input, val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriGetHostPort(const v8::Arguments& args/*const std::string& uri, std::string& value*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getHostPort(input, val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriSetHostPort(const v8::Arguments& args/*std::string& uri, const char* hostPort*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  if (OSS::SIP::SIPURI::setHostPort(input, newval.c_str()))
    return v8::String::New(input.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriGetParams(const v8::Arguments& args/*const std::string& uri, std::string& params*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getParams(input, val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriSetParams(const v8::Arguments& args/*std::string& uri, const std::string& params*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  if (OSS::SIP::SIPURI::setParams(input, newval.c_str()))
    return v8::String::New(input.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriHasParam(const v8::Arguments& args/*const std::string& uri, const char* paraName*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string val = OSS::JS::string_from_js_string(args[1]);
  return v8::Boolean::New(OSS::SIP::SIPURI::hasParam(input, val.c_str()));
}

static v8::Handle<v8::Value>/*bool*/ uriGetParam(const v8::Arguments& args/*const std::string& uri, const char* paramName, std::string& paramValue*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string name = OSS::JS::string_from_js_string(args[1]);
  std::string value;
  if (OSS::SIP::SIPURI::getParam(input, name.c_str(), value))
    return v8::String::New(value.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriGetParamEx(const v8::Arguments& args/*const std::string& params, const char* paramName, std::string& paramValue*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string name = OSS::JS::string_from_js_string(args[1]);
  std::string value;
  if (OSS::SIP::SIPURI::getParamEx(input, name.c_str(), value))
    return v8::String::New(value.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriSetParam(const v8::Arguments& args/*std::string& uri, const char* paramName, const char* paramValue*/)
{
  if (args.Length() < 3)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string name = OSS::JS::string_from_js_string(args[1]);
  std::string newval = OSS::JS::string_from_js_string(args[2]);
  return v8::Boolean::New(OSS::SIP::SIPURI::setParam(input, name.c_str(), newval.c_str()));
}

static v8::Handle<v8::Value>/*bool*/ uriSetParamEx(const v8::Arguments& args/*std::string& params, const char* paramName, const char* paramValue*/)
{
  if (args.Length() < 3)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string name = OSS::JS::string_from_js_string(args[1]);
  std::string newval = OSS::JS::string_from_js_string(args[2]);
  return v8::Boolean::New(OSS::SIP::SIPURI::setParam(input, name.c_str(), newval.c_str()));
}

static v8::Handle<v8::Value>/*void*/ uriEscapeUser(const v8::Arguments& args/*std::string& result, const char* user*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string result;
  OSS::SIP::SIPURI::escapeUser(result, input.c_str());
    return v8::String::New(result.c_str());
}

static v8::Handle<v8::Value>/*void*/ uriEscapeParam(const v8::Arguments& args/*std::string& result, const char* param*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string result;
  OSS::SIP::SIPURI::escapeParam(result, input.c_str());
  return v8::String::New(result.c_str());
}

static v8::Handle<v8::Value>/*bool*/ uriGetHeaders(const v8::Arguments& args/*const std::string& uri, std::string& headers*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string val;
  if (OSS::SIP::SIPURI::getHeaders(input, val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriSetHeaders(const v8::Arguments& args/*std::string& uri, const std::string& headers*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  if (OSS::SIP::SIPURI::setHeaders(input, newval.c_str()))
    return v8::String::New(input.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value>/*bool*/ uriVerify(const v8::Arguments& args/*const char* uri*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  return v8::Boolean::New(OSS::SIP::SIPURI::verify(input.c_str()));
}

//
// From Processing
//

static v8::Handle<v8::Value> /*bool*/ fromGetDisplayName(const v8::Arguments& args/*const std::string& from, std::string& displayName*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string val;
  if (OSS::SIP::SIPFrom::getDisplayName(input, val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value> /*bool*/ fromSetDisplayName(const v8::Arguments& args/*std::string& from, const char* uri*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  if (OSS::SIP::SIPFrom::setDisplayName(input, newval.c_str()))
    return v8::String::New(input.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value> /*bool*/ fromGetURI(const v8::Arguments& args/*const std::string& from, std::string& uri*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string val;
  if (OSS::SIP::SIPFrom::getURI(input, val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value> /*bool*/ fromSetURI(const v8::Arguments& args/*std::string& from, const char* uri*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  if (OSS::SIP::SIPFrom::setURI(input, newval.c_str()))
    return v8::String::New(input.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value> /*bool*/ fromGetHeaderParams(const v8::Arguments& args/*const std::string& from, std::string& headerParams*/)
{
  if (args.Length() < 1)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string val;
  if (OSS::SIP::SIPFrom::getHeaderParams(input, val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value> /*bool*/ fromSetHeaderParams(const v8::Arguments& args/*std::string& from, const char* headerParams*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string newval = OSS::JS::string_from_js_string(args[1]);
  if (OSS::SIP::SIPFrom::setHeaderParams(input, newval.c_str()))
    return v8::String::New(input.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value> /*bool*/ fromGetHeaderParam(const v8::Arguments& args/*const std::string& from, const char* paramName, std::string& paramValue*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string name = OSS::JS::string_from_js_string(args[1]);
  std::string val;
  if (OSS::SIP::SIPFrom::getHeaderParam(input, name.c_str(), val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value> /*bool*/ fromGetHeaderParamEx(const v8::Arguments& args/*const std::string& headerParams, const char* paramName, std::string& paramValue*/)
{
  if (args.Length() < 2)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string name = OSS::JS::string_from_js_string(args[1]);
  std::string val;
  if (OSS::SIP::SIPFrom::getHeaderParamEx(input, name.c_str(), val))
    return v8::String::New(val.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value> /*bool*/ fromSetHeaderParam(const v8::Arguments& args/*std::string& from, const char* paramName, const char* paramValue*/)
{
  if (args.Length() < 3)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string name = OSS::JS::string_from_js_string(args[1]);
  std::string newval = OSS::JS::string_from_js_string(args[2]);
  if (OSS::SIP::SIPFrom::setHeaderParam(input, name.c_str(), newval.c_str()))
    return v8::String::New(input.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value> /*bool*/ fromSetHeaderParamEx(const v8::Arguments& args/*std::string& headerParams, const char* paramName, const char* paramValue*/)
{
  if (args.Length() < 3)
    return v8::Undefined();
  std::string input = OSS::JS::string_from_js_string(args[0]);
  std::string name = OSS::JS::string_from_js_string(args[1]);
  std::string newval = OSS::JS::string_from_js_string(args[2]);
  if (OSS::SIP::SIPFrom::setHeaderParamEx(input, name.c_str(), newval.c_str()))
    return v8::String::New(input.c_str());
  return v8::Undefined();
}

static v8::Handle<v8::Value> msgGetRequestUri(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string requestURI;
  if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
  {
    return v8::String::New(requestURI.c_str());
  }
  return v8::Undefined();
}


static v8::Handle<v8::Value> msgSetRequestUri(const v8::Arguments& args)
{
  if (args.Length() < 2)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  std::string uri = OSS::JS::string_from_js_string(args[1]);

  try
  {
    return v8::Boolean::New(SIPRequestLine::setURI(pMsg->startLine(), uri.c_str()));
  }
  catch(...)
  {
    return v8::Boolean::New(false);
  }
}

static v8::Handle<v8::Value> msgGetRequestUriUser(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string requestURI;
  if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
  {
    std::string user;
    if (SIPURI::getUser(requestURI, user))
      return v8::String::New(user.c_str());
  }
  return v8::String::New("");
}

static v8::Handle<v8::Value> msgSetRequestUriUser(const v8::Arguments& args)
{
  if (args.Length() < 2)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  std::string user = OSS::JS::string_from_js_string(args[1]);

  try
  {
    std::string requestURI;
    if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
    {
      SIPURI::setUserInfo(requestURI, user.c_str());
      if (SIPRequestLine::setURI(pMsg->startLine(), requestURI.c_str()))
        return v8::Boolean::New(true);
      else
        return v8::Boolean::New(false);
    }
  }
  catch(...)
  {
    return v8::Boolean::New(false);
  }
  return v8::Boolean::New(false);
}

static v8::Handle<v8::Value> msgGetRequestUriHostPort(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string requestURI;
  if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
  {
    std::string hostPort;
    if (SIPURI::getHostPort(requestURI, hostPort))
      return v8::String::New(hostPort.c_str());
  }
  return v8::Undefined();
}

static v8::Handle<v8::Value> msgSetRequestUriHostPort(const v8::Arguments& args)
{
  if (args.Length() < 2)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  std::string hostPort = OSS::JS::string_from_js_string(args[1]);

  try
  {
    std::string requestURI;
    if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
    {
      SIPURI::setHostPort(requestURI, hostPort.c_str());
      if (SIPRequestLine::setURI(pMsg->startLine(), requestURI.c_str()))
        return v8::Boolean::New(true);
      else
        return v8::Boolean::New(false);
    }
  }
  catch(...)
  {
    return v8::Boolean::New(false);
  }
  return v8::Boolean::New(false);
}

static v8::Handle<v8::Value> msgGetRequestUriHost(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string requestURI;
  if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
  {
    std::string host;
    if (SIPURI::getHost(requestURI, host))
      return v8::String::New(host.c_str());
  }
  return v8::Undefined();
}

static v8::Handle<v8::Value> msgGetRequestUriPort(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string requestURI;
  if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
  {
    std::string port;
    if (SIPURI::getPort(requestURI, port))
      return v8::String::New(port.c_str());
  }
  return v8::Undefined();
}

static v8::Handle<v8::Value> msgGetRequestUriParameters(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string requestURI;
  if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
  {
    std::string params;
    if (SIPURI::getParams(requestURI, params))
      return v8::String::New(params.c_str());
  }
  return v8::Undefined();
}

static v8::Handle<v8::Value> msgSetRequestUriParameters(const v8::Arguments& args)
{
  if (args.Length() < 2)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);
  
  std::string params = OSS::JS::string_from_js_string(args[1]);

  try
  {
    std::string requestURI;
    if (SIPRequestLine::getURI(pMsg->startLine(), requestURI))
    {
      SIPURI::setParams(requestURI, params);
    }
  }
  catch(...)
  {
    return v8::Boolean::New(false);
  }
  return v8::Boolean::New(true);
}

v8::Handle<v8::Value> msgGetToUser(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
  std::string user;
  if (!SIPFrom::getUser(to, user))
    return v8::Undefined();
  return v8::String::New(user.c_str());
}

v8::Handle<v8::Value> msgSetToUser(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  try
  {
    std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
    std::string user = OSS::JS::string_from_js_string(args[1]);
    if (!SIPFrom::setUser(to, user.c_str()))
      return v8::Boolean::New(false);
    if (!pMsg->hdrSet(OSS::SIP::HDR_TO, to.c_str()))
      return v8::Boolean::New(false);
    return v8::Boolean::New(true);
  }
  catch(...)
  {
    return v8::Boolean::New(false);
  }
}

v8::Handle<v8::Value> msgGetToHostPort(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
  std::string hostPort;
  if (!SIPFrom::getHostPort(to, hostPort))
    return v8::Undefined();
  return v8::String::New(hostPort.c_str());
}

v8::Handle<v8::Value> msgGetToHost(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
  std::string host;
  if (!SIPFrom::getHost(to, host))
    return v8::Undefined();
  return v8::String::New(host.c_str());
}

v8::Handle<v8::Value> msgGetToPort(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
  std::string host;
  if (!SIPFrom::getHostPort(to, host))
    return v8::Undefined();
  
  std::vector<std::string> tokens = OSS::string_tokenize(host, ":");
  if (tokens.size() <= 1)
  {
    return v8::Undefined();
  }
  
  return v8::String::New(tokens[1].c_str());
}

v8::Handle<v8::Value> msgSetToHostPort(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  try
  {
    std::string to = pMsg->hdrGet(OSS::SIP::HDR_TO);
    std::string hostPort = OSS::JS::string_from_js_string(args[1]);
    if (!SIPFrom::setHostPort(to, hostPort.c_str()))
      return v8::Boolean::New(false);
    if (!pMsg->hdrSet(OSS::SIP::HDR_TO, to.c_str()))
      return v8::Boolean::New(false);
    return v8::Boolean::New(true);
  }
  catch(...)
  {
    return v8::Boolean::New(false);
  }
}

v8::Handle<v8::Value> msgGetFromUser(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_FROM);
  std::string user;
  if (!SIPFrom::getUser(to, user))
    return v8::Undefined();
  return v8::String::New(user.c_str());
}

v8::Handle<v8::Value> msgSetFromUser(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  try
  {
    std::string from = pMsg->hdrGet(OSS::SIP::HDR_FROM);
    std::string user = OSS::JS::string_from_js_string(args[1]);
    if (!SIPFrom::setUser(from, user.c_str()))
      return v8::Boolean::New(false);
    if (!pMsg->hdrSet(OSS::SIP::HDR_FROM, from.c_str()))
      return v8::Boolean::New(false);
    return v8::Boolean::New(true);
  }
  catch(...)
  {
    return v8::Boolean::New(false);
  }
}

v8::Handle<v8::Value> msgGetFromHostPort(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string to = pMsg->hdrGet(OSS::SIP::HDR_FROM);
  std::string hostPort;
  if (!SIPFrom::getHostPort(to, hostPort))
    return v8::Undefined();
  return v8::String::New(hostPort.c_str());
}

v8::Handle<v8::Value> msgGetFromHost(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string from = pMsg->hdrGet(OSS::SIP::HDR_FROM);
  std::string host;
  if (!SIPFrom::getHost(from, host))
    return v8::Undefined();
  return v8::String::New(host.c_str());
}

v8::Handle<v8::Value> msgGetFromPort(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string from = pMsg->hdrGet(OSS::SIP::HDR_FROM);
  std::string host;
  if (!SIPFrom::getHostPort(from, host))
    return v8::Undefined();
  
  std::vector<std::string> tokens = OSS::string_tokenize(host, ":");
  if (tokens.size() <= 1)
  {
    return v8::Undefined();
  }
  
  return v8::String::New(tokens[1].c_str());
}

v8::Handle<v8::Value> msgSetFromHostPort(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  try
  {
    std::string from = pMsg->hdrGet(OSS::SIP::HDR_FROM);
    std::string hostPort = OSS::JS::string_from_js_string(args[1]);
    if (!SIPFrom::setHostPort(from, hostPort.c_str()))
      return v8::Boolean::New(false);
    if (!pMsg->hdrSet(OSS::SIP::HDR_FROM, from.c_str()))
      return v8::Boolean::New(false);
    return v8::Boolean::New(true);
  }
  catch(...)
  {
    return v8::Boolean::New(false);
  }
}

v8::Handle<v8::Value> msgGetContactUri(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string hContactList = pMsg->hdrGet(OSS::SIP::HDR_CONTACT);
  if (hContactList.empty())
    return v8::Undefined();

  ContactURI contact;
  if (!hContactList.empty())
    SIPContact::getAt(hContactList, contact, 0);

  std::string contactUri = contact.getURI();

  return v8::String::New(contactUri.c_str());
}

v8::Handle<v8::Value> msgGetContactParameter(const v8::Arguments& args)
{
  if (args.Length() < 2)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string param = OSS::JS::string_from_js_string(args[1]);
  if (param.empty())
    return v8::Undefined();

  std::string hContactList = pMsg->hdrGet(OSS::SIP::HDR_CONTACT);
  if (hContactList.empty())
    return v8::Undefined();

  ContactURI contact;
  if (!hContactList.empty())
    SIPContact::getAt(hContactList, contact, 0);

  std::string value = contact.getHeaderParam(param.c_str());
  if (value.empty())
    return v8::Undefined();

  return v8::String::New(value.c_str());
}

v8::Handle<v8::Value> msgGetAuthenticator(const v8::Arguments& args)
{
  if (args.Length() < 2)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string realm = OSS::JS::string_from_js_string(args[1]);
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

  return v8::String::New(authenticator.c_str());
}

static v8::Handle<v8::Value> msgSetTransactionProperty(const v8::Arguments& args)
{
  if (args.Length() < 3)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  SIPB2BTransaction* pTrn = static_cast<SIPB2BTransaction*>(pMsg->userData());
  if (!pTrn)
    return v8::Boolean::New(false);

  std::string name = OSS::JS::string_from_js_string(args[1]);
  std::string value = OSS::JS::string_from_js_string(args[2]);

  if (name.empty() || value.empty())
    return v8::Boolean::New(false);

  pTrn->setProperty(name, value);

  return v8::Boolean::New(true);
}

static v8::Handle<v8::Value> msgGetTransactionProperty(const v8::Arguments& args)
{
  if (args.Length() < 2)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  SIPB2BTransaction* pTrn = static_cast<SIPB2BTransaction*>(pMsg->userData());
  if (!pTrn)
    return v8::Undefined();

  std::string name = OSS::JS::string_from_js_string(args[1]);

  if (name.empty())
    return v8::Undefined();
  std::string value;
  if (name == "log-id")
    value = pTrn->getLogId();
  else
    pTrn->getProperty(name, value);

  return v8::String::New(value.c_str());
}

static v8::Handle<v8::Value> msgSetProperty(const v8::Arguments& args)
{
  if (args.Length() < 3)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Boolean::New(false);

  std::string name = OSS::JS::string_from_js_string(args[1]);
  std::string value = OSS::JS::string_from_js_string(args[2]);

  if (name.empty() || value.empty())
    return v8::Boolean::New(false);

  pMsg->setProperty(name, value);

  return v8::Boolean::New(true);
}

static v8::Handle<v8::Value> msgGetProperty(const v8::Arguments& args)
{
  if (args.Length() < 2)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  std::string name = OSS::JS::string_from_js_string(args[1]);

  if (name.empty())
    return v8::Undefined();
  std::string value;
  pMsg->getProperty(name, value);

  return v8::String::New(value.c_str());
}


static v8::Handle<v8::Value> msgGetSourceAddress(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  SIPB2BTransaction* pTrn = static_cast<SIPB2BTransaction*>(pMsg->userData());
  if (!pTrn || !pTrn->serverTransport())
    return v8::Undefined();

  OSS::Net::IPAddress addr = pTrn->serverTransport()->getRemoteAddress();

  return v8::String::New(addr.toString().c_str());
}

static v8::Handle<v8::Value> msgGetSourcePort(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  SIPB2BTransaction* pTrn = static_cast<SIPB2BTransaction*>(pMsg->userData());
  if (!pTrn || !pTrn->serverTransport())
    return v8::Undefined();

  OSS::Net::IPAddress addr = pTrn->serverTransport()->getRemoteAddress();

  return v8::Integer::New(addr.getPort());
}

static v8::Handle<v8::Value> msgGetInterfaceAddress(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  SIPB2BTransaction* pTrn = static_cast<SIPB2BTransaction*>(pMsg->userData());
  if (!pTrn || !pTrn->serverTransport())
    return v8::Undefined();

  OSS::Net::IPAddress addr = pTrn->serverTransport()->getLocalAddress();
  return v8::String::New(addr.toString().c_str());
}

static v8::Handle<v8::Value> msgGetInterfacePort(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return v8::Undefined();

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = OSS::JS::unwrap_external_object<OSS::SIP::SIPMessage>(args);
  if (!pMsg)
    return v8::Undefined();

  SIPB2BTransaction* pTrn = static_cast<SIPB2BTransaction*>(pMsg->userData());
  if (!pTrn || !pTrn->serverTransport())
    return v8::Undefined();

  OSS::Net::IPAddress addr = pTrn->serverTransport()->getLocalAddress();

  return v8::Integer::New(addr.getPort());
}

JS_EXPORTS_INIT()
{
  js_export_method("msgGetMethod", msgGetMethod);
  js_export_method("msgHdrPresent", msgHdrPresent);
  js_export_method("msgHdrGetSize", msgHdrGetSize);
  js_export_method("msgHdrGet", msgHdrGet);
  js_export_method("msgHdrSet", msgHdrSet);
  js_export_method("msgHdrRemove", msgHdrRemove);
  js_export_method("msgHdrListAppend", msgHdrListAppend);
  js_export_method("msgHdrListPrepend", msgHdrListPrepend);
  js_export_method("msgHdrListPopFront", msgHdrListPopFront);
  js_export_method("msgHdrListRemove", msgHdrListRemove);
  js_export_method("msgIsRequest", msgIsRequest);
  js_export_method("msgIsResponse", msgIsResponse);
  js_export_method("msgIs1xx", msgIs1xx);
  js_export_method("msgIs2xx", msgIs2xx);
  js_export_method("msgIs3xx", msgIs3xx);
  js_export_method("msgIs4xx", msgIs4xx);
  js_export_method("msgIs5xx", msgIs5xx);
  js_export_method("msgIs6xx", msgIs6xx);
  js_export_method("msgIsResponseFamily", msgIsResponseFamily);
  js_export_method("msgIsErrorResponse", msgIsErrorResponse);
  js_export_method("msgIsMidDialog", msgIsMidDialog);
  js_export_method("msgGetBody", msgGetBody);
  js_export_method("msgSetBody", msgSetBody);
  js_export_method("msgGetStartLine", msgGetStartLine);
  js_export_method("msgSetStartLine", msgSetStartLine);
  js_export_method("msgGetData", msgGetData);
  js_export_method("msgCommitData", msgCommitData);
  js_export_method("msgGetRequestUri", msgGetRequestUri);
  js_export_method("msgSetRequestUri", msgSetRequestUri);
  js_export_method("msgGetRequestUriUser", msgGetRequestUriUser);
  js_export_method("msgSetRequestUriUser", msgSetRequestUriUser);
  js_export_method("msgSetRequestUriHostPort", msgSetRequestUriHostPort);
  js_export_method("msgGetRequestUriHostPort", msgGetRequestUriHostPort);
  js_export_method("msgGetRequestUriHost", msgGetRequestUriHost);
  js_export_method("msgGetRequestUriPort", msgGetRequestUriPort);
  js_export_method("msgGetRequestUriParameters", msgGetRequestUriParameters);
  js_export_method("msgSetRequestUriParameters", msgSetRequestUriParameters);
  js_export_method("msgGetToUser", msgGetToUser);
  js_export_method("msgSetToUser", msgSetToUser);
  js_export_method("msgGetToHostPort", msgGetToHostPort);
  js_export_method("msgGetToHost", msgGetToHost);
  js_export_method("msgSetToHostPort", msgSetToHostPort);
  js_export_method("msgGetToPort", msgGetToPort);
  js_export_method("msgGetFromUser", msgGetFromUser);
  js_export_method("msgSetFromUser", msgSetFromUser);
  js_export_method("msgGetFromHostPort", msgGetFromHostPort);
  js_export_method("msgGetFromHost", msgGetFromHost);
  js_export_method("msgGetFromPort", msgGetFromPort);
  js_export_method("msgSetFromHostPort", msgSetFromHostPort);
  js_export_method("msgGetContactUri", msgGetContactUri);
  js_export_method("msgGetContactParameter", msgGetContactParameter);
  js_export_method("msgGetAuthenticator", msgGetAuthenticator);
  js_export_method("msgSetProperty", msgSetProperty);
  js_export_method("msgGetProperty", msgGetProperty);
  js_export_method("msgSetTransactionProperty", msgSetTransactionProperty);
  js_export_method("msgGetTransactionProperty", msgGetTransactionProperty);
  js_export_method("msgGetSourceAddress", msgGetSourceAddress);
  js_export_method("msgGetSourcePort", msgGetSourcePort);
  js_export_method("msgGetInterfaceAddress", msgGetInterfaceAddress);
  js_export_method("msgGetInterfacePort", msgGetInterfacePort);
  //
  // Request-Line
  //
  js_export_method("requestLineGetMethod", requestLineGetMethod);
  js_export_method("requestLineGetURI", requestLineGetURI);
  js_export_method("requestLineGetVersion", requestLineGetVersion);
  js_export_method("requestLineSetMethod", requestLineSetMethod);
  js_export_method("requestLineSetURI", requestLineSetURI);
  js_export_method("requestLineSetVersion", requestLineSetVersion);

  //
  // Status-Line
  //
  js_export_method("statusLineGetVersion", statusLineGetVersion);
  js_export_method("statusLineSetVersion", statusLineSetVersion);
  js_export_method("statusLineGetStatusCode", statusLineGetStatusCode);
  js_export_method("statusLineSetStatusCode", statusLineSetStatusCode);
  js_export_method("statusLineGetReasonPhrase", statusLineGetReasonPhrase);
  js_export_method("statusLineSetReasonPhrase", statusLineSetReasonPhrase);

  //
  // URI
  //
  js_export_method("uriSetScheme", uriSetScheme);
  js_export_method("uriGetScheme", uriGetScheme);
  js_export_method("uriGetUser", uriGetUser);
  js_export_method("uriSetUserInfo", uriSetUserInfo);
  js_export_method("uriGetPassword", uriGetPassword);
  js_export_method("uriGetHostPort", uriGetHostPort);
  js_export_method("uriSetHostPort", uriSetHostPort);
  js_export_method("uriGetParams", uriGetParams);
  js_export_method("uriSetParams", uriSetParams);
  js_export_method("uriHasParam", uriHasParam);
  js_export_method("uriGetParam", uriGetParam);
  js_export_method("uriGetParamEx", uriGetParamEx);
  js_export_method("uriSetParam", uriSetParam);
  js_export_method("uriSetParamEx", uriSetParamEx);
  js_export_method("uriEscapeUser", uriEscapeUser);
  js_export_method("uriEscapeParam", uriEscapeParam);
  js_export_method("uriGetHeaders", uriGetHeaders);
  js_export_method("uriSetHeaders", uriSetHeaders);
  js_export_method("uriVerify", uriVerify);
  
  //
  // From Processing
  //
  js_export_method("fromGetDisplayName", fromGetDisplayName);
  js_export_method("fromSetDisplayName", fromSetDisplayName);
  js_export_method("fromGetURI", fromGetURI);
  js_export_method("fromSetURI", fromSetURI);
  js_export_method("fromGetHeaderParams", fromGetHeaderParams);
  js_export_method("fromSetHeaderParams", fromSetHeaderParams);
  js_export_method("fromGetHeaderParam", fromGetHeaderParam);
  js_export_method("fromGetHeaderParamEx", fromGetHeaderParamEx);
  js_export_method("fromSetHeaderParam", fromSetHeaderParam);
  js_export_method("fromSetHeaderParamEx", fromSetHeaderParamEx);

  //
  // To Processing
  //
  js_export_method("toGetDisplayName", fromGetDisplayName);
  js_export_method("toSetDisplayName", fromSetDisplayName);
  js_export_method("toGetURI", fromGetURI);
  js_export_method("toSetURI", fromSetURI);
  js_export_method("toGetHeaderParams", fromGetHeaderParams);
  js_export_method("toSetHeaderParams", fromSetHeaderParams);
  js_export_method("toGetHeaderParam", fromGetHeaderParam);
  js_export_method("toGetHeaderParamEx", fromGetHeaderParamEx);
  js_export_method("toSetHeaderParam", fromSetHeaderParam);
  js_export_method("toSetHeaderParamEx", fromSetHeaderParamEx);

  js_export_finalize(); 
}

JS_REGISTER_MODULE(JSSIPParser);



