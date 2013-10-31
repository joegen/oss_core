
#include "OSS/Core.h"
#include "OSS/ServiceDaemon.h"
#include "OSS/Exec/Command.h"
#include "OSS/Exec/Process.h"
#include <fstream>


namespace OSS {



ServiceDaemon::ServiceDaemon(int argc, char** argv, const std::string& daemonName) :
  ServiceOptions(argc, argv, daemonName)
{
  _procPath = argv[0];
  boost::filesystem::path procPath(_procPath.c_str());
  _procName = OSS::boost_file_name(procPath);
  
  addDaemonOptions();
  addOptionString("run-directory", "The directory where application data would be stored.");
}

ServiceDaemon::~ServiceDaemon()
{
}


int ServiceDaemon::pre_initialize()
{
  //
  // Parse command line options
  //
  if (!parseOptions())
    return -1;
  
  
  std::string newRunDir;
  if (getOption("run-directory", newRunDir) && !newRunDir.empty())
  {
    _runDir = newRunDir;
  }
  else
  {
    _runDir = OSS::boost_path(boost::filesystem::current_path());
  }
  
  OSS_ASSERT(!_runDir.empty());
  
  ::chdir(_runDir.c_str());
  
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


}
