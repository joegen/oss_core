// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "GNU Lesser General Public License (LGPL)".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef OSS_SERVICEDAEMON_INCLUDED
#define OSS_SERVICEDAEMON_INCLUDED

#include "OSS/OSS.h"
#include "OSS/Application.h"
#include "OSS/ServiceOptions.h"


namespace OSS {

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


