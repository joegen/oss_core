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


#include "OSS/UTL/FileMonitor.h"
#include "OSS/UTL/Thread.h"
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <sys/poll.h>


namespace OSS { 
namespace UTL {

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))


typedef std::map<int, NotifyInfo> Handlers;

int inotify_fd = -1;
int inotify_wakeup_fd[2];
boost::thread* inotify_thread = 0;
Handlers inotify_handlers;
OSS::mutex_critic_sec inotify_handlers_mutex;
bool inotify_is_shutting_down = false;

static void inotify_call_events(NotifyInfo& info)
{
  char buf[BUF_LEN] __attribute__ ((aligned(8)));
  uint32_t revents = 0;
  char *p = 0;
  
  int numRead = read(info.fd, buf, BUF_LEN);
  
  if (numRead > 0)
  {
    for (p = buf; p < buf + numRead; )
    {
      inotify_event *event = (inotify_event *)p;
      if (event->mask & IN_ACCESS)        revents = revents | IN_ACCESS;
      if (event->mask & IN_ATTRIB)        revents = revents | IN_ATTRIB;
      if (event->mask & IN_CLOSE_NOWRITE) revents = revents | IN_CLOSE_NOWRITE;
      if (event->mask & IN_CLOSE_WRITE)   revents = revents | IN_CLOSE_WRITE;
      if (event->mask & IN_CREATE)        revents = revents | IN_CREATE;
      if (event->mask & IN_DELETE)        revents = revents | IN_DELETE;
      if (event->mask & IN_DELETE_SELF)   revents = revents | IN_DELETE_SELF;
      if (event->mask & IN_MODIFY)        revents = revents | IN_MODIFY;
      if (event->mask & IN_MOVE_SELF)     revents = revents | IN_MOVE_SELF;
      if (event->mask & IN_MOVED_FROM)    revents = revents | IN_MOVED_FROM;
      if (event->mask & IN_MOVED_TO)      revents = revents | IN_MOVED_TO;
      if (event->mask & IN_OPEN)          revents = revents | IN_OPEN;
      if (event->mask & IN_UNMOUNT)       revents = revents | IN_UNMOUNT;
      p += sizeof(struct inotify_event) + event->len;
    }
    info.revents = revents;
    info.handler(info);
  }
}

static void inotify_poll()
{
  int inotify_wakeup_fd[2];
  if (pipe(inotify_wakeup_fd) != 0)
  {
    return;
  }
  
  while (!inotify_is_shutting_down)
  {
    inotify_handlers_mutex.lock();
    std::size_t pollfd_size = inotify_handlers.size() + 1;
    pollfd pfds[pollfd_size];
    pfds[0].fd = inotify_wakeup_fd[0];
    pfds[0].events = POLLIN;
    std::size_t index = 1;
    for (Handlers::iterator iter  = inotify_handlers.begin(); iter != inotify_handlers.end(); iter++)
    {
      pollfd item;
      item.fd = iter->second.fd;
      item.events = POLLIN;
      pfds[index++] = item;
    }
    inotify_handlers_mutex.unlock();
    
    int poll_ret = poll(pfds, pollfd_size, -1);
    if (poll_ret == 0)
    {
      continue;
    }
    else if (poll_ret == -1)
    {
      break;
    }
    
    for (std::size_t i = 0; i < pollfd_size; i++)
    {
      if (pfds[i].revents & POLLIN)
      {
        if (i == 0)
        {
          break; // this is just a wakeup call
        }
        else
        {
          inotify_handlers_mutex.lock();
          Handlers::iterator iter = inotify_handlers.find(pfds[i].fd);
          if (iter != inotify_handlers.end())
          {
            NotifyInfo info = iter->second;
            inotify_handlers_mutex.unlock();
            inotify_call_events(info);
          }
          else
          {
            inotify_handlers_mutex.unlock();
          }
        }
      }
    }
  }
  
  close(inotify_wakeup_fd[0]);
  close(inotify_wakeup_fd[1]);
}

static void inotify_wakeup()
{
  if (inotify_fd != -1)
  {
    write(inotify_wakeup_fd[1], "\0", 1);
  }
}

FileMonitor::FileMonitor()
{
  inotify_fd = inotify_init();
  if (inotify_fd != -1)
  {
    inotify_thread = new boost::thread(inotify_poll);
  }
}

FileMonitor::~FileMonitor()
{
}

FileMonitor& FileMonitor::instance()
{
  static FileMonitor monitor;
  return monitor;
}
  
int FileMonitor::addWatch(const std::string& path, EventHandler handler)
{
  if (!inotify_thread || inotify_fd == -1 || inotify_is_shutting_down)
  {
    return 0;
  }
  
  NotifyInfo info;
  info.path = path;
  info.handler = handler;
  info.fd = inotify_add_watch(inotify_fd, path.c_str(), IN_ALL_EVENTS);
  info.revents = 0;
  
  if (info.fd != -1)
  {
    OSS::mutex_critic_sec_lock lock(inotify_handlers_mutex);
    inotify_handlers[info.fd] = info;
    inotify_wakeup();
  }
  return info.fd;
}

void FileMonitor::removeWatch(int fd)
{
  OSS::mutex_critic_sec_lock lock(inotify_handlers_mutex);
  inotify_handlers.erase(fd);
  inotify_rm_watch(inotify_fd, fd);
  inotify_wakeup();
}

void FileMonitor::shutdown()
{
  if (!inotify_thread || inotify_is_shutting_down)
  {
    return;
  }
  inotify_is_shutting_down = true;
  inotify_wakeup();
} 


} }



