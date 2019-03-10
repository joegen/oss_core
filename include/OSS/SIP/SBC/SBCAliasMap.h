
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

#ifndef SBCALIASMAP_H_INCLUDED
#define	SBCALIASMAP_H_INCLUDED


#include <set>
#include <vector>
#include "OSS/SIP/SIPMessage.h"
#include "OSS/UTL/Thread.h"


namespace OSS {
namespace SIP {
namespace SBC {
  

class SBCAliasMap
{
public:
  typedef std::map<std::string, std::string> AliasMap;
  SBCAliasMap();
  
  ~SBCAliasMap();
  
  void mapAlias(const std::string& user,  const std::string& domain, const std::set<std::string>& aliases);
  
  bool retargetRequest(const SIPMessage::Ptr& pRequest);
  
private:
  AliasMap _aliasMap;
  OSS::mutex_critic_sec _aliasMapMutex;
};
  
} } } // OSS::SIP::SBC

#endif	// SBCALIASMAP_H_INCLUDED

