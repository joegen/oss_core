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
#include "OSS/SIP/SIPURI.h"
#include "OSS/ABNF/ABNFSIPURI.h"
#include "OSS/ABNF/ABNFSIPScheme.h"
#include "OSS/ABNF/ABNFSIPUserInfo.h"
#include "OSS/ABNF/ABNFSIPHostPort.h"
#include "OSS/ABNF/ABNFSIPURIParameters.h"
#include "OSS/ABNF/ABNFSIPURIHeaders.h"
#include "OSS/ABNF/ABNFSIPUser.h"
#include "OSS/ABNF/ABNFSIPPassword.h"


namespace OSS{
namespace SIP{


using namespace OSS::ABNF;
static ABNF_SIP_scheme schemeParser;
static ABNF_SIP_userinfo userInfoParser;
static ABNF::ABNF_SIP_pvalue pvalueParser;
static ABNF::ABNF_SIP_pname pnameParser;
static ABNFEvaluate<ABNF_SIP_pname> pnameVerify;
static ABNFEvaluate<ABNF_SIP_pvalue> pvalueVerify;
static ABNFEvaluate<ABNF_SIP_URI> uriVerify;


static ABNFEvaluate< ABNFLRSequence2<
    ABNF_SIP_user, 
    ABNFLROptional< ABNFLRSequence2<ABNFCharComparison<':'>, ABNF_SIP_password > 
  > > > userInfoVerify;


typedef ABNFLRSequence2<
  ABNFLRSequence3<ABNF_SIP_scheme, ABNFCharComparison<':'>, ABNFLROptional<ABNF_SIP_userinfo> >, 
  ABNF_SIP_hostport> HostPortParser;
static HostPortParser hostPortParser;
static ABNFEvaluate<ABNF_SIP_hostport> hostPortVerify;

typedef ABNFLRSequence2<
  HostPortParser,
  ABNFLROptional<ABNF_SIP_uri_parameters> > URIParametersParser;
static URIParametersParser uriParametersParser;

typedef ABNFLRSequence2<URIParametersParser, ABNFLROptional<ABNF_SIP_headers> > URIHeadersParser;
static URIHeadersParser uriHeadersParser;
ABNFEvaluate<ABNF_SIP_headers> headersVerify;

SIPURI::SIPURI()
{
  _data = "sip:invalid";
}

SIPURI::SIPURI(const std::string& uri)
{
  _data = uri;
}

SIPURI::SIPURI(const SIPURI& uri) 
{
  _data = uri._data; 
}

SIPURI& SIPURI::operator = (const SIPURI& uri)
{
  SIPURI clonable(uri);
  swap(clonable);
  return *this;
}

SIPURI& SIPURI::operator = (const std::string& uri)
{
  SIPURI clonable(uri);
  swap(clonable);
  return *this;
}

void SIPURI::swap(SIPURI& uri)
{
  std::swap(_data, uri._data);
}

std::string SIPURI::getScheme() const
{
  std::string value;
  getScheme(_data, value);
  return value;
}

bool SIPURI::getScheme(const std::string& uri, std::string& value)
{
  char * offSet = schemeParser.parse(uri.c_str());
  if( offSet == uri.c_str())
    return false;
  value = std::string(uri.c_str(), (const char*)offSet);
  return true;
}

bool SIPURI::setScheme(const char* scheme)
{
  return setScheme(_data, scheme);
}

bool SIPURI::setScheme(std::string& uri, const char* scheme)
{
  char * offSet = schemeParser.parse(uri.c_str());
  if( offSet == uri.c_str())
    return false;

  std::string front = scheme; 
  front += offSet;
  uri = front;
  return true;
}

std::string SIPURI::getUser() const
{
  std::string value;
  getUser(_data, value);
  return value;
}

bool SIPURI::getUser(const std::string& uri, std::string& value)
{
  char* offSet = ABNF::findNextIterFromString(":", uri.c_str());
  if (offSet == uri.c_str())
    return false;
  ABNFTokens tokens;
  userInfoParser.parseTokens(offSet, tokens);
  if (tokens.size() != 3)
    return false;
  value = tokens[0];
  return !value.empty();
}

bool SIPURI::setUserInfo(const char* userInfo)
{
  return setUserInfo(_data, userInfo);
}

bool SIPURI::setUserInfo(std::string& uri, const char* userInfo)
{
  if (userInfo != 0)
  {
    if (!userInfoVerify(userInfo))
      throw OSS::SIP::SIPParserException("ABNF Syntax Exception");
  }

  char* schemeOffSet = ABNF::findNextIterFromString(":", uri.c_str());
  if (schemeOffSet == uri.c_str())
    return false;

  std::string front(uri.c_str(), (const char*)schemeOffSet);
  if (userInfo != 0)
  {
    front += userInfo;
    front += "@";
  }
  char* tailOffSet = userInfoParser.parse(schemeOffSet);
  if (tailOffSet == schemeOffSet)
  {
    front += schemeOffSet;
  }
  else
  {
    front += tailOffSet;
  }
  uri = front;
  return true;
}

std::string SIPURI::getPassword() const
{
  std::string value;
  getPassword(_data, value);
  return value;
}

bool SIPURI::getPassword(const std::string& uri, std::string& value)
{
  char* offSet = ABNF::findNextIterFromString(":", uri.c_str());
  if (offSet == uri.c_str())
    return false;
  ABNFTokens tokens;
  userInfoParser.parseTokens(offSet, tokens);
  if (tokens.size() != 3)
    return false;
  if (!tokens[1].empty())
    value = tokens[1].c_str() + 1;
  return !value.empty();
}

bool SIPURI::getHostPort(std::string& host, unsigned short& port) const
{
  std::string hostPort = getHostPort();
  if (hostPort.empty())
    return false;

  std::vector<std::string> tokens = OSS::string_tokenize(hostPort, ":");
  if (tokens.size() <= 1)
  {
    host = hostPort;
    port = 0;
    return true;
  }

  host = tokens[0];
  port = OSS::string_to_number<unsigned short>(tokens[1].c_str());
  return true;
}

std::string SIPURI::getHostPort() const
{
  std::string value;
  getHostPort(_data, value);
  return value;
}

bool SIPURI::getHostPort(const std::string& uri, std::string& value)
{
  ABNFTokens tokens;
  hostPortParser.parseTokens(uri.c_str(), tokens);
  if (tokens.size() != 2)
    return false;
  value = tokens[1];
  return true;
}

std::string SIPURI::getHost() const
{
  std::string host;
  getHost(_data, host);
  return host;
}

bool SIPURI::getHost(const std::string& uri, std::string& host)
{
  std::string hostPort;
  
  if (!SIPURI::getHostPort(uri, hostPort) || hostPort.empty())
    return false;

  std::vector<std::string> tokens = OSS::string_tokenize(hostPort, ":");
  if (tokens.size() <= 1)
  {
    host = hostPort;
  }
  else
  {
    host = tokens[0];
  }
  return true;
}

std::string SIPURI::getPort() const
{
  std::string port;
  getPort(_data, port);
  return port;
}

bool SIPURI::getPort(const std::string& uri, std::string& port)
{
  std::string hostPort;

  if (!SIPURI::getHostPort(uri, hostPort) || hostPort.empty())
    return false;

  std::vector<std::string> tokens = OSS::string_tokenize(hostPort, ":");
  if (tokens.size() == 2)
  {
    port = tokens[1];
    return true;
  }
  return false;
}
    /// Returns the port

bool SIPURI::setHost(const char* host)
{
  std::string port;
  if (!SIPURI::getPort(_data, port))
  {
    return setHostPort(host);
  }
  else
  {
    std::ostringstream hostPort;
    hostPort << host << ":" << port;
    return setHostPort(hostPort.str().c_str());
  }
}

bool SIPURI::setPort(const char* port)
{
  std::string host;
  host = getHost();
  std::ostringstream hostPort;
  if (port && strcmp(port, "0"))
  { 
    hostPort << host << ":" << port;
  }
  else
  {
    hostPort << host;
  }
  return setHostPort(hostPort.str().c_str());
}

bool SIPURI::setHostPort(const char* hostPort)
{
  return setHostPort(_data, hostPort);
}

bool SIPURI::setHostPort(const OSS::Net::IPAddress& hostPort)
{
  std::string host = hostPort.toIpPortString();
  return setHostPort(_data, host.c_str());
}

std::string SIPURI::getIdentity(bool includeScheme) const
{
  std::string identity;
  getIdentity(_data, identity, includeScheme);
  return identity;
}

bool SIPURI::getIdentity(const std::string& uri, std::string& identity, bool includeScheme)
{
  std::string scheme, user, hostPort;
  SIPURI::getUser(uri, user);

  if (!SIPURI::getScheme(uri, scheme))
    return false;

  if (!SIPURI::getHostPort(uri, hostPort))
    return false;

  if (includeScheme)
  {
    identity = scheme;
    identity += ":";
  }
  else
  {
    identity = "";
  }
  
  if (!user.empty())
  {
    identity += user;
    identity += "@";
  }
  identity += hostPort;
  
  return true;
}

bool SIPURI::setHostPort(std::string& uri, const char* hostPort)
{
  if (!hostPortVerify(hostPort))
    throw OSS::SIP::SIPParserException("ABNF Syntax Exception");

  ABNFTokens tokens;
  char* tailOffSet = hostPortParser.parseTokens(uri.c_str(), tokens);
  if (tokens.size() != 2)
    return false;
  std::string front = tokens[0];
  front += hostPort;
  front += tailOffSet;
  uri = front;
  return true;
}

std::string SIPURI::getParams() const
{
  std::string params;
  getParams(_data, params);
  return params;
}

bool SIPURI::getParams(const std::string& uri, std::string& params)
{
  ABNFTokens tokens;
  uriParametersParser.parseTokens(uri.c_str(), tokens);
  if (tokens.size() != 2)
    return false;
  params = tokens[1];
  return true;
}

bool SIPURI::setParams(const std::string& params)
{
  return setParams(_data, params);
}

bool SIPURI::setParams(std::string& uri, const std::string& params)
{
  static ABNFSIPURIHeaders headersParser;

  char* hostPortOffSet = hostPortParser.parse(uri.c_str());
  if (hostPortOffSet == uri.c_str())
    return false;

  std::string front(uri.c_str(), (const char*)hostPortOffSet);

  char* uriParamsOffSet = uriParametersParser.parse(hostPortOffSet);
  if (uriParamsOffSet == hostPortOffSet)
    front += params;
 
  char* uriHeadersOffSet = headersParser.parse(uriParamsOffSet);
  if (uriHeadersOffSet > uriParamsOffSet)
  {
    std::string headers(uriParamsOffSet, uriHeadersOffSet);
    front += headers;
  }

  uri = front;
  
  return true;
}

std::string SIPURI::getParam(const char* paramName) const
{
  std::string paramValue;
  getParam(_data, paramName, paramValue);
  return paramValue;
}

bool SIPURI::getParam(const std::string& uri, const char* paramName, std::string& paramValue)
{
  std::string params;
  getParams(uri, params);
  if (params.empty())
    return false;
  
  return getParamEx(params, paramName, paramValue);
}

bool SIPURI::getParamEx(const std::string& params, const char* paramName, std::string& paramValue)
{
  std::string k = paramName;
  if (k != "lr")
    k += "=";
  std::string key = ";" + k;

  char* startIter = ABNF::findNextIterFromString(key, params.c_str());
  if (startIter == params.c_str())
  {
    return false;
  }

  if (key == ";lr")
    return true;

  char* newIter = pvalueParser.parse(startIter);
  if (newIter == startIter)
    return false;
  
  paramValue = std::string(startIter, newIter);
  return true;
}

bool SIPURI::hasParam(const char* paramName) const
{
  return hasParam(_data, paramName);
}

bool SIPURI::hasParam(const std::string& uri, const char* paramName)
{
  std::string param;
  return SIPURI::getParam(uri, paramName, param);
}

bool SIPURI::setParam(const char* paramName, const char* paramValue)
{
  return setParam(_data, paramName, paramValue);
}

bool SIPURI::setParam(std::string& uri, const char* paramName, const char* paramValue)
{
  std::string params;
  getParams(uri, params);
  
  if (!setParamEx(params, paramName, paramValue))
    return false;

  return setParams(uri, params);
}

bool SIPURI::setParamEx(std::string& params, const char* paramName, const char* paramValue)
{
  if (!pnameVerify(paramName) || !pvalueVerify(paramValue))
    throw OSS::SIP::SIPParserException("ABNF Syntax Exception");
  
  std::string k = paramName;
  boost::to_lower(k);
  std::string key = ";" + k + "=";
  
  char* offSet = ABNF::findNextIterFromString(key, params.c_str());
  if (offSet == params.c_str())
  {
    std::ostringstream strm; 
    if (::strcasecmp(paramName, "lr") != 0)
      strm << ";" << paramName << "=" << paramValue;
    else
      strm << ";lr";
    params += strm.str();
    return true;
  }

  std::string front(params.c_str(), (const char*)offSet);
  if(front.at(front.size() - 1) != '=')
    front += "=";
  front += paramValue;

  char* tailOffSet = ABNF::findNextIterFromString(";", offSet);
  if (tailOffSet != offSet)
  {
    tailOffSet--;
    front += tailOffSet;
  }

  params = front;
  return true;
}

void SIPURI::escapeUser(std::string& result, const char* user)
{
  /// unreserved / escaped / user-unreserved
  /// mark = "-" / "_" / "." / "!" / "~" / "*" / "'" / "(" / ")"
  /// user-unreserved = "&" / "=" / "+" / "$" / "," / ";" / "?" / "/"
  static const char * safeChars = "abcdefghijklmnopqrstuvwxyz"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "0123456789"
                                  "-_.!~*'()" // mark
                                  "&=+$,;?/"; // user-unreserved
  SIPParser::escape(result, user, safeChars);
}

void SIPURI::escapeParam(std::string& result, const char* param)
{
  static const char * safeChars = "abcdefghijklmnopqrstuvwxyz"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "0123456789"
                                  "-_.!~*'()" // mark
                                  "[]/:&+$"; // param-unreserved
  SIPParser::escape(result, param, safeChars);
}

std::string SIPURI::getHeader(const char* h)
{
  std::string header = h;
  OSS::string_to_lower(header);
  std::string headers;
  std::string value;
  getHeaders(_data, headers);
  if (!headers.empty())
  {
    if (headers.at(0) == '?')
      headers = headers.substr(1);
    
    std::vector<std::string> tokens = OSS::string_tokenize(headers, "&");
    if (tokens.size() == 1)
      value = headers;
    else
    {
      for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
      {
        std::string v = iter->substr(0, header.size());
        if (v == header)
        {
          value = *iter;
          break;
        }
      }
    }
  }
  size_t offSet = header.size() + 1;
  if (value.size() > offSet)
    return value.substr(offSet);
  return std::string();
}

bool SIPURI::getHeaders(SIPURI::header_tokens& tokens) const
{
  std::string headers;
  getHeaders(_data, headers);
  if (headers.empty())
    return false;

  if (headers.at(0) == '?')
    headers = headers.substr(1);

  std::vector<std::string> elements = OSS::string_tokenize(headers, "&");
  if (elements.size() == 1)
  {
    std::string header = headers;
    size_t nameBound = header.find_first_of('=');
    if (nameBound == std::string::npos)
    {
      return false;
    }
    std::string name = header.substr(0,nameBound);
    std::string value = header.substr(nameBound + 1);
    boost::trim(name);
    boost::trim(value);
    if (!name.empty() && !value.empty())
    {
      tokens[name] = value;
    }
  }
  else
  {
    for (std::vector<std::string>::iterator iter = elements.begin(); iter != elements.end(); iter++)
    {
      std::string header = *iter;
      size_t nameBound = header.find_first_of('=');
      if (nameBound == std::string::npos)
      {
        return false;
      }
      std::string name = header.substr(0,nameBound);
      std::string value = header.substr(nameBound + 1);
      boost::trim(name);
      boost::trim(value);
      if (!name.empty() && !value.empty())
      {
        tokens[name] = value;
      }
    }
  }
  
  return tokens.size() != 0;
}

std::string SIPURI::getHeaders() const
{
  std::string headers;
  getHeaders(_data, headers);
  return headers;
}

bool SIPURI::getHeaders(const std::string& uri, std::string& headers)
{
  //typedef ABNFLRSequence2<URIParametersParser, ABNF_SIP_headers> URIHeadersParser;
  //static URIHeadersParser uriHeadersParser;
  //ABNFEvaluate<URIHeadersParser> headersVerify;
  ABNFTokens tokens;
  uriHeadersParser.parseTokens(uri.c_str(), tokens);
  if (tokens.size() == 2)
  {
    headers = tokens[1];
    return true;
  }
  return false;
}

bool SIPURI::setHeaders(const std::string& headers)
{
  return setHeaders(_data, headers);
}

bool SIPURI::setHeaders(std::string& uri, const std::string& headers)
{
  if (!headersVerify(headers.c_str()))
    throw OSS::SIP::SIPParserException("ABNF Syntax Exception");
  char* paramsOffSet = uriParametersParser.parse(uri.c_str());
  if (paramsOffSet == uri.c_str())
    return false;
  std::string front(uri.c_str(), (const char*)paramsOffSet);
  front += headers;
  uri = front;
  return true;
}

bool SIPURI::verify() const
{
  return verify(_data.c_str());
}

bool SIPURI::verify(const char* uri)
{
  return uriVerify(uri);
}


}} // OSS::SIP

