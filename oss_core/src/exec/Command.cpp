
// OSS Software Solutions Application Programmer Interface
// Package: SBC
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


#include "OSS/Exec/Command.h"
#include "OSS/Exec/pstream.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <vector>

using namespace redi;

// explicit instantiations of template classes
template class redi::basic_pstreambuf<char>;
template class redi::pstream_common<char>;
template class redi::basic_ipstream<char>;
template class redi::basic_opstream<char>;
template class redi::basic_pstream<char>;
template class redi::basic_rpstream<char>;

#if defined(__sun)
  int sh_cmd_not_found = 1;
#else
  int sh_cmd_not_found = 127;
#endif

namespace OSS {
namespace Exec {

Command::Command() :
  _pstream(0)
{
}

Command::Command(const std::string& command) :
  _pstream(0),
  _command(command)
{
}

Command::Command(const std::string& command, const std::vector<std::string>& args) :
  _pstream(0)
{
  std::ostringstream buff;
  buff << command;
  for (std::vector<std::string>::const_iterator iter = args.begin();
    iter != args.end(); iter++)
  {
    buff << " " << *iter;
  }
  _command = buff.str();
}

Command::~Command()
{
  close();
}

void Command::close()
{
  if (_pstream)
  {
    ipstream* pStrm = static_cast<ipstream*>(_pstream);
    pStrm->clear();
    pStrm->close();
    delete pStrm;
    _pstream = 0;
  }
}

bool Command::execute()
{
  close();
  _pstream = new ipstream();
  ipstream* pStrm = static_cast<ipstream*>(_pstream);
  pStrm->open(_command);
  return !isEOF() && isGood();
}

bool Command::execute(const std::string& command)
{
  _command = command;
  return execute();
}

char Command::readChar() const
{
  ipstream* pStrm = static_cast<ipstream*>(_pstream);
  if (!pStrm)
    return 0;
  char c;
  pStrm->get(c);
  return c;
}

std::string Command::readLine() const
{
  ipstream* pStrm = static_cast<ipstream*>(_pstream);
  if (!pStrm)
    return 0;
  std::string buf;
  getline(pStrm->out(), buf);
  return buf;
}

bool Command::isEOF() const
{
  ipstream* pStrm = static_cast<ipstream*>(_pstream);
  if (!pStrm)
    return true;
  return pStrm->eof();
}

bool Command::isGood() const
{
  ipstream* pStrm = static_cast<ipstream*>(_pstream);
  if (!pStrm)
    return false;

  return pStrm->good();
}

bool Command::kill()
{
  ipstream* pStrm = static_cast<ipstream*>(_pstream);
  if (!pStrm)
    return false;

  pstreambuf* pbuf = pStrm->rdbuf();
  if (!pbuf)
    return false;
  pbuf->kill();
  return true;
}

bool Command::kill(int signal)
{
  ipstream* pStrm = static_cast<ipstream*>(_pstream);
  if (!pStrm)
    return false;

  pstreambuf* pbuf = pStrm->rdbuf();
  if (!pbuf)
    return false;
  pbuf->kill(signal);
  return true;
}


bool Command::exited()
{
  ipstream* pStrm = static_cast<ipstream*>(_pstream);
  if (!pStrm)
    return -1;
  pstreambuf* pbuf = pStrm->rdbuf();
  if (!pbuf)
    return true;
  return pbuf->exited();
}


} } // OSS::Exec


