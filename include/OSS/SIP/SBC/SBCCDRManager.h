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


#ifndef SBCCDRMANAGER_H_INCLUDED
#define	SBCCDRMANAGER_H_INCLUDED


#include "OSS/SIP/SBC/SBCWorkSpaceManager.h"
#include "OSS/SIP/SBC/SBCCDREvent.h"
#include "OSS/SIP/SBC/SBCCDRRecord.h"
#include "OSS/UTL/BlockingQueue.h"
#include "OSS/UTL/LogFile.h"
#include "OSS/SIP/SBC/SBCChannelLimits.h"
#include "OSS/SIP/SBC/SBCDomainLimits.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
class SBCManager;

  
class SBCCDRManager
{
public:
  typedef BlockingQueue<SBCCDREvent*> EventQueue;
  
  SBCCDRManager();
  
  ~SBCCDRManager();
  
  void initialize(SBCManager* pSBCManager);
  
  std::size_t onCallSetup(
    const SIPMessage::Ptr& pRequest,
    const SIPTransaction::Ptr& pTransaction,
    std::size_t& channelLimit
  );
  // Returns the number of calls active if the channel count is monitored
  
  void onCallProgress(
    const SIPMessage::Ptr& pRequest,
    const SIPTransaction::Ptr& pTransaction
  );
  
  void onCallFinal(
    const SIPMessage::Ptr& pRequest,
    const SIPTransaction::Ptr& pTransaction
  );
  
  void onCallTerminated(
    const SIPMessage::Ptr& pRequest,
    const SIPTransaction::Ptr& pTransaction
  );
  
  void onCallTransferred(
    const SIPMessage::Ptr& pRequest,
    const SIPTransaction::Ptr& pTransaction
  );
  
  SBCWorkSpaceManager::WorkSpace& cdr();
  
  void setCDRLifeTime(unsigned int cdrLifeTime);
  
  unsigned int getCDRLifeTime() const;
  
  OSS::UTL::LogFile& logger();
  
  SBCChannelLimits& channelLimits();
  SBCDomainLimits& domainLimits();
  
  SBCManager* sbcManager();
  
protected:
  void onHandleEvent();
  
private:  
  EventQueue _eventQueue;
  boost::thread* _pEventQueueThread;
  SBCManager* _pManager;
  SBCWorkSpaceManager* _pWorkSpaceManager;
  SBCWorkSpaceManager::WorkSpace _pCDRDb;
  unsigned int _cdrLifeTime;
  OSS::UTL::LogFile _logger;
  SBCChannelLimits _channelLimits;
  SBCDomainLimits _domainLimits;
};

//
// Inlines
//


inline SBCWorkSpaceManager::WorkSpace& SBCCDRManager::cdr()
{
  return _pCDRDb;
}

inline void SBCCDRManager::onCallProgress(
  const SIPMessage::Ptr& pRequest,
  const SIPTransaction::Ptr& pTransaction
)
{
  _eventQueue.enqueue(new SBCCDREvent(pRequest, pTransaction, SBCCDREvent::EVENT_PROGRESS));
}

inline void SBCCDRManager::onCallFinal(
  const SIPMessage::Ptr& pRequest,
  const SIPTransaction::Ptr& pTransaction
)
{
  _eventQueue.enqueue(new SBCCDREvent(pRequest, pTransaction, SBCCDREvent::EVENT_FINAL));
}

inline void SBCCDRManager::onCallTerminated(
  const SIPMessage::Ptr& pRequest,
  const SIPTransaction::Ptr& pTransaction
)
{
  _eventQueue.enqueue(new SBCCDREvent(pRequest, pTransaction, SBCCDREvent::EVENT_TERMINATED));
}

inline void SBCCDRManager::onCallTransferred(
  const SIPMessage::Ptr& pRequest,
  const SIPTransaction::Ptr& pTransaction
)
{
  _eventQueue.enqueue(new SBCCDREvent(pRequest, pTransaction, SBCCDREvent::EVENT_TRANSFERRED));
}

inline void SBCCDRManager::setCDRLifeTime(unsigned int cdrLifeTime)
{
  _cdrLifeTime = cdrLifeTime;
}
  
inline unsigned int SBCCDRManager::getCDRLifeTime() const
{
  return _cdrLifeTime;
}

inline OSS::UTL::LogFile& SBCCDRManager::logger()
{
  return _logger;
}

inline SBCChannelLimits& SBCCDRManager::channelLimits()
{
  return _channelLimits;
}

inline SBCDomainLimits& SBCCDRManager::domainLimits()
{
  return _domainLimits;
}

inline SBCManager* SBCCDRManager::sbcManager()
{
  return _pManager;
}
  
} } } // OSS::SIP::SBC


#endif	// SBCCDRMANAGER_H_INCLUDED

