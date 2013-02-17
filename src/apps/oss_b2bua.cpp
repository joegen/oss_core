// OSS Software Solutions Application Programmer Interface
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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


#include "OSS/ServiceDaemon.h"
#include "OSS/SIP/B2BUA/SIPB2BTransactionManager.h"
#include "OSS/SIP/B2BUA/SIPB2BScriptableHandler.h"
#include "OSS/SIP/B2BUA/SIPB2BDialogStateManager.h"

#define TCP_PORT_BASE 20000
#define TCP_PORT_MAX  30000

using namespace OSS;
using namespace OSS::SIP;
using namespace OSS::SIP::B2BUA;


struct ListenerInfo
{
  std::string address;
  std::string externalAddress;
  int port;
  ListenerInfo() : port(5060){}
};


class oss_b2bua :
  public SIPB2BTransactionManager,
  public SIPB2BDialogStateManager,
  public SIPB2BScriptableHandler
{

oss_b2bua() :
  SIPB2BTransactionManager(2, 1024),
  SIPB2BDialogStateManager(dynamic_cast<SIPB2BTransactionManager*>(this)),
  SIPB2BScriptableHandler(dynamic_cast<SIPB2BTransactionManager*>(this), dynamic_cast<SIPB2BDialogStateManager*>(this))
{
}

~oss_b2bua()
{
}

bool onProcessRequest(MessageType type, const SIPMessage::Ptr& pRequest)
{
  pRequest->setProperty("route-action", "accept");
  return true;
}

}; // class oss_b2bua


inline void  daemonize(int argc, char** argv, bool& isDaemon)
{
  for (int i = 0; i < argc; i++)
  {
    std::string arg = argv[i];
    if (arg == "-D" || arg == "--daemonize")
    {
      isDaemon = true;
      break;
    }
  }

  if (isDaemon)
  {
     int pid = 0;
   if(getppid() == 1)
     return;
   pid=fork();
   if (pid<0) exit(1); /* fork error */
   if (pid>0) exit(0); /* parent exits */
   /* child (daemon) continues */
   setsid(); /* obtain a new process group */

   for (int descriptor = getdtablesize();descriptor >= 0;--descriptor)
   {
     close(descriptor); /* close all descriptors we have inheritted from parent*/
   }

   int h = open("/dev/null",O_RDWR); dup(h); dup(h); /* handle standard I/O */

   ::close(STDIN_FILENO);
  }
}

bool prepareOptions(ServiceOptions& options)
{
  options.addOptionFlag('h', "help", "Display help information.");
  options.addOptionFlag('D', "daemonize", "Run as system daemon.");
  options.addOptionString('P', "pid-file", "PID file when running as daemon.");
  options.addOptionString('i', "interface-address", "The IP Address where the B2BUA will listener for connections.");
  options.addOptionInt('p', "port", "The port where the B2BUA will listen for connections.");
  return options.parseOptions();
}

void saveProcessId(ServiceOptions& options)
{
  std::string pidFilePath;
  if (options.getOption("pid-file", pidFilePath))
  {
    if (!pidFilePath.empty())
    {
      std::ofstream ostr(pidFilePath.c_str());
      if (ostr.good())
        ostr << getpid() << std::endl;
    }
  }
}

void prepareListenerInfo(ListenerInfo& listenerInfo, ServiceOptions& options)
{
  if (!options.getOption("interface-address", listenerInfo.address))
  {
    std::cout << "[Error] You must provide value for interface-address." << std::endl;
    options.displayUsage(std::cout);
    _exit(-1);
  }
  
  if (!options.getOption("port", listenerInfo.port))
  {
    std::cout << "[Error] You must provide value for port." << std::endl;
    options.displayUsage(std::cout);
    _exit(-1);
  }
}

int main(int argc, char** argv)
{
  bool isDaemon = false;
  daemonize(argc, argv, isDaemon);

  ServiceOptions options(argc, argv, "oss_b2bua");
  if (!prepareOptions(options) || options.hasOption("help"))
  {
    options.displayUsage(std::cout);
    _exit(-1);
  }

  saveProcessId(options);

  ListenerInfo listenerInfo;
  prepareListenerInfo(listenerInfo, options);

  return 0;
}