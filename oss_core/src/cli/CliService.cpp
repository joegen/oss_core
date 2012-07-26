// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: CLI
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


#include "OSS/CLI/Service.h"
#include "OSS/Thread.h"

extern "C"
{
#include <stdio.h>
#include <sys/types.h>
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include "libcli.h"
};

#define PORT_DEFAULT                9999
#define MODE_CONFIG_INT             10


#ifdef WIN32
typedef int socklen_t;

int winsock_init()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    // Start up sockets
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        return 0;
    }

    /*
     * Confirm that the WinSock DLL supports 2.2
     * Note that if the DLL supports versions greater than 2.2 in addition to
     * 2.2, it will still return 2.2 in wVersion since that is the version we
     * requested.
     * */
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        WSACleanup();
        return 0;
    }
    return 1;
}
#endif

namespace OSS {
namespace CLI {


static Service* _pService = 0;

Service::Service() :
  _pCli(0),
  _port(PORT_DEFAULT),
  _socket(-1),
  _exit(false)
{
  assert(!_pService);
  _pCli = cli_init();
  _banner = "Welcome to libOSS CLI.";
  _promptString = "libOSS";
  _priviledgedPassword = "libOSS";
  _pService = this;
}

Service::~Service()
{
  cli_done(_pCli);
}

bool Service::registerCommand(const std::string& group,
      const std::string& command,
      const std::string& helpString,
      const Command::Handler& handler,
      bool priviledged)
{
  std::ostringstream id;
  if (!group.empty())
    id << group << " ";
  id << command;
  Command::Ptr pCommand = Command::Ptr(new Command(group, command, helpString, handler));
  

  if (!group.empty())
  {
    //
    // register the group if its not there yet
    //
    Command::Map::iterator groupIter = _groups.find(group);
    if (groupIter == _groups.end())
    {
      pCommand->setGroupHandle(
        cli_register_command(_pCli,
          0,
          const_cast<char*>(group.c_str()),
          0,
          PRIVILEGE_UNPRIVILEGED,
          MODE_EXEC,
          0) );
      //
      // Register the pilot command for this group
      //
      _groups[group] = pCommand;
    }
    else
    {
      //
      // A group already exists, so just get its handle and inherit from it
      //
      pCommand->setGroupHandle(groupIter->second->getGroupHandle());
    }
  }

  //
  // Register the new command
  //
  char* strCommand = const_cast<char*>(command.c_str());
  char* strHelpString = const_cast<char*>(pCommand->getHelpString().c_str());
  cli_command* pGroupHandle = pCommand->getGroupHandle();
  cli_register_command(
    _pCli,
    pGroupHandle,
    strCommand,
    &Service::commandHandler,
    priviledged ? PRIVILEGE_PRIVILEGED : PRIVILEGE_UNPRIVILEGED,
    MODE_EXEC, 
    strHelpString);


  _commands[id.str()] = pCommand;

  return true;
}

void Service::registerHeartBeatHandler(
  const boost::function<void(std::string& statusMsg)>& handler,
  unsigned int timeInSeconds)
{
  cli_regular(_pCli, &Service::heartBeatHandler);
  cli_regular_interval(_pCli, timeInSeconds);
  _hearBeatHandlerCallback = handler;
}

bool Service::initTransport()
{
#ifdef WIN32
  if (!winsock_init())
  {
    return false;
  }
#endif
  if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    return false;
  }
  int on = 1;
  setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(_port);
  if (bind(_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
  {
    return false;
  }

  if (listen(_socket, 50) < 0)
  {
    return false;
  }
  return true;
}

int Service::checkPriviledged(char* password)
{
  return _pService->_priviledgedPassword != password ? CLI_ERROR : CLI_OK;
}

int Service::authHandler(char* user, char* password)
{
  if (_pService->_users.size() != 0)
  {
    std::map<std::string, std::string>::iterator account = _pService->_users.find(user);
    if ( account == _pService->_users.end())
        return CLI_ERROR;

    if (account->second != password)
        return CLI_ERROR;
    return CLI_OK;
  }
  return CLI_ERROR;
}

int Service::commandHandler(struct cli_def* cli, char* command_, char* argv[], int argc)
{
  std::string command = command_;
  std::vector<std::string> argVector;
  for (int i = 0; i < argc; i++)
    argVector.push_back(argv[i]);

  Command::Map::iterator pCommand = _pService->_commands.find(command);
  if (pCommand != _pService->_commands.end())
  {
    std::string statusMessage;
    pCommand->second->getHandler()(command, argVector, statusMessage);
    if (!statusMessage.empty())
    {
      cli_print(cli, "%s", statusMessage.c_str());
    }
  }
  return CLI_OK;
}

int Service::heartBeatHandler(struct cli_def *cli)
{
  std::string statusMessage;
  if (_pService->_hearBeatHandlerCallback)
  {
    _pService->_hearBeatHandlerCallback(statusMessage);
    if (!statusMessage.empty())
    {
      cli_print(cli, "%s", statusMessage.c_str());
    }
  }
  return CLI_OK;
}

void Service::run()
{
  if (!initTransport())
    return;

  //
  // Set prompt and welcome note
  //
  cli_set_banner(_pCli, const_cast<char*>(_banner.c_str()));
  cli_set_hostname(_pCli, const_cast<char*>(_promptString.c_str()));
  //
  // Register authenticators
  //
  cli_set_auth_callback(_pCli, &Service::authHandler);
  cli_set_enable_callback(_pCli, &Service::checkPriviledged);

  
  int inboundSocket = -1;
  while (!_exit && (inboundSocket = accept(_socket, 0, 0)))
  {
    //
    // run in its own thread
    //
    if (!_exit)
      OSS::thread_pool::static_schedule(boost::bind(&Service::readEvents, this, inboundSocket));
  }
}

void Service::readEvents(int inboundSocket)
{
  if (_pCli && inboundSocket)
  {
    cli_loop(_pCli, inboundSocket);
    close(inboundSocket);
  }
}

void Service::stop()
{
  _exit = true;
  close(_socket);
}




} } // OSS::CLI