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

#ifndef SBCREDISMANAGER_H_INCLUDED
#define	SBCREDISMANAGER_H_INCLUDED


#include "OSS/Persistent/RedisClient.h"
#include "OSS/SIP/SBC/SBCWorkSpaceConfig.h"
#include "OSS/UTL/BlockingQueue.h"
#include "OSS/JSON/reader.h"
#include "OSS/JSON/writer.h"
#include "OSS/SIP/SBC/SBCWorkSpace.h"


namespace OSS {
namespace SIP {
namespace SBC {

class SBCWorkSpaceManager
{
public:

  enum RedisWorkspace
  {
    SBC_SYSTEMDB = 0,
    SBC_REGDB = 1,
    SBC_DIALOGDB = 2,
    SBC_RTPDB = 3,
    SBC_CDRDB = 4,
    SBC_ACCOUNTDB = 5,
    SBC_LOCAL_REGDB = 6,
    SBC_BANNED_ADDRESSDB = 7,
    SBC_ROUTEDB = 8        
  };
  
  typedef boost::shared_ptr<SBCWorkSpace> WorkSpace;
  typedef boost::shared_ptr<Persistent::RedisClient> Subscriber;
  typedef BlockingQueue<json::Object*> EventQueue;
  typedef boost::function<void(const std::string& /*eventName*/, json::Object& /*eventObject*/)> EventCallback;
  typedef std::map<std::string, EventCallback> EventHandlers; 
  
  SBCWorkSpaceManager();
  
  ~SBCWorkSpaceManager();
  
  void initialize(const boost::filesystem::path& configFile);
  
  void initialize(SBCWorkSpaceConfig& config);
  
  void stop();
   
  const WorkSpace& getSystemDb() const;
  
  const WorkSpace& getRegDb() const;
  
  const WorkSpace& getDialogDb() const;
  
  const WorkSpace& getRTPDb() const;
  
  const WorkSpace& getCDRDb() const;
  
  const WorkSpace& getAccountDb() const;
  
  const WorkSpace& getLocalRegDb() const;
  
  const WorkSpace& getBannedAddressDb() const;
  
  const WorkSpace& getRouteDb() const;
  
  const WorkSpace& getWorkSpace(RedisWorkspace workSpace) const;
  
  const std::string& getChannelName() const;
  
  void addEventHandler(const std::string& eventName, const EventCallback& handler);
  
 
private:
  
  WorkSpace _systemDb;
  Subscriber _subscriber;
  WorkSpace _regDb;
  WorkSpace _dialogDb;
  WorkSpace _rtpDb;
  WorkSpace _cdrDb;
  WorkSpace _accountDb;
  WorkSpace _localRegDb;
  WorkSpace _bannedAddressDb;
  WorkSpace _routeDb;
  WorkSpace _undefinedDb;
  boost::thread* _pSubscribeThread;
  bool _isTerminating;
  std::string _channelName;
  EventHandlers _handlers;
};


//
// Inlines
//

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getSystemDb() const
{
  return getWorkSpace(SBC_SYSTEMDB);
}
  
inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getRegDb() const
{
  return getWorkSpace(SBC_REGDB);
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getDialogDb() const
{
  return getWorkSpace(SBC_DIALOGDB);
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getRTPDb() const
{
  return getWorkSpace(SBC_RTPDB);
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getCDRDb() const
{
  return getWorkSpace(SBC_CDRDB);
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getAccountDb() const
{
  return getWorkSpace(SBC_ACCOUNTDB);
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getLocalRegDb() const
{
  return getWorkSpace(SBC_LOCAL_REGDB);
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getBannedAddressDb() const
{
  return getWorkSpace(SBC_BANNED_ADDRESSDB);
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getRouteDb() const
{
  return getWorkSpace(SBC_ROUTEDB);
}

inline const std::string& SBCWorkSpaceManager::getChannelName() const
{
  return _channelName;
}

inline void SBCWorkSpaceManager::addEventHandler(const std::string& eventName, const EventCallback& handler)
{
  _handlers[eventName] = handler;
}

} } } // OSS::SIP::SBC

#endif	// SBCREDISMANAGER_H_INCLUDED

