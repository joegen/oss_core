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
  if (!pTransaction->fsm())
  {
    pTransaction->type() = SIPTransaction::TYPE_ICT;
    pTransaction->fsm() = SIPIct::Ptr(new SIPIct(_ioService, _timerProps));
    pTransaction->fsm()->setOwner(new SIPTransaction::WeakPtr(pTransaction));
    pTransaction->fsm()->dispatch() = dispatch();
  }
}


} } // OSS::SIP

