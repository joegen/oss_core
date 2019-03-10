
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

#include "OSS/SIP/SBC/SBCVideoBridge.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/UTL/Logger.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/UTL/PropertyMap.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SDP/SDPSession.h"


namespace OSS {
namespace SIP {
namespace SBC {

  
using OSS::EP::EndpointListener;
using OSS::EP::EndpointConnection;
using OSS::SIP::B2BUA::SIPB2BTransaction;

static const char* VIDEO_BRIGE_EP = "~~vb-ep";
static const char* X_SBC_ROOM_HEADER = "X-Conference-ID";
static const char* EP_CONTACT = "sip:~~vb-ep@127.0.0.1:0";

SBCVideoBridge::SBCVideoBridge() :
  EndpointListener(VIDEO_BRIGE_EP),
  _pManager(0),
  _roomHeader(X_SBC_ROOM_HEADER)
{
}

SBCVideoBridge::~SBCVideoBridge()
{
}

void SBCVideoBridge::attachSBCManager(SBCManager* pManager)
{
  _pManager = pManager;
}

void SBCVideoBridge::handleStart()
{
  OSS_LOG_NOTICE("SBC Video Bridge event loop has STARTED");
}

void SBCVideoBridge::handleStop()
{
  OSS_LOG_NOTICE("SBC Video Bridge event loop has ENDED");
}

void SBCVideoBridge::onHandleEvent(const SIPMessage::Ptr& pRequest)
{
  if (pRequest->isRequest("INVITE"))
  {
    onHandleINVITE(pRequest);
  }
  else if (pRequest->isRequest("ACK"))
  {
    onHandleACK(pRequest);
  }
  else if (pRequest->isRequest("BYE"))
  {
    onHandleBYE(pRequest);
  }
  else
  {
    SIPMessage::Ptr pResponse;
    pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_501_NotImplemented);
    dispatchMessage(pResponse);
    return;
  }
}

void SBCVideoBridge::onHandleINVITE(const SIPMessage::Ptr& pRequest)
{
  std::string roomId = pRequest->hdrGet(_roomHeader.c_str());
  if (roomId.empty())
  {
    SIPMessage::Ptr pResponse;
    pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_404_NotFound, "Missing Room ID Header");
    dispatchMessage(pResponse);
    return;
  }
   
  if ( pRequest->body().empty())
  {
    SIPMessage::Ptr pResponse;
    pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_500_InternalServerError, "Late Media Negotation Not Supported");
    dispatchMessage(pResponse);
    return;
  }
  
  dispatchMessage(handleParticipant(pRequest));
}

SIPMessage::Ptr SBCVideoBridge::handleParticipant(const SIPMessage::Ptr& pRequest)
{
  std::ostringstream tag;
  tag << OSS::string_hash(pRequest->hdrGet(OSS::SIP::HDR_CALL_ID).c_str());
  SIPMessage::Ptr pResponse;
  pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_200_Ok, "OK", tag.str(), EP_CONTACT);
  return pResponse;
}

void SBCVideoBridge::onHandleACK(const SIPMessage::Ptr& pRequest)
{
  //
  // Just absorb the ACK silently
  //
}

void SBCVideoBridge::onHandleBYE(const SIPMessage::Ptr& pRequest)
{
  std::ostringstream tag;
  tag << OSS::string_hash(pRequest->hdrGet(OSS::SIP::HDR_CALL_ID).c_str());
  SIPMessage::Ptr pResponse;
  pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_200_Ok, "OK", tag.str(), EP_CONTACT);
  dispatchMessage(pResponse);
}

} } } // OSS::SIP::SBC



