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

#ifndef SIP_SIPIstPool_INCLUDED
#define SIP_SIPIstPool_INCLUDED


#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPTransactionPool.h"


namespace OSS {
namespace SIP {


class SIPFSMDispatch;

class OSS_API SIPIstPool: public SIPTransactionPool
  /// An implementation of SIP RFC 3261 Invite Client Transaction State Machine Pool.
{
public:
  virtual void onAttachFSM(const SIPTransaction::Ptr& pTransaction);
    /// Attach an FSM to a new created transaction

  void addAckableTransaction(const std::string& dialogId, SIPTransaction::Ptr trn);
    /// Promote the IST as a dialog so that ACK requests will properly be mapped

  SIPTransaction::Ptr findAckableTransaction(const std::string& dialogId);
    /// Returns a smart pointer to the ACKable IST

  void removeAckableTransaction(const std::string& dialogId);
    /// Remove the the ACKable transaction from the pool

protected:
  SIPIstPool(SIPFSMDispatch* dispatch);
  virtual ~SIPIstPool();
  
  OSS::mutex_critic_sec _ackPoolMutex;
  TransactionPool _ackPool;

  friend class SIPFSMDispatch;
};


} } // OSS::SIP
#endif // SIP_SIPIstPool_INCLUDED
