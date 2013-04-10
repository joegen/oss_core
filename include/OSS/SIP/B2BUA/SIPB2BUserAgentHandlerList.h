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

#ifndef OSS_SIPB2BUSERAGENTHANDLERLIST_H_INCLUDED
#define	OSS_SIPB2BUSERAGENTHANDLERLIST_H_INCLUDED


#include "OSS/Thread.h"
#include "OSS/SIP/B2BUA/SIPB2BUserAgentHandler.h"
#include "OSS/DynamicHashTable.h"

namespace OSS {
namespace SIP {
namespace B2BUA {


class SIPB2BUserAgentHandlerList : boost::noncopyable
{
public:
  struct Item
  {
    SIPB2BUserAgentHandler* handler;
    bool operator < (const SIPB2BUserAgentHandlerList::Item& h) const;
  };

  SIPB2BUserAgentHandlerList();

  ~SIPB2BUserAgentHandlerList();

  void addHandler(SIPB2BUserAgentHandler* pHandler);

  SIPB2BUserAgentHandler::Action operator()(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);
private:
  std::list<Item> _handlers;
  OSS::mutex_critic_sec _mutex;
};


//
// Inlines
//

inline bool SIPB2BUserAgentHandlerList::Item::operator < (const SIPB2BUserAgentHandlerList::Item& h) const
{
  return handler->priority() < h.handler->priority();
}

} } } // OSS::SIP::B2BUA



#endif	/// OSS_SIPB2BUSERAGENTHANDLERLIST_H_INCLUDED

