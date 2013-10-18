// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
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


#include "OSS/SIP/B2BUA/SIPB2BDialogStateManager.h"


namespace OSS {
namespace SIP {
namespace B2BUA {


bool SIPB2BDialogDataStoreCb::dbPersist(const DialogData& dialogData)
{
  if (persist)
  {
    return persist(dialogData);
  }
  else
  {
    mutex_critic_sec_lock lock(_storageMutex);
    std::string data;
    dialogData.toJsonString(data);
    _dialogs[dialogData.sessionId] = data;
    return true;
  }
}

void SIPB2BDialogDataStoreCb::dbGetAll(DialogList& dialogs)
{
  if (getAll)
  {
    getAll(dialogs);
  }
  else
  {
    mutex_critic_sec_lock lock(_storageMutex);
    for (Storage::const_iterator iter = _dialogs.begin(); iter != _dialogs.end(); iter++)
    {
      DialogData dialog;
      dialog.fromJsonString(iter->second);
      dialogs.push_back(dialog);
    }
  }
}

void SIPB2BDialogDataStoreCb::dbRemoveSession(const std::string& sessionId)
{
  if (removeSession)
  {
    removeSession(sessionId);
  }
  else
  {
    mutex_critic_sec_lock lock(_storageMutex);
    _dialogs.erase(sessionId);
  }
}

void SIPB2BDialogDataStoreCb::dbRemoveAllDialogs(const std::string& callId)
{
  if (removeAllDialogs)
  {
    removeAllDialogs(callId);
  }
  else
  {
    std::vector<std::string> deleteThese;
    DialogList dialogs;
    dbGetAll(dialogs);

    //
    // Only lock the mutex after dbGetAll (it's non-recursive!)
    //
    mutex_critic_sec_lock lock(_storageMutex);
    for (DialogList::const_iterator iter = dialogs.begin(); iter != dialogs.end(); iter++)
      if (iter->leg1.callId == callId)
        deleteThese.push_back(iter->sessionId);

    for (std::vector<std::string>::const_iterator iter = deleteThese.begin(); iter != deleteThese.end(); iter++)
      _dialogs.erase(*iter);
  }
}

bool SIPB2BDialogDataStoreCb::dbPersistReg(const RegData& regData)
{
  if (persistReg)
  {
    return persistReg(regData);
  }
  else
  {
    mutex_critic_sec_lock lock(_storageMutex);
    if (regData.key.empty() || regData.aor.empty() || regData.contact.empty())
    {
      OSS_LOG_ERROR("Invalid registration record.");
      return false;
    }
    std::string data;
    regData.toJsonString(data);
    OSS_LOG_INFO("Persisting registration " << regData.key << " for AOR: " << regData.aor << " Binding: " << regData.contact);
    _registry[regData.key] = data;
    return true;
  }
}

bool SIPB2BDialogDataStoreCb::dbGetOneReg(const std::string& regId, RegData& regData)
{
  if (getOneReg)
  {
    return getOneReg(regId, regData);
  }
  else
  {
    mutex_critic_sec_lock lock(_storageMutex);
    OSS_LOG_INFO("OSSB2BUA::getOneReg " << regId);
    std::string data;
    if (_registry.find(regId) == _registry.end())
    {
      OSS_LOG_INFO("Unable to find registration for " << regId );
      return false;
    }
    OSS_LOG_INFO("OSSB2BUA::getOneReg " << regId << " FOUND");
    data = _registry[regId];
    regData.fromJsonString(data);
    OSS_LOG_INFO("Found registration for " << regData.key << " AOR: " << regData.aor << " Binding: " << regData.contact);
    return true;
  }
}

bool SIPB2BDialogDataStoreCb::dbGetReg(const std::string& regIdPrefix, RegList& regData)
{
  if (getReg)
  {
    return getReg(regIdPrefix, regData);
  }
  else
  {
    mutex_critic_sec_lock lock(_storageMutex);
    for (Storage::const_iterator iter = _registry.begin(); iter != _registry.end(); iter++)
    {
      if (OSS::string_starts_with(iter->first, regIdPrefix.c_str()))
      {
        RegData data;
        data.fromJsonString(iter->second);
        regData.push_back(data);
      }
    }
    return true;
  }
}

void SIPB2BDialogDataStoreCb::dbRemoveReg(const std::string& regId)
{
  if (removeReg)
  {
    removeReg(regId);
  }
  else
  {
    mutex_critic_sec_lock lock(_storageMutex);
    _registry.erase(regId);
  }
}

void SIPB2BDialogDataStoreCb::dbRemoveAllReg(const std::string& regIdPrefix)
{
  if (removeAllReg)
  {
    removeAllReg(regIdPrefix);
  }
  else
  {
    std::vector<std::string> deleteThese;
    RegList regs;
    dbGetReg(regIdPrefix, regs);
    //
    // Only lock the mutex after dbGetReg (it's non-recursive!)
    //
    mutex_critic_sec_lock lock(_storageMutex);
    for (RegList::const_iterator iter = regs.begin(); iter != regs.end(); iter++)
        deleteThese.push_back(iter->key);
    for (std::vector<std::string>::const_iterator iter = deleteThese.begin(); iter != deleteThese.end(); iter++)
      _registry.erase(*iter);
  }
}

void SIPB2BDialogDataStoreCb::dbGetAllReg(RegList& regs)
{
  if (getAllReg)
  {
    getAllReg(regs);
  }
  else
  {
    mutex_critic_sec_lock lock(_storageMutex);
    for (Storage::const_iterator iter = _registry.begin(); iter != _registry.end(); iter++)
    {
      RegData data;
      data.fromJsonString(iter->second);
      regs.push_back(data);
    }
  }
}

SIPB2BDialogStateManager::SIPB2BDialogStateManager(
  SIPB2BTransactionManager* pTransactionManager,
  int cacheLifeTime) :
  _pTransactionManager(pTransactionManager),
  _cacheLifeTime(cacheLifeTime),
  _dialogs(cacheLifeTime),
  _exitSync(0, 0xFFF),
  _pThread(0)
{
    populateFromStore();
}

SIPB2BDialogStateManager::~SIPB2BDialogStateManager()
{
  stop();
}

void SIPB2BDialogStateManager::run()
{
  OSS_VERIFY(!_pThread);
  _pThread = new boost::thread(boost::bind(&SIPB2BDialogStateManager::runTask, this));
}

void SIPB2BDialogStateManager::stop()
{
  if (_pThread)
  {
    _exitSync.set();
    _pThread->join();
    delete _pThread;
    _pThread = 0;
  }
}

SIPB2BDialogDataStoreCb& SIPB2BDialogStateManager::datastore()
{
  return _dataStore;
}

void SIPB2BDialogStateManager::runTask()
{
  populateFromStore();

  while (!_exitSync.wait(30000))
  {
    updateSessionAge();
  }
}

void SIPB2BDialogStateManager::populateFromStore()
{
  OSS::UInt64 timeStamp = OSS::getTime();
  DialogList dialogs;
  _dataStore.dbGetAll(dialogs);
  for (DialogList::const_iterator iter = dialogs.begin(); iter != dialogs.end(); iter++)
  {
    if (((timeStamp - iter->sessionAge) / 1000) < 60)
      addDialog(iter->leg1.callId, *iter);
    else
      _dataStore.dbRemoveSession(iter->sessionId);
  }
}

void SIPB2BDialogStateManager::updateSessionAge()
{

}


SIPMessage::Ptr SIPB2BDialogStateManager::onMidDialogTransactionCreated(
  const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction)
{
  return SIPMessage::Ptr();
}

bool SIPB2BDialogStateManager::hasDialog(const std::string& callId) const
  // This function will return true if the cache has the dialog
{
  _csDialogsMutex.lock();
  bool yes = _dialogs.has(callId);
  _csDialogsMutex.unlock();
  return yes;
}

void SIPB2BDialogStateManager::addDialog(const std::string& callId, const DialogData& dialogData)
{
  _csDialogsMutex.lock();
  Cacheable::Ptr dialogs;
  if (!_dialogs.has(callId))
  {
    DialogList dialogList;
    dialogList.push_back(dialogData);
    dialogs = Cacheable::Ptr(new Cacheable(callId, dialogList));
  }
  else
  {
    dialogs = _dialogs.get(callId);
    DialogList& dialogList = boost::any_cast<DialogList&>(dialogs->data());
    dialogList.push_back(dialogData);
  }
  OSS_LOG_DEBUG("Added new dialog " << "Session-ID: " << dialogData.sessionId << " Call-ID: " << callId);
  _dialogs.add(dialogs);
  _csDialogsMutex.unlock();
}

void SIPB2BDialogStateManager::updateDialog(const std::string& sessionId, const DialogData::LegInfo& leg, int legIndex)
{
  OSS::mutex_critic_sec_lock lock(_csDialogsMutex);
  if (_dialogs.has(leg.callId))
  {
    Cacheable::Ptr dialogs = _dialogs.get(leg.callId);
    DialogList& dialogList = boost::any_cast<DialogList&>(dialogs->data());
    for (DialogList::iterator iter = dialogList.begin();
      iter != dialogList.end(); iter++)
    {
      if (iter->sessionId == sessionId)
      {
        if (legIndex == 1)
          iter->leg1 = leg;
        else if(legIndex == 2)
          iter->leg2 = leg;
        else
          assert(false);
        break;
      }
    }
  }
}

void SIPB2BDialogStateManager::updateDialog(const DialogData& dialog)
{
  OSS::mutex_critic_sec_lock lock(_csDialogsMutex);
  if (_dialogs.has(dialog.leg1.callId))
  {
    Cacheable::Ptr dialogs = _dialogs.get(dialog.leg1.callId);
    DialogList& dialogList = boost::any_cast<DialogList&>(dialogs->data());
    for (DialogList::iterator iter = dialogList.begin();
      iter != dialogList.end(); iter++)
    {
      if (iter->sessionId == dialog.sessionId)
      {
        iter->leg1 = dialog.leg1;
        iter->leg2 = dialog.leg2;
        break;
      }
    }
  }
}

void SIPB2BDialogStateManager::removeDialog(const std::string& callId, const std::string& sessionId)
{
  OSS::mutex_critic_sec_lock lock(_csDialogsMutex);
  if (_dialogs.has(callId))
  {
    Cacheable::Ptr dialogs = _dialogs.get(callId);
    DialogList& dialogList = boost::any_cast<DialogList&>(dialogs->data());
    for (DialogList::iterator iter = dialogList.begin();
      iter != dialogList.end(); iter++)
    {
      if (iter->sessionId == sessionId)
      {
        dialogList.erase(iter);
        break;
      }
    }
    if (dialogList.empty())
      _dialogs.remove(callId);
  }
  _dataStore.dbRemoveSession(sessionId);
}

bool SIPB2BDialogStateManager::findDialog(const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pMsg, DialogData& dialogData, const std::string& sessionId)
{
  OSS::mutex_critic_sec_lock lock(_csDialogsMutex);
  std::string callId = pMsg->hdrGet("call-id");
  if (callId.empty())
  {
    OSS_LOG_ERROR("Unable to determine Call-ID while calling findDialog.");
    return false;
  }

  SIPFrom from = pMsg->hdrGet("from");
  SIPTo to = pMsg->hdrGet("to");
  std::string fromTag = from.getTag();
  std::string toTag = to.getTag();

  OSS_LOG_DEBUG("Finding dialog for Call-ID: " << callId << " SessionId: (" << sessionId << ")");

  if (_dialogs.has(callId))
  {
    OSS_LOG_DEBUG("Dialog database has a record for Call-ID: " << callId);
    Cacheable::Ptr dialogs = _dialogs.get(callId);
    DialogList& dialogList = boost::any_cast<DialogList&>(dialogs->data());
    if (dialogList.size() == 1)
    {
      dialogData = dialogList.front();
      OSS_LOG_DEBUG("Dialog database has 1 session for Call-ID: " << callId << " SessionId: (" << dialogData.sessionId << ")");
      if (!sessionId.empty())
        return dialogData.sessionId == sessionId;
      else
        return true;
    }
    else if (!sessionId.empty())
    {
      OSS_LOG_DEBUG("Dialog database has multiple sessions for Call-ID: " << callId << " SessionId: (" << sessionId << ")");
      for (DialogList::const_iterator iter = dialogList.begin(); iter != dialogList.end(); iter++)
      {
        if (iter->sessionId == sessionId)
        {
          std::string leg1ToTag = SIPFrom::getTag(iter->leg1.to);
          std::string leg2ToTag = SIPFrom::getTag(iter->leg2.to);
          if (toTag == leg1ToTag || toTag == leg2ToTag)
          {
            dialogData = *iter;
            return true;
          }
        }
      }
      OSS_LOG_WARNING("Multiple sessions exist for Call-ID: " << callId << " but non of them is Session-ID: " << sessionId);
      return false;
    }
    //
    // Do it the hard way
    //
    std::string tag;
    if (pMsg->isRequest())
      tag = toTag;
    else
      tag = fromTag;

    DialogData& first = dialogList.front();
    DialogData& last = dialogList.back();
    try
    {
      //
      // Check if the to tag corresponds to the dialog leg1 from tag of the first state file
      //
      std::string leg1FromTag = SIPFrom::getTag(first.leg1.from);
      if (leg1FromTag == tag)
      {
        dialogData = first;
        return true;
      }
      else
      {
        std::string leg2FromTag = SIPFrom::getTag(last.leg2.from);
        if (leg2FromTag == tag)
        {
          dialogData = last;
          return true;
        }
      }
    }
    catch(std::exception& e)
    {
      OSS_LOG_ERROR("Exception caught while calling findDialog - Error: " << e.what());
      return false;
    }
    catch(...)
    {
      OSS_LOG_ERROR("Exception caught while calling findDialog - Error: Unknown exception.");
      return false;
    }
  }

  OSS_LOG_WARNING("No dialog exists for Call-ID " << callId);

  return false;
}

bool SIPB2BDialogStateManager::findDialog(
  const SIPB2BTransaction::Ptr& pTransaction,
  const SIPMessage::Ptr& pMsg,
  const std::string& logId,
  std::string& senderLeg,
  std::string& targetLeg,
  std::string& sessionId,
  DialogData& dialogData)
{
  SIPRequestLine rline = pMsg->startLine();
  SIPURI requestUri;
  if (!rline.getURI(requestUri))
    return false;

  std::string method = pMsg->getMethod();
  std::string user = requestUri.getUser();
  //std::vector<std::string> userTokens = OSS::string_tokenize(user, "-");

  SIPB2BContact::SessionInfo sessionInfo;
  bool hasSessionInfo = SIPB2BContact::getSessionInfo(
    _pTransactionManager,
    pMsg,
    pTransaction,
    sessionInfo);


  if (!hasSessionInfo)
  {
    OSS_LOG_DEBUG(logId << "Unable to determine session information from request-uri - " << user);

    if (!findDialog(pTransaction, pMsg, dialogData))
    {
      OSS_LOG_WARNING(logId << "No existing dialog for " << method);
      return false;
    }

    std::string from = pMsg->hdrGet("from");
    std::string fromTag = SIPFrom::getTag(from);
    try
    {
      std::string leg1ToTag = SIPFrom::getTag(dialogData.leg1.to);
      if (leg1ToTag == fromTag)
      {
        senderLeg = "leg-1";
        targetLeg = "leg-2";
        sessionId = dialogData.sessionId;
        return true;
      }
      else
      {
        std::string leg2ToTag = SIPFrom::getTag(dialogData.leg2.to);
        if (leg2ToTag == fromTag)
        {
          senderLeg = "leg-2";
          targetLeg = "leg-1";
          sessionId = dialogData.sessionId;
          return true;
        }
        else
        {
          OSS_LOG_WARNING(logId << "Unable to determine dialog for " << method);
          return false;
        }
      }
    }catch(OSS::Exception e)
    {
      OSS_LOG_WARNING(logId << "No existing dialog state for " << method << " - " << e.message());
      return false;
    }
  }

  //
  // Session-id is compliant so there is less guess work we will do
  //
  sessionId = sessionInfo.sessionId;
  if (sessionInfo.callIndex == 1)
  {
    targetLeg = "leg-2";
    senderLeg = "leg-1";
  }
  else if (sessionInfo.callIndex == 2)
  {
    targetLeg = "leg-1";
    senderLeg = "leg-2";
  }
  //
  // Now find if the specific session exists in the dialog list
  //
  if (!findDialog(pTransaction, pMsg, dialogData, sessionId))
  {
    OSS_LOG_WARNING(logId << "No existing dialog for " << method);
    return false;
  }
  return true;
}

void SIPB2BDialogStateManager::onUpdateInitialUASState(
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
      SIPB2BContact::SessionInfo sessionInfo;
      sessionInfo.sessionId = sessionId;
      sessionInfo.callIndex = 1;
      SIPB2BContact::transform(_pTransactionManager,
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
        DialogData dialogData;
        if (!findDialog(pTransaction, pResponse, dialogData, sessionId))
          return;

        dialogData.sessionId = sessionId;
        DialogData::LegInfo& leg1 = dialogData.leg1;

        leg1.callId = pResponse->hdrGet("call-id");
        leg1.from = pResponse->hdrGet("to");
        leg1.to = pResponse->hdrGet("from");
        leg1.remoteContact = pTransaction->serverRequest()->hdrGet("contact");
        leg1.localContact = pResponse->hdrGet("contact");
        std::string localRecordRoute;
        if (pResponse->hdrPresent("record-route"))
          localRecordRoute = pResponse->hdrGet("record-route");
        leg1.remoteIp = pTransaction->serverTransport()->getRemoteAddress().toIpPortString();

        //
        // Preserve the transport identifier
        // so we can recycle it for requests in mid dialog
        //
        leg1.transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier());

        leg1.targetTransport = pTransaction->serverTransport()->getTransportScheme().c_str();

        std::string contentType = pResponse->hdrGet("content-type");
        OSS::string_to_lower(contentType);
        if (!pResponse->body().empty() && contentType == "application/sdp")
        {
          leg1.localSdp = pResponse->body();
        }

        contentType = pTransaction->serverRequest()->hdrGet("content-type");
        OSS::string_to_lower(contentType);
        if (!pTransaction->serverRequest()->body().empty() && contentType == "application/sdp")
        {
          leg1.remoteSdp = pTransaction->serverRequest()->body();
        }

        if (pResponse->hdrGetSize("record-route") > 0)
        {
          //
          // Preserve the record routes
          //
          std::list<std::string> recordRoutes;
          SIPRoute::msgGetRecordRoutes(pResponse.get(), recordRoutes);

          if (!recordRoutes.empty())
            leg1.routeSet.clear();

          for (std::list<std::string>::reverse_iterator iter = recordRoutes.rbegin();
            iter != recordRoutes.rend(); iter++)
          {
            leg1.routeSet.push_back(*iter);
          }
        }

        //
        // Check if the server request is XOR encrypted.  This is a proprietary
        // encryption used by Karoo Bridge
        //
        if (pTransaction->serverTransaction()->isXOREncrypted())
        {
          leg1.encryption = "xor";
        }

        std::string noRTPProxy;
        leg1.noRtpProxy = (pTransaction->getProperty("no-rtp-proxy", noRTPProxy) && noRTPProxy == "1");

        _dataStore.dbPersist(dialogData);

        //if (pResponse->is2xx())
        updateDialog(sessionId, leg1, 1);
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
        removeDialog(pResponse->hdrGet("call-id"), sessionId);
      }catch(...){}
    }
  }
}

void SIPB2BDialogStateManager::onUpdateInitialUACState(
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
      //
      // Preserve leg 2 dialog state
      //
      DialogData dialogData;
      dialogData.sessionId = sessionId;

      DialogData::LegInfo& leg2 = dialogData.leg2;
      leg2.callId = pResponse->hdrGet("call-id").c_str();
      leg2.from = pResponse->hdrGet("from").c_str();
      leg2.to = pResponse->hdrGet("to").c_str();
      leg2.remoteContact = pResponse->hdrGet("contact").c_str();
      std::string seqNum;
      SIPCSeq::getNumber(pResponse->hdrGet("cseq"), seqNum);
      leg2.localCSeq = OSS::string_to_number<unsigned long>(seqNum.c_str());
      OSS_VERIFY(pTransaction->getProperty("leg2-contact", leg2.localContact));
      pTransaction->getProperty("leg2-rr", leg2.localRecordRoute);
      leg2.remoteIp = pTransaction->clientTransport()->getRemoteAddress().toIpPortString();
      leg2.transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->clientTransport()->getIdentifier());
      leg2.targetTransport = pTransaction->clientTransport()->getTransportScheme();

      std::string contentType = pResponse->hdrGet("content-type");
      OSS::string_to_lower(contentType);
      if (!pResponse->body().empty() && contentType == "application/sdp")
      {
        leg2.remoteSdp = pResponse->body();
      }

      contentType = pTransaction->clientRequest()->hdrGet("content-type");
      OSS::string_to_lower(contentType);
      if (!pTransaction->clientRequest()->body().empty() && contentType == "application/sdp")
      {
        leg2.localSdp = pTransaction->clientRequest()->body();
      }

      std::string noRTPProxy;
      leg2.noRtpProxy = (pTransaction->getProperty("no-rtp-proxy", noRTPProxy) && noRTPProxy == "1");

      if (pResponse->hdrGetSize("record-route") > 0)
      {
        //
        // Preserve the record routes
        //
        std::list<std::string> recordRoutes;
        SIPRoute::msgGetRecordRoutes(pResponse.get(), recordRoutes);

        if (!recordRoutes.empty())
            leg2.routeSet.clear();

        for (std::list<std::string>::iterator iter = recordRoutes.begin();
          iter != recordRoutes.end(); iter++)
        {
          leg2.routeSet.push_back(*iter);
        }
      }

      //
      // Check if the client request is XOR encrypted
      //
      if (pTransaction->clientTransaction()->isXOREncrypted())
      {
        leg2.encryption = "xor";
      }


      addDialog(leg2.callId, dialogData);
      _dataStore.dbPersist(dialogData);
    }
    catch(OSS::Exception e)
    {
      std::ostringstream logMsg;
      logMsg << pTransaction->getLogId() << "Unable to save dialog state for leg 2 - " << e.message();
      OSS::log_error(logMsg.str());
    }
  }
}


void SIPB2BDialogStateManager::onUpdateMidCallUASState(
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
    SIPB2BContact::SessionInfo sessionInfo;
    sessionInfo.sessionId = sessionId;
    sessionInfo.callIndex = OSS::string_to_number<unsigned>(legIndexNumber.c_str());
    SIPB2BContact::transform(_pTransactionManager,
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
        DialogData dialogData;
        if (!findDialog(pTransaction, pResponse, dialogData, sessionId))
          return;

        DialogData::LegInfo* pLeg;
        if (legIndexNumber == "1")
          pLeg = &dialogData.leg1;
        else if (legIndexNumber == "2")
          pLeg = &dialogData.leg2;
        else
          assert(false);

        //
        // Preserve leg dialog state
        //
        pLeg->remoteContact = pTransaction->serverRequest()->hdrGet("contact");
        pLeg->remoteIp = pTransaction->serverTransport()->getRemoteAddress().toIpPortString();
        pLeg->transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier());


        std::string contentType = pResponse->hdrGet("content-type");
        OSS::string_to_lower(contentType);
        if (!pResponse->body().empty() && contentType == "application/sdp")
        {
          pLeg->localSdp = pResponse->body();
        }

        contentType = pTransaction->serverRequest()->hdrGet("content-type");
        OSS::string_to_lower(contentType);
        if (!pTransaction->serverRequest()->body().empty() && contentType == "application/sdp")
        {
          pLeg->remoteSdp = pTransaction->serverRequest()->body();
        }

        updateDialog(dialogData.sessionId, *pLeg, boost::lexical_cast<int>(legIndexNumber));
        _dataStore.dbPersist(dialogData);
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

void SIPB2BDialogStateManager::onUpdateMidCallUACState(
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
    DialogData dialogData;
    if (!findDialog(pTransaction, pResponse, dialogData, sessionId))
      return;

    std::string seqNum;
    SIPCSeq::getNumber(pResponse->hdrGet("cseq"), seqNum);

    DialogData::LegInfo* pLeg;
    if (legIndexNumber == "1")
    {
      pLeg = &dialogData.leg1;
      dialogData.leg2.localCSeq = OSS::string_to_number<unsigned long>(seqNum.c_str());
    }
    else if (legIndexNumber == "2")
    {
      pLeg = &dialogData.leg2;
      dialogData.leg1.localCSeq = OSS::string_to_number<unsigned long>(seqNum.c_str());
    }
    else
    {
      assert(false);
    }

    try
    {
      //
      // Preserve leg dialog state
      //
      pLeg->remoteContact = pTransaction->serverRequest()->hdrGet("contact");
      pLeg->remoteIp = pTransaction->serverTransport()->getRemoteAddress().toIpPortString();
      pLeg->transportId = OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier());


      std::string contentType = pResponse->hdrGet("content-type");
      OSS::string_to_lower(contentType);
      if (!pResponse->body().empty() && contentType == "application/sdp")
      {
        pLeg->remoteSdp = pResponse->body();
      }

      contentType = pTransaction->clientRequest()->hdrGet("content-type");
      OSS::string_to_lower(contentType);
      if (!pTransaction->clientRequest()->body().empty() && contentType == "application/sdp")
      {
        pLeg->localSdp = pTransaction->clientRequest()->body();
      }

      updateDialog(dialogData);
      _dataStore.dbPersist(dialogData);
    }
    catch(OSS::Exception e)
    {
      std::ostringstream logMsg;
      logMsg << pTransaction->getLogId() << "Unable to save dialog state for reinvite " << e.message();
      OSS::log_error(logMsg.str());
    }
  }
}

void SIPB2BDialogStateManager::encodeRouteSet(  DialogData::LegInfo* pLeg,
                      const std::string& method,
                      const SIPFrom& remoteContact,
                      const SIPMessage::Ptr& pMsg,
                      std::string& transportScheme,
                      OSS::IPAddress& targetAddress  )
{
  if (pLeg->routeSet.size() > 0)
  {
    int topRouteIndex = pLeg->routeSet.size() - 1;
    
    SIPFrom topRoute = pLeg->routeSet[topRouteIndex];
    bool strictRoute = topRoute.data().find(";lr") == std::string::npos;
    if (strictRoute)
    {
      std::ostringstream sline;
      sline << method << " " << topRoute.getURI() << " SIP/2.0";
      pMsg->startLine() = sline.str();
      std::ostringstream newRoute;
      newRoute << "<" << remoteContact.getURI() << ">";
      pLeg->routeSet[topRouteIndex] = newRoute.str();
    }

    for (std::vector<std::string>::iterator iter = pLeg->routeSet.begin();
      iter != pLeg->routeSet.end(); iter++)
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
      if (port == 0)
        port = 5060;
      targetAddress = *(hosts.begin());
      targetAddress.setPort(port);
      std::string scheme;
      if (topRouteURI.getParam("transport", scheme))
      {
        transportScheme = scheme;
        boost::to_upper(transportScheme);
      }
    }
  }
}

SIPMessage::Ptr SIPB2BDialogStateManager::onRouteMidDialogTransaction(
  SIPMessage::Ptr& pMsg,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::IPAddress& localAddress,
  OSS::IPAddress& targetAddress)
{
  SIPRequestLine rline = pMsg->startLine();
  SIPURI requestUri;
  std::string oldVia;
  SIPVia::msgGetTopVia(pMsg.get(), oldVia);
  std::string branch;
  SIPVia::getBranch(oldVia, branch);
  if (branch.empty())
  {
    SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_400_BadRequest, "Missing Via Branch Parameter");
    return serverError;
  }
  if (!rline.getURI(requestUri))
  {
    SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_400_BadRequest, "Bad Request URI");
    return serverError;
  }

  const std::string& logId = pTransaction->getLogId();
  std::string senderLeg;
  std::string targetLeg;
  std::string sessionId;
  DialogData dialogData;
  std::string transportScheme = "UDP";
  
  try
  {
    if (!findDialog(pTransaction, pMsg, logId,  senderLeg, targetLeg, sessionId, dialogData))
    {
      SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_481_TransactionDoesNotExist, "Unable to match dialog");
      return serverError;
    }

    //
    // Preserve the contact user to be used by the outbound response later on
    //
    DialogData::LegInfo* pLeg;
    pTransaction->setProperty("leg-identifier", requestUri.getUser());
    if (senderLeg == "leg-1")
    {
      pTransaction->setProperty("leg-index", "1");
      pLeg = &dialogData.leg2;
    }
    else
    {
      pTransaction->setProperty("leg-index", "2");
      pLeg = &dialogData.leg1;
    }
    pTransaction->setProperty("session-id", sessionId);


    std::string callId = pLeg->callId;
    SIPFrom from;
    from = pLeg->from;
    SIPFrom to;
    to = pLeg->to;
    SIPFrom remoteContact;
    remoteContact = pLeg->remoteContact;
    SIPFrom localContact;
    localContact = pLeg->localContact;


    if (pLeg->noRtpProxy)
       pTransaction->setProperty("no-rtp-proxy", "1");

    std::string hSeqNum;
    SIPCSeq::getNumber(pMsg->hdrGet("cseq"), hSeqNum);
    unsigned long seqNum = 0;
    unsigned long requestSeqNum = OSS::string_to_number<unsigned long>(hSeqNum.c_str());
    if (requestSeqNum > pLeg->localCSeq)
      seqNum = requestSeqNum;
    pLeg->localCSeq = seqNum;

    if (pLeg->encryption == "xor")
      pMsg->setProperty("xor", "1");

    _dataStore.dbPersist(dialogData);

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
    pMsg->hdrSet("Call-ID", pLeg->callId);
    if (!pLeg->localRecordRoute.empty())
      pMsg->hdrSet("Record-Route", pLeg->localRecordRoute.c_str());

    std::ostringstream newCSeq;
    newCSeq  << seqNum << " " << method;
    pMsg->hdrSet("CSeq", newCSeq.str().c_str());

    localAddress = IPAddress::fromV4IPPort(localContact.getHostPort().c_str());
    //
    // The local contact may point to the external mapped address
    // so make sure we convert it back
    //
    _pTransactionManager->getInternalAddress(localAddress, localAddress);

    

    transportScheme = pLeg->targetTransport;
    OSS::string_to_upper(transportScheme);
    pMsg->setProperty("target-transport", transportScheme.c_str());


    pMsg->setProperty("transport-id", pLeg->transportId);
    OSS_LOG_DEBUG(logId << "Target transport identifier set by dialog data: transport-id=" << pLeg->transportId);

    std::string via = SIPB2BContact::constructVia(_pTransactionManager,
      pMsg,
      localAddress,
      transportScheme,
      branch);

    pMsg->hdrSet("Via", via.c_str());
    
    encodeRouteSet(pLeg, method, remoteContact, pMsg, transportScheme, targetAddress);
    
    //
    // User request URI if no route is set
    //
    if (!targetAddress.isValid() && transportScheme != "WS")
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
        std::string scheme;
        if (requestTarget.getParam("transport", scheme))
        {
          transportScheme = scheme;
          boost::to_upper(transportScheme);
        }
      }

    }

    if (!targetAddress.isValid() || (targetAddress.isPrivate() && transportScheme == "UDP"))
    {
      //
      // Still unable to resolve request-uri
      //
      if (!pLeg->remoteIp.empty())
        targetAddress = IPAddress::fromV4IPPort(pLeg->remoteIp.c_str());
    }

    if (!targetAddress.isValid() && transportScheme != "WS")
    {
      SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_481_TransactionDoesNotExist, "Unable to determine dialog target");
      return serverError;
    }
  }
  catch(...)
  {
    SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_481_TransactionDoesNotExist, "Unable to match dialog");
    return serverError;
  }

  if (!targetAddress.isValid() && transportScheme != "WS")
  {
    SIPMessage::Ptr serverError = pMsg->createResponse(SIPMessage::CODE_500_InternalServerError, "Can't Resolve Requested Target");
    return serverError;
  }

  unsigned short targetPort = targetAddress.getPort();
  if (targetPort == 0)
      targetPort = 5060;
  pMsg->setProperty("target-address", targetAddress.toString());
  pMsg->setProperty("target-port", OSS::string_from_number<unsigned short>(targetPort));

  return SIPMessage::Ptr();
}

void SIPB2BDialogStateManager::onRouteAckRequest(
  const SIPMessage::Ptr& pMsg,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  OSS::CacheManager& retransmitCache,
  std::string& sessionId,
  std::string& peerXOR,
  OSS::IPAddress& routeLocalInterface,
  OSS::IPAddress& targetAddress)
{
  std::string logId = pMsg->createContextId(true);
  if (!pMsg->isRequest("ACK"))
  {
    OSS_LOG_ERROR(logId << "Non-ACK Request fed to onRouteAckRequest" << pMsg->startLine());
    throw B2BUAStateException("Non-ACK Request fed to onRouteAckRequest");
    return;
  }

  OSS_LOG_DEBUG(logId << "Attempting to route session " << sessionId  << pMsg->startLine());

  std::string maxForwards = pMsg->hdrGet("max-forwards");
  if (maxForwards.empty())
  {
    maxForwards = "70";
  }
  int maxF = OSS::string_to_number<int>(maxForwards.c_str());
  if (--maxF == 0)
  {
    throw B2BUAStateException("Max-forward reached.");
    return;
  }
  pMsg->hdrRemove("max-forwards");
  pMsg->hdrSet("Max-Forwards", OSS::string_from_number(maxF).c_str());

  pMsg->clearProperties();
  SIPRequestLine rline = pMsg->startLine();
  SIPURI requestUri;
  rline.getURI(requestUri);

  std::string senderLeg;
  std::string targetLeg;

  SIPB2BTransaction::Ptr pTransaction; /// DUMMY
  DialogData dialogData;
  if (!findDialog(pTransaction, pMsg, logId, senderLeg, targetLeg, sessionId, dialogData))
  {
    OSS_LOG_DEBUG(logId << "No dialog data found for ACK request." );
    throw B2BUAStateException("No dialog exist.");
    return;
  }

  try
  {
    DialogData::LegInfo* pLeg;
    if (targetLeg == "leg-1")
      pLeg = &dialogData.leg1;
    else if (targetLeg == "leg-2")
      pLeg = &dialogData.leg2;
    else
      assert(false);

    std::string callId = pLeg->callId;
    SIPFrom from;
    from = pLeg->from;
    SIPFrom to;
    to = pLeg->to;
    SIPFrom remoteContact;
    remoteContact = pLeg->remoteContact;
    SIPFrom localContact;
    localContact = pLeg->localContact;
    std::string localRR = pLeg->localRecordRoute;
    std::string remoteIp = pLeg->remoteIp;

    OSS_LOG_DEBUG(logId << "Found dialog data for ACK request."
            << " Target-leg: " << targetLeg
            << " Session-Id" << dialogData.sessionId
            << " Contact: " << pLeg->remoteContact);

    bool isXOREncrypted = false;
    //
    // Check if propritery xor is set
    //
    pMsg->getProperty("xor", peerXOR);
    pMsg->setProperty("peer-xor", peerXOR);
    if (pLeg->encryption == "xor")
    {
      pMsg->setProperty("xor", "1");
      isXOREncrypted = true;
    }

    std::string transportScheme = pLeg->targetTransport;
    OSS::string_to_upper(transportScheme);
    pMsg->setProperty("target-transport", transportScheme.c_str());
    std::string transportId = pLeg->transportId;

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

    if (!localRR.empty())
    {
      pMsg->hdrSet("Record-Route", localRR.c_str());
    }

    SIPCSeq cseq = pMsg->hdrGet("cseq");
    cseq.setMethod("INVITE");

    encodeRouteSet(pLeg, "ACK", remoteContact, pMsg, transportScheme, targetAddress);

    //
    // Use request URI if no route is set
    //
    std::string host;
    unsigned short port = 0;
    if (!targetAddress.isValid() && transportScheme != "WS")
    {
      SIPURI requestTarget = remoteContact.getURI();
      requestTarget.getHostPort(host, port);
      OSS::dns_host_record_list hosts = OSS::dns_lookup_host(host);
      if (!hosts.empty())
      {

        if (port == 0)
          port = 5060;
        OSS_LOG_INFO("Setting target from contact Address: " << host << " Port: " << port);
        targetAddress = *(hosts.begin());
        targetAddress.setPort(port);
        std::string scheme;
        if (requestTarget.getParam("transport", scheme))
          transportScheme = scheme;
        boost::to_upper(transportScheme);
      }
    }

    if (!remoteIp.empty() &&(!targetAddress.isValid() || (targetAddress.isPrivate() && transportScheme == "UDP")))
    {
      //
      // Still unable to resolve request-uri
      //
      OSS_LOG_INFO("Setting target from source Address: " << remoteIp);
      targetAddress = IPAddress::fromV4IPPort(remoteIp.c_str());
    }

    if (!targetAddress.isValid() && transportScheme != "WS")
      throw B2BUAStateException("Unable to determine target for ACK request");

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
          throw B2BUAStateException("Unable to process ACK. Old via appears to have no branch parameter.");
        }

        OSS::IPAddress viaHost = OSS::IPAddress::fromV4IPPort(localContact.getHostPort().c_str());
        std::string newVia = SIPB2BContact::constructVia(_pTransactionManager, pMsg, viaHost, transportScheme, branch);
        pMsg->hdrListPrepend("Via", newVia.c_str());
      }
    }
    else
    {
      OSS_LOG_ERROR(logId << " Unable to find 2xx dialog-id: " << cacheId.str() << " in retransmission cache.");
      throw B2BUAStateException("Unable to process ACK. There is no 2xx retransmisison in cache.");
    }

    if (!_pTransactionManager->getUserAgentName().empty())
      pMsg->hdrSet("User-Agent", _pTransactionManager->getUserAgentName().c_str());

    std::string contentType = pMsg->hdrGet("content-type");
    OSS::string_to_lower(contentType);


    //
    // Set the local address to the local contact
    // This would have previously been set to the external address if one was used
    // So make sure we transform it back to it's original state
    //
    routeLocalInterface = IPAddress::fromV4IPPort(localContact.getHostPort().c_str());
    OSS_LOG_DEBUG(logId << "ACK Local interface preliminary address is " << routeLocalInterface.toString());
    _pTransactionManager->getInternalAddress(routeLocalInterface, routeLocalInterface);
    OSS_LOG_DEBUG(logId << "ACK Local interface post conversion address is " << routeLocalInterface.toString()
      << " external=(" <<  routeLocalInterface.externalAddress() << ")");

    if (contentType == "application/sdp")
    {
      std::string sdp = pMsg->getBody();


      DialogData::LegInfo* pSenderLeg;
      if (senderLeg == "leg-1")
        pSenderLeg = &dialogData.leg1;
      else
        pSenderLeg = &dialogData.leg2;

      pSenderLeg->remoteSdp = pMsg->body();
      //
      // Now set the B-Leg local interface using the transport info
      //
      OSS::IPAddress packetLocalInterface = pTransport->getLocalAddress();
      packetLocalInterface.externalAddress() = pTransport->getExternalAddress();

      pMsg->setBody(sdp);
      std::string clen = OSS::string_from_number<size_t>(sdp.size());
      pMsg->hdrSet("Content-Length", clen.c_str());
      pLeg->localSdp = sdp;

      _dataStore.dbPersist(dialogData);
      updateDialog(dialogData);
    }

    pMsg->commitData();

    std::string finalTarget;
    if (targetAddress.isValid())
      finalTarget = targetAddress.toIpPortString();
    else
      finalTarget = host;

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

  }
  catch(OSS::Exception e)
  {
    std::ostringstream logMsg;
    logMsg << logId << "Exception: " << e.message();
    OSS::log_warning(logMsg.str());
    throw;
  }
}


bool SIPB2BDialogStateManager::findRegistration(const std::string& key, RegList& regList)
{
  return _dataStore.dbGetReg(key, regList);
}

bool SIPB2BDialogStateManager::findOneRegistration(const std::string& key, RegData& regData)
{
  return _dataStore.dbGetOneReg(key, regData);
}

void SIPB2BDialogStateManager::addRegistration(const RegData& regData)
{
  _dataStore.dbPersistReg(regData);
}

void SIPB2BDialogStateManager::removeRegistration(const std::string& key)
{
  _dataStore.dbRemoveReg(key);
}

void SIPB2BDialogStateManager::removeAllRegistration(const std::string& key)
{
  _dataStore.dbRemoveAllReg(key);
}

void SIPB2BDialogStateManager::getAllRegistrationRecords(RegList& regList)
{
  _dataStore.dbGetAllReg(regList);
}


} } } // OSS::SIP::B2BUA