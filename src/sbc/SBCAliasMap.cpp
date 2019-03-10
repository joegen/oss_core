

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


#include "OSS/SIP/SBC/SBCAliasMap.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {
namespace SBC {
  

SBCAliasMap::SBCAliasMap()
{
}
  
  
SBCAliasMap::~SBCAliasMap()
{
}
  
void SBCAliasMap::mapAlias(const std::string& user,  const std::string& domain, const std::set<std::string>& aliases)
{
  if (user.empty() || domain.empty())
  {
    return;
  }
  
  std::ostringstream uri;
  uri << "sip:" << user << "@" << domain;
  
  OSS::mutex_critic_sec_lock lock(_aliasMapMutex);
  for (std::set<std::string>::const_iterator iter = aliases.begin(); iter != aliases.end(); iter++)
  {
    _aliasMap[*iter] = uri.str();
  }
}
  
bool SBCAliasMap::retargetRequest(const SIPMessage::Ptr& pRequest)
{
  if (pRequest->isResponse()) 
   return false;

  OSS::SIP::SIPRequestLine rline(pRequest->startLine());
  OSS::SIP::SIPURI ruri;
  rline.getURI(ruri);
  std::string user = ruri.getUser();

  OSS::mutex_critic_sec_lock lock(_aliasMapMutex);
  AliasMap::const_iterator alias = _aliasMap.find(user);
  if (alias == _aliasMap.end())
  {
    return false;
  }
  
  std::string aor = alias->second;
  rline.setURI(aor.c_str()); 
  pRequest->setStartLine(rline.data());
  
  OSS_LOG_INFO(pRequest->createContextId(true) << "SBCAliasMap::retargetRequest - " << user << " -> " << aor);
  
  return true;
}
  

  
} } } // OSS::SIP::SBC



