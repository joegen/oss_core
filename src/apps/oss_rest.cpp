#include <execinfo.h>
#include <sys/resource.h>
#include <signal.h>
#include "OSS/Persistent/RESTKeyValueStore.h"
#include "OSS/ServiceDaemon.h"
#include "OSS/ServiceOptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

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
  options.addOptionString('i', "host", "The IP Address where the REST Server will listen for connections.");
  options.addOptionInt('p', "port", "The port where the B2BUA will listen for connections.");
  options.addOptionString('d', "data-directory", "The directory where the REST Server will store data.");
  options.addOptionString('u', "auth-user", "User for Basic/Digest authentication.");
  options.addOptionString('w', "auth-password", "Password for Basic/Digest authentication.");

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
  
  std::string user;
  std::string pass;
  std::string host;
  int port = 8010;
  std::string dataDir;
  options.getOption("auth-user", user);
  options.getOption("auth-password", pass);
  options.getOption("host", host);
  options.getOption("port", port, 8010);
  options.getOption("data-directory", dataDir);
  
  if (!user.empty())
    server.setCredentials(user, pass);
  
  if (!dataDir.empty())
    server.setDataDirectory(dataDir);
  
  bool started = false;
  if (!host.empty())
    started = server.start(host, port, false);
  else
    started = server.start(port, false);
  
  if (started)
    options.waitForTerminationRequest();
  
  server.stop();
  
  OSS::OSS_deinit();
  return 0;
}
