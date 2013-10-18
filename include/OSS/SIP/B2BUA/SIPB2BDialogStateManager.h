// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
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

#include <boost/function.hpp>

#include "OSS/Cache.h"
#include "OSS/Core.h"
#include "OSS/Thread.h"
#include "OSS/Logger.h"

#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/SIPVia.h"

#include "OSS/SIP/B2BUA/SIPB2BContact.h"
#include "OSS/SIP/B2BUA/SIPB2BDialogData.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"



namespace OSS {
namespace SIP {
namespace B2BUA {


struct SIPB2BDialogDataStoreCb
{
  boost::function<bool(const DialogData&)> persist;
  boost::function<void(DialogList&)> getAll;
  boost::function<void(const std::string&)> removeSession;
  boost::function<void(const std::string&)> removeAllDialogs;
  boost::function<bool(const RegData&)> persistReg;
  boost::function<bool(const std::string&, RegData&)> getOneReg;
  boost::function<bool(const std::string&, RegList&)> getReg;
  boost::function<void(const std::string&)> removeReg;
  boost::function<void(const std::string&)> removeAllReg;
  boost::function<void(RegList&)> getAllReg;

  bool dbPersist(const DialogData& dialogData);
  void dbGetAll(DialogList& dialogs);
  void dbRemoveSession(const std::string& sessionId);
  void dbRemoveAllDialogs(const std::string& callId);
  bool dbPersistReg(const RegData& regData);
  bool dbGetOneReg(const std::string& regId, RegData& regData);
  bool dbGetReg(const std::string& regIdPrefix, RegList& regData);
  void dbRemoveReg(const std::string& regId);
  void dbRemoveAllReg(const std::string& regIdPrefix);
  void dbGetAllReg(RegList& regs);

  typedef std::map<std::string, std::string> Storage;
  Storage _dialogs;
  Storage _registry;
  mutex_critic_sec _storageMutex;
};

class  SIPB2BDialogStateManager 
{
public:
  SIPB2BDialogStateManager(
    SIPB2BTransactionManager* pTransactionManager,
    int cacheLifeTime = 3600*24);

  ~SIPB2BDialogStateManager();

  bool findRegistration(const std::string& key, RegList& regList);

  bool findOneRegistration(const std::string& key, RegData& regData);

  void addRegistration(const RegData& regData);

  void removeRegistration(const std::string& key);

  void removeAllRegistration(const std::string& key);

  void getAllRegistrationRecords(RegList& regList);
    void run();

  void stop();

  SIPB2BDialogDataStoreCb& datastore();

protected:
  void runTask();

  void populateFromStore();

  void updateSessionAge();
  
  void encodeRouteSet(  DialogData::LegInfo* pLeg,
                      const std::string& method,
                      const SIPFrom& remoteContact,
                      const SIPMessage::Ptr& pMsg,
                      std::string& transportScheme,
                      OSS::IPAddress& targetAddress  );

public:
  SIPMessage::Ptr onMidDialogTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);

  bool hasDialog(const std::string& callId) const;

  void addDialog(const std::string& callId, const DialogData& dialogData);

  void updateDialog(const std::string& sessionId, const DialogData::LegInfo& leg, int legIndex);
  void updateDialog(const DialogData& dialog);

  void removeDialog(const std::string& callId, const std::string& sessionId);

  bool findDialog(const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pMsg, DialogData& dialogData, const std::string& sessionId = "");

  bool findDialog(
    const SIPB2BTransaction::Ptr& pTransaction,
    const SIPMessage::Ptr& pMsg,
    const std::string& logId,
    std::string& senderLeg,
    std::string& targetLeg,
    std::string& sessionId,
    DialogData& dialogData);

  void onUpdateInitialUASState(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction,
    const std::string& sessionId);
  void onUpdateInitialUACState(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction,
    const std::string& sessionId);

  void onUpdateMidCallUASState(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction,
    const std::string& sessionId);

  void onUpdateMidCallUACState(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction,
    const std::string& sessionId);

  SIPMessage::Ptr onRouteMidDialogTransaction(
    SIPMessage::Ptr& pMsg,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::IPAddress& localAddress,
    OSS::IPAddress& targetAddress);

  void onRouteAckRequest(
    const SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    OSS::CacheManager& retransmitCache,
    std::string& sessionId,
    std::string& peerXOR,
    OSS::IPAddress& routeLocalInterface,
    OSS::IPAddress& targetAddress);

protected:
  SIPB2BTransactionManager* _pTransactionManager;
  SIPB2BDialogDataStoreCb _dataStore;
  mutable OSS::mutex_critic_sec _csDialogsMutex;
  int _cacheLifeTime;
  CacheManager _dialogs;
  OSS::semaphore _exitSync;
  boost::thread* _pThread;
};

} } } // OSS::SIP::B2BUA

#endif	/* SIPB2BDIALOGSTATEMANAGER_H */

