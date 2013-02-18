# Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([memory.h strings.h unistd.h netinet/in_systm.h])
AC_CHECK_HEADERS([netinet/in.h stdlib.h string.h sys/socket.h sys/time.h])
AC_CHECK_HEADERS([ifaddrs.h net/if_dl.h net/if_types.h])
AC_CHECK_HEADERS([sys/sockio.h net/if_arp.h], [], [],
[[
#include <sys/types.h>
#ifdef HAS_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAS_NETINET_IN_H
#include <netinet/in.h>
#endif
]])
AC_CHECK_HEADERS([fcntl.h sys/fcntl.h ioctl.h sys/ioctl.h])

dnl Check for endianness
AC_C_BIGENDIAN

dnl Check for types
AC_TYPE_SIGNAL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_TYPE(dev_t, , [AC_DEFINE(dev_t, unsigned int, [dev_t type])],
[
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
])

dnl Check for sizes
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

dnl Socket things

AC_CHECK_FUNC(connect, , [AC_CHECK_LIB(socket, connect)])
AC_CHECK_FUNC(gethostbyname, , [AC_CHECK_LIB(resolv, gethostbyname)])
AC_CHECK_FUNC(gethostbyname, , [AC_CHECK_LIB(nsl, gethostbyname)])

if test "x$ac_cv_lib_nsl_gethostbyname" != "xyes" && test "x$ac_cv_func_gethostbyname" != "xyes" ; then
  AC_CHECK_FUNC(gethostbyname, , [AC_CHECK_LIB(socket, gethostbyname)])
fi

if test "$ac_cv_lib_nsl_gethostbyname" = "$ac_cv_func_gethostbyname" ; then
  AC_MSG_CHECKING([if we can include libnsl + libsocket])
  LIBS="-lnsl -lsocket $LIBS"
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[(void) gethostbyname]])],[my_ac_link_result=yes],
                 [my_ac_link_result=no])
  if test "$my_ac_link_result" = "no" ; then
    AC_MSG_RESULT([failure])
    AC_MSG_ERROR([unable to use gethostbyname()])
  else
    AC_MSG_RESULT([success])
  fi
fi

# Checks for libraries.
AC_CHECK_LIB([pcap], [pcap_lookupdev])

dnl Types - continued

AC_CHECK_TYPE(socklen_t, , [AC_DEFINE(socklen_t, int, [socklen_t type])],
[
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
])

AC_CHECK_TYPE(ssize_t, , [AC_DEFINE(ssize_t, long int, [ssize_t type])],
[
#include <stdio.h>
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <sys/types.h>
])

AC_CHECK_TYPE(sig_atomic_t, , [AC_DEFINE(sig_atomic_t, signed char,
                                         [sig_atomic_t type])],
[
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <sys/types.h>
#include <signal.h>
])

AC_CHECK_TYPE(nfds_t, , [AC_DEFINE(nfds_t, unsigned long, [nfds_t type])],
[
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/poll.h>
])

dnl Check for typedefs, structures, and compiler characteristics.
AC_PROG_GCC_TRADITIONAL
AC_C_CONST
AC_C_INLINE

dnl Options

AC_ARG_WITH(debug,
[AS_HELP_STRING(--with-debug,For maintainers only - please do not use)],
[ if test "x$withval" = "xyes" ; then
    CFLAGS="$CFLAGS -DDEBUG=1 -g -Wall -W -Wcast-align -Wbad-function-cast -Wstrict-prototypes -Wwrite-strings -Wreturn-type "
  fi ])

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MEMCMP
AC_FUNC_STRFTIME
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_FUNC_FORK
AC_FUNC_WAIT3

AC_CHECK_FUNCS(initgroups setrlimit waitpid setproctitle getopt_long)
AC_CHECK_FUNCS(seteuid setreuid setresuid setegid setregid setresgid)
AC_CHECK_FUNCS(strtoull strtoq)
AC_CHECK_FUNCS(memset munmap strdup fileno)
AC_CHECK_FUNCS(getaddrinfo getnameinfo inet_ntop inet_pton)
AC_CHECK_FUNCS(snprintf vsnprintf vfprintf gethostname)
AC_CHECK_FUNCS(setlocale)
AC_CHECK_FUNCS(random srandomdev arc4random)
AC_CHECK_FUNCS(getifaddrs)

if test "x$sysconfdir" = 'xNONE' || test "x$sysconfdir" = 'x'; then
  CONFDIR='/etc'
else
  if test "x$sysconfdir" = 'x${prefix}/etc'; then
    if test "x$prefix" = 'xNONE' || test "x$prefix" = 'x/usr'; then
      CONFDIR='/etc'
    else
      CONFDIR="$sysconfdir"
    fi
  else
    CONFDIR="$sysconfdir"
  fi
fi

if test -r /dev/arandom; then
  AC_MSG_NOTICE([You have /dev/arandom - Great])
  AC_DEFINE(HAVE_DEV_ARANDOM,,[Define if you have /dev/arandom])
fi
if test -r /dev/urandom; then
  AC_MSG_NOTICE([You have /dev/urandom - Great])
  AC_DEFINE(HAVE_DEV_URANDOM,,[Define if you have /dev/urandom])
fi
if test -r /dev/random; then
  AC_MSG_NOTICE([You have /dev/random - Great])
  AC_DEFINE(HAVE_DEV_RANDOM,,[Define if you have /dev/random])
fi

AC_MSG_CHECKING(whether snprintf is C99 conformant)
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdio.h>
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

int main(void)
{
    char buf[4];

    (void) fprintf(fopen("conftestval", "w"), "%d\n",
        (int) snprintf(buf, sizeof buf, "12345678"));
    return 0;
}
]])],[CONF_SNPRINTF_TYPE=`cat conftestval`
],[],[])
AC_MSG_RESULT(done)
if test "x$CONF_SNPRINTF_TYPE" = "x" ; then
  AC_MSG_WARN(your operating system doesn't implement snprintf)
else
  AC_DEFINE_UNQUOTED(CONF_SNPRINTF_TYPE, $CONF_SNPRINTF_TYPE,
                     [return value of an overflowed snprintf])
fi

AC_MSG_CHECKING(whether you already have a standard SHA1 implementation)
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sha1.h>

int main(void)
{
    SHA1_CTX ctx;
    char b[41];

    SHA1Init(&ctx);
    SHA1Update(&ctx, (const unsigned char *) "test", 4U);
    SHA1End(&ctx, b);
    b[40] = 0;

    return strcasecmp(b, "a94a8fe5ccb19ba61c4c0873d391e987982fbbd3");
}
]])],[
AC_MSG_RESULT(yes)
AC_DEFINE(USE_SYSTEM_CRYPT_SHA1,,[Define if you already have standard
SHA1 functions])
],[AC_MSG_RESULT(no)
],[])

AC_MSG_CHECKING([whether syslog names are available])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#define SYSLOG_NAMES 1
#include <stdio.h>
#include <syslog.h>
]], [[
 (void) facilitynames
]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_SYSLOG_NAMES,,[define if syslog names are available])
],[
  AC_MSG_RESULT(no)
])

sysconfdir="$CONFDIR"
AC_SUBST(sysconfdir)
CPPFLAGS="$CPPFLAGS -DCONFDIR=\\\"$sysconfdir\\\""