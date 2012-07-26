#include "OSS/ServiceDaemon.h"
#include "OSS/Exec/Command.h"
#include "OSS/Exec/Process.h"
#include <fstream>

namespace OSS {


void ServiceOptions::addOptionFlag(char shortForm, const std::string& optionName, const std::string description)
{
  std::ostringstream strm;
  strm << optionName << "," << shortForm;
  _optionItems.add_options()(strm.str().c_str(), description.c_str());
}

void ServiceOptions::addOptionFlag(const std::string& optionName, const std::string description)
{
  _optionItems.add_options()(optionName.c_str(), description.c_str());
}

void ServiceOptions::addOptionString(char shortForm, const std::string& optionName, const std::string description)
{
  std::ostringstream strm;
  strm << optionName << "," << shortForm;
  _optionItems.add_options()(strm.str().c_str(), boost::program_options::value<std::string>(), description.c_str());
}

void ServiceOptions::addOptionString(const std::string& optionName, const std::string description)
{
  _optionItems.add_options()(optionName.c_str(), boost::program_options::value<std::string>(), description.c_str());
}

void ServiceOptions::addOptionInt(char shortForm, const std::string& optionName, const std::string description)
{
  std::ostringstream strm;
  strm << optionName << "," << shortForm;
  _optionItems.add_options()(strm.str().c_str(), boost::program_options::value<int>(), description.c_str());
}

void ServiceOptions::addOptionInt(const std::string& optionName, const std::string description)
{
  _optionItems.add_options()(optionName.c_str(), boost::program_options::value<int>(), description.c_str());
}

bool ServiceOptions::parseOptions()
{
  try
  {
    boost::program_options::store(boost::program_options::parse_command_line(_argc, _argv, _optionItems), _options);
    boost::program_options::notify(_options);
  }
  catch(const std::exception& e)
  {
    if (!onParseUnknownOptions(_argc, _argv))
    {
      std::cerr << _daemonName << "is not able to parse the options - " << e.what() << std::endl;
      return false;
    }
  }
  return true;
}

void ServiceOptions::displayUsage(std::ostream& strm) const
{
  strm << _optionItems;
}

size_t ServiceOptions::hasOption(const std::string& optionName) const
{
  return _options.count(optionName.c_str());
}

bool ServiceOptions::getOption(const std::string& optionName, std::string& value) const
{
  if (!hasOption(optionName))
    return false;
  value = _options[optionName.c_str()].as<std::string>();
  return true;
}

bool ServiceOptions::getOption(const std::string& optionName, int& value) const
{
  if (!hasOption(optionName))
    return false;
  value = _options[optionName.c_str()].as<int>();
  return true;
}


ServiceOptions::ServiceOptions(int argc, char** argv, const std::string& daemonName) :
  _daemonName(daemonName),
  _argc(argc),
  _argv(argv),
  _optionItems(_daemonName  + " Options")
{
}

ServiceOptions::~ServiceOptions()
{
}


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
