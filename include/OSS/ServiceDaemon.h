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
#include "OSS/IPCQueue.h"


namespace OSS {

class OSS_API ServiceDaemon : public ServiceOptions
{
public:
  ServiceDaemon(int argc, char** argv, const std::string& daemonName);
  virtual ~ServiceDaemon();

  
  virtual int initialize() = 0;
  virtual int main() = 0;
  virtual int pre_initialize();
  virtual int post_initialize();
  virtual int post_main();
  
  const std::string& getProcPath() const;
  const std::string& getProcName() const;
  const std::string& getRunDirectory() const;
  
protected:
  std::string _procPath;
  std::string _procName;
  std::string _runDir;
  friend int main(int argc, char** argv);

};

//
// Inlines
//

inline const std::string& ServiceDaemon::getProcPath() const
{
  return _procPath;
}

inline const std::string& ServiceDaemon::getProcName() const
{
  return _procName;
}

inline const std::string& ServiceDaemon::getRunDirectory() const
{
  return _runDir;
}

} // OSS

#define DAEMONIZE(Daemon, daemonName) \
int main(int argc, char** argv) \
{ \
  bool isDaemon = false; \
  ServiceOptions::daemonize(argc, argv, isDaemon); \
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


