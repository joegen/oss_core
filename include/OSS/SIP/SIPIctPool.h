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

#ifndef SIP_SIPIctPool_INCLUDED
#define SIP_SIPIctPool_INCLUDED


#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPTransactionPool.h"


namespace OSS {
namespace SIP {

class SIPFSMDispatch;

class OSS_API SIPIctPool: protected SIPTransactionPool
  /// An implementation of SIP RFC 3261 Invite Client Transaction State Machine Pool.
{
public:
  virtual void onAttachFSM(const SIPTransaction::Ptr& pTransaction);
    /// Attach an FSM to a new created transaction
protected:
  SIPIctPool(SIPFSMDispatch* dispatch);
    /// Creates a new ICT Pool

  virtual ~SIPIctPool();
    /// Destroys the ICT Pool object

  virtual SIPTransaction::Ptr findChildTransaction(const SIPMessage::Ptr& pMsg, const SIPTransportSession::Ptr& pTransport, const SIPTransaction::Ptr& pParent);
    /// Determines if parent transaction can be forked.  returns child if it can be forked.  null if not
 
  friend class SIPFSMDispatch;
};


} } // OSS::SIP
#endif // SIP_SIPIctPool_INCLUDED
