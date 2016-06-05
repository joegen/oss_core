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


#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/ABNF/ABNFSIPFromSpec.h"
#include "OSS/ABNF/ABNFSIPNameAddr.h"
#include "OSS/ABNF/ABNFSIPURIParameters.h"
#include "OSS/SIP/SIPParserException.h"


using namespace OSS::ABNF;
static ABNFSIPFromSpec fromSpecParser;
static ABNFSIPNameAddr nameAddrParser;
static ABNF_SIP_addr_spec addrSpecParser;
static ABNF_SIP_URI uriParser;
static ABNFWhileNot<ABNF_SIP_LAQUOT, false> laquotFinder_0;
static ABNFWhileNot<ABNF_SIP_LAQUOT, true> laquotFinder_1;
static ABNFWhileNot<ABNF_SIP_RAQUOT, false> raquotFinder_0;
static ABNFWhileNot<ABNF_SIP_RAQUOT, true> raquotFinder_1;
static ABNFWhileNot<ABNFSIPURIParameters> uriParamsFinder;
static ABNF_SIP_pvalue pvalueParser;
static ABNF_SIP_pname pnameParser;
static ABNFEvaluate<ABNF_SIP_pname> pnameVerify;
static ABNFEvaluate<ABNF_SIP_pvalue> pvalueVerify;

namespace OSS {
namespace SIP {


void check_empty(SIPFrom* header)
{
  if (header->data().empty())
  {
    header->data() = SIPURI::EMPTY_URI;
  }
}

SIPFrom::SIPFrom()
{
  check_empty(this);
}

SIPFrom::SIPFrom(const std::string& from)
{
  _data = from;
}

SIPFrom::SIPFrom(const SIPFrom& from)
{
  _data = from._data;
}

SIPFrom::~SIPFrom()
{
}

SIPFrom& SIPFrom::operator = (const std::string& from)
{
  _data = from;
  return *this;
}

SIPFrom& SIPFrom::operator = (const SIPFrom& from)
{
  SIPFrom clonable(from);
  swap(clonable);
  return *this;
}

void SIPFrom::swap(SIPFrom& from)
{
  std::swap(_data, from._data);
}

std::string SIPFrom::getDisplayName() const
{
  std::string displayName;
  getDisplayName(_data, displayName);
  return displayName;
}

bool SIPFrom::getDisplayName(const std::string& from, std::string& displayName)
{
  char* offSet = laquotFinder_0.parse(from.c_str());
  if (offSet == from.c_str())
    return false;
  displayName = std::string(from.c_str(), (const char*)offSet);
  return true;
}

bool SIPFrom::setDisplayName(const char* displayName)
{
  check_empty(this);
  return setDisplayName(_data, displayName);
}

static bool display_name_needs_quote(const char* displayName)
{
  size_t len = strlen(displayName);
  if (!len)
  {
    return false;
  }
  
  if (displayName[0] == '"')
  {
    return false;
  }
  
  for (size_t i = 0; i < len; i++)
  {
    if (isspace(displayName[i]) != 0)
    {
      return true;
    }
  }
  
  return false;
}

bool SIPFrom::setDisplayName(std::string& from, const char* displayName)
{
  ABNFTokens fsTokens;
  fromSpecParser.parseTokens(from.c_str(), fsTokens);
  if (fsTokens.size() != 2) 
    return false;
  
  bool empty = (strlen(displayName) == 0);
  
  ABNFTokens naTokens;
  char* offSet = nameAddrParser.parseTokens(fsTokens[0].c_str(), naTokens);
  if (offSet == fsTokens[0].c_str() || naTokens.size() != 4)
  {
    if (!display_name_needs_quote(displayName))
    {
      from = displayName;
    }
    else
    {
      std::ostringstream quoted;
      quoted << "\"" << displayName << "\"";
      from = quoted.str();
    }
    
    if (!empty)
    {
      from += " <";
    }
    else
    {
      from += "<";
    }
    
    char* paramsOffSet = uriParamsFinder.parse(fsTokens[0].c_str());
    if (paramsOffSet == fsTokens[0].c_str())
    {
      from += fsTokens[0];
      from += ">";
      from += fsTokens[1];
      return true;
    }
    std::string uriNoParams(fsTokens[0].c_str(), (const char*)paramsOffSet);
    from += uriNoParams;
    from += ">";
    from += paramsOffSet;

    return true;
  }

  if (!display_name_needs_quote(displayName))
  {
    from = displayName;
  }
  else
  {
    std::ostringstream quoted;
    quoted << "\"" << displayName << "\"";
    from = quoted.str();
  }
  
  if (!empty)
  {
    from += " <";
  }
  else
  {
    from += "<";
  }
  from += naTokens[2];
  from += ">";
  from += fsTokens[1];

  return true;
}

std::string SIPFrom::getURI() const
{
  std::string uri;
  getURI(_data, uri);
  return uri;
}

bool SIPFrom::getURI(const std::string& from, std::string& uri)
{
  char* laQuotOffSet = laquotFinder_1.parse(from.c_str());
  if (laQuotOffSet == from.c_str())
  {
    //uri params is considered as header params
    ABNFTokens fsTokens;
    fromSpecParser.parseTokens(from.c_str(), fsTokens);
    if (fsTokens.size() != 2) 
      return false;
    char* addrSpecOffSet = addrSpecParser.parse(fsTokens[0].c_str());
    if (addrSpecOffSet == fsTokens[0].c_str())
      return false;

    std::string addrSpec(fsTokens[0].c_str(), (const char*)addrSpecOffSet);
    char* paramsOffSet = uriParamsFinder.parse(addrSpec.c_str());
    if (paramsOffSet == addrSpec.c_str())
    {
      uri = addrSpec;
      return true;
    }
    else
    {
      uri = std::string(addrSpec.c_str(), (const char*)paramsOffSet);
      return true;
    }
  }
  else
  {
    char* raQuotOffSet = raquotFinder_0.parse(laQuotOffSet);
    if (raQuotOffSet == laQuotOffSet)
      return false;
    uri = std::string(laQuotOffSet, raQuotOffSet);
    return true;
  }
  return true;
}


bool SIPFrom::setURI(const char* uri)
{
  check_empty(this);
  return setURI(_data, uri);
}

bool SIPFrom::setURI(std::string& from, const char* uri)
{
  char* laQuotOffSet = laquotFinder_1.parse(from.c_str());
  if (laQuotOffSet == from.c_str())
  {
    //
    // uri params is considered as header params if
    // there is no angle bracket enclosure
    //
    ABNFTokens fsTokens;
    fromSpecParser.parseTokens(from.c_str(), fsTokens);
    if (fsTokens.size() != 2) 
      return false;

    char* addrSpecOffSet = addrSpecParser.parse(fsTokens[0].c_str());
    if (addrSpecOffSet == fsTokens[0].c_str())
      return false;

    std::string addrSpec(fsTokens[0].c_str(), (const char*)addrSpecOffSet);
    char* paramsOffSet = uriParamsFinder.parse(addrSpec.c_str());
    if (paramsOffSet == addrSpec.c_str())
    {
      from = uri;
      return true;
    }
    else
    {
      std::string headerParams = paramsOffSet;
      from = uri;
      from += headerParams;
      return true;
    }
  }
  else
  {
    ABNFTokens fsTokens;
    fromSpecParser.parseTokens(from.c_str(), fsTokens);
    if (fsTokens.size() != 2) 
      return false;
    /// first check if the uri has LQUOT/RQUOT enclosure
    bool hasEnclosure = laquotFinder_1.parse(uri) != uri;
    if (hasEnclosure)
    {
      from = uri;
      from += fsTokens[1];
      return true;
    }
    else
    {
      char* raQuotOffSet = raquotFinder_0.parse(laQuotOffSet);
      if (raQuotOffSet == laQuotOffSet)
        return false;
      std::string headerParams = raQuotOffSet;
      from = std::string(from.c_str(), (const char*)laQuotOffSet);
      from += uri;
      from += headerParams;
      return true;
    }
  }
  return false;
}

bool SIPFrom::setUser(const char* user)
{
  check_empty(this);
  return setUser(_data, user);
}

bool SIPFrom::setUser(std::string& from, const char* user)
{
  std::string uri;
  if (!getURI(from, uri))
    return false;
  
  if (!SIPURI::setUserInfo(uri, user))
    return false;
  return setURI(from, uri.c_str());
}

std::string SIPFrom::getUser() const
{
  std::string user;
  getUser(_data, user);
  return user;
}

bool SIPFrom::getUser(const std::string& from, std::string& user)
{
  std::string uri;
  if (!getURI(from, uri))
    return false;
  return SIPURI::getUser(uri, user);
}

bool SIPFrom::setHostPort(const char* hostPort)
{
  check_empty(this);
  return setHostPort(_data, hostPort);
}

bool SIPFrom::setHostPort(std::string& from, const char* hostPort)
{
  std::string uri;
  if (!getURI(from, uri))
    return false;

  if (!SIPURI::setHostPort(uri, hostPort))
    return false;
  return setURI(from, uri.c_str());
}

std::string SIPFrom::getHostPort() const
{
  std::string hostPort;
  getHostPort(_data, hostPort);
  return hostPort;
}

bool SIPFrom::getHostPort(const std::string& from, std::string& hostPort)
{
  std::string uri;
  if (!getURI(from, uri))
    return false;
  return SIPURI::getHostPort(uri, hostPort);
}

std::string SIPFrom::getHost() const
{
  std::string host;
  getHost(_data, host);
  return host;
}

bool SIPFrom::getHost(const std::string& from, std::string& host)
{
  std::string uri;
  if (!getURI(from, uri))
    return false;
  return SIPURI::getHost(uri, host);
}

std::string SIPFrom::getAor(bool includeScheme) const
{
  std::string uri;
  if (!getURI(_data, uri))
    return "";
  std::string aor;
  SIPURI::getIdentity(uri, aor, includeScheme);
  return aor;
}

std::string SIPFrom::getHeaderParams() const
{
  std::string headerParams;
  getHeaderParams(_data, headerParams);
  return headerParams;
}

bool SIPFrom::getHeaderParams(const std::string& from, std::string& headerParams)
{
  char* raQuotOffSet = raquotFinder_1.parse(from.c_str());
  if (raQuotOffSet != from.c_str())
  {
    headerParams = raQuotOffSet;
    return true;
  }
  else
  {
    ABNFTokens fsTokens;
    fromSpecParser.parseTokens(from.c_str(), fsTokens);
    if (fsTokens.size() != 2) 
      return false;
    char* addrSpecOffSet = addrSpecParser.parse(fsTokens[0].c_str());
    if (addrSpecOffSet == fsTokens[0].c_str())
      return false;

    std::string addrSpec(fsTokens[0].c_str(), (const char*)addrSpecOffSet);
    char* paramsOffSet = uriParamsFinder.parse(addrSpec.c_str());
    if (paramsOffSet == addrSpec.c_str())
      return false;

    headerParams = paramsOffSet;
  }
  return true;
}

bool SIPFrom::setHeaderParams(const char* headerParams)
{
  check_empty(this);
  return setHeaderParams(_data, headerParams);
}

bool SIPFrom::setHeaderParams(std::string& from, const char* headerParams)
{
  char* raQuotOffSet = raquotFinder_1.parse(from.c_str());
  if (raQuotOffSet != from.c_str())
  {
    from = std::string(from.c_str(), (const char*)raQuotOffSet);
    from += headerParams;
    return true;
  }
  else
  {
    ABNFTokens fsTokens;
    fromSpecParser.parseTokens(from.c_str(), fsTokens);
    if (fsTokens.size() != 2) 
      return false;
    char* addrSpecOffSet = addrSpecParser.parse(fsTokens[0].c_str());
    if (addrSpecOffSet == fsTokens[0].c_str())
      return false;

    std::string addrSpec(fsTokens[0].c_str(), (const char*)addrSpecOffSet);
    char* paramsOffSet = uriParamsFinder.parse(addrSpec.c_str());
    if (paramsOffSet == addrSpec.c_str())
    {
      from += headerParams;
    }
    else
    {
      from = std::string(addrSpec.c_str(), (const char*)paramsOffSet);
      from += headerParams;
    }
  }
  return true;
}

std::string SIPFrom::getHeaderParam(const char* paramName) const
{
  std::string paramValue;
  getHeaderParam(_data, paramName, paramValue);
  return paramValue;
}

bool SIPFrom::getHeaderParam(const std::string& from, const char* paramName, std::string& paramValue)
{
  std::string headerParams;
  if (!getHeaderParams(from, headerParams))
    return false;
  return getHeaderParamEx(headerParams, paramName, paramValue);
}

bool SIPFrom::getHeaderParamEx(const std::string& headerParams, const char* paramName, std::string& paramValue)
{
  std::string key = paramName;
  key += "=";

  char* startIter = ABNF::findNextIterFromString(key, headerParams.c_str());
  if (startIter == headerParams.c_str())
    return false;

  char* newIter = pvalueParser.parse(startIter);
  if (newIter == startIter)
    return false;
  
  paramValue = std::string(startIter, newIter);
  return true;
}

bool SIPFrom::setHeaderParam(const char* paramName, const char* paramValue)
{
  check_empty(this);
  return setHeaderParam(_data, paramName, paramValue);
}

bool SIPFrom::setHeaderParam(std::string& from, const char* paramName, const char* paramValue)
{
  std::string headerParams;
  getHeaderParams(from, headerParams);
  setHeaderParamEx(headerParams, paramName, paramValue);
  return setHeaderParams(from, headerParams.c_str());
}

bool SIPFrom::setHeaderParamEx(std::string& headerParams, const char* paramName, const char* paramValue)
{
  if (!pnameVerify(paramName) || !pvalueVerify(paramValue))
    throw OSS::SIP::SIPParserException("ABNF Syntax Error");
  
  std::string key = paramName;
  boost::to_lower(key);
  key += "="; 
  
  char* offSet = ABNF::findNextIterFromString(key, headerParams.c_str());
  if (offSet == headerParams.c_str())
  {
    std::ostringstream strm;
    strm << ";" << paramName << "=" << paramValue;
    headerParams += strm.str();
    return true;
  }

  std::string front(headerParams.c_str(), (const char*)offSet);
  if(front.at(front.size() - 1) != '=')
    front += "=";
  front += paramValue;

  char* tailOffSet = ABNF::findNextIterFromString(";", offSet);
  if (tailOffSet != offSet)
  {
    tailOffSet--;
    front += tailOffSet;
  }

  headerParams = front;
  return true;
}

} } // OSS::SIP



