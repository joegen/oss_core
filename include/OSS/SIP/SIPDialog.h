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

#ifndef OSS_SIPDIALOG_H_INCLUDED
#define OSS_SIPDIALOG_H_INCLUDED

#include "OSS/OSS.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/SIP/SIPMessage.h"

namespace OSS {
namespace SIP {


class SIPDialog : boost::noncopyable
{
public:
  enum Direction
  {
    UAS,
    UAC
  };
  
  enum Type
  {
    INVITE,
    SUBSCRIBE,
    REGISTRATION,
    UNKNOWN_TYPE
  };
  
  enum State
  {
    INIT,
    PROCEEDING,
    EARLY,
    CONNECTED,
    CONFIRMED,
    TERMINATED
  };
  
  typedef boost::shared_ptr<SIPDialog> Ptr;
  typedef boost::weak_ptr<SIPDialog> WeakPtr;
  
  SIPDialog(Direction direction);
  SIPDialog(Direction direction, const SIPMessage::Ptr& initialRequest);
  ~SIPDialog();
  
  Type getType() const;
  State getState() const;
  Direction getDirection() const;
  
  const std::string getCallID() const;
  const std::string getLocalTag() const;
  const std::string getRemoteTag() const;
  const SIPContact getLocalContact() const;
  const SIPContact getRemoteContact() const;
  const SIPRoute getRouteSet() const;
  OSS::UInt32 getLocalCSeq() const;
  OSS::UInt32 getRemoteCSeq() const;
  const SIPMessage::Ptr&  getLastReceivedRequest() const;
  const SIPMessage::Ptr&  getLastSentRequest() const;
  const SIPMessage::Ptr&  getLastReceivedResponse() const;
  const SIPMessage::Ptr&  getLastSentResponse() const;
  
private:
  Direction _direction;
  State _state;
  Type _type;
  
  std::string _callId;
  std::string _localTag;
  std::string _remoteTag;
  SIPContact _localContact;
  SIPContact _remoteContact;
  SIPRoute _routeSet;
  OSS::UInt32 _localCSeq;
  OSS::UInt32 _remoteCSeq;
  SIPMessage::Ptr _lastReceivedRequest;
  SIPMessage::Ptr _lastSentRequest;
  SIPMessage::Ptr _lastReceivedResponse;
  SIPMessage::Ptr _lastSentResponse;
};


//
// Inlines 
//

inline SIPDialog::Type SIPDialog::getType() const
{
  return _type;
}

inline SIPDialog::State SIPDialog::getState() const
{
  return _state;
}

inline SIPDialog::Direction SIPDialog::getDirection() const
{
  return _direction;
}

inline const std::string SIPDialog::getCallID() const
{
  return _callId;
}

inline const std::string SIPDialog::getLocalTag() const
{
  return _localTag;
}

inline const std::string SIPDialog::getRemoteTag() const
{
  return _remoteTag;
}

inline const SIPContact SIPDialog::getLocalContact() const
{
  return _localContact;
}
inline const SIPContact SIPDialog::getRemoteContact() const
{
  return _remoteContact;
}

inline const SIPRoute SIPDialog::getRouteSet() const
{
  return _routeSet;
}

inline OSS::UInt32 SIPDialog::getLocalCSeq() const
{
  return _localCSeq;
}

inline OSS::UInt32 SIPDialog::getRemoteCSeq() const
{
  return _remoteCSeq;
}

inline const SIPMessage::Ptr& SIPDialog::getLastReceivedRequest() const
{
  return _lastReceivedRequest;
}

inline const SIPMessage::Ptr& SIPDialog::getLastSentRequest() const
{
  return _lastSentRequest;
}

inline const SIPMessage::Ptr& SIPDialog::getLastReceivedResponse() const
{
  return _lastReceivedResponse;
}

inline const SIPMessage::Ptr& SIPDialog::getLastSentResponse() const
{
  return _lastSentResponse;
}


} } // OSS::SIP

#endif // OSS_SIPDIALOG_H_INCLUDED

