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

#ifndef SBCVIDEOBRIDGE_H_INCLUDED
#define	SBCVIDEOBRIDGE_H_INCLUDED


#include "OSS/EP/EndpointListener.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"

namespace OSS {
namespace SIP {
namespace SBC {

  
using OSS::EP::EndpointListener;
using OSS::EP::EndpointConnection;
using OSS::SIP::B2BUA::SIPB2BTransaction;


class SBCManager;

class SBCVideoBridge  : public EndpointListener
{
public:
  SBCVideoBridge();
  
  virtual ~SBCVideoBridge();
  
  virtual void handleStart();
    /// handle a start request.  This should not block. 
  
  virtual void handleStop();
    /// handle a stop request.  This should not block
   
  void attachSBCManager(SBCManager* pManager);
    /// Attach the SBC Manager pointer
  
  const std::string& getRoomHeader() const;
    /// Returns the custom header to be used to specify the room id
protected:
  virtual void onHandleEvent(const SIPMessage::Ptr& pRequest);
  void onHandleINVITE(const SIPMessage::Ptr& pRequest);
  void onHandleACK(const SIPMessage::Ptr& pRequest);
  void onHandleBYE(const SIPMessage::Ptr& pRequest);
  SIPMessage::Ptr handleParticipant(const SIPMessage::Ptr& pRequest);
    /// handle an incoming SIP event
  SBCManager* _pManager;
private:
  std::string _roomHeader;
};


//
// Inlines
//
inline const std::string& SBCVideoBridge::getRoomHeader() const
{
  return _roomHeader;
}


} } } // OSS::SIP::SBC

#endif	// SBCVIDEOBRIDGE_H_INCLUDED

