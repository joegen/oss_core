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


#ifndef OSS_EXEC_PROCESS_H_INCLUDED
#define	OSS_EXEC_PROCESS_H_INCLUDED

#include "OSS/Thread.h"
#include "OSS/Exec/Command.h"
#include <unistd.h>
#include <signal.h>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>


namespace OSS {
namespace Exec {

class OSS_API Process  : boost::noncopyable
{
public:
  enum Action
  {
    ProcessNormal,
    ProcessRestart,
    ProcessBackoff,
    ProcessShutdown,
    ProcessUnmonitor
  };

  typedef boost::function<Action(int)> DeadProcHandler;
  typedef boost::function<Action(int, double)> MemViolationHandler;
  typedef boost::function<Action(int, double)> CpuViolationHandler;

  Process(const std::string& processName, const std::string& startupCommand, const std::string& shutdownCommand = "", const std::string& pidFile = "");
  ~Process();
  bool execute();
  bool executeAndMonitor();
  bool executeAndMonitorMem(double maxMemPercent);
  bool executeAndMonitorCpu(double maxCpuPercent);
  bool executeAndMonitorMemCpu(double maxMemPercent, double maxCpuPercent);
  pid_t pollPid(const std::string& process, const std::string& pidFile, int maxIteration, long interval);
  int kill(int signal);
  bool shutDown(int signal = SIGTERM);
  bool restart();
  void unmonitor();
  bool exists(double& currentMem, double& currentCpu);

  DeadProcHandler deadProcHandler;
  Action onDeadProcess(int consecutiveHits);
  MemViolationHandler memViolationHandler;
  Action onMemoryViolation(int consecutiveHits, double mem);
  CpuViolationHandler cpuViolationHandler;
  Action onCpuViolation(int consecutiveHits, double cpu);

  static int countProcessInstances(const std::string& process);
  static int getProcessId(const std::string& process, bool includeDefunct = false);
  static void killAll(const std::string& process, int signal);
  static void killAllDefunct(const std::string& process);
  bool isAlive() const;
  bool& noChild();
  void setInitializeWait(unsigned int ms);
protected:
  void internalExecuteAndMonitor(int intialWait);
  std::string _processName;
  std::string _startupCommand;
  std::string _shutdownCommand;
  std::string _pidFile;
  pid_t _pid;
  boost::thread* _pMonitorThread;
  OSS::semaphore _frequencySync;
  OSS::semaphore _backoffSync;
  OSS::semaphore _pidSync;
  unsigned int _frequencyTime;
  unsigned int _backoffTime;
  unsigned int _maxIteration;
  unsigned int _deadProcessIteration;
  unsigned int _maxMemViolationIteration;
  unsigned int _maxCpuViolationIteration;
  double _maxCpuUsage;
  double _maxMemUsage;
  bool _unmonitor;
  bool _monitored;
  bool _isAlive;
  bool _noChild;
  unsigned int _initWait;
};

//
// Inlines
//

inline bool Process::isAlive() const
{
  return _isAlive;
}

inline bool& Process::noChild()
{
  return _noChild;
}

inline void Process::setInitializeWait(unsigned int ms)
{
  _initWait = ms;
}

} }  // OSS::Exec

#endif	// OSS_EXEC_PROCESS_H_INCLUDED



