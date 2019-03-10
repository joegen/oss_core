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

#ifndef SBCCDREVENT_H_INCLUDED
#define	SBCCDREVENT_H_INCLUDED


#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/SIPMessage.h"

namespace OSS {
namespace SIP {
namespace SBC {
  

using OSS::SIP::SIPTransaction; 
  
class SBCCDREvent
{
public:
  enum Type 
  { 
    EVENT_PROGRESS, 
    EVENT_FINAL, 
    EVENT_TRANSFERRED, 
    EVENT_TERMINATED,
    EVENT_EXIT_QUEUE,
    EVENT_UNKNOWN 
  };
  
  SBCCDREvent();
  
  SBCCDREvent(
    const SIPMessage::Ptr& pRequest,
    const SIPTransaction::Ptr& pTransaction,
    Type type
  );
  
  SBCCDREvent(const SBCCDREvent& cdrEvent);
  
  ~SBCCDREvent();
  
  void swap(SBCCDREvent& cdrEvent);
  
  SBCCDREvent& operator= (const SBCCDREvent& event);
  
  const SIPMessage::Ptr& getRequest() const;
  
  void setRequest(const SIPMessage::Ptr& pRequest);
  
  const SIPTransaction::Ptr& getTransaction() const;
  
  void setTransaction(const SIPTransaction::Ptr& pTransaction);
  
  Type getType() const;
  
  void setType(Type type);
  
  const std::string& getSessionId() const;
  
private:
  SIPMessage::Ptr _pRequest;
  SIPTransaction::Ptr _pTransaction;
  Type _type;
  std::string _sessionId;
};


//
// Inlines
//

inline SBCCDREvent::SBCCDREvent() :
  _type(EVENT_UNKNOWN)
{
  
}
  
inline SBCCDREvent::SBCCDREvent(
  const SIPMessage::Ptr& pRequest,
  const SIPTransaction::Ptr& pTransaction,
  Type type
) :
 _type(type)
{
  _pRequest = pRequest;
  _pTransaction = pTransaction;
  if (_pTransaction->isParent())
  {
    _pTransaction->getProperty("session-id", _sessionId);
  }
  else
  {
    _pTransaction->getParent()->getProperty("session-id", _sessionId);
  }
}

inline SBCCDREvent::SBCCDREvent(const SBCCDREvent& cdrEvent) :
  _type(cdrEvent.getType())
{
  _pRequest = cdrEvent.getRequest();
  _pTransaction = cdrEvent.getTransaction();
  _sessionId = cdrEvent.getSessionId();
}

inline SBCCDREvent::~SBCCDREvent()
{
}

inline SBCCDREvent& SBCCDREvent::operator= (const SBCCDREvent& event)
{
  if (&event == this)
  {
    return *this;
  }
  
  _type = event.getType();
  _pRequest = event.getRequest();
  _pTransaction = event.getTransaction();
  _sessionId =  event.getSessionId();
  return *this;
}


inline const SIPMessage::Ptr& SBCCDREvent::getRequest() const
{
  return _pRequest;
}
  
inline void SBCCDREvent::setRequest(const SIPMessage::Ptr& pRequest)
{
  _pRequest = pRequest;
}

inline const SIPTransaction::Ptr& SBCCDREvent::getTransaction() const
{
  return _pTransaction;
}

inline void SBCCDREvent::setTransaction(const SIPTransaction::Ptr& pTransaction)
{
  _pTransaction = pTransaction;
}

inline SBCCDREvent::Type SBCCDREvent::getType() const
{
  return _type;
}

inline void SBCCDREvent::setType(Type type)
{
  _type = type;
}

inline const std::string& SBCCDREvent::getSessionId() const
{
  return _sessionId;
}

} } } // OSS::SIP::SBC


#endif	// SBCCDREVENT_H_INCLUDED

