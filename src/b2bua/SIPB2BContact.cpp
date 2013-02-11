

// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: B2BUA
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


#include "OSS/OSS.h"
#include "OSS/Core.h"
#include "OSS/Logger.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/SIP/B2BUA/SIPB2BContact.h"
#include "OSS/SIP/SIPRequestLine.h"


namespace OSS {
namespace SIP {
namespace B2BUA {


bool SIPB2BContact::_dialogStateInParams = false;
bool SIPB2BContact::_registerStateInParams = false;
bool SIPB2BContact::_dialogStateInRecordRoute = false;
const char* SIPB2BContact::_regPrefix = "sbc-reg";

bool SIPB2BContact::transform(SIPB2BTransactionManager* pManager,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  const OSS::IPAddress& localInterface,
  const SessionInfo& sessionInfo)
{
  std::string transportScheme = "udp";
  if (pRequest->isResponse())
  {
    transportScheme = pTransaction->serverTransport()->getTransportScheme();
  }
  else
  {
    std::string via = pRequest->hdrGet("via");
    OSS::string_to_lower(via);

    if (via.find("2.0/udp") != std::string::npos)
      transportScheme = "udp";
    else if (via.find("2.0/tcp") != std::string::npos)
      transportScheme = "tcp";
    else if (via.find("2.0/tls") != std::string::npos)
      transportScheme = "tls";
  }

  OSS::IPAddress iface = localInterface;
  std::string externalIp;
  if (pManager->getExternalAddress(transportScheme, localInterface, externalIp))
    iface.externalAddress() = externalIp;

  if (SIPB2BContact::_dialogStateInParams)
  {
    return SIPB2BContact::transformAsParams(
      pManager,
      pRequest,
      pTransaction,
      iface,
      transportScheme,
      sessionInfo);
  }
  else if (_dialogStateInRecordRoute)
  {
    return SIPB2BContact::transformAsRecordRouteParams(
      pManager,
      pRequest,
      pTransaction,
      iface,
      transportScheme,
      sessionInfo);
  }
  else
  {
    return SIPB2BContact::transformAsUserInfo(
      pManager,
      pRequest,
      pTransaction,
      iface,
      transportScheme,
      sessionInfo);
  }
}

bool SIPB2BContact::transformAsParams(SIPB2BTransactionManager* pManager,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  const OSS::IPAddress& localInterface,
  const std::string& transportScheme,
  const SessionInfo& sessionInfo)
{
  SIPFrom from = pRequest->hdrGet("from");
  std::ostringstream contact;
  std::string user = from.getUser();

  if (!user.empty())
    contact << user << " ";

  contact << "<sip:";
  if (!user.empty())
    contact << user << "@";

  OSS::IPAddress hostPort;
  if (!const_cast<OSS::IPAddress&>(localInterface).externalAddress().empty())
    hostPort = OSS::IPAddress(const_cast<OSS::IPAddress&>(localInterface).externalAddress(), localInterface.getPort());
  else
    hostPort = localInterface;

  contact << hostPort.toIpPortString();
  contact << ";transport=" << transportScheme;

  if (!sessionInfo.sessionId.empty())
    contact << ";" << "sbc-session-id=" << sessionInfo.sessionId;
  if (sessionInfo.callIndex != 0)
    contact << ";" << "sbc-call-index=" << sessionInfo.callIndex;
  contact<< ">";

  pRequest->hdrListRemove("Contact");
  pRequest->hdrListPrepend("Contact", contact.str());

  if (sessionInfo.callIndex == 1)
    pTransaction->setProperty("leg1-contact", contact.str());
  else if (sessionInfo.callIndex == 2)
    pTransaction->setProperty("leg2-contact", contact.str());

  return true;
}

bool SIPB2BContact::transformAsRecordRouteParams(SIPB2BTransactionManager* pManager,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  const OSS::IPAddress& localInterface,
  const std::string& transportScheme,
  const SessionInfo& sessionInfo)
{
  SIPFrom from = pRequest->hdrGet("from");
  std::ostringstream contact;
  std::string user = from.getUser();

  //
  // Set the display name
  //
  if (!user.empty())
    contact << user << " ";

  contact << "<sip:";
  if (!user.empty())
    contact << user << "@";


  OSS::IPAddress hostPort;
  if (!const_cast<OSS::IPAddress&>(localInterface).externalAddress().empty())
    hostPort = OSS::IPAddress(const_cast<OSS::IPAddress&>(localInterface).externalAddress(), localInterface.getPort());
  else
    hostPort = localInterface;

  contact << hostPort.toIpPortString();
  contact << ";transport=" << transportScheme << ">";

  pRequest->hdrListRemove("Contact");
  pRequest->hdrListPrepend("Contact", contact.str());

  if (sessionInfo.callIndex == 1)
    pTransaction->setProperty("leg1-contact", contact.str());
  else if (sessionInfo.callIndex == 2)
    pTransaction->setProperty("leg2-contact", contact.str());

  std::ostringstream recordRoute;
  recordRoute << "<sip:" <<  hostPort.toIpPortString() << ";" << "transport=" << transportScheme;

  if (!sessionInfo.sessionId.empty())
    recordRoute << ";" << "sbc-session-id=" << sessionInfo.sessionId;
  if (sessionInfo.callIndex != 0)
    recordRoute << ";" << "sbc-call-index=" << sessionInfo.callIndex;
  recordRoute<< ";lr>";

  if (sessionInfo.callIndex == 1)
    pTransaction->setProperty("leg1-rr", recordRoute.str());
  else if (sessionInfo.callIndex == 2)
    pTransaction->setProperty("leg2-rr", recordRoute.str());

  pRequest->hdrListPrepend("Record-Route", recordRoute.str().c_str());
  return true;
}

bool SIPB2BContact::transformAsUserInfo(SIPB2BTransactionManager* pManager,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  const OSS::IPAddress& localInterface,
  const std::string& transportScheme,
  const SessionInfo& sessionInfo)
{
  SIPFrom from = pRequest->hdrGet("from");
  std::string user = from.getUser();
  std::ostringstream contact;

  if (!user.empty())
    contact << user << " ";

  if (!sessionInfo.sessionId.empty())
    contact << "<sip:" << sessionInfo.sessionId << "-" << sessionInfo.callIndex << "@";
  else
    contact << "<sip:" << user << "@";

  OSS::IPAddress hostPort;
  if (!const_cast<OSS::IPAddress&>(localInterface).externalAddress().empty())
    hostPort = OSS::IPAddress(const_cast<OSS::IPAddress&>(localInterface).externalAddress(), localInterface.getPort());
  else
    hostPort = localInterface;

  contact << hostPort.toIpPortString();
  contact << ";transport=" << transportScheme << ">";

  pRequest->hdrListRemove("Contact");
  pRequest->hdrListPrepend("Contact", contact.str());

  if (sessionInfo.callIndex == 1)
    pTransaction->setProperty("leg1-contact", contact.str());
  else if (sessionInfo.callIndex == 2)
    pTransaction->setProperty("leg2-contact", contact.str());

  return true;
}

bool SIPB2BContact::getSessionInfo(
  SIPB2BTransactionManager* pManager,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  SessionInfo& sessionInfo)
{
  //
  // WARNING! pTransaction can be NULL for ACKs
  //
  std::string logId;
  if (!pTransaction)
    logId = pRequest->createContextId(true);
  else
    logId = pTransaction->getLogId();

  SIPRequestLine rline = pRequest->startLine();
  SIPURI requestUri;
  if (!rline.getURI(requestUri))
    return false;

  std::string method = pRequest->getMethod();
  std::string user = requestUri.getUser();
  if (requestUri.hasParam("sbc-session-id") && requestUri.hasParam("sbc-call-index"))
  {
    sessionInfo.sessionId = requestUri.getParam("sbc-session-id");
    sessionInfo.callIndex = OSS::string_to_number<unsigned>(requestUri.getParam("sbc-call-index").c_str());
    OSS_LOG_DEBUG(logId << "Session-ID in Params - " << sessionInfo.sessionId << "-" << sessionInfo.callIndex);
    return true;
  }
  else
  {
    /// check if the session-id is in the route header
    //pRequest->hdrGet("route");
    std::string routeList = pRequest->hdrGet("route");
    RouteURI routeUri;
    if (!routeList.empty())
    {
      SIPRoute::getAt(routeList, routeUri, 0);
      SIPURI uri = routeUri.getURI();
      if (uri.hasParam("sbc-session-id") && uri.hasParam("sbc-call-index"))
      {
        pRequest->hdrListRemove("route");
        sessionInfo.sessionId = uri.getParam("sbc-session-id");
        sessionInfo.callIndex = OSS::string_to_number<unsigned>(uri.getParam("sbc-call-index").c_str());
        OSS_LOG_DEBUG(logId << "Session-ID in Route Header - " << sessionInfo.sessionId << "-" << sessionInfo.callIndex);
        return true;
      }
    }
    else if (!requestUri.hasParam("sbc-session-id") || !requestUri.hasParam("sbc-call-index"))
    {
      std::vector<std::string> userTokens = OSS::string_tokenize(user, "-");
      if (userTokens.size() == 2 && userTokens[1].size() == 1)
      {
        sessionInfo.sessionId = userTokens[0];
        sessionInfo.callIndex = OSS::string_to_number<unsigned>(userTokens[1].c_str());
        OSS_LOG_DEBUG(logId << "Session-ID in User Info - " << sessionInfo.sessionId << "-" << sessionInfo.callIndex);
        return true;
      }
    }
  }

  OSS_LOG_ERROR(logId << "Unable to parse Session-ID for request " << pRequest->startLine());
  return false;
}

bool SIPB2BContact::transformRegister(
  SIPB2BTransactionManager* pManager,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  const OSS::IPAddress& localInterface)
{
  //
  // Extract the REGISTER expiration
  //
  std::string hContactList = pRequest->hdrGet("contact");
  std::string expires = pRequest->hdrGet("expires");
  ContactURI curi;
  if (!hContactList.empty())
  {
    SIPContact::getAt(hContactList, curi, 0);
    pRequest->hdrListRemove("Contact");
  }

  if (expires.empty())
  {
    if (hContactList == "*")
    {
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest);
      return serverError;
    }
    if (!hContactList.empty())
    {
      expires = curi.getHeaderParam("expires");
      if (expires.empty())
        expires = "3600";
    }
  }

  if (!hContactList.empty() && hContactList != "*")
  {
    if (SIPB2BContact::_registerStateInParams)
    {
      return transformRegisterAsParams(
        pManager,
        pRequest,
        curi,
        localInterface,
        expires);
    }
    else
    {
      return transformRegisterAsUserInfo(
        pManager,
        pRequest,
        curi,
        localInterface,
        expires);
    }
  }
  else if (hContactList == "*")
  {
    pRequest->hdrListPrepend("Contact", "*");
    return true;
  }
  else
  {
    //
    // No contact, this is a registration query
    //
    return true;
  }

  return false;
}

bool SIPB2BContact::transformRegisterAsUserInfo(
  SIPB2BTransactionManager* pManager,
  const SIPMessage::Ptr& pRequest,
  const ContactURI& curi,
  const OSS::IPAddress& localInterface,
  const std::string& expires)
{
  //
  // Prepare the new contact
  //
  std::string hdrTo = pRequest->hdrGet("to");
  std::string toURI;
  if (!OSS::SIP::SIPTo::getURI(hdrTo, toURI))
  {
    return false;
  }
  std::string toUser;
  OSS::SIP::SIPURI::getUser(toURI, toUser);
  std::ostringstream contact;
  std::string hostPort = curi.getHostPort();

  if (const_cast<OSS::IPAddress&>(localInterface).externalAddress().empty())
  {
    contact << "<sip:" << _regPrefix << "-" << toUser << "-" << OSS::string_hash(hostPort.c_str()) << "@"
      << localInterface.toIpPortString() << ">";
  }
  else
  {
    contact << "<sip:" << _regPrefix << "-" << toUser << "-" << OSS::string_hash(hostPort.c_str()) << "@"
      << const_cast<OSS::IPAddress&>(localInterface).externalAddress()
      << ":" <<  const_cast<OSS::IPAddress&>(localInterface).getPort() << ">";
  }

  if (!expires.empty())
    contact << ";expires=" << expires;

  pRequest->hdrListPrepend("Contact", contact.str());
  return true;
}

bool SIPB2BContact::transformRegisterAsParams(
  SIPB2BTransactionManager* pManager,
  const SIPMessage::Ptr& pRequest,
  const ContactURI& curi,
  const OSS::IPAddress& localInterface,
  const std::string& expires)
{
  //
  // Prepare the new contact
  //
  std::string hdrTo = pRequest->hdrGet("to");
  std::string toURI;
  if (!OSS::SIP::SIPTo::getURI(hdrTo, toURI))
  {
    return false;
  }
  std::string toUser;
  OSS::SIP::SIPURI::getUser(toURI, toUser);
  std::ostringstream contact;
  std::string hostPort = curi.getHostPort();

  if (const_cast<OSS::IPAddress&>(localInterface).externalAddress().empty())
  {
    contact << "<sip:" << toUser << "@" << localInterface.toIpPortString() ;
  }
  else
  {
    contact << "<sip:" << toUser << "@"
      << const_cast<OSS::IPAddress&>(localInterface).externalAddress()
      << ":" <<  const_cast<OSS::IPAddress&>(localInterface).getPort();
  }
  contact << ";" << _regPrefix << "=" << OSS::string_hash(hostPort.c_str())<< ">";

  if (!expires.empty())
    contact << ";expires=" << expires;

  pRequest->hdrListPrepend("Contact", contact.str());
  return true;
}


bool SIPB2BContact::isRegisterRoute(const SIPMessage::Ptr& pRequest)
{
  return (pRequest->startLine().find(_regPrefix) != std::string::npos);
}

std::string SIPB2BContact::constructVia(SIPB2BTransactionManager* pManager,
  const SIPMessage::Ptr& pRequest,
  const OSS::IPAddress& localInterface,
  const std::string& transportScheme,
  const std::string& branchHash,
  bool canUseExternalAddress)
{
  std::string targetTransport = transportScheme;
  OSS::string_to_upper(targetTransport);
  std::ostringstream via;
  via << "SIP/2.0/" << targetTransport << " " ;

  std::string externalIp;
  if (canUseExternalAddress && pManager->getExternalAddress(transportScheme, localInterface, externalIp) && !externalIp.empty())
    via << externalIp << ":" << localInterface.getPort();
  else
    via << localInterface.toIpPortString();

  via << ";branch=" << branchHash << ";rport";

  return via.str();
}

} } } // OSS::SIP::B2BUA



