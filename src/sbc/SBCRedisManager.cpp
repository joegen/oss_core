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


#include "OSS/SIP/SBC/SBCRedisManager.h"


namespace OSS {
namespace SIP {
namespace SBC {


static const char* REDIS_SUBSCRIPTION_CHANNEL = "SBC-CHANNEL";


SBCRedisManager::SBCRedisManager() :
  _pSubscribeThread(0),
  _isTerminating(false),
  _channelName(REDIS_SUBSCRIPTION_CHANNEL)
{
}

SBCRedisManager::~SBCRedisManager()
{
  stop();
}

void SBCRedisManager::stop()
{
  _isTerminating = true;
  if (_pSubscribeThread && _subscriber)
  {
    _subscriber->disconnect();
    _pSubscribeThread->join();
    delete _pSubscribeThread;
    _pSubscribeThread = 0;
  }
}

void SBCRedisManager::initialize(const boost::filesystem::path& configFile)
{
  SBCRedisConfig connector("SBCRedisManager", configFile);
  connector.dumpConnectionInfoToFile();
  initialize(connector);
}

void SBCRedisManager::initialize(SBCRedisConfig& connector)
{
  OSS_LOG_NOTICE("[REDIS] SBCRedisManager::initialize(SBC_SYSTEMDB) in workspace " << SBC_SYSTEMDB );
  _systemDb = WorkSpace(new Persistent::RedisBroadcastClient());
  if (!connector.connect(*_systemDb, SBC_SYSTEMDB, false))
  {
    OSS_LOG_WARNING("[REDIS] SBCRedisManager::initialize(SBC_SYSTEMDB) in workspace " << SBC_SYSTEMDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCRedisManager::initialize(SBC_SUBSCRIBER) in workspace " << SBC_SYSTEMDB );
  _subscriber = Subscriber(new Persistent::RedisClient());
  if (!connector.connect(*_subscriber, SBC_SYSTEMDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCRedisManager::initialize(SBC_SUBSCRIBER) in workspace " << SBC_SYSTEMDB << " ** Temporary Failure **");
  }
  // Start the subscription thread
  //
  _pSubscribeThread = new boost::thread(boost::bind(&SBCRedisManager::handleSubscription, this));
  
  OSS_LOG_NOTICE("[REDIS] SBCRedisManager::initialize(SBC_REGDB) in workspace " << SBC_REGDB );
  _regDb = WorkSpace(new Persistent::RedisBroadcastClient());
  if (!connector.connect(*_regDb, SBC_REGDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCRedisManager::initialize(SBC_REGDB) in workspace " << SBC_REGDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCRedisManager::initialize(SBC_DIALOGDB) in workspace " << SBC_DIALOGDB );
  _dialogDb = WorkSpace(new Persistent::RedisBroadcastClient());
  if (!connector.connect(*_dialogDb, SBC_DIALOGDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCRedisManager::initialize(SBC_DIALOGDB) in workspace " << SBC_DIALOGDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCRedisManager::initialize(SBC_RTPDB) in workspace " << SBC_RTPDB );
  _rtpDb = WorkSpace(new Persistent::RedisBroadcastClient());
  if (!connector.connect(*_rtpDb, SBC_RTPDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCRedisManager::initialize(SBC_RTPDB) in workspace " << SBC_RTPDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCRedisManager::initialize(SBC_CDRDB) in workspace " << SBC_CDRDB );
  _cdrDb = WorkSpace(new Persistent::RedisBroadcastClient());
  if (!connector.connect(*_cdrDb, SBC_CDRDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCRedisManager::initialize(SBC_CDRDB) in workspace " << SBC_CDRDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCRedisManager::initialize(SBC_ACCOUNTDB) in workspace " << SBC_ACCOUNTDB );
  _accountDb = WorkSpace(new Persistent::RedisBroadcastClient());
  if (!connector.connect(*_accountDb, SBC_ACCOUNTDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCRedisManager::initialize(SBC_ACCOUNTDB) in workspace " << SBC_ACCOUNTDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCRedisManager::initialize(SBC_LOCAL_REGDB) in workspace " << SBC_LOCAL_REGDB );
  _localRegDb = WorkSpace(new Persistent::RedisBroadcastClient());
  if (!connector.connect(*_localRegDb, SBC_LOCAL_REGDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCRedisManager::initialize(SBC_LOCAL_REGDB) in workspace " << SBC_LOCAL_REGDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCRedisManager::initialize(SBC_BANNED_ADDRESSDB) in workspace " << SBC_BANNED_ADDRESSDB );
  _bannedAddressDb = WorkSpace(new Persistent::RedisBroadcastClient());
  if (!connector.connect(*_bannedAddressDb, SBC_BANNED_ADDRESSDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCRedisManager::initialize(SBC_BANNED_ADDRESSDB) in workspace " << SBC_BANNED_ADDRESSDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCRedisManager::initialize(SBC_ROUTEDB) in workspace " << SBC_ROUTEDB );
  _routeDb = WorkSpace(new Persistent::RedisBroadcastClient());
  if (!connector.connect(*_routeDb, SBC_ROUTEDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCRedisManager::initialize(SBC_ROUTEDB) in workspace " << SBC_ROUTEDB << " ** Temporary Failure **");
  }
}

bool SBCRedisManager::subscribe()
{
  //
  // Try to subscribe to the channel
  //
  while(!_isTerminating)
  {
    std::vector<std::string> reply;
    if (_subscriber->subscribe(_channelName, reply))
    {
      return true;
    }
    else
    {
      //
      // try a reconnect
      //
      if (_subscriber->connect())
      {
        std::vector<std::string> resubscribe;
        if (_subscriber->subscribe(_channelName, resubscribe))
        {
          return true;
        }
        else
        {
          OSS::thread_sleep(1000);
        }
      }
      else
      {
        OSS::thread_sleep(1000);
      }
    }
  }
  
  return false;
}

void SBCRedisManager::dispatchEvent(json::Object& eventObject)
{
  json::Object::iterator iter;
  std::string eventName;
  iter = eventObject.Find("event-name");
  if (iter != eventObject.End())
  {
    json::String element = iter->element;
    eventName = element.Value();
  }

  if (!eventName.empty())
  {
    EventHandlers::iterator handler = _handlers.find(eventName);
    if (handler != _handlers.end())
    {
      handler->second(eventName, eventObject);
    }
  }
}

void SBCRedisManager::handleSubscription()
{
  if (!subscribe())
  {
    return;
  }
  
  while(!_isTerminating)
  {
    std::vector<std::string> eventData;
    _subscriber->receive(eventData);
    if (eventData.size() == 3 && eventData[0] == "io-error")
    {
      //
      // this is a timeout
      //
      if (!_isTerminating)
      {
        OSS::thread_sleep(100);  /// just in case next read goes into a nutty state
      }
      continue;
    }
    else if (eventData.size() == 3 && eventData[0] == "connection-error")
    {
      OSS_LOG_WARNING("SBCRedisManager::handleSubscription - Connection Error: << " << eventData[1] << " : " << eventData[2]);
      if (!_isTerminating)
      {
        //
        // Try to reconnect
        //
        if (!subscribe())
        {
          break;
        }
      }
    }
    else if (eventData.size() == 3)
    {
      try
      {
        std::stringstream rstrm;
        rstrm << eventData[2];
        json::Object eventObject;
        json::Reader::Read(eventObject, rstrm);
        dispatchEvent(eventObject);
      }
      catch(...)
      {
        OSS_LOG_ERROR("SBCRedisManager::handleSubscription - Unable to parse event " << eventData[2]);
      }
    }
  }
}

const SBCRedisManager::WorkSpace& SBCRedisManager::getWorkSpace(RedisWorkspace workSpace) const
{
  switch(workSpace)
  {
  case SBC_SYSTEMDB:
    return _systemDb;
  case SBC_REGDB:
    return _regDb;
  case SBC_DIALOGDB:
    return _dialogDb;
  case SBC_RTPDB:
    return _rtpDb;
  case SBC_CDRDB:
    return _cdrDb;
  case SBC_ACCOUNTDB:
    return _accountDb;
  case SBC_LOCAL_REGDB:
    return _localRegDb;
  case SBC_BANNED_ADDRESSDB:
    return _bannedAddressDb;
  case SBC_ROUTEDB:
    return _routeDb;
  default:
    return _undefinedDb;
  }
}

  
} } } // OSS::SIP::SBC




