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


#include "OSS/SIP/SBC/SBCRFC2543HoldFix.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SDP/SDPSession.h"


namespace OSS {
namespace SIP {
namespace SBC {


SBCRFC2543HoldFix::SBCRFC2543HoldFix(SBCManager* pManager)
{
}

SBCRFC2543HoldFix::~SBCRFC2543HoldFix()
{
}

void SBCRFC2543HoldFix::fixRFC2543Hold(
  const SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  const std::string& previousSDP)
{
  std::string newSDP = pRequest->body();
  if (newSDP.empty())
    return;

  OSS::SDP::SDPSession offer(newSDP.c_str());
  std::string sessionAddress = offer.getAddress();
}


} } } // OSS::SIP::SBC




