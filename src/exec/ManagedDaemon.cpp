
// OSS Software Solutions Application Programmer Interface
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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


#include "OSS/Exec/ManagedDaemon.h"


namespace OSS {
namespace Exec {


ManagedDaemon::ManagedDaemon(
  const std::string& executablePath, 
  const std::string& alias,
  const std::string& runDirectory,
  const std::string& startupScript,
  const std::string& shutdownScript,
  const std::string& pidFile,
  int maxRestart) :
    _path(executablePath),
    _alias(alias),
    _runDirectory(runDirectory),
    _pProcess(0)
{
}

ManagedDaemon::~ManagedDaemon()
{
}
  
bool ManagedDaemon::isAlive() const
{
  return false;
}

int ManagedDaemon::getPid() const
{
  return -1;
}

bool ManagedDaemon::readMessage(std::string& message, bool blocking)
{
  return false;
}

bool ManagedDaemon::stop()
{
  return false;
}

bool ManagedDaemon::start()
{
  return false;
}

bool ManagedDaemon::restart()
{
  return false;
}

Process::Action ManagedDaemon::onDeadProcess(int consecutiveCount)
{
  if (_maxRestart > consecutiveCount)
    return Process::ProcessRestart;
  else
    return Process::ProcessBackoff;
}
  
} } // OSS::Exec