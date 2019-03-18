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

#ifndef SBCWORKSPACEMANAGER_H_INCLUDED
#define	SBCWORKSPACEMANAGER_H_INCLUDED


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
  typedef boost::shared_ptr<SBCWorkSpace> WorkSpace;
  typedef BlockingQueue<json::Object*> EventQueue;
  
  SBCWorkSpaceManager();
  
  ~SBCWorkSpaceManager();
  

  bool initialize();
  
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
  
  const std::string& getChannelName() const;
  
private:
  
  WorkSpace _systemDb;
  WorkSpace _regDb;
  WorkSpace _dialogDb;
  WorkSpace _rtpDb;
  WorkSpace _cdrDb;
  WorkSpace _accountDb;
  WorkSpace _localRegDb;
  WorkSpace _bannedAddressDb;
  WorkSpace _routeDb;
  bool _isTerminating;
};


//
// Inlines
//

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getSystemDb() const
{
  return _systemDb;
}
  
inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getRegDb() const
{
  return _regDb;
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getDialogDb() const
{
  return _dialogDb;
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getRTPDb() const
{
  return _rtpDb;
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getCDRDb() const
{
  return _cdrDb;
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getAccountDb() const
{
  return _accountDb;
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getLocalRegDb() const
{
  return _localRegDb;
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getBannedAddressDb() const
{
  return _bannedAddressDb;
}

inline const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getRouteDb() const
{
  return _routeDb;
}

} } } // OSS::SIP::SBC

#endif	// SBCWORKSPACEMANAGER_H_INCLUDED

