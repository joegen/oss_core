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





#include "OSS/SIP/SBC/SBCAutoBanRules.h"
#include "OSS/SIP/SBC/SBCManager.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
SBCAutoBanRules::SBCAutoBanRules(SBCManager* pManager)
{
}
  
SBCAutoBanRules::~SBCAutoBanRules()
{ 
}


SIPMessage::Ptr SBCAutoBanRules::processBanRules(
  SIPMessage::Ptr& pRequest,
  B2BUA::SIPB2BTransaction::Ptr pTransaction
)
{
  bool banned = false;
  
  SIPFrom from(pRequest->hdrGet(OSS::SIP::HDR_FROM));
  std::string userId = from.getUser();
  std::string displayName = from.getDisplayName();
  std::string ua = pRequest->hdrGet(OSS::SIP::HDR_USER_AGENT);
  
  if (!userId.empty() && _user.find(userId) != _user.end())
  {
    OSS_LOG_NOTICE("SBCAutoBanRules::processBanRules - Banned user detected (" << userId << ")");
    banned = true;
  }
  
  if (!banned && !displayName.empty() && _display.find(displayName) != _display.end())
  {
    OSS_LOG_NOTICE("SBCAutoBanRules::processBanRules - Banned display name detected (" << displayName << ")");
    banned = true;
  }
  
  if (!banned && !ua.empty() && _agent.find(ua) != _agent.end())
  {
    OSS_LOG_NOTICE("SBCAutoBanRules::processBanRules - Banned user agent name detected (" << ua << ")");
    banned = true;
  }
  
  if (banned)
  {
    SIPMessage::Ptr pResponse = pRequest->createResponse(SIPMessage::CODE_403_Forbidden, "Congratulations!  Your Address is now banned.");
    return pResponse;
  }
 
  
  return SIPMessage::Ptr();
}


} } } // OSS::SIP::SBC



