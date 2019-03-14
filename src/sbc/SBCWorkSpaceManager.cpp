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


#include "OSS/SIP/SBC/SBCWorkSpaceManager.h"


namespace OSS {
namespace SIP {
namespace SBC {


static const char* REDIS_SUBSCRIPTION_CHANNEL = "SBC-CHANNEL";


SBCWorkSpaceManager::SBCWorkSpaceManager() :
  _isTerminating(false),
  _channelName(REDIS_SUBSCRIPTION_CHANNEL)
{
}

SBCWorkSpaceManager::~SBCWorkSpaceManager()
{
  stop();
}

void SBCWorkSpaceManager::stop()
{
  _isTerminating = true;
}

void SBCWorkSpaceManager::initialize(const boost::filesystem::path& configFile)
{
  SBCWorkSpaceConfig connector("SBCWorkSpaceManager", configFile);
  connector.dumpConnectionInfoToFile();
  initialize(connector);
}

void SBCWorkSpaceManager::initialize(SBCWorkSpaceConfig& connector)
{
  OSS_LOG_NOTICE("[REDIS] SBCWorkSpaceManager::initialize(SBC_SYSTEMDB) in workspace " << SBC_SYSTEMDB );
  _systemDb = WorkSpace(new SBCWorkSpace("sbc_system.bdb"));
  if (!connector.connect(*_systemDb, SBC_SYSTEMDB, false))
  {
    OSS_LOG_WARNING("[REDIS] SBCWorkSpaceManager::initialize(SBC_SYSTEMDB) in workspace " << SBC_SYSTEMDB << " ** Temporary Failure **");
  }
  

  OSS_LOG_NOTICE("[REDIS] SBCWorkSpaceManager::initialize(SBC_REGDB) in workspace " << SBC_REGDB );
  _regDb = WorkSpace(new SBCWorkSpace("sbc_reg.bdb"));
  if (!connector.connect(*_regDb, SBC_REGDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCWorkSpaceManager::initialize(SBC_REGDB) in workspace " << SBC_REGDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCWorkSpaceManager::initialize(SBC_DIALOGDB) in workspace " << SBC_DIALOGDB );
  _dialogDb = WorkSpace(new SBCWorkSpace("sbc_dialog.bdb"));
  if (!connector.connect(*_dialogDb, SBC_DIALOGDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCWorkSpaceManager::initialize(SBC_DIALOGDB) in workspace " << SBC_DIALOGDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCWorkSpaceManager::initialize(SBC_RTPDB) in workspace " << SBC_RTPDB );
  _rtpDb = WorkSpace(new SBCWorkSpace("sbc_rtp.bdb"));
  if (!connector.connect(*_rtpDb, SBC_RTPDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCWorkSpaceManager::initialize(SBC_RTPDB) in workspace " << SBC_RTPDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCWorkSpaceManager::initialize(SBC_CDRDB) in workspace " << SBC_CDRDB );
  _cdrDb = WorkSpace(new SBCWorkSpace("sbc_cdr.bdb"));
  if (!connector.connect(*_cdrDb, SBC_CDRDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCWorkSpaceManager::initialize(SBC_CDRDB) in workspace " << SBC_CDRDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCWorkSpaceManager::initialize(SBC_ACCOUNTDB) in workspace " << SBC_ACCOUNTDB );
  _accountDb = WorkSpace(new SBCWorkSpace("sbc_account.bdb"));
  if (!connector.connect(*_accountDb, SBC_ACCOUNTDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCWorkSpaceManager::initialize(SBC_ACCOUNTDB) in workspace " << SBC_ACCOUNTDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCWorkSpaceManager::initialize(SBC_LOCAL_REGDB) in workspace " << SBC_LOCAL_REGDB );
  _localRegDb = WorkSpace(new SBCWorkSpace("sbc_local_reg.bdb"));
  if (!connector.connect(*_localRegDb, SBC_LOCAL_REGDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCWorkSpaceManager::initialize(SBC_LOCAL_REGDB) in workspace " << SBC_LOCAL_REGDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCWorkSpaceManager::initialize(SBC_BANNED_ADDRESSDB) in workspace " << SBC_BANNED_ADDRESSDB );
  _bannedAddressDb = WorkSpace(new SBCWorkSpace("sbc_banned_address.bdb"));
  if (!connector.connect(*_bannedAddressDb, SBC_BANNED_ADDRESSDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCWorkSpaceManager::initialize(SBC_BANNED_ADDRESSDB) in workspace " << SBC_BANNED_ADDRESSDB << " ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[REDIS] SBCWorkSpaceManager::initialize(SBC_ROUTEDB) in workspace " << SBC_ROUTEDB );
  _routeDb = WorkSpace(new SBCWorkSpace("sbc_route.bdb"));
  if (!connector.connect(*_routeDb, SBC_ROUTEDB))
  {
    OSS_LOG_WARNING("[REDIS] SBCWorkSpaceManager::initialize(SBC_ROUTEDB) in workspace " << SBC_ROUTEDB << " ** Temporary Failure **");
  }
}


const SBCWorkSpaceManager::WorkSpace& SBCWorkSpaceManager::getWorkSpace(RedisWorkspace workSpace) const
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




