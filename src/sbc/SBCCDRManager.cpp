
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


#include <OSS/SIP/SIPHeaderTokens.h>

#include "OSS/SIP/SBC/SBCCDRManager.h"
#include "OSS/SIP/SBC/SBCCDRRecord.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SIPStatusLine.h"
#include "OSS/UTL/Logger.h"
#include "OSS/SIP/SBC/SBCManager.h"


namespace OSS {
namespace SIP {
namespace SBC {
  

const unsigned int  DEFAULT_CDR_LIFETIME = (60 * 60) * 12; // 12 hours
  
static void on_handle_progress(SBCCDRManager* pManager, SBCCDREvent* pEvent)
{
  //
  // So far we don't have a need for this but let's keep it handy just in case
  //
}

static void flush_stale_records(SBCCDRManager* pManager, bool force = false)
{
  static OSS::UInt64 lastFlushTime = OSS::getTime();
  OSS::UInt64 now =  OSS::getTime();
  if (force || now - lastFlushTime > 600000)
  {
    lastFlushTime = now;
    std::vector<std::string> keys;
    pManager->cdr()->getKeys("*", keys);
    for (std::vector<std::string>::iterator iter = keys.begin(); iter != keys.end(); iter++)
    {
      SBCCDRRecord cdr;
      if (cdr.readFromWorkSpace(*(pManager->cdr()), *iter))
      {
        if (!cdr.connectTime() && cdr.setupTime() + 300000 < now)
        {
          pManager->cdr()->del(*iter);
        }
      }
    }
  }
}

static void flush_record(SBCCDRManager* pManager, const std::string& sessionId, SBCCDRRecord& cdr)
{
  //
  // Delete the old record and save it as a new key with the date
  //
  pManager->cdr()->del(sessionId);
  cdr.writeToLogFile(pManager->logger());
 
  
  //
  // Flush stale records
  //
  flush_stale_records(pManager);
}

static void on_handle_final(SBCCDRManager* pManager, SBCCDREvent* pEvent)
{
  SBCCDRRecord cdr;
  if (!cdr.readFromWorkSpace(*(pManager->cdr()), pEvent->getSessionId()))
  {
    return;
  }
  
  std::string callId = pEvent->getRequest()->hdrGet(OSS::SIP::HDR_CALL_ID);
  
  if (pEvent->getRequest()->is4xx(401) || pEvent->getRequest()->is4xx(407))
  {
    //
    // Check if this is a challenge response.
    // Delete it if it is
    //
    pManager->cdr()->del(pEvent->getSessionId());

    //
    // Remove it from the active channels
    //
    SIPURI ruri(cdr.requestUri());
    SIPURI furi(cdr.fromUri());
    pManager->channelLimits().removeCall(callId, ruri.getUser());
    pManager->channelLimits().removeCall(callId, furi.getHost());

    
    return;
  }
  else if (pEvent->getRequest()->isResponseFamily(200))
  {
    //
    // Set the connect time
    //
    cdr.connectTime() = OSS::getTime();
    //
    // Get the called contact
    //
    SIP::SIPContact hContact(pEvent->getRequest()->hdrGet(SIP::HDR_CONTACT));
    SIP::ContactURI calledUri;
    hContact.getAt(calledUri, 0);
    cdr.calledContact() = calledUri.data();
    cdr.writeToWorkSpace(*(pManager->cdr()), pEvent->getSessionId(), pManager->getCDRLifeTime());
    
    OSS_LOG_INFO(pEvent->getRequest()->createContextId(true) << "SBCCDRManager::onHandleEvent::on_handle_final CONNECTED" 
      << " connectTime: " <<  cdr.connectTime());

  }
  else
  {
    //
    // Set the disconnect time
    //
    cdr.disconnectTime() = OSS::getTime();
    //
    // Set the error response;
    //
    cdr.errorResponse() = pEvent->getRequest()->getStartLine();
    
    //
    // Record this CDR
    //
    flush_record(pManager, pEvent->getSessionId(), cdr);
    
    //
    // Remove it from the active channels
    //
    SIPURI ruri(cdr.requestUri());
    SIPURI furi(cdr.fromUri());
    pManager->channelLimits().removeCall(callId, ruri.getUser());
    pManager->channelLimits().removeCall(callId, furi.getHost());
  }
  
  pManager->sbcManager()->modules().notifyCdrEvent("CallSetupComplete", cdr);
}

static void on_handle_transferred(SBCCDRManager* pManager, SBCCDREvent* pEvent)
{
  //
  // We do not need this for now but let's make handy just in case
  //
}

static void on_handle_terminated(SBCCDRManager* pManager, SBCCDREvent* pEvent)
{
  SBCCDRRecord cdr;
  if (!cdr.readFromWorkSpace(*(pManager->cdr()), pEvent->getSessionId()))
  {
    OSS_LOG_INFO(pEvent->getRequest()->createContextId(true) << "SBCCDRManager::onHandleEvent::on_handle_terminated " 
      << "Session-ID: " << pEvent->getSessionId() << " not found"); 
    return;
  }
  
  std::string callId = pEvent->getRequest()->hdrGet(OSS::SIP::HDR_CALL_ID);
  
  //
  // Set the connect time
  //
  cdr.disconnectTime() = OSS::getTime();
  
  //
  // Record this CDR
  //
  flush_record(pManager,  pEvent->getSessionId(), cdr);
  
  //
  // Remove it from the active channels
  //
  SIPURI ruri(cdr.requestUri());
  SIPURI furi(cdr.fromUri());
  pManager->channelLimits().removeCall(callId, ruri.getUser());
  pManager->channelLimits().removeCall(callId, furi.getHost());
  
  unsigned duration = 0;
  if (cdr.connectTime() && cdr.disconnectTime() && (cdr.disconnectTime() > cdr.connectTime() ))
  {
    duration = (cdr.disconnectTime() - cdr.connectTime()) / 1000;
  }
  
  OSS_LOG_INFO(pEvent->getRequest()->createContextId(true) << "SBCCDRManager::onHandleEvent::on_handle_terminated " 
    << " Call Duration: " << duration << " seconds" 
    << " connectTime: " <<  cdr.connectTime()
    << " disconnectTime: " << cdr.disconnectTime());
  
  pManager->sbcManager()->modules().notifyCdrEvent("CallTerminated", cdr);
}

   
SBCCDRManager::SBCCDRManager() :
  _pEventQueueThread(0),
  _pWorkSpaceManager(0),
  _cdrLifeTime(DEFAULT_CDR_LIFETIME),
  _logger("SBCCDRManager")
{
}
  
SBCCDRManager::~SBCCDRManager()
{
  if (_pEventQueueThread)
  {
    SBCCDREvent* pExit = new SBCCDREvent();
    pExit->setType(SBCCDREvent::EVENT_EXIT_QUEUE);
    _eventQueue.enqueue(pExit);
    _pEventQueueThread->join();
    delete _pEventQueueThread;
  }
}
  
void SBCCDRManager::initialize(SBCManager* pSBCManager)
{
  OSS_ASSERT(!_pEventQueueThread);
  _pManager = pSBCManager;
  _pWorkSpaceManager = &pSBCManager->workspace();
  _pCDRDb = _pWorkSpaceManager->getCDRDb();
  
  //
  // Open the logger
  //
  std::ostringstream logFilePath;
  logFilePath << OSS::boost_path(_pManager->getLogDirectory()) << "/cdr.csv";
  _logger.open(logFilePath.str(), OSS::UTL::LogFile::PRIO_NOTICE, "%t");
  
  //
  // Start the event queue
  //
  _pEventQueueThread = new boost::thread(boost::bind(&SBCCDRManager::onHandleEvent, this));
  
  //
  // Initialize the Channel limiters
  //
  _channelLimits.initialize(pSBCManager);
  
  //
  // Force flushing of stale records
  //
  flush_stale_records(this, true);
}

std::size_t SBCCDRManager::onCallSetup(
  const SIPMessage::Ptr& pRequest,
  const SIPTransaction::Ptr& pTransaction,
  std::size_t& channelLimit
)
{
  std::string callId = pRequest->hdrGet(OSS::SIP::HDR_CALL_ID);
  std::string sessionId;
  OSS_VERIFY(pTransaction->getProperty("session-id", sessionId));
 
  SBCCDRRecord cdr;
  //
  // Set the date
  //
  cdr.date() = boost_format_time(boost::posix_time::second_clock::universal_time(), "%Y-%m-%d %H:%M:%S");
    
  //
  // Get the From URI
  //
  SIP::SIPFrom hFrom(pRequest->hdrGet(SIP::HDR_FROM));
  cdr.fromUri() = hFrom.getAor();
  
  //
  // Get the To URI
  //
  SIP::SIPTo hTo(pRequest->hdrGet(SIP::HDR_TO));
  cdr.toUri() = hTo.getAor();
  
  //
  // Get the caller contact
  //
  SIP::SIPContact hContact(pRequest->hdrGet(SIP::HDR_CONTACT));
  SIP::ContactURI callerUri;
  hContact.getAt(callerUri, 0);
  cdr.callerContact() = callerUri.data();
  
  //
  // Get the source address
  //
  OSS::Net::IPAddress srcAddress = pTransaction->transport()->getRemoteAddress();
  cdr.srcAddress() = srcAddress.toIpPortString();
  
  //
  // Get the Request URI
  //
  SIP::SIPRequestLine rLine(pRequest->getStartLine());
  SIPURI ruri;
  rLine.getURI(ruri);
  cdr.requestUri() = ruri.data();
  
  //
  // Get the From URI
  //
  SIPFrom from(pRequest->hdrGet(OSS::SIP::HDR_FROM));
  
  //
  // Get the callId
  //
  cdr.callId() = callId;
  
  //
  // Get the sessionId
  //
  cdr.sessionId() = sessionId;
  
  //
  // Generate the Setup time
  //
  cdr.setupTime() = OSS::getTime();
  
  cdr.writeToWorkSpace(*(_pCDRDb), sessionId, getCDRLifeTime());
  
  _pManager->modules().notifyCdrEvent("CallSetup", cdr);
  
  //
  // Return the channel count
  //
  std::size_t prefixCount = 0;
  std::size_t prefixLimit = 0;
  std::size_t domainCount = 0;
  std::size_t domainLimit = 0;
  std::size_t callCount = 0;
  
  
  prefixCount = _channelLimits.addCall(callId, ruri.getUser(), prefixLimit);
  domainCount = _domainLimits.addCall(callId, from.getHost(), domainLimit);
  
  if (prefixLimit < domainLimit && prefixCount)
  {
    OSS_LOG_INFO(pTransaction->getLogId() << "SBCCDRManager::onCallSetup - Enforcing  prefix limits. count: " << prefixCount << " limit: " << prefixLimit);
    channelLimit = prefixLimit;
    callCount = prefixCount;
  }
  else if (prefixLimit > domainLimit && domainCount)
  {
    OSS_LOG_INFO(pTransaction->getLogId() << "SBCCDRManager::onCallSetup - Enforcing  domain limits. count: " << domainCount << " limit: " << domainLimit);
    channelLimit = domainLimit;
    callCount = domainCount;
  }
  
  return callCount;
}

void SBCCDRManager::onHandleEvent()
{
  bool terminated = false;
  while (!terminated)
  {
    SBCCDREvent* pEvent;
    _eventQueue.dequeue(pEvent);
    OSS_ASSERT(pEvent);
    
    if (pEvent->getSessionId().empty())
    {
      return;
    }
    
    std::string eventName = "CallUnknown";
    switch (pEvent->getType())
    {
      case SBCCDREvent::EVENT_PROGRESS:
        eventName = "CallProgress";
        on_handle_progress(this, pEvent);
        break;
      case SBCCDREvent::EVENT_FINAL:
        eventName = "CallFinal";
        on_handle_final(this, pEvent);
        break;
      case SBCCDREvent::EVENT_TRANSFERRED:
        eventName = "CallTransferred";
        on_handle_transferred(this, pEvent);
        break;
      case SBCCDREvent::EVENT_TERMINATED:
        on_handle_terminated(this, pEvent);
        eventName = "CallTerminated";
        break;
      case SBCCDREvent::EVENT_EXIT_QUEUE:
        terminated = true;
        break;
      case SBCCDREvent::EVENT_UNKNOWN:
        break;
    }

    delete pEvent;
  }
}

  
} } } // OSS::SIP::SBC




