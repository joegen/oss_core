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


#include "OSS/SIP/UA/SIPSessionState.h"

namespace OSS {
namespace SIP {
namespace UA {

SIPSessionState::SIPSessionState()
{
}

SIPSessionState::~SIPSessionState()
{
}

SIPSessionState::SIPSessionState(const SIPSessionState& state)
{
  _expireTime = state._expireTime;
  _expireInterval = state._expireInterval;
  _eventPackage = state._eventPackage;
  _lastLocalCSeq = state._lastLocalCSeq;
  _lastRemoteCSeq = state._lastRemoteCSeq;
  _localTag = state._localTag;
  _remoteTag = state._remoteTag;
  _remoteContact = state._remoteContact;
  _localContact = state._localContact;
  _localUri = state._localUri;
  _remoteUri = state._remoteUri;
  _remoteTargetAddress = state._remoteTargetAddress;
  _localTargetAddress = state._localTargetAddress;
  _transportScheme = state._transportScheme;
  _transportId = state._transportId;
  _routeSet = state._routeSet;
  _callId = state._callId;
  _dialogId = state._dialogId;
  _properties = state._properties;
}


void SIPSessionState::swap(SIPSessionState& state)
{
  std::swap(_expireTime, state._expireTime);
  std::swap(_expireInterval, state._expireInterval);
  std::swap(_eventPackage, state._eventPackage);
  std::swap(_lastLocalCSeq, state._lastLocalCSeq);
  std::swap(_lastRemoteCSeq, state._lastRemoteCSeq);
  std::swap(_localTag, state._localTag);
  std::swap(_remoteTag, state._remoteTag);
  std::swap(_remoteContact, state._remoteContact);
  std::swap(_localContact, state._localContact);
  std::swap(_localUri, state._localUri);
  std::swap(_remoteUri, state._remoteUri);
  std::swap(_remoteTargetAddress, state._remoteTargetAddress);
  std::swap(_localTargetAddress, state._localTargetAddress);
  std::swap(_transportScheme, state._transportScheme);
  std::swap(_transportId, state._transportId);
  std::swap(_routeSet, state._routeSet);
  std::swap(_callId, state._callId);
  std::swap(_dialogId, state._dialogId);
  std::swap(_properties, state._properties);
}

SIPSessionState& SIPSessionState::operator=(const SIPSessionState& state)
{
  SIPSessionState clonable(state);
  swap(clonable);
  return *this;
}

bool SIPSessionState::update(
  const SIPMessage::Ptr& pMsg,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  return false;
}

bool SIPSessionState::update(const SIPMessage::Ptr& pMsg)
{
  return false;
}

bool SIPSessionState::getProperty(const std::string& name, std::string& value) const
{
  OSS::mutex_critic_sec_lock lock(_propertiesMutex);
  std::map<std::string, std::string>::const_iterator iter = _properties.find(name);
  if (iter == _properties.end())
    return false;
  value = iter->second;
  return true;
}

void SIPSessionState::setProperty(const std::string& name, const std::string& value)
{
  OSS::mutex_critic_sec_lock lock(_propertiesMutex);
  _properties[name] = value;
}

const std::string& SIPSessionState::getDialogId() const
{
  if (_dialogId.empty() && !_localTag.empty() && !_remoteTag.empty())
    _dialogId = _localTag + _remoteTag;
  return _dialogId;
}

} } } // OSS::SIP::UA