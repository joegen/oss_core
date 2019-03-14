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


#ifndef SBCCHANNELLIMITS_H_INCLUDED
#define	SBCCHANNELLIMITS_H_INCLUDED


#include "OSS/SIP/SBC/SBCWorkSpaceManager.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"
#include "OSS/UTL/BlockingQueue.h"
#include "OSS/UTL/LogFile.h"
#include <Poco/ExpireCache.h>


namespace OSS {
namespace SIP {
namespace SBC {
  
  
class SBCManager;

  
class SBCChannelLimits
{
public: 
  typedef Poco::ExpireCache<std::string, std::string> ExpireCache;
  typedef boost::shared_ptr<ExpireCache> ExpireCachePtr;
  typedef std::map<std::string, ExpireCachePtr> ChannelMap;
  typedef std::map<std::string, std::size_t> Limits;
  typedef std::map<std::string, std::string> PrefixAliases;
  
  SBCChannelLimits();
  
  ~SBCChannelLimits();
  
  void initialize(SBCManager* pSBCManager);
  
  void registerDialPrefix(const std::string& prefix, std::size_t channelLimit);
  
  std::size_t addCall(const std::string& sessionId, const std::string& dialString, std::size_t& channelLimit);
  
  std::size_t removeCall(const std::string& sessionId, const std::string& dialString);
  
  void setSystemDb(const SBCWorkSpaceManager::WorkSpace& systemDb);
  
  std::size_t getCallCount(const std::string& prefix);
  
protected:
  void resolveAliasPrefix(const std::string& dialString, std::string& prefix);
  
private:
  ChannelMap _prefixes;
  OSS::mutex_critic_sec _prefixesMutex;
  std::size_t _shortestPrefix;
  Poco::Timestamp::TimeDiff _cacheExpire;
  Limits _limits;
  PrefixAliases _aliases;
  SBCManager* _pManager;
  SBCWorkSpaceManager::WorkSpace _systemDb;
};

//
// Inlines
//
inline void SBCChannelLimits::setSystemDb(const SBCWorkSpaceManager::WorkSpace& systemDb)
{
  _systemDb = systemDb;
}

} } } // OSS::SIP::SBC

#endif // SBCCHANNELLIMITS_H_INCLUDED

