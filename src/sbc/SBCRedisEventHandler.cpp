
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


#include "OSS/UTL/Logger.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/SIP/SBC/SBCRedisEventHandler.h"
#include "OSS/SIP/SBC/SBCAccountRecord.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/Exec/Command.h"


namespace OSS {
namespace SIP {
namespace SBC {

static bool json_get_string(json::Object& eventObject, const std::string& eventName, std::string& value)
{
  try
  {
    json::Object::iterator iter = eventObject.Find(eventName);
    if (iter != eventObject.End())
    {
      json::String element = iter->element;;
      value = element.Value();
      return true;
    }
  }
  catch(json::Exception& e)
  {
    OSS_LOG_ERROR("SBCRedisEventHandler::json_get_string exception: " << e.what());
  }
  return false;
}
  
static bool get_event_ags(json::Object& eventObject, std::string& eventArgs)
{
  return json_get_string(eventObject, "event-args", eventArgs);
}
  
SBCRedisEventHandler::SBCRedisEventHandler() :
  _pManager(0),
  _pTaskThread(0)
{
}
  
SBCRedisEventHandler::~SBCRedisEventHandler()
{
  if (_pTaskThread)
  {
    _tasks.enqueue(0);
    _pTaskThread->join();
    delete _pTaskThread;
    _pTaskThread = 0;
  }
}
  
bool SBCRedisEventHandler::initialize(SBCManager* pManager)
{
  if (_pManager)
  {
    return false;
  }
  _pManager = pManager;
  _pManager->workspace().addEventHandler("log-level", boost::bind(&SBCRedisEventHandler::setLogLevel, this, _1, _2));
  _pManager->workspace().addEventHandler("user-add", boost::bind(&SBCRedisEventHandler::addLocalAccount, this, _1, _2));

  initializeBlockedSources();
 
  _pTaskThread = new boost::thread(boost::bind(&SBCRedisEventHandler::runTasks, this));
  
  return true;
}

void SBCRedisEventHandler::initializeBlockedSources()
{
  _pManager->workspace().addEventHandler("unblock-source", boost::bind(&SBCRedisEventHandler::unblockSource, this, _1, _2));
  _pManager->workspace().addEventHandler("block-source", boost::bind(&SBCRedisEventHandler::blockSource, this, _1, _2));
  _pManager->workspace().addEventHandler("unblock-network", boost::bind(&SBCRedisEventHandler::unblockNetwork, this, _1, _2));
  _pManager->workspace().addEventHandler("block-network", boost::bind(&SBCRedisEventHandler::blockNetwork, this, _1, _2));
  
  _pManager->workspace().addEventHandler("whitelist-source", boost::bind(&SBCRedisEventHandler::whiteListSource, this, _1, _2));
  _pManager->workspace().addEventHandler("whitelist-network", boost::bind(&SBCRedisEventHandler::whiteListNetwork, this, _1, _2));
  _pManager->workspace().addEventHandler("remove-whitelist-source", boost::bind(&SBCRedisEventHandler::removeWhiteListSource, this, _1, _2));
  _pManager->workspace().addEventHandler("remove-whitelist-network", boost::bind(&SBCRedisEventHandler::removeWhiteListNetwork, this, _1, _2));
  
  SIPTransportSession::rateLimit().setBanCallback(boost::bind(&SBCRedisEventHandler::handleBlockSource, this, _1));
  
  std::string pattern("violator-*");
  std::vector<std::string> keys;
  _pManager->workspace().getBannedAddressDb()->getKeys(pattern, keys);
  for (std::vector<std::string> ::iterator iter = keys.begin(); iter != keys.end(); iter++)
  {
    std::string value;
    _pManager->workspace().getBannedAddressDb()->get(*iter, value);
    if (!value.empty())
    {
      try
      {
        boost::asio::ip::address address;
        address = boost::asio::ip::address::from_string(value);
        SIPTransportSession::rateLimit().banAddress(address);
      }
      catch(...)
      {
      }
    }
  }
  
  pattern = std::string("ip-blacklist-*");
  keys.clear();
  _pManager->workspace().getBannedAddressDb()->getKeys(pattern, keys);
  for (std::vector<std::string> ::iterator iter = keys.begin(); iter != keys.end(); iter++)
  {
    std::string value;
    _pManager->workspace().getBannedAddressDb()->get(*iter, value);
    if (!value.empty())
    {
      try
      {
        boost::asio::ip::address address;
        address = boost::asio::ip::address::from_string(value);
        SIPTransportSession::rateLimit().blackListAddress(address, true);
      }
      catch(...)
      {
      }
    }
  }
  
  pattern = std::string("network-blacklist-*");
  keys.clear();
  _pManager->workspace().getBannedAddressDb()->getKeys(pattern, keys);
  for (std::vector<std::string> ::iterator iter = keys.begin(); iter != keys.end(); iter++)
  {
    std::string value;
    _pManager->workspace().getBannedAddressDb()->get(*iter, value);
    if (!value.empty())
    {
      try
      {
        SIPTransportSession::rateLimit().blackListNetwork(value);
      }
      catch(...)
      {
      }
    }
  }
}

void SBCRedisEventHandler::runTasks()
{
  while (true)
  {
    Task* pTask = 0;
    _tasks.dequeue(pTask);
    if (pTask)
    {
      handleTask(*pTask);
      delete pTask;
    }
    else
    {
      break;
    }
  }
}

void SBCRedisEventHandler::handleTask(Task& task)
{
  if (task.taskName == "execute-on-ban")
  {
    if (task.taskProperties.find("script") != task.taskProperties.end() && task.taskProperties.find("address") != task.taskProperties.end() )
    {
      std::ostringstream args;
      args << task.taskProperties["script"] << " " << task.taskProperties["address"];
      OSS::Exec::Command cmd;
      cmd.execute(args.str());
    }
  }
}

void SBCRedisEventHandler::setLogLevel(const std::string& eventName, json::Object& eventObject)
{
  
  std::string sLogLevel;
  if (get_event_ags(eventObject, sLogLevel) && !sLogLevel.empty())
  {
    OSS::string_to_lower(sLogLevel);
    
    OSS_LOG_NOTICE("SBCRedisEventHandler::setLogLevel( " << sLogLevel << " ) INVOKED" );
    
    if (sLogLevel == "fatal")//  A fatal error. The application will most likely terminate. This is the highest priority.
    {
      OSS::log_reset_level(OSS::PRIO_FATAL);
    }
    else if (sLogLevel == "critical")//  A critical error. The application might not be able to continue running successfully.
    {
      OSS::log_reset_level(OSS::PRIO_CRITICAL);
    }
    else if (sLogLevel == "error")//  An error. An operation did not complete successfully, but the application as a whole is not affected.
    {
      OSS::log_reset_level(OSS::PRIO_ERROR);
    }
    else if (sLogLevel == "warning")//  A warning. An operation completed with an unexpected result.
    {
      OSS::log_reset_level(OSS::PRIO_WARNING);
    }
    else if (sLogLevel == "notice")//  A notice, which is an information with just a higher priority.
    {
      OSS::log_reset_level(OSS::PRIO_NOTICE);
    }
    else if (sLogLevel == "information")//  An informational message, usually denoting the successful completion of an operation.
    {
      OSS::log_reset_level(OSS::PRIO_INFORMATION);
    }
    else if (sLogLevel == "debug")//  A debugging message.
    {
      OSS::log_reset_level(OSS::PRIO_DEBUG);
    }
    else if (sLogLevel == "trace")//  A tracing message. This is the lowest priority.
    {
      OSS::log_reset_level(OSS::PRIO_TRACE);
    }
  }
}

void SBCRedisEventHandler::addLocalAccount(const std::string& eventName, json::Object& eventObject)
{
  std::string args;
  if (get_event_ags(eventObject, args) && !args.empty())
  {
    std::vector<std::string> argsTokens;
    argsTokens = OSS::string_tokenize(args, " ");
    if (argsTokens.size() != 2)
    {
      return;
    }
    
    std::string identity = argsTokens[0];
    std::string password = argsTokens[1];
    //
    // strip the scheme if it's there
    //
    if (identity.find("sip:") == 0)
    {
      identity = identity.data() + 4;
    }
    else if (identity.find("sips:") == 0)
    {
      identity = identity.data() + 5;
    }

    std::vector<std::string> identityTokens;
    identityTokens = OSS::string_tokenize(identity, "@");

    if (identityTokens.size() == 2)
    {
      std::string user = identityTokens[0];
      std::string realm = identityTokens[1];
      std::string a1Hash;

      SBCAccountRecord accountRecord;
      accountRecord.setIdentity(identity);
      accountRecord.setRealm(realm);
      accountRecord.setUser(user);
      accountRecord.computeA1Hash(password, a1Hash);
      accountRecord.setA1Hash(a1Hash);
      
  
      OSS_LOG_NOTICE("SBCRedisEventHandler::addLocalAccount( " << identity << " ) INVOKED");
      if (_pManager->authenticator().accounts().addAccount(accountRecord))
      {
        OSS_LOG_NOTICE("SBCRedisEventHandler::addLocalAccount( " << identity << " ) OK");
      }
      else
      {
        OSS_LOG_NOTICE("SBCRedisEventHandler::addLocalAccount( " << identity << " ) FAILED");
      }
    }
  }
}

  
void SBCRedisEventHandler::handleBlockSource(const boost::asio::ip::address& address)
{
  std::string key("violator-");
  key += address.to_string();
  _pManager->workspace().getBannedAddressDb()->set(key, address.to_string(), SIPTransportSession::rateLimit().getBanLifeTime());
  
  if (!_pManager->getExecuteOnBanScript().empty())
  {
    //
    // this can take long so use the task thread to spawn it
    //
    Task* pTask = new Task();
    pTask->taskName = "execute-on-ban";
    pTask->taskProperties["script"] = _pManager->getExecuteOnBanScript();
    pTask->taskProperties["address"] = address.to_string();
  }
}

void SBCRedisEventHandler::unblockSource(const std::string& eventName, json::Object& eventObject)
{
  std::string arg;
  if (get_event_ags(eventObject, arg) && !arg.empty())
  {
    boost::asio::ip::address address;
    try
    {
      address = boost::asio::ip::address::from_string(arg);
      SIPTransportSession::rateLimit().clearAddress(address, false);
      std::string key("violator-");
      key += address.to_string();
       _pManager->workspace().getBannedAddressDb()->del(key);
    }
    catch(...)
    {
    }
  }
}

void SBCRedisEventHandler::blockSource(const std::string& eventName, json::Object& eventObject)
{
  std::string arg;
  if (get_event_ags(eventObject, arg) && !arg.empty())
  {
    boost::asio::ip::address address;
    try
    {
      address = boost::asio::ip::address::from_string(arg);
      SIPTransportSession::rateLimit().blackListAddress(address, true);
      std::string key("ip-blacklist-");
      key += address.to_string();
      _pManager->workspace().getBannedAddressDb()->set(key, address.to_string());
    }
    catch(...)
    {
    }
  }
}

void SBCRedisEventHandler::blockNetwork(const std::string& eventName, json::Object& eventObject)
{
  std::string cidr;
  if (get_event_ags(eventObject, cidr) && !cidr.empty())
  {
    try
    {
      std::string key("network-blacklist-");
      key += cidr;
      _pManager->workspace().getBannedAddressDb()->set(key, cidr);
      SIPTransportSession::rateLimit().blackListNetwork(cidr);
    }
    catch(...)
    {
    }
  }
}

void SBCRedisEventHandler::unblockNetwork(const std::string& eventName, json::Object& eventObject)
{
  std::string cidr;
  if (get_event_ags(eventObject, cidr) && !cidr.empty())
  {
    try
    {
      std::string key("network-blacklist-");
      key += cidr;
      _pManager->workspace().getBannedAddressDb()->del(key);
      SIPTransportSession::rateLimit().clearNetwork(cidr);
    }
    catch(...)
    {
    }
  }
}

void SBCRedisEventHandler::whiteListSource(const std::string& eventName, json::Object& eventObject)
{
  std::string arg;
  if (get_event_ags(eventObject, arg) && !arg.empty())
  {
    boost::asio::ip::address address;
    try
    {
      address = boost::asio::ip::address::from_string(arg);
      SIPTransportSession::rateLimit().whiteListAddress(address, true);
      std::string key("ip-whitelist-");
      key += address.to_string();
      _pManager->workspace().getBannedAddressDb()->set(key, address.to_string());
    }
    catch(...)
    {
    }
  }
}

void SBCRedisEventHandler::whiteListNetwork(const std::string& eventName, json::Object& eventObject)
{
  std::string cidr;
  if (get_event_ags(eventObject, cidr) && !cidr.empty())
  {
    try
    {
      std::string key("network-whitelist-");
      key += cidr;
      _pManager->workspace().getBannedAddressDb()->set(key, cidr);
      SIPTransportSession::rateLimit().whiteListNetwork(cidr);
    }
    catch(...)
    {
    }
  }
}

void SBCRedisEventHandler::removeWhiteListSource(const std::string& eventName, json::Object& eventObject)
{
  std::string arg;
  if (get_event_ags(eventObject, arg) && !arg.empty())
  {
    boost::asio::ip::address address;
    try
    {
      address = boost::asio::ip::address::from_string(arg);
      SIPTransportSession::rateLimit().clearWhiteList(address);
      std::string key("ip-whitelist-");
      key += address.to_string();
      _pManager->workspace().getBannedAddressDb()->del(key);
    }
    catch(...)
    {
    }
  }
}

void SBCRedisEventHandler::removeWhiteListNetwork(const std::string& eventName, json::Object& eventObject)
{
  std::string cidr;
  if (get_event_ags(eventObject, cidr) && !cidr.empty())
  {
    try
    {
      std::string key("network-whitelist-");
      key += cidr;
      _pManager->workspace().getBannedAddressDb()->del(key);
      SIPTransportSession::rateLimit().clearWhiteListNetwork(cidr);
    }
    catch(...)
    {
    }
  }
}


} } } // OSS::SIP::SBC


