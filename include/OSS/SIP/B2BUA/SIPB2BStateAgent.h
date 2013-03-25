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

#ifndef OSS_SIPSTATEAGENT_H_INCLUDED
#define	OSS_SIPSTATEAGENT_H_INCLUDED


#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "OSS/SIP/OSSSIP.h"


namespace OSS {
namespace SIP {
namespace B2BUA {

class SIPB2BTransactionManager;

class SIPB2BStateAgent : boost::noncopyable
{
public:

  typedef boost::function<bool(
    SIPB2BStateAgent&,
    const OSS::SIP::SIPMessage::Ptr&,
    const OSS::SIP::SIPTransportSession::Ptr&,
    const OSS::SIP::SIPTransaction::Ptr&)> Handler;

  SIPB2BStateAgent(SIPB2BTransactionManager* pB2BUA);
  ~SIPB2BStateAgent();

  bool handleRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);

protected:
  SIPB2BTransactionManager* _pB2BUA;
  Handler _handler;
};


} } } // OSS::SIP::B2BUA


#endif	// OSS_SIPSTATEAGENT_H_INCLUDED

