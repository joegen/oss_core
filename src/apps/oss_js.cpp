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

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Usage:  oss_js [script] [script_options]" << std::endl;
    _exit(1);
  }
  
  boost::filesystem::path path = boost::filesystem::path(argv[1]);
  if (!boost::filesystem::exists(path))
  {
    std::cerr << "Script " << argv[1] << " not found." << std::endl;
    _exit(1);
  }
  
  set_limits();

  std::set_terminate(&ServiceOptions::catch_global);

  OSS::OSS_init(argc, argv);
  JS::JSBase vm("oss_js");
  vm.setEnableCommonJS(true);

  if (!vm.run(path))
  {
    _exit(-1);
  }
  _exit(0);
}