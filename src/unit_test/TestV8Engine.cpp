#include "gtest/gtest.h"
#include "OSS/OSS.h"
#include "OSS/JS/JSSIPMessage.h"


static char gMainScript[] =
  "function Route(){};"
  "Route.prototype = new RouteProfile();"
  "Route.prototype.routeByDomain = function(domain, target, protocol, port)"
  "{"
  "    if (domain == \"*\" || this.sipMessage.getFromHost() == domain)"
  "    {"
  "      this.sipMessage.setRequestUriHostPort(target);"
  "      this.setTargetAddress(protocol, target, port);"
  "      return true;"
  "    }else"
  "    {"
  "      return false;"
  "    }"
  "    return true;"
  "};"

  "Route.prototype.getRequestTarget = function()"
  "{"
  "    var target = this.sipMessage.getRequestUriHostPort();"
  "    var hostPort = target.split(\":\");"
  "    return hostPort[0];"
  "};"

  "Route.prototype.isRoutable = function()"
  "{"
  "    var domain = this.sipMessage.getProperty(\"local-domain\");"
  "    var target = this.getRequestTarget();"
  "    return this.routeByDomain(domain, target, \"udp\", \"5060\");"
  "};"

  "function Route404(){};"
  "Route404.prototype = new RouteProfile();"
  "Route404.prototype.routeRequest = function()"
  "{"
  "  log_info(\"VERIFIED Route404.isRoutable\");"
  "  this.setRejectReason(\"No route available\");"
  "  this.routeReject();"
  "};"

  "var route = new Route();"
  "var route404 = new Route404();"

  "function handle_request(request)"
  "{"
  "  route.setRequest(request);"
  "  route404.setRequest(request);"

  "  if (route.isRoutable())"
  "  {"
  "    route.sipMessage.setProperty(\"enable-refer-retarget\", \"yes\");"
  "    route.routeRequest();"
  "  }"
  "  else"
  "  {"
  "    route404.routeRequest();"
  "  }"
  "};";


static OSS::SIP::SIPMessage* unwrapRequest(const v8::Arguments& args)
{
  if (args.Length() < 1)
    return 0;
  v8::Handle<v8::Value> obj = args[0];
  if (!obj->IsObject())
    return 0;
  v8::Handle<v8::External> field = v8::Handle<v8::External>::Cast(obj->ToObject()->GetInternalField(0));
  void* ptr = field->Value();
  return static_cast<OSS::SIP::SIPMessage*>(ptr);
}

static std::string jsvalToString(const v8::Handle<v8::Value>& str)
{
  if (!str->IsString())
    return "";
  v8::String::Utf8Value value(str);
  return *value;
}

static v8::Handle<v8::Value> msgSetProperty(const v8::Arguments& args)
{
  if (args.Length() < 3)
    return v8::Boolean::New(false);

  v8::HandleScope scope;
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return v8::Boolean::New(false);


  std::string name = jsvalToString(args[1]);
  std::string value = jsvalToString(args[2]);

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
  OSS::SIP::SIPMessage* pMsg = unwrapRequest(args);
  if (!pMsg)
    return v8::Undefined();

  std::string name = jsvalToString(args[1]);

  if (name.empty())
    return v8::Undefined();

  std::string value;
  pMsg->getProperty(name, value);

  return v8::String::New(value.c_str());
}

static v8::Handle<v8::Value> msgSetTransactionProperty(const v8::Arguments& args)
{
  return v8::Boolean::New(false);
}

static v8::Handle<v8::Value> msgGetTransactionProperty(const v8::Arguments& args)
{
  return v8::Undefined();
}

static void msgRegisterGlobals(OSS::OSS_HANDLE objectTemplate)
{
  v8::Handle<v8::ObjectTemplate>& global = *(static_cast<v8::Handle<v8::ObjectTemplate>*>(objectTemplate));
  global->Set(v8::String::New("msgSetProperty"), v8::FunctionTemplate ::New(msgSetProperty));
  global->Set(v8::String::New("msgGetProperty"), v8::FunctionTemplate ::New(msgGetProperty));
  global->Set(v8::String::New("msgSetTransactionProperty"), v8::FunctionTemplate ::New(msgSetTransactionProperty));
  global->Set(v8::String::New("msgGetTransactionProperty"), v8::FunctionTemplate ::New(msgGetTransactionProperty));
}

TEST(V8Test, test_routing)
{
  using OSS::SIP::CRLF;
  
  std::ostringstream msg;
  msg << "INVITE sip:9001@192.168.0.100 SIP/2.0" << CRLF;
  msg << "To: <sip:9001@192.168.0.100>" << CRLF;
  msg << "From: 9011<sip:9011@192.168.0.100>;tag=6657e067" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.1;branch=001;rport, SIP/2.0/UDP 192.168.0.2;branch=002, SIP/2.0/UDP 192.168.0.3;branch=003" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.4;branch=004" << CRLF;
  msg << "Via: SIP/2.0/UDP 192.168.0.5;branch=005" << CRLF;
  msg << "Call-ID: 885e5e180c04c509" << CRLF;
  msg << "CSeq: 1 INVITE" << CRLF;
  msg << "Contact: <sip:9011@192.168.0.152:9644>" << CRLF;
  msg << "Max-Forwards: 70" << CRLF;
  msg << "Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, NOTIFY, MESSAGE, SUBSCRIBE, INFO" << CRLF;
  msg << "Content-Type: application/sdp" << CRLF;
  msg << "Route: <sip:10.0.0.1;lr>" << CRLF;
  msg << "Route: <sip:10.0.0.2;lr>" << CRLF;
  msg << "Route: <sip:10.0.0.3;lr>" << CRLF;
  msg << "Content-Length: 237" << CRLF;
  msg << CRLF; /// End of headers
  msg << "v=0" << CRLF;
  msg << "o=- 10818229 10818359 IN IP4 192.168.0.152" << CRLF;
  msg << "s=-" << CRLF;
  msg << "c=IN IP4 192.168.0.152" << CRLF;
  msg << "t=0 0" << CRLF;
  msg << "m=audio 35000 RTP/AVP 0 8 101" << CRLF;
  msg << "a=fmtp:101 0-15" << CRLF;
  msg << "a=rtpmap:101 telephone-event/8000" << CRLF;
  msg << "a=sendrecv" << CRLF;
  
  OSS::SIP::SIPMessage::Ptr pMsg(new OSS::SIP::SIPMessage(msg.str()));
  OSS::JS::JSSIPMessage jsMessage("TestV8Engine");
  ASSERT_TRUE(jsMessage.initialize("handle_request", "", gMainScript, msgRegisterGlobals));
  
  std::string property;
  
  pMsg->setProperty("local-domain", "192.168.0.100");
  ASSERT_TRUE(jsMessage.processRequest(pMsg));
  pMsg->getProperty("route-action", property);
  ASSERT_STREQ(property.c_str(), "continue");
  
  
  pMsg->setProperty("local-domain", "192.168.0.200");
  ASSERT_TRUE(jsMessage.processRequest(pMsg));
  pMsg->getProperty("route-action", property);
  ASSERT_STREQ(property.c_str(), "reject");
}
