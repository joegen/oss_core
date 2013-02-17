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

#include <Poco/Exception.h>

#include "OSS/ServiceDaemon.h"
#include "OSS/DNS.h"
#include "OSS/Net.h"
#include "OSS/Exec/Command.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/B2BUA/SIPB2BScriptableHandler.h"
#include "OSS/SIP/B2BUA/SIPB2BDialogStateManager.h"

#define TCP_PORT_BASE 20000
#define TCP_PORT_MAX  30000

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


class oss_b2bua :
  public SIPB2BTransactionManager,
  public SIPB2BDialogStateManager,
  public SIPB2BScriptableHandler
{

oss_b2bua() :
  SIPB2BTransactionManager(2, 1024),
  SIPB2BDialogStateManager(dynamic_cast<SIPB2BTransactionManager*>(this)),
  SIPB2BScriptableHandler(dynamic_cast<SIPB2BTransactionManager*>(this), dynamic_cast<SIPB2BDialogStateManager*>(this))
{
}

~oss_b2bua()
{
}

bool onProcessRequest(MessageType type, const SIPMessage::Ptr& pRequest)
{
  pRequest->setProperty("route-action", "accept");
  return true;
}

}; // class oss_b2bua


inline void  daemonize(int argc, char** argv, bool& isDaemon)
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

bool prepareOptions(ServiceOptions& options)
{
  options.addOptionFlag('h', "help", "Display help information.");
  options.addOptionFlag('D', "daemonize", "Run as system daemon.");
  options.addOptionString('P', "pid-file", "PID file when running as daemon.");
  options.addOptionString('i', "interface-address", "The IP Address where the B2BUA will listener for connections.");
  options.addOptionInt('p', "port", "The port where the B2BUA will listen for connections.");
  options.addOptionString('t', "target-domain", "IP Address, Host or DNS/SRV address of your SIP Server.");
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

void prepateListenerInfo(Config& config, ServiceOptions& options)
{
  if (!options.getOption("interface-address", config.address))
  {
    //
    // Try to find the default route to the internet
    //
    
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
}

int main(int argc, char** argv)
{
  bool isDaemon = false;
  daemonize(argc, argv, isDaemon);

  OSS::OSS_init();
  
  ServiceOptions options(argc, argv, "oss_b2bua");
  if (!prepareOptions(options) || options.hasOption("help"))
  {
    options.displayUsage(std::cout);
    _exit(-1);
  }

  saveProcessId(options);

  Config config;
  prepateListenerInfo(config, options);
  prepareTargetInfo(config, options);
  OSS::OSS_deinit();
  
  return 0;
}