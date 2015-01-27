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

#include "OSS/UTL/Application.h"
#include "OSS/UTL/ServiceDaemon.h"
#include "OSS/UTL/Thread.h"
#include "OSS/Net/DNS.h"
#include "OSS/Net/Net.h"
#include "OSS/Exec/Command.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/B2BUA/SIPB2BScriptableHandler.h"
#include "OSS/SIP/B2BUA/SIPB2BDialogStateManager.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/STUN/STUNClient.h"
#include "OSS/SIP/UA/SIPEventLoop.h"


#if HAVE_CONFIG_H
#include "config.h"
#endif

#if ENABLE_TURN
#include "OSS/Net/TurnServer.h"
#endif

#define TCP_PORT_BASE 20000
#define TCP_PORT_MAX  30000
#define RTP_PROXY_THREAD_COUNT 10
#define EXTERNAL_IP_HOST_URL "stun.ezuce.com"
#define DEFAULT_SIP_PORT 5060
#define DEFAULT_SIP_TLS_PORT 5061
#define DEFAULT_SIP_WS_PORT 5062

using namespace OSS;
using namespace OSS::SIP;
using namespace OSS::SIP::B2BUA;
using namespace OSS::Exec;


struct Config
{
  enum TargetType
  {
    UNKNOWN,
    IP,
    HOST,
    SRV
  };
  std::string address;
  std::string externalAddress;
  int port;
  int tlsPort;
  int wsPort;
  std::string target;
  std::string targetInterface;
  std::string targetExternalAddress;
  std::string targetTransport;
  TargetType targetType;
  bool allowRelay;
  int targetInterfacePort;
  std::string routeScript;
  std::string tlsCertFile;
  std::string tlsPrivateKey;
  std::string tlsPeerCa;
  std::string tlsPeerCaDirectory;
  std::string tlsCertPassword;
  std::string regUri;
  std::string regUser;
  std::string regPassword;
  
  Config() : 
    port(5060), 
    tlsPort(5061),
    wsPort(5062),
    targetType(UNKNOWN), 
    allowRelay(false),
    targetInterfacePort(0)
    {
    }
};

static OSS::SIP::UA::SIPEventLoop* gpEventLoop = 0;

void local_reg_exit_handler()
{
  gpEventLoop->stop(false, false, false);
}


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
    // Initialize route script if specified
    //
    if (!_config.routeScript.empty())
    {
      boost::filesystem::path script(_config.routeScript);
      if (boost::filesystem::exists(script))
      {
        if (!loadRouteScript(script, 0, "", ""))
        {
          OSS_LOG_ERROR("Unable to load route script " << script);
          _exit(-1);
        }
      }
      else
      {
        OSS_LOG_ERROR("File not found - " << script);
        _exit(-1);
      }
    }
    
    //
    // Initialize the transport
    //
    OSS::Net::IPAddress listener;
    listener = _config.address;
    listener.externalAddress() = _config.externalAddress;
    listener.setPort(config.port);
    stack().udpListeners().push_back(listener);
    stack().tcpListeners().push_back(listener);
    
    //
    // Initialize the local registration agent
    //
    std::ostringstream regRoute;
    regRoute << "sip:" << _config.address << ":" << _config.port;
    startLocalRegistrationAgent("oss-core", regRoute.str(), local_reg_exit_handler);
    
    if (!_config.targetInterface.empty() && _config.targetInterfacePort)
    {
      OSS::Net::IPAddress targetListener;
      targetListener = _config.targetInterface;
      targetListener.externalAddress() = _config.targetExternalAddress;
      targetListener.setPort(_config.targetInterfacePort);
      stack().udpListeners().push_back(targetListener);
      stack().tcpListeners().push_back(targetListener);
    }

    OSS::Net::IPAddress wsListener;
    wsListener = _config.address;
    wsListener.externalAddress() = _config.externalAddress;
    wsListener.setPort(config.wsPort);
    stack().wsListeners().push_back(wsListener);
    
    //
    // Check if TLS certificate files are configured
    //
    bool enableTls = !_config.tlsCertFile.empty() && boost::filesystem::exists(_config.tlsCertFile);
    
    if (enableTls)
    {
      bool verifyPeer = !_config.tlsPeerCa.empty() || !_config.tlsPeerCaDirectory.empty();
      
      if (stack().initializeTlsContext(_config.tlsCertFile, _config.tlsPrivateKey, _config.tlsCertPassword, _config.tlsPeerCa, _config.tlsPeerCaDirectory, verifyPeer))
      {
        OSS::Net::IPAddress tlsListener;
        tlsListener = _config.address;
        tlsListener.externalAddress() = _config.externalAddress;
        tlsListener.setPort(config.tlsPort);
        stack().tlsListeners().push_back(tlsListener);    
        OSS_LOG_INFO("TLS Transport initialized");
      }
    }
    

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
    
    if (!_config.regUri.empty())
    {
      //
      // sleep to make sure all the the threads have started running
      //
      OSS::thread_sleep(1000);
      OSS::SIP::SIPURI regTarget(_config.regUri);
      sendLocalRegister(
        regTarget.getUser(),
        _config.regUser,
        _config.regPassword,
        regTarget.getHost(),
        3600
      );
    }
    
    return true;
  }
  
  bool onProcessRequest(OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction,MessageType type, const SIPMessage::Ptr& pRequest)
  {
    if (type == SIPB2BScriptableHandler::TYPE_AUTH)
    {
      pRequest->setProperty(OSS::PropertyMap::PROP_AuthAction, "accept");
    }
    if (type == SIPB2BScriptableHandler::TYPE_INBOUND)
    {
      if (!_config.routeScript.empty() && !pRequest->isMidDialog())
      {
        //
        //  Set the variables so that the route script knows about our configuration
        //
        pRequest->setProperty("var_listener_ip", _config.address);
        pRequest->setProperty("var_listener_port", OSS::string_from_number<int>(_config.port));
        if (!_config.externalAddress.empty())
          pRequest->setProperty("var_listener_ip_external", _config.externalAddress);
        
        if (!_config.target.empty())
        {
          std::vector<std::string> tokens = OSS::string_tokenize(_config.target, ":");
          if (tokens.size() == 2)
          {
            pRequest->setProperty("var_default_target_address", tokens[0]);
            pRequest->setProperty("var_default_target_port", tokens[1]);
          }
          else
          {
            pRequest->setProperty("var_default_target_address", _config.target);
          }
          
          if (!_config.targetTransport.empty())
            pRequest->setProperty("var_default_target_transport", _config.targetTransport);
        }
        
        if (!_config.targetInterface.empty())
        {
          pRequest->setProperty("var_default_target_interface", _config.targetInterface);
          pRequest->setProperty("var_default_target_interface_port", OSS::string_from_number<int>(_config.targetInterfacePort));
        }
        
        if (_config.allowRelay)
        {
          pRequest->setProperty("var_allow_relay", "true");
        }
        else
        {
          pRequest->setProperty("var_allow_relay", "false");
        }
      }
    }
    else if (type == SIPB2BScriptableHandler::TYPE_ROUTE)
    {
      pRequest->setProperty(OSS::PropertyMap::PROP_RouteAction, "accept");
      std::ostringstream route;
      std::vector<std::string> tokens = OSS::string_tokenize(_config.target, ":");
      if (tokens.size() == 2)
      {
        pRequest->setProperty(OSS::PropertyMap::PROP_TargetAddress, tokens[0]);
        pRequest->setProperty(OSS::PropertyMap::PROP_TargetPort, tokens[1]);
        route << "sip:" << tokens[0] << ":" << tokens[1] << ";lr";
      }
      else
      {
        pRequest->setProperty(OSS::PropertyMap::PROP_TargetAddress, _config.target);
        route << "sip:" << _config.target << ";lr";
      }

      if (!_config.targetTransport.empty())
      {
        pRequest->setProperty(OSS::PropertyMap::PROP_TargetTransport, _config.targetTransport);
      }
      
      if (!_config.targetInterface.empty())
      {
        pRequest->setProperty(OSS::PropertyMap::PROP_InterfaceAddress, _config.targetInterface);
        pRequest->setProperty(OSS::PropertyMap::PROP_InterfacePort, OSS::string_from_number<int>(_config.targetInterfacePort));
      }
      
      SIPRoute::msgAddRoute(pRequest.get(), route.str());
    }

    
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

   ::close(STDIN_FILENO);
   ::close(STDOUT_FILENO);
   ::close(STDERR_FILENO);
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


std::string getExternalIp(const std::string& stunServer, const std::string& localAddress)
{
  OSS::Net::IPAddress localIp(localAddress);
  OSS::Net::IPAddress externalIp;
  OSS::STUN::STUNClient::getNATAddress(stunServer, localIp, externalIp);
  
  OSS_LOG_NOTICE("Detecting NAT type using STUN");
  int ntype = OSS::STUN::STUNClient::detectNATType(stunServer, localIp, 2);
  OSS_LOG_NOTICE("STUN NAT Type returned " << OSS::STUN::STUNClient::getTypeString(ntype));
  return externalIp.toString();
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
    OSS_LOG_INFO("Using default port " << DEFAULT_SIP_PORT);
    config.port = DEFAULT_SIP_PORT;
  }
  
  if (!options.getOption("tls-port", config.tlsPort))
  {
    OSS_LOG_INFO("Using default WebSocket port " <<  DEFAULT_SIP_TLS_PORT);
    config.tlsPort = DEFAULT_SIP_TLS_PORT;
  }
  
  if (!options.getOption("ws-port", config.wsPort))
  {
    OSS_LOG_INFO("Using default WebSocket port " << DEFAULT_SIP_WS_PORT);
    config.wsPort = DEFAULT_SIP_WS_PORT;
  }
  
  options.getOption("external-address", config.externalAddress);

  if (config.externalAddress.empty() && options.hasOption("guess-external-address"))
  {
    config.externalAddress = getExternalIp(EXTERNAL_IP_HOST_URL, config.address);
    OSS_LOG_NOTICE("STUN external address set to " << config.externalAddress);
  }
  
  if (options.hasOption("target-interface"))
  {
    options.getOption("target-interface", config.targetInterface);
    options.getOption("target-external-address", config.targetExternalAddress);
    options.getOption("target-interface-port", config.targetInterfacePort);
    
    if (!config.targetInterfacePort)
    {
      OSS_LOG_ERROR("target-interface-port not specified.");
      options.displayUsage(std::cout);
      _exit(-1);
    }
  }
  
  //
  // TLS cert paths
  //
  options.getOption("tls-cert", config.tlsCertFile);
  options.getOption("tls-private-key", config.tlsPrivateKey);
  options.getOption("tls-peer-ca", config.tlsPeerCa);
  options.getOption("tls-peer-ca-directory", config.tlsPeerCaDirectory);
  options.getOption("tls-password", config.tlsCertPassword);
  
}

void prepareTargetInfo(Config& config, ServiceOptions& options)
{
  if (options.getOption("target-address", config.target) && !config.target.empty())
  {
    config.allowRelay = options.hasOption("allow-relay");

    if (!options.getOption("target-transport", config.targetTransport))
    {
      OSS_LOG_INFO("target-transport is not set.  Using udp transport by default.");
    }
  }
  else
  {
    //
    // Allowing relay.  We dont have a static target set
    //
    OSS_LOG_INFO("target-address is not set.  Allowing relay by default.");
    config.allowRelay = true;
  }
  
  options.getOption("route-script", config.routeScript);
  
  options.getOption("reg-uri", config.regUri);
  options.getOption("reg-user", config.regUser);
  options.getOption("reg-pass", config.regPassword);
}

bool prepareOptions(ServiceOptions& options)
{
  options.addDaemonOptions();
  options.addOptionString('i', "interface-address", "The IP Address where the B2BUA will listen for connections.");
  options.addOptionString('x', "external-address", "The Public IP Address if the B2BUA is behind a firewall.");
  options.addOptionFlag('X', "guess-external-address", "If this flag is set, the external IP will be automatically assigned.");
  options.addOptionInt('p', "port", "The port where the B2BUA will listen for UDP and TCP connections.");
  options.addOptionInt('w', "ws-port", "The port where the B2BUA will listen for Web Socket connections.");
  options.addOptionInt('s', "tls-port", "The port where the B2BUA will listen for TLS connections.");
  options.addOptionString('t', "target-address", "IP-Address[:port], Host[:port] or DNS/SRV address of your SIP Server.");
  options.addOptionString('T', "target-interface", "IP-Address of the interface facing the target SIP Server");
  options.addOptionInt('I', "target-interface-port", "The port where the B2BUA will listen for connections.");
  options.addOptionString('X', "target-external-address", "The Public IP Address if the B2BUA is behind a firewall facing the SIP Server.");
  options.addOptionString('P', "target-transport", "Transport to be used to communicate with your SIP Server.");
  options.addOptionFlag('r', "allow-relay", "Allow relaying of transactions towards SIP Servers other than the one specified in the target-domain.");
  options.addOptionFlag('n', "no-rtp-proxy", "Disable built in media relay.");
  options.addOptionInt('R', "rtp-port-low", "Lowest port used for RTP");
  options.addOptionInt('H', "rtp-port-high", "Highest port used for RTP");
  options.addOptionString('J', "route-script", "Path for the route script");
  options.addOptionString("tls-cert", "Certificate to be used by this server.  File should be in PEM format.");
  options.addOptionString("tls-private-key", "Private Key to be used by this server.  File should be in PEM format.");
  options.addOptionString("tls-peer-ca", "Peer CA File. If the remote peer this server is connecting to uses a self signed certificate, this file is used to verify authenticity of the peer identity.");
  options.addOptionString("tls-peer-ca-directory", "Additional CA file directory. The files must be named with the CA subject name hash value.");
  options.addOptionString("tls-password", "TLS Certificate Key Password");
  options.addOptionString("reg-uri", "SIP URI representing an ITSP account.  Example:  sip:1234@mydomain.com");
  options.addOptionString("reg-user", "User credential to be used for registration");
  options.addOptionString("reg-pass", "Password credential to be used for registration");
  

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
    //
    // Run the UA event loop
    //
    OSS::SIP::UA::SIPEventLoop eventLoop;
    gpEventLoop = &eventLoop;
    eventLoop.run(true);
    
    OSSB2BUA ua(config);
    ua.run();

    if (options.hasOption("no-rtp-proxy"))
    {
      ua.rtpProxy().disable();
    }
    else if (options.hasOption("rtp-port-low") && options.hasOption("rtp-port-high"))
    {
      int portLow = 0;
      int portHigh = 0;
      
      options.getOption("rtp-port-low", portLow);
      options.getOption("rtp-port-high", portHigh);
      
      if (portLow >= portHigh)
      {
        OSS_LOG_ERROR("Invalid RTP port range");
        options.displayUsage(std::cout);
        _exit(-1);
      }
      
      OSS_LOG_INFO("Setting RTP port range " << portLow << "-" << portHigh);
      
      ua.rtpProxy().setUdpPortBase(portLow);
      ua.rtpProxy().setUdpPortMax(portHigh);
    }

#if ENABLE_TURN
    if (options.hasOption("enable-turn-relay"))
    {

      options.getOption("turn-cert-file", OSS::Net::TurnServer::instance().config().cert);
      options.getOption("turn-pkey-file", OSS::Net::TurnServer::instance().config().pkey);
      OSS::Net::TurnServer::instance().run();
    }
#endif

    OSS::app_wait_for_termination_request();
    
    //
    // Stop the SBC
    //
    ua.stop();
    
    //
    // Stop the registration agent
    //
    ua.stopLocalRegistrationAgent();
   
    
    //
    // Force exit.  We have freed whatever we need to free at this point
    //
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
