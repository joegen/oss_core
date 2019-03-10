

// OSS Software Solutions Application Programmer Interface
// Package: SBC
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


#include <OSS/SIP/SIPMessage.h>
#include <OSS/SIP/SIPHeaderTokens.h>

#include "OSS/SIP/SBC/SBCReferBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"


namespace OSS {
namespace SIP {
namespace SBC {

using OSS::SIP::SIPMessage;

SBCReferBehavior::SBCReferBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_REFER, "SBC REFER Request Handler")
{
  setName("SBC REFER Request Handler");
}

SBCReferBehavior::~SBCReferBehavior()
{
}

void SBCReferBehavior::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  SBCDefaultBehavior::onProcessResponseOutbound(pResponse, pTransaction);

  std::string sessionId;
  std::string legIndex;

  pTransaction->getProperty("session-id", sessionId);
  pTransaction->getProperty("leg-index", legIndex);

  if (!pResponse->is1xx(100))
  {
    SBCContact::SessionInfo sessionInfo;
    sessionInfo.sessionId = sessionId;

    if (!legIndex.empty())
      sessionInfo.callIndex = OSS::string_to_number<unsigned>(legIndex.c_str());

    SBCContact::transform(this->getManager(),
      pResponse,
      pTransaction,
      pTransaction->serverTransport()->getLocalAddress(),
      sessionInfo);
  }
}

SIPMessage::Ptr SBCReferBehavior::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  
  OSS_LOG_DEBUG("SBCReferBehavior::onRouteTransaction INVOKED");
  SIPMessage::Ptr ret = SBCDefaultBehavior::onRouteTransaction(
    pRequest, pTransaction, localInterface, target);

  if (ret)
    return ret;

  //
  // Check if bridge has a retarget header
  //
  if (!pRequest->hdrPresent("X-Bridge-Retarget"))
  {
    std::string host;
    std::string port;
    std::string retargetRefer;
    if (pRequest->getProperty("retarget-refer", retargetRefer) && !retargetRefer.empty())
    {
      std::string hReferTo = pRequest->hdrGet("refer-to");
      if (!hReferTo.empty())
      {
        std::string id = "xfer-";
        id += SIPParser::createTagString();
        _referMapMutex.lock();
        _referMap[id] = hReferTo;
        _referMapMutex.unlock();
        SIPReferTo referTo = "sip:" + id + "@" + retargetRefer;
        pRequest->hdrSet("Refer-To", referTo.data());
        pRequest->hdrSet("X-SBC-Retarget", hReferTo.c_str());
        pRequest->commitData();
      }
      else
      {
        OSS_LOG_DEBUG("SBCReferBehavior::onRouteTransaction Refer-to is empty");
      }
    }
    else
    {
      OSS_LOG_DEBUG("SBCReferBehavior::onRouteTransaction enable-refer-retarget or transport not set");
    }
  }
  
  return OSS::SIP::SIPMessage::Ptr();
}

bool SBCReferBehavior::onRetargetTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  if (!pRequest->isRequest("INVITE")) // we are only interested in invites
    return false;

  SIPRequestLine rline = pRequest->startLine();
  SIPURI ruri;
  if (rline.getURI(ruri))
  {
    std::string user;
    user = ruri.getUser();
    SIPReferTo referTo;
    if (user.substr(0, 4) == "xfer")
    {
      _referMapMutex.lock();
      std::map<std::string, std::string>::iterator iter = _referMap.find(user);
      if (iter != _referMap.end())
      {
        referTo = iter->second;
      }
      _referMapMutex.unlock();
    }

    if (!referTo.isEmpty())
    {
      //
      // Do the magic
      //

      SIPURI referToUri = referTo.getURI();
      std::string replaces = referToUri.getHeader("replaces");
      std::string params = referToUri.getParams();
      if (!replaces.empty())
      {
        std::string repl;
        SIPParser::unescape(repl, replaces.c_str());
        pRequest->hdrSet("Replaces", repl);
      }

      std::string toUri = referToUri.getIdentity();
      pRequest->hdrSet("To", toUri);
      
      std::ostringstream retarget;
      retarget << referToUri.getIdentity();
      if (!params.empty())
        retarget << params;

      rline.setURI(retarget.str().c_str());
      pRequest->setStartLine(rline.data());
      pRequest->commitData();
      
      pRequest->setProperty("retargeted", "1");
    }
  }

  //
  // Note: We always return false so that the route scripts are still evaluated
  //
  return false;
}

bool SBCReferBehavior::transformPreDialogPersist(const SIPMessage::Ptr& pRequest, const SIPB2BTransaction::Ptr& pTransaction)
{
  if (!pRequest->isRequest(SIP::REQ_INVITE))
  {
    return false;
  }

  SIPTo to = pRequest->hdrGet("to");
  std::string user = to.getUser();
  
  if (user.substr(0, 4) != "xfer")
  {
    return false;
  }
  
  OSS::mutex_lock lock(_referMapMutex);
  SIPReferTo referTo;
  std::map<std::string, std::string>::iterator iter = _referMap.find(user);
  if (iter != _referMap.end())
  {
    return false;
  }
  
  referTo = iter->second;
  user = referTo.getUser();
  to.setUser(user.c_str());
  pRequest->hdrSet(OSS::SIP::HDR_TO, to.data());
  
  return true;
}

} } } // OSS::SIP::SBC


