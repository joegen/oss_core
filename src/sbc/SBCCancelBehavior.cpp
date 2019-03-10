

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


#include "OSS/SIP/SBC/SBCCancelBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SIPCSeq.h"


namespace OSS {
namespace SIP {
namespace SBC {

using OSS::SIP::SIPMessage;
using OSS::Net::IPAddress;

SBCCancelBehavior::SBCCancelBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_CANCEL, "SBC CANCEL Request Handler")
{
  setName("SBC CANCEL Request Handler");
}

SBCCancelBehavior::~SBCCancelBehavior()
{
}

SIPMessage::Ptr SBCCancelBehavior::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  
  std::string inviteId;
  SIPMessage::Ptr pInvite;
  pRequest->getTransactionId(inviteId, "invite");
  {
    OSS::mutex_read_lock _rwlock(_rwInvitePoolMutex);
    std::map<std::string, SIPMessage::Ptr>::iterator inviteIter = _invitePool.find(inviteId);
    if (inviteIter == _invitePool.end())
    {
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_481_TransactionDoesNotExist );
      return serverError;
    }
    pInvite = inviteIter->second;
  }


  
  pRequest.reset();
  pRequest = SIPMessage::Ptr(new SIPMessage(*(pInvite.get())));
  
  SIPRequestLine startLine = pRequest->startLine();
  startLine.setMethod("CANCEL");
  pRequest->setStartLine(startLine.data());

  SIPCSeq cseq = pRequest->hdrGet("cseq");
  cseq.setMethod("CANCEL");
  pRequest->hdrRemove("cseq");
  pRequest->hdrSet("CSeq", cseq.data().c_str());

  pRequest->setBody("");
  pRequest->hdrRemove("content-length");
  pRequest->hdrSet("Content-Length", "0");

  //
  // Remove headers that do not have semantics in CANCEL
  //
  //
  // Note we use hdrListRemove to also cover buggy UAs that insert multiple headers
  //
  pRequest->hdrListRemove("content-type");
  pRequest->hdrListRemove("min-se");
  pRequest->hdrListRemove("session-expires");
  pRequest->hdrListRemove("proxy-authorization");
  pRequest->hdrListRemove("authorization");
  pRequest->hdrListRemove("allow");
  pRequest->hdrListRemove("supported");

  std::string isXorValue;
  if (pInvite->getProperty("xor", isXorValue) && isXorValue == "1")
    pRequest->setProperty("xor", "1");

  std::string localAddress;
  OSS_VERIFY(pInvite->getProperty("local-address", localAddress));
  pRequest->setProperty("local-address", localAddress);

  std::string targetAddress;
  OSS_VERIFY(pInvite->getProperty("target-address", targetAddress));
  pRequest->setProperty("target-address", targetAddress);

  std::string targetTransport;
  OSS_VERIFY(pInvite->getProperty(OSS::PropertyMap::PROP_TargetTransport, targetTransport));
  pRequest->setProperty(OSS::PropertyMap::PROP_TargetTransport, targetTransport);
  
  localInterface = IPAddress::fromV4IPPort(localAddress.c_str());
  OSS_VERIFY(localInterface.isValid());
  
  target = IPAddress::fromV4IPPort(targetAddress.c_str());
  OSS_VERIFY(target.isValid());

  return OSS::SIP::SIPMessage::Ptr();
}

void SBCCancelBehavior::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
}

void SBCCancelBehavior::registerInvite(const std::string& id, const SIPMessage::Ptr& pInvite)
{
  OSS::mutex_write_lock _rwlock(_rwInvitePoolMutex);
  _invitePool[id] = pInvite;
}

void SBCCancelBehavior::removeInvite(const std::string& id)
{
  OSS::mutex_write_lock _rwlock(_rwInvitePoolMutex);
  _invitePool.erase(id);
}


} } } // OSS::SIP::SBC


