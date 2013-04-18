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

#ifndef OSS_SIPSESSION_H_INCLUDED
#define	OSS_SIPSESSION_H_INCLUDED

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "OSS/SIP/UA/SIPSessionState.h"
#include "OSS/SIP/SIPMessage.h"


namespace OSS {
namespace SIP {
namespace UA {


class SIPSession : boost::noncopyable
{
public:
  typedef std::vector<SIPSessionState> Branches;
  typedef boost::shared_ptr<SIPSession> Ptr;
  enum UAType
  {
    UAS,
    UAC
  };

  SIPSession(UAType uaType);
  ~SIPSession();

  static bool getSessionId(const SIPMessage::Ptr& pMsg, UAType uaType, std::string& sessionId);

private:
  UAType _uaType;
  std::string _callId;
};



//
// Inlines
//

} } } // OSS::SIP::UA



#endif	/// OSS_SIPSESSION_H_INCLUDED

