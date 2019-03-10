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

#ifndef SIP_SBCPRACKBEHAV_INCLUDED
#define SIP_SBCPRACKBEHAV_INCLUDED


#include "OSS/SIP/SBC/SBC.h"
#include "OSS/SIP/SBC/SBCDefaultBehavior.h"


namespace OSS {
namespace SIP {
namespace SBC {


class OSS_API SBCPrackBehavior : public SBCDefaultBehavior
{
public:
  SBCPrackBehavior(SBCManager* pManager);
    /// Creates a new SBC register behavior
    /// This is the base class of all SBC behaviors

  virtual ~SBCPrackBehavior();
    /// Destroys the register behavior

  virtual SIPMessage::Ptr onTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);

  virtual void onProcessResponseOutbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);
    /// This is the last chance for the application to process
    /// the outbound response before it gets sent out to the transport.
    ///
    /// This is normally the place where application would want to
    /// insert application-specific headers as well as change existing
    /// headers to the desired application-specific values for as long
    /// as it wont conflict with dialog creation states.
  
  virtual SIPMessage::Ptr onRouteTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
  
  bool queueReliableResponse(const SIPB2BTransaction::Ptr pTransaction, const SIPMessage::Ptr& pResponse);
  bool removeReliableResponse(const std::string& callId, const std::string& rseq = std::string());
private:
  class ReliableResponse
  {
  public:
    SIPMessage::Ptr response;
    int lastDuration;
    int timeRemaining;
    int retransmitCount;
    OSS::Net::IPAddress target;
    OSS::Net::IPAddress localAddress;
    std::string callId;
    std::string rseq;
    
    ReliableResponse();
    ReliableResponse(const ReliableResponse& response);
    ReliableResponse& operator=(const ReliableResponse& response);
  };
  typedef std::list<ReliableResponse> ReliableResponseQueue;
  
  void run100RelThread();
  void start100RelThread();
  void stop100RelThread();
  void retransmit100Rel();
  bool send100Rel(ReliableResponse& response);
  boost::thread* _p100RelThread;
  bool _isTerminating;
  ReliableResponseQueue _100RelQueue;
  OSS::mutex_critic_sec _100RelMutex;
};

//
// Inlines
//

} } } // OSS::SIP::SBC

#endif // SIP_SBCPRACKBEHAV_INCLUDED

