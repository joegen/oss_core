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

#ifndef _SBCDIALOGSTATEMANAGER_H
#define	_SBCDIALOGSTATEMANAGER_H


#include "OSS/SIP/SBC/SBC.h"
#include "OSS/SIP/SBC/SBCException.h"
#include "OSS/SIP/B2BUA/SIPB2BHandler.h"
#include "OSS/SIP/B2BUA/SIPB2BDialogData.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/UTL/Cache.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Thread.h"


namespace OSS {
namespace SIP {
namespace SBC {

using namespace OSS::SIP::B2BUA;
  
class SBCManager;

class OSS_API SBCDialogStateManager
{
  //
  // This class manages the state files responsible
  // for the persistence of dialog states and exposes
  // utility functions that fetches state information
  // like routing mid dialog requests, etc.
  //
public:
  SBCDialogStateManager(SBCManager* pManager);
    /// Creates a new Dialog State Manager
  
  ~SBCDialogStateManager();
    /// Destroys the dialog state manager

  SIPMessage::Ptr onRouteMidDialogTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
    /// Route a mid-dialog transaction.  This is called from SBCDefaultBehavior
    /// when it needs to route a request with an existing tag.

  void onRouteAckRequest(
    const SIPMessage::Ptr& ack,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    OSS::CacheManager& retransmitCache,
    std::string& sessionId,
    std::string& peerXOR,
    OSS::Net::IPAddress& locaAddress,
    OSS::Net::IPAddress& targetAddress);
    /// Route an ACK request

  void onUpdateInitialUASState(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction,
    const std::string& sessionId);
    /// This is called by SBCInviteBehavior::onProcessResponseOutbound() to
    /// update the dialog state of leg-1 prior to sending out a
    /// dialog creating response.  Take note that this method is only called
    /// for the very first transaction that created the dialog.  Reinvites are
    /// handled separately.  See onUpdateReinviteUASState.

  void onUpdateInitialUACState(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction,
    const std::string& sessionId);
    /// This is called by SBCInviteBehavior::onProcessResponseInbound() to
    /// update the dialog state of leg-2 prior to sending out a
    /// dialog creating response.  Take note that this method is only called
    /// for the very first transaction that created the dialog.  Reinvites are
    /// handled separately.  See onUpdateReinviteUACState.

  void onUpdateReinviteUASState(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction,
    const std::string& sessionId);
    /// This is called by SBCInviteBehavior::onProcessReinviteResponseOutbound() to
    /// update the dialog state of the UAS Transaction prior to sending out a
    /// dialog creating response.

  void onUpdateReinviteUACState(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction,
    const std::string& sessionId);
    /// This is called by SBCInviteBehavior::onProcessReinviteResponseInbound() to
    /// update the dialog state of the UAC transaction prior to sending out a
    /// dialog creating response.

  SIPMessage::Ptr onMidDialogTransactionCreated(
    const SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction);
  /// Called by SBCDefaultBehavior::onTransactionCreated signalling the creation of the transaction.
  /// This precedes any other transaction callbacks and therefore is the best place
  /// to initialize anything that would be needed by the transaction processing.

  void fixRFC2543Hold(
    const SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction);
    // This class is a utility object that fixes RFC 2543 style
    // Hold where the c lines is 0.0.0.0 and convert it to RFC3264
    // style hold where the actual IP address remains the same
    // and the direction media attribute set.
  
  void addDialog(const std::string& callId, const boost::filesystem::path& stateFile);
    /// Add a dialog state-file into the dialog cache

  void removeDialog(const std::string& callId, const boost::filesystem::path& stateFile, bool deleteFile);
    /// Remove the dialog from cache
  
  void removeDialog(const SIPB2BTransaction::Ptr& pTransaction, bool deleteFile);
    /// Remove the dialog from cache

  void removeDialog(const SIPMessage::Ptr& pMsg, bool deleteFile);
    /// Remove the dialog from cache.
    /// This method would try to locate the correct state-file automatically.
    /// Take note that the request that is fed to this function must come from the
    /// peer and not requests that the SBC generates (leg2)

  bool findDialog(const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pMsg, boost::filesystem::path& stateFile);
    /// Find the correct state-file for a corresponding SIP Message

  bool findDialog(const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pMsg,
    const std::string& logId,
    boost::filesystem::path& stateFile,
    std::string& senderLeg,
    std::string& targetLeg,
    std::string& sessionId,
    OSS::Persistent::ClassType& persistent);
    /// Find the correct state-file for a corresponding SIP Message
  
  bool findDialog(const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pMsg, OSS::Persistent::ClassType& dialog);

  bool hasDialog(const std::string& callId) const;
    /// Returns true if a certain call-id already have a dialog associated with it.
    /// This is used to determine if an incoming invite without a to-tag is retransmission

  bool findReplacesTarget(
    const SIPMessage::Ptr& pMsg,
    SIPB2BDialogData::LegInfo& legInfo);
    /// Returns the leg-info being replaced
  
  void run();
    /// Start the monitor thread

  void stop();
    /// Stop the monitor thread

  void setStateFileMaxLifetime(int minutes);
    /// Set the maximum state file lifetime before it gets forcibly flushed
    /// by the housekeeper thread.

  int getStateFileMaxLifeTime() const;
    /// Return the maximum state file lifetime.

  static void updateRouteSet(const OSS::Persistent::DataType& dialog, const OSS::SIP::SIPMessage::Ptr& pMsg, OSS::Net::IPAddress& targetAddress, const std::string& logId);
    /// update the configured route-set from a dialog object

private:
  void runTask();
    /// The monitor thread task

  void flushStale();
    /// Flush state-files more than a day old
  
  
  OSS::semaphore _exitSync;
  boost::thread* _pThread;
  mutable OSS::mutex_critic_sec _csDialogsMutex;
  CacheManager _dialogs;
  SBCManager* _pManager;
  int _stateFileMaxLifeTime;
  boost::filesystem::path _stateDir;
};


//
// Inlines
//


inline void SBCDialogStateManager::setStateFileMaxLifetime(int minutes)
{
  _stateFileMaxLifeTime = minutes;
}

inline int SBCDialogStateManager::getStateFileMaxLifeTime() const
{
  return _stateFileMaxLifeTime;
}

} } } // OSS::SIP::SBC

#endif	// _SBCDIALOGSTATEMANAGER_H

