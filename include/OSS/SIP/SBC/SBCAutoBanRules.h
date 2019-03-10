// OSS Software Solutions Application Programmer Interface
// Package: Karoo
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
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


#ifndef SBCAUTOBANRULES_H_INCLUDED
#define	SBCAUTOBANRULES_H_INCLUDED


#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
class SBCManager;

  
class SBCAutoBanRules
{
public: 
  
  typedef std::set<std::string> FromUserMap;
  typedef std::set<std::string> FromDisplayNameMap;
  typedef std::set<std::string> UserAgentMap;
  
  SBCAutoBanRules(SBCManager* pManager);
  
  ~SBCAutoBanRules();
  
  void banUserById(const std::string& userId);
  
  void banUserByDisplayName(const std::string& displayName);
  
  void banUserAgent(const std::string& userAgent);
  
  SIPMessage::Ptr processBanRules(
    SIPMessage::Ptr& pRequest,
    B2BUA::SIPB2BTransaction::Ptr pTransaction
  );
  
private:
  SBCManager* _pManager;
  FromUserMap _user;
  FromDisplayNameMap _display;
  UserAgentMap _agent;
};

//
// Inlines
//

inline void SBCAutoBanRules::banUserById(const std::string& userId)
{
  _user.insert(userId);
}
  
inline void SBCAutoBanRules::banUserByDisplayName(const std::string& displayName)
{
  _display.insert(displayName);
}

inline void SBCAutoBanRules::banUserAgent(const std::string& userAgent)
{
  _agent.insert(userAgent);
}

} } } // OSS::SIP::SBC

#endif // SBCAUTOBANRULES_H_INCLUDED

