// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
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


#ifndef OSS_FILEMONITOR_H_INCLUDED
#define OSS_FILEMONITOR_H_INCLUDED

#include "OSS/build.h"
#if OSS_HAVE_INOTIFY

#include <string>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "OSS/UTL/CoreUtils.h"

namespace OSS { 
namespace UTL {


struct FileMonitorNotifyInfo
{
  typedef boost::function<void(FileMonitorNotifyInfo&)> EventHandler;
  std::string path;
  int fd;
  int revents;
  EventHandler handler;
};

class FileMonitor
{
public:
  
  static FileMonitor& instance();
  typedef FileMonitorNotifyInfo NotifyInfo;
  typedef NotifyInfo::EventHandler EventHandler;
  int addWatch(const boost::filesystem::path& path, EventHandler handler, int mask = 0);
  int addWatch(const std::string& path, EventHandler handler, int mask = 0);
  void removeWatch(int fd);
  void shutdown();
 
private:
  FileMonitor();
  ~FileMonitor();
};


//
// Inlines
//

inline int FileMonitor::addWatch(const boost::filesystem::path& path, EventHandler handler, int mask)
{
  return addWatch(OSS::boost_path(path), handler, mask);
}

} }


#endif // OSS_HAVE_INOTIFY
#endif // FILEMONITOR_H 

