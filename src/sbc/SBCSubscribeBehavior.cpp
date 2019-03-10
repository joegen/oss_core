

// OSS Software Solutions Application Programmer Interface
// Package: SBC
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/SIP/SBC/SBCSubscribeBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPRoute.h"

namespace OSS {
namespace SIP {
namespace SBC {

using OSS::SIP::SIPMessage;
using OSS::Net::IPAddress;

SBCSubscribeBehavior::SBCSubscribeBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_SUBSCRIBE, "SBC SUBSCRIBE Request Handler"),
  _subscriptions_2(2*3600),
  _subscriptions_4(4*3600),
  _subscriptions_8(8*3600),
  _subscriptions_16(16*3600),
  _subscriptions_24(24*3600)
{
  setName("SBC SUBSCRIBE Request Handler");
  setMaxPacketsPerSecond(pManager->getMaxSubscribesPerSecond());
}

SBCSubscribeBehavior::~SBCSubscribeBehavior()
{
}

SIPMessage::Ptr SBCSubscribeBehavior::onUpdateSubscription(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target)
{
  return SIPMessage::Ptr();
}

SIPMessage::Ptr SBCSubscribeBehavior::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());

  SIPMessage::Ptr sub;
  if (!pRequest->isMidDialog())
  {
    SIPMessage::Ptr ret = SBCDefaultBehavior::onRouteTransaction(pRequest, pTransaction, localInterface, target);
    if (ret)
      return ret;

    pTransaction->serverRequest()->setProperty("subscribe-target-address", target.toIpPortString());
    pTransaction->serverRequest()->setProperty("subscribe-localInterface", localInterface.toIpPortString());
  }
  else
  {
    sub = findSubscription(pRequest, pTransaction);

    if (!sub)
    {
      return pRequest->createResponse(481);
    }

    std::string targetAddress;
    std::string localInterfaceAddress;
    sub->getProperty("subscribe-target-address", targetAddress);
    sub->getProperty("subscribe-localInterface", localInterfaceAddress);

    if (!targetAddress.empty() && !localInterfaceAddress.empty())
    {
      target = OSS::Net::IPAddress::fromV4IPPort(targetAddress.c_str());
      localInterface = OSS::Net::IPAddress::fromV4IPPort(localInterfaceAddress.c_str());
    }
  }

  std::string oldVia;
  SIPVia::msgGetTopVia(pRequest.get(), oldVia);
  std::string branch;
  SIPVia::getBranch(oldVia, branch);
  if (branch.empty())
  {
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Missing Via Branch Parameter");
    return serverError;
  }
  //
  // Prepare the SIP Message for outbound
  //
  pRequest->hdrListRemove("Route");
  pRequest->hdrListRemove("Record-Route");
  pRequest->hdrListRemove("Via");

  //
  // Preserver the contact as property header.  it will be used for transformation later
  //
  pRequest->setProperty("inbound-contact", pRequest->hdrGet("contact").c_str());
  pRequest->hdrListRemove("Contact");

  //
  // Set the target tranport
  //
  std::string targetTransport;
  if (!pRequest->getProperty(OSS::PropertyMap::PROP_TargetTransport, targetTransport) || targetTransport.empty())
  {
    targetTransport = "udp";
    pRequest->setProperty(OSS::PropertyMap::PROP_TargetTransport, "udp");
  }

  //
  // Prepare the new via
  //
  OSS::string_to_upper(targetTransport);
  std::ostringstream viaBranch;
  viaBranch << "z9hG4bK" << OSS::string_hash(branch.c_str());
  std::string newVia = SBCContact::constructVia(_pManager, pRequest, localInterface, targetTransport, viaBranch.str());
  pRequest->hdrListPrepend("Via", newVia);

  //
  // Prepare the new contact
  //
  std::ostringstream sessionId;
  sessionId << OSS::string_hash(pRequest->hdrGet("call-id").c_str()) << OSS::getRandom();
  pTransaction->setProperty("session-id", sessionId.str());

  SBCContact::SessionInfo sessionInfo;
  sessionInfo.sessionId = sessionId.str();
  sessionInfo.callIndex = 2;

  SBCContact::transform(this->getManager(),
    pRequest,
    pTransaction,
    localInterface,
    sessionInfo);

  if (pRequest->isMidDialog())
  {
    //
    // rewrite the request line
    //
    if (!sub)
    {
      return pRequest->createResponse(481);
    }
    std::string hContact;
    sub->getProperty("subscribe-target-uri", hContact);
    SIPContact contact(hContact);
    ContactURI contactUri;
    contact.getAt(contactUri, 0);
    SIPRequestLine rline(pRequest->startLine());
    rline.setURI(contactUri.getURI().c_str());
    pRequest->setStartLine(rline.data());

    //
    // add the route headers
    //
    if (sub->hdrPresent("route"))
    {
      size_t routeSize = sub->hdrGetSize("route");
      for (size_t i = 0; i < routeSize; i++)
      {
        std::string route = sub->hdrGet("route", i);
        pRequest->hdrListAppend("Route", route.c_str());
      }
    }
  }

  return OSS::SIP::SIPMessage::Ptr();
}





void SBCSubscribeBehavior::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
  /// This is the last chance for the application to process
  /// the outbound response before it gets sent out to the transport.
  ///
  /// This is normally the place where application would want to
  /// insert application-specific headers as well as change existing
  /// headers to the desired application-specific values for as long
  /// as it wont conflict with dialog creation states.
{
  SBCDefaultBehavior::onProcessResponseOutbound(pResponse, pTransaction);

  std::string sessionId;
  std::string legIndex;

  pTransaction->getProperty("session-id", sessionId);
  if (!pTransaction->getProperty("leg-index", legIndex) || legIndex.empty())
    legIndex = "1";

  if (!pResponse->is1xx(100))
  {
    SBCContact::SessionInfo sessionInfo;
    sessionInfo.sessionId = sessionId;

    if (!legIndex.empty())
      sessionInfo.callIndex = OSS::string_to_number<unsigned>(legIndex.c_str());

    SBCContact::transform(this->getManager(),
      pResponse,
      pTransaction,
      pTransaction->serverTransport()->getLocalAddress(),
      sessionInfo);

    std::string newContact = pResponse->hdrGet("contact");
    pTransaction->serverRequest()->setProperty("notify-local-contact", newContact.c_str());
    pTransaction->serverRequest()->setProperty("notify-local-address",
    pTransaction->serverTransport()->getLocalAddress().toIpPortString().c_str());
    pTransaction->serverRequest()->setProperty("notify-target-address",
    pTransaction->serverTransport()->getRemoteAddress().toIpPortString().c_str());
    pTransaction->serverRequest()->setProperty("notify-from-uri", pResponse->hdrGet("to").c_str());
  }

  //
  // Register the subscription
  //
  registerSubscribe(pTransaction->serverRequest(), pResponse, pTransaction);
}

void SBCSubscribeBehavior::onProcessResponseInbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  if (pResponse->is2xx())
  {
    SIPMessage::Ptr pRequest = pTransaction->serverRequest();
    //
    // Preserve the contact
    //
    std::string contact;
    contact = pResponse->hdrGet("contact", 0);
    if (!contact.empty())
      pRequest->setProperty("subscribe-target-uri", contact);

    if (pResponse->hdrGetSize("record-route") > 0)
    {
      //
      // Preserve the record routes
      //
      std::list<std::string> recordRoutes;
      SIPRoute::msgGetRecordRoutes(pResponse.get(), recordRoutes);

      for (std::list<std::string>::reverse_iterator iter = recordRoutes.rbegin();
        iter != recordRoutes.rend(); iter++)
      {
        SIPRoute route(*iter);
        ContactURI ruri;
        if (route.getAt(ruri, 0))
        {
          SIPURI routeUri(ruri.getURI());
          std::string host;
          unsigned short port;
          if (routeUri.getHostPort(host, port))
          {
            IPAddress routeAddr(host, port);
            if (_pManager->isLocalTransport("udp", routeAddr) ||
              _pManager->isLocalTransport("tcp", routeAddr) ||
              _pManager->isLocalTransport("tls", routeAddr))
              continue;
            pRequest->hdrListAppend("Route", (*iter).c_str());
          }
        }
      }
    }
  }
}

void SBCSubscribeBehavior::registerSubscribe(
  const SIPMessage::Ptr& pRequest,
  const SIPMessage::Ptr& pResponse,
  const SIPB2BTransaction::Ptr& pTransaction)
{
  std::string hExpires = pResponse->hdrGet("expires");
  if (hExpires.empty())
    return;
  std::string hCallId = pRequest->hdrGet("call-id");
  if (hCallId.empty())
    return;
  std::string hEvent = pRequest->hdrGet("event");
  if (hEvent.empty())
    return;
  std::string hContact = pRequest->hdrGet("contact");
  if (hContact.empty())
    return;


  pRequest->setProperty(OSS::PropertyMap::PROP_TransportId, OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier()).c_str());
  pRequest->setProperty(OSS::PropertyMap::PROP_TargetTransport,  pTransaction->serverTransport()->getTransportScheme().c_str());
  

  int expires = OSS::string_to_number<int>(hExpires.c_str());

  CacheManager* pCache = &_subscriptions_24;
  if (expires <= (2*3600))
    pCache = &_subscriptions_2;
  else if (expires <= (4*3600))
    pCache = &_subscriptions_4;
  else if (expires <= (8*3600))
    pCache = &_subscriptions_8;
  else if (expires <= (16*3600))
    pCache = &_subscriptions_16;
  else
    pCache = &_subscriptions_24;

  std::string sid = hEvent + hCallId;

  if (expires > 0)
  {
    boost::any cacheData = pRequest;
    Cacheable::Ptr pSubscription(new Cacheable(sid, cacheData));
    pCache->add(pSubscription);
  }
  else
  {
    expireSubscription(pRequest, pTransaction);
  }
}

SIPMessage::Ptr SBCSubscribeBehavior::findSubscription(
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction)
{
  std::string hCallId = pRequest->hdrGet("call-id");
  if (hCallId.empty())
  {
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Bad Call-ID Header");
    return SIPMessage::Ptr();
  }

  std::string hEvent = pRequest->hdrGet("event");
  if (hEvent.empty())
  {
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Bad Event Header");
    return SIPMessage::Ptr();
  }
  std::string sid = hEvent + hCallId;

  Cacheable::Ptr pSubscription = _subscriptions_2.get(sid);
  if (!pSubscription)
    pSubscription = _subscriptions_4.get(sid);
  if (!pSubscription)
    pSubscription = _subscriptions_8.get(sid);
  if (!pSubscription)
    pSubscription = _subscriptions_16.get(sid);
  if (!pSubscription)
    pSubscription = _subscriptions_24.get(sid);

  if (!pSubscription)
    return SIPMessage::Ptr();

  return boost::any_cast<SIPMessage::Ptr&>(pSubscription->data());
}

void SBCSubscribeBehavior::expireSubscription(
    const SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction)
{
  std::string hCallId = pRequest->hdrGet("call-id");
  if (hCallId.empty())
  {
    return;
  }

  std::string hEvent = pRequest->hdrGet("event");
  if (hEvent.empty())
  {
    return;
  }
  std::string sid = hEvent + hCallId;

  CacheManager* pCache = &_subscriptions_2;
  bool hasSubscription = _subscriptions_2.has(sid);
  if (!hasSubscription)
  {
    pCache = &_subscriptions_4;
    hasSubscription = _subscriptions_4.has(sid);
  }
  if (!hasSubscription)
  {
    pCache = &_subscriptions_8;
    hasSubscription = _subscriptions_8.has(sid);
  }
  if (!hasSubscription)
  {
    pCache = &_subscriptions_16;
    hasSubscription = _subscriptions_16.has(sid);
  }
  if (!hasSubscription)
  {
    pCache = &_subscriptions_24;
    hasSubscription = _subscriptions_24.has(sid);
  }

  if (pCache && hasSubscription)
  {
    pCache->remove(sid);
  }
}

SIPMessage::Ptr SBCSubscribeBehavior::onRouteNotify(
    SIPMessage::Ptr& pRequest,
    const SIPB2BTransaction::Ptr& pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target)
{
  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  
  SIPMessage::Ptr pSubscribe = findSubscription(pRequest, pTransaction);
  if (!pSubscribe)
  {
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_404_NotFound, "Subscription Does Not Exist");
    return serverError;
  }
  
  return onInternalRouteNotify(pRequest, pSubscribe, pTransaction, localInterface, target);
}

SIPMessage::Ptr SBCSubscribeBehavior::onInternalRouteNotify(
  SIPMessage::Ptr& pNotify,
  const SIPMessage::Ptr& pSubscribe,
  const SIPB2BTransaction::Ptr& pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  std::string hContact = pSubscribe->hdrGet("contact");
  std::string localAddress;
  OSS_VERIFY(pSubscribe->getProperty("notify-local-address", localAddress));
  std::string targetAddress;
  OSS_VERIFY(pSubscribe->getProperty("notify-target-address", targetAddress));
  OSS_VERIFY(!hContact.empty() && !localAddress.empty() && !targetAddress.empty());
  std::string transportId;
  OSS_VERIFY(pSubscribe->getProperty(OSS::PropertyMap::PROP_TransportId, transportId) && !transportId.empty());
  pNotify->setProperty(OSS::PropertyMap::PROP_TransportId, transportId);
  std::string targetTransport;
  OSS_VERIFY(pSubscribe->getProperty(OSS::PropertyMap::PROP_TargetTransport,  targetTransport) && !targetTransport.empty());
  OSS::string_to_upper(targetTransport);
  pNotify->setProperty(OSS::PropertyMap::PROP_TargetTransport, targetTransport);


  localInterface = OSS::Net::IPAddress::fromV4IPPort(localAddress.c_str());
  _pManager->getInternalAddress(localInterface, localInterface);

  ContactURI rURI;
  if (!SIPContact::getAt(hContact, rURI, 0))
  {
    SIPMessage::Ptr serverError = pNotify->createResponse(SIPMessage::CODE_500_InternalServerError, "Bad Subscription Request");
    return serverError;
  }
  //
  // Set the target address for the notify
  //
  OSS::Net::IPAddress requestTarget = OSS::Net::IPAddress::fromV4IPPort(rURI.getHostPort().c_str());
  if (requestTarget.isPrivate())
    target = OSS::Net::IPAddress::fromV4IPPort(targetAddress.c_str());
  else
    target = requestTarget;
  //
  // Set the new startline
  //
  std::ostringstream startLine;
  startLine << "NOTIFY " << rURI.getURI() << " SIP/2.0";
  pNotify->setStartLine(startLine.str());
  //
  // Set the call-id
  //
  pNotify->hdrSet("Call-ID", pSubscribe->hdrGet("call-id").c_str());
  //
  // Set the from header
  //
  std::string from;
  OSS_VERIFY(pSubscribe->getProperty("notify-from-uri", from) && !from.empty());
  pNotify->hdrSet("From", from.c_str());
  //
  // Set the to header
  //
  pNotify->hdrSet("To", pSubscribe->hdrGet("from").c_str());
  //
  // Set the new contact
  //
  pNotify->hdrListRemove("contact");
  std::string newContact;
  OSS_VERIFY(pSubscribe->getProperty("notify-local-contact", newContact));
  OSS_VERIFY(!newContact.empty());
  pNotify->hdrSet("Contact", newContact.c_str());
  //
  // set the new via
  //
  std::string oldVia;
  SIPVia::msgGetTopVia(pNotify.get(), oldVia);
  std::string branch;
  SIPVia::getBranch(oldVia, branch);
  if (branch.empty())
  {
    SIPMessage::Ptr serverError = pNotify->createResponse(SIPMessage::CODE_400_BadRequest, "Missing Via Branch Parameter");
    return serverError;
  }
  pNotify->hdrListRemove("Via");
  std::ostringstream via;
  if (localInterface.externalAddress().empty())
    via << "SIP/2.0/" << targetTransport << " " << localAddress << ";branch=" << branch << ";rport";
  else
    via << "SIP/2.0/" << targetTransport << " " << localInterface.externalAddress() << ":" << localInterface.getPort() << ";branch=" << branch << ";rport";

  pNotify->hdrSet("Via", via.str().c_str());
  //
  // Add the route set
  //
  pNotify->hdrListRemove("route");
  pNotify->hdrListRemove("record-route");
  std::list<std::string> routeList;
  if (SIPRoute::msgGetRecordRoutes(pSubscribe.get(), routeList) > 0)
  {
    SIPFrom topRoute = *(routeList.begin());
    bool strictRoute = topRoute.data().find(";lr") == std::string::npos;
    if (strictRoute)
    {
      std::ostringstream sline;
      sline << "NOTIFY " << topRoute.getURI() << " SIP/2.0";
      pNotify->startLine() = sline.str();
      std::ostringstream newRoute;
      newRoute << "<" << rURI.getURI() << ">";
      *(routeList.begin()) = newRoute.str();
    }

    for (std::list<std::string>::iterator iter = routeList.begin();
      iter != routeList.end(); iter++)
    {
      SIPRoute::msgAddRoute(pNotify.get(), *iter);
    }

    SIPURI topRouteURI = topRoute.getURI();

    std::string host;
    unsigned short port = 0;
    topRouteURI.getHostPort(host, port);

    OSS::dns_host_record_list hosts = OSS::dns_lookup_host(host);
    if (!hosts.empty())
    {
      if (port == 0)
        port = 5060;
      target = *(hosts.begin());
      target.setPort(port);
    }
  }

  return SIPMessage::Ptr();
}

} } } // OSS::SIP::SBC


