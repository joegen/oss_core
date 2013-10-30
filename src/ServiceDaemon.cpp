#include "OSS/ServiceDaemon.h"
#include "OSS/Exec/Command.h"
#include "OSS/Exec/Process.h"
#include <fstream>


namespace OSS {


ServiceDaemon::ServiceDaemon(int argc, char** argv, const std::string& daemonName) :
  ServiceOptions(argc, argv, daemonName)
{
  addOptionFlag('h', "help", "Display help information.");
  addOptionFlag('D', "daemonize", "Run as system daemon.");
  addOptionString('p', "pid-file", "PID file when running as daemon.");
}

ServiceDaemon::~ServiceDaemon()
{
}


int ServiceDaemon::pre_initialize()
{
  //
  // Parse command line options
  //
  parseOptions();

  if (hasOption("daemonize"))
  {
    if (!hasOption("pid-file"))
    {
      std::cerr << "You need to specify a PID file when running as daemon!" << std::endl;
      displayUsage(std::cerr);
      return -1;
    }

    getOption("pid-file", _pidFile);
    OSS::app_set_pid_file(_pidFile);
    daemonize();
  }


  return 0;
}

int ServiceDaemon::post_initialize()
{
  return 0;
}

int ServiceDaemon::post_main()
{
  return 0;
}

void ServiceDaemon::daemonize()
{
  pid_t pid;
  if ((pid = fork()) < 0)
  {
    std::cerr << "Unable to fork daemon!"  << std::endl;
    std::cerr.flush();
    _exit(0);
  }
  else if (pid != 0)
  {
    _exit(0);
  }
  setpgrp();
  ::close(STDIN_FILENO);

  if (!_pidFile.empty())
  {
    std::ofstream ostr(_pidFile.c_str());
    if (ostr.good())
      ostr << getpid() << std::endl;
  }
}




}
