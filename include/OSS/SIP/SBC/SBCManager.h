// OSS Software Solutions Application Programmer Interface
//
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


#ifndef SIP_SBCMANAGER_INCLUDED
#define SIP_SBCMANAGER_INCLUDED

#include <set>
#include "OSS/SIP/SBC/SBC.h"
#include "OSS/SIP/SBC/SBCException.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/RTP/RTPProxyManager.h"
#include "OSS/SIP/SBC/SBCDialogStateManager.h"
#include "OSS/STUN/STUNClient.h"
#include "OSS/UTL/Cache.h"
#include "OSS/UTL/Thread.h"
#include "OSS/STUN/STUNServer.h"
#include "OSS/SIP/SBC/SBCPluginManager.h"
#include "OSS/UTL/IPCQueue.h"
#include "OSS/SIP/SBC/SBCStaticRouter.h"
#include "OSS/SIP/SBC/SBCMediaProxy.h"
#include "OSS/SIP/SBC/SBCCDRManager.h"
#include "OSS/SIP/SBC/SBCRedisManager.h"
#include "OSS/SIP/SBC/SBCRedisEventHandler.h"
#include "OSS/SIP/SBC/SBCAuthenticator.h"
#include "OSS/SIP/SBC/SBCRegistrar.h"
#include "OSS/SIP/SBC/SBCAutoBanRules.h"
#include "OSS/SIP/SBC/SBCConsole.h"
#include "OSS/SIP/SBC/SBCJSModuleManager.h"
#include "OSS/SIP/SBC/SBCJsonRpcHandler.h"


namespace OSS {
namespace SIP {
namespace SBC {

class SBCReferBehavior;
class SBCRegisterBehavior;
class SBCInviteBehavior;
class SBCPrackBehavior;


class OSS_API SBCManager
{
public:
  typedef boost::function<bool(const std::string&, std::ostringstream& output)> ExecProc;
  typedef std::map<std::string, ExecProc> ExecProcList;
  typedef std::map<std::string, OSS::UInt64> CallTimers;

  SBCManager(int minThreadCount = 2, int maxThreadCount = 1024);
    /// Create a new SBC Manager

  ~SBCManager();
    /// Destroy the SBC Manager
  
  void initialize(const boost::filesystem::path& cfgDirectory);
    /// Initialize the manager configuration using the configuration path specified.
    /// If an error occurs, this method will throw a PersistenceException.
    ///
    /// Take note that the configuration directory must be both readable and writeble
    /// by the user that owns the SBC process.

  void deinitialize();
    /// Deinitialize the manager.  This is usually called when the application
    /// is about the exit.  This is the place where the manager performs final
    /// trash management.

  void run();
    /// Starts the SBC event subsytem.
    ///
    /// This will block until a call to stop() received.

  void stop();
    /// Stops the SBC event subsytem.
    ///
    /// This function will block until both the fsm and transport
    /// subsystems have abandoned all pending work safely.
    ///
    /// All pending call to read and write methods will return with
    /// an io exception.

  OSS::SIP::B2BUA::SIPB2BTransactionManager& transactionManager();
    /// Return a reference to the transactionManager

  OSS::SIP::SBC::SBCMediaProxy& rtpProxy();
    /// Return a reference to the RTP Proxy
  
  const boost::filesystem::path& getConfigurationDirectory() const;
    /// Returns the directory where the SBC will store configuration files

  const boost::filesystem::path& getPathsConfigurationFile() const;
    /// Returns the path of the paths configuration file.
    /// The paths configuraiton file contains the location of
    /// the rest of the files and directories that the sbc will
    /// need for its operation.

  const boost::filesystem::path& getDialogStateDirectory() const;
    /// Returns the path where the SBC will store dialog states.
    /// This is normally a hidden directory since state files
    /// must never be altered by users while the sbc is running.

  const boost::filesystem::path& getRTPStateDirectory() const;
    /// Returns the path where the SBC will store rtp proxy states.
    /// This is normally a hidden directory since state files
    /// must never be altered by users while the sbc is running.

  const boost::filesystem::path& getRegistrationRecordsDirectory() const;
    /// Returns the path where the SBC will persist registration records.
    /// This is normally a hidden directory since registration records
    /// must never be altered by users while the sbc is running.
    /// Although admin may forcibly delete records if registrations
    /// are over staying.

  const boost::filesystem::path& getSIPConfigurationFile() const;
    /// Returns the sip configuration file path

  const std::string& getUserAgentName() const;
    /// Return the user agent name

  void setUserAgentName(const std::string& userAgentName);
    /// Set the user agent name

  void registerExecProc(const char* cmd, const ExecProc& proc);
    /// Register a procedure to handle cmds requested by
    /// SBCExecBehavior

  bool execProc(const std::string& cmd, const std::string& args,
    std::ostringstream& output);
    /// Execute the SBC Exec procedure and send back the output

  SBCDialogStateManager& dialogStateManager();
    /// Returns a reference to the dialog state manager

  bool onRouteByAOR(SIPMessage* pRequest, SIPB2BTransaction* pTransaction,
    bool userComparisonOnly, OSS::Net::IPAddress& localInterface, OSS::Net::IPAddress& target);
    /// This is called from the route scripts to attempt routing via the address
    /// of record of a registered user.  If userComparisonOnly is set to true,
    /// the domain/host portion of the request-uri will be ignored and only compare
    /// the user agains the registration state.   This will only allow to the first
    /// occurence of a registration state with the same AOR.  Thus, this function
    /// may result to a stale route if there are mutiple registrations using the same
    /// AOR.  Future version of this function may allow forking

  bool onRouteByRURI(SIPMessage* pRequest, SIPB2BTransaction* pTransaction);
    /// This is called from the route scripts to attempt routing by resolving 
    /// the request-uri directly.  DNS/DNS-SRV will be tried.  Future version
    /// of this function may allow forking if DNS-SRV records yields mutiple
    /// destination with the same priority.

  bool isLocalTransport(const std::string& transport, const OSS::Net::IPAddress& address);
    /// Returns true if the address is an ip:port we are binding to.  If port is zero
    /// it will be assumed as 5060

  bool isLocalInterface(const std::string& transport, const OSS::Net::IPAddress& address);
    /// Returns true if the address is an ip we are binding to.

  bool getFirstTransport(const std::string& proto, const std::string& iface, OSS::Net::IPAddress& transport);
    /// Return the first transport configured for the protocol bound to a particular interface

  bool getExternalAddress(
    const OSS::Net::IPAddress& internalIp,
    std::string& externalIp) const;
    /// Return the external interface for a given internal listener

  bool getExternalAddress(
    const std::string& proto,
    const OSS::Net::IPAddress& internalIp,
    std::string& externalIp) const;
    /// Return the external interface for a given internal listener

  bool getInternalAddress(
    const OSS::Net::IPAddress& externalIp,
    OSS::Net::IPAddress& internalIp) const;
    /// Return the internal IP if the host:port for the external IP is known

  bool getInternalAddress(
    const std::string& proto,
    const OSS::Net::IPAddress& externalIp,
    OSS::Net::IPAddress& internalIp) const;
    /// Return the internal IP if the host:port for the external IP is known
  
  const OSS::Net::IPAddress& getDefaultTransportAddress() const;
    /// Return the configured default transport address  

  bool onRetargetTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction);
    //
    // This allows the bridge to execute a retarget prior to actual route scripts being called
    //

  void onPostRouteTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction);
    //
    // Called after the route script ahs been processed
    // This allows the SBC to add extra state or to totally re-route bypassing the script
    //

  void onPostRouteMidDialogTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction);
    //
    // Called after a mid-dialog request has been routed
    //
  
  void registerDomainRouter(const std::string& domain, SIPB2BHandler::Ptr handler);
    // Register a static router for a particular domain
  
  void onLocalRegistrationStopped();
    // Called when all local registration transactions has completed after closing the UA
protected:
  void initializeRTPProxy(const boost::filesystem::path& cfgDirectory);
    /// Initialize the RTP Proxy

  void initializePaths(const boost::filesystem::path& cfgDirectory);
    /// Initialize the path configuration
    /// If an error occurs, this method will throw a PersistenceException.

  void initializeHandlers();
    /// Initialize the request handlers
    /// If an error occurs, this method will throw a PersistenceException.

  void initializeUserAgent(const boost::filesystem::path& cfgDirectory);
    /// Initialize User-Agent specific properties
 
  void initializeSTUNServer(const boost::filesystem::path& cfgDirectory);
    /// Initialize the built-in STUN server

  void initializePlugins();
    /// Initialize the SBC Plugins

  void intializeProcessMonitor(const boost::filesystem::path& cfgDirectory);
    /// Initialize the process monitoring module

public:
  void setSipConfigFile(const std::string& sipConfigFile);
    /// Set the file name of the sip configuration file.
    /// This defaults to sip.cfg

  const OSS::Net::IPAddress& getGlobalAddress() const;
    /// If a STUN server is configured, this will return the IP address of the
    /// NAT/Firewall if it is present

  bool& enableRedis();

  void reportCriticalState(const std::string& msg);

  SBCRegisterBehavior* registerHandler();

  void addTrunkRegistration(const std::string& aor);

  bool isTrunkRegistration(const std::string& aor);

  bool isOptionsRoutingEnabled() const;
  
  bool isOptionsKeepAliveEnabled() const;

  bool& requireRtpForRegistrations();
    /// If set to true, all calls to registered user agents will force
    /// RTP proxy to be enforced regardless of their NAT standing.
  
  void onDispatchTransaction(SIPB2BTransactionManager* pManager, SIPB2BTransaction* pTransaction);
    /// Schedule a transaction task to run.
  
  void markCallStartTime(const std::string& callId);
    /// Mark the time for call connect
  
  OSS::UInt64 getCallDuration(const std::string& callId) const;
    /// Get the current duration of the call in milliseconds
    
  void removeCallStartTime(const std::string& callId);
    /// Remove the call start time from the map
  
  SBCCDRManager& cdr();
    /// CDR Manager
  
  SBCRedisManager& redis();
    /// Redis Manager
  
  const boost::filesystem::path& getLogDirectory() const;
    /// Return the log directory configured for the SBC
  
  void setLogDirectory(const boost::filesystem::path& logDirectory);
    /// Set the log directory configured for the SBC
  
  const boost::filesystem::path& getTempDirectory() const;
    /// Return the temp directory configured for the SBC
  
  void setTempDirectory(const boost::filesystem::path& tempDirectory);
    /// Set the temp directory configured for the SBC
  
  
  SBCAuthenticator& authenticator();
  
  SBCRegistrar& registrar();
  
  SBCConsole& console();
  
  const std::string& getExecuteOnBanScript() const;
    /// Return the configured script to be executed when a source is blocked
    /// by the access-control layer.
  
  SBCAutoBanRules& autoBanRules();
  
  SBCReferBehavior* getReferHandler();
  SBCRegisterBehavior* getRegisterHandler();
  SBCInviteBehavior* getInviteHandler();
  SBCPrackBehavior* getPrackHandler();
  
  bool transformPreDialogPersist(const SIPMessage::Ptr& pRequest, const SIPB2BTransaction::Ptr& pTransaction);
    /// Allow the manager to change any part of the SIP message prior to determining dialog state
  
  
  SBCJSModuleManager& modules();
  
  unsigned long getMaxInvitesPerSecond() const;
  unsigned long getMaxRegistersPerSecond() const;
  unsigned long getMaxSubscribesPerSecond() const;
  
protected:
  friend class SBConfigurationMonitor;
  OSS::SIP::B2BUA::SIPB2BTransactionManager _transactionManager;
  OSS::SIP::SBC::SBCMediaProxy _rtpProxy;
  boost::filesystem::path _configurationDirectory;
  boost::filesystem::path _pathsConfigurationFile;
  boost::filesystem::path _dialogStateDirectory;
  boost::filesystem::path _rtpStateDirectory;
  boost::filesystem::path _registrationRecordsDirectory;
  boost::filesystem::path _sipConfigurationFile;
  boost::filesystem::path _logDirectory;
  boost::filesystem::path _tempDirectory;
  std::string _userAgentName;
  std::string _sipConfigFile;
  OSS::mutex_critic_sec _csDialogsMutex;
  CacheManager _dialogs;
  SBCDialogStateManager _dialogStateManager;
  ExecProcList _execProcs;
  OSS::Net::IPAddress _globalAddress;
  OSS::STUN::STUNServer _stunServer;
  bool _enableStunServer;
  SBCReferBehavior* _pReferHandler;
  SBCRegisterBehavior* _pRegisterHandler;
  SBCInviteBehavior* _pInviteHandler;
  SBCPrackBehavior* _pPrackHandler;
  SBCPluginManager _plugins;
  std::set<std::string> _trunkRegistrations;
  bool _enableOptionsRouting;
  bool _enableOptionsKeepAlive;
  SBCStaticRouter _staticRouter;
  bool _requireRtpForRegistrations;
  bool _enableBridge;
  bool _enableRedis;
  int _delayedDisconnectMinConnectTime;
  int _delayedDisconnectYieldTime;
  std::string _executeOnBanScript;
  
  //
  // CallTimer
  //
  CallTimers _callStartTime;
  mutable OSS::mutex_critic_sec _callStartTimeMutex;

  //
  // CDR Manager related 
  //
  SBCCDRManager _cdr;
  
  //
  // RedisManager
  //
  SBCRedisManager _redis;
  SBCRedisEventHandler _redisEventHandler;
   
  //
  // Authenticator for local domain transactions
  //
  SBCAuthenticator _authenticator;
  
  //
  // Registrar
  //
  SBCRegistrar _registrar;
  
  //
  // AutoBanRules
  //
  SBCAutoBanRules _autoBanRules;

  //
  // Auxiliary Socket for CLI operations
  //
  SBCConsole _console;
  
  //
  // JavaScript mnodules
  //
  SBCJSModuleManager& _jsModules;
  
  //
  //  JSON-RPC Handler
  //
  SBCJsonRpcHandler _jsonRpcHandler;
  
  //
  // Packet Rates
  //
  unsigned long _maxInvitesPerSecond;
  unsigned long _maxRegistersPerSecond;
  unsigned long _maxSubscribesPerSecond;
  

};

//
// Inlines
//

inline const std::string& SBCManager::getExecuteOnBanScript() const
{
  return _executeOnBanScript;
}

inline void SBCManager::addTrunkRegistration(const std::string& aor)
{
  _trunkRegistrations.insert(aor);
}

inline bool SBCManager::isTrunkRegistration(const std::string& aor)
{
  return _trunkRegistrations.find(aor) != _trunkRegistrations.end();
}

inline OSS::SIP::B2BUA::SIPB2BTransactionManager& SBCManager::transactionManager()
{
  return _transactionManager;
}

inline OSS::SIP::SBC::SBCMediaProxy& SBCManager::rtpProxy()
{
  return _rtpProxy;
}

inline const boost::filesystem::path& SBCManager::getConfigurationDirectory() const
{
  return _configurationDirectory;
}

inline const boost::filesystem::path& SBCManager::getPathsConfigurationFile() const
{
  return _pathsConfigurationFile;
}

inline const boost::filesystem::path& SBCManager::getDialogStateDirectory() const
{
  return _dialogStateDirectory;
}

inline const boost::filesystem::path& SBCManager::getRTPStateDirectory() const
{
  return _rtpStateDirectory;
}

inline const boost::filesystem::path& SBCManager::getRegistrationRecordsDirectory() const
{
  return _registrationRecordsDirectory;
}

inline const boost::filesystem::path& SBCManager::getSIPConfigurationFile() const
{
  return _sipConfigurationFile;
}

inline const std::string& SBCManager::getUserAgentName() const
{
  return _userAgentName;
}

inline void SBCManager::setUserAgentName(const std::string& userAgentName)
{
  _userAgentName = userAgentName;
}

inline void SBCManager::setSipConfigFile(const std::string& sipConfigFile)
{
  _sipConfigFile = sipConfigFile;
}

inline SBCDialogStateManager& SBCManager::dialogStateManager()
{
  return _dialogStateManager;
}

inline const OSS::Net::IPAddress&  SBCManager::getGlobalAddress() const
{
  return _globalAddress;
}

inline bool SBCManager::getExternalAddress(
    const OSS::Net::IPAddress& internalIp,
    std::string& externalIp) const
{
  return const_cast<SBCManager*>(this)->transactionManager().stack().transport().getExternalAddress(
    internalIp, externalIp);
}

inline bool SBCManager::getExternalAddress(
  const std::string& proto,
  const OSS::Net::IPAddress& internalIp,
  std::string& externalIp) const
{
  return const_cast<SBCManager*>(this)->transactionManager().stack().transport().getExternalAddress(
    proto, internalIp, externalIp);
}

inline bool SBCManager::getInternalAddress(
  const OSS::Net::IPAddress& externalIp,
  OSS::Net::IPAddress& internalIp) const
{
  return const_cast<SBCManager*>(this)->transactionManager().stack().transport().getInternalAddress(
   externalIp, internalIp);
}

inline bool SBCManager::getInternalAddress(
  const std::string& proto,
  const OSS::Net::IPAddress& externalIp,
  OSS::Net::IPAddress& internalIp) const
{
  return const_cast<SBCManager*>(this)->transactionManager().stack().transport().getInternalAddress(
    proto, externalIp, internalIp);
}

inline bool& SBCManager::enableRedis()
{
  return _enableRedis;
}

inline SBCRegisterBehavior* SBCManager::registerHandler()
{
  return _pRegisterHandler;
}

inline bool SBCManager::isOptionsRoutingEnabled() const
{
  return _enableOptionsRouting;
}

inline bool SBCManager::isOptionsKeepAliveEnabled() const
{
  return _enableOptionsKeepAlive;
}

inline bool& SBCManager::requireRtpForRegistrations()
{
  return _requireRtpForRegistrations;
}

inline void SBCManager::registerDomainRouter(const std::string& domain, SIPB2BHandler::Ptr handler)
{
  _transactionManager.registerDomainRouter(domain, handler);
}

inline SBCCDRManager& SBCManager::cdr()
{
  return _cdr;
}

inline SBCRedisManager& SBCManager::redis()
{
  return _redis;
}

inline const boost::filesystem::path& SBCManager::getLogDirectory() const
{
  return _logDirectory;
}
  
inline void SBCManager::setLogDirectory(const boost::filesystem::path& logDirectory)
{
  _logDirectory = logDirectory;
}
  
inline const boost::filesystem::path& SBCManager::getTempDirectory() const
{
  return _tempDirectory;
}
  
inline void SBCManager::setTempDirectory(const boost::filesystem::path& tempDirectory)
{
  _tempDirectory = tempDirectory;
}

inline SBCAuthenticator& SBCManager::authenticator()
{
  return _authenticator;
}

inline SBCRegistrar& SBCManager::registrar()
{
  return _registrar;
}

inline SBCAutoBanRules& SBCManager::autoBanRules()
{
  return _autoBanRules;
}

inline SBCReferBehavior* SBCManager::getReferHandler()
{
  return _pReferHandler;
}

inline SBCRegisterBehavior* SBCManager::getRegisterHandler()
{
  return _pRegisterHandler;
}

inline SBCInviteBehavior* SBCManager::getInviteHandler()
{
  return _pInviteHandler;
}

inline SBCPrackBehavior* SBCManager::getPrackHandler()
{
  return _pPrackHandler;
}

inline SBCConsole& SBCManager::console()
{
  return _console;
}

inline const OSS::Net::IPAddress& SBCManager::getDefaultTransportAddress() const
{
  return const_cast<SBCManager*>(this)->transactionManager().stack().transport().defaultListenerAddress();
}

inline SBCJSModuleManager& SBCManager::modules()
{
  return _jsModules;
}

inline unsigned long SBCManager::getMaxInvitesPerSecond() const
{
  return _maxInvitesPerSecond;
}

inline unsigned long SBCManager::getMaxRegistersPerSecond() const
{
  return _maxRegistersPerSecond;
}

inline unsigned long SBCManager::getMaxSubscribesPerSecond() const
{
  return _maxSubscribesPerSecond;
}

} } } /// OSS::SIP::SBC


#endif // SIP_SBCMANAGER_INCLUDED

