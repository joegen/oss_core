
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

#include "OSS/SIP/B2BUA/SIPB2BStateAgent.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"


namespace OSS {
namespace SIP {
namespace B2BUA {


SIPB2BStateAgent::SIPB2BStateAgent(SIPB2BTransactionManager* pB2BUA) :
  _pB2BUA(pB2BUA)
{
}

SIPB2BStateAgent::~SIPB2BStateAgent()
{
}

bool SIPB2BStateAgent::handleRequest(
  const OSS::SIP::SIPMessage::Ptr& pMsg,
  const OSS::SIP::SIPTransportSession::Ptr& pTransport,
  const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  if (!_handler)
    return false;
  return _handler(*this, pMsg, pTransport, pTransaction);
}



} } } // OSS::SIP::B2BUA




