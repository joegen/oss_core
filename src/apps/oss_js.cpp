#include <execinfo.h>
#include <sys/resource.h>
#include <signal.h>

#include "OSS/UTL/Application.h"
#include "OSS/UTL/ServiceDaemon.h"
#include "OSS/UTL/Thread.h"
#include "OSS/UTL/Console.h"
#include "OSS/JS/JSBase.h"


using namespace OSS;


void set_limits()
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


bool prepareOptions(ServiceOptions& options)
{
  options.addDaemonOptions();
  options.addOptionString('x', "script", "The Script to execute");
  options.addOptionString('m', "modules-dir", "Directory where to find the commonJS modules"); 
  return options.parseOptions();
}

int main(int argc, char** argv)
{
  set_limits();

  bool isDaemon = false;
  ServiceOptions::daemonize(argc, argv, isDaemon);

  std::set_terminate(&ServiceOptions::catch_global);

  OSS::OSS_init();

  ServiceOptions options(argc, argv, "oss_js", "1.0.0", "Copyright (c) OSS Software Solutions");
  if (!prepareOptions(options) || options.hasOption("help"))
  {
    options.displayUsage(std::cout);
    _exit(-1);
  }
  
  std::string script;
  if (!options.getOption("script", script) || script.empty())
  {
    options.displayUsage(std::cout);
    _exit(-1);
  }

  options.getOption("modules-dir", JS::JSBase::_modulesDir);

  boost::filesystem::path path = boost::filesystem::path(script.c_str());

  JS::JSBase::initModules();
  JS::JSBase vm("oss_js");
  if (!vm.run(path))
  {
    _exit(-1);
  }
  _exit(0);
}