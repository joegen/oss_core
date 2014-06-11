#include <execinfo.h>
#include <sys/resource.h>
#include <signal.h>
#include "OSS/Persistent/RESTKeyValueStore.h"
#include "OSS/ServiceDaemon.h"
#include "OSS/ServiceOptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define DEFAULT_PORT 8010
#define DEFAULT_SECURE_PORT 8011

using namespace OSS;

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

bool prepareOptions(ServiceOptions& options)
{
  options.addDaemonOptions();
  options.addOptionString("host", "The IP Address where the REST Server will listen for connections.");
  options.addOptionInt("port", "The port where the B2BUA will listen for connections.");
  options.addOptionString("secure-host", "The IP Address where the REST Server will listen for TLS connections.");
  options.addOptionInt("secure-port", "The port where the B2BUA will listen for TLS connections.");
  options.addOptionString("data-directory", "The directory where the REST Server will store data.");
  options.addOptionString("auth-user", "User for Basic/Digest authentication.");
  options.addOptionString("auth-password", "Password for Basic/Digest authentication.");
  options.addOptionString("private-key-file", "Contains the path to the private key file used for encryption");
  options.addOptionString("certificate-file", "Contains the path to the certificate file (in PEM format)");
  options.addOptionString("ca-location", "Contains the path to the file or directory containing the CA/root certificates.");
  options.addOptionFlag("secure-transport-only", "Set this flag if only TLS transport (https) will be allowed.");

  return options.parseOptions();
}

int main(int argc, char** argv)
{
  setSystemParameters();

  bool isDaemon = false;
  daemonize(argc, argv, isDaemon);

  std::set_terminate(&ServiceOptions::catch_global);
  
  OSS::OSS_init();

  ServiceOptions options(argc, argv, "oss_rest", VERSION, "Copyright (c) OSS Software Solutions");
  if (!prepareOptions(options) || options.hasOption("help"))
  {
    options.displayUsage(std::cout);
    _exit(0);
  }

  saveProcessId(options);
  
  OSS::Persistent::RESTKeyValueStore server;
  OSS::Persistent::RESTKeyValueStore server_secure(&server);
  
  std::string user;
  std::string pass;
  std::string host;
  int port = DEFAULT_PORT;
  std::string dataDir;
  options.getOption("auth-user", user);
  options.getOption("auth-password", pass);
  options.getOption("host", host);
  options.getOption("port", port, DEFAULT_PORT);
  options.getOption("data-directory", dataDir);
  
  std::string secureHost;
  int securePort = DEFAULT_SECURE_PORT;
  
  options.getOption("secure-host", secureHost);
  options.getOption("secure-port", securePort, DEFAULT_SECURE_PORT);

  
  if (!user.empty())
  {
    server.setCredentials(user, pass);
    server_secure.setCredentials(user, pass);
  }
  
  if (!dataDir.empty())
  {
    server.setDataDirectory(dataDir);
    server_secure.setDataDirectory(dataDir);
  }
  bool secureTransportOnly = options.hasOption("secure-transport-only");
  bool started = false;
  bool secure_started = false;
  
  if (!secureTransportOnly)
  {
    if (!host.empty())
      started = server.start(host, port, false);
    else
      started = server.start(port, false);
  }
   
  std::string privateKeyFile;
  std::string certificateFile;
  std::string caLocation;
  
  options.getOption("private-key-file", privateKeyFile);
  options.getOption("certificate-file", certificateFile);
  options.getOption("ca-location", caLocation);
  
  
  if (!caLocation.empty())
  {
    OSS::Net::TLSManager::instance().initialize("any.pem", "any.pem", "rootcert.pem", "secret", true, OSS::Net::TLSManager::VERIFY_RELAXED);

    if (!secureHost.empty())
      secure_started = server_secure.start(secureHost, securePort, true);
    else
      secure_started = server_secure.start(securePort, true);
  }
   
  if (started || secure_started)
  {
    OSS_LOG_NOTICE("REST Server STARTED." << std::endl
      << "\tport: " << (started ? port : -1) << std::endl
      << "\tsecure-port: " << (secure_started ? securePort : -1));
    options.waitForTerminationRequest();
  }
  
  if (started)
  {
    OSS_LOG_NOTICE("Stopping HTTP service.");
    server.stop();
  }
  
  if (secure_started)
  {
    OSS_LOG_NOTICE("Stopping HTTPS service.");
    server_secure.stop();
  }
  
  OSS::OSS_deinit();
  OSS_LOG_NOTICE("REST Server SHUTDOWN.");
  return 0;
}
