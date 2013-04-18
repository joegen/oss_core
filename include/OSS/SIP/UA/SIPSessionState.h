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

#ifndef OSS_SIPSESSIONSTATE_H_INCLUDED
#define	OSS_SIPSESSIONSTATE_H_INCLUDED


#include <map>
#include <string>

#include "OSS/Thread.h"
#include "OSS/IPAddress.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/SIPTransportSession.h"


namespace OSS {
namespace SIP {
namespace UA {


class SIPSessionState
{
public:
  SIPSessionState();

  SIPSessionState(const SIPSessionState& state);

  ~SIPSessionState();

  bool update(
    const SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);

  bool update(const SIPMessage::Ptr& pMsg);

  void swap(SIPSessionState& state);

  SIPSessionState& operator=(const SIPSessionState& state);

  OSS::UInt64 getExpireTime() const;
  unsigned int getExpireInterval() const;
  const std::string& getEventPackage() const;
  unsigned int getLastLocalCSeq() const;
  unsigned int getLastRemoteCSeq() const;
  const std::string& getLocalTag() const;
  const std::string& getRemoteTag() const;
  const OSS::SIP::SIPURI& getRemoteContact() const;
  const OSS::SIP::SIPURI& getLocalContact() const;
  const OSS::SIP::SIPURI& getLocalUri() const;
  const OSS::SIP::SIPURI& getRemoteUri() const;
  const OSS::IPAddress& getRemoteTargetAddress() const;
  const OSS::IPAddress& getLocalTargetAddress() const;
  const std::string& getTransportScheme() const;
  const std::string& getTransportId() const;
  const std::vector<OSS::SIP::SIPURI>& getRouteSet() const;
  const std::string& getCallId() const;
  bool getProperty(const std::string& name, std::string& value) const;
  const std::string& getDialogId() const;

  void setExpireTime(OSS::UInt64 expireTime);
  void setExpireInterval(unsigned int expireInterval);
  void setEventPackage(const std::string& eventPackage);
  void setLastLocalCSeq(unsigned int lastLocalCSeq);
  void setLastRemoteCSeq(unsigned int lastRemoteCSeq);
  void setLocalTag(const std::string& localTag);
  void setRemoteTag(const std::string& remoteTag);
  void setRemoteContact(const OSS::SIP::SIPURI& remoteContact);
  void setLocalContact(const OSS::SIP::SIPURI& localContact);
  void setLocalUri(const OSS::SIP::SIPURI& localUri);
  void setRemoteUri(const OSS::SIP::SIPURI& remoteUri);
  void setRemoteTargetAddress(const OSS::IPAddress& remoteTargetAddress);
  void setLocalTargetAddress(const OSS::IPAddress& localTargetAddress);
  void setTransportScheme(const std::string& transportScheme);
  void setTransportId(const std::string& transportId);
  void setRouteSet(const std::vector<OSS::SIP::SIPURI>& routeSet);
  void setCallId(const std::string& callId);
  void setProperty(const std::string& name, const std::string& value);

protected:
  OSS::UInt64 _expireTime; /// expressed in milliseconds since epoch
  unsigned int _expireInterval; /// expire interval in seconds
  std::string _eventPackage;
  unsigned int _lastLocalCSeq; 
  unsigned int _lastRemoteCSeq; 
  std::string _localTag;
  std::string _remoteTag;
  OSS::SIP::SIPURI _remoteContact;
  OSS::SIP::SIPURI _localContact;
  OSS::SIP::SIPURI _localUri;
  OSS::SIP::SIPURI _remoteUri;
  OSS::IPAddress _remoteTargetAddress;
  OSS::IPAddress _localTargetAddress;
  std::string _transportScheme;
  std::string _transportId;
  std::vector<OSS::SIP::SIPURI> _routeSet;
  std::string _callId;
  mutable std::string _dialogId;
  std::map<std::string, std::string> _properties;
  mutable OSS::mutex_critic_sec _propertiesMutex;
};

//
// Inlines
//

inline OSS::UInt64 SIPSessionState::getExpireTime() const
{
  return _expireTime;
}

inline unsigned int SIPSessionState::getExpireInterval() const
{
  return _expireInterval;
}

inline const std::string& SIPSessionState::getEventPackage() const
{
  return _eventPackage;
}

inline unsigned int SIPSessionState::getLastLocalCSeq() const
{
  return _lastLocalCSeq;
}

inline unsigned int SIPSessionState::getLastRemoteCSeq() const
{
  return _lastRemoteCSeq;
}

inline const std::string& SIPSessionState::getLocalTag() const
{
  return _localTag;
}

inline const std::string& SIPSessionState::getRemoteTag() const
{
  return _remoteTag;
}

inline const OSS::SIP::SIPURI& SIPSessionState::getRemoteContact() const
{
  return _remoteContact;
}

inline const OSS::SIP::SIPURI& SIPSessionState::getLocalContact() const
{
  return _localContact;
}

inline const OSS::SIP::SIPURI& SIPSessionState::getLocalUri() const
{
  return _localUri;
}

inline const OSS::SIP::SIPURI& SIPSessionState::getRemoteUri() const
{
  return _remoteUri;
}

inline const OSS::IPAddress& SIPSessionState::getRemoteTargetAddress() const
{
  return _remoteTargetAddress;
}

inline const OSS::IPAddress& SIPSessionState::getLocalTargetAddress() const
{
  return _localTargetAddress;
}

inline const std::string& SIPSessionState::getTransportScheme() const
{
  return _transportScheme;
}

inline const std::string& SIPSessionState::getTransportId() const
{
  return _transportId;
}

inline const std::vector<OSS::SIP::SIPURI>& SIPSessionState::getRouteSet() const
{
  return _routeSet;
}

inline const std::string& SIPSessionState::getCallId() const
{
  return _callId;
}

inline void SIPSessionState::setExpireTime(OSS::UInt64 expireTime)
{
  _expireTime = expireTime;
}

inline void SIPSessionState::setExpireInterval(unsigned int expireInterval)
{
  _expireInterval = expireInterval;
}

inline void SIPSessionState::setEventPackage(const std::string& eventPackage)
{
  _eventPackage = eventPackage;
}

inline void SIPSessionState::setLastLocalCSeq(unsigned int lastLocalCSeq)
{
  _lastLocalCSeq = lastLocalCSeq;
}

inline void SIPSessionState::setLastRemoteCSeq(unsigned int lastRemoteCSeq)
{
  _lastRemoteCSeq = lastRemoteCSeq;
}

inline void SIPSessionState::setLocalTag(const std::string& localTag)
{
  _localTag = localTag;
}

inline void SIPSessionState::setRemoteTag(const std::string& remoteTag)
{
  _remoteTag = remoteTag;
}

inline void SIPSessionState::setRemoteContact(const OSS::SIP::SIPURI& remoteContact)
{
  _remoteContact = remoteContact;
}

inline void SIPSessionState::setLocalContact(const OSS::SIP::SIPURI& localContact)
{
  _localContact = localContact;
}

inline void SIPSessionState::setLocalUri(const OSS::SIP::SIPURI& localUri)
{
  _localUri = localUri;
}

inline void SIPSessionState::setRemoteUri(const OSS::SIP::SIPURI& remoteUri)
{
  _remoteUri = remoteUri;
}

inline void SIPSessionState::setRemoteTargetAddress(const OSS::IPAddress& remoteTargetAddress)
{
  _remoteTargetAddress = remoteTargetAddress;
}

inline void SIPSessionState::setLocalTargetAddress(const OSS::IPAddress& localTargetAddress)
{
  _localTargetAddress = localTargetAddress;
}

inline void SIPSessionState::setTransportScheme(const std::string& transportScheme)
{
  _transportScheme = transportScheme;
}

inline void SIPSessionState::setTransportId(const std::string& transportId)
{
  _transportId = transportId;
}

inline void SIPSessionState::setRouteSet(const std::vector<OSS::SIP::SIPURI>& routeSet)
{
  _routeSet = routeSet;
}

inline void SIPSessionState::setCallId(const std::string& callId)
{
  _callId = callId;
}

} } } // OSS::SIP::UA



#endif	/// OSS_SIPSESSIONSTATE_H_INCLUDED

