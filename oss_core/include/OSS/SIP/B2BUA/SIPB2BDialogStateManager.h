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
  // This is an implementation of a cache manager for dialog states.
  // External application must be to provide a persistent datastore
  // that can insert, update and delete DialogData objects.  
  //
{
protected:
  SIPB2BTransactionManager& _transactionManager;
  T& _dataStore;
  OSS::semaphore _exitSync;
  boost::thread* _pThread;
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
    _exitSync(0, 0xFFF),
    _pThread(0),
    _cacheLifeTime(cacheLifeTime),
    _dialogs(cacheLifeTime)
  {
  }

  ~SIPB2BDialogStateManager()
  {
    stop();
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

  bool findDialog(const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pMsg, DialogData& dialogData)
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
};

} } // OSS::SIP

#endif	/* SIPB2BDIALOGSTATEMANAGER_H */

