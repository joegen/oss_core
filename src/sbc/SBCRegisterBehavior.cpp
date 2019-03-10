

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

#include "OSS/OSS.h"

#include "OSS/Net/DNS.h"
#include "OSS/UTL/Logger.h"
#include "OSS/SIP/SBC/SBCRegisterBehavior.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SIPXOR.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/SIP/SIPVia.h"


namespace OSS {
namespace SIP {
namespace SBC {


using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;
using OSS::Net::IPAddress;

#define DEFAULT_KEEP_ALIVE_FREQUENCY_IN_SECONDS 5

SBCRegisterBehavior::SBCRegisterBehavior(SBCManager* pManager) :
  SBCDefaultBehavior(pManager, OSS::SIP::B2BUA::SIPB2BHandler::TYPE_REGISTER, "SBC REGISTER Request Handler"),
  _pOptionsThread(0),
  _optionsThreadExit(0, 0xFFFF),
  _pOptionsResponseThread(0),
  _optionsResponseThreadExit(0, 0xFFFF),
  _threadPool(1, 10),
  _enableOptionsKeepAlive(true)
{
  setName("SBC REGISTER Request Handler");
  _keepAliveResponseCb = boost::bind(&SBCRegisterBehavior::handleOptionsResponse, this, _1, _2, _3, _4);
  setMaxPacketsPerSecond(pManager->getMaxRegistersPerSecond());
}

SBCRegisterBehavior::~SBCRegisterBehavior()
{
  delete _pOptionsThread;
  delete _pOptionsResponseThread;
}

void SBCRegisterBehavior::initialize()
{
  _redisClient = _pManager->redis().getRegDb();
  _enableOptionsKeepAlive = _pManager->isOptionsKeepAliveEnabled();
  
  _pManager->console().addCommand("gateway", "gateway status", "Display the gateway status");
  _pManager->console().addCommand("gateway", "gateway status all", "Display the status of all gateways");
  _pManager->console().addCommandHandler(boost::bind(&SBCRegisterBehavior::cliGetGatewayStatus, this, _1, _2));
}

void SBCRegisterBehavior::startOptionsThread()
{
  //
  // Initialize the options keep-alive thread
  //
  OSS_ASSERT(_pOptionsResponseThread == 0);
  OSS_ASSERT(_pOptionsThread == 0);
  _pOptionsThread = new boost::thread(boost::bind(&SBCRegisterBehavior::runOptionsThread, this));
  _pOptionsResponseThread = new boost::thread(boost::bind(&SBCRegisterBehavior::runOptionsResponseThread, this));

}

void SBCRegisterBehavior::deinitialize()
{
  //
  // Exit the option keep-alive loop
  //
  _optionsThreadExit.set();
  _pOptionsThread->join();
  _optionsResponseThreadExit.set();
  _optionsResponseQueue.enqueue("exit");
  _pOptionsResponseThread->join();
}

void SBCRegisterBehavior::runOptionsThread()
{
  int currentIteration = 0;
  unsigned int segmentSize = 0;
  unsigned int nextSegment = 0;
  std::vector<std::string> keys;
  
  OSS_LOG_INFO("SBCRegisterBehavior::runOptionsThread - Keep-alive thread STARTED");
  
  while(!_optionsThreadExit.tryWait(DEFAULT_KEEP_ALIVE_FREQUENCY_IN_SECONDS * 1000))
  {
    if (_pauseKeepAlive)
    {
      //
      // Keep-alive is paused
      //
      OSS_LOG_INFO("SBCRegisterBehavior::runOptionsThread - Keep-alive thread is PAUSED");
      continue;
    }
    
    //
    // Send CRLF keep alive for every iteration
    //
    {
      OSS::mutex_read_lock readLock(_rwKeepAliveListMutex);
      //_keepAliveList[packetSource] = localInterface;
      for (KeepAliveList::iterator iter = _keepAliveList.begin(); iter != _keepAliveList.end(); iter++)
        sendUDPKeepAlive(iter->second, iter->first);
    }
    
    //
    // Send keep=alive to gateways
    //
    sendKeepAliveToGateways();
    
    if (_enableOptionsKeepAlive)
    {
      currentIteration++;

      if (currentIteration == 1)
      {
        std::vector<std::string> upperReg;
        _redisClient->getKeys("sbc-reg*", upperReg);

        std::vector<std::string> localReg;
        _pManager->redis().getLocalRegDb()->getKeys("*", localReg);

        keys.clear();
        keys.reserve(upperReg.size() + localReg.size());
        keys.insert( keys.end(), upperReg.begin(), upperReg.end() );
        keys.insert( keys.end(), localReg.begin(), localReg.end() );

        segmentSize = keys.size() / 12;
        if (segmentSize == 0)
          segmentSize = 12;
        nextSegment = 0;
        size_t maxIter = keys.size();
        for (size_t i = nextSegment; i < nextSegment + segmentSize && i < maxIter; i++)
        {
          sendOptionsKeepAlive(keys[i]);
        }
        nextSegment += segmentSize;
      }
      else
      {
        size_t maxIter = keys.size();
        for (size_t i = nextSegment; i < nextSegment + segmentSize && i < maxIter; i++)
        {
          sendOptionsKeepAlive(keys[i]);
        }
        nextSegment += segmentSize;
        if (currentIteration == 12)
          currentIteration = 0;
      }
    }
  }
}

void SBCRegisterBehavior::sendOptionsKeepAlive(const std::string& regRecord)
{ 
  if (_pauseKeepAlive)
  {
    //
    // Keep-alive is paused
    //
    return;
  }
  static int cseqNo = 1;
  
  try
  {
    //
    // Upper-reg and local-reg  keys different.
    // Local-reg is the full identity of the account + the call-id + the binding
    //
    SBCRegistrationRecord registration;
    std::string localUser;
    std::size_t atIndex = regRecord.find("@");
    std::ostringstream aor;
    if (atIndex != std::string::npos)
    {
      localUser = OSS::string_left(regRecord, atIndex);
      
      if (!registration.readFromRedis(*(_pManager->redis().getLocalRegDb()), regRecord))
        return;
      aor << "sip:" << registration.aor();
    }
    else
    {
      localUser = regRecord;
      if (!registration.readFromRedis(*_redisClient, regRecord))
        return;
      aor << registration.aor();
    }
    
    

    
    
    std::string contact = registration.contact();
    //
    // Skip local reg OPTIONS
    //
    if (contact.find(PropertyMap::propertyString(PropertyMap::PROP_LocalReg)) != std::string::npos)
    {
      return;
    }

    std::string callId = OSS::string_create_uuid();
    size_t hash = OSS::string_hash(callId.c_str());

    SIPTo to(aor.str());
    std::string localInterface = registration.localInterface();
    std::string packetSource = registration.packetSource();
    
    
    bool isXOREcrypted = registration.enc();
    std::string transportScheme = registration.targetTransport();
    OSS::string_to_upper(transportScheme);
    IPAddress src = IPAddress::fromV4IPPort(localInterface.c_str());
    IPAddress target = IPAddress::fromV4IPPort(packetSource.c_str());

    _pManager->getInternalAddress(src, src);

    std::ostringstream options;
    options << "OPTIONS " << contact << " SIP/2.0" << OSS::SIP::CRLF;
    options << "To: " << to.data() << OSS::SIP::CRLF;
    options << "From: sip:" << localUser << "@" << to.getHostPort() << ";tag=" << hash << OSS::SIP::CRLF;
    
    if (src.externalAddress().empty())
      options << "Via: " << "SIP/2.0/" << transportScheme << " " << localInterface << ";branch=z9hG4bK" << hash << ";rport" << OSS::SIP::CRLF;
    else
      options << "Via: " << "SIP/2.0/" << transportScheme << " " << src.externalAddress() << ":" << src.getPort() << ";branch=z9hG4bK" << hash << ";rport"  << OSS::SIP::CRLF;

    options << "Call-ID: " << callId << OSS::SIP::CRLF;
    options << "CSeq: " << cseqNo++ << " OPTIONS" << OSS::SIP::CRLF;

    if (src.externalAddress().empty())
      options << "Contact: " << "<sip:" << localUser << "@" << localInterface << ">" << OSS::SIP::CRLF;
    else
      options << "Contact: " << "<sip:" << localUser << "@" << src.externalAddress() << ":" << src.getPort() << ">" << OSS::SIP::CRLF;

    options << "X-Reg-Key: " << regRecord << OSS::SIP::CRLF;
    options << "Content-Length: 0" << OSS::SIP::CRLF << OSS::SIP::CRLF;

    
    SIPMessage::Ptr msg(new SIPMessage(options.str()));
    if (isXOREcrypted)
      msg->setProperty("xor", "1");

    msg->setProperty(OSS::PropertyMap::PROP_TargetTransport, transportScheme.c_str());

    std::string transportId = registration.transportId();
    if (!transportId.empty())
      msg->setProperty(OSS::PropertyMap::PROP_TransportId, transportId.c_str());
    
    msg->setProperty(OSS::PropertyMap::PROP_RequirePersistentConnection, "yes");

    _pManager->transactionManager().stack().sendRequest(msg, src, target, _keepAliveResponseCb, OSS::SIP::SIPTransaction::TerminateCallback());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream logMsg;
    logMsg << "SBCRegisterBehavior::sendOptionsKeepAlive Failure - "
      << e.message();
    OSS::log_warning(logMsg.str());
  }
}

void SBCRegisterBehavior::runOptionsResponseThread()
{
  while(!_optionsResponseThreadExit.tryWait(0))
  {
    std::string response;
    _optionsResponseQueue.dequeue(response);
    if (response == "exit")
      break;
    else
    {
      try
      {
        SBCRegistrationRecord registration;
        
        std::size_t atIndex = response.find("@");
        
        if (!registration.readFromRedis(atIndex == std::string::npos ? *_redisClient : *(_pManager->redis().getLocalRegDb()), response))
          continue;

        std::ostringstream logMsg;
        logMsg << "Registration Expires: " << response;
        OSS::log_information(logMsg.str());
        //
        // Remove from the keep-alive list
        //
        OSS::mutex_write_lock writeLock(_rwKeepAliveListMutex);
        _keepAliveList.erase(OSS::Net::IPAddress::fromV4IPPort(registration.packetSource().c_str()));

        registration.eraseRedisRecord(atIndex == std::string::npos ? *_redisClient : *(_pManager->redis().getLocalRegDb()));
      }
      catch(OSS::Exception e)
      {
        #if 0
        std::ostringstream logMsg;
        logMsg << "SBCRegisterBehavior::runOptionsResponseThread Failure - "
          << e.message();
        OSS::log_warning(logMsg.str());
        #endif
      }
    }
  }
}

void SBCRegisterBehavior::handleOptionsResponse(
    const OSS::SIP::SIPTransaction::Error& e,
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    const OSS::SIP::SIPTransaction::Ptr& pTransaction)
{
  //
  // First check if this is a gateway options.
  // No need to report gateway options to the queue.
  // We simply flag the gateway as offline or online
  //
  std::string gatewayName;
  SIPMessage::Ptr pRequest = pTransaction->getInitialRequest();
  if (pRequest)
  {
    pRequest->getProperty("gateway-name", gatewayName);
  }
    
  //
  // Only report errors to the queue
  //
  if (e)
  {
    if (gatewayName.empty())
    {
      std::string key = pRequest->hdrGet("X-Reg-Key");
      _optionsResponseQueue.enqueue(key);
    }
    else if (pMsg && pMsg->is4xx(408))
    {
      setGatewayStatus(gatewayName, GatewayOffline);
    }
    
    if (pMsg)
    {
      OSS_LOG_DEBUG("SBCRegisterBehavior::handleOptionsResponse - " << pMsg->startLine());
    }
  }
  else if (!gatewayName.empty())
  {
    setGatewayStatus(gatewayName, GatewayOnline);
  }
    
}

////////////////////////////////////////////////////////////////////////////////


SIPMessage::Ptr SBCRegisterBehavior::onRouteTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  pRequest->userData() = static_cast<OSS_HANDLE>(pTransaction.get());
  
  bool isLocalReg = _pManager->registrar().onRouteLocalReg(pRequest, pTransaction, localInterface, target);
  if (!isLocalReg)
  {
    SIPMessage::Ptr ret = SBCDefaultBehavior::onRouteTransaction(pRequest, pTransaction, localInterface, target);
    if (ret)
    {
      return ret;
    }
  }

  std::string invokeLocalHandler = "0";
  if (pTransaction->getProperty("invoke-local-handler", invokeLocalHandler ) && invokeLocalHandler == "1")
  {
    SIPMessage::Ptr();
  }


   pRequest->hdrListRemove("Route");
    
  if (!isLocalReg)
  {
    pRequest->hdrListRemove("Record-Route");
    pRequest->hdrListRemove("Via");

    
  }
  //
  // Prepare the new via
  //
  std::string targetTransport;
  if (!pRequest->getProperty(OSS::PropertyMap::PROP_TargetTransport, targetTransport) || targetTransport.empty())
    targetTransport = "udp";
  OSS::string_to_upper(targetTransport);
  std::string viaBranch = "z9hG4bK";
  viaBranch += OSS::string_create_uuid();
  std::string newVia = SBCContact::constructVia(_pManager, pRequest, localInterface, targetTransport, viaBranch);
  pRequest->hdrListPrepend("Via", newVia);

  if (!isLocalReg && !SBCContact::transformRegister(_pManager, pRequest, pTransaction, localInterface))
  {
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest);
    return serverError;
  }
  
  //
  // Flag local registration
  //
  if (isLocalReg)
  {
    pTransaction->setProperty("is-local-reg", "yes");
    pRequest->hdrSet("X-SBC-Transport-Id", OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier()).c_str());
    pRequest->hdrSet("X-SBC-Packet-Source", pTransaction->serverTransport()->getRemoteAddress().toIpPortString().c_str());
    pRequest->hdrSet("X-SBC-Local-Interface", pTransaction->serverTransport()->getLocalAddress().toIpPortString().c_str());
    pRequest->hdrSet("X-SBC-Target-Transport", pTransaction->serverTransport()->getTransportScheme().c_str());
  }

  return OSS::SIP::SIPMessage::Ptr();
}

void SBCRegisterBehavior::onProcessResponseInbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  SBCDefaultBehavior::onProcessResponseInbound(pResponse, pTransaction);

  std::string strIsLocalReg;
  if (pTransaction->getProperty("is-local-reg", strIsLocalReg) && strIsLocalReg == "yes")
  {
    return;
  }
  
  std::string logId = pResponse->createContextId();
  
  if (!pResponse->is1xx() && pResponse->hdrGetSize("contact") == 0)
  {
    std::string ct;
    ct = pTransaction->clientRequest()->hdrGet("contact");
    if (ct != "*")
    {
      if (pResponse->is2xx())
      {
        OSS_LOG_WARNING(logId << "Missing contact header in REGISTER response.");
      }
      SIPContact contact;
      contact = ct;
      ContactURI curi;
      contact.getAt(curi, 0);
      //std::string contactUser;
      //contactUser = curi.getUser();
      std::string regId;
      SBCContact::getRegistrationIdentifier(curi, regId);
      if (!regId.empty())
      {
        try
        {
          if (pResponse->is2xx())
          {
            OSS_LOG_INFO(logId << "Deleting REGISTER state " << regId << " Call-ID: " << pResponse->hdrGet(OSS::SIP::HDR_CALL_ID));
          }
          SBCRegistrationRecord::eraseRedisRecord(*_redisClient, regId);
          //
          // Remove from the keep-alive list
          //
          OSS::mutex_write_lock writeLock(_rwKeepAliveListMutex);
          _keepAliveList.erase(pTransaction->serverTransport()->getRemoteAddress());
        }catch(...){}
      }
    }
  }
}

void SBCRegisterBehavior::updateContactsFromStorage(const std::string& logId, const OSS::SIP::SIPMessage::Ptr& pResponse, std::set<std::string>* excludeRegId)
{
  std::vector<std::string> bindings;
  int count = SIPContact::msgGetContacts(pResponse.get(), bindings);
  if (count > 0)
  {
    pResponse->hdrListRemove("contact");
    for(std::vector<std::string>::iterator iter = bindings.begin(); iter != bindings.end(); iter++)
    {
      ContactURI curi = *iter;
      std::string currentRegId;
      if (SBCContact::getRegistrationIdentifier(curi, currentRegId))
      {
        if (excludeRegId && excludeRegId->find(currentRegId) != excludeRegId->end())
        {
          continue;
        }

        try
        {
          SBCRegistrationRecord registration;
          if (!registration.readFromRedis(*_redisClient, currentRegId))
          {
            OSS_LOG_DEBUG(logId << "SBCRegisterBehavior::updateContactsFromStorage - Excluding expired rregistration  " << currentRegId);
            continue;
          }
          std::string expires = curi.getHeaderParam("expires");
          ContactURI binding = registration.contact();
          if (!expires.empty())
          {
            binding.setHeaderParam("expires", expires.c_str());
          }

          OSS_LOG_INFO(logId << "Inserting shared upper-reg binding [" << binding.data() << " -> " << *iter << "]");
          pResponse->hdrListAppend("contact", binding.data().c_str());
        }
        catch(...)
        {
          OSS_LOG_DEBUG(logId << "SBCRegisterBehavior::updateContactsFromStorage - Unknown exception while processing " << *iter);
        }
      }
      else if (iter->find("sbc-reg") == std::string::npos)
      {
        OSS_LOG_INFO(logId << "Inserting External-Reg binding " << "[" << *iter << "]");
        pResponse->hdrListAppend("contact", iter->c_str());
      }
    }
  }
}

void SBCRegisterBehavior::onProcessResponseOutbound(
  SIPMessage::Ptr& pResponse,
  SIPB2BTransaction::Ptr pTransaction)
{
  SBCDefaultBehavior::onProcessResponseOutbound(pResponse, pTransaction);
  
  std::string logId = pResponse->createContextId(true);
  
  std::string strIsLocalReg;
  if (pTransaction->getProperty("is-local-reg", strIsLocalReg) && strIsLocalReg == "yes")
  {
    OSS::Net::IPAddress packetSource = pTransaction->serverTransport()->getRemoteAddress();
    OSS::Net::IPAddress localInterface = pTransaction->serverTransport()->getLocalAddress();
    OSS::mutex_write_lock writeLock(_rwKeepAliveListMutex);
    if (!pTransaction->serverTransport()->isReliableTransport())
    {
      _keepAliveList[packetSource] = localInterface;
    }
    return;
  }
  
  //
  // Extract the REGISTER expiration
  //
  std::string expires = pResponse->hdrGet("expires");
  std::string hContactList = pResponse->hdrGet("contact");
  
  bool is2xx = pResponse->is2xx();
  bool isTrunkReg = pTransaction->hasProperty("is-trunk-reg");
  
  ContactURI curi;
  bool hasContact = false;
  std::string regId;
  
  if (isTrunkReg)
  {
    hasContact = SIPContact::getAt(hContactList, curi, 0);
  }
  else
  {
    hasContact = SBCContact::getContactUriFromRegisterResponse(logId, pTransaction, pResponse, curi, regId);
  }

  if (expires.empty())
  {
    if (hasContact)
    {
      expires = curi.getHeaderParam("expires");
      if (expires.empty())
        expires = "3600";
    }
  }
  else
  {
    pResponse->hdrRemove("expires");
  }
  //
  // Rewrite the contact uri
  //
  ContactURI oldCuri;
  bool hasOldContact = false;
  SIPMessage::Ptr pRequest = pTransaction->serverRequest();
  std::string to;
  to = pRequest->hdrGet("to");
  if (!hContactList.empty() && hasContact)
  {
    std::string oldContactList = pRequest->hdrGet("contact");
    hasOldContact = SIPContact::getAt(oldContactList, oldCuri, 0);
  }
   
  if(!isTrunkReg && is2xx && hasContact && !hasOldContact)
  {
    //
    // Check if this was a QUERY (REGISTER with no contact)
    //
    updateContactsFromStorage(logId, pResponse);
  }
  else if (!isTrunkReg && is2xx && hasContact && hasOldContact)
  {
    OSS_LOG_DEBUG(logId << "Registration update detected for " << oldCuri.data().c_str());
    std::set<std::string> exludeRegId;
    exludeRegId.insert(regId);
    updateContactsFromStorage(logId, pResponse, &exludeRegId);

    oldCuri.setHeaderParam("expires", expires.c_str());
    pResponse->hdrListAppend("Contact", oldCuri.data().c_str());

    try
    { 
      OSS::Net::IPAddress packetSource = pTransaction->serverTransport()->getRemoteAddress();
      OSS::Net::IPAddress localInterface = pTransaction->serverTransport()->getLocalAddress();

      SBCRegistrationRecord registration;
      //
      // Preserve the contact
      //
      registration.contact() = oldCuri.getURI().c_str();
      std::string localReg = oldCuri.getHeaderParam(PropertyMap::propertyString(PropertyMap::PROP_LocalReg));
      if (!localReg.empty())  
      {
        registration.contact().append(";");
        registration.contact().append(PropertyMap::propertyString(PropertyMap::PROP_LocalReg));
        registration.contact().append("=true");
      }
      //
      // Preserve the call-id
      //
      registration.callId() = pResponse->hdrGet(OSS::SIP::HDR_CALL_ID);
      //
      // Preserve the packet source address
      //
      registration.packetSource() =  packetSource.toIpPortString().c_str();
      //
      // Preserve the local interface
      //
      registration.localInterface() = localInterface.toIpPortString().c_str();
      //
      // Preserve the transport ID for connection reuse
      //
      registration.transportId() = OSS::string_from_number<OSS::UInt64>(pTransaction->serverTransport()->getIdentifier()).c_str();
      //
      // Preserver the trasnport scheme for connection reuse
      //
      registration.targetTransport() = pTransaction->serverTransport()->getTransportScheme().c_str();
      //
      // Preserve the to header
      //

      std::string toURI;
      SIPTo::getURI(to, toURI);
      registration.aor() = toURI.c_str();
      
      //
      // Set XOR Property if XOR is enabled
      //
      if (OSS::SIP::SIPXOR::isEnabled() && pTransaction->serverTransaction()->isXOREncrypted())
        registration.enc() = true;
      
      //
      // Set expires
      //
      if (!expires.empty())
        registration.expires() = OSS::string_to_number<int>(expires.c_str());
      else
        registration.expires() = 3600;

      OSS::mutex_write_lock writeLock(_rwKeepAliveListMutex);
      if (!pTransaction->serverTransport()->isReliableTransport())
      {
        _keepAliveList[packetSource] = localInterface;
      }
      
      //
      // Compute the reconnect target
      //

      IPAddress potentialTarget = IPAddress::fromV4IPPort(oldCuri.getHostPort().c_str());
      if (potentialTarget.isValid() && !potentialTarget.isPrivate())
      {
        //
        // The contact is public.  Use it as the target
        //
        pTransaction->serverTransport()->setReconnectAddress(potentialTarget);
      }
      else if (potentialTarget.isValid() && potentialTarget.isPrivate())
      {
        //
        //The contact is private.  Check if old target has the same IP as the contact.
        //
        if (potentialTarget.address() == packetSource.address())
        {
          //
          // The IP is private but the connection is internal and not traversing a NAT.  Use the contact.
          //
          pTransaction->serverTransport()->setReconnectAddress(potentialTarget);
        }
        else
        {
          pTransaction->serverTransport()->setReconnectAddress(packetSource);
        }
      }
      else
      {
        pTransaction->serverTransport()->setReconnectAddress(packetSource);
      }
    
      OSS_LOG_INFO(logId << "Saving persistent REGISTER state - " << curi.data() << " to REG-ID: " << regId << " Call-ID: " << pResponse->hdrGet(OSS::SIP::HDR_CALL_ID));
      if (registration.writeToRedis(*_redisClient, regId))
      {
        //
        // WE preserve the registration towards this user so that we can use it
        // to route by AOR.  This however will break if AOR is shared by multiple bindings
        //

        std::string userId;
        OSS::SIP::SIPURI::getUser(toURI, userId);
        OSS_LOG_DEBUG(logId << "Saving persistent REGISTER state - " << curi.data() << " to USER-ID: " << userId);
        registration.writeToRedis(*_redisClient, userId);

      }
      else
      {
        OSS_LOG_ERROR(logId << "Unable to save REGISTER state - "  << regId);
      }
    }
    catch(...)
    {
      /// We wont be able to recover on this error.
      /// transactions towards this registration will be unroutable
      std::ostringstream logMsg;
      logMsg << "Warning: " << "Unable to save persistent information for registration " << to;
      OSS::log_warning(logMsg.str());
    }
  }
  else if (isTrunkReg && is2xx && hasContact && hasOldContact)
  {
    //
    // Simply remove the contact returned by the trunk and insert the old one
    //
     pResponse->hdrListRemove("contact");
     oldCuri.setHeaderParam("expires", expires.c_str());
     pResponse->hdrListAppend("Contact", oldCuri.data().c_str());
  }
}


void SBCRegisterBehavior::deleteUpperRegistration(const std::string& regId)
{
  if (_redisClient->del(regId))
  {
    OSS_LOG_INFO("Registration " << regId << " DELETED");
  }
}

SIPMessage::Ptr SBCRegisterBehavior::onRouteUpperReg(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target)
{
  SIPRequestLine rline = pRequest->startLine();
  SIPURI ruri;
  if (!rline.getURI(ruri))
  {
   SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_400_BadRequest, "Unable to parse request-uri");
   return serverError;
  }

  std::string regId;
  SBCContact::getRegistrationIdentifier(ruri, regId);

  

  try
  {
    SBCRegistrationRecord registration;


    if (!registration.readFromRedis(*_redisClient, regId))
    {
      OSS_LOG_ERROR("Unable to load " << regId << " from Redis database.");
      SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_404_NotFound);
      return serverError;
    }

    pTransaction->setProperty("upper-reg-tran", "1");
    
    std::string contact;
    contact = registration.contact();
    if (contact.find(PropertyMap::propertyString(PropertyMap::PROP_LocalReg)) != std::string::npos)
    {
      pTransaction->setProperty(PropertyMap::PROP_LocalReg, "true");
      return OSS::SIP::SIPMessage::Ptr();
    }
    
    
    SIPTo to(registration.aor());
    localInterface = IPAddress::fromV4IPPort(registration.localInterface().c_str());
    target = IPAddress::fromV4IPPort(registration.packetSource().c_str());

    
    SIPFrom contactHeader = contact;
    IPAddress potentialTarget = IPAddress::fromV4IPPort(contactHeader.getHostPort().c_str());
    if (potentialTarget.isValid() && !potentialTarget.isPrivate())
    {
      //
      // The contact is public.  Use it as the target
      //
      target = potentialTarget;
    }
    else if (potentialTarget.isValid() && potentialTarget.isPrivate())
    {
      //
      //The contact is private.  Check if old target has the same IP as the contact.
      //
      if (potentialTarget.address() == target.address())
      {
        //
        // The IP is private but the connection is internal and not traversing a NAT.  Use the contact.
        //
        target = potentialTarget;
      }
    }

    //
    // Check if the contact is non-UDP.  We will try to recycle a persistent connection for TCP
    //
    std::string transportScheme = registration.targetTransport();
    OSS::string_to_upper(transportScheme);


    pRequest->setProperty(OSS::PropertyMap::PROP_TargetTransport, transportScheme.c_str());

    std::string transportId = registration.transportId();
    if (!transportId.empty())
        pRequest->setProperty(OSS::PropertyMap::PROP_TransportId, transportId.c_str());

    //
    // Check if this call is to be encrypted
    //
    if (registration.enc())
      pRequest->setProperty("xor", "1");
   

    std::ostringstream requestLine;
    requestLine << pRequest->getMethod() << " " << contact << " SIP/2.0";
    pRequest->setStartLine(requestLine.str().c_str());
    pRequest->hdrSet("to", to.getURI().c_str());
    //
    // Proxy media for upper reg
    //
    if (_pManager->requireRtpForRegistrations())
      pTransaction->setProperty("require-rtp-proxy", "1");
  }
  catch(OSS::Exception e)
  {
    OSS_LOG_ERROR("Unable to load " << regId << " Error=" << e.what());
    SIPMessage::Ptr serverError = pRequest->createResponse(SIPMessage::CODE_404_NotFound);
    return serverError;
  }

  //
  // always make sure we have a persistent connection for registered endpoints
  //
  pRequest->setProperty(OSS::PropertyMap::PROP_RequirePersistentConnection, "yes");
  pTransaction->setProperty("Registration-Id", regId);
  
  return OSS::SIP::SIPMessage::Ptr();
}

bool SBCRegisterBehavior::onRouteByAOR(
  SIPMessage* pRequest,
  SIPB2BTransaction* pTransaction,
  bool userComparisonOnly,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  SIPRequestLine rline = pRequest->startLine();
  SIPURI ruri;
  if (!rline.getURI(ruri))
  {
    return false;
  }

  std::string ruriUser =  ruri.getUser();
  std::string domain = ruri.getHost();
  

  try
  {
    OSS_LOG_INFO("SBCRegisterBehavior::onRouteByAOR - Looking up user-id " << ruriUser);

    SBCRegistrationRecord registration;

    if (!registration.readFromRedis(*_redisClient, ruriUser))
    {
      OSS_LOG_ERROR("SBCRegisterBehavior::onRouteByAOR - Unable to locate user-id " << ruriUser);
    }

    SIPTo to(registration.aor());
    if (to.getUser() == ruriUser)
    {
      if (!userComparisonOnly && to.getHost() != domain)
      {
        OSS_LOG_ERROR("SBCRegisterBehavior::onRouteByAOR - domain mismatch for user-id " << ruriUser);
        return false;
      }



      localInterface = IPAddress::fromV4IPPort(registration.localInterface().c_str());
      target = IPAddress::fromV4IPPort(registration.packetSource().c_str());

      OSS_LOG_INFO("SBCRegisterBehavior::onRouteByAOR - " << ruriUser << "->" << registration.packetSource().c_str());

      SIPFrom contact;
      contact = registration.contact();
      IPAddress potentialTarget = IPAddress::fromV4IPPort(contact.getHostPort().c_str());
      if (potentialTarget.isValid() && !potentialTarget.isPrivate())
        target = potentialTarget;

      //
      // Check if this call is to be encrypted
      //
      if (registration.enc())
        pRequest->setProperty("xor", "1");

      std::ostringstream requestLine;
      requestLine << pRequest->getMethod() << " " << contact.getURI() << " SIP/2.0";
      pRequest->setStartLine(requestLine.str().c_str());
      pRequest->hdrSet("to", to.getURI().c_str());
      //
      // Proxy media for upper reg
      //
      if (_pManager->requireRtpForRegistrations())
        pTransaction->setProperty("require-rtp-proxy", "1");

      pRequest->setProperty("interface-address", localInterface.toString());
      pRequest->setProperty("interface-port", OSS::string_from_number<unsigned short>(localInterface.getPort()));
      pRequest->setProperty("target-address", target.toString());
      pRequest->setProperty("target-port", OSS::string_from_number<unsigned short>(target.getPort()));

      //
      // always make sure we have a persistent connection for registered endpoints
      //
      pRequest->setProperty(OSS::PropertyMap::PROP_RequirePersistentConnection, "yes");
      return true;
    }
  }
  catch(...)
  {

  }

  return false;
  
}

SIPMessage::Ptr SBCRegisterBehavior::onProcessRequestBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
  /// This method allows the application to
  /// process the body of the request
  /// before it gets sent out.
  ///
  /// This method allows the upper layer to modify the body
  /// based on specific application requirements.  For
  /// example, a media proxy may modify SDP address and port
  /// so that RTP passes through the application.
  ///
  /// If the return value is an error response, the transaction
  /// will send it automatically to the sender.
  ///
  /// If the body is supported, the return value must be a null-Ptr.
{
  return OSS::SIP::SIPMessage::Ptr();
}

void SBCRegisterBehavior::onProcessResponseBody(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
  /// This method allows the application to
  /// process the body of the response
  /// before it gets sent out.
  ///
  /// This method allows the upper layer to modify the body
  /// based on specific application requirements.  For
  /// example, a media proxy may modify SDP address and port
  /// so that RTP passes through the application.
  ///
{
}

void SBCRegisterBehavior::onProcessOutbound(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
  /// This is the last chance for the application to process
  /// the outbound request before it gets sent out to the transport.
  ///
  /// This is normally the place where application would want to
  /// insert application-specific headers as well as change existing
  /// headers to the desired application-specific values for as long
  /// as it wont conflict with dialog creation states.
{
}



void SBCRegisterBehavior::onTransactionError(
  OSS::SIP::SIPTransaction::Error e,
  SIPMessage::Ptr pErrorResponse,
  SIPB2BTransaction::Ptr pTransaction)
  /// Signals that an error occured on the transaction
  ///
  /// The transaction will be destroyed automatically after this function call
{
}

void SBCRegisterBehavior::addGateway(const std::string& name, Gateway& gateway)
{
  OSS::mutex_critic_sec_lock lock(_gatewaysMutex);
  if (gateway.name.empty())
  {
    gateway.name = name;
  }
  
  OSS_LOG_INFO("SBCRegisterBehavior::addGateway - adding gateway " << gateway.name << " to keep-alive list");
  
  if (gateway.frequencyInSeconds < 45)
  {
    OSS_LOG_WARNING("SBCRegisterBehavior::addGateway - Increasing frequency from " << gateway.frequencyInSeconds << " to 45 seconds");
    gateway.frequencyInSeconds = 45;
  }
  _gateways[name] = gateway;
}
  
SBCRegisterBehavior::GatewayStatus SBCRegisterBehavior::getGatewayStatus(const std::string& name) const
{
  OSS::mutex_critic_sec_lock lock(_gatewaysMutex);
  GatewayList::const_iterator iter = _gateways.find(name);
  if (iter == _gateways.end())
  {
    return GatewayUnknownStatus;
  }
  return iter->second.status;
}

void SBCRegisterBehavior::setGatewayStatus(const std::string& name, GatewayStatus status)
{
  OSS::mutex_critic_sec_lock lock(_gatewaysMutex);
  GatewayList::iterator iter = _gateways.find(name);
  if (iter != _gateways.end())
  {
    iter->second.status = status;
    
    std::string statusString = "UNKNOWN";
    if (status == GatewayOnline)
    {
      statusString = "ONLINE";
    }
    else if (status == GatewayOffline)
    {
      statusString = "OFFLINE";
    }
    OSS_LOG_INFO("SBCRegisterBehavior::setGatewayStatus - Changing gateway status for " << name << " to " << statusString);
  }
}

bool SBCRegisterBehavior::cliGetGatewayStatus(const SBCConsole::CommandTokens& data, std::string& result)
{
  if (SBCConsole::isCommand("gateway status", data))
  {
    result = "gateway status<-->";
    
    for (std::size_t i = 2; i < data.size(); i++)
    {
      result += data[i] + "<-->";
    }
    
    return true;
  }
  return false;
}
    /// CLI request for all gateway status
  
void SBCRegisterBehavior::sendKeepAliveToGateways()
{
  OSS::mutex_critic_sec_lock lock(_gatewaysMutex);
  for (GatewayList::iterator iter = _gateways.begin(); iter != _gateways.end(); iter++)
  {
    sendGatewayKeepAlive(iter->second);
  }
}

void SBCRegisterBehavior::sendGatewayKeepAlive(Gateway& gateway)
{
  static int cseqNo = 1;
  gateway.nextSendCounter -= DEFAULT_KEEP_ALIVE_FREQUENCY_IN_SECONDS;
  
  if (gateway.nextSendCounter > 0)
  {
    return;
  }
  gateway.nextSendCounter = gateway.frequencyInSeconds;

  std::string callId = OSS::string_create_uuid();
  size_t hash = OSS::string_hash(callId.c_str());
    
  OSS::Net::IPAddress target(gateway.targetAdddress);
  target.setPort(gateway.targetPort);

  OSS::Net::IPAddress src(_pManager->getDefaultTransportAddress());
  
  if(!gateway.localInterfaceAddress.empty())
  {
    src = OSS::Net::IPAddress(gateway.localInterfaceAddress);
    src.setPort(gateway.localInterfacePort);
  }
    
  //
  // Request URI
  //
  std::ostringstream requestUri;
  requestUri << gateway.host;
  if (gateway.port)
  {
    requestUri << ":" << gateway.port;
  }
  
  //
  // Via and contact
  //
  std::string transportScheme = gateway.transport;
  OSS::string_to_upper(transportScheme);
  std::string localInterface;
  localInterface = src.toIpPortString();

  try
  {
    std::ostringstream options;
    options << "OPTIONS sip:" << requestUri.str() << " SIP/2.0" << OSS::SIP::CRLF;
    options << "From: sip:anonymous@anonymous.invalid"  << ";tag=" << hash << OSS::SIP::CRLF;
    options << "To: sip:" << gateway.domain << OSS::SIP::CRLF;
    options << "Via: " << "SIP/2.0/" << transportScheme << " " << localInterface << ";branch=z9hG4bK" << hash << ";rport" << OSS::SIP::CRLF;
    options << "Call-ID: " << callId << OSS::SIP::CRLF;
    options << "CSeq: " << cseqNo++ << " OPTIONS" << OSS::SIP::CRLF;
    options << "Contact: " << "<sip:" << localInterface << ">" << OSS::SIP::CRLF;

    if (!gateway.outboundProxy.empty())
    {
      options << "Route: sip:" << gateway.outboundProxy;
      if (gateway.outboundProxyPort)
      {
        options << ":" << gateway.outboundProxyPort;
      }
      options << ";lr" << OSS::SIP::CRLF;
    }

    options << "User-Agent: " << _pManager->getUserAgentName() << OSS::SIP::CRLF;
    options << "Subject: Keep-Alive" << OSS::SIP::CRLF;
    options << "Content-Length: 0" << OSS::SIP::CRLF << OSS::SIP::CRLF;
    
    SIPMessage::Ptr msg(new SIPMessage(options.str()));

    msg->setProperty(OSS::PropertyMap::PROP_TargetTransport, transportScheme.c_str());
    msg->setProperty("gateway-name", gateway.name);
    _pManager->transactionManager().stack().sendRequest(msg, src, target, _keepAliveResponseCb, OSS::SIP::SIPTransaction::TerminateCallback());
  }
  catch(OSS::Exception e)
  {
    std::ostringstream logMsg;
    logMsg << "SBCRegisterBehavior::sendOptionsKeepAlive Failure - "
      << e.message();
    OSS::log_warning(logMsg.str());
  }

}

} } } // OSS::SIP::SBC


