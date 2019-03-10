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

#include "OSS/SIP/SBC/SBCDialogStateManager.h"
#include "OSS/Net/DNS.h"
#include "OSS/SIP/SBC/SBCDefaultBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SBC/SBCException.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/SIPReplaces.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/UTL/Logger.h"
#include "OSS/SDP/SDPSession.h"
#include "OSS/SIP/SBC/SBCContact.h"
#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace SIP {
namespace SBC {


using namespace OSS::SDP;
using namespace OSS::RTP;
using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;
using OSS::Net::IPAddress;


SBCDialogStateManager::SBCDialogStateManager(SBCManager* pManager) :
  _exitSync(0, 0xFFF),
  _pThread(0),
  _stateDirectory(),
  _dialogs(3600*24),
  _pManager(pManager),
  _stateFileMaxLifeTime(60 * 12)
{
}

SBCDialogStateManager::~SBCDialogStateManager()
{
  stop();
}

bool SBCDialogStateManager::hasDialog(const std::string& callId) const
{
  _csDialogsMutex.lock();
  bool yes = _dialogs.has(callId);
  _csDialogsMutex.unlock();
  return yes;
}

void SBCDialogStateManager::addDialog(const std::string& callId, const boost::filesystem::path& stateFile)
{
  _csDialogsMutex.lock();
  Cacheable::Ptr dialogs;
  if (!_dialogs.has(callId))
  {
    std::list<boost::filesystem::path> stateFiles;
    stateFiles.push_back(stateFile);
    dialogs = Cacheable::Ptr(new Cacheable(callId, stateFiles));
    OSS_LOG_DEBUG("Added new dialog for Call-ID " << callId << " with session-id " << OSS::boost_file_name(stateFile));
  }
  else
  {
    dialogs = _dialogs.get(callId);
    OSS_LOG_DEBUG("Added new session to existing dialog for Call-ID " << callId << " with session-id " << OSS::boost_file_name(stateFile));
  }
  _dialogs.add(dialogs);
  _csDialogsMutex.unlock();
}

void SBCDialogStateManager::removeDialog(const SIPB2BTransaction::Ptr& pTransaction, bool deleteFile)
{
  std::string sessionId;
  pTransaction->getProperty("session-id", sessionId);
  assert(!sessionId.empty());
  
  std::string legIndex;
  if (!pTransaction->getProperty("leg-index", legIndex))
  {
    legIndex = "1";
  }
  
  boost::filesystem::path stateFile = operator/(_pManager->getDialogStateDirectory(), sessionId);
  std::string cid1 = pTransaction->serverRequest()->hdrGet(OSS::SIP::HDR_CALL_ID);
  std::string cid2 = pTransaction->clientRequest()->hdrGet(OSS::SIP::HDR_CALL_ID);
  std::string callId;
  if (cid1 == cid2)
  {
    callId = cid1;
  }
  else if (legIndex == "1")
  {
    callId = cid1;
  }
  else
  {
    callId = cid2;
  }
  removeDialog(callId, stateFile, deleteFile);
}

void SBCDialogStateManager::removeDialog(const std::string& callId, const boost::filesystem::path& stateFile, bool deleteFile)
{
  _csDialogsMutex.lock();
  if (_dialogs.has(callId))
  {
    Cacheable::Ptr dialogs = _dialogs.get(callId);
    std::list<boost::filesystem::path>& stateFiles =
      boost::any_cast<std::list<boost::filesystem::path>&>(dialogs->data());
    for (std::list<boost::filesystem::path>::iterator iter = stateFiles.begin();
      iter != stateFiles.end(); iter++)
    {
      if (boost_file_name(stateFile) == boost_file_name(*iter))
      {
        stateFiles.erase(iter);
        break;
      }
    }
    if (stateFiles.empty())
      _dialogs.remove(callId);
    
    OSS_LOG_DEBUG("Successfully removed dialog " << callId << " with sessionId " << boost_file_name(stateFile));
  }
  else
  {
    OSS_LOG_ERROR("Unable to remove dialog for Call-ID " << callId << " with sessionId " << boost_file_name(stateFile));
  }
  
  if (deleteFile)
  {
    ClassType::remove(stateFile);
  }
  
  _csDialogsMutex.unlock();
}

void SBCDialogStateManager::removeDialog(const SIPMessage::Ptr& pMsg, bool deleteFile)
{
  OSS::mutex_critic_sec_lock lock(_csDialogsMutex);
  std::string callId = pMsg->hdrGet("call-id");
  OSS_VERIFY(!callId.empty());
  SIPFrom from = pMsg->hdrGet("from");
  SIPTo to = pMsg->hdrGet("to");
  if (_dialogs.has(callId))
  {
    Cacheable::Ptr dialogs = _dialogs.get(callId);
    std::list<boost::filesystem::path>& stateFiles =
      boost::any_cast<std::list<boost::filesystem::path>&>(dialogs->data());
    if (stateFiles.size() <= 1)
    {
      _dialogs.remove(callId);
      return;
    }
    //
    // Do it the hard way
    //
    std::string tag;
    std::string fromTag = from.getTag();
    std::string toTag = to.getTag();
    if (pMsg->isRequest())
      tag = toTag;
    else
      tag = fromTag;
   
    boost::filesystem::path& first = stateFiles.front();
    boost::filesystem::path& last = stateFiles.back();
    try
    {  
      ClassType persistent1;
      if (!persistent1.load(first))
        return;
      DataType root1 = persistent1.self();
      DataType leg1 = root1["leg-1"];
      //
      // Check if the to tag corresponds to the dialog leg1 from tag of the first state file
      //
      std::string leg1From = (const char*)leg1["from"];
      std::string leg1FromTag = SIPFrom::getTag(leg1From);
      if (leg1FromTag == tag)
      {
        stateFiles.pop_front();
        if (stateFiles.empty())
        {
          _dialogs.remove(callId);
          return;
        }
        
        if (deleteFile)
        {
          ClassType::remove(first);
        }
        
        OSS_LOG_DEBUG("Successfully removed dialog " << callId << " with sessionId " << boost_file_name(first));
        return;
      }
      else
      {
        ClassType persistent2;
        if (!persistent1.load(last))
          return;

        DataType root2 = persistent2.self();
        DataType leg2 = root1["leg-2"];
        //
        // Check if the to tag corresponds to the dialog leg2 from tag of the last state file
        //
        std::string leg2From = (const char*)leg2["from"];
        std::string leg2FromTag = SIPFrom::getTag(leg2From);
        if (leg2FromTag == tag)
        {
          stateFiles.pop_back();
          if (stateFiles.empty())
          {
            _dialogs.remove(callId);
            return;
          }
          
          if (deleteFile)
          {
            ClassType::remove(last);
          }
          
          OSS_LOG_DEBUG("Successfully removed dialog " << callId << " with sessionId " << boost_file_name(last));
          return;
        }
      }
    }
    catch(...)
    {
      return;
    }
  }
  else
  {
    OSS_LOG_ERROR("Unable to remove dialog for Call-ID " << callId);
  }
}

bool SBCDialogStateManager::findDialog(const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pMsg, boost::filesystem::path& stateFile)
{
  OSS::mutex_critic_sec_lock lock(_csDialogsMutex);
  std::string callId = pMsg->hdrGet("call-id");
  OSS_VERIFY(!callId.empty());
  SIPFrom from = pMsg->hdrGet("from");
  SIPTo to = pMsg->hdrGet("to");
  if (_dialogs.has(callId))
  {
    Cacheable::Ptr dialogs = _dialogs.get(callId);
    std::list<boost::filesystem::path>& stateFiles =
      boost::any_cast<std::list<boost::filesystem::path>&>(dialogs->data());
    if (stateFiles.size() == 1)
    {
      stateFile = stateFiles.front();
      return true;
    }
    //
    // Do it the hard way
    //
    std::string tag;
    std::string fromTag = from.getTag();
    std::string toTag = to.getTag();
    if (pMsg->isRequest())
      tag = toTag;
    else
      tag = fromTag;

    boost::filesystem::path& first = stateFiles.front();
    boost::filesystem::path& last = stateFiles.back();
    try
    {
      ClassType persistent1;
      if (!persistent1.load(first))
        return false;
      DataType root1 = persistent1.self();
      DataType leg1 = root1["leg-1"];
      //
      // Check if the to tag corresponds to the dialog leg1 from tag of the first state file
      //
      std::string leg1From = (const char*)leg1["from"];
      std::string leg1FromTag = SIPFrom::getTag(leg1From);
      if (leg1FromTag == tag)
      {
        stateFile = stateFiles.front();
        return true;
      }
      else
      {
        ClassType persistent2;
        if (!persistent1.load(last))
          return false;
        DataType root2 = persistent2.self();
        DataType leg2 = root1["leg-2"];
        //
        // Check if the to tag corresponds to the dialog leg2 from tag of the last state file
        //
        std::string leg2From = (const char*)leg2["from"];
        std::string leg2FromTag = SIPFrom::getTag(leg2From);
        if (leg2FromTag == tag)
        {
          stateFile = stateFiles.back();
          return true;
        }
      }
    }
    catch(...)
    {
      return false;
    }
  }
  return false;
}

bool SBCDialogStateManager::findReplacesTarget(
    const SIPMessage::Ptr& pMsg,
    SIPB2BDialogData::LegInfo& legInfo)
{
  OSS::SIP::SIPReplaces replaces(pMsg->hdrGet(OSS::SIP::HDR_REPLACES));
    
    
  std::string logId = pMsg->createContextId(true);
  std::string callId = replaces.getCallId();
  if (callId.empty())
  {
    OSS_LOG_ERROR(logId << "SBCDialogStateManager::findReplacesTarget- Unable to determine Call-ID while calling findReplacesTarget.");
    return false;
  }
  
  OSS::mutex_critic_sec_lock lock(_csDialogsMutex);
  if (_dialogs.has(callId))
  {
    OSS_LOG_DEBUG(logId << "SBCDialogStateManager::findReplacesTarget - Dialog database has a record for Call-ID: " << callId);
    Cacheable::Ptr dialogs = _dialogs.get(callId);
    DialogList& dialogList = boost::any_cast<DialogList&>(dialogs->data());
     
    for (DialogList::const_iterator iter = dialogList.begin(); iter != dialogList.end(); iter++)
    {

      std::string leg1LocalTag = SIPFrom::getTag(iter->leg1.to);
      std::string leg1RemoteTag = SIPFrom::getTag(iter->leg1.from);
          
      if (leg1LocalTag == replaces.getToTag() && leg1RemoteTag == replaces.getFromTag())
      {
        OSS_LOG_INFO(logId << "SBCDialogStateManager::findReplacesTarget - will be replacing leg 1 : " << callId);
        legInfo = iter->leg1;
        return true;
      }
          
      std::string leg2LocalTag = SIPFrom::getTag(iter->leg2.to);
      std::string leg2RemoteTag =  SIPFrom::getTag(iter->leg2.from);
      if (leg2LocalTag == replaces.getToTag() && leg2RemoteTag == replaces.getFromTag())
      {
        OSS_LOG_INFO(logId << "SBCDialogStateManager::findReplacesTarget - will be replacing leg 2 : " << callId);
        legInfo = iter->leg2;
        return true;
      }
    }
  }
  
  OSS_LOG_INFO(logId << "SBCDialogStateManager::findReplacesTarget - did not find any target for : " << callId);
  
  return false;
}

void SBCDialogStateManager::onUpdateInitialUASState(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction,
  const std::string& sessionId)
{
  if (!pResponse->is3xx())
    pResponse->hdrListRemove("contact");

  if (!pResponse->is1xx(100))
  {
    //
    // Prepare the new contact
    //
    
    if (!pResponse->is3xx())
    {
      SBCContact::SessionInfo sessionInfo;
      sessionInfo.sessionId = sessionId;
      sessionInfo.callIndex = 1;
      SBCContact::transform(this->_pManager,
        pResponse,
        pTransaction,
        pTransaction->serverTransport()->getLocalAddress(),
        sessionInfo);
    }
    //
    // Save the dialog state
    //
    if (pResponse->isMidDialog() && (pResponse->is2xx() || pResponse->is1xx()))
    {
      try
      {
        //
        // Preserve leg 1 dialog state
        //
        boost::filesystem::path stateFile = operator/(_pManager->getDialogStateDirectory(), sessionId);

        ClassType leg1Persistent;
      
        if (!leg1Persistent.load(stateFile))
        {
          OSS_LOG_WARNING("Unable to load state file " << OSS::boost_path(stateFile));
          return;
        }
        
        DataType root = leg1Persistent.self();
        
        //
        // If we already have an early dialog for leg-1,
        // then there is no need to update the dialog again
        //
        if (root.exists("leg-1"))
        {
          //
          // Just add the early dialog to cache
          //
          if (pResponse->is2xx())
          {
            addDialog(pResponse->hdrGet("call-id"), stateFile);
          }
          return;
        }
        
        DataType leg1Dialog = root.addGroupElement("leg-1", DataType::TypeGroup);

        DataType callId = leg1Dialog.addGroupElement("call-id", DataType::TypeString);
        callId = pResponse->hdrGet("call-id").c_str();

        DataType from = leg1Dialog.addGroupElement("from", DataType::TypeString);
        from = pResponse->hdrGet("to").c_str();

        DataType to = leg1Dialog.addGroupElement("to", DataType::TypeString);
        to = pResponse->hdrGet("from").c_str();

        DataType remoteContact = leg1Dialog.addGroupElement("remote-contact", DataType::TypeString);
        remoteContact = pTransaction->serverRequest()->hdrGet("contact").c_str();

        std::string lContact;
        DataType localContact = leg1Dialog.addGroupElement("local-contact", DataType::TypeString);
        localContact = pResponse->hdrGet("contact").c_str();
        
        std::string dialogStatePlacement;
        pTransaction->getProperty("dialog-state-placement", dialogStatePlacement);
        bool dialogStateInParams = SBCContact::_dialogStateInParams;
        bool dialogStateInRecordRoute = SBCContact::_dialogStateInRecordRoute;
        if (dialogStatePlacement == "c")
        {
          dialogStateInParams = true;
          dialogStateInRecordRoute = false;
        }
        else if (dialogStatePlacement == "r")
        {
          dialogStateInParams = false;
          dialogStateInRecordRoute = true;
        }

        if (!dialogStateInParams && dialogStateInRecordRoute && pResponse->hdrPresent(OSS::SIP::HDR_RECORD_ROUTE))
        {
          //
          // Check if we have a record-route containing the session information
          //
          std::string routeList = pResponse->hdrGet(OSS::SIP::HDR_RECORD_ROUTE);
          if (!routeList.empty())
          {
            RouteURI routeUri;
            SIPRoute::getAt(routeList, routeUri, 0);
            SIPURI uri = routeUri.getURI();
            if (uri.hasParam("sbc-session-id") && uri.hasParam("sbc-call-index"))
            {
              std::string lRecordRoute;
              DataType localRecordRoute = leg1Dialog.addGroupElement("local-rr", DataType::TypeString);
              localRecordRoute = pResponse->hdrGet("record-route").c_str();
            }
          }
        }

        DataType packetSrc = leg1Dialog.addGroupElement("remote-ip", DataType::TypeString);
        packetSrc = pTransaction->serverTransport()->getRemoteAddress().toIpPortString().c_str();

        //
        // Preserve the transport identifier
        // so we can recycle it for requests in mid dialog
        //
        DataType transportId = leg1Dialog.addGroupElement("transport-id", DataType::TypeString);
        transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier()).c_str();

        DataType transportScheme = leg1Dialog.addGroupElement("target-transport", DataType::TypeString);
        transportScheme = pTransaction->serverTransport()->getTransportScheme().c_str();

        std::string contentType = pResponse->hdrGet("content-type");
        OSS::string_to_lower(contentType);
        if (!pResponse->body().empty() && contentType == "application/sdp")
        {
          DataType localSDP = leg1Dialog.addGroupElement("local-sdp", DataType::TypeString);
          localSDP = pResponse->body();
        }
        
        contentType = pTransaction->serverRequest()->hdrGet("content-type");
        OSS::string_to_lower(contentType);
        if (!pTransaction->serverRequest()->body().empty() && contentType == "application/sdp")
        {
          DataType remoteSDP = leg1Dialog.addGroupElement("remote-sdp", DataType::TypeString);
          remoteSDP = pTransaction->serverRequest()->body();
        }

        if (pResponse->hdrGetSize("record-route") > 0)
        {
          //
          // Preserve the record routes
          //
          DataType routes = leg1Dialog.addGroupElement("routes", DataType::TypeArray);
          std::list<std::string> recordRoutes;
          SIPRoute::msgGetRecordRoutes(pResponse.get(), recordRoutes);

          for (std::list<std::string>::reverse_iterator iter = recordRoutes.rbegin();
            iter != recordRoutes.rend(); iter++)
          {
            DataType routeRecord = routes.addArrayOrListElement(DataType::TypeString);
            routeRecord = (*iter).c_str();
          }
        }

        //
        // Check if the server request is XOR encrypted
        //
        if (pTransaction->serverTransaction()->isXOREncrypted())
        {
          DataType xorEncrypted = leg1Dialog.addGroupElement("ENC", DataType::TypeBoolean);
          xorEncrypted = true;
        }

        std::string noRTPProxy;
        if (pTransaction->getProperty("no-rtp-proxy", noRTPProxy) && noRTPProxy == "1")
        {
          DataType  noRTPProxyProp = leg1Dialog.addGroupElement("no-rtp-proxy", DataType::TypeBoolean);
          noRTPProxyProp = true;
        }

        leg1Persistent.persist(stateFile);

        if (pResponse->is2xx())
        {
          addDialog(pResponse->hdrGet("call-id"), stateFile);
        }
      }
      catch(OSS::Exception e)
      {
        std::ostringstream logMsg;
        logMsg << pTransaction->getLogId() << "Unable to save dialog state for leg 1 - " << e.message();
        OSS::log_error(logMsg.str());
      }
    }
    else if (pResponse->isErrorResponse())
    {
      //
      // Remove the state files if they were created
      //
      try
      {
        boost::filesystem::path stateFile = operator/(_pManager->getDialogStateDirectory(), sessionId);
        ClassType::remove(stateFile);
      }catch(...){}
    }
  }
}

void SBCDialogStateManager::onUpdateInitialUACState(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction,
  const std::string& sessionId)
{
  if (pResponse->is1xx(100))
    return;

  if (pResponse->isMidDialog() && (pResponse->is2xx() || pResponse->is1xx()))
  {
    try
    {
      boost::filesystem::path stateFile = operator/(_pManager->getDialogStateDirectory(), sessionId);
      //
      // Preserve leg 2 dialog state
      //
      ClassType leg2Persistent;
      if (boost::filesystem::exists(stateFile))
      {
        leg2Persistent.load(stateFile);
      }
      
      DataType root = leg2Persistent.self();
      if (root.exists("leg-2"))
      {
        //
        // we already have an early dialog.  Just update contact, to and record-routes just in case the call was forked
        //
        DataType leg2Dialog = root["leg-2"];
        leg2Dialog["to"] = pResponse->hdrGet("to").c_str();
        
        //
        // There are some idiots who do not send contact in 183 with to-tags
        //
        if (leg2Dialog.exists("remote-contact"))
        {
          leg2Dialog["remote-contact"] = pResponse->hdrGet("contact").c_str();
        }
        else
        {
          DataType remoteContact = leg2Dialog.addGroupElement("remote-contact", DataType::TypeString);
          remoteContact = pResponse->hdrGet("contact").c_str();
        }
        
        //
        // Same as contact
        //
        if (pResponse->hdrGetSize("record-route") > 0)
        {
          //
          // Preserve the record routes
          //
          if (leg2Dialog.exists("routes"))
          {
            leg2Dialog.remove("routes");
          }
          DataType routes = leg2Dialog.addGroupElement("routes", DataType::TypeArray);
          std::list<std::string> recordRoutes;
          SIPRoute::msgGetRecordRoutes(pResponse.get(), recordRoutes);

          for (std::list<std::string>::iterator iter = recordRoutes.begin();
            iter != recordRoutes.end(); iter++)
          {
            DataType routeRecord = routes.addArrayOrListElement(DataType::TypeString);
            routeRecord = (*iter).c_str();
          }
        }
        leg2Persistent.persist(stateFile);
        return;
      }
      
      DataType leg2Dialog = root.addGroupElement("leg-2", DataType::TypeGroup);

      DataType callId = leg2Dialog.addGroupElement("call-id", DataType::TypeString);
      callId = pResponse->hdrGet("call-id").c_str();

      DataType from = leg2Dialog.addGroupElement("from", DataType::TypeString);
      from = pResponse->hdrGet("from").c_str();

      DataType to = leg2Dialog.addGroupElement("to", DataType::TypeString);
      to = pResponse->hdrGet("to").c_str();

      DataType remoteContact = leg2Dialog.addGroupElement("remote-contact", DataType::TypeString);
      remoteContact = pResponse->hdrGet("contact").c_str();

      DataType localCSeq = leg2Dialog.addGroupElement("local-cseq", DataType::TypeInt);
      std::string seqNum;
      SIPCSeq::getNumber(pResponse->hdrGet("cseq"), seqNum);
      localCSeq = OSS::string_to_number<int>(seqNum.c_str());
      
      DataType localInviteCSeq = leg2Dialog.addGroupElement("local-invite-cseq", DataType::TypeInt);
      localInviteCSeq = OSS::string_to_number<int>(seqNum.c_str());

      std::string lContact;
      OSS_VERIFY(pTransaction->getProperty("leg2-contact", lContact));
      DataType localContact = leg2Dialog.addGroupElement("local-contact", DataType::TypeString);
      localContact = lContact.c_str();

      std::string lRecordRoute;
      if (pTransaction->getProperty("leg2-rr", lRecordRoute) && !lRecordRoute.empty())
      {
        DataType localRR = leg2Dialog.addGroupElement("local-rr", DataType::TypeString);
        localRR = lRecordRoute.c_str();
      }

      DataType packetSrc = leg2Dialog.addGroupElement("remote-ip", DataType::TypeString);
      packetSrc = pTransaction->clientTransport()->getRemoteAddress().toIpPortString().c_str();

      DataType transportId = leg2Dialog.addGroupElement("transport-id", DataType::TypeString);
      transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->clientTransport()->getIdentifier()).c_str();

      DataType transportScheme = leg2Dialog.addGroupElement("target-transport", DataType::TypeString);
      transportScheme = pTransaction->clientTransport()->getTransportScheme().c_str();

      std::string contentType = pResponse->hdrGet("content-type");
      OSS::string_to_lower(contentType);
      if (!pResponse->body().empty() && contentType == "application/sdp")
      {
        DataType remoteSDP = leg2Dialog.addGroupElement("remote-sdp", DataType::TypeString);
        remoteSDP = pResponse->body();
      }

      contentType = pTransaction->clientRequest()->hdrGet("content-type");
      OSS::string_to_lower(contentType);
      if (!pTransaction->clientRequest()->body().empty() && contentType == "application/sdp")
      {
        DataType localSDP = leg2Dialog.addGroupElement("local-sdp", DataType::TypeString);
        localSDP = pTransaction->clientRequest()->body();
      }

      std::string noRTPProxy;
      if (pTransaction->getProperty("no-rtp-proxy", noRTPProxy) && noRTPProxy == "1")
      {
        DataType  noRTPProxyProp = leg2Dialog.addGroupElement("no-rtp-proxy", DataType::TypeBoolean);
        noRTPProxyProp = true;
      }

      if (pResponse->hdrGetSize("record-route") > 0)
      {
        //
        // Preserve the record routes
        //
        DataType routes = leg2Dialog.addGroupElement("routes", DataType::TypeArray);
        std::list<std::string> recordRoutes;
        SIPRoute::msgGetRecordRoutes(pResponse.get(), recordRoutes);

        for (std::list<std::string>::iterator iter = recordRoutes.begin();
          iter != recordRoutes.end(); iter++)
        {
          DataType routeRecord = routes.addArrayOrListElement(DataType::TypeString);
          routeRecord = (*iter).c_str();
        }
      }

      //
      // Check if the client request is XOR encrypted
      //
      if (pTransaction->clientTransaction()->isXOREncrypted())
      {
        DataType xorEncrypted = leg2Dialog.addGroupElement("ENC", DataType::TypeBoolean);
        xorEncrypted = true;
      }
      
      //
      // Check if X-CID header needs to be inserted
      //
      std::string xCid_;
      if (pTransaction->getProperty("X-CID", xCid_))
      {
        DataType xCid = leg2Dialog.addGroupElement("X-CID", DataType::TypeString);
        xCid = xCid_.c_str();
      }
      
      //
      // Check if we will handle 100rel locally
      //
      std::string local100RelValue;
      if (pTransaction->getProperty("local-100-rel", local100RelValue) && local100RelValue == "1")
      {
        DataType  local100Rel = root.addGroupElement("local-100-rel", DataType::TypeBoolean);
        local100Rel = true;
        OSS_LOG_DEBUG(pTransaction->getLogId() << "Local reliable provisional response handling ENABLED");
      }
      
      //
      // Check if we will handle UPDATE locally
      //
      std::string localUpdateValue;
      if (pTransaction->getProperty("local-update", localUpdateValue) && localUpdateValue == "1")
      {
        DataType  localUpdate = root.addGroupElement("local-update", DataType::TypeBoolean);
        localUpdate = true;
        OSS_LOG_DEBUG(pTransaction->getLogId() << "Local UPDATE request handling ENABLED");
      }
      
      leg2Persistent.persist(stateFile);
    }
    catch(OSS::Exception e)
    {
      std::ostringstream logMsg;
      logMsg << pTransaction->getLogId() << "Unable to save dialog state for leg 2 - " << e.message();
      OSS::log_error(logMsg.str());
    }
  }
}


void SBCDialogStateManager::onUpdateReinviteUASState(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction,
  const std::string& sessionId)
{
  pResponse->hdrListRemove("contact");
  if (!pResponse->is1xx(100))
  {
    std::string userId;
    std::string legIndexNumber;
    OSS_VERIFY(pTransaction->getProperty("leg-identifier", userId));
    OSS_VERIFY(pTransaction->getProperty("leg-index", legIndexNumber));

    //
    // Prepare the new contact
    //
    SBCContact::SessionInfo sessionInfo;
    sessionInfo.sessionId = sessionId;
    sessionInfo.callIndex = OSS::string_to_number<unsigned>(legIndexNumber.c_str());
    SBCContact::transform(this->_pManager,
      pResponse,
      pTransaction,
      pTransaction->serverTransport()->getLocalAddress(),
      sessionInfo);


    //
    // Save the dialog state
    //
    if (pResponse->is2xx())
    {
      std::string legIndex;
      try
      {
        std::string callerDialogFile = sessionId;
        
        if (legIndexNumber == "1")
           legIndex = "leg-1";
        else
          legIndex= "leg-2";

        //
        // Preserve leg dialog state
        //
        boost::filesystem::path stateFile = operator/(_pManager->getDialogStateDirectory(), callerDialogFile);
        ClassType legPersistent;
        if (!legPersistent.load(stateFile))
          return;
        DataType root = legPersistent.self();
        DataType legDialog = root[legIndex];

        DataType remoteContact = legDialog["remote-contact"];
        remoteContact = pTransaction->serverRequest()->hdrGet("contact").c_str();

        DataType packetSrc = legDialog["remote-ip"];
        packetSrc = pTransaction->serverTransport()->getRemoteAddress().toIpPortString().c_str();

        DataType transportId = legDialog["transport-id"];
        transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier()).c_str();


        std::string contentType = pResponse->hdrGet("content-type");
        OSS::string_to_lower(contentType);
        if (!pResponse->body().empty() && contentType == "application/sdp")
        {
          if (!legDialog.exists("local-sdp"))
          {
            DataType localSDP = legDialog.addGroupElement("local-sdp", DataType::TypeString);
            localSDP = pResponse->body();
          }
          else
          {
            legDialog["local-sdp"] = pResponse->body().c_str();
          }
        }

        contentType = pTransaction->serverRequest()->hdrGet("content-type");
        OSS::string_to_lower(contentType);
        if (!pTransaction->serverRequest()->body().empty() && contentType == "application/sdp")
        {
          if (!legDialog.exists("remote-sdp"))
          {
            DataType remoteSDP = legDialog.addGroupElement("remote-sdp", DataType::TypeString);
            remoteSDP = pTransaction->serverRequest()->body();
          }
          {
            legDialog["remote-sdp"] = pTransaction->serverRequest()->body().c_str();
          }
        }

        legPersistent.persist(stateFile);
      }
      catch(OSS::Exception e)
      {
        std::ostringstream logMsg;
        logMsg << pTransaction->getLogId() << "Unable to save dialog state for " << legIndex << " - " << e.message();
        OSS::log_error(logMsg.str());
      }
    }
  }
}

void SBCDialogStateManager::onUpdateReinviteUACState(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction,
  const std::string& sessionId)
{

  //
  // Reinvites are target refresh so update the dialog state once we get the final response
  //
  std::string userId;
  std::string legIndexNumber;
  OSS_VERIFY(pTransaction->getProperty("leg-identifier", userId));
  OSS_VERIFY(pTransaction->getProperty("leg-index", legIndexNumber));

  if (pResponse->is2xx())
  {
    std::string calleeDialogFile = sessionId;
    std::string legIndex;
    if (legIndexNumber == "1")
       legIndex = "leg-2";
    else
      legIndex= "leg-1";

    try
    {
      //
      // Preserve leg dialog state
      //
      boost::filesystem::path stateFile = operator/(_pManager->getDialogStateDirectory(), calleeDialogFile);
      ClassType legPersistent;
      if (!legPersistent.load(stateFile))
        return;
      DataType root = legPersistent.self();
      DataType legDialog = root[legIndex];

      DataType remoteContact = legDialog["remote-contact"];
      remoteContact = pResponse->hdrGet("contact").c_str();

      DataType packetSrc = legDialog["remote-ip"];
      packetSrc = pTransaction->clientTransport()->getRemoteAddress().toIpPortString().c_str();

      DataType transportId = legDialog["transport-id"];
      transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->clientTransport()->getIdentifier()).c_str();
      
      SIPCSeq cseq =  pResponse->hdrGet(SIP::HDR_CSEQ);
      if (legDialog.exists("local-invite-cseq"))
      {
        DataType localInviteCSeq = legDialog["local-invite-cseq"];
        localInviteCSeq = cseq.getNumberAsUint();
      }
      else
      {
        DataType localInviteCSeq = legDialog.addGroupElement("local-invite-cseq", DataType::TypeInt);
        localInviteCSeq = cseq.getNumberAsUint();
      }

      std::string contentType = pResponse->hdrGet("content-type");
      OSS::string_to_lower(contentType);
      if (!pResponse->body().empty() && contentType == "application/sdp")
      {
        if (!legDialog.exists("remote-sdp"))
        {
          DataType remoteSDP = legDialog.addGroupElement("remote-sdp", DataType::TypeString);
          remoteSDP = pResponse->body();
        }
        else
        {
          legDialog["remote-sdp"] = pResponse->body().c_str();
        }
      }

      contentType = pTransaction->clientRequest()->hdrGet("content-type");
      OSS::string_to_lower(contentType);
      if (!pTransaction->clientRequest()->body().empty() && contentType == "application/sdp")
      {
        if (!legDialog.exists("local-sdp"))
        {
          DataType localSDP = legDialog.addGroupElement("local-sdp", DataType::TypeString);
          localSDP = pTransaction->clientRequest()->body();
        }
        else
        {
          legDialog["local-sdp"] = pTransaction->clientRequest()->body().c_str();
        }
      }

      legPersistent.persist(stateFile);
    }
    catch(OSS::Exception e)
    {
      std::ostringstream logMsg;
      logMsg << pTransaction->getLogId() << "Unable to save dialog state for reinvite " << e.message();
      OSS::log_error(logMsg.str());
    }
  }
}

void SBCDialogStateManager::run(const boost::filesystem::path& stateDirectory)
{
  if (_pThread)
    OSS_VERIFY(false);
  _stateDirectory = stateDirectory;
  _pThread = new boost::thread(boost::bind(&SBCDialogStateManager::runTask, this));
}

void SBCDialogStateManager::stop()
{
  if (_pThread)
  {
    _exitSync.set();
    _pThread->join();
    delete _pThread;
    _pThread = 0;
  }
}

void SBCDialogStateManager::runTask()
{
  OSS::log_information("SBC State File Manager started.");
  while(!_exitSync.tryWait(60000 * 60))
  {
    flushStale();
  }
  OSS::log_information("SBC State File Manager ended.");
}

void SBCDialogStateManager::flushStale()
{
  std::vector<boost::filesystem::path> staleFiles;
  try
  {
    boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
    for (boost::filesystem::directory_iterator itr(_stateDirectory);
          itr != end_itr;
          ++itr)
    {
      if (boost::filesystem::is_directory(itr->status()))
      {
        continue;
      }
      else
      {
        boost::filesystem::path currentFile = operator/(_stateDirectory, boost_file_name(itr->path()));
        if (boost::filesystem::is_regular(currentFile) && OSS::isFileOlderThan(currentFile, _stateFileMaxLifeTime))
        {
          staleFiles.push_back(currentFile);
        }
      }
    }

    for (std::vector<boost::filesystem::path>::const_iterator iter = staleFiles.begin();
      iter != staleFiles.end(); iter++)
    {
      std::string callId;
      try
      {
        ClassType leg1Persistent;
        if (leg1Persistent.load(*iter))
        {
          DataType root = leg1Persistent.self();
          callId = (const char*)root["leg-1"]["call-id"];
        }
      }catch(...){}
      
      if (!callId.empty())
      {
        _csDialogsMutex.lock();
        _dialogs.remove(callId);
        _csDialogsMutex.unlock();
      }
      ClassType::remove(*iter);
    }
  }
  catch(OSS::Exception e)
  {
    std::ostringstream logMsg;
    logMsg << "purge_state_files() Failure - "
      << e.message();
    OSS::log_warning(logMsg.str());
  }
}
bool SBCDialogStateManager::findDialog(const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pMsg, OSS::Persistent::ClassType& dialog)
{
  boost::filesystem::path stateFile;
  std::string senderLeg;
  std::string targetLeg;
  std::string sessionId;
  return findDialog(pTransaction, pMsg, pTransaction->getLogId(), stateFile, senderLeg, targetLeg, sessionId, dialog);
}

bool SBCDialogStateManager::findDialog(
const SIPB2BTransaction::Ptr& pTransaction,
  const SIPMessage::Ptr& pMsg,
  const std::string& logId,
  boost::filesystem::path& stateFile,
  std::string& senderLeg,
  std::string& targetLeg,
  std::string& sessionId,
  OSS::Persistent::ClassType& persistent)
{
  SIPRequestLine rline = pMsg->startLine();
  SIPURI requestUri;
  if (!rline.getURI(requestUri))
    return false;

  std::string method = pMsg->getMethod();
  std::string user = requestUri.getUser();
  //std::vector<std::string> userTokens = OSS::string_tokenize(user, "-");

  SBCContact::SessionInfo sessionInfo;
  if (!SBCContact::getSessionInfo(
    this->_pManager,
    pMsg,
    pTransaction,
    sessionInfo) )
  //if (userTokens.size() != 2 || userTokens[1].size() != 1)
  {
    OSS_LOG_DEBUG(logId << "Found none-compliant request-uri format - " << user);

    if (!findDialog(pTransaction, pMsg, stateFile))
    {
      OSS_LOG_WARNING(logId << "No existing dialog for " << method);
      return false;
    }

    std::string from = pMsg->hdrGet("from");
    std::string fromTag = SIPFrom::getTag(from);
    try
    {
      if (!persistent.load(stateFile))
        return false;
      DataType root = persistent.self();

      DataType leg1 = root["leg-1"];
      std::string leg1To =  (const char*)leg1["to"];
      std::string leg1ToTag = SIPFrom::getTag(leg1To);
      if (leg1ToTag == fromTag)
      {
        senderLeg = "1";
        targetLeg = "leg-2";
        sessionId = boost_file_name(stateFile);
      }
      else
      {
        DataType leg2 = root["leg-2"];
        std::string leg2To =  (const char*)leg2["to"];
        std::string leg2ToTag = SIPFrom::getTag(leg2To);
        if (leg2ToTag == fromTag)
        {
          senderLeg = "2";
          targetLeg = "leg-1";
          sessionId = boost_file_name(stateFile);
        }
        else
        {
          OSS_LOG_WARNING(logId << "Unable to determine dialog for " << method);
          return false;
        }
      }
    }catch(OSS::Exception e)
    {
      OSS_LOG_WARNING(logId << "No existing dialog state file for " << method << " - " << e.message());
      return false;
    }
  }
  else
  {
    sessionId = sessionInfo.sessionId;
    senderLeg = OSS::string_from_number<unsigned>(sessionInfo.callIndex);

    if (senderLeg == "1")
      targetLeg = "leg-2";
    else
      targetLeg = "leg-1";
    stateFile = operator/(_pManager->getDialogStateDirectory(), sessionId);
    if (!boost::filesystem::exists(stateFile))
    {
      OSS_LOG_DEBUG(logId << "Found compliant request-uri format but no state file exists for " << stateFile);
      if (!findDialog(pTransaction, pMsg, stateFile))
      {
        OSS_LOG_WARNING(logId << "No existing dialog for " << method);
        return false;
      }

      std::string from = pMsg->hdrGet("from");
      std::string fromTag = SIPFrom::getTag(from);
      try
      {
        if (!persistent.load(stateFile))
          return false;
        DataType root = persistent.self();

        DataType leg1 = root["leg-1"];
        std::string leg1To =  (const char*)leg1["to"];
        std::string leg1ToTag = SIPFrom::getTag(leg1To);
        if (leg1ToTag == fromTag)
        {
          senderLeg = "1";
          targetLeg = "leg-2";
          sessionId = boost_file_name(stateFile);
        }
        else
        {
          DataType leg2 = root["leg-2"];
          std::string leg2To =  (const char*)leg2["to"];
          std::string leg2ToTag = SIPFrom::getTag(leg2To);
          if (leg2ToTag == fromTag)
          {
            senderLeg = "2";
            targetLeg = "leg-1";
            sessionId = boost_file_name(stateFile);
          }
          else
          {
            OSS_LOG_WARNING(logId << "Unable to determine dialog for " << method);
            throw SBCStateException("Unable to determine dialog");
          }
        }
      }catch(OSS::Exception e)
      {
        OSS_LOG_WARNING(logId << "No existing dialog state file for " << method << " - " << e.message() );
        return false;
      }
    }
    else
    {
      sessionId = boost_file_name(stateFile);
      if (!persistent.load(stateFile))
        return false;
    }
  }
  senderLeg = "leg-" + senderLeg;
  return true;
}

SIPMessage::Ptr SBCDialogStateManager::onRouteMidDialogTransaction(
  SIPMessage::Ptr& pMsg,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localAddress,
  OSS::Net::IPAddress& targetAddress)
{
  SIPRequestLine rline = pMsg->startLine();
  SIPURI requestUri;
  std::string oldVia;
  SIPVia::msgGetTopVia(pMsg.get(), oldVia);
  std::string branch;
  SIPVia::getBranch(oldVia, branch);
  if (branch.empty())
  {
    //SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_400_BadRequest, "Missing Via Branch Parameter");
    //return serverError;
    branch = SIPParser::createBranchString();
  }
  if (!rline.getURI(requestUri))
  {
    SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_400_BadRequest, "Bad Request URI");
    return serverError;
  }

  const std::string& logId = pTransaction->getLogId();
  boost::filesystem::path stateFile;
  std::string senderLeg;
  std::string targetLeg;
  std::string sessionId;
  ClassType persistent;

  try
  {
    //
    // HACK: This can be a NOTIFY for a subscribe transaction that has not received a 2xx just yet
    // we wait for it for 1 second prior to failing
    //
    if (pMsg->isRequest("NOTIFY") && _pManager->transactionManager().isSubscriptionPending(pMsg->hdrGet(OSS::SIP::HDR_CALL_ID)))
    {
      OSS_LOG_WARNING(logId << "Received a NOTIFY prior to receiving a final response for the subscription.  Waiting for subscription transaction to end.");
      if (_pManager->transactionManager().getMaxThreadCount() > 1)
      {
        for (int i = 0; i < 20; i++)
        {
          OSS::thread_sleep(50);
          if (_pManager->transactionManager().isSubscriptionPending(pMsg->hdrGet(OSS::SIP::HDR_CALL_ID)))
          {
            continue;
          }
          else
          {
            break;
          }
        }
      }
    }
      
    if (!findDialog(pTransaction, pMsg, logId, stateFile, senderLeg, targetLeg, sessionId, persistent))
    {
      SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_481_TransactionDoesNotExist, "Unable to match dialog");
      return serverError;
    }

    //
    // Preserve the contact user to be used by the outbound response later on
    //
    pTransaction->setProperty("leg-identifier", requestUri.getUser());
    if (senderLeg == "leg-1")
      pTransaction->setProperty("leg-index", "1");
    else
      pTransaction->setProperty("leg-index", "2");
    pTransaction->setProperty("session-id", boost_file_name(stateFile));

    DataType root = persistent.self();
    DataType dialog = root[targetLeg];
    
    //
    // Set global transaction properties
    //
    if (root.exists("local-100-rel") && (bool)root["local-100-rel"])
    {
      pTransaction->setProperty("local-100-rel", "1");
    }

    if (root.exists("local-update") && (bool)root["local-update"])
    {
      pTransaction->setProperty("local-update", "1");
    }
    
    std::string callId = (const char*)dialog["call-id"];
    SIPFrom from;
    from = (const char*)dialog["from"];
    SIPFrom to;
    to = (const char*)dialog["to"];
    SIPFrom remoteContact;
    remoteContact = (const char*)dialog["remote-contact"];
    SIPFrom localContact;
    localContact = (const char*)dialog["local-contact"];
    std::string remoteIp = (const char*)dialog["remote-ip"];

    std::string localRR;
    if (!SBCContact::_dialogStateInParams && SBCContact::_dialogStateInRecordRoute && dialog.exists("local-rr"))
    {
      localRR = (const char*)dialog["local-rr"];
    }

    if (dialog.exists("no-rtp-proxy"))
    {
      bool noRTPProxy = (bool)dialog["no-rtp-proxy"];
      if (noRTPProxy)
        pTransaction->setProperty("no-rtp-proxy", "1");
    }

    std::string hCSeq = pMsg->hdrGet(SIP::HDR_CSEQ);
    int seqNum = 0;
    int requestSeqNum = 0;
    SIPCSeq::getNumber(hCSeq, requestSeqNum);
    if (!dialog.exists("local-cseq"))
    {
      DataType localCSeq = dialog.addGroupElement("local-cseq", DataType::TypeInt);
      localCSeq = seqNum = 1;
    }
    else
    {
      seqNum = (int)dialog["local-cseq"];
      seqNum++;
    }
    if (requestSeqNum > seqNum)
      seqNum = requestSeqNum;

    dialog["local-cseq"] = seqNum;
    
    if (dialog.exists("ENC") && (bool)dialog["ENC"])
      pMsg->setProperty("xor", "1");
    
    if (dialog.exists("X-CID"))
    {
      std::string xcid = (const char*)dialog["X-CID"];
      pMsg->hdrSet("X-CID", xcid);
    }

    persistent.persist(stateFile);

    pMsg->hdrRemove("call-id");
    pMsg->hdrRemove("from");
    pMsg->hdrRemove("to");
    pMsg->hdrRemove("cseq");
    pMsg->hdrListRemove("contact");
    pMsg->hdrListRemove("via");
    pMsg->hdrListRemove("route");
    pMsg->hdrListRemove("record-route");

    std::string method;
    rline.getMethod(method);
    std::ostringstream newRLine;
    newRLine << method << " " << remoteContact.getURI() << " SIP/2.0";
    pMsg->startLine() = newRLine.str();

    pMsg->hdrSet("From", from.data().c_str());
    pMsg->hdrSet("To", to.data().c_str());
    pMsg->hdrSet("Contact", localContact.data().c_str());
    pMsg->hdrSet("Call-ID", callId.c_str());

    std::ostringstream newCSeq;
    newCSeq  << seqNum << " " << method;
    pMsg->hdrSet("CSeq", newCSeq.str().c_str());

    localAddress = IPAddress::fromV4IPPort(localContact.getHostPort().c_str());
    //
    // The local contact may point to the external mapped address
    // so make sure we convert it back
    //
    _pManager->getInternalAddress(localAddress, localAddress);
    
    std::string transportScheme = "UDP";
    std::string transportId;
    if (dialog.exists("target-transport"))
    {
        transportScheme = (const char*)dialog["target-transport"];
        OSS::string_to_upper(transportScheme);
    }

    pMsg->setProperty("target-transport", transportScheme.c_str());

    if (dialog.exists("transport-id"))
    {
      transportId = (const char*)dialog["transport-id"];
      pMsg->setProperty("transport-id", transportId.c_str());
      OSS_LOG_DEBUG(logId << "Target transport identifier set by statefile: transport-id=" << transportId);
    }

    std::ostringstream via;

    if (!localAddress.externalAddress().empty())
      via << "SIP/2.0/" << transportScheme << " " << localAddress.externalAddress() << ":" << localAddress.getPort();
    else
      via << "SIP/2.0/" << transportScheme << " " << localAddress.toIpPortString();
    
    via << ";branch=" << branch << ";rport";
    
    
    pMsg->hdrSet(SIP::HDR_VIA, via.str().c_str());

    updateRouteSet(dialog, pMsg, targetAddress, logId);

    //
    // User request URI if no route is set
    //
    if (!targetAddress.isValid())
    {
      SIPURI requestTarget = remoteContact.getURI();
      std::string host;
      unsigned short port = 0;
      requestTarget.getHostPort(host, port);
      OSS::dns_host_record_list hosts = OSS::dns_lookup_host(host);
      if (!hosts.empty())
      {
        if (port == 0)
          port = 5060;
        targetAddress = *(hosts.begin());
        targetAddress.setPort(port);
      }
    }

    if (!targetAddress.isValid() || (targetAddress.isPrivate() && transportScheme == "UDP"))
    {
      //
      // Still unable to resolve request-uri
      //
      targetAddress = IPAddress::fromV4IPPort(remoteIp.c_str());
    }
  }
  catch(...)
  {
    SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_481_TransactionDoesNotExist, "Unable to match dialog");
    return serverError;
  }

  if (!targetAddress.isValid())
  {
    SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_500_InternalServerError, "Can't Resolve Requested Target");
    return serverError;
  }

  unsigned short targetPort = targetAddress.getPort();
  if (targetPort == 0)
      targetPort = 5060;
  
  //
  // Allow the echo server to hijack this transaciton
  //

  pMsg->setProperty("target-address", targetAddress.toString());
  pMsg->setProperty("target-port", OSS::string_from_number<unsigned short>(targetPort));

 
  return SIPMessage::Ptr();
}

void SBCDialogStateManager::updateRouteSet(const OSS::Persistent::DataType& dialog, const SIPMessage::Ptr& pMsg, OSS::Net::IPAddress& targetAddress, const std::string& logId)
{
  std::vector<std::string> routeList;
  std::string localRR;
  if (dialog.exists("local-rr"))
  {
    localRR = (const char*)dialog["local-rr"];
    pMsg->hdrSet("Record-Route", localRR.c_str());
    OSS_LOG_INFO(logId << "Inserting record-route header " << localRR);
  }
  
  if (dialog.exists("routes"))
  {
    std::string rrHostPort;
    
    if (!localRR.empty())
    {
      SIPRoute rr(localRR);
      ContactURI rrUri;
      rr.getAt(rrUri, 0);
      rrHostPort = rrUri.getHostPort();
    }

    DataType routes = dialog["routes"];
    int count = routes.getElementCount();
    for (int i = count - 1; i >= 0; i--)
    {
      std::string route = (const char*)routes[i];
      SIPRoute rt(route);
      ContactURI rtUri;
      rt.getAt(rtUri, 0);
      
      if (!rrHostPort.empty())
      {
        std::string rtHostPort = rtUri.getHostPort();
        //
        // Skip the top most route header if it points back to us
        //
        if (i == count - 1 && rrHostPort == rtHostPort)
        {
          OSS_LOG_DEBUG(logId << "Removing route header " << route);
          continue;
        }
      }

      OSS_LOG_INFO(logId << "Inserting route header " << route);

      routeList.push_back(route);
    }
  }
  
  
  if (routeList.size() > 0)
  {
    SIPFrom topRoute = routeList[0];
    bool strictRoute = topRoute.data().find(";lr") == std::string::npos;
    if (strictRoute)
    {
      SIPFrom remoteContact;
      remoteContact = (const char*)dialog["remote-contact"];
    
      std::ostringstream sline;
      sline << pMsg->getMethod() << " " << topRoute.getURI() << " SIP/2.0";
      pMsg->startLine() = sline.str();
      std::ostringstream newRoute;
      newRoute << "<" << remoteContact.getURI() << ">";
      routeList[0] = newRoute.str();
    }

    //
    // msgAddRoute will insert items on top.  Use a reverse iterator here.
    //
    for (std::vector<std::string>::reverse_iterator iter = routeList.rbegin();
      iter != routeList.rend(); iter++)
    {
      SIPRoute::msgAddRoute(pMsg.get(), *iter);
    }

    SIPURI topRouteURI = topRoute.getURI();

    std::string host;
    unsigned short port = 0;
    topRouteURI.getHostPort(host, port);

    OSS::dns_host_record_list hosts = OSS::dns_lookup_host(host);
    if (!hosts.empty())
    {
      OSS_LOG_INFO(logId << "Setting target address from top-most route header " << host);
      if (port == 0)
        port = 5060;
      targetAddress = *(hosts.begin());
      targetAddress.setPort(port);
    }
  }
}

void SBCDialogStateManager::onRouteAckRequest(
  const SIPMessage::Ptr& pMsg,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  OSS::CacheManager& retransmitCache,
  std::string& sessionId,
  std::string& peerXOR,
  OSS::Net::IPAddress& routeLocalInterface,
  OSS::Net::IPAddress& targetAddress)
{
  std::string logId = pMsg->createContextId(true);
  if (!pMsg->isRequest("ACK"))
  {
    OSS_LOG_ERROR(logId << "Non-ACK Request fed to onRouteAckRequest" << pMsg->startLine());
    throw SBCStateException("Non-ACK Request fed to onRouteAckRequest");
    return;
  }

  OSS_LOG_DEBUG(logId << "Attempting to route " << pMsg->startLine());

  std::string maxForwards = pMsg->hdrGet("max-forwards");
  if (maxForwards.empty())
  {
    maxForwards = "70";
  }
  int maxF = OSS::string_to_number<int>(maxForwards.c_str());
  if (--maxF == 0)
  {
    throw SBCStateException("Max-forward reached.");
    return;
  }
  pMsg->hdrRemove("max-forwards");
  pMsg->hdrSet("Max-Forwards", OSS::string_from_number(maxF).c_str());

  pMsg->clearProperties();
  SIPRequestLine rline = pMsg->startLine();
  SIPURI requestUri;
  rline.getURI(requestUri);

  
  boost::filesystem::path stateFile;
  std::string senderLeg;
  std::string targetLeg;
  ClassType persistent;

  SIPB2BTransaction::Ptr pTransaction; /// DUMMY
  if (!findDialog(pTransaction, pMsg, logId, stateFile, senderLeg, targetLeg, sessionId, persistent))
  {
    OSS_LOG_DEBUG(logId << "No state file found for ACK request." );
    throw SBCStateException("No dialog exist.");
    return;
  }
  else
  {
    OSS_LOG_DEBUG(logId << "Found state file found for ACK request. Target leg = " << targetLeg << " " << stateFile );
  }

  try
  {
    DataType root = persistent.self();
    DataType dialog = root[targetLeg];
    std::string callId = (const char*)dialog["call-id"];
    SIPFrom from;
    from = (const char*)dialog["from"];
    SIPFrom to;
    to = (const char*)dialog["to"];
    SIPFrom remoteContact;
    remoteContact = (const char*)dialog["remote-contact"];
    SIPFrom localContact;
    localContact = (const char*)dialog["local-contact"];
    
    std::string remoteIp = (const char*)dialog["remote-ip"];
    bool isXOREncrypted = false;

    pMsg->getProperty("xor", peerXOR);
    pMsg->setProperty("peer-xor", peerXOR);

    if (dialog.exists("ENC") && (bool)dialog["ENC"])
    {
      pMsg->setProperty("xor", "1");
      isXOREncrypted = true;
    }

    std::string transportScheme = "UDP";
    std::string transportId;
    if (dialog.exists("target-transport"))
    {
        transportScheme = (const char*)dialog["target-transport"];
        OSS::string_to_upper(transportScheme);
        OSS_LOG_DEBUG(logId << "Target transport set by statefile: Transport=" << transportScheme);
    }
    else
    {
      OSS_LOG_WARNING(logId << "No target transport defined in dialog state.  Defaulting to UDP.");
    }

    pMsg->setProperty("target-transport", transportScheme.c_str());

    if (dialog.exists("transport-id"))
    {
        transportId = (const char*)dialog["transport-id"];
        pMsg->setProperty("transport-id", transportId.c_str());
        OSS_LOG_DEBUG(logId << "Target transport identifier set by statefile: transport-id=" << transportId);
    }
    else
    {
      OSS_LOG_WARNING(logId << "No persistent transport identifier defined in dialog state.  Defaulting to UDP.");
    }
    
    if (dialog.exists("X-CID"))
    {
      std::string xcid = (const char*)dialog["X-CID"];
      pMsg->hdrSet("X-CID", xcid);
    }
    else
    {
      pMsg->hdrRemove("X-CID");
    }

    pMsg->hdrRemove("call-id");
    pMsg->hdrRemove("from");
    pMsg->hdrRemove("to");
    pMsg->hdrListRemove("contact");
    pMsg->hdrListRemove("via");
    pMsg->hdrListRemove("route");
    pMsg->hdrListRemove("record-route");

    std::ostringstream newRLine;
    newRLine << "ACK " << remoteContact.getURI() << " SIP/2.0";
    pMsg->startLine() = newRLine.str();

    pMsg->hdrSet("From", from.data().c_str());
    pMsg->hdrSet("To", to.data().c_str());
    pMsg->hdrSet("Contact", localContact.data().c_str());
    pMsg->hdrSet("Call-ID", callId.c_str());

    SIPCSeq cseq = pMsg->hdrGet("cseq");
    cseq.setMethod("INVITE");

    std::ostringstream cacheId;
    cacheId << pMsg->getDialogId(true) << cseq.data();
    Cacheable::Ptr cacheItem = retransmitCache.get(cacheId.str());
    if (cacheItem)
    {
      SIPMessage::Ptr p2xx = boost::any_cast<SIPMessage::Ptr>(cacheItem->data());
      if (p2xx)
      {
        std::string oldVia;
        SIPVia::msgGetTopVia(p2xx.get(), oldVia);
        std::string branch;
        SIPVia::getBranch(oldVia, branch);
        if (branch.empty())
        {
          OSS_LOG_WARNING(logId << "Unable to process ACK. Old via appears to have no branch parameter.");
          throw SBCStateException("Unable to process ACK. Old via appears to have no branch parameter.");
        }

        OSS::Net::IPAddress viaHost = OSS::Net::IPAddress::fromV4IPPort(localContact.getHostPort().c_str());
        std::string newVia = SBCContact::constructVia(_pManager, pMsg, viaHost, transportScheme, branch);
        pMsg->hdrListPrepend("Via", newVia.c_str());
        
        DataType localInviteCseq = dialog["local-invite-cseq"];
        cseq.setMethod("ACK");
        cseq.setNumber((int)localInviteCseq);
        pMsg->hdrSet(SIP::HDR_CSEQ, cseq.data());
      }
      else
      {
        OSS_LOG_WARNING(logId << "Unable to process ACK. Cached 2xx is null.");
          throw SBCStateException("Unable to process ACK. Cached 2xx is null.");
      }
    }
    else
    {
      OSS_LOG_WARNING(logId <<"Unable to process ACK. There is no 2xx retransmisison in cache.");
      throw SBCStateException("Unable to process ACK. There is no 2xx retransmisison in cache.");
    }

    updateRouteSet(dialog, pMsg, targetAddress, logId);

    //
    // User request URI if no route is set
    //
    if (!targetAddress.isValid())
    {
      SIPURI requestTarget = remoteContact.getURI();
      std::string host;
      unsigned short port = 0;
      requestTarget.getHostPort(host, port);
      OSS::dns_host_record_list hosts = OSS::dns_lookup_host(host);
      if (!hosts.empty())
      {
        if (port == 0)
          port = 5060;
        targetAddress = *(hosts.begin());
        targetAddress.setPort(port);
      }
    }

    if (!targetAddress.isValid() || (targetAddress.isPrivate() && transportScheme == "UDP"))
    {
      //
      // Still unable to resolve request-uri
      //
      targetAddress = IPAddress::fromV4IPPort(remoteIp.c_str());
    }

    if (!_pManager->getUserAgentName().empty())
      pMsg->hdrSet("User-Agent", _pManager->getUserAgentName().c_str());

    std::string contentType = pMsg->hdrGet("content-type");
    OSS::string_to_lower(contentType);


    //
    // Set the local address to the local contact
    // This would have previously been set to the external address if one was used
    // So make sure we transform it back to it's original state
    //
    routeLocalInterface = IPAddress::fromV4IPPort(localContact.getHostPort().c_str());
    OSS_LOG_DEBUG(logId << "ACK Local interface preliminary address is " << routeLocalInterface.toString());
    _pManager->getInternalAddress(routeLocalInterface, routeLocalInterface);
    OSS_LOG_DEBUG(logId << "ACK Local interface post conversion address is " << routeLocalInterface.toString()
      << " external=(" <<  routeLocalInterface.externalAddress() << ")");

    if (contentType == "application/sdp")
    {
      std::string sdp = pMsg->getBody();
      RTPProxy::Attributes rtpAttributes;
      rtpAttributes.verbose = false;
      rtpAttributes.forcePEAEncryption = peerXOR == "1";
      rtpAttributes.forceCreate = false;

      DataType senderDialog = root[senderLeg.c_str()];
      if (!senderDialog.exists("remote-sdp"))
      {
        DataType remoteSDP = senderDialog.addGroupElement("remote-sdp", DataType::TypeString);
        remoteSDP = pMsg->body();
      }
      else
      {
        senderDialog["remote-sdp"] = pMsg->body().c_str();
      }

      
      //
      // Now set the B-Leg local interface using the transport info
      //
      OSS::Net::IPAddress packetLocalInterface = pTransport->getLocalAddress();
      packetLocalInterface.externalAddress() = pTransport->getExternalAddress();

      _pManager->rtpProxy().handleSDP(logId, sessionId,
              pTransport->getRemoteAddress(), // sentyBy
              pTransport->getRemoteAddress(), // packetSourceIp
              packetLocalInterface,  // packetLocalInterface
              targetAddress, // route
              routeLocalInterface,  // routeLocalInterface
              RTPProxySession::INVITE_ACK, sdp, rtpAttributes);

      pMsg->setBody(sdp);
      std::string clen = OSS::string_from_number<size_t>(sdp.size());
      pMsg->hdrSet("Content-Length", clen.c_str());

      if (!dialog.exists("local-sdp"))
      {
        DataType localSDP = dialog.addGroupElement("local-sdp", DataType::TypeString);
        localSDP = sdp;
      }
      else
      {
        dialog["local-sdp"] = sdp.c_str();
      }

      persistent.persist(stateFile);
    }

    pMsg->commitData();

    if (!pTransport->isEndpoint())
    {
      std::ostringstream logMsg;
      logMsg << logId << ">>> " << pMsg->startLine()
        << " LEN: " << pMsg->data().size()
        << " SRC: " << routeLocalInterface.toIpPortString()
        << " DST: " << targetAddress.toIpPortString()
        << " ENC: " << isXOREncrypted
        << " PROT: " << transportScheme;
      OSS::log_information(logMsg.str());

      if (OSS::log_get_level() >= OSS::PRIO_DEBUG)
        OSS::log_debug(pMsg->createLoggerData());
      
      SIPTransportService::dumpHepPacket(
        transportScheme == "udp" ? OSS::Net::IPAddress::UDP : OSS::Net::IPAddress::TCP,
        routeLocalInterface,
        targetAddress,
        pMsg->data()
      );
    }
  
  }
  catch(OSS::Exception e)
  {
    std::ostringstream logMsg;
    logMsg << logId << "Unable to process ACK.  Exception: " << e.message();
    OSS::log_warning(logMsg.str());
    throw;
  }
}

SIPMessage::Ptr SBCDialogStateManager::onMidDialogTransactionCreated(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  if (pRequest->isRequest("INVITE"))
  {
    std::string rfc2543HoldFix;
    if (pTransaction->getProperty("rfc2543-hold-fix", rfc2543HoldFix) && rfc2543HoldFix == "1")
    {
      fixRFC2543Hold(pRequest, pTransaction);
    }
  }
  return SIPMessage::Ptr();
}

void SBCDialogStateManager::fixRFC2543Hold(
  const SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  std::string contentType = pRequest->hdrGet("content-type");
  OSS::string_to_lower(contentType);
  if (contentType != "application/sdp")
    return;
  std::string newSDP = pRequest->body();
  if (newSDP.empty())
    return;

  SDPSession offer(newSDP.c_str());
  std::string sessionAddress = offer.getAddress();
  bool isBlackhole = (sessionAddress == "0.0.0.0");

  SDPMedia::Ptr audio = offer.getMedia(SDPMedia::TYPE_AUDIO);
  if (!audio)
    return;

  if (!isBlackhole)
  {
    std::string address = audio->getAddress();
    isBlackhole = (address == "0.0.0.0");
  }
  if (!isBlackhole)
    return;
  
  const std::string& logId = pTransaction->getLogId();
  boost::filesystem::path stateFile;
  std::string senderLeg;
  std::string targetLeg;
  std::string sessionId;
  OSS::Persistent::ClassType persistent;

  if (!findDialog(pTransaction, pRequest, logId, stateFile, senderLeg, targetLeg, sessionId, persistent))
    return;

  DataType root = persistent.self();
  DataType dialog = root[senderLeg];
  if (!dialog.exists("remote-sdp"))
    return;

  std::string oldSDP = (const char*)dialog["remote-sdp"];
  if (oldSDP.empty())
    return;

  SDPSession oldOffer(oldSDP.c_str());
  std::string oldSessionAddress = oldOffer.getAddress();
  SDPMedia::Ptr oldAudio = oldOffer.getMedia(SDPMedia::TYPE_AUDIO);
  if (!oldAudio)
    return;
  std::string oldMediaAddress = oldAudio->getAddress();
  unsigned short oldDataPort = oldAudio->getDataPort();
  unsigned short oldControlPort = oldAudio->getControlPort();

  std::string finalMediaAddress = oldMediaAddress;
  bool hasSessionAddress = !sessionAddress.empty();
  if (finalMediaAddress.empty())
    finalMediaAddress = oldSessionAddress;
  if (finalMediaAddress.empty())
    return;

  if (hasSessionAddress)
  {
    offer.changeAddress(finalMediaAddress, "IP4");
  }
  else
  {
    audio->setAddressV4(finalMediaAddress);
  }

  audio->setDataPort(oldDataPort);
  audio->setControlPort(oldControlPort);
  audio->setDirection(SDPMedia::MEDIA_SEND_ONLY);

  pRequest->setBody(offer.toString());
  pRequest->updateLength();
}


} } } // OSS::SIP::SBC



