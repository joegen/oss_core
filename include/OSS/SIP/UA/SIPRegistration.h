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


#ifndef OSS_SIPREGISTRATION_H_INCLUDED
#define	OSS_SIPREGISTRATION_H_INCLUDED

#include <boost/shared_ptr.hpp>
#include "OSS/Net/Net.h"
#include "OSS/UTL/Thread.h"
#include "OSS/SIP/UA/SIPUserAgent.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPURI.h"
#include "OSS/SIP/SIPContact.h"


namespace OSS {
namespace SIP {
namespace UA {

  
class SIPRegistration
{
public:
  typedef boost::shared_ptr<SIPRegistration> Ptr;
  typedef boost::function<void(SIPRegistration::Ptr, const SIPMessage::Ptr&, const std::string& )> ResponseHandler;
  typedef std::vector<ResponseHandler> ResponseHandlerList;
  
  SIPRegistration(SIPUserAgent& ua);
  
  ~SIPRegistration();
  
  void schedule(int millis);
  
  bool run();
  
  void stop();
  
  SIPUserAgent& ua();
  
  OSS_HANDLE registration_handle();
   
  void addResponseHandler(ResponseHandler responseHandler);
  
  const ResponseHandlerList& getResponseHandlers() const;
  
  void setCredentials(const std::string& authUser, const std::string& authPassword);
  
  const std::string& getAuthUser() const;
  
  const std::string& getAuthPassword() const;
 
  void setDomain(const std::string& domain);
  
  const std::string& getDomain() const;
  
  void setRealm(const std::string& realm);
  
  const std::string& getRealm() const;
  
  void setToUser(const std::string& toUser);
  
  const std::string& getToUser() const;
  
  void setFromUser(const std::string& fromUser);
  
  const std::string& getFromUser() const;
  
  void setContactUser(const std::string& contactUser);
  
  const std::string& getContactUser() const;
  
  void setContactParams(const std::string& contactParams);
  
  const std::string& getContactParams() const;
  
  void setRouteHeader(const std::string& routeHeader);
  
  const std::string& getRouteHeader() const;
  
  void setExpires(OSS::UInt32 expires);
  
  OSS::UInt32 getExpires() const;
  
  void setExtraHeaders(const std::string& extraHeaders);
  
  const std::string& getExtraHeaders() const;
  
  void setStatus(bool isRegistered);
  
  bool isRegistered() const;
  
  void setRegId(OSS::UInt32 regId);
  
  OSS::UInt32 getRegId() const;
  
  void setContactList(const OSS::SIP::SIPContact& contactList);
  
  void getContactList(OSS::SIP::SIPContact& contactList) const;
  
  bool isRegisteredBinding(const OSS::SIP::SIPURI& binding) const;
  
  SIPRegistration* clone() const;
  
protected:
  void schedule_handler();
  
private:
  SIPUserAgent& _ua;
  OSS_HANDLE _registration_handle;
  std::string _authUser;
  std::string _authPassword;
  std::string _domain;
  std::string _realm;
  std::string _fromUser;
  std::string _toUser;
  std::string _contactUser;
  std::string _routeHeader;
  std::string _contactParams;
  OSS::UInt32 _expires;
  std::string _extraHeaders;
  bool _isRegistered;
  OSS::UInt32 _regId;
  mutable OSS::mutex_critic_sec _contactListMutex;
  OSS::SIP::SIPContact _contactList;
  ResponseHandlerList _responseHandlers;
  NET_TIMER_HANDLE _scheduleTimer;
};

//
// Inlines
//

inline SIPUserAgent& SIPRegistration::ua()
{
  return _ua;
}

inline OSS_HANDLE SIPRegistration::registration_handle()
{
  return _registration_handle;
}

inline void SIPRegistration::addResponseHandler(ResponseHandler responseHandler)
{
  _responseHandlers.push_back(responseHandler);
}
  
inline const SIPRegistration::ResponseHandlerList& SIPRegistration::getResponseHandlers() const
{
  return _responseHandlers;
}

inline void SIPRegistration::setCredentials(const std::string& authUser, const std::string& authPassword)
{
  _authUser = authUser;
  _authPassword = authPassword;
}

inline const std::string& SIPRegistration::getAuthUser() const
{
  return _authUser;
}
  
inline const std::string& SIPRegistration::getAuthPassword() const
{
  return _authPassword;
}

inline void SIPRegistration::setDomain(const std::string& domain)
{
  _domain = domain;
}
  
inline const std::string& SIPRegistration::getDomain() const
{
  return _domain;
}

inline void SIPRegistration::setRealm(const std::string& realm)
{
  _realm = realm;
}

inline const std::string& SIPRegistration::getRealm() const
{
  return _realm;
}

inline void SIPRegistration::setToUser(const std::string& toUser)
{
  _toUser = toUser;
}

inline const std::string& SIPRegistration::getToUser() const
{
  return _toUser;
}

inline void SIPRegistration::setFromUser(const std::string& fromUser)
{
  _fromUser = fromUser;
}

inline const std::string& SIPRegistration::getFromUser() const
{
  return _fromUser;
}

inline void SIPRegistration::setContactUser(const std::string& contactUser)
{
  _contactUser = contactUser;
}

inline const std::string& SIPRegistration::getContactUser() const
{
  return _contactUser;
}

inline void SIPRegistration::setRouteHeader(const std::string& routeHeader)
{
  _routeHeader = routeHeader;
}

inline const std::string& SIPRegistration::getRouteHeader() const
{
  return _routeHeader;
}

inline void SIPRegistration::setContactParams(const std::string& contactParams)
{
  _contactParams = contactParams;
}
  
inline const std::string& SIPRegistration::getContactParams() const
{
  return _contactParams;
}

inline void SIPRegistration::setExpires(OSS::UInt32 expires)
{
  _expires = expires;
}
  
inline OSS::UInt32 SIPRegistration::getExpires() const
{
  return _expires;
}

inline void SIPRegistration::setExtraHeaders(const std::string& extraHeaders)
{
  _extraHeaders = extraHeaders;
}
  
inline const std::string& SIPRegistration::getExtraHeaders() const
{
  return _extraHeaders;
}

inline void SIPRegistration::setStatus(bool isRegistered)
{
  _isRegistered = isRegistered;
}
  
inline bool SIPRegistration::isRegistered() const
{
  return _isRegistered;
}

inline void SIPRegistration::setRegId(OSS::UInt32 regId)
{
  _regId = regId;
}
  
inline OSS::UInt32 SIPRegistration::getRegId() const
{
  return _regId;
}

inline void SIPRegistration::setContactList(const OSS::SIP::SIPContact& contactList)
{
  OSS::mutex_critic_sec_lock lock(_contactListMutex);
  _contactList = contactList;
}
  
inline void  SIPRegistration::getContactList(OSS::SIP::SIPContact& contactList) const
{
  OSS::mutex_critic_sec_lock lock(_contactListMutex);
  contactList = _contactList;
}
  
} } } // OSS::SIP::UA

#endif	// OSS_SIPREGISTRATION_H_INCLUDED

