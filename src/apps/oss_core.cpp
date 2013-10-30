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


#include <execinfo.h>
#include <sys/resource.h>
#include <signal.h>


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


#if HAVE_CONFIG_H
#include "config.h"
#endif

#if ENABLE_TURN
#include "OSS/Net/TurnServer.h"
#endif

#define TCP_PORT_BASE 20000
#define TCP_PORT_MAX  30000
#define RTP_PROXY_THREAD_COUNT 10
#define EXTERNAL_IP_HOST_URL "myip.ossapp.com"

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
    // Initialize the transport
    //
    OSS::IPAddress listener;
    listener = _config.address;
    listener.externalAddress() = _config.externalAddress;
    listener.setPort(config.port);
    stack().udpListeners().push_back(listener);
    stack().tcpListeners().push_back(listener);


    OSS::IPAddress wsListener;
    wsListener = _config.address;
    wsListener.externalAddress() = _config.externalAddress;
    wsListener.setPort(config.port + 1000);
    stack().wsListeners().push_back(wsListener);

    stack().transport().defaultListenerAddress() = listener;
    stack().transport().setTCPPortRange(TCP_PORT_BASE, TCP_PORT_MAX);
    stack().transport().setWSPortRange(40000, 50000);
    stack().transportInit();

    registerDefaultHandler(dynamic_cast<SIPB2BHandler*>(this));
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


void setSystemParameters()
{
	srandom((unsigned int) time(0));
	setlocale(LC_ALL, "C");

	/* Ignore SIGPIPE from TCP sockets */
	signal(SIGPIPE, SIG_IGN);

	{
		struct rlimit rlim;
		if(getrlimit(RLIMIT_NOFILE, &rlim)<0) {
			perror("Cannot get system limit");
		} else {
			rlim.rlim_cur = rlim.rlim_max;
			if(setrlimit(RLIMIT_NOFILE, &rlim)<0) {
				perror("Cannot set system limit");
			}
		}
	}
}

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


std::string getExternalIp(const std::string& host, const std::string& path)
{
  std::string ip;
  OSS::dns_host_record_list targets = dns_lookup_host(host);
  if (targets.empty())
  {
    OSS_LOG_ERROR("Unable to resolve HTTP server " << host);
    return ip;
  }

  
  OSS::socket_handle sock = OSS::socket_tcp_client_create();
  if (!sock)
    return ip;

  try
  {
    std::ostringstream strm;
    strm << "GET " << path << " HTTP/1.0" << "\r\n"
      << "Host: " << host << "\r\n"
      << "User-Agent: oss_b2bua" << "\r\n\r\n";
    OSS::socket_tcp_client_connect(sock, *targets.begin(), 80, 5000);
    if (!socket_tcp_client_send_bytes(sock, strm.str().c_str(), strm.str().length()))
    {
      OSS_LOG_ERROR("Unable to send to HTTP server at " << host);
    }
    
    std::string reply;

    while (true)
    {
      char buff[256];
      int len = socket_tcp_client_receive_bytes(sock, buff, 256);
      if (!len)
        break;
      reply += std::string(buff, len);
    }

    if (!reply.empty())
    {
      OSS::SIP::SIPMessage msg(reply);
      ip = msg.getBody();
    }

  }
  catch(std::exception& e)
  {
    OSS_LOG_ERROR("Unable to connect to HTTP server at " << host << " Error: " << e.what());
  }

  if (sock)
  {
    OSS::socket_tcp_client_shutdown(sock);
    OSS::socket_free(sock);
  }

  return ip;
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
  
  options.getOption("external-address", config.externalAddress);

  if (config.externalAddress.empty() && options.hasOption("guess-external-address"))
  {
    config.externalAddress = getExternalIp(EXTERNAL_IP_HOST_URL, "/");
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

bool prepareOptions(ServiceOptions& options)
{
  options.addDaemonOptions();
  options.addOptionString('i', "interface-address", "The IP Address where the B2BUA will listen for connections.");
  options.addOptionString('x', "external-address", "The Public IP Address if the B2BUA is behind a firewall.");
  options.addOptionFlag('X', "guess-external-address", "If this flag is set, the external IP will be automatically assigned.");
  options.addOptionInt('p', "port", "The port where the B2BUA will listen for connections.");
  options.addOptionString('t', "target-address", "IP Address, Host or DNS/SRV address of your SIP Server.");
  options.addOptionFlag('r', "allow-relay", "Allow relaying of transactions towards SIP Servers other than the one specified in the target-domain.");
  options.addOptionFlag('n', "no-rtp-proxy", "Disable built in media relay.");

#if ENABLE_TURN
  options.addOptionFlag('T', "enable-turn-relay", "Run the built in turn server.");
  options.addOptionString('c', "turn-cert-file", "The certificate file to be used for TLS and DTLS.");
  options.addOptionString('k', "turn-pkey-file", "The private key file to be used for TLS and DTLS.");
#endif

  return options.parseOptions();
}

int main(int argc, char** argv)
{
  setSystemParameters();

  bool isDaemon = false;
  daemonize(argc, argv, isDaemon);

  std::set_terminate(&ServiceOptions::catch_global);

  OSS::OSS_init();

  ServiceOptions options(argc, argv, "oss_core", VERSION, "Copyright (c) OSS Software Solutions");
  if (!prepareOptions(options) || options.hasOption("help"))
  {
    options.displayUsage(std::cout);
    _exit(-1);
  }

  saveProcessId(options);
  Config config;
  prepareListenerInfo(config, options);
  prepareTargetInfo(config, options);

  try
  {
    OSSB2BUA ua(config);
    ua.run();

    if (options.hasOption("no-rtp-proxy"))
      ua.rtpProxy().disable();

#if ENABLE_TURN
    if (options.hasOption("enable-turn-relay"))
    {

      options.getOption("turn-cert-file", OSS::Net::TurnServer::instance().config().cert);
      options.getOption("turn-pkey-file", OSS::Net::TurnServer::instance().config().pkey);
      OSS::Net::TurnServer::instance().run();
    }
#endif

    OSS::app_wait_for_termination_request();
    ua.stop();
    _exit(0);
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
