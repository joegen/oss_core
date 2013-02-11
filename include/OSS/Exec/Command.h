// OSS Software Solutions Application Programmer Interface
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: EXEC
//
// Copyright (c) 2011, OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version..
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//


#ifndef OSS_COMMAND_H_INCLUDED
#define	OSS_COMMAND_H_INCLUDED

#include <string>
#include <sstream>
#include <vector>


#include "OSS/Core.h"

namespace OSS {
namespace Exec {
    
class OSS_API Command : boost::noncopyable
{
public:
  Command();
  Command(const std::string& command);
  Command(const std::string& command, const std::vector<std::string>& args);
  ~Command();
  bool execute(const std::string& command);
  bool execute();
  char readChar() const;
  std::string readLine() const;
  bool isEOF() const;
  bool isGood() const;
  void join() const;
  void join(std::vector<std::string>& output);
  bool kill();
  bool kill(int signal);
  void close();
  void setCommand(const std::string& command);
  const std::string& getCommand() const;
  bool exited();
private:
  OSS_HANDLE _pstream;
  std::string _command;
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


} } // OSS::Exec

#define OSS_EXEC(cmd) \
{ \
  std::ostringstream strm; \
  strm << cmd; \
  OSS::Exec::Command command; \
  if (command.execute(strm.str())) \
    command.join(); \
}

#define OSS_EXEC_EX(cmd, output) \
{ \
  std::ostringstream strm; \
  strm << cmd; \
  OSS::Exec::Command command; \
  if (command.execute(strm.str())) \
    command.join(output); \
}


#endif	/* OSS_EXEC_H_INCLUDED */

