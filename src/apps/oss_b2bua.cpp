// OSS Software Solutions Application Programmer Interface
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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

#include <execinfo.h>
#include <Poco/Exception.h>

#include "OSS/Application.h"
#include "OSS/ServiceDaemon.h"
#include "OSS/Thread.h"
#include "OSS/DNS.h"
#include "OSS/Net.h"
#include "OSS/Exec/Command.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/B2BUA/SIPB2BScriptableHandler.h"
#include "OSS/SIP/B2BUA/SIPB2BDialogStateManager.h"

#define TCP_PORT_BASE 20000
#define TCP_PORT_MAX  30000
#define RTP_PROXY_THREAD_COUNT 10

using namespace OSS;
using namespace OSS::SIP;
using namespace OSS::SIP::B2BUA;
using namespace OSS::Exec;


struct Config
{
  enum TargetType
  {
    IP,
    HOST,
    SRV
  };
  std::string address;
  std::string externalAddress;
  int port;
  std::string target;
  TargetType targetType;
  bool allowRelay;
  Config() : port(5060){}
};


class OSSB2BUA :
  public SIPB2BTransactionManager,
  public SIPB2BDialogStateManager,
  public SIPB2BScriptableHandler
{
public:
  typedef std::map<std::string, std::string> Storage;

  Config& _config;
  Storage _dialogs;
  Storage _registry;
  mutex_critic_sec _storageMutex;


  OSSB2BUA(Config& config) :
    SIPB2BTransactionManager(2, 1024),
    SIPB2BDialogStateManager(dynamic_cast<SIPB2BTransactionManager*>(this)),
    SIPB2BScriptableHandler(dynamic_cast<SIPB2BTransactionManager*>(this), dynamic_cast<SIPB2BDialogStateManager*>(this)),
    _config(config)
  {
    //
    // Register Datastore functions
    //
    datastore().persist = boost::bind(&OSSB2BUA::dbPersist, this, _1);
    datastore().getAll = boost::bind(&OSSB2BUA::dbGetAll, this, _1);
    datastore().removeSession = boost::bind(&OSSB2BUA::dbRemoveSession, this, _1);
    datastore().removeAllDialogs = boost::bind(&OSSB2BUA::dbRemoveAllDialogs, this, _1);
    datastore().persistReg = boost::bind(&OSSB2BUA::dbPersistReg, this, _1);
    datastore().getOneReg = boost::bind(&OSSB2BUA::dbGetOneReg, this, _1, _2);
    datastore().getReg = boost::bind(&OSSB2BUA::dbGetReg, this, _1, _2);
    datastore().removeReg = boost::bind(&OSSB2BUA::dbRemoveReg, this, _1);
    datastore().removeAllReg = boost::bind(&OSSB2BUA::dbRemoveAllReg, this, _1);
    datastore().getAllReg = boost::bind(&OSSB2BUA::dbGetAllReg, this, _1);
    //
    // Initialize the transport
    //
    OSS::IPAddress listener;
    listener = _config.address;
    listener.externalAddress() = _config.externalAddress;
    listener.setPort(config.port);
    stack().udpListeners().push_back(listener);
    stack().tcpListeners().push_back(listener);
    stack().transport().defaultListenerAddress() = listener;
    stack().transport().setTCPPortRange(TCP_PORT_BASE, TCP_PORT_MAX);
    stack().transportInit();

    registerDefaultHandler(dynamic_cast<SIPB2BHandler*>(this));
  }

  bool dbPersist(const DialogData& dialogData)
  {
    mutex_critic_sec_lock lock(_storageMutex);
    std::string data;
    dialogData.toJsonString(data);
    _dialogs[dialogData.sessionId] = data;
    return true;
  }
  void dbGetAll(DialogList& dialogs)
  {
    mutex_critic_sec_lock lock(_storageMutex);
    for (Storage::const_iterator iter = _dialogs.begin(); iter != _dialogs.end(); iter++)
    {
      DialogData dialog;
      dialog.fromJsonString(iter->second);
      dialogs.push_back(dialog);
    }
  }

  void dbRemoveSession(const std::string& sessionId)
  {
    mutex_critic_sec_lock lock(_storageMutex);
    _dialogs.erase(sessionId);
  }

  void dbRemoveAllDialogs(const std::string& callId)
  {
    std::vector<std::string> deleteThese;
    DialogList dialogs;
    dbGetAll(dialogs);

    //
    // Only lock the mutex after dbGetAll (it's non-recursive!)
    //
    mutex_critic_sec_lock lock(_storageMutex);
    for (DialogList::const_iterator iter = dialogs.begin(); iter != dialogs.end(); iter++)
      if (iter->leg1.callId == callId)
        deleteThese.push_back(iter->sessionId);

    for (std::vector<std::string>::const_iterator iter = deleteThese.begin(); iter != deleteThese.end(); iter++)
      _dialogs.erase(*iter);
  }

  bool dbPersistReg(const RegData& regData)
  {
    mutex_critic_sec_lock lock(_storageMutex);
    if (regData.key.empty() || regData.aor.empty() || regData.contact.empty())
    {
      OSS_LOG_ERROR("Invalid registration record.");
      return false;
    }
    std::string data;
    regData.toJsonString(data);
    OSS_LOG_INFO("Persisting registration " << regData.key << " for AOR: " << regData.aor << " Binding: " << regData.contact);
    _registry[regData.key] = data;
    return true;
  }

  bool dbGetOneReg(const std::string& regId, RegData& regData)
  {
    mutex_critic_sec_lock lock(_storageMutex);
    OSS_LOG_INFO("OSSB2BUA::getOneReg " << regId);
    std::string data;
    if (_registry.find(regId) == _registry.end())
    {
      OSS_LOG_INFO("Unable to find registration for " << regId );
      return false;
    }
    OSS_LOG_INFO("OSSB2BUA::getOneReg " << regId << " FOUND");
    data = _registry[regId];
    regData.fromJsonString(data);
    OSS_LOG_INFO("Found registration for " << regData.key << " AOR: " << regData.aor << " Binding: " << regData.contact);
    return true;
  }

  bool dbGetReg(const std::string& regIdPrefix, RegList& regData)
  {
    mutex_critic_sec_lock lock(_storageMutex);
    for (Storage::const_iterator iter = _registry.begin(); iter != _registry.end(); iter++)
    {
      if (OSS::string_starts_with(iter->first, regIdPrefix.c_str()))
      {
        RegData data;
        data.fromJsonString(iter->second);
        regData.push_back(data);
      }
    }
    return true;
  }

  void dbRemoveReg(const std::string& regId)
  {
    mutex_critic_sec_lock lock(_storageMutex);
    _registry.erase(regId);
  }

  void dbRemoveAllReg(const std::string& regIdPrefix)
  {
    std::vector<std::string> deleteThese;
    RegList regs;
    dbGetReg(regIdPrefix, regs);
    //
    // Only lock the mutex after dbGetReg (it's non-recursive!)
    //
    mutex_critic_sec_lock lock(_storageMutex);
    for (RegList::const_iterator iter = regs.begin(); iter != regs.end(); iter++)
        deleteThese.push_back(iter->key);
    for (std::vector<std::string>::const_iterator iter = deleteThese.begin(); iter != deleteThese.end(); iter++)
      _registry.erase(*iter);
  }

  void dbGetAllReg(RegList& regs)
  {
    mutex_critic_sec_lock lock(_storageMutex);
    for (Storage::const_iterator iter = _registry.begin(); iter != _registry.end(); iter++)
    {
      RegData data;
      data.fromJsonString(iter->second);
      regs.push_back(data);
    }
  }

  bool run()
  {
    stack().run();
    dynamic_cast<SIPB2BDialogStateManager*>(this)->run();
    dynamic_cast<SIPB2BScriptableHandler*>(this)->rtpProxy().run(RTP_PROXY_THREAD_COUNT);
    return true;
  }

  bool onProcessRequest(MessageType type, const SIPMessage::Ptr& pRequest)
  {
    pRequest->setProperty("route-action", "accept");
    pRequest->setProperty("auth-action", "accept");
    return true;
  }

}; // class OSSB2BUA


void  daemonize(int argc, char** argv, bool& isDaemon)
{
  for (int i = 0; i < argc; i++)
  {
    std::string arg = argv[i];
    if (arg == "-D" || arg == "--daemonize")
    {
      isDaemon = true;
      break;
    }
  }

  if (isDaemon)
  {
     int pid = 0;
   if(getppid() == 1)
     return;
   pid=fork();
   if (pid<0) exit(1); /* fork error */
   if (pid>0) exit(0); /* parent exits */
   /* child (daemon) continues */
   setsid(); /* obtain a new process group */

   for (int descriptor = getdtablesize();descriptor >= 0;--descriptor)
   {
     close(descriptor); /* close all descriptors we have inheritted from parent*/
   }

   int h = open("/dev/null",O_RDWR); h = dup(h); h = dup(h); /* handle standard I/O */

   ::close(STDIN_FILENO);
  }
}

static void globalCatch()
{
#define catch_global_print(msg)  \
  std::ostringstream bt; \
  bt << msg << std::endl; \
  void* trace_elems[20]; \
  int trace_elem_count(backtrace( trace_elems, 20 )); \
  char** stack_syms(backtrace_symbols(trace_elems, trace_elem_count)); \
  for (int i = 0 ; i < trace_elem_count ; ++i ) \
    bt << stack_syms[i] << std::endl; \
  OSS_LOG_CRITICAL(bt.str().c_str()); \
  std::cerr << bt.str().c_str(); \
  free(stack_syms);

  try
  {
      throw;
  }
  catch (std::string& e)
  {
    catch_global_print(e.c_str());
  }
  catch (boost::exception& e)
  {
    catch_global_print(diagnostic_information(e).c_str());
  }
  catch (std::exception& e)
  {
    catch_global_print(e.what());
  }
  catch (...)
  {
    catch_global_print("Error occurred. Unknown exception type.");
  }

  std::abort();
}

bool prepareOptions(ServiceOptions& options)
{
  options.addOptionFlag('h', "help", "Display help information.");
  options.addOptionFlag('D', "daemonize", "Run as system daemon.");
  options.addOptionString('P', "pid-file", "PID file when running as daemon.");

  options.addOptionInt('l', "log-level",
      "Specify the application log priority level."
      "Valid level is between 0-7.  "
      "0 (EMERG) 1 (ALERT) 2 (CRIT) 3 (ERR) 4 (WARNING) 5 (NOTICE) 6 (INFO) 7 (DEBUG)");
  options.addOptionString('L', "log-file", "Specify the application log file.");

  options.addOptionString('i', "interface-address", "The IP Address where the B2BUA will listener for connections.");
  options.addOptionInt('p', "port", "The port where the B2BUA will listen for connections.");
  options.addOptionString('t', "target-address", "IP Address, Host or DNS/SRV address of your SIP Server.");
  options.addOptionFlag('r', "allow-relay", "Allow relaying of transactions towards SIP Servers other than the one specified in the target-domain.");
  return options.parseOptions();
}

void saveProcessId(ServiceOptions& options)
{
  std::string pidFilePath;
  if (options.getOption("pid-file", pidFilePath))
  {
    if (!pidFilePath.empty())
    {
      std::ofstream ostr(pidFilePath.c_str());
      if (ostr.good())
        ostr << getpid() << std::endl;
    }
  }
}

bool ipRouteGet(const std::string& destination, std::string& source, std::string& gateway)
{
  if (destination.empty())
    return false;
  
  std::ostringstream cmd;
  
  cmd << OSS_IP_ROUTE_2 << " route get " << destination;
  
  Command command;
  command.execute(cmd.str());
  if (!command.isGood())
    return false;
  
  std::string result;
  while (command.isGood() && !command.isEOF())
  {
    result.push_back(command.readChar());
  }
  
  std::vector<std::string> tokens = OSS::string_tokenize(result, " ");
  bool foundSrc = false;
  for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
  {
    if (foundSrc)
    {
      source = *iter;
      break;
    }
    foundSrc = (*iter == "src"); 
  }
  
  bool foundGw = false;
  for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
  {
    if (foundGw)
    {
      gateway = *iter;
      break;
    }
    foundGw = (*iter == "via"); 
  }
  
  return !source.empty() && !gateway.empty();
}

bool testListen(const Config& config)
{
  OSS::socket_handle tcpSock = OSS::socket_tcp_server_create();
  if (!tcpSock)
    return false;
  try
  {
    OSS::socket_tcp_server_bind(tcpSock, config.address, config.port, false);
  }
  catch(std::exception& e)
  {
    OSS_LOG_ERROR(e.what());
    OSS::socket_free(tcpSock);
    return false;
  }
  catch(...)
  {
    OSS_LOG_ERROR("Unknown socket bind exception.");
    OSS::socket_free(tcpSock);
    return false;
  }
  OSS::socket_free(tcpSock);
  
  OSS::socket_handle udpSock = OSS::socket_udp_create();
  if (!udpSock)
    return false;
  try
  {
    OSS::socket_udp_bind(udpSock, config.address, config.port, false);
  }
  catch(std::exception& e)
  {
    OSS_LOG_ERROR(e.what());
    OSS::socket_free(udpSock);
    return false;
  }
  catch(...)
  {
    OSS_LOG_ERROR("Unknown socket bind exception.");
    OSS::socket_free(udpSock);
    return false;
  }
  OSS::socket_free(udpSock);
  return true;
}

void prepareListenerInfo(Config& config, ServiceOptions& options)
{
  if (!options.getOption("interface-address", config.address))
  {
    //
    // Try to find the default route to the internet
    //
    try
    {
      dns_host_record_list hosts = dns_lookup_host("ossapp.com");
      if (hosts.empty())
      {
        OSS_LOG_ERROR("You must provide value for interface-address.  Unable to get a connection to the internet.");
        options.displayUsage(std::cout);
        _exit(-1);
      }
      std::string address;
      std::string gateway;
      if (!ipRouteGet(*hosts.begin(), address, gateway))
      {
        OSS_LOG_ERROR("You must provide value for interface-address.  Unable to get default interface.");
        options.displayUsage(std::cout);
        _exit(-1);
      }
      else
      {
        OSS_LOG_INFO("Using default address " << address << "." );
        config.address = address;
      }
    }
    catch(std::exception& e)
    {
      OSS_LOG_ERROR("Exception occured while preparing listener information: " << e.what());
      options.displayUsage(std::cout);
      _exit(-1);
    }
    catch(...)
    {
      OSS_LOG_ERROR("Exception occured while preparing listener information: Unknown exception.");
      options.displayUsage(std::cout);
      _exit(-1);
    }
  }
  
  if (!options.getOption("port", config.port))
  {
    OSS_LOG_INFO("Using default port 5060.");
    config.port = 5060;
  }
  
  if (!testListen(config))
  {
    OSS_LOG_FATAL("Unable to bind to interface " << config.address << ":" << config.port << "!");
    _exit(-1);
  }
}

void prepareTargetInfo(Config& config, ServiceOptions& options)
{
  if (options.getOption("target-address", config.target) && !config.target.empty())
  {
    config.allowRelay = options.hasOption("allow-relay");
  }
  else
  {
    //
    // Allowing relay.  We dont have a static target set
    //
    OSS_LOG_INFO("target-address is not set.  Allowing relay by default.");
    config.allowRelay = true;
  }
}

void prepareLogger(ServiceOptions& options)
{
  std::string logFile;
  int priorityLevel = 6;
  bool compress = true;
  int purgeCount = 7;
  std::string pattern = "%h-%M-%S.%i: %t";

  if (options.getOption("log-file", logFile) && !logFile.empty())
  {
    if (!options.getOption("log-level", priorityLevel))
      priorityLevel = 6;
    OSS::logger_init(logFile, (OSS::LogPriority)priorityLevel, pattern, compress ? "true" : "false", boost::lexical_cast<std::string>(purgeCount));
  }
}

int main(int argc, char** argv)
{
  bool isDaemon = false;
  daemonize(argc, argv, isDaemon);

  std::set_terminate(globalCatch);

  OSS::OSS_init();

  ServiceOptions options(argc, argv, "OSSB2BUA");
  if (!prepareOptions(options) || options.hasOption("help"))
  {
    options.displayUsage(std::cout);
    _exit(-1);
  }

  saveProcessId(options);
  prepareLogger(options);
  Config config;
  prepareListenerInfo(config, options);
  prepareTargetInfo(config, options);

  try
  {
    OSSB2BUA ua(config);
    ua.run();
    OSS::app_wait_for_termination_request();
  }
  catch(std::exception& e)
  {
    OSS_LOG_CRITICAL("Fatal Exception: " << e.what());
  }
  catch(...)
  {
    OSS_LOG_CRITICAL("Unknown Fatal Exception.");
  }

  
  OSS::OSS_deinit();
  return 0;
}