// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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


#include "OSS/SIP/SIPIctPool.h"
#include "OSS/SIP/SIPIct.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPFrom.h"


namespace OSS {
namespace SIP {


SIPIctPool::SIPIctPool(SIPFSMDispatch* dispatch) :
  SIPTransactionPool(dispatch)
{
}

SIPIctPool::~SIPIctPool()
{
}

void SIPIctPool::onAttachFSM(const SIPTransaction::Ptr& pTransaction)
{
  pTransaction->type() = SIPTransaction::TYPE_ICT;
  pTransaction->fsm() = SIPIct::Ptr(new SIPIct(_ioService, _timerProps));
  pTransaction->fsm()->setOwner(new SIPTransaction::WeakPtr(pTransaction));
  pTransaction->fsm()->dispatch() = dispatch();
}

SIPTransaction::Ptr SIPIctPool::findChildTransaction(const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport, const SIPTransaction::Ptr& pParent)
{
  if (!dispatch()->getEnableIctForking())
    return SIPTransaction::Ptr();

  if (pMsg->isMidDialog())
  {
    if (pParent->isChildTransaction())
    {
      //
      // transaction is already a child.
      //
    	//TODO: What was the intention with this code? Maybe a return is missing?
      SIPTransaction::Ptr();
    }

    SIPTo to;
    to = pMsg->hdrGet("to");
    std::string toTag;
    toTag = to.getHeaderParam("tag");

    if (pParent->getRemoteTag().empty())
    {
      pParent->setRemoteTag(toTag);
      return SIPTransaction::Ptr();
    }
    else if (pParent->getRemoteTag() != toTag)
    {
      //
      // We got a fork
      //
      return pParent->createChildTransactionFromResponse(pMsg, pTransport, true);
    }
  }
  else if (pMsg->isRequest())
  {
    //
    // TODO!  This is a request that was forked downstream
    //
  }

  return SIPTransaction::Ptr();
}


} } // OSS::SIP

