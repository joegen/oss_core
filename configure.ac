AC_PREREQ(2.57)

#
# Init library version
#
AC_INIT(oss_core, 2.0.3, info@ossapp.com)
OSS_CORE_VERSION_CURRENT="2"
OSS_CORE_VERSION_AGE="0"
OSS_CORE_VERSION_REVISION="3"

AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([subdir-objects foreign] )
AC_CONFIG_SRCDIR([src/OSS.cpp])
AM_CONFIG_HEADER([include/OSS/config.h])
AC_PROG_LIBTOOL
AC_LTDL_SHLIBEXT
AC_PATH_PROG([BASH], [bash])
AC_PROG_CXX
AX_COMPILER_VENDOR
AC_PROG_CC

#
# Export version variables
#
OSS_CORE_VERSION_INFO="$OSS_CORE_VERSION_CURRENT:$OSS_CORE_VERSION_REVISION:$OSS_CORE_VERSION_AGE"
AC_SUBST(OSS_CORE_VERSION_CURRENT)
AC_SUBST(OSS_CORE_VERSION_REVISION)
AC_SUBST(OSS_CORE_VERSION_AGE)
AC_SUBST(OSS_CORE_VERSION_INFO)

#
# --disable-dep-check will simply display missing 
# dependencies as warnings.  This is useful when you
# simply need to generate the makefile for tasks that
# do not require building.  Example is "make dist"
#
AC_ARG_ENABLE(dep-check,
      AS_HELP_STRING([--disable-dep-check],
      [Do not check for dependencies other then those required for dist target]))

AC_DEFUN([ERROR_MISSING_DEP],[
    if test "x$enable_dep_check" != "xno"; then
        AC_MSG_ERROR([$2])
    fi
    AC_SUBST([$1], 0)
])

AC_DEFUN([ENABLE_FEATURE],[
    AM_CONDITIONAL([ENABLE_FEATURE_$1], true)
    AC_SUBST([ENABLE_FEATURE_$1], 1)
    AC_MSG_NOTICE([$1 feature ENABLED])
])

AC_DEFUN([DISABLE_FEATURE],[
    AM_CONDITIONAL([ENABLE_FEATURE_$1], false)
    AC_SUBST([ENABLE_FEATURE_$1], 0)
    AC_MSG_NOTICE([$1 feature DISABLED])
])


#
# Enable REST KV compilation
#
AC_ARG_ENABLE([restkv],
    AC_HELP_STRING([--enable-turn], [Enable REST Key Value Store Feature]),
    [ENABLE_FEATURE(RESTKV)],
    [DISABLE_FEATURE(RESTKV)])

#
# Enable TURN compilation
#
AC_ARG_ENABLE([turn],
    AC_HELP_STRING([--enable-turn], [Enable TURN Server Feature]),
    [ENABLE_FEATURE(TURN)],
    [DISABLE_FEATURE(TURN)])

#
# Enable libre UA wrapper compilation
#
AC_ARG_ENABLE([libre],
    AC_HELP_STRING([--enable-libre], [Enable libre UA Feature]),
    [ENABLE_FEATURE(LIBRE)],
    [DISABLE_FEATURE(LIBRE)])

#
# Enable UCARP compilation
#
AC_ARG_ENABLE([carp],
    AC_HELP_STRING([--enable-carp], [Enable CARP-HA Feature]),
    [ENABLE_FEATURE(CARP)],
    [DISABLE_FEATURE(CARP)])

#
# Enable V8 compilation
#
AC_ARG_ENABLE([v8],
    AC_HELP_STRING([--enable-v8], [Enable V8 JavaScript Feature]),
    [ENABLE_FEATURE(V8)],
    [DISABLE_FEATURE(V8)])

#
# Enable B2BUA compilation
#
AC_ARG_ENABLE([b2bua],
    AC_HELP_STRING([--enable-b2bua], [Enable B2BUA Feature]),
    [ENABLE_FEATURE(B2BUA)],
    [DISABLE_FEATURE(B2BUA)])

#
# Enable Websocket compilation
#
AC_ARG_ENABLE([websockets],
    AC_HELP_STRING([--enable-websockets], [Enable Websockets Feature]),
    [ENABLE_FEATURE(WEBSOCKETS)],
    [DISABLE_FEATURE(WEBSOCKETS)])


#
# Enable RTP Proxy compilation
#
AC_ARG_ENABLE([rtp],
    AC_HELP_STRING([--enable-rtp], [Enable RTP Proxy Feature]),
    [ENABLE_FEATURE(RTP)],
    [DISABLE_FEATURE(RTP)])

#
# Flag dependecy whether they are missing or not
#
AC_DEFUN([FLAG_MISSING_DEP],[
    AC_SUBST([$1], 0)
])

#
# Flag existing CXX dependencies
#
AC_DEFUN([FLAG_EXISTING_CXX_DEP],[
    AC_SUBST([$1], 1)
    OSS_CORE_DEP_LIBS="$2 $OSS_CORE_DEP_LIBS"
])

#
# Search for mandatory packages
#

#
# OpenSSL
#
AC_CHECK_HEADER(openssl/conf.h,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_SSL, -lssl)], 
    [ERROR_MISSING_DEP(OSS_HAVE_SSL,"OpenSSL Library not installed")])

AC_CANONICAL_HOST
case $host_os in
    darwin* )
        OSS_CORE_DEP_LIBS="-L/usr/local/opt/openssl/lib $OSS_CORE_DEP_LIBS"
        ;;

esac


#
# Boost
#

AC_CHECK_LIB(boost_thread-mt, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_THREAD, -lboost_thread-mt)],
    [AC_CHECK_LIB(boost_thread, main,
    	[FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_THREAD, -lboost_thread)],
    	[ERROR_MISSING_DEP(OSS_HAVE_BOOST_THREAD, "Boost Thread Library is not installed")])])

AC_CHECK_LIB(boost_date_time-mt, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_DATE_TIME, -lboost_date_time-mt)],
    [AC_CHECK_LIB(boost_date_time, main,
    	[FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_DATE_TIME, -lboost_date_time)],
    	[ERROR_MISSING_DEP(OSS_HAVE_BOOST_DATE_TIME, "Boost Date Library is not installed")])])

AC_CHECK_LIB(boost_filesystem-mt, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_FILESYSTEM, -lboost_filesystem-mt)],
    [AC_CHECK_LIB(boost_filesystem, main,
    	[FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_FILESYSTEM, -lboost_filesystem)],
    	[ERROR_MISSING_DEP(OSS_HAVE_BOOST_FILESYSTEM, "Boost Filesystem Library is not installed")])])

AC_CHECK_LIB(boost_system-mt, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_SYSTEM, -lboost_system-mt)],
    [AC_CHECK_LIB(boost_system, main,
    	[FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_SYSTEM, -lboost_system)],
    	[ERROR_MISSING_DEP(OSS_HAVE_BOOST_SYSTEM, "Boost System Library is not installed")])])

AC_CHECK_LIB(boost_program_options-mt, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_PROGRAM_OPTIONS, -lboost_program_options-mt)],
    [AC_CHECK_LIB(boost_program_options, main,
    	[FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_PROGRAM_OPTIONS, -lboost_program_options)],
    	[ERROR_MISSING_DEP(OSS_HAVE_BOOST_PROGRAM_OPTIONS, "Boost System Library is not installed")])])

AC_CHECK_LIB(boost_iostreams-mt, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_IOSTREAMS, -lboost_iostreams-mt)],
    [AC_CHECK_LIB(boost_iostreams, main,
    	[FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_IOSTREAMS, -lboost_iostreams)],
    	[ERROR_MISSING_DEP(OSS_HAVE_BOOST_IOSTREAMS, "Boost Iostreams Library is not installed")])])

AC_CHECK_LIB(boost_regex-mt, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_REGEX, -lboost_regex-mt)],
    [AC_CHECK_LIB(boost_regex, main,
    	[FLAG_EXISTING_CXX_DEP(OSS_HAVE_BOOST_regex, -lboost_regex)],
    	[ERROR_MISSING_DEP(OSS_HAVE_BOOST_REGEX, "Boost Regex Library is not installed")])])


#
# POCO
#
AC_CHECK_LIB(PocoFoundation, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_POCO_FOUNDATION, -lPocoFoundation)],
    [ERROR_MISSING_DEP(OSS_HAVE_POCO_FOUNDATION,"Poco C++ Foundation Library not found")])

AC_CHECK_LIB(PocoUtil, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_POCO_UTIL, -lPocoUtil)],
    [ERROR_MISSING_DEP(OSS_HAVE_POCO_UTIL,"Poco C++ Util Library not found")])

AC_CHECK_LIB(PocoNet, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_POCO_NET, -lPocoNet -lPocoNetSSL)],
    [ERROR_MISSING_DEP(OSS_HAVE_POCO_NET,"Poco C++ Net Library not found")])



#
# Search for optional packages
#

AC_CHECK_HEADER(libconfig.h,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_CONFIGPP, -lconfig++)], 
    [FLAG_MISSING_DEP(OSS_HAVE_CONFIGPP)])

AC_CHECK_HEADER(hiredis/hiredis.h,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_HIREDIS, -lhiredis)], 
    [FLAG_MISSING_DEP(OSS_HAVE_HIREDIS)])

AC_LANG_PUSH([C++])
AC_CHECK_HEADER(v8.h, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_V8, -lv8)],
    [FLAG_MISSING_DEP(OSS_HAVE_V8)])
AC_LANG_POP([C++])

AC_CHECK_HEADER(leveldb/db.h, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_LEVELDB, -lleveldb)], 
    [FLAG_MISSING_DEP(OSS_HAVE_LEVELDB)])

AC_CHECK_LIB(gtest, main, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_GTEST, -lgtest)],
    [FLAG_MISSING_DEP(OSS_HAVE_GTEST)])

AC_CHECK_LIB(ltdl, main, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_LTDL, -lltdl)],
    [FLAG_MISSING_DEP(OSS_HAVE_LTDL)])

AC_CHECK_HEADER(mcrypt.h,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_MCRYPT, -lmcrypt)], 
    [FLAG_MISSING_DEP(OSS_HAVE_MCRYPT)])


AC_CHECK_LIB(crypto, main, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_CRYPTO, -lcrypto)], 
    [FLAG_MISSING_DEP(OSS_HAVE_CRYPTO)])

AC_CHECK_LIB(pthread, main, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_THREAD, -lpthread)], 
    [FLAG_MISSING_DEP(OSS_HAVE_THREAD)])

AC_CHECK_LIB(dl, main,
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_DL, -ldl)], 
    [FLAG_MISSING_DEP(OSS_HAVE_DL)])

AC_CHECK_LIB(rt, main,     
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_RT, -lrt)], 
    [FLAG_MISSING_DEP(OSS_HAVE_RT)])

AC_CHECK_LIB(z, main, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_ZLIB, -lz)], 
    [FLAG_MISSING_DEP(OSS_HAVE_ZLIB)])

AC_CHECK_LIB(crypt, main, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_CRYPT, -lcrypt)], 
    [FLAG_MISSING_DEP(OSS_HAVE_CRYPT)])

AC_CHECK_LIB(resolv, main, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_RESOLV, -lresolv)], 
    [FLAG_MISSING_DEP(OSS_HAVE_RESOLV)])

AC_CHECK_LIB(pcap, main, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_PCAP, -lpcap)], 
    [FLAG_MISSING_DEP(OSS_HAVE_PCAP)])

AC_CHECK_LIB(gtest, main, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_GTEST, -lgtest)], 
    [FLAG_MISSING_DEP(OSS_HAVE_GTEST)])

AC_CHECK_LIB(zmq, main, 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_ZMQ, -lzmq)], 
    [FLAG_MISSING_DEP(OSS_HAVE_ZMQ)])

AC_CHECK_LIB([event], [main],     
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_LIBEVENT, -levent)], 
    [FLAG_MISSING_DEP(OSS_HAVE_LIBEVENT)])

AC_CHECK_LIB([event_openssl], [main], 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_LIBEVENT_SSL, -levent_openssl)], 
    [FLAG_MISSING_DEP(OSS_HAVE_LIBEVENT_SSL)])

AC_CHECK_LIB([event_pthreads], [main], 
    [FLAG_EXISTING_CXX_DEP(OSS_HAVE_LIBEVENT_PTHREADS, -levent_pthreads)], 
    [FLAG_MISSING_DEP(OSS_HAVE_LIBEVENT_PTHREAD)])

CURRENT_DIR=`pwd`
cd ${srcdir}
OSS_CORE_SRCDIR=`pwd`
AC_SUBST(OSS_CORE_SRCDIR)
cd ${CURRENT_DIR}

#
# Additional CXX and C Flags
#
OSS_CXX_C_FLAGS="-fmessage-length=0"
OSS_CXX_WARNINGS="-Wall -Wformat -Wwrite-strings -Wpointer-arith"
CXXFLAGS="$CXXFLAGS $OSS_CXX_C_FLAGS $OSS_CXX_WARNINGS"
CFLAGS="$CFLAGS $OSS_CXX_C_FLAGS -Wnested-externs"

AC_SUBST(CXXFLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(OSS_CORE_DEP_LIBS)

AC_CONFIG_FILES([
    include/OSS/OSS.h
    include/OSS/build.h
    Makefile
    src/Makefile
    include/Makefile
    src/ua/core/Makefile
])

AC_OUTPUT
