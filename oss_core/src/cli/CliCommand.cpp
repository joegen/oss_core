
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


#include "OSS/CLI/Command.h"

extern "C"
{
#include "libcli.h"
}

namespace OSS {
namespace CLI {


Command::Command() :
  _pGroupHandle(0)
{
}

Command::~Command()
{
}

Command::Command(const std::string& group,
  const std::string& command,
  const std::string& helpString,
  const Handler& handler) :
    _handler(handler),
    _group(group),
    _command(command),
    _helpString(helpString),
    _pGroupHandle(0)
{
}

cli_command* Command::getGroupHandle()
{
  return _pGroupHandle;
}

void Command::setGroupHandle(cli_command* pGroupHandle)
{
  _pGroupHandle = pGroupHandle;
}

} } // OSS::CLI


