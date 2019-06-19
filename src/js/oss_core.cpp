#include <execinfo.h>
#include <sys/resource.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>

#include "OSS/UTL/Application.h"
#include "OSS/UTL/ServiceDaemon.h"
#include "OSS/UTL/Thread.h"
#include "OSS/UTL/Console.h"
#include "OSS/JS/JSIsolateManager.h"
#include "OSS/SIP/SBC/SBCManager.h"


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
    std::cerr << "Usage:  " << argv[0] << " [script] [script_options]" << std::endl;
    _exit(1);
  }
  
  bool isDaemon = strcmp(argv[1], "-d") == 0;
  if (isDaemon)
  {
    if (argc < 3)
    {
      std::cerr << "Usage:  ossjs -d [script] [script_options]" << std::endl;
      _exit(1);
    }
    //
    // fork the process
    //
    pid_t pid;
    if ((pid = fork()) < 0)
      _exit(0);
    else if (pid != 0)
      _exit(0);
    setpgrp();
    int h = open("/dev/null",O_RDWR); dup(h); dup(h); /* handle standard I/O */
    ::close(STDIN_FILENO);
  }
  OSS::OSS_init(argc, argv);
  int scriptIndex = isDaemon ? 2 : 1;

  boost::filesystem::path path = boost::filesystem::path(argv[scriptIndex]);
  if (!boost::filesystem::exists(path))
  {
    std::cerr << "Script " << argv[scriptIndex] << " not found." << std::endl;
    OSS::OSS_deinit();
    _exit(1);
  }
  
  set_limits();

  //std::set_terminate(&ServiceOptions::catch_global);

  
  
  //
  // Set optind so getopt knows the correct index where to find scrip params
  //
  optind = isDaemon ? 3 : 2;

  JS::JSIsolate::Ptr pIsolate = JS::JSIsolateManager::instance().rootIsolate();
  OSS::SIP::SBC::SBCManager::instance()->modules().run(OSS::boost_path(path), false);
  OSS::OSS_deinit();
  _exit(pIsolate->getExitValue());
}