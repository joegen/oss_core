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
#include "OSS/UTL/Console.h"
#include "OSS/Net/DNS.h"
#include "OSS/Net/Net.h"
#include "OSS/Net/Carp.h"
#include "OSS/Exec/Command.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/B2BUA/SIPB2BScriptableHandler.h"
#include "OSS/SIP/B2BUA/SIPB2BDialogStateManager.h"
#include "OSS/SIP/SIPRoute.h"
#include "OSS/STUN/STUNClient.h"
#include "OSS/SIP/UA/SIPEventLoop.h"


#if HAVE_CONFIG_H
#include "config.h"
#include "net/carp/globals.h"
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
  std::string targetDomain;
  TargetType targetType;
  bool allowRelay;
  int targetInterfacePort;
  std::string routeScript;
  std::string tlsCertFile;
  std::string tlsPrivateKey;
  std::string tlsPeerCa;
  std::string tlsPeerCaDirectory;
  std::string tlsCertPassword;
  std::string carpVirtualIp;
  std::string carpInterface;
  std::string carpUpScript;
  std::string carpDownScript;
  bool rewriteCallId;
  bool tlsVerifyPeer;
  int testLoopbackIterationCount;
  std::string testLoopbackTargetUri;
  
  Config() : 
    port(5060), 
    tlsPort(5061),
    wsPort(5062),
    targetType(UNKNOWN), 
    allowRelay(false),
    targetInterfacePort(0),
    rewriteCallId(false),
    tlsVerifyPeer(false),
    testLoopbackIterationCount(0)
    {
    }
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
  bool _carpEnabled;

  OSSB2BUA(Config& config) :
    SIPB2BTransactionManager(2, 1024),
    SIPB2BDialogStateManager(dynamic_cast<SIPB2BTransactionManager*>(this)),
    SIPB2BScriptableHandler(dynamic_cast<SIPB2BTransactionManager*>(this), dynamic_cast<SIPB2BDialogStateManager*>(this)),
    _config(config),
    _carpEnabled(false)
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
    // Check if CARP is enabled
    //
    
    if (!_config.carpVirtualIp.empty() && 
      !_config.carpUpScript.empty() && 
      !_config.carpDownScript.empty() &&
      !_config.carpInterface.empty())
    {
      OSS::Net::Carp::Config& carpConfig = OSS::Net::Carp::instance()->config();
      
      std::string virtualIp(_config.carpVirtualIp);
      std::string subnet("24");
      std::vector<std::string> virtualIpTokens = OSS::string_tokenize(virtualIp, "/");
      if (virtualIpTokens.size() == 2)
      {
        virtualIp = virtualIpTokens[0];
        subnet = virtualIpTokens[1];
      }
      
      carpConfig.interface = _config.carpInterface;
      carpConfig.addr = virtualIp;
      carpConfig.subnet = subnet;
      carpConfig.srcip = _config.address;
      carpConfig.upscript = _config.carpUpScript;
      carpConfig.downscript = _config.carpDownScript;
      carpConfig.vhid = 222;
      carpConfig.pass = "oss_core";
      _carpEnabled = true;
  
      
      
      //
      // Initialize the Virtual Transport
      //
      std::vector<std::string> addrTokens = OSS::string_tokenize(_config.carpVirtualIp, "/");
      if (!addrTokens.empty())
      {
        OSS::Net::IPAddress listener;
        listener = addrTokens[0];
        listener.externalAddress() = _config.externalAddress;
        listener.setPort(config.port);
        listener.setVirtual(true);
        listener.alias() = "external";
        stack().udpListeners().push_back(listener);
        stack().tcpListeners().push_back(listener);
        listener.setPort(config.wsPort);
        stack().wsListeners().push_back(listener);
      }
    }
    
    //
    // Initialize the transport
    //
    OSS::Net::IPAddress listener;
    listener = _config.address;
    listener.externalAddress() = _config.externalAddress;
    listener.setPort(config.port);
    listener.alias() = "external";
    stack().udpListeners().push_back(listener);
    stack().tcpListeners().push_back(listener);
    stack().transport().defaultListenerAddress() = listener;

    OSS::Net::IPAddress wsListener;
    wsListener = _config.address;
    wsListener.externalAddress() = _config.externalAddress;
    wsListener.setPort(config.wsPort);
    wsListener.alias() = "external";
    stack().wsListeners().push_back(wsListener);
    
    //
    // Check if TLS certificate files are configured
    //
    bool enableTls = !_config.tlsCertFile.empty() && boost::filesystem::exists(_config.tlsCertFile);
    
    if (enableTls)
    {
      bool verifyPeer = (config.tlsVerifyPeer) && (!_config.tlsPeerCa.empty() || !_config.tlsPeerCaDirectory.empty());
      
      if (stack().initializeTlsContext(_config.tlsCertFile, _config.tlsPrivateKey, _config.tlsCertPassword, _config.tlsPeerCa, _config.tlsPeerCaDirectory, verifyPeer))
      {
        OSS::Net::IPAddress tlsListener;
        tlsListener = _config.address;
        tlsListener.externalAddress() = _config.externalAddress;
        tlsListener.setPort(config.tlsPort);
        tlsListener.alias() = "external";
        stack().tlsListeners().push_back(tlsListener);    
        OSS_LOG_INFO("TLS Transport initialized");
      }
    }
        
    //
    // Initialize the transport facing the PBX
    //
    if (!_config.targetInterface.empty() && _config.targetInterfacePort)
    {
      OSS::Net::IPAddress targetListener;
      targetListener = _config.targetInterface;
      targetListener.externalAddress() = _config.targetExternalAddress;
      targetListener.setPort(_config.targetInterfacePort);
      targetListener.alias() = "internal";
      stack().udpListeners().push_back(targetListener);
      stack().tcpListeners().push_back(targetListener);
    }
    
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
    
    if (_carpEnabled)
    {
      //
      // Run the CARP subsystem
      //
      OSS::Net::Carp::setStateChangeHandler(boost::bind(&OSSB2BUA::handleCarpState, this, _1));
      OSS::Net::Carp::instance()->run();
    }
    
    return true;
  }
  
  void deinit()
  {
    if (_carpEnabled)
    {
      OSS::Net::Carp::signal_exit();
    }
  }
  
  //
  // route calls coming from the external->internal
  //
  bool onRouteExternalCall(OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction, const SIPMessage::Ptr& pRequest)
  {
    std::string target = _config.target;
    SIPURI testLoopbackUri;
    if (!_config.testLoopbackTargetUri.empty())
    {
      testLoopbackUri = _config.testLoopbackTargetUri;
    }
    //
    // Check if loopback mode is in effect
    //
    if (_config.testLoopbackIterationCount && !_config.testLoopbackTargetUri.empty())
    {
      if (this->rtpProxy().getSessionCount() > (unsigned int)_config.testLoopbackIterationCount)
      {
        //
        // Iteration count is reached.  retarget the request to the final destination
        //
        target = testLoopbackUri.getHostPort();
        SIPRequestLine rline(pRequest->startLine());
        rline.setURI(testLoopbackUri);
        pRequest->startLine() = rline.data();
      }
    }
    else
    {
      //
      // If target domain is not empty, use it as the host:port
      //
      if (!_config.targetDomain.empty())
      {
        SIPRequestLine rline(pRequest->startLine());
        SIPURI ruri;
        rline.getURI(ruri);
        ruri.setHostPort(_config.targetDomain.c_str());
        rline.setURI(ruri);
        pRequest->startLine() = rline.data();
      }
    }

    pRequest->setProperty(OSS::PropertyMap::PROP_RouteAction, "accept");
    std::ostringstream route;
    std::vector<std::string> tokens = OSS::string_tokenize(target, ":");
    if (tokens.size() == 1)
    {
      pRequest->setProperty(OSS::PropertyMap::PROP_TargetAddress, target);
      pRequest->setProperty(OSS::PropertyMap::PROP_TargetPort, "5060");
      route << "sip:" << target << ":" << "5060" << ";lr";
    }
    else if (tokens.size() == 2)
    {
      pRequest->setProperty(OSS::PropertyMap::PROP_TargetAddress, tokens[0]);
      pRequest->setProperty(OSS::PropertyMap::PROP_TargetPort, tokens[1]);
      route << "sip:" << tokens[0] << ":" << tokens[1] << ";lr";
    }
    else if (tokens.size() == 3)
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

    //
    // Check if rewrite call-id is set.
    // The default behavior is to use the same call-id
    // for both inbound and outbound legs
    //
    if (_config.rewriteCallId && pRequest->isRequest("INVITE"))
    {
      const std::string& callId = pRequest->hdrGet(OSS::SIP::HDR_CALL_ID);
      unsigned int hash = OSS::string_to_js_hash(callId);
      std::ostringstream strm;
      strm << hash << "-" << OSS::string_create_uuid();
      pRequest->hdrSet(OSS::SIP::HDR_CALL_ID, strm.str());
    }

    SIPRoute::msgAddRoute(pRequest.get(), route.str());
      
    return true;;
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
      std::string transportAlias;
      if (!pRequest->getProperty(OSS::PropertyMap::PROP_TransportAlias, transportAlias) || transportAlias.empty())
      {
        pRequest->setProperty(OSS::PropertyMap::PROP_RouteAction, "reject");
        return true;
      }
      
      if (transportAlias == "external")
      {
        return onRouteExternalCall(pTransaction, pRequest);
      }
      else
      {
        //
        // We do not do routing from internal->external yet
        //
        pRequest->setProperty(OSS::PropertyMap::PROP_RouteAction, "reject");
        return true;
      }

    }

    
    return true;
  }
  
  void handleCarpState(int state)
  {
    if (state == OSS::Net::Carp::MASTER)
    {
      stack().transport().runVirtualTransports();
    }
    else
    {
      stack().transport().stopVirtualTransports();
    }
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

void  fork_me(int argc, char** argv, bool& isDaemon)
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
    config.address = OSS::Net::IPAddress::getDefaultAddress().toString();
    OSS_LOG_INFO("Using default address " << config.address);
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
  
  //
  // CARP HA
  //
  options.getOption("carp-virtual-ip", config.carpVirtualIp);
  options.getOption("carp-interface", config.carpInterface);
  options.getOption("carp-up-script", config.carpUpScript);
  options.getOption("carp-down-script", config.carpDownScript);
  
}

void prepareTargetInfo(Config& config, ServiceOptions& options)
{
  if (options.getOption("target-address", config.target) && !config.target.empty())
  {
    config.allowRelay = options.hasOption("allow-relay");
    options.getOption("target-domain", config.targetDomain);
    
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

  //
  // Check if we need to rewrite the call-id.
  // The default is to reuse the call-id
  // sent by the caller
  //
  config.rewriteCallId = options.hasOption("rewrite-call-id");

  //
  // Check if we need to verify certificates
  //
  config.tlsVerifyPeer = options.hasOption("tls-verify-peer");
  
  //
  // Check if loopback test is enabled
  //
  if (options.hasOption("test-loopback-target-uri") && options.hasOption("test-loopback-iteration-count"))
  {
    options.getOption("test-loopback-iteration-count", config.testLoopbackIterationCount);
    options.getOption("test-loopback-target-uri", config.testLoopbackTargetUri);
    OSS_LOG_INFO("Loop Back Test Enabled:" 
      << " iteration-count=" << config.testLoopbackIterationCount 
      << " target-uri=" << config.testLoopbackTargetUri);
  }
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
  options.addOptionString("target-domain", "Domain to be used in the request uri for sending calls to the SIP server");
  options.addOptionFlag('r', "allow-relay", "Allow relaying of transactions towards SIP Servers other than the one specified in the target-domain.");
  options.addOptionFlag('n', "no-rtp-proxy", "Disable built in media relay.");
  options.addOptionInt('R', "rtp-port-low", "Lowest port used for RTP");
  options.addOptionInt('H', "rtp-port-high", "Highest port used for RTP");
  options.addOptionString('J', "route-script", "Path for the route script");
  options.addOptionFlag("rewrite-call-id", "Use a different call-id for outbound legs");
  options.addOptionFlag("test-loopback-iteration-count", "Emulate traffic by looping the call back to the sender");
  options.addOptionFlag("test-loopback-target-uri", "Final URI where the loopback will be sent after the designated iteration has completed");
  options.addOptionString("tls-cert", "Certificate to be used by this server.  File should be in PEM format.");
  options.addOptionString("tls-private-key", "Private Key to be used by this server.  File should be in PEM format.");
  options.addOptionString("tls-peer-ca", "Peer CA File. If the remote peer this server is connecting to uses a self signed certificate, this file is used to verify authenticity of the peer identity.");
  options.addOptionString("tls-peer-ca-directory", "Additional CA file directory. The files must be named with the CA subject name hash value.");
  options.addOptionString("tls-password", "TLS Certificate Key Password");
  options.addOptionFlag("tls-verify-peer", "Verify TLS peer certificates");
  options.addOptionString("carp-virtual-ip", "Virtual IP assigned for CARP. Take note that this requires root permissions to work.");
  options.addOptionString("carp-interface", "Interface where the virtual IP will be registered.  Example: eth0");
  options.addOptionString("carp-up-script", "Script called to bring up the virtual interface");
  options.addOptionString("carp-down-script", "Script called to bring up the virtual interface");
  
  return options.parseOptions();
}

int main(int argc, char** argv)
{
  setSystemParameters();

  bool isDaemon = false;
  fork_me(argc, argv, isDaemon);

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
    SIPB2BContact::_dialogStateInParams = true;
    SIPB2BContact::_registerStateInParams = false;

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
      
      if (config.testLoopbackIterationCount && !config.testLoopbackTargetUri.empty())
      {
        ua.rtpProxy().alwaysProxyMedia(true);
      }
    }
    
    ua.rtpProxy().enableHairpins() = true;

#if ENABLE_TURN
    if (options.hasOption("enable-turn-relay"))
    {

      options.getOption("turn-cert-file", OSS::Net::TurnServer::instance().config().cert);
      options.getOption("turn-pkey-file", OSS::Net::TurnServer::instance().config().pkey);
      OSS::Net::TurnServer::instance().run();
    }
#endif

    if (isDaemon)
    {
      OSS::app_wait_for_termination_request();
    }
    else
    {
      OSS::UTL::Console::registerCompletion("/q", "/quit");
      while(true)
      {
        std::string line = OSS::UTL::Console::prompt("oss_core> ");
        if (line == "/quit")
        {
          break;
        }
      }
    }
    
    //
    // Deinit
    //
    ua.deinit();
    
    //
    // Stop the SBC
    //
    ua.stop();
    
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
