
// OSS Software Solutions Application Programmer Interface
//
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


#include "OSS/OSS.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/SIP/SBC/SBCContact.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/SIP/SBC/SBCDirectories.h"


namespace OSS {
namespace SIP {
namespace SBC {


using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;
using OSS::Persistent::PersistenceException;

bool SBCContact::_dialogStateInParams = false;
bool SBCContact::_registerStateInParams = false;
bool SBCContact::_dialogStateInRecordRoute = true;

void SBCContact::initialize(const boost::filesystem::path& configFile)
{
  ClassType config;
  if (config.load(configFile))
  {
    DataType root = config.self();
    DataType userAgent = root["user-agent"];
    if (userAgent.exists("register-state-in-contact-params"))
      SBCContact::_registerStateInParams = (bool)userAgent["register-state-in-contact-params"];
    if (userAgent.exists("dialog-state-in-contact-params"))
      SBCContact::_dialogStateInParams = (bool)userAgent["dialog-state-in-contact-params"];
  }
}

bool SBCContact::isFromRegisteredUser(
  SBCManager* pSBC,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  std::string& regId)
{
  //
  // Check if this user is registered
  // and use the sbc-reg as the username
  //
  std::string hContactList;
  
  if (!pRequest->getProperty("inbound-contact", hContactList) || hContactList.empty())
    return false;

  ContactURI curi;
  SIPContact::getAt(hContactList, curi, 0);
  std::ostringstream regState;
  std::string hostPort = curi.getHostPort();
  SIPFrom from = pRequest->hdrGet("from");
  std::string user =  from.getUser();
  regState << "sbc-reg-" << user << "-" << OSS::string_hash(hostPort.c_str());
  
  boost::filesystem::path fn(SBCDirectories::instance()->getRegistrationStateDirectory());
  operator/(fn, regState.str());
  bool yes = boost::filesystem::exists(fn);
  if (yes)
    regId = regState.str();

  return yes;
}

bool SBCContact::transform(SBCManager* pSBC,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  const OSS::Net::IPAddress& localInterface,
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
    else if (via.find("2.0/wss") != std::string::npos)
      transportScheme = "wss";
    else if (via.find("2.0/ws") != std::string::npos)
      transportScheme = "ws";
  }

  OSS::Net::IPAddress iface = localInterface;
  std::string externalIp;
  if (pSBC->getExternalAddress(transportScheme, localInterface, externalIp))
    iface.externalAddress() = externalIp;
  
  std::string dialogStatePlacement;
  pTransaction->getProperty("dialog-state-placement", dialogStatePlacement);
  
  if (dialogStatePlacement == "c")
  {
    OSS_LOG_DEBUG(pTransaction->getLogId() << "Dialog state transformation using Contact parameters.");
    return SBCContact::transformAsParams(
      pSBC,
      pRequest,
      pTransaction,
      iface,
      transportScheme,
      sessionInfo);
  }
  else if (dialogStatePlacement == "r")
  {
    OSS_LOG_DEBUG(pTransaction->getLogId() << "Dialog state transformation using Record-Route parameters.");
    return SBCContact::transformAsRecordRouteParams(
      pSBC,
      pRequest,
      pTransaction,
      iface,
      transportScheme,
      sessionInfo);
  }
  else if (SBCContact::_dialogStateInParams)
  {
    OSS_LOG_DEBUG(pTransaction->getLogId() << "Dialog state transformation using Contact parameters.");


    return SBCContact::transformAsParams(
      pSBC,
      pRequest,
      pTransaction,
      iface,
      transportScheme,
      sessionInfo);
  }
  else if (_dialogStateInRecordRoute)
  {
    OSS_LOG_DEBUG(pTransaction->getLogId() << "Dialog state transformation using Record-Route parameters.");

    return SBCContact::transformAsRecordRouteParams(
      pSBC,
      pRequest,
      pTransaction,
      iface,
      transportScheme,
      sessionInfo);
  }
  else
  {
    OSS_LOG_DEBUG(pTransaction->getLogId() << "Dialog state transformation using User-Info.");
    return SBCContact::transformAsUserInfo(
      pSBC,
      pRequest,
      pTransaction,
      iface,
      transportScheme,
      sessionInfo);
  }
}

bool SBCContact::transformAsParams(SBCManager* pSBC,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  const OSS::Net::IPAddress& localInterface,
  const std::string& transportScheme,
  const SessionInfo& sessionInfo)
{
  pSBC->transformPreDialogPersist(pRequest, pTransaction);
    
  SIPFrom from = pRequest->hdrGet("from");
  SIPTo to = pRequest->hdrGet("to");

  std::ostringstream contact;
  std::string user = pRequest->isRequest() ? from.getUser() : to.getUser();

  if (!user.empty())
    contact << user << " ";

  contact << "<sip:";
  if (!user.empty())
    contact << user << "@";

  OSS::Net::IPAddress hostPort;
  if (!const_cast<OSS::Net::IPAddress&>(localInterface).externalAddress().empty())
    hostPort = OSS::Net::IPAddress(const_cast<OSS::Net::IPAddress&>(localInterface).externalAddress(), localInterface.getPort());
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

bool SBCContact::transformAsRecordRouteParams(SBCManager* pSBC,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  const OSS::Net::IPAddress& localInterface,
  const std::string& transportScheme,
  const SessionInfo& sessionInfo)
{
  pSBC->transformPreDialogPersist(pRequest, pTransaction);
  
  SIPFrom from = pRequest->hdrGet("from");
  SIPTo to = pRequest->hdrGet("to");
  std::ostringstream contact;
  std::string user = pRequest->isRequest() ? from.getUser() : to.getUser();

  //
  // Set the display name
  //
  if (!user.empty())
    contact << user << " ";

  std::string regId;
  if (isFromRegisteredUser(pSBC, pRequest, pTransaction, regId))
  {
    contact << "<sip:";
    if (!regId.empty())
      contact << regId << "@";
  }
  else
  {
    contact << "<sip:";
    if (!user.empty())
      contact << user << "@";
  }

  OSS::Net::IPAddress hostPort;
  if (!const_cast<OSS::Net::IPAddress&>(localInterface).externalAddress().empty())
    hostPort = OSS::Net::IPAddress(const_cast<OSS::Net::IPAddress&>(localInterface).externalAddress(), localInterface.getPort());
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

bool SBCContact::transformAsUserInfo(SBCManager* pSBC,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  const OSS::Net::IPAddress& localInterface,
  const std::string& transportScheme,
  const SessionInfo& sessionInfo)
{
  pSBC->transformPreDialogPersist(pRequest, pTransaction);
  
  SIPFrom from = pRequest->hdrGet("from");
  SIPFrom to = pRequest->hdrGet("to");
  std::string user = pRequest->isRequest() ? from.getUser() : to.getUser();
  std::ostringstream contact;

  if (!user.empty())
    contact << user << " ";

  if (!sessionInfo.sessionId.empty())
    contact << "<sip:" << sessionInfo.sessionId << "-" << sessionInfo.callIndex << "@";
  else
    contact << "<sip:" << user << "@";

  OSS::Net::IPAddress hostPort;
  if (!const_cast<OSS::Net::IPAddress&>(localInterface).externalAddress().empty())
    hostPort = OSS::Net::IPAddress(const_cast<OSS::Net::IPAddress&>(localInterface).externalAddress(), localInterface.getPort());
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

bool SBCContact::getSessionInfo(
  SBCManager* pSBC,
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
  
  
  if (pTransaction)
  {
    std::string sessionId;
    if (pTransaction->getProperty("session-id", sessionId) && !sessionId.empty())
    {
      sessionInfo.sessionId = sessionId;
      sessionInfo.callIndex = 0;
      OSS_LOG_DEBUG(logId << "Unable to parse Call-Index for request " << pRequest->startLine());
      return true;
    }
  }

  OSS_LOG_ERROR(logId << "Unable to parse Session-ID for request " << pRequest->startLine());
  return false;
}

bool SBCContact::transformRegister(
  SBCManager* pSBC,
  const SIPMessage::Ptr& pRequest,
  const SIPB2BTransaction::Ptr& pTransaction,
  const OSS::Net::IPAddress& localInterface)
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
      return false;
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
    if (SBCContact::_registerStateInParams)
    {
      return transformRegisterAsParams(
        pSBC,
        pRequest,
        curi,
        localInterface,
        expires);
    }
    else
    {
      return transformRegisterAsUserInfo(
        pSBC,
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

bool SBCContact::transformRegisterAsUserInfo(
  SBCManager* pSBC,
  const SIPMessage::Ptr& pRequest,
  const ContactURI& curi,
  const OSS::Net::IPAddress& localInterface,
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
  SIPURI contactUri(curi.getURI());
  std::string rinstance;
  contactUri.getParam("rinstance", rinstance);
  std::string hashable = hostPort + rinstance;

  if (const_cast<OSS::Net::IPAddress&>(localInterface).externalAddress().empty())
  {
    contact << "<sip:sbc-reg-" << toUser << "-" << OSS::string_hash(hashable.c_str()) << "@"
      << localInterface.toIpPortString() << ">";
  }
  else
  {
    contact << "<sip:sbc-reg-" << toUser << "-" << OSS::string_hash(hashable.c_str()) << "@"
      << const_cast<OSS::Net::IPAddress&>(localInterface).externalAddress()
      << ":" <<  const_cast<OSS::Net::IPAddress&>(localInterface).getPort() << ">";
  }

  if (!expires.empty())
  {
    pRequest->hdrRemove(OSS::SIP::HDR_EXPIRES);
    contact << ";expires=" << expires;
  }

  pRequest->hdrListPrepend("Contact", contact.str());
  return true;
}

bool SBCContact::transformRegisterAsParams(
  SBCManager* pSBC,
  const SIPMessage::Ptr& pRequest,
  const ContactURI& curi,
  const OSS::Net::IPAddress& localInterface,
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
  SIPURI contactUri(curi.getURI());
  std::string rinstance;
  contactUri.getParam("rinstance", rinstance);
  std::string hashable = hostPort + rinstance;

  if (const_cast<OSS::Net::IPAddress&>(localInterface).externalAddress().empty())
  {
    contact << "<sip:" << toUser << "@" << localInterface.toIpPortString() ;
  }
  else
  {
    contact << "<sip:" << toUser << "@"
      << const_cast<OSS::Net::IPAddress&>(localInterface).externalAddress()
      << ":" <<  const_cast<OSS::Net::IPAddress&>(localInterface).getPort();
  }
  contact << ";sbc-reg=" << OSS::string_hash(hashable.c_str())<< ">";

  if (!expires.empty()) 
  {
    pRequest->hdrRemove(OSS::SIP::HDR_EXPIRES);
    contact << ";expires=" << expires;
  }

  pRequest->hdrListPrepend("Contact", contact.str());
  return true;
}


bool SBCContact::isRegisterRoute(const SIPMessage::Ptr& pRequest)
{
  return (pRequest->startLine().find("sbc-reg") != std::string::npos);
}

bool SBCContact::getRegistrationIdentifier(const ContactURI& curi, std::string& regId)
{
  SIPURI uri = curi.getURI();
  return getRegistrationIdentifier(uri, regId);
}

bool SBCContact::getRegistrationIdentifier(const SIPURI& uri, std::string& regId)
{
  std::string user = uri.getUser();
  if (user.find("sbc-reg") != std::string::npos)
  {
    regId = user;
    return true;
  }
  else
  {
    if (!uri.hasParam("sbc-reg"))
      return false;
    regId = "sbc-reg-";
    regId += user;
    regId += "-";
    regId += uri.getParam("sbc-reg");
    return true;
  }
  return false;
}

bool SBCContact::getContactUriFromRegisterResponse(const std::string& logId, const SIPB2BTransaction::Ptr& pTransaction, const SIPMessage::Ptr& pResponse, ContactURI& curi, std::string& regId_)
{
  SIPMessage::Ptr pClientRequest = pTransaction->clientRequest();
  if (!pClientRequest)
  {
    OSS_LOG_ERROR(logId << "SBCContact::getContactUriFromResponse - Client Request is NULL");
    return false;
  }
  
  std::vector<std::string> bindings;
  if (SIPContact::msgGetContacts(pClientRequest.get(), bindings) == 0)
  {
    OSS_LOG_ERROR(logId << "SBCContact::getContactUriFromResponse - Unable to parse contact from client request.");
    return false;
  }
  std::string localBinding = bindings[0];
  if (localBinding.find("sbc-reg") == std::string::npos)
  {
    OSS_LOG_ERROR(logId << "SBCContact::getContactUriFromResponse - Missing sbc-reg prefix in binding " << localBinding);
    return false;
  }
  
  ContactURI localUri = localBinding;
  std::string regId;
  if (!SBCContact::getRegistrationIdentifier(localUri, regId) || regId.empty())
  {
    OSS_LOG_ERROR(logId << "SBCContact::getContactUriFromResponse - Unable to parse regId from " << localBinding);
    return false;
  }
  
  std::vector<std::string> responseBindings;
  if (SIPContact::msgGetContacts(pResponse.get(), responseBindings) == 0)
  {
    if (pResponse->is2xx())
    {
      OSS_LOG_ERROR(logId << "SBCContact::getContactUriFromResponse - Unable to parse contact from response");
    }
    return false;
  }
  
  for (std::vector<std::string>::iterator iter = responseBindings.begin(); iter != responseBindings.end(); iter++)
  {
    if (iter->find("sbc-reg") == std::string::npos)
    {
      continue;
    }
    ContactURI responseUri = *iter;
    std::string responseRegId;
    if (SBCContact::getRegistrationIdentifier(responseUri, responseRegId) && responseRegId == regId)
    {
      OSS_LOG_DEBUG(logId << "SBCContact::getContactUriFromResponse - reg-id " << responseRegId << " FOUND");
      regId_ = regId;
      curi = responseUri;
      return true;
    }
  }
 
  OSS_LOG_ERROR(logId << "SBCContact::getContactUriFromResponse - Our contact is missing from remote rsponse");
  return false;
}

std::string SBCContact::constructVia(SBCManager* pSBC,
  const SIPMessage::Ptr& pRequest,
  const OSS::Net::IPAddress& localInterface,
  const std::string& transportScheme,
  const std::string& branchHash)
{
  std::string targetTransport = transportScheme;
  OSS::string_to_upper(targetTransport);
  std::ostringstream via;
  via << "SIP/2.0/" << targetTransport << " " ;
  
  std::string externalIp;
  if (pSBC->getExternalAddress(transportScheme, localInterface, externalIp) && !externalIp.empty())
    via << externalIp << ":" << localInterface.getPort();
  else
    via << localInterface.toIpPortString();

  via << ";branch=" << branchHash << ";rport";

  return via.str();
}

} } } // OSS::SIP::SBC



