
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


#include <exception>
#include <fstream>
#include <sys/inotify.h>

#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/UTL/Logger.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/UTL/Application.h"
#include "OSS/UTL/Cache.h"
#include "OSS/UTL/StackTrace.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/SIP/SBC/SBCDefaultBehavior.h"
#include "OSS/SIP/SBC/SBCByeBehavior.h"
#include "OSS/SIP/SBC/SBCCancelBehavior.h"
#include "OSS/SIP/SBC/SBCExecBehavior.h"
#include "OSS/SIP/SBC/SBCInfoBehavior.h"
#include "OSS/SIP/SBC/SBCInviteBehavior.h"
#include "OSS/SIP/SBC/SBCMessageBehavior.h"
#include "OSS/SIP/SBC/SBCNotifyBehavior.h"
#include "OSS/SIP/SBC/SBCOptionsBehavior.h"
#include "OSS/SIP/SBC/SBCPrackBehavior.h"
#include "OSS/SIP/SBC/SBCPublishBehavior.h"
#include "OSS/SIP/SBC/SBCPublishBehavior.h"
#include "OSS/SIP/SBC/SBCReferBehavior.h"
#include "OSS/SIP/SBC/SBCRegisterBehavior.h"
#include "OSS/SIP/SBC/SBCSubscribeBehavior.h"
#include "OSS/SIP/SBC/SBCSDPBehavior.h"
#include "OSS/SIP/SBC/SBCUpdateBehavior.h"


using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;
using OSS::Persistent::PersistenceException;


namespace OSS {
namespace SIP {
namespace SBC {


SBCManager::SBCManager() :
  _transactionManager(2, 8120),
  _rtpProxy(this),
  _sipConfigFile("sip.cfg"),
  _dialogs(3600*24),
  _dialogStateManager(this),
  _enableStunServer(true),
  _pReferHandler(0),
  _pRegisterHandler(0),
  _pInviteHandler(0),
  _pPrackHandler(0),
  _plugins(this),
  _enableOptionsRouting(false),
  _enableOptionsKeepAlive(true),
  _staticRouter(this),
  _requireRtpForRegistrations(true),
  _delayedDisconnectMinConnectTime(-1),
  _delayedDisconnectYieldTime(-1),
  _autoBanRules(this),
  _console(),
  _jsModules(*SBCJSModuleManager::createInstance(this)),
  _jsonRpcHandler(this),
  _maxInvitesPerSecond(0),
  _maxRegistersPerSecond(0),
  _maxSubscribesPerSecond(0)
{
  _transactionManager.setExternalDispatch(boost::bind(&SBCManager::onDispatchTransaction, this, _1, _2));
}

SBCManager::~SBCManager()
{
  _workspace.stop();
}

void SBCManager::initializePaths(const boost::filesystem::path& cfgDirectory)
{

  _pathsConfigurationFile = operator/(cfgDirectory, "paths.cfg");
  OSS_LOG_INFO("Loading PATHS configuration from " << _pathsConfigurationFile);
  ClassType pathCfg;
  if (!pathCfg.load(_pathsConfigurationFile))
  {
    reportCriticalState("Unable to load paths.cfg");
    return;
  }
  _dialogStateDirectory = static_cast<const char*>(pathCfg["paths"]["dialog-state-dir"]);
  _rtpStateDirectory = static_cast<const char*>(pathCfg["paths"]["rtp-state-dir"]);
  _registrationRecordsDirectory = static_cast<const char*>(pathCfg["paths"]["registration-records-dir"]);
  boost::filesystem::path logDirectory = OSS::logger_get_path().parent_path();

  try
  {
    boost::filesystem::create_directories(_dialogStateDirectory);
    boost::filesystem::create_directories(_rtpStateDirectory);
    boost::filesystem::create_directories(_registrationRecordsDirectory);
  }catch(...){}

  OSS_LOG_INFO("PATHS configuration loaded - " << _pathsConfigurationFile);
}

void SBCManager::initializePlugins()
{
  _plugins.loadPlugins(_pathsConfigurationFile, _sipConfigurationFile);
}

void SBCManager::initializeHandlers()
{
  SBCDefaultBehavior* pSBCHandler = 0;
  
  pSBCHandler = new SBCByeBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = new SBCCancelBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));

  pSBCHandler = new SBCExecBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = new SBCDefaultBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = new SBCInfoBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = _pInviteHandler = new SBCInviteBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = new SBCMessageBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = new SBCNotifyBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = new SBCOptionsBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = _pPrackHandler = new SBCPrackBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = new SBCPublishBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = _pReferHandler = new SBCReferBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = _pRegisterHandler = new SBCRegisterBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));

  pSBCHandler = new SBCSubscribeBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = new SBCUpdateBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));
  
  pSBCHandler = new SBCSDPBehavior(this);
  _transactionManager.registerHandler(OSS::SIP::B2BUA::SIPB2BHandler::Ptr(pSBCHandler));


  SIPB2BTransactionManager::MessageHandlers::iterator cancelIter = _transactionManager.handlers().find(SIPB2BHandler::TYPE_CANCEL);
  OSS_VERIFY(cancelIter != _transactionManager.handlers().end());
  SIPB2BTransactionManager::MessageHandlers::iterator inviteIter = _transactionManager.handlers().find(SIPB2BHandler::TYPE_INVITE);
  OSS_VERIFY(inviteIter != _transactionManager.handlers().end());
  dynamic_cast<SBCInviteBehavior*>(inviteIter->second.get())->_pCancelBehavior = dynamic_cast<SBCCancelBehavior*>(cancelIter->second.get());

  SIPB2BTransactionManager::MessageHandlers::iterator subscribeIter = _transactionManager.handlers().find(SIPB2BHandler::TYPE_SUBSCRIBE);
  OSS_VERIFY(subscribeIter != _transactionManager.handlers().end());
  SIPB2BTransactionManager::MessageHandlers::iterator notifyIter = _transactionManager.handlers().find(SIPB2BHandler::TYPE_NOTIFY);
  OSS_VERIFY(notifyIter != _transactionManager.handlers().end());
  dynamic_cast<SBCNotifyBehavior*>(notifyIter->second.get())->_pSubscribeBehavior = dynamic_cast<SBCSubscribeBehavior*>(subscribeIter->second.get());
}

void SBCManager::initializeRTPProxy(const boost::filesystem::path& cfgDirectory)
{
}

void SBCManager::initializeUserAgent(const boost::filesystem::path& cfgDirectory)
{
  ClassType config;
  boost::filesystem::path configFile = operator/(cfgDirectory, _sipConfigFile);
  if (!config.load(configFile))
  {
    reportCriticalState("Unable to load sip.cfg");
    return;
  }
  
  DataType root = config.self();
  if (!root.exists("user-agent"))
  {
    reportCriticalState("Unable to load sip.cfg.  User-Agent section missing.");
    return;
  }
  
  DataType userAgent = root["user-agent"];

    
  if (userAgent.exists("user-agent-name"))
  {
    _userAgentName = (const char*)userAgent["user-agent-name"];
  }

  if (userAgent.exists("enable-options-routing"))
  {
    _enableOptionsRouting = (bool)userAgent["enable-options-routing"];
  }

  if (userAgent.exists("disable-options-keep-alive"))
  {
    _enableOptionsKeepAlive = !(bool)userAgent["disable-options-keep-alive"];
  }

  if (userAgent.exists("require-rtp-for-registrations"))
  {
    _requireRtpForRegistrations = (bool)userAgent["require-rtp-for-registrations"];
  }
  
  if (userAgent.exists("max-registers-per-second"))
  {
    _maxRegistersPerSecond = (int)userAgent["max-registers-per-second"];
  }
  
  if (userAgent.exists("max-invites-per-second"))
  {
    _maxInvitesPerSecond = (int)userAgent["max-invites-per-second"];
  }
  
  if (userAgent.exists("max-subscribes-per-second"))
  {
    _maxSubscribesPerSecond = (int)userAgent["max-subscribes-per-second"];
  }

  if (userAgent.exists("delayed-disconnect-min-connect-time"))
  {
    _delayedDisconnectMinConnectTime = (int)userAgent["delayed-disconnect-min-connect-time"];
  }

  if (userAgent.exists("delayed-disconnect-yield-time"))
  {
    _delayedDisconnectYieldTime = (int)userAgent["delayed-disconnect-yield-time"];
    if (_delayedDisconnectYieldTime >= 30 || _delayedDisconnectYieldTime < 1) 
    {
      OSS_LOG_WARNING("SBCManager::initializeUserAgent - invalid value for \"delayed-disconnect-yield-time\".");
      _delayedDisconnectYieldTime = -1;
    }
    else
    {
      OSS_LOG_INFO("SBCManager::initializeUserAgent - Disconnect yield time set to " << _delayedDisconnectYieldTime << " secs");
    }
  }


  SBCContact::initialize(configFile);

  _workspace.initialize();
  
  _authenticator.accounts().initialize(_workspace.getAccountDb());
  
  _staticRouter.loadStaticRoutes();
  
  //
  // WARNING: Make sure to add the endpoint first prior to running it or the _dispatch 
  // will be null and cause an assertion
  //
  
  //
  // Initialize the registrar endpoint
  //
  _registrar.attachSBCManager(this);
  _registrar.setDatabase(_workspace.getLocalRegDb());
  transactionManager().stack().transport().addEndpoint(&_registrar);
  _registrar.setAddress("127.0.0.1");
  _registrar.setPort("0");
  _registrar.run();
  
  _cdr.initialize(this);
  
  _jsonRpcHandler.listen();
}

void SBCManager::onLocalRegistrationStopped()
{
}

void SBCManager::initializeSTUNServer(const boost::filesystem::path& cfgDirectory)
{
  if (!_enableStunServer)
    return;

  ClassType config;
  boost::filesystem::path configFile = operator/(cfgDirectory, _sipConfigFile);
  if (!config.load(configFile))
  {
    OSS_LOG_ERROR("Unable to load " << _sipConfigFile << " while attempting to initialize STUN server.");
    reportCriticalState("Unable to load sip.cfg");
    return;
  }
  DataType root = config.self();
  DataType listeners = root["listeners"];

  //stun-primary-ip = "192.168.1.10:3478";
  //stun-secondary-ip = "192.168.1.11:3479";
  if (!listeners.exists("stun-primary-ip") || !listeners.exists("stun-secondary-ip"))
  {
    _enableStunServer = false;
    return;
  }

  std::string primary = listeners["stun-primary-ip"];
  std::string secondary = listeners["stun-secondary-ip"];

  if (primary.empty() || secondary.empty())
  {
    _enableStunServer = false;
    return;
  }

  OSS::Net::IPAddress primaryAddress = OSS::Net::IPAddress::fromV4IPPort(primary.c_str());
  OSS::Net::IPAddress secondaryAddress = OSS::Net::IPAddress::fromV4IPPort(secondary.c_str());

  if (primaryAddress.compare(secondaryAddress, true) || primaryAddress.compare(secondaryAddress, false))
  {
    _enableStunServer = false;
    return;
  }

  _stunServer.initialize(primaryAddress, secondaryAddress);
}

 bool SBCManager::initialize()
 {
   boost::filesystem::path configDir(SBCDirectories::instance()->getConfigDirectory().c_str());
   if (!boost::filesystem::exists(configDir))
   {
     return false;
   }
   
   try
   {
     initialize(configDir);
     return true;
   }
   catch(...)
   {
     return false;
   }
 }
void SBCManager::initialize(const boost::filesystem::path& cfgDirectory)
{
  _configurationDirectory = boost::filesystem::system_complete(cfgDirectory);
  OSS_LOG_INFO("Initializing SBC Manager using config data from " << _configurationDirectory);

  _sipConfigurationFile = operator/(_configurationDirectory, _sipConfigFile);

  //
  // Intialize the paths configuration
  //
  try
  {
    initializePaths(_configurationDirectory);
  }
  catch(...)
  {
    OSS::log_fatal("Unable to initialize path configuration.");
    throw SBCConfigException("Unable to initialize path configuration.");
  }

  
  //
  // Initialize RTPProxy
  //
  OSS_LOG_INFO("Initializing RTP Proxy ...");
  try
  {
    initializeRTPProxy(_configurationDirectory);
  }
  catch(const std::exception& e)
  {
    OSS_LOG_FATAL("Unable to initialize RTP Proxy configuration. Exception: " << e.what());
    throw SBCConfigException("Unable to initialize RTP Proxy configuration.");
  }
  catch(...)
  {
    OSS::log_fatal("Unable to initialize RTP Proxy configuration.");
    throw SBCConfigException("Unable to initialize RTP Proxy configuration.");
  }
  OSS_LOG_INFO("RTP Proxy initialized ...");
  //
  // Initialize the stun
  //
  OSS_LOG_INFO("Initializing STUN Server ...");
  try
  {
    initializeSTUNServer(_configurationDirectory);
  }
  catch(...)
  {
    OSS::log_fatal("Unable to initialize STUN Server configuration.");
  }
  OSS_LOG_INFO("STUN Server intialized.");

  //
  // Initialize User agent
  //
  OSS_LOG_INFO("Intializing User Agent ...");
  try
  {
    initializeUserAgent(_configurationDirectory);
  }
  catch(...)
  {
    OSS::log_fatal("Unable to initialize User Agent configuration.");
    reportCriticalState("Unable to initialize User Agent configuration.");
    throw;
  }
  OSS_LOG_INFO("User Agent initialized.");
  //
  // Initialize the transaction manager
  //
  OSS_LOG_INFO("Initilizing Transaction Manager ...");
  try
  {
    _transactionManager.setSipConfigFile(_sipConfigFile);
    _transactionManager.initialize(_configurationDirectory);
  }
  catch(std::exception& e)
  {
    OSS_LOG_FATAL("Unable to initialize transaction manager. - " << e.what());
    reportCriticalState("Unable to initialize transaction manager.");
    throw;
  }
  catch(...)
  {
    OSS_LOG_FATAL("Unable to initialize transaction manager. - Unknown exception.");
    reportCriticalState("Unable to initialize transaction manager.");
    throw;
  }

  //
  // Initialize handlers
  //
  try
  {
    initializeHandlers();
  }
  catch(std::exception& e)
  {
    std::ostringstream error;
    error << "Unable to initialize handlers. - " << e.what();
    OSS::log_fatal(error.str().c_str());
    reportCriticalState(error.str().c_str());
    throw SBCConfigException(error.str().c_str());
  }
  catch(...)
  {
    std::ostringstream error;
    error << "Unable to initialize handlers. - Unknown exception.";
    OSS::log_fatal(error.str().c_str());
    reportCriticalState("Unknown exception.");
    throw SBCConfigException("Unknown exception.");
  }

  try
  {
    initializePlugins();
  }
  catch(...)
  {
    OSS::log_fatal("Error encountered while intializing plugins");
  }

  //
  // Initialize the auxiliary socket for CLI operations
  //
  _console.run();
  _console.publishEvent("Karoo Bridge initialized");
  
  OSS_LOG_INFO("Transaction Manager initialized.");
}

void SBCManager::deinitialize()
{
  _transactionManager.deinitialize();
}

bool SBCManager::run()
{
  try
  {
    _transactionManager.stack().run();
    _dialogStateManager.run(_dialogStateDirectory);

    OSS_LOG_INFO("Starting STUN Server ...");
    if (_enableStunServer)
    {
      _stunServer.run();
      OSS_LOG_INFO("STUN Server STARTED");
    }else
    {
      OSS_LOG_INFO("STUN Server is DISABLED");
    }

    _pRegisterHandler->startOptionsThread();
  
  }
  catch(...)
  {
    reportCriticalState("Unable to run SBCManager");
    return false;
  }
  return true;
}

void SBCManager::stop()
{
  if (_enableStunServer)
  {
    std::cout << "Stopping STUN Server ..." << std::endl;
    _stunServer.stop();
    std::cout << "STUN Server STOPPED ..." << std::endl;
  }


  std::cout << "Stopping Dialog State Manager ..." << std::endl;
  _dialogStateManager.stop();
  std::cout << "Dialog State Manager STOPPED ..." << std::endl;

  std::cout << "Stopping SIP Stack ..." << std::endl;
  _transactionManager.deinitialize();
  _transactionManager.stack().stop();
  std::cout << "SIP Stack Stopped..." << std::endl;
  
}

void SBCManager::registerExecProc(const char* cmd, const ExecProc& proc)
{
   _execProcs[cmd] = proc;
}

bool SBCManager::execProc(const std::string& cmd, const std::string& args, std::ostringstream& output)
{
  ExecProcList::iterator procIter = _execProcs.find(cmd);
  if (procIter == _execProcs.end())
    return false;
  return procIter->second(args, output);
}

bool SBCManager::onRouteByAOR(
  SIPMessage* pRequest,
  SIPB2BTransaction* pTransaction,
  bool userComparisonOnly,
  OSS::Net::IPAddress& localInterface,
  OSS::Net::IPAddress& target)
{
  SIPB2BTransactionManager::MessageHandlers::iterator handler = transactionManager().handlers().find(OSS::SIP::B2BUA::SIPB2BHandler::TYPE_REGISTER);
  if (handler != transactionManager().handlers().end())
  {
    OSS::SIP::B2BUA::SIPB2BHandler::Ptr pRegHandlerPtr = handler->second;
    if (pRegHandlerPtr)
    {
      SBCRegisterBehavior* pRegHandler = dynamic_cast<SBCRegisterBehavior*>(pRegHandlerPtr.get());
      return pRegHandler->onRouteByAOR(pRequest, pTransaction, userComparisonOnly, localInterface, target);
    }
  }
  return false;
}


bool SBCManager::onRouteByRURI(SIPMessage* pRequest, SIPB2BTransaction* pTransaction)
{
  return false;
}

bool SBCManager::isLocalTransport(const std::string& transport, const OSS::Net::IPAddress& address)
{
  OSS::mutex_lock lockTransport(_transactionManager.stack().transportMutex());
  
  if (transport == "udp")
  {
    OSS::socket_address_list& listeners = _transactionManager.stack().udpListeners();
    for (OSS::socket_address_list::iterator iter = listeners.begin(); iter != listeners.end(); iter++)
    {
      if (*iter == address)
        return true;
    }
  }
  else if (transport == "tcp")
  {
    OSS::socket_address_list& listeners = _transactionManager.stack().tcpListeners();
    for (OSS::socket_address_list::iterator iter = listeners.begin(); iter != listeners.end(); iter++)
    {
      if (*iter == address)
        return true;
    }
  }
  else if (transport == "tls")
  {
    OSS::socket_address_list& listeners = _transactionManager.stack().tlsListeners();
    for (OSS::socket_address_list::iterator iter = listeners.begin(); iter != listeners.end(); iter++)
    {
      if (*iter == address)
        return true;
    }
  }

  return false;
}

bool SBCManager::isLocalInterface(const std::string& transport, const OSS::Net::IPAddress& address)
{
  OSS::mutex_lock lockTransport(_transactionManager.stack().transportMutex());
  
  if (transport == "udp")
  {
    OSS::socket_address_list& listeners = _transactionManager.stack().udpListeners();
    for (OSS::socket_address_list::iterator iter = listeners.begin(); iter != listeners.end(); iter++)
    {
      if (iter->compare(address, false))
        return true;
    }
  }
  else if (transport == "tcp")
  {
    OSS::socket_address_list& listeners = _transactionManager.stack().tcpListeners();
    for (OSS::socket_address_list::iterator iter = listeners.begin(); iter != listeners.end(); iter++)
    {
      if (iter->compare(address, false))
        return true;
    }
  }
  else if (transport == "tls")
  {
    OSS::socket_address_list& listeners = _transactionManager.stack().tcpListeners();
    for (OSS::socket_address_list::iterator iter = listeners.begin(); iter != listeners.end(); iter++)
    {
      if (iter->compare(address, false))
        return true;
    }
  }

  return false;
}

bool SBCManager::getFirstTransport(const std::string& proto, const std::string& iface, OSS::Net::IPAddress& transport)
{
  OSS::mutex_lock lockTransport(_transactionManager.stack().transportMutex());
  
  if (proto == "udp")
  {
    OSS::socket_address_list& listeners = _transactionManager.stack().udpListeners();
    for (OSS::socket_address_list::iterator iter = listeners.begin(); iter != listeners.end(); iter++)
    {
      if (iter->toString() == iface)
      {
        transport = *iter;
        return true;
      }
    }
  }
  else if (proto == "tcp")
  {
    OSS::socket_address_list& listeners = _transactionManager.stack().tcpListeners();
    for (OSS::socket_address_list::iterator iter = listeners.begin(); iter != listeners.end(); iter++)
    {
      if (iter->toString() == iface)
      {
        transport = *iter;
        return true;
      }
    }
  }
  else if (proto == "tls")
  {
    OSS::socket_address_list& listeners = _transactionManager.stack().tlsListeners();
    for (OSS::socket_address_list::iterator iter = listeners.begin(); iter != listeners.end(); iter++)
    {
      if (iter->toString() == iface)
      {
        transport = *iter;
        return true;
      }
    }
  }

  return false;
}


bool SBCManager::onRetargetTransaction(
  SIPMessage::Ptr& pRequest,
  SIPB2BTransaction::Ptr pTransaction)
{
  return _pReferHandler->onRetargetTransaction(pRequest, pTransaction);
}


void SBCManager::onPostRouteTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction)
{
}


void SBCManager::onPostRouteMidDialogTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction)
{
}


void SBCManager::reportCriticalState(const std::string& msg)
{
}

void SBCManager::onDispatchTransaction(SIPB2BTransactionManager* pManager, SIPB2BTransaction* pTransaction)
{  
  if (_delayedDisconnectMinConnectTime != -1 && _delayedDisconnectYieldTime != -1 && pTransaction->serverRequest()->isRequest("BYE"))
  {
    OSS::UInt64 duration = getCallDuration(pTransaction->serverRequest()->hdrGet(OSS::SIP::HDR_CALL_ID));
    if (duration < (unsigned)(_delayedDisconnectMinConnectTime*1000))
    {
      OSS_LOG_NOTICE(pTransaction->serverRequest()->createContextId(true) << "SBCManager::onDispatchTransaction - delaying disconnect dispatch for this call by " << _delayedDisconnectYieldTime << " seconds");
      pManager->threadPool().schedule(boost::bind(&SIPB2BTransaction::runTask, pTransaction), _delayedDisconnectYieldTime*1000);
    }
    else
    {
      if (pManager->threadPool().schedule(boost::bind(&SIPB2BTransaction::runTask, pTransaction)) == -1)
      {
        pTransaction->runTask();
      }
    }
  }
  else
  {
    if (pManager->threadPool().schedule(boost::bind(&SIPB2BTransaction::runTask, pTransaction)) == -1)
    {
      pTransaction->runTask();
    }
  }
}

void SBCManager::markCallStartTime(const std::string& callId)
{
  OSS::mutex_critic_sec_lock lock(_callStartTimeMutex);
  OSS::UInt64 now = OSS::getTime();
  
  if (_callStartTime.find(callId) == _callStartTime.end())
  {
    _callStartTime[callId] = now;
  }
}
  
OSS::UInt64 SBCManager::getCallDuration(const std::string& callId) const
{
  OSS::mutex_critic_sec_lock lock(_callStartTimeMutex);
  
  CallTimers::const_iterator iter = _callStartTime.find(callId);
  if (iter == _callStartTime.end())
  {
    return 0;
  }
  
  OSS::UInt64 now = OSS::getTime();
  return now - iter->second;
}

void SBCManager::removeCallStartTime(const std::string& callId)
{
  OSS::mutex_critic_sec_lock lock(_callStartTimeMutex);
  _callStartTime.erase(callId);
}

bool SBCManager::transformPreDialogPersist(const SIPMessage::Ptr& pRequest, const SIPB2BTransaction::Ptr& pTransaction)
{
  return _pReferHandler->transformPreDialogPersist(pRequest, pTransaction);
}


} } } /// OSS::SIP::SBC


