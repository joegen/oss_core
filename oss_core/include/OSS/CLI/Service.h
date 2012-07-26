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


#ifndef OSS_CLI_SERVICE_H_INLCUDED
#define	OSS_CLI_SERVICE_H_INLCUDED

#include "OSS/CLI/Command.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

struct cli_def;

namespace OSS {
namespace CLI {

class OSS_API Service : boost::noncopyable
{
public:

  Service();

  ~Service();

  bool registerCommand(const std::string& group,
      const std::string& command,
      const std::string& helpString,
      const Command::Handler& handler,
      bool priviledged = false);

  void registerHeartBeatHandler(
    const boost::function<void(std::string& statusMsg)>& handler,
    unsigned int timeInSeconds);

  void run();

  void stop();
  
  void setPort(unsigned short port);

  unsigned short getPort() const;

  void setPriviledgedPassword(const std::string& password); 
  
  void setBanner(const std::string& banner);
  const std::string& getBanner() const;
  
  void setPromptString(const std::string& promptString);
  const std::string& getPromptString() const;

  void addUser(const std::string& user, const std::string& password);
protected:
  bool initTransport();
  void readEvents(int inboundSocket);
  bool initCommands();

  static int authHandler(char* user, char* password);
  static int commandHandler(struct cli_def* cli, char* command, char* argv[], int argc);
  static int checkPriviledged(char*);
  static int heartBeatHandler(struct cli_def *cli);
  boost::function<void(std::string& statusMsg)> _hearBeatHandlerCallback;

private:
  std::string _priviledgedPassword;
  std::string _promptString;
  std::string _banner;
  Command::Map _commands;
  Command::Map _groups;
  std::map<std::string, std::string> _users;
  cli_def*_pCli;
  unsigned short _port;
  int _socket;
  bool _exit;
};


//
// Inlines
//

inline void Service::addUser(const std::string& user, const std::string& password)
{
  _users[user] = password;
}

inline void Service::setPort(unsigned short port)
{
  _port = port;
}

inline unsigned short Service::getPort() const
{
  return _port;
}

inline void Service::setPriviledgedPassword(const std::string& password)
{
  _priviledgedPassword = password;
}

inline void Service::setBanner(const std::string& banner)
{
  _banner = banner;
}

inline const std::string& Service::getBanner() const
{
  return _banner;
}

inline void Service::setPromptString(const std::string& promptString)
{
  _promptString = promptString;
}

inline const std::string& Service::getPromptString() const
{
  return _promptString;
}

} } // OSS::CLI


#endif	//OSS_CLI_SERVICE_H_INLCUDED



