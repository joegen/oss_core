
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


#include "OSS/SIP/SIPReplaces.h"
#include "OSS/ABNF/ABNFSIPRules.h"
#include "OSS/ABNF/ABNFSIPURIParameters.h"


//
// Replaces        = "Replaces" HCOLON callid *(SEMI replaces-param)
// replaces-param  = to-tag / from-tag / early-flag / generic-param
// to-tag          = "to-tag" EQUAL token
// from-tag        = "from-tag" EQUAL token
// early-flag      = "early-only"
//

namespace OSS {
namespace SIP {
  
using namespace OSS::ABNF;  
/////   PARSERS  /////
static ABNFWhileNot<ABNFSIPURIParameters> paramsFinder;
static ABNF_SIP_pvalue pvalueParser;

/////   STATIC MEMBERS   //////

static bool getParamEx(const char* params, const char* paramName, std::string& paramValue)
{
  std::string k = paramName;
  if (k != "early-only")
    k += "=";
  std::string key = ";" + k;

  char* startIter = ABNF::findNextIterFromString(key, params);
  
  
  if (startIter == params)
  {
    return false;
  }
  
  if (key == ";early-only")
    return true;

  char* newIter = pvalueParser.parse(startIter);
  if (newIter == startIter)
    return false;
  
  paramValue = std::string(startIter, newIter);
  return true;
}


std::string SIPReplaces::getCallId(const std::string& replaces) 
{
  const char* paramsPtr = paramsFinder.parse(replaces.c_str());
  if (paramsPtr == replaces.c_str())
    return std::string();
  return std::string(replaces.c_str(), paramsPtr);
}

bool SIPReplaces::setCallId(std::string& replaces, const std::string& callId)
{
  std::ostringstream hdr;
  std::string fromTag="null";
  std::string toTag="null";
  bool isEarlyFlagSet = false;
  std::string earlyFlag;
  getParamEx(replaces.c_str(), "from-tag", fromTag);
  getParamEx(replaces.c_str(), "to-tag", toTag);
  isEarlyFlagSet = getParamEx(replaces.c_str(), "from-tag", earlyFlag);
  hdr << callId << ";" << "from-tag=" << fromTag << ";" << "to-tag=" << toTag;
  if (isEarlyFlagSet)
    hdr << ";early-only";
  replaces = hdr.str();
  return true;
}


std::string SIPReplaces::getFromTag(const std::string& replaces) 
{
  std::string tag;
  getParamEx(replaces.c_str(), "from-tag", tag);
  return tag;
}


bool SIPReplaces::setFromTag(std::string& replaces, const std::string& fromTag)
{
  std::ostringstream hdr;
  std::string callId="null";
  std::string toTag="null";
  bool isEarlyFlagSet = false;
  std::string earlyFlag;
  callId = getCallId(replaces.c_str());
  if (callId.empty())
    callId = "null";
  getParamEx(replaces.c_str(), "to-tag", toTag);
  isEarlyFlagSet = getParamEx(replaces.c_str(), "from-tag", earlyFlag);
  hdr << callId << ";" << "from-tag=" << fromTag << ";" << "to-tag=" << toTag;
  if (isEarlyFlagSet)
    hdr << ";early-only";
  replaces = hdr.str();
  return true;
}


std::string SIPReplaces::getToTag(const std::string& replaces) 
{
  std::string tag;
  getParamEx(replaces.c_str(), "to-tag", tag);
  return tag;
}

bool SIPReplaces::setToTag(std::string& replaces, const std::string& toTag)
{
  std::ostringstream hdr;
  std::string callId="null";
  std::string fromTag="null";
  bool isEarlyFlagSet = false;
  std::string earlyFlag;
  callId = getCallId(replaces.c_str());
  if (callId.empty())
    callId = "null";
  getParamEx(replaces.c_str(), "from-tag", fromTag);
  isEarlyFlagSet = getParamEx(replaces.c_str(), "from-tag", earlyFlag);
  hdr << callId << ";" << "from-tag=" << fromTag << ";" << "to-tag=" << toTag;
  if (isEarlyFlagSet)
    hdr << ";early-only";
  replaces = hdr.str();
  return true;
}

bool SIPReplaces::setEarlyFlag(std::string& replaces, bool isEarly)
{
  std::ostringstream hdr;
  std::string callId = "null";
  std::string fromTag = "null";
  std::string toTag = "null"; 
  std::string earlyFlag;
  callId = getCallId(replaces.c_str());
  if (callId.empty())
    callId = "null";
  getParamEx(replaces.c_str(), "from-tag", fromTag);
  getParamEx(replaces.c_str(), "to-tag", toTag);

  hdr << callId << ";" << "from-tag=" << fromTag << ";" << "to-tag=" << toTag;
  if (isEarly)
    hdr << ";early-only";
  
  replaces = hdr.str();
  return true;
}

bool SIPReplaces::isEarlyFlagSet(const std::string& replaces)
{
  std::string earlyFlag;
  return getParamEx(replaces.c_str(), "early-only", earlyFlag);
}

  
  

SIPReplaces::SIPReplaces()
{
}

SIPReplaces::SIPReplaces(const std::string& replaces)
{
  _data = replaces;
}

SIPReplaces::SIPReplaces(const SIPReplaces& replaces)
{
}

SIPReplaces::~SIPReplaces()
{
}

SIPReplaces& SIPReplaces::operator = (const std::string& replaces)
{
  return *this;
}

SIPReplaces& SIPReplaces::operator = (const SIPReplaces& replaces)
{
  return *this;
}

void SIPReplaces::swap(SIPReplaces& replaces)
{
}

std::string SIPReplaces::getCallId() const
{
  return getCallId(_data);
}

bool SIPReplaces::setCallId(const std::string& callId)
{
  return setCallId(_data, callId);
}

std::string SIPReplaces::getFromTag() const
{
  return getFromTag(_data);
}

bool SIPReplaces::setFromTag(const std::string& tag)
{
  return setFromTag(_data, tag);
}

std::string SIPReplaces::getToTag() const
{
  return getToTag(_data);
}

bool SIPReplaces::setToTag(const std::string& tag)
{
  return setToTag(_data, tag);
}

bool SIPReplaces::setEarlyFlag(bool isEarly)
{
  return setEarlyFlag(_data, isEarly);
}

bool SIPReplaces::isEarlyFlagSet()
{
  return isEarlyFlagSet(_data);
}



} } // OSS::SIP




