
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



#include "OSS/SIP/SBC/SBCDomainLimits.h"
#include "OSS/SIP/SBC/SBCManager.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/SIP/SBC/SBCConfiguration.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
  
static const Poco::Timestamp::TimeDiff DEFAULT_CACHE_EXPIRE = 60*60*1000;  /// 1 hour lifetime
static const std::size_t MINIMUM_DIAL_STRING_LENGTH = 7;
static const char* DOMAIN_COUNT_PREFIX = "sbc.domain-channel-count-";
static const char* DOMAIN_COUNT_PREFIX_WILD_CARD = "sbc.domain-channel-count-*";


using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;

  
SBCDomainLimits::SBCDomainLimits() :
  _cacheExpire(DEFAULT_CACHE_EXPIRE),
  _pManager(0)
{
}

SBCDomainLimits::~SBCDomainLimits()
{
}

void SBCDomainLimits::initialize(SBCManager* pSBCManager)
{
  _pManager = pSBCManager;
  _systemDb = _pManager->workspace().getSystemDb();
  
  OSS_LOG_NOTICE("SBCDomainLimits::initialize - INVOKED");
  
  const OSS::JSON::Object& userAgent = SBCConfiguration::instance()->userAgent();
  if (userAgent.Exists("domain_limits"))
  {
    //
    // Clear out the previous domain count
    //
    std::vector<std::string> keys;
    _systemDb->getKeys(DOMAIN_COUNT_PREFIX_WILD_CARD, keys);
    for (std::vector<std::string>::iterator iter = keys.begin(); iter != keys.end(); iter++)
    {
      _systemDb->del(*iter);
    }
    
    try
    {
      OSS::JSON::Array limits = userAgent["domain_limits"];
      int dbCount = limits.Size();
      for (int i = 0; i < dbCount; i++)
      {
        OSS::JSON::Object dbInfo = limits[i];

        if (dbInfo.Exists("enabled") && dbInfo.Exists("domain") && dbInfo.Exists("max_channels"))
        {
          OSS::JSON::Boolean enabled = dbInfo["enabled"];
          if (enabled.Value())
          {
            OSS::JSON::String domain = dbInfo["domain"];
            OSS::JSON::Number maxChannels = dbInfo["max_channels"];
            registerDomain(domain.Value(), maxChannels.Value());
          }
        }
      }
    }
    catch(...)
    {
    }
  }
}
 
void SBCDomainLimits::registerDomain(const std::string& domain, std::size_t channelLimit)
{
  OSS::mutex_critic_sec_lock lock(_domainsMutex);
  
 _domains[domain] =  ExpireCachePtr(new ExpireCache(_cacheExpire));
  _limits[domain] = channelLimit;
  
  //
  // Zero out the workspace counter
  //
  std::ostringstream counterKey;
  counterKey << DOMAIN_COUNT_PREFIX << domain;
  json::Object params;
  params["domain"] = json::String(domain);
  params["max-call-count"] = json::Number(channelLimit);
  params["active-call-count"] = json::Number(0);
  
  if (_systemDb)
  {
    _systemDb->set(counterKey.str(), params);
  }
  
  OSS_LOG_NOTICE("SBCDomainLimits::registerDomain - Enforcing channel limit " << channelLimit << " for domain " << domain);
}

std::size_t SBCDomainLimits::addCall(const std::string& sessionId, const std::string& domain, std::size_t& channelLimit)
{
  OSS::mutex_critic_sec_lock lock(_domainsMutex);
  std::size_t count = 0;
  

  DomainMap::iterator iter = _domains.find(domain);
  if (iter != _domains.end())
  {
    iter->second->add(sessionId, domain);
    count = iter->second->size();
    channelLimit = _limits[domain];

    //
    // Update the workspace counter
    //
    std::ostringstream counterKey;
    counterKey << DOMAIN_COUNT_PREFIX << domain;
    json::Object params;
    params["domain"] = json::String(domain);
    params["max-call-count"] = json::Number(channelLimit);
    params["active-call-count"] = json::Number(count);

    OSS_LOG_INFO("SBCDomainLimits::addCall - " << domain << " matches domain domain " << domain << " Current count is " << count);

    if (_systemDb)
    {
      _systemDb->set(counterKey.str(), params);
    }
  }
    
  if (count > channelLimit)
  {
    OSS_LOG_WARNING("SBCDomainLimits::addCall - Domain limit violation for call " << domain << " Current domain count is already " << count);
  }

  return count;
}

std::size_t SBCDomainLimits::getCallCount(const std::string& domain)
{
  std::size_t callCount = 0;
  
  if (!domain.empty())
  {
    OSS::mutex_critic_sec_lock lock(_domainsMutex);
    DomainMap::iterator iter = _domains.find(domain);
    if (iter != _domains.end())
    {
      callCount = iter->second->size();
      OSS_LOG_INFO("SBCDomainLimits::getCallCount(" << domain << ") returned " << callCount);
    }
    else
    {
      OSS_LOG_WARNING("SBCDomainLimits::getCallCount() - Invalid domain " << domain);
    }
  }
  
  return callCount;
}
  
std::size_t SBCDomainLimits::removeCall(const std::string& sessionId, const std::string& domain)
{ 
 
  //
  // Note:  do not lock the mutex before resolveDomainAlias() or you will deadlock)
  //
  OSS::mutex_critic_sec_lock lock(_domainsMutex);
  std::size_t count = 0;
  

  DomainMap::iterator iter = _domains.find(domain);
  if (iter != _domains.end())
  {
    iter->second->remove(sessionId);
    count = iter->second->size();

    //
    // Update the workspace counter
    //
    int channelLimit = _limits[domain];
    std::ostringstream counterKey;
    counterKey << DOMAIN_COUNT_PREFIX << domain;
    json::Object params;
    params["domain"] = json::String(domain);
    params["max-call-count"] = json::Number(channelLimit);
    params["active-call-count"] = json::Number(count);

    if (_systemDb)
    {
      _systemDb->set(counterKey.str(), params);
    }
  }

  
  return count;
}


} } } // OSS::SIP::SBC



