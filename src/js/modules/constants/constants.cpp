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

#include "OSS/OSS.h"
#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  
  //
  // Mutable Properties
  //
  
  //
  // Standard fcntl system constants
  //
#ifdef __USE_XOPEN2K8
  CONST_EXPORT(O_DIRECTORY);    /* Must be a directory.	 */
  CONST_EXPORT(O_NOFOLLOW);     /* Do not follow links.	 */
  CONST_EXPORT(O_CLOEXEC);      /* Set close_on_exec.  */
#endif

#ifdef __USE_GNU
  CONST_EXPORT(O_DIRECT);       /* Direct disk access.	*/
  CONST_EXPORT(O_NOATIME);      /* Do not set atime.  */
  CONST_EXPORT(O_PATH);         /* Resolve pathname but do not open file.  */
#ifdef O_TMPFILE
  CONST_EXPORT(O_TMPFILE);      /* Atomically create nameless file.  */
#endif
#endif
  
  CONST_EXPORT(STDOUT_FILENO);
  CONST_EXPORT(STDIN_FILENO);
  CONST_EXPORT(STDERR_FILENO);
  
  //
  // Signals
  //
  CONST_EXPORT(SIGHUP);   /* Hangup (POSIX).  */
  CONST_EXPORT(SIGINT);   /* Interrupt (ANSI).  */
  CONST_EXPORT(SIGQUIT);  /* Quit (POSIX).  */
  CONST_EXPORT(SIGILL);   /* Illegal instruction (ANSI).  */
  CONST_EXPORT(SIGTRAP);  /* Trace trap (POSIX).  */
  CONST_EXPORT(SIGABRT);  /* Abort (ANSI).  */
  CONST_EXPORT(SIGIOT);   /* IOT trap (4.2 BSD).  */
  CONST_EXPORT(SIGBUS);   /* BUS error (4.2 BSD).  */
  CONST_EXPORT(SIGFPE);   /* Floating-point exception (ANSI).  */
  CONST_EXPORT(SIGKILL);  /* Kill, unblockable (POSIX).  */
  CONST_EXPORT(SIGUSR1);  /* User-defined signal 1 (POSIX).  */
  CONST_EXPORT(SIGSEGV);  /* Segmentation violation (ANSI).  */
  CONST_EXPORT(SIGUSR2);  /* User-defined signal 2 (POSIX).  */
  CONST_EXPORT(SIGPIPE);  /* Broken pipe (POSIX).  */
  CONST_EXPORT(SIGALRM);  /* Alarm clock (POSIX).  */
  CONST_EXPORT(SIGCHLD);  /* Child status has changed (POSIX).  */
  CONST_EXPORT(SIGCONT);  /* Continue (POSIX).  */
  CONST_EXPORT(SIGSTOP);  /* Stop, unblockable (POSIX).  */
  CONST_EXPORT(SIGTSTP);  /* Keyboard stop (POSIX).  */
  CONST_EXPORT(SIGTTIN);  /* Background read from tty (POSIX).  */
  CONST_EXPORT(SIGTTOU);  /* Background write to tty (POSIX).  */
  CONST_EXPORT(SIGURG);   /* Urgent condition on socket (4.2 BSD).  */
  CONST_EXPORT(SIGXCPU);  /* CPU limit exceeded (4.2 BSD).  */
  CONST_EXPORT(SIGXFSZ);  /* File size limit exceeded (4.2 BSD).  */
  CONST_EXPORT(SIGVTALRM);/* Virtual alarm clock (4.2 BSD).  */
  CONST_EXPORT(SIGPROF);  /* Profiling alarm clock (4.2 BSD).  */
  CONST_EXPORT(SIGSYS);   /* Bad system call.  */
  CONST_EXPORT(SIGTERM);  /* Termination (ANSI).  */
  CONST_EXPORT(SIGWINCH); /* Window size change (4.3 BSD, Sun).  */
  CONST_EXPORT(SIGIO);    /* I/O now possible (4.2 BSD).  */
#if !OSS_PLATFORM_MAC_OS_X
  CONST_EXPORT(SIGSTKFLT);/* Stack fault.  */
  CONST_EXPORT(SIGPWR);   /* Power failure restart (System V).  */
  CONST_EXPORT(SIGPOLL);  /* Pollable event occurred (System V).  */
  CONST_EXPORT(SIGCLD);   /* Same as SIGCHLD (System V).  */
#endif
  return exports;
}

JS_REGISTER_MODULE(Const);
