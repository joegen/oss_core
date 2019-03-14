
// OSS Software Solutions Application Programmer Interface
// Package: Karoo
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
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



#include "OSS/SIP/SBC/SBCChannelLimits.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
  
static const Poco::Timestamp::TimeDiff DEFAULT_CACHE_EXPIRE = 60*60*1000;  /// 1 hour lifetime
static const std::size_t MINIMUM_DIAL_STRING_LENGTH = 7;
static const char* CHANNEL_COUNT_PREFIX = "sbc.channel-count-";
static const char* CHANNEL_COUNT_PREFIX_WILD_CARD = "sbc.channel-count-*";


using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;

  
SBCChannelLimits::SBCChannelLimits() :
  _shortestPrefix(0),
  _cacheExpire(DEFAULT_CACHE_EXPIRE),
  _pManager(0)
{
}

SBCChannelLimits::~SBCChannelLimits()
{
}

void SBCChannelLimits::initialize(SBCManager* pSBCManager)
{
  _pManager = pSBCManager;
  _systemDb = _pManager->workspace().getSystemDb();
  
  const boost::filesystem::path& configFile = _pManager->getSIPConfigurationFile();
  ClassType config;
  if (!config.load(configFile))
  {
    return;
  }
  
  OSS_LOG_NOTICE("SBCChannelLimits::initialize - INVOKED");
  
  DataType root = config.self();
  if (root.exists("user-agent"))
  {
    //
    // Clear out the previous channel count
    //
    std::vector<std::string> keys;
    _systemDb->getKeys(CHANNEL_COUNT_PREFIX_WILD_CARD, keys);
    for (std::vector<std::string>::iterator iter = keys.begin(); iter != keys.end(); iter++)
    {
      _systemDb->del(*iter);
    }
    
    try
    {
      DataType userAgent = root["user-agent"];
      if (userAgent.exists("channel-limits"))
      {
        DataType limits = userAgent["channel-limits"];
        int dbCount = limits.getElementCount();
        for (int i = 0; i < dbCount; i++)
        {
          DataType dbInfo = limits[i];
          
          if (dbInfo.exists("enabled") && dbInfo.exists("prefix") && dbInfo.exists("max-channels"))
          {
            bool enabled = (bool)dbInfo["enabled"];
            if (enabled)
            {
              std::string prefix = (const char*)dbInfo["prefix"];
              int maxChannels = (int)dbInfo["max-channels"];
              registerDialPrefix(prefix, maxChannels);
            }
          }
        }
      }
    }
    catch(...)
    {
    }
  }
}
 
void SBCChannelLimits::registerDialPrefix(const std::string& prefix_, std::size_t channelLimit)
{
  OSS::mutex_critic_sec_lock lock(_prefixesMutex);
  
  std::string prefix = prefix_;
  std::vector<std::string> tokens;
  tokens = OSS::string_tokenize(prefix, ",");
  
  if (tokens.size() > 1)
  {
    prefix = tokens[0];
    
    for (std::size_t i = 1; i < tokens.size(); i++)
    {
      std::string aliasPrefix = tokens[i];
      OSS::string_trim(aliasPrefix);
      
      _aliases[aliasPrefix] = prefix;
           
      if (!_shortestPrefix || _shortestPrefix > aliasPrefix.length())
      {
        _shortestPrefix = aliasPrefix.length();
      }
    }
  }
  
  if (!_shortestPrefix || _shortestPrefix > prefix.length())
  {
    _shortestPrefix = prefix.length();
  }
  
  _prefixes[prefix] =  ExpireCachePtr(new ExpireCache(_cacheExpire));
  _limits[prefix] = channelLimit;
  
  //
  // Zero out the workspace counter
  //
  std::ostringstream counterKey;
  counterKey << CHANNEL_COUNT_PREFIX << prefix;
  json::Object params;
  params["prefix"] = json::String(prefix);
  params["max-call-count"] = json::Number(channelLimit);
  params["active-call-count"] = json::Number(0);
  
  if (_systemDb)
  {
    _systemDb->set(counterKey.str(), params);
  }
  
  OSS_LOG_NOTICE("SBCChannelLimits::registerDialPrefix - Enforcing channel limit " << channelLimit << " for prefix " << prefix);
}

void SBCChannelLimits::resolveAliasPrefix(const std::string& dialString, std::string& prefix)
{
  OSS::mutex_critic_sec_lock lock(_prefixesMutex);
  std::string lowerBoundKey = OSS::string_left(dialString, _shortestPrefix);
  std::string lastMatch;
   
  for (PrefixAliases::iterator iter = _aliases.lower_bound(lowerBoundKey); iter != _aliases.end(); iter++)
  {
    if (OSS::string_starts_with(dialString, iter->first.c_str()))
    {
      lastMatch = iter->second;
      break;
    }
  }
  
  if (!lastMatch.empty())
  {
    prefix = lastMatch;
  }
}

std::size_t SBCChannelLimits::addCall(const std::string& sessionId, const std::string& dialString, std::size_t& channelLimit)
{
  std::string alias;
  resolveAliasPrefix(dialString, alias);
  
  //
  // Note:  do not lock the mutex before resolveAliasPrefix() or you will deadlock)
  //
  OSS::mutex_critic_sec_lock lock(_prefixesMutex);
  std::string lowerBoundKey = OSS::string_left(dialString, _shortestPrefix);
  std::size_t count = 0;
  
  std::string lastMatch;
  
  if (alias.empty())
  {
    for (ChannelMap::iterator iter = _prefixes.lower_bound(lowerBoundKey); iter != _prefixes.end(); iter++)
    {
      if (OSS::string_starts_with(dialString, iter->first.c_str()))
      {
        lastMatch = iter->first;
        break;
      }
    }
  }
  else
  {
    lastMatch = alias;
  }
  
  if (!lastMatch.empty())
  {
    ChannelMap::iterator iter = _prefixes.find(lastMatch);
    if (iter != _prefixes.end())
    {
      iter->second->add(sessionId, dialString);
      count = iter->second->size();
      channelLimit = _limits[lastMatch];
      
      //
      // Update the workspace counter
      //
      std::ostringstream counterKey;
      counterKey << CHANNEL_COUNT_PREFIX << lastMatch;
      json::Object params;
      params["prefix"] = json::String(lastMatch);
      params["max-call-count"] = json::Number(channelLimit);
      params["active-call-count"] = json::Number(count);
      
      OSS_LOG_INFO("SBCChannelLimits::addCall - " << dialString << " matches channel prefix " << lastMatch << " Current count is " << count);
      
      if (_systemDb)
      {
        _systemDb->set(counterKey.str(), params);
      }
    }
    
    if (count > channelLimit)
    {
      OSS_LOG_WARNING("SBCChannelLimits::addCall - Channel limit violation for call " << dialString << " Current channel count is already " << count);
    }
  }
  else
  {
    OSS_LOG_INFO("SBCChannelLimits::addCall - " << dialString << " did not match any registrered channel prefix");
  }
  
  
  
  return count;
}

std::size_t SBCChannelLimits::getCallCount(const std::string& prefix)
{
  std::size_t callCount = 0;
  
  if (!prefix.empty())
  {
    OSS::mutex_critic_sec_lock lock(_prefixesMutex);
    ChannelMap::iterator iter = _prefixes.find(prefix);
    if (iter != _prefixes.end())
    {
      callCount = iter->second->size();
      OSS_LOG_INFO("SBCChannelLimits::getCallCount(" << prefix << ") returned " << callCount);
    }
    else
    {
      OSS_LOG_WARNING("SBCChannelLimits::getCallCount() - Invalid prefix " << prefix);
    }
  }
  
  return callCount;
}
  
std::size_t SBCChannelLimits::removeCall(const std::string& sessionId, const std::string& dialString)
{ 
  std::string alias;
  resolveAliasPrefix(dialString, alias);
  
  //
  // Note:  do not lock the mutex before resolveAliasPrefix() or you will deadlock)
  //
  OSS::mutex_critic_sec_lock lock(_prefixesMutex);
  std::string lowerBoundKey = OSS::string_left(dialString, _shortestPrefix);
  std::size_t count = 0;
  
  std::string lastMatch;
  if (alias.empty())
  {
    for (ChannelMap::iterator iter = _prefixes.lower_bound(lowerBoundKey); iter != _prefixes.end(); iter++)
    {
      if (OSS::string_starts_with(dialString, iter->first.c_str()))
      {
        lastMatch = iter->first;
        break;
      }
    }
  }
  else
  {
    lastMatch = alias;
  }
  
  if (!lastMatch.empty())
  {
    ChannelMap::iterator iter = _prefixes.find(lastMatch);
    if (iter != _prefixes.end())
    {
      iter->second->remove(sessionId);
      count = iter->second->size();
      
      //
      // Update the workspace counter
      //
      int channelLimit = _limits[lastMatch];
      std::ostringstream counterKey;
      counterKey << CHANNEL_COUNT_PREFIX << lastMatch;
      json::Object params;
      params["prefix"] = json::String(lastMatch);
      params["max-call-count"] = json::Number(channelLimit);
      params["active-call-count"] = json::Number(count);
      
      if (_systemDb)
      {
        _systemDb->set(counterKey.str(), params);
      }
    }
  }
  
  return count;
}


} } } // OSS::SIP::SBC



