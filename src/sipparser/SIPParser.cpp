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


#include "OSS/UTL/CoreUtils.h"
#include "OSS/SIP/SIPParser.h"


namespace OSS {
namespace SIP {


SIPParser::SIPParser()
{
}

SIPParser::~SIPParser()
{
}

const std::string & SIPParser::data() const
{
  return _data;
}

void SIPParser::escape(std::string& result, const char* _str, const char* validChars)
{
  static const char * safeChars = "abcdefghijklmnopqrstuvwxyz"
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "0123456789$-_.!*'(),+#";

  int pos = -1;
  char* offSet = const_cast<char*>(_str);
  char* str = const_cast<char*>(_str);
  int len = strlen(str);

  std::string front;
  while ((pos += (int)(1+strspn(&str[pos+1], validChars == 0 ? safeChars : validChars))) < len)
  {
    std::string escaped;
    if (!OSS::string_sprintf_string<4>(escaped, "%%%02X", static_cast<const unsigned char>(str[pos])))
    {
      front = str;
      return;
    }
    front += std::string(offSet, str + pos );
    front += escaped;
    offSet = const_cast<char*>(str) + pos + 1;
  }
  front += std::string(offSet, str + pos );
  result = front;
}

void SIPParser::unescape(std::string& result, const char* str)
{
  result = str;
  int pos = -1;

  while (((size_t)(pos = (int)result.find('%', pos+1))) != std::string::npos) 
  {
    int digit1 = result[pos+1];
    int digit2 = result[pos+2];
    if (isxdigit(digit1) && isxdigit(digit2)) 
    {
      result[pos] = (char)((isdigit(digit2) ? (digit2-'0') : (toupper(digit2)-'A'+10)) +((isdigit(digit1) ? (digit1-'0') : (toupper(digit1)-'A'+10)) << 4));
      result.erase(pos+1, 2);
    }
  }
}

void SIPParser::unquoteString(std::string& quotedString)
{
  OSS::string_trim(quotedString);
  if (quotedString[0] == '"' && quotedString[quotedString.size() - 1] == '"')
  {
    quotedString = quotedString.substr(1, quotedString.size() - 2);
  }
}

void SIPParser::enquoteString(std::string& quotedString)
{
  OSS::string_trim(quotedString);
  if (quotedString[0] != '"' && quotedString[quotedString.size() - 1] != '"')
  {
    quotedString = "\"" + quotedString + "\"";
  }
}

bool SIPParser::isChar(int c)
{
  return c >= 0 && c <= 127;
}

bool SIPParser::isCtl(int c)
{
  return (c >= 0 && c <= 31) || c == 127;
}

bool SIPParser::isDigit(int c)
{
  return c >= '0' && c <= '9';
}

void SIPParser::getReasonPhrase( 
  int statusCode,
  std::string& reasonPhrase
)
{
  if (!reasonPhrase.empty())
    return;

  static const char* s100("Trying");
  static const char* s180("Ringing");
  static const char* s181("Call Is Being Forwarded");
  static const char* s182("Queued");
  static const char* s183("Session Progress");
  static const char* s200("OK");
  static const char* s202("Accepted");
  static const char* s300("Multiple Choices");
  static const char* s301("Moved Permanently");
  static const char* s302("Moved Temporarily");
  static const char* s305("Use Proxy");
  static const char* s380("Alternative Service");
  static const char* s400("Bad Request");
  static const char* s401("Unauthorized");
  static const char* s402("Payment Required");
  static const char* s403("Forbidden");
  static const char* s404("Not Found");
  static const char* s405("Method Not Allowed");
  static const char* s406("Not Acceptable");
  static const char* s407("Proxy Authentication Required");
  static const char* s408("Request Timeout");
  static const char* s409("Conflict");
  static const char* s410("Gone");
  static const char* s411("Length Required");
  static const char* s412("Conditional Request Failed");
  static const char* s413("Request Entity Too Large");
  static const char* s414("Request-URI Too Large");
  static const char* s415("Unsupported Media Type");
  static const char* s416("Unsupported Uri Scheme");
  static const char* s420("Bad Extension");
  static const char* s422("Session Interval Too Small" );
  static const char* s423("Interval Too Short");
  static const char* s480("Temporarily not available");
  static const char* s481("Call Leg/Transaction Does Not Exist");
  static const char* s482("Loop Detected");
  static const char* s483("Too Many Hops");
  static const char* s484("Address Incomplete");
  static const char* s485("Ambiguous");
  static const char* s486("Busy Here");
  static const char* s487("Request Cancelled");
  static const char* s488("Not Acceptable Here");
  static const char* s489("Bad Event");
  static const char* s491("Request Pending");
  static const char* s500("Internal Server Error");
  static const char* s501("Not Implemented");
  static const char* s502("Bad Gateway");
  static const char* s503("Service Unavailable");
  static const char* s504("Gateway Time-out");
  static const char* s505("SIP Version not supported");
  static const char* s600("Busy Everywhere");
  static const char* s603("Decline");
  static const char* s604("Does not exist anywhere");
  static const char* s606("Not Acceptable");

  static const char* s999( "Reason Unknown" );

  int i = statusCode / 100;

  if (i == 1)
  {				/* 1xx  */
    if (statusCode == 100)
      reasonPhrase = s100;
    else if (statusCode == 180)
      reasonPhrase = s180;
    else if (statusCode == 181)
      reasonPhrase = s181;
    else if (statusCode == 182)
      reasonPhrase = s182;
    else if (statusCode == 183)
      reasonPhrase = s183;
    else
      reasonPhrase = s100;
  }else if (i == 2)
  {	if (statusCode == 200)
      reasonPhrase = s200;
    else if (statusCode == 202)
      reasonPhrase = s202;
    else
      reasonPhrase = s200;
  }else if (i == 3)
  {				/* 3xx */
    if (statusCode == 300)
      reasonPhrase = s300;
    else if (statusCode == 301)
      reasonPhrase = s301;
    else if (statusCode == 302)
      reasonPhrase = s302;
    else if (statusCode == 305)
      reasonPhrase = s305;
    else if (statusCode == 380)
      reasonPhrase = s380;
    else
      reasonPhrase = s300;
  }else if (i == 4)
  {				/* 4xx */
    if (statusCode == 400)
      reasonPhrase = s400;
    else if (statusCode == 401)
      reasonPhrase = s401;
    else if (statusCode == 402)
      reasonPhrase = s402;
    else if (statusCode == 403)
      reasonPhrase = s403;
    else if (statusCode == 404)
      reasonPhrase = s404;
    else if (statusCode == 405)
      reasonPhrase = s405;
    else if (statusCode == 406)
      reasonPhrase = s406;
    else if (statusCode == 407)
      reasonPhrase = s407;
    else if (statusCode == 408)
      reasonPhrase = s408;
    else if (statusCode == 409)
      reasonPhrase = s409;
    else if (statusCode == 410)
      reasonPhrase = s410;
    else if (statusCode == 411)
      reasonPhrase = s411;
    else if (statusCode == 412)
      reasonPhrase = s412;
    else if (statusCode == 413)
      reasonPhrase = s413;
    else if (statusCode == 414)
      reasonPhrase = s414;
    else if (statusCode == 415)
      reasonPhrase = s415;
    else if (statusCode == 416)
      reasonPhrase = s416;
    else if (statusCode == 420)
      reasonPhrase = s420;
    else if (statusCode == 422)
      reasonPhrase = s422;
    else if (statusCode == 423)
      reasonPhrase = s423;
    else if (statusCode == 480)
      reasonPhrase = s480;
    else if (statusCode == 481)
      reasonPhrase = s481;
    else if (statusCode == 482)
      reasonPhrase = s482;
    else if (statusCode == 483)
      reasonPhrase = s483;
    else if (statusCode == 484)
      reasonPhrase = s484;
    else if (statusCode == 485)
      reasonPhrase = s485;
    else if (statusCode == 486)
      reasonPhrase = s486;
    else if (statusCode == 487)
      reasonPhrase = s487;
    else if (statusCode == 488)
      reasonPhrase = s488;
    else if (statusCode == 489)
      reasonPhrase = s489;
    else if (statusCode == 491)
      reasonPhrase = s491;
    else
      reasonPhrase = s400;
  }else if (i == 5)
  {				/* 5xx */
    if (statusCode == 500)
      reasonPhrase = s500;
    else if (statusCode == 501)
      reasonPhrase = s501;
    else if (statusCode == 502)
      reasonPhrase = s502;
    else if (statusCode == 503)
      reasonPhrase = s503;
    else if (statusCode == 504)
      reasonPhrase = s504;
    else if (statusCode == 505)
      reasonPhrase = s505;
    else
      reasonPhrase = s500;
  }else if (i == 6)
  {				/* 6xx */
    if (statusCode == 600)
      reasonPhrase = s600;
    else if (statusCode == 603)
      reasonPhrase = s603;
    else if (statusCode == 604)
      reasonPhrase = s604;
    else if (statusCode == 606)
      reasonPhrase = s606;
    else
      reasonPhrase = s600;
  }else
  {
    reasonPhrase = s999;
  }
}

std::string SIPParser::createBranchString()
{
  std::stringstream strm;
  strm << "z9hG4bK" << OSS::string_create_uuid();
  return strm.str();
}

std::string SIPParser::createTagString()
{
  return OSS::string_create_uuid();
}

} } //OSS::SIP

