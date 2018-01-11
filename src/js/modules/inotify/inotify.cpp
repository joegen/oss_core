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

#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

JS_METHOD_IMPL(__inotify_init)
{
  return JSInt32(inotify_init());
}

JS_METHOD_IMPL(__inotify_add_watch)
{
  js_method_arg_assert_size_eq(3);
  js_method_arg_assert_int32(0);
  js_method_arg_assert_string(1);
  js_method_arg_assert_uint32(2);
  
  int32_t fd = js_method_arg_as_int32(0);
  std::string pathname = js_method_arg_as_std_string(1);
  uint32_t mask = js_method_arg_as_uint32(2);
  
  return JSInt32(inotify_add_watch(fd, pathname.c_str(), mask));
}

JS_METHOD_IMPL(__inotify_rm_watch)
{
  js_method_arg_assert_size_eq(2);
  js_method_arg_assert_int32(0);
  js_method_arg_assert_int32(1);

  
  int32_t fd = js_method_arg_as_int32(0);
  int32_t wd = js_method_arg_as_int32(0);
  
  return JSInt32(inotify_rm_watch(fd, wd));
}

JS_METHOD_IMPL(__inotify_get_events)
{
  js_method_arg_assert_size_eq(1);
  js_method_arg_assert_int32(0);
  int32_t fd = js_method_arg_as_int32(0);
  char buf[BUF_LEN] __attribute__ ((aligned(8)));
  uint32_t revents = 0;
  char *p = 0;
  
  int numRead = read(fd, buf, BUF_LEN);
  
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
  }
  
  return JSUInt32(revents);
}


JS_EXPORTS_INIT()
{
  js_export_method("inotify_init", __inotify_init);
  js_export_method("inotify_add_watch", __inotify_add_watch);
  js_export_method("inotify_rm_watch", __inotify_rm_watch);
  js_export_method("inotify_get_events", __inotify_get_events);
  
  /* Supported events suitable for MASK parameter of INOTIFY_ADD_WATCH.  */
  js_export_const(IN_ACCESS);           /* File was accessed.  */
  js_export_const(IN_MODIFY);           /* File was modified.  */
  js_export_const(IN_ATTRIB);           /* Metadata changed.  */
  js_export_const(IN_CLOSE_WRITE);      /* Writtable file was closed.  */
  js_export_const(IN_CLOSE_NOWRITE);    /* Unwrittable file closed.  */
  js_export_const(IN_CLOSE);            /* Close.  */
  js_export_const(IN_OPEN);             /* File was opened.  */
  js_export_const(IN_MOVED_FROM);       /* File was moved from X.  */
  js_export_const(IN_MOVED_TO);         /* File was moved to Y.  */
  js_export_const(IN_MOVE);             /* Moves.  */
  js_export_const(IN_CREATE);           /* Subfile was created.  */
  js_export_const(IN_DELETE);           /* Subfile was deleted.  */
  js_export_const(IN_DELETE_SELF);      /* Self was deleted.  */
  js_export_const(IN_MOVE_SELF);        /* Self was moved.  */

  /* Events sent by the kernel.  */
  js_export_const(IN_UNMOUNT);          /* Backing fs was unmounted.  */
  js_export_const(IN_Q_OVERFLOW);       /* Event queued overflowed.  */
  js_export_const(IN_IGNORED);          /* File was ignored.  */

  /* Special flags.  */
  js_export_const(IN_ONLYDIR);          /* Only watch the path if it is a
               directory.  */
  js_export_const(IN_DONT_FOLLOW);      /* Do not follow a sym link.  */
  js_export_const(IN_EXCL_UNLINK);      /* Exclude events on unlinked
               objects.  */
  js_export_const(IN_MASK_ADD);         /* Add to the mask of an already existing watch.  */
  js_export_const(IN_ISDIR);            /* Event occurred against dir.  */
  js_export_const(IN_ONESHOT);          /* Only send event once.  */

  /* All events which a program can wait on.  */
  js_export_const(IN_ALL_EVENTS);	
  
  js_export_finalize();
}

JS_REGISTER_MODULE(JSInotify);
