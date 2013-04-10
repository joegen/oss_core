
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


#include "OSS/SIP/B2BUA/SIPB2BUserAgentHandlerList.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"


namespace OSS {
namespace SIP {
namespace B2BUA {



SIPB2BUserAgentHandlerList::SIPB2BUserAgentHandlerList()
{
}

SIPB2BUserAgentHandlerList::~SIPB2BUserAgentHandlerList()
{
}



void SIPB2BUserAgentHandlerList::addHandler(SIPB2BUserAgentHandler* pHandler)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  Item item;
  item.handler = pHandler;
  _handlers.push_back(item);
  _handlers.sort();
}

SIPB2BUserAgentHandler::Action SIPB2BUserAgentHandlerList::operator()(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  OSS::mutex_critic_sec_lock lock(_mutex);
  SIPB2BUserAgentHandler::Action action = SIPB2BUserAgentHandler::Continue;
  for (std::list<Item>::iterator iter = _handlers.begin(); iter != _handlers.end(); iter++)
  {
    action = iter->handler->handleRequest(pMsg, pTransport, pTransaction);
    if (action != SIPB2BUserAgentHandler::Continue)
      return action;
  }

  return action;
}

} } } // OSS::SIP::B2BUA




