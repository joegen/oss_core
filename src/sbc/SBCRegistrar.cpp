

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


#include "OSS/SIP/SBC/SBCRegistrar.h"
#include "OSS/UTL/Logger.h"
#include "OSS/SIP/SIPContact.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/UTL/PropertyMap.h"
#include "OSS/SIP/SIPRequestLine.h"
#include "OSS/SIP/SBC/SBCManager.h"



static const char* REG_EP = "reg-ep";


namespace OSS {
namespace SIP {
namespace SBC {
  

typedef std::vector<std::string> Keys;  
  

SBCRegistrar::SBCRegistrar() :
  EndpointListener(REG_EP),
  _pManager(0)
{
}

SBCRegistrar::~SBCRegistrar()
{ 
}

void SBCRegistrar::attachSBCManager(SBCManager* pManager)
{
  _pManager = pManager;
}

void SBCRegistrar::handleStart()
{ 
  OSS_LOG_NOTICE("SBC Local Registrar event loop has STARTED");
}

void SBCRegistrar::handleStop()
{ 
  OSS_LOG_NOTICE("SBC Local Registrar event loop has ENDED");
}

void SBCRegistrar::onHandleEvent(const SIPMessage::Ptr& pRequest)
{
  OSS_LOG_DEBUG( pRequest->createContextId(true) << "SBCRegistrar::onHandleEvent() - " << pRequest->startLine());
  
  if (!pRequest->isRequest("REGISTER"))
  {
    SIPMessage::Ptr pResponse;
    pResponse = pRequest->createResponse(501, "Registrar Received A Non-REGISTER Request");
    dispatchMessage(pResponse);
    return;
  }
  //
  // Get the contact list
  //
  ContactList contactList;
  SIPContact::msgGetContacts(pRequest.get(), contactList);
  
  //
  // Get the to header
  //
  SIPTo hdrTo(pRequest->hdrGet(SIP::HDR_TO));
  
  //
  // Get the message-wide expires header
  //
  std::string expires(pRequest->hdrGet(SIP::HDR_EXPIRES));
  OSS::string_trim(expires);
  
  //
  // Determine the type of message
  //
  EventType eventType = REG_INVALID;
  if (contactList.empty())
  {
    if (expires == "0")
    {
      eventType = REG_UNREG;
    }
    else
    {
      eventType = REG_QUERY;
    }
      
  }
  else if (expires == "0")
  {
    eventType = REG_UNREG;
  }
  else if (!expires.empty())
  {
    eventType = REG_UPDATE;
  }
  else
  {
    //
    // There is no global expires so won't be able to figure out if this is an unregister or an update
    // We need to actually check the contacts for individual expires.  handleUpdate will take care of this
    //
    eventType = REG_UPDATE;
  }
  //
  // Note:  We do not take care of authenticating REGISTER request.  
  // We will assume that everything that reaches the registrar are
  // already preauthenticated and has a valid account in the system
  //
  switch (eventType)
  {
    case REG_UPDATE:
      OSS_LOG_DEBUG( pRequest->createContextId(true) << "SBCRegistrar::onHandleEvent(REG_UPDATE) - " << pRequest->startLine());
      handleUpdate(pRequest, hdrTo, contactList, expires);
      break;
    case REG_QUERY:
      OSS_LOG_DEBUG( pRequest->createContextId(true) << "SBCRegistrar::onHandleEvent(REG_QUERY) - " << pRequest->startLine());
      handleQuery(pRequest, hdrTo);
      break;
    case REG_UNREG:
      OSS_LOG_DEBUG( pRequest->createContextId(true) << "SBCRegistrar::onHandleEvent(REG_UNREG) - " << pRequest->startLine());
      handleUnreg(pRequest, hdrTo, contactList);
      break;
    case REG_INVALID:
      OSS_LOG_ERROR( pRequest->createContextId(true) << "SBCRegistrar::onHandleEvent(REG_INVALID) - " << pRequest->startLine());
      break;
  }  
}

void SBCRegistrar::handleQuery(const SIPMessage::Ptr& pRequest, const SIPTo& to)
{
  SIPURI toUri(to.getURI());
  dispatchContacts(pRequest, toUri);
}

void SBCRegistrar::handleUnreg(const SIPMessage::Ptr& pRequest, const SIPTo& to, ContactList& contactList)
{
  if (contactList.empty())
  {
    removeBinding(pRequest, to, "*");
  }
  else
  {
    removeBinding(pRequest, to, contactList.front().data());
  }
}

void SBCRegistrar::handleUpdate(const SIPMessage::Ptr& pRequest, const SIPTo& to, const ContactList& contactList, const std::string& expires)
{ 
  if (contactList.size() > 1)
  {
    //
    // We know this is allowed but we yet have to see an actual use case for this.  Instead of putting up the logic now
    // we will wait if there is really an actual need for that.  For now, we warn.
    //
    OSS_LOG_WARNING("SBCRegistrar::handleUpdate got a register with multiple contacts.  Only using the first contact.");
  }

  //
  // Get the expires from the first item if it exists
  //
  std::string paramExpires = contactList.front().getHeaderParam("expires");
  if (paramExpires.empty())
  {
    paramExpires = expires;
  }

  if (paramExpires.empty())
  {
    //
    // Assume the SIP default
    //
    paramExpires = "3600";
  }

  OSS::string_trim(paramExpires);
  
  if (paramExpires != "0")
  {
    updateBinding(pRequest, to, contactList.front(), paramExpires);
  }
  else
  {
    removeBinding(pRequest, to, contactList.front().data());
  }
}

bool SBCRegistrar::storeBinding(const std::string& key, const SBCRegistrationRecord& binding)
{
  if (!_regDb)
  {
    return false;
  }
  return binding.writeToRedis(*_regDb, key);
}

void SBCRegistrar::dispatchContacts(const SIPMessage::Ptr& pRequest, const SIPURI& aor)
{
  if (!_regDb)
  {
    SIPMessage::Ptr pResponse;
    pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_500_InternalServerError);
    dispatchMessage(pResponse);
    return;
  }
  
  SIPMessage::Ptr pResponse;
  pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_200_Ok);
  
  std::ostringstream key;
  key << aor.getIdentity(false) << "-*";
  
  std::vector<std::string> keys;
  _regDb->getKeys(key.str(), keys);
  
  for (std::vector<std::string>::iterator iter = keys.begin(); iter != keys.end(); iter++)
  {
    SBCRegistrationRecord binding;
    if (binding.readFromRedis(*_regDb, *iter))
    {
      int elapsedTime = (OSS::getTime() - binding.timeStamp()) / 1000;
      int actualExpires = binding.expires() - elapsedTime;
      ContactURI curi(binding.contact());
      curi.setHeaderParam("expires", OSS::string_from_number<int>(actualExpires).c_str());
      pResponse->hdrListAppend(OSS::SIP::HDR_CONTACT, curi.data());
    }
  }
  
  pResponse->commitData(); 
  dispatchMessage(pResponse);
}

void SBCRegistrar::updateBinding(const SIPMessage::Ptr& pRequest, const SIPTo& to, const ContactURI& contact, const std::string& expires)
{
  SIPURI toUri(to.getURI());
  std::string binding(contact.getHostPort());
    
  //
  // Create the registration record
  //
  SBCRegistrationRecord record;
  record.aor() = to.getAor(false);
  record.callId() = pRequest->hdrGet(SIP::HDR_CALL_ID);
  record.contact() = contact.getURI();
  record.enc() = false;
  record.expires() = OSS::string_to_number<int>(expires);
  record.transportId() = pRequest->hdrGet("X-SBC-Transport-Id");
  record.localInterface() = pRequest->hdrGet("X-SBC-Local-Interface");
  record.packetSource() = pRequest->hdrGet("X-SBC-Packet-Source");
  record.targetTransport() = pRequest->hdrGet("X-SBC-Target-Transport");

  
  std::ostringstream key;
  key << record.aor() << "-" << record.callId() << "-" << binding;
  
  std::ostringstream allKeys;
  allKeys << record.aor() << "-*";
  
  if (!_regDb)
  {
    SIPMessage::Ptr pResponse;
    pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_500_InternalServerError);
    dispatchMessage(pResponse);
    return;
  }
  
  Keys keys;
  _regDb->getKeys(allKeys.str(), keys);
  
  if (keys.empty() || (keys.size() == 1 && keys.front() == key.str()))
  {
    //
    // This is a fresh registration or update. Simply set it in the db
    //
    if (!storeBinding(key.str(), record))
    {
      SIPMessage::Ptr pResponse;
      pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_500_InternalServerError);
      dispatchMessage(pResponse);
      return;
    }
    else
    {
      dispatchContacts(pRequest, toUri);
      return;
    }
  }

  
  //
  // If we reach this point, it means we have a bunch of existing registrations.
  // Make sure we get rid of duplicates
  //
  std::ostringstream duplicateKeys;
  duplicateKeys << record.aor() << "-*-" << binding;
  keys.clear();
  _regDb->getKeys(duplicateKeys.str(), keys);
  for (Keys::iterator iter = keys.begin(); iter != keys.end(); iter++)
  {
    _regDb->del(*iter);
  }
  
  if (!storeBinding(key.str(), record))
  {
    SIPMessage::Ptr pResponse;
    pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_500_InternalServerError);
    dispatchMessage(pResponse);
    return;
  }
  else
  {
    dispatchContacts(pRequest, toUri);
    return;
  }
}

void SBCRegistrar::removeBinding(const SIPMessage::Ptr& pRequest, const SIPTo& to, const std::string& contact)
{
  if (!_regDb)
  {
    SIPMessage::Ptr pResponse;
    pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_500_InternalServerError);
    dispatchMessage(pResponse);
    return;
  }
  
  //
  // Get the call-id
  //
  std::string callId(pRequest->hdrGet(SIP::HDR_CALL_ID));
  std::string aor = to.getAor(false);
  SIPURI toUri(to.getURI());
  
  if (contact == "*")
  {
    std::ostringstream key;
    key << aor << "-*";
    Keys keys;
    _regDb->getKeys(key.str(), keys);
    for (Keys::iterator iter = keys.begin(); iter != keys.end(); iter++)
    {
      _regDb->del(*iter);
    }
  }
  else
  {
    ContactURI curi(contact);
    std::string binding = curi.getHostPort();
    std::ostringstream key;
    key << aor << "-" << callId << "-" << binding;
    _regDb->del(key.str());
  }
  
  dispatchContacts(pRequest, toUri);
}

bool SBCRegistrar::getBindings(const SIPURI& aor, ContactList& bindings)
{
  if (!_regDb)
  {
    return false;
  }
  
  std::ostringstream key;
  key << aor.getIdentity(false) << "-*";
  
  std::vector<std::string> keys;
  _regDb->getKeys(key.str(), keys);
  
  for (std::vector<std::string>::iterator iter = keys.begin(); iter != keys.end(); iter++)
  {
    
    json::Object regRecord;
    _regDb->get(*iter, regRecord);

    json::Object::iterator contact = regRecord.Find("contact");
    if (contact != regRecord.End())
    {
      json::String element = contact->element;
      ContactURI curi(element.Value());
      bindings.push_back(curi);
    }
  }
  
  return !bindings.empty();
}

bool SBCRegistrar::getRegistrations(const std::string& identity, Registrations& registrations)
{
  if (!_regDb)
  {
    return false;
  }
  
  std::ostringstream key;
  key << identity << "-*";
  
  std::vector<std::string> keys;
  _regDb->getKeys(key.str(), keys);
  
  for (std::vector<std::string>::iterator iter = keys.begin(); iter != keys.end(); iter++)
  {
    SBCRegistrationRecord registration;
    if (registration.readFromRedis(*_regDb, *iter))
    {
      registrations.insert(registration);
    }
  }
  
  return !registrations.empty();
}

bool SBCRegistrar::onRouteLocalReg(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target)
{
  //
  // We follow a simple rule.
  // If this is a REGISTER request and the authenticator processed it
  // Then this is a local registration
  //
  
  if (!pRequest->isRequest("REGISTER"))
  {
    return false;
  }
  std::string locallyAythenticated;
  if (!pTransaction->getProperty(OSS::PropertyMap::PROP_IsLocallyAuthenticated, locallyAythenticated) || locallyAythenticated != "yes")
  {
    return false;
  }
  
  //
  // Route this request to the local registrar
  //
  pRequest->setProperty(OSS::PropertyMap::PROP_TargetTransport, getEndpointName());
  pRequest->setProperty(OSS::PropertyMap::PROP_TargetAddress, "127.0.0.1");
  pRequest->setProperty(OSS::PropertyMap::PROP_TargetPort, "0");
  localInterface = OSS::Net::IPAddress("127.0.0.1");
  target = OSS::Net::IPAddress("127.0.0.1"); 
  
  OSS_LOG_INFO(pRequest->createContextId(true) << "SBCRegistrar::onRouteTransaction - Flagged " << pRequest->hdrGet(OSS::SIP::HDR_FROM) << " as LOCAL");
  
  return true;
}


SIPMessage::Ptr SBCRegistrar::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  SIPMessage::Ptr pResponse;
  if (!_pManager)
  {
    return pResponse;
  }
  
  std::string logId = pRequest->createContextId(true);

  SIPRequestLine rline(pRequest->startLine());
  SIPURI ruri;
  rline.getURI(ruri);
  std::string identity = ruri.getIdentity(false, false);
  std::string realm = ruri.getHost();
  
  //
  // Check if an explicit target identity is given.
  // This would bypass the request-uri
  //
  std::string targetIdentity;
  if (pRequest->getProperty(OSS::PropertyMap::PROP_TargetIdentity, targetIdentity))
  {
    identity = targetIdentity;
  }

  //
  // Realms are very cheap to check so we check this first prior to actually
  // checking if the identity exists
  //
  if (!_pManager->authenticator().accounts().isKnownRealm(realm))
  {
    //
    // Not ours
    //
    OSS_LOG_INFO(logId << "SBCRegistrar::onRouteTransaction - Realm " << realm << " is not local");
    
    if (!targetIdentity.empty())
    {
      pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_404_NotFound);
    }
    
    return pResponse;
  }
  
//
// WebRTC demo accounts are not known identities so we do not check here
// instead, getRegistrations should simply return false
//  
#if 0
  if (!_pManager->authenticator().accounts().isKnownIdentity(identity))
  {
    //
    // Not ours
    //
    OSS_LOG_INFO(logId << "SBCRegistrar::onRouteTransaction - Identity " << identity << " is not local");
    if (!targetIdentity.empty())
    {
      pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_404_NotFound);
    }
    return pResponse;
  }
#endif  
  //
  // Reject subscribes and publish until we support it
  //
  if (pRequest->isRequest("SUBSCRIBE") || pRequest->isRequest("PUBLISH"))
  {
    pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_501_NotImplemented);
    return pResponse;
  }
  
  
  //
  // Let's see if this identity got a registered binding
  //
  Registrations registrations;
  if (getRegistrations(identity, registrations) && !registrations.empty())
  {
    //
    // We only support single bindings for now.  This will change when we can already do forking
    //
    Registrations::iterator reg = registrations.begin();    
    pRequest->setProperty(OSS::PropertyMap::PROP_TargetTransport, reg->targetTransport());
    target = OSS::Net::IPAddress::fromV4IPPort(reg->packetSource().c_str());
    localInterface = OSS::Net::IPAddress::fromV4IPPort(reg->localInterface().c_str());
    
    pRequest->setProperty(OSS::PropertyMap::PROP_TransportId, reg->transportId());
    pRequest->setProperty(OSS::PropertyMap::PROP_InterfaceAddress, localInterface.toString());
    pRequest->setProperty(OSS::PropertyMap::PROP_InterfacePort, OSS::string_from_number<unsigned short>(localInterface.getPort()));
    pRequest->setProperty(OSS::PropertyMap::PROP_TargetAddress, target.toString());
    pRequest->setProperty(OSS::PropertyMap::PROP_TargetPort, OSS::string_from_number<unsigned short>(target.getPort()));
    pRequest->setProperty(OSS::PropertyMap::PROP_RouteAction, "accept");
    
    pTransaction->setProperty("local-reg-tran", "1");
    //
    // rewrite the startline
    //
    rline.setURI(reg->contact().c_str());
    pRequest->setStartLine(rline.data());
      
    OSS_LOG_INFO(logId << "SBCRegistrar::onRouteTransaction - Identity " << identity << " has an existing registration.  Target: " << reg->packetSource() << " Local: " <<reg->localInterface());
    
    if (!target.isValid())
    {
      OSS_LOG_WARNING(logId << "SBCRegistrar::onRouteTransaction - Identity " << identity << " has an existing registration but the target address is not valid");
    }
    
    if (!localInterface.isValid())
    {
      OSS_LOG_WARNING(logId << "SBCRegistrar::onRouteTransaction - Identity " << identity << " has an existing registration but the local interface address is not valid");
    }
    
    return pResponse;
  }
  else
  {
    OSS_LOG_INFO(logId << "SBCRegistrar::onRouteTransaction - Identity " << identity << " has no existing registrations");
    if (!_pManager->authenticator().accounts().isKnownIdentity(identity))
    {
      OSS_LOG_INFO(logId << "SBCRegistrar::onRouteTransaction - Identity " << identity << " allowing dial plan routing");
      return pResponse;
    }
  }
  
  pResponse = pRequest->createResponse(OSS::SIP::SIPMessage::CODE_480_TemporarilyNotAvailable);
  
  return pResponse;
}
  

} } }  // OSS::SIP::SBC::REG




