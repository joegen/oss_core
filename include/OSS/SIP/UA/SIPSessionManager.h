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

#include <boost/noncopyable.hpp>

#include "OSS/SIP/UA/SIPSession.h"

#ifndef OSS_SIPSESSIONMANAGER_H_INCLUDED
#define	OSS_SIPSESSIONMANAGER_H_INCLUDED


namespace OSS {
namespace SIP {
namespace UA {


class SIPSessionManager : boost::noncopyable
{
public:
  typedef std::map<std::string, SIPSession::Ptr> Sessions;
  SIPSessionManager();
  ~SIPSessionManager();

  bool findSessionAsUas(const SIPMessage::Ptr& pMsg, SIPSession::Ptr& pSession) const;
  bool findSessionAsUas(const std::string& sessionId, SIPSession::Ptr& pSession) const;
  bool findSessionAsUac(const SIPMessage::Ptr& pMsg, SIPSession::Ptr& pSession) const;
  bool findSessionAsUac(const std::string& sessionId, SIPSession::Ptr& pSession) const;

private:
  OSS::mutex_critic_sec _uasMutex;
  OSS::mutex_critic_sec _uacMutex;
  Sessions _uas;
  Sessions _uac;
};

//
// Inlines
//

} } } // OSS::SIP::UA



#endif	/// OSS_SIPSESSIONMANAGER_H_INCLUDED

