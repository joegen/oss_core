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

#ifndef SIPB2BDIALOGSTATEMANAGER_H
#define	SIPB2BDIALOGSTATEMANAGER_H

#include <list>
#include "OSS/SIP/B2BUA/SIPB2BContact.h"
#include "OSS/SIP/B2BUA/SIPB2BDialogData.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/Cache.h"
#include "OSS/Core.h"
#include "OSS/Thread.h"

namespace OSS {
namespace SIP {

template <typename T>
class SIPB2BDialogStateManager
  /// This is an implementation of a cache manager for dialog states.
  /// External application must be to provide a persistent datastore
  /// that can insert, update and delete DialogData objects.
  ///
{
protected:
  SIPB2BTransactionManager& _transactionManager;
  T& _dataStore;
  mutable OSS::mutex_critic_sec _csDialogsMutex;
  int _cacheLifeTime;
  CacheManager _dialogs;

public:
  typedef SIPB2BDialogData DialogData;
  typedef std::list<DialogData> DialogList;

  SIPB2BDialogStateManager(
    SIPB2BTransactionManager& transactionManager,
    T& dataStore,
    int cacheLifeTime = 3600*24) :
    _transactionManager(transactionManager),
    _dataStore(dataStore),
    _cacheLifeTime(cacheLifeTime),
    _dialogs(cacheLifeTime)
  {
  }

  ~SIPB2BDialogStateManager()
  {
  }


  bool hasDialog(const std::string& callId) const
    // This function will return true if the cache has the dialog
  {
    _csDialogsMutex.lock();
    bool yes = _dialogs.has(callId);
    _csDialogsMutex.unlock();
    return yes;
  }

  void addDialog(const std::string& callId, const DialogData& dialogData)
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
    }
    _dialogs.add(dialogs);
    _csDialogsMutex.unlock();
  }

  void updateDialog(const std::string& sessionId, const DialogData::LegInfo& leg, int legIndex)
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

  void removeDialog(const std::string& callId, const std::string& sessionId)
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
      _csDialogsMutex.unlock();
    }
  }

  void removeDialog(const SIPMessage::Ptr& pMsg)
  {
    OSS::mutex_critic_sec_lock lock(_csDialogsMutex);
    std::string callId = pMsg->hdrGet("call-id");
    OSS_VERIFY(!callId.empty());
    SIPFrom from = pMsg->hdrGet("from");
    SIPTo to = pMsg->hdrGet("to");

    if (_dialogs.has(callId))
    {
      Cacheable::Ptr dialogs = _dialogs.get(callId);
      DialogList& dialogList = boost::any_cast<DialogList&>(dialogs->data());
      if (dialogList.size() <= 1)
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
          dialogList.pop_front();
          if (dialogList.empty())
          {
            _dialogs.remove(callId);
            return;
          }
          return;
        }
        else
        {
          //
          // Check if the to tag corresponds to the dialog leg2 from tag of the last state file
          //
          std::string leg2FromTag = SIPFrom::getTag(last.leg2.from);
          if (leg2FromTag == tag)
          {
            dialogList.pop_back();
            if (dialogList.empty())
            {
              _dialogs.remove(callId);
              return;
            }
            return;
          }
        }
      }
      catch(...)
      {
        return;
      }
    }
  }

  bool findDialog(const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pMsg, DialogData& dialogData, const std::string& sessionId = "")
  {
    OSS::mutex_critic_sec_lock lock(_csDialogsMutex);
    std::string callId = pMsg->hdrGet("call-id");
    OSS_VERIFY(!callId.empty());
    SIPFrom from = pMsg->hdrGet("from");
    SIPTo to = pMsg->hdrGet("to");
    if (_dialogs.has(callId))
    {
      Cacheable::Ptr dialogs = _dialogs.get(callId);
      DialogList& dialogList = boost::any_cast<std::list<DialogList>&>(dialogs->data());
      if (dialogList.size() == 1)
      {
        dialogData = dialogList.front();
        if (!sessionId.empty())
          return dialogData.sessionId == sessionId;
      }
      else if (!sessionId.empty())
      {
        for (DialogList::const_iterator iter = dialogList.begin(); iter != dialogList.end(); iter++)
        {
          if (iter->sessionId == sessionId)
          {
            dialogData = *iter;
            return true;
          }
        }
        return false;
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
      catch(...)
      {
        return false;
      }
    }
    return false;
  }

  bool findDialog(
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
      this,
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
        OSS_LOG_WARNING(logId << "No existing dialog state file for " << method << " - " << e.message());
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

  void onUpdateInitialUASState(
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
        SIPB2BContact::transform(&_transactionManager,
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
          dialogData.sessionId = sessionId;
          DialogData::LegInfo& leg1 = dialogData.leg1;

          leg1.callId = pResponse->hdrGet("call-id");
          leg1.from = pResponse->hdrGet("to");
          leg1.to = pResponse->hdrGet("from");
          leg1.remoteContact = pTransaction->serverRequest()->hdrGet("contact");
          leg1.localContact = pResponse->hdrGet("contact");
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
     

          _dataStore.persist(dialogData);

          if (pResponse->is2xx())
            addDialog(leg1.dialogId, dialogData);
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

          _dataStore.remove(sessionId)
          removeDialog(pResponse->hdrGet("call-id"), sessionId);
        }catch(...){}
      }
    }
  }

  void onUpdateInitialUACState(
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
        if (_dataStore.retrieve(sessionId, dialogData))
          return;
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
        remoteIp = pTransaction->clientTransport()->getRemoteAddress().toIpPortString();
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
          leg2.localSDP = pTransaction->clientRequest()->body();
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

        updateDialog(dialogData.sessionId, leg2, 2);
        _dataStore.persist(dialogData);
      }
      catch(OSS::Exception e)
      {
        std::ostringstream logMsg;
        logMsg << pTransaction->getLogId() << "Unable to save dialog state for leg 2 - " << e.message();
        OSS::log_error(logMsg.str());
      }
    }
  }


  void SBCDialogStateManager::onUpdateMidCallUASState(
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
      SIPB2BContact::transform(this,
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
          if (_dataStore.retrieve(sessionId, dialogData))
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
          _dataStore.persist(dialogData);
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

  void onUpdateMidCallUACState(
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
      if (!_dataStore.retrieve(sessionId, dialogData))
        return;

      DialogData::LegInfo* pLeg;
      if (legIndexNumber == "1")
        pLeg = &dialogData.leg1;
      else if (legIndexNumber == "2")
        pLeg = &dialogData.leg2;
      else
        assert(false);

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

        updateDialog(dialogData.sessionId, *pLeg, boost::lexical_cast<int>(legIndexNumber));
        _dataStore.persist(dialogData);
      }
      catch(OSS::Exception e)
      {
        std::ostringstream logMsg;
        logMsg << pTransaction->getLogId() << "Unable to save dialog state for reinvite " << e.message();
        OSS::log_error(logMsg.str());
      }
    }
  }

  SIPMessage::Ptr onRouteMidDialogTransaction(
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

    //
    // INSERT HERE
    //
    const std::string& logId = pTransaction->getLogId();
    boost::filesystem::path stateFile;
    std::string senderLeg;
    std::string targetLeg;
    std::string sessionId;
    DialogData dialogData;

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
        pLeg = &dialogData.leg1;
      }
      else
      {
        pTransaction->setProperty("leg-index", "2");
        pLeg = &dialogData.leg2;
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
      std::string remoteIp = pLeg->remoteIp;

      std::string localRR = pLeg->localRecordRoute;

      if (pLeg->noRtpProxy)
         pTransaction->setProperty("no-rtp-proxy", "1");
      
      std::string hSeqNum;
      SIPCSeq::getNumber(pMsg->hdrGet("cseq"), hSeqNum);
      int seqNum = 0;
      int requestSeqNum = OSS::string_to_number<int>(hSeqNum.c_str());
      if (requestSeqNum > pLeg->localCSeq)
        seqNum = requestSeqNum;
      pLeg->localCSeq = seqNum;

      if (pLeg->encryption == "xor")
        pMsg->setProperty("xor", "1");

      dataStore.persist(dialogData);

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
      if (!localRR.empty())
        pMsg->hdrSet("Record-Route", localRR.c_str());

      std::ostringstream newCSeq;
      newCSeq  << seqNum << " " << method;
      pMsg->hdrSet("CSeq", newCSeq.str().c_str());

      std::vector<std::string> routeList = pLeg->routeSet;

      localAddress = IPAddress::fromV4IPPort(localContact.getHostPort().c_str());
      //
      // The local contact may point to the external mapped address
      // so make sure we convert it back
      //
      _pManager->getInternalAddress(localAddress, localAddress);

      std::string transportScheme = "UDP";
      std::string transportId;
      transportScheme = pLeg->targetTransport;
      OSS::string_to_upper(transportScheme);
      pMsg->setProperty("target-transport", transportScheme.c_str());


      transportId = pLeg->transportId;
      pMsg->setProperty("transport-id", transportId.c_str());
      OSS_LOG_DEBUG(logId << "Target transport identifier set by statefile: transport-id=" << transportId);


      std::ostringstream via;

      if (!localAddress.externalAddress().empty())
        via << "SIP/2.0/" << transportScheme << " " << localAddress.externalAddress() << ":" << localAddress.getPort();
      else
        via << "SIP/2.0/" << transportScheme << " " << localAddress.toIpPortString();

      via << ";branch=" << branch << ";rport";


      pMsg->hdrSet("Via", via.str().c_str());

      if (routeList.size() > 0)
      {
        SIPFrom topRoute = routeList[0];
        bool strictRoute = topRoute.data().find(";lr") == std::string::npos;
        if (strictRoute)
        {
          std::ostringstream sline;
          sline << method << " " << topRoute.getURI() << " SIP/2.0";
          pMsg->startLine() = sline.str();
          std::ostringstream newRoute;
          newRoute << "<" << remoteContact.getURI() << ">";
          routeList[0] = newRoute.str();
        }

        for (std::vector<std::string>::iterator iter = routeList.begin();
          iter != routeList.end(); iter++)
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
        }
      }

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
    pMsg->setProperty("target-address", targetAddress.toString());
    pMsg->setProperty("target-port", OSS::string_from_number<unsigned short>(targetPort));

    return SIPMessage::Ptr();
  }
};

} } // OSS::SIP

#endif	/* SIPB2BDIALOGSTATEMANAGER_H */

