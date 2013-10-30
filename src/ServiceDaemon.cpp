#include "OSS/ServiceDaemon.h"
#include "OSS/Exec/Command.h"
#include "OSS/Exec/Process.h"
#include <fstream>


namespace OSS {


ServiceDaemon::ServiceDaemon(int argc, char** argv, const std::string& daemonName) :
  ServiceOptions(argc, argv, daemonName, "1.1.1", "OSS Software Solutions")
{
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
