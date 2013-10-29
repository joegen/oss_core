
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


#include "OSS/Core.h"
#include "OSS/Logger.h"
#include "OSS/Exec/ManagedDaemon.h"


namespace OSS {
namespace Exec {


const int STOP_YIELD_TIME = 100; /// yield time in milliseconds
const int STOP_MAX_YIELD_TIME = 15000; /// maximum yield time in milliseconds


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
  boost::filesystem::path path(_path.c_str());
  std::string prog = OSS::boost_file_name(path);
  
  if (prog.empty())
  {
    OSS_LOG_ERROR("ManagedDaemon::ManagedDaemon - Unable to parse executable from " << _path);
    return;
  }
  
  //Process(const std::string& processName, const std::string& startupCommand, const std::string& shutdownCommand = "", const std::string& pidFile = "");
  _pProcess = new Process(prog, startupScript, shutdownScript, pidFile);
    
}

ManagedDaemon::~ManagedDaemon()
{
  stop();
  delete _pProcess;
}
  
bool ManagedDaemon::readMessage(std::string& message, bool blocking)
{
  return false;
}

bool ManagedDaemon::start()
{
  OSS_ASSERT(_pProcess);
  stop(); /// stop all lingering process
  return _pProcess->executeAndMonitor();
}

bool ManagedDaemon::stop()
{
  //
  // Unmonitor the process so it doesn't get restarted
  //
  _pProcess->unmonitor();
  
  //
  // Send the process a SIGTERM
  //
  Process::killAll(_alias, SIGTERM);
  int totalYieldTime = 0;
  for (pid_t pid = getProcessId(); (pid = getProcessId()) == -1 && totalYieldTime < STOP_MAX_YIELD_TIME; totalYieldTime += STOP_YIELD_TIME)
  {
    OSS::thread_sleep(STOP_YIELD_TIME); // sleep for 100 milliseconds
  }
  
  //
  // Send the process a SIGKILL
  //
  Process::killAll(_alias, SIGKILL);
  totalYieldTime = 0;
  for (pid_t pid = getProcessId(); (pid = getProcessId()) == -1 && totalYieldTime < STOP_MAX_YIELD_TIME; totalYieldTime += STOP_YIELD_TIME)
  {
    OSS::thread_sleep(STOP_YIELD_TIME); // sleep for 100 milliseconds
  }

  return getProcessId() == -1;
}



bool ManagedDaemon::restart()
{
  if (!stop())
    return false;
  return start();
}

Process::Action ManagedDaemon::onDeadProcess(int consecutiveCount)
{
  if (_maxRestart > consecutiveCount)
    return Process::ProcessRestart;
  else
    return Process::ProcessBackoff;
}
  
} } // OSS::Exec