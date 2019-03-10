
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


#include "OSS/SIP/SBC/SBCAccounts.h"
#include "OSS/SIP/SIPDigestAuth.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
SBCAccounts::SBCAccounts() :
  _hasDeterminedRealms(false)
{
}
  
SBCAccounts::~SBCAccounts()
{
}
  
void SBCAccounts::initialize(const SBCRedisManager::WorkSpace& workspace)
{
  _workspace = workspace;
  determineRealms();
}

void SBCAccounts::determineRealms()
{
  //
  // Get the local domains based on the keys
  //
  if (_hasDeterminedRealms || !_workspace)
  {
    return;
  }
   
  std::vector<std::string> identities;
  if(_workspace->getKeys("*", identities))
  {
    _hasDeterminedRealms = true;
  }
  
  OSS::mutex_critic_sec_lock lock(_volatileAccountsMutex);  
  for (std::vector<std::string>::iterator iter = identities.begin(); iter != identities.end(); iter++)
  {
    std::vector<std::string> tokens = OSS::string_tokenize(*iter, "@");
    if (tokens.size() == 2)
    {
      _realms.insert(tokens[1]);
    }
  }
}

bool SBCAccounts::isKnownRealm(const std::string& realm) const
{
  const_cast<SBCAccounts*>(this)->determineRealms();
  OSS::mutex_critic_sec_lock lock(_volatileAccountsMutex);  // do not lock the mutex before determineRealms()
  return _realms.find(realm) != _realms.end();
}

bool SBCAccounts::isKnownIdentity(const std::string& identity) const
{
  SBCAccountRecord account;
  return findAccount(identity, account);
}

bool SBCAccounts::findAccount(const std::string& identity, SBCAccountRecord& account) const
{ 
  {
    OSS::mutex_critic_sec_lock lock(_volatileAccountsMutex);
    VolatileAccounts::const_iterator iter = _volatileAccounts.find(identity);
    if (iter != _volatileAccounts.end())
    {
      account = iter->second;
      return account.isValid();
    }
  }
  
  if (!_workspace || !account.readFromRedis(_workspace, identity))
  {
    return false;
  }
  
  return account.isValid();
}

bool SBCAccounts::addAccount(SBCAccountRecord& account)
{
  if (!_workspace )
  {
    OSS_LOG_ERROR("SBCAccounts::addAccount - Workspace is not set");
    return false;
  }
  
  if  (!account.isValid())
  {
    OSS_LOG_ERROR("SBCAccounts::addAccount - Invalid account record");
    return false;
  }
  
  if(!account.writeToRedis(_workspace, account.getIdentity()))
  {
    OSS_LOG_ERROR("SBCAccounts::addAccount - Unable to store account to Redis database");
    return false;
  }
  OSS::mutex_critic_sec_lock lock(_volatileAccountsMutex);
  _realms.insert(account.getRealm());
  return true;
}

bool SBCAccounts::addVolatileAccount(SBCAccountRecord& account)
{
  if (!account.isValid())
  {
    return false;
  }
  OSS::mutex_critic_sec_lock lock(_volatileAccountsMutex);
  _volatileAccounts[account.getIdentity()] = account;
  _realms.insert(account.getRealm());
  return true;
}

void SBCAccounts::addRealm(const std::string& realm)
{
  OSS::mutex_critic_sec_lock lock(_volatileAccountsMutex);
  _realms.insert(realm);
}
  

} } }  // OSS::SIP::SBC



