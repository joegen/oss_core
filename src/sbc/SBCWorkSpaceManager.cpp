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


SBCWorkSpaceManager::SBCWorkSpaceManager() :
  _isTerminating(false)
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

void SBCWorkSpaceManager::initialize()
{
  OSS_LOG_NOTICE("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_SYSTEMDB) in workspace SBC_SYSTEMDB" );
  _systemDb = WorkSpace(new SBCWorkSpace("sbc_system.bdb"));
  if (!_systemDb->open())
  {
    OSS_LOG_WARNING("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_SYSTEMDB) in workspace SBC_SYSTEMDB ** Temporary Failure **");
  }
  

  OSS_LOG_NOTICE("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_REGDB) in workspace SBC_REGDB" );
  _regDb = WorkSpace(new SBCWorkSpace("sbc_reg.bdb"));
  if (!_regDb->open())
  {
    OSS_LOG_WARNING("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_REGDB) in workspace SBC_REGDB ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_DIALOGDB) in workspace SBC_DIALOGDB" );
  _dialogDb = WorkSpace(new SBCWorkSpace("sbc_dialog.bdb"));
  if (!_dialogDb->open())
  {
    OSS_LOG_WARNING("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_DIALOGDB) in workspace SBC_DIALOGDB ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_RTPDB) in workspace SBC_RTPDB" );
  _rtpDb = WorkSpace(new SBCWorkSpace("sbc_rtp.bdb"));
  if (!_rtpDb->open())
  {
    OSS_LOG_WARNING("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_RTPDB) in workspace SBC_RTPDB ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_CDRDB) in workspace SBC_CDRDB" );
  _cdrDb = WorkSpace(new SBCWorkSpace("sbc_cdr.bdb"));
  if (!_cdrDb->open())
  {
    OSS_LOG_WARNING("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_CDRDB) in workspace SBC_CDRDB ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_ACCOUNTDB) in workspace SBC_ACCOUNTDB" );
  _accountDb = WorkSpace(new SBCWorkSpace("sbc_account.bdb"));
  if (!_accountDb->open())
  {
    OSS_LOG_WARNING("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_ACCOUNTDB) in workspace SBC_ACCOUNTDB ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_LOCAL_REGDB) in workspace SBC_LOCAL_REGDB" );
  _localRegDb = WorkSpace(new SBCWorkSpace("sbc_local_reg.bdb"));
  if (!_localRegDb->open())
  {
    OSS_LOG_WARNING("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_LOCAL_REGDB) in workspace SBC_LOCAL_REGDB ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_BANNED_ADDRESSDB) in workspace SBC_BANNED_ADDRESSDB" );
  _bannedAddressDb = WorkSpace(new SBCWorkSpace("sbc_banned_address.bdb"));
  if (!_bannedAddressDb->open())
  {
    OSS_LOG_WARNING("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_BANNED_ADDRESSDB) in workspace SBC_BANNED_ADDRESSDB ** Temporary Failure **");
  }
  
  OSS_LOG_NOTICE("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_ROUTEDB) in workspace SBC_ROUTEDB" );
  _routeDb = WorkSpace(new SBCWorkSpace("sbc_route.bdb"));
  if (!_routeDb->open())
  {
    OSS_LOG_WARNING("[WORKSPACE] SBCWorkSpaceManager::initialize(SBC_ROUTEDB) in workspace SBC_ROUTEDB ** Temporary Failure **");
  }
}


} } } // OSS::SIP::SBC




