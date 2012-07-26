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


#ifndef CLI_COMMAND_H_INCLUDED
#define	CLI_COMMAND_H_INCLUDED


#include "OSS/OSS.h"
#include <map>

struct cli_command;

namespace OSS {
namespace CLI {
  

class OSS_API Command : boost::noncopyable
{
public:
  typedef boost::shared_ptr<Command> Ptr;
  typedef std::map<std::string, Ptr> Map;
  typedef boost::function<void(const std::string&, const std::vector<std::string>&, std::string& msg)> Handler;
  Command();
  Command(const std::string& group,
    const std::string& command,
    const std::string& helpString,
    const Handler& handler);
  ~Command();
  void setCommand(const std::string& command);
  const std::string& getCommand() const;
  void setGroup(const std::string& group);
  const std::string& getGroup() const;
  void setHandler(const Handler& handler);
  const Handler& getHandler() const;
  void setHelpString(const std::string& helpString);
  const std::string& getHelpString() const;
  cli_command* getGroupHandle();
  void setGroupHandle(cli_command* pGroupHandle);
private:
  Handler _handler;
  std::string _group;
  std::string _command;
  std::string _helpString;
  cli_command* _pGroupHandle;
};


//
// Inlines
//

inline void Command::setCommand(const std::string& command)
{
  _command = command;
}

inline const std::string& Command::getCommand() const
{
  return _command;
}

inline void Command::setGroup(const std::string& group)
{
  _group = group;
}

inline const std::string& Command::getGroup() const
{
  return _group;
}

inline void Command::setHandler(const Handler& handler)
{
  _handler = handler;
}

inline const Command::Handler& Command::getHandler() const
{
  return _handler;
}

inline void Command::setHelpString(const std::string& helpString)
{
  _helpString = helpString;
}

inline const std::string& Command::getHelpString() const
{
  return _helpString;
}

} } // OSS::CLI


#endif	//  CLI_COMMAND_H_INCLUDED

