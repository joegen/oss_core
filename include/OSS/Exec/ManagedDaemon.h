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
#ifndef MANAGEDDAEMON_H_INCLUDED
#define	MANAGEDDAEMON_H_INCLUDED


#include "OSS/ServiceDaemon.h"
#include "OSS/IPCQueue.h"


namespace OSS {
namespace Exec {
    
  
#define DAEMON_IPC_QUEUE_SUFFIX "-process-control.ipc"  
  
class OSS_API ManagedDaemon : public ServiceDaemon
{
public:
  class DaemonIPC : public IPCJsonBidirectionalQueue
  {
  public:
    DaemonIPC(ManagedDaemon& daemon, const std::string& readQueuePath,  const std::string& writeQueuePath) :
      IPCJsonBidirectionalQueue(readQueuePath,writeQueuePath),
      _daemon(daemon)
    {
    }

    void onReceivedIPCMessage(const json::Object& params)
    {
      _daemon.onReceivedIPCMessage(params);
    }

    ManagedDaemon& _daemon;
  };

  ManagedDaemon(int argc, char** argv, const std::string& daemonName);
  virtual ~ManagedDaemon();

  
  int pre_initialize();
  virtual void onReceivedIPCMessage(const json::Object& message) = 0;
  bool sendIPCMessage(const json::Object& message);
  
  static std::string get_queue_path(char type, const std::string& runDir, const std::string& alias);
protected:
  DaemonIPC* _pIPC;
  friend int main(int argc, char** argv);

};

  
  
} } // OSS::Exec


#endif	// MANAGEDDAEMON_H_INCLUDED

