
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
#include "OSS/SIP/SBC/SBCConfiguration.h"


using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;
using OSS::Persistent::PersistenceException;


namespace OSS {
namespace SIP {
namespace SBC {

SBCManager* SBCManager::_instance = 0;

SBCManager* SBCManager::instance()
{
  if (!SBCManager::_instance)
  {
    SBCManager::_instance = new SBCManager();
  }
  return SBCManager::_instance;
}

void SBCManager::deleteInstance()
{
  delete SBCManager::_instance;
  SBCManager::_instance = 0;
}


SBCManager::SBCManager() :
  _transactionManager(2, 8120),
  _rtpProxy(this),
  _sipConfigFile("sip.cfg"),
  _dialogs(3600*24),
  _dialogStateManager(this),
  _pReferHandler(0),
  _pRegisterHandler(0),
  _pInviteHandler(0),
  _pPrackHandler(0),
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
  _maxSubscribesPerSecond(0),
	_thread(0)
{
  _transactionManager.setExternalDispatch(boost::bind(&SBCManager::onDispatchTransaction, this, _1, _2));
}

SBCManager::~SBCManager()
{
  _workspace.stop();
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

void SBCManager::initializeUserAgent()
{
  if (!SBCConfiguration::instance()->initUserAgent())
  {
    reportCriticalState("Unable to reteive user agent configuraiton");
    return;
  }

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


void SBCManager::initialize()
{
  
  //
  // Initialize User agent
  //
  OSS_LOG_INFO("Intializing User Agent ...");
  try
  {
    initializeUserAgent();
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
	  SBCConfiguration::instance()->initTransport();
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

void SBCManager::internal_run()
{
  try
  {
    SBCDirectories::instance()->prepareDirectories();
	  initialize();
    _transactionManager.stack().run();
    _dialogStateManager.run();
    _pRegisterHandler->startOptionsThread();
    _pRegisterHandler->pauseKeepAlive(false);
  }
  catch(...)
  {
    reportCriticalState("Unable to run SBCManager");
  }
}

bool SBCManager::run()
{
	if (_thread)
	{
		return false;
	}
	_thread = new boost::thread(boost::bind(&SBCManager::internal_run, this));
	return true;
}

void SBCManager::stop()
{
	if (!_thread)
	{
		return;
	}
	
  std::cout << "Stopping Dialog State Manager ..." << std::endl;
  _dialogStateManager.stop();
  std::cout << "Dialog State Manager STOPPED ..." << std::endl;

  std::cout << "Stopping SIP Stack ..." << std::endl;
  _transactionManager.deinitialize();
  _transactionManager.stack().stop();
  std::cout << "SIP Stack Stopped..." << std::endl;
  
  _thread->join();
  delete _thread;
  _thread = 0;
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
  std::string eventName = "criticalState";
  SBCJSModuleManager::CustomEventArgs args, result;
  args["error_message"] = msg;
  SBCJSModuleManager::instance()->processCustomEvent(eventName, args, result);
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


