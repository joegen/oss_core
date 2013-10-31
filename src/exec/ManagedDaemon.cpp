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
    
std::string ManagedDaemon::get_queue_path(char type, const std::string& runDir, const std::string& alias)
{
  std::ostringstream path;
  path << runDir << "/" << alias << "-" << type << "-" << DAEMON_IPC_QUEUE_SUFFIX;
  return path.str();
}


ManagedDaemon::ManagedDaemon(int argc, char** argv, const std::string& daemonName) :
  ServiceDaemon(argc, argv, daemonName),
  _pIPC(0)
{
}

ManagedDaemon::~ManagedDaemon()
{
  delete _pIPC;
}

int ManagedDaemon::pre_initialize()
{
  int ret = ServiceDaemon::pre_initialize();
  if (ret != 0)
    return ret;
    
  _pIPC = new DaemonIPC(*this, ManagedDaemon::get_queue_path('0', getRunDirectory(), getProcName()),  ManagedDaemon::get_queue_path('1', getRunDirectory(), getProcName()));
  
  return 0;
}

bool ManagedDaemon::sendIPCMessage(const json::Object& message)
{
  if (!_pIPC)
    return false;
  return _pIPC->sendIPCMessage(message);
}
 
  
} } // OSS::Exec




