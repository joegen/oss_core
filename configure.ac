AC_PREREQ(2.57)
AC_INIT(oss_core, 2.0.0, info@ossapp.com)
AC_CONFIG_AUX_DIR(config)
m4_include([config/general.m4])
m4_include([config/oss_lib.m4])
AM_INIT_AUTOMAKE(foreign tar-ustar)
SFAC_AUTOMAKE_VERSION([1.6])
AC_CONFIG_SRCDIR([src/Core.cpp])
AM_CONFIG_HEADER([src/config.h])
AC_PROG_LIBTOOL
AC_CHECK_LIB(boost_thread, main, [BOOST_LIBS="-lboost_date_time -lboost_filesystem -lboost_system -lboost_thread -lboost_program_options -lboost_iostreams -lboost_random -lboost_regex"],
    [AC_CHECK_LIB(boost_thread-mt, main,
    [BOOST_LIBS="-lboost_date_time-mt -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt -lboost_program_options-mt -lboost_iostreams-mt -lboost_random-mt -lboost_regex-mt"],
    [AC_MSG_ERROR("no boost thread found")])])
AC_SUBST(BOOST_LIBS)
AC_LTDL_SHLIBEXT
AC_PATH_PROG([BASH], [bash])
AC_PROG_CXX
AX_COMPILER_VENDOR
AC_PROG_CC
CHECK_XARGS_REPLACE
CHECK_MSG_NOSIGNAL
CHECK_LIBRT
AC_SEARCH_LIBS([dlopen], [dl])
CHECK_SSL
SFAC_INIT_FLAGS
AC_CHECK_FUNCS(setenv)
AC_CONFIG_FILES([
  Makefile
  include/Makefile
  src/Makefile
])

AC_OUTPUT
