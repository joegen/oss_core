#ifndef OSS_SERVICEDAEMON_INCLUDED
#define OSS_SERVICEDAEMON_INCLUDED

#include "OSS/OSS.h"
#include "OSS/Application.h"
#include <boost/program_options.hpp>


namespace OSS {


class ServiceOptions
{
public:
  ServiceOptions(int argc, char** argv, const std::string& daemonName);
  ~ServiceOptions();
  //
  // Options processing
  //
  void addOptionFlag(char shortForm, const std::string& optionName, const std::string description);
  void addOptionFlag(const std::string& optionName, const std::string description);
  void addOptionString(char shortForm, const std::string& optionName, const std::string description);
  void addOptionString(const std::string& optionName, const std::string description);
  void addOptionInt(char shortForm, const std::string& optionName, const std::string description);
  void addOptionInt(const std::string& optionName, const std::string description);
  bool parseOptions();
  void displayUsage(std::ostream& strm) const;
  size_t hasOption(const std::string& optionName) const;
  bool getOption(const std::string& optionName, std::string& value) const;
  bool getOption(const std::string& optionName, int& value) const;

  virtual bool onParseUnknownOptions(int argc, char** argv) {return false;};

protected:
  int _argc;
  char** _argv;
  std::string _daemonName;
  boost::program_options::options_description _optionItems;
  boost::program_options::variables_map _options;
};

class OSS_API ServiceDaemon : public ServiceOptions, boost::noncopyable
{
public:
  struct ProcessStatus
  {
    std::string user;
    int pid;
    double cpu;
    double mem;
    int vsz;
    int rss;
    std::string tty;
    std::string stat;
    std::string start;
    std::string time;
    std::string command;
  };

  typedef std::map<int, ProcessStatus> ProcessStatusMap;

  ServiceDaemon(int argc, char** argv, const std::string& daemonName);
  virtual ~ServiceDaemon();

  
  virtual int initialize() = 0;
  virtual int main() = 0;

  int pre_initialize();
  int post_initialize();
  int post_main();
  
protected:
  void daemonize();
  std::string _pidFile;
  friend int main(int argc, char** argv);

};


} // OSS

#define DAEMONIZE(Daemon, daemonName) \
int main(int argc, char** argv) \
{ \
  Daemon daemon(argc, argv, daemonName); \
  int ret = 0; \
  ret = daemon.pre_initialize(); \
  if (ret != 0) \
    exit(ret); \
  ret = daemon.initialize(); \
  if (ret != 0) \
    exit(ret); \
  ret = daemon.main(); \
  if ( ret != 0) \
    exit(ret); \
  ret = daemon.post_main(); \
  if ( ret != 0) \
    exit(ret); \
  exit(0); \
}


#endif // OSS_SERVICEDAEMON_INCLUDED


