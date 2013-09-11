##
## Libs from SipFoundry
##

## Common C and C++ flags for oss related source
AC_DEFUN([SFAC_INIT_FLAGS],
[
    m4_include([config/oss_lib2.m4])
    m4_include([config/carp.m4])
    AC_REQUIRE([SFAC_OSSLIBS_GLOBAL_OPTS])

    
    ## TODO Remove cpu specifics and use make variables setup for this
    ##
    ## NOTES:
    ##   -D_REENTRANT             - 
    ##   -fmessage-length=0       - ?
    ##

    CFLAGS="-I${prefix}/include $CFLAGS"
    CXXFLAGS="-I${prefix}/include $CXXFLAGS"
    LDFLAGS="-L${prefix}/lib ${LDFLAGS}"

    if test x_"${ax_cv_c_compiler_vendor}" = x_gnu
    then
    	SF_CXX_C_FLAGS="-D_linux_ -D_REENTRANT -D_FILE_OFFSET_BITS=64 -fmessage-length=0"
    	SF_CXX_WARNINGS="-Wall -Wformat -Wwrite-strings -Wpointer-arith"
    	#SF_CXX_WARNINGS="-fpermissive"
        CXXFLAGS="$CXXFLAGS $SF_CXX_C_FLAGS $SF_CXX_WARNINGS"
    	CFLAGS="$CFLAGS $SF_CXX_C_FLAGS -Wnested-externs -Wmissing-declarations"
    elif test x_"${ax_cv_c_compiler_vendor}" = x_sun
    then
        SF_CXX_C_FLAGS="-D_REENTRANT -D_FILE_OFFSET_BITS=64 -mt -fast -v"
        SF_CXX_FLAGS="-D_REENTRANT -D_FILE_OFFSET_BITS=64 -mt -xlang=c99 -fast -v"
        SF_CXX_WARNINGS=""
        CXXFLAGS="$CXXFLAGS $SF_CXX_FLAGS $SF_CXX_WARNINGS"
        CFLAGS="$CFLAGS $SF_CXX_C_FLAGS $SF_CXX_WARNINGS"
    else
        SF_CXX_C_FLAGS="-D_linux_ -D_REENTRANT -D_FILE_OFFSET_BITS=64"
        SF_CXX_WARNINGS=""
        CXXFLAGS="$CXXFLAGS $SF_CXX_C_FLAGS $SF_CXX_WARNINGS"
        CFLAGS="$CFLAGS $SF_CXX_C_FLAGS $SF_CXX_WARNINGS"
    fi

    ## set flag for gcc
    AM_CONDITIONAL(ISGCC, [test  x_"${GCC}" != x_])
])

# oss_-specific options that affect everything and so should be visible at the top level
AC_DEFUN([SFAC_OSSLIBS_GLOBAL_OPTS],
[
    AC_ARG_ENABLE(rpmbuild, 
      AC_HELP_STRING([--enable-rpmbuild], [Build an rpm]),
      enable_rpmbuild=yes)

    AC_ARG_ENABLE(buildnumber,
                 AC_HELP_STRING([--enable-buildnumber],
                                [Enable build number as part of RPM name]),
                 enable_buildnumber=yes)


    AC_ARG_VAR(OSSLIB_NAME, [Label for oss, default is 'oss'])
    test -z "$OSSLIB_NAME" && OSSLIB_NAME='oss'

    # Enable profiling via gprof
    ENABLE_PROFILE
])

## Check to see that we are using the minimum required version of automake
AC_DEFUN([SFAC_AUTOMAKE_VERSION],[
   AC_MSG_CHECKING(for automake version >= $1)
   sf_am_version=`automake --version | head -n 1 | awk '/^automake/ {print $NF}'`
   AX_COMPARE_VERSION( [$1], [le], [$sf_am_version], AC_MSG_RESULT($sf_am_version is ok), AC_MSG_ERROR(found $sf_am_version - you must upgrade automake))
])

## oss_coreLib 
# SFAC_LIB_CORE attempts to find the sf portability library and include
# files by looking in /usr/[lib|include], /usr/local/[lib|include], and
# relative paths.
#
# If not found, the configure is aborted.  Otherwise, variables are defined
# for both the INC and LIB paths 
# AND the paths are added to the CFLAGS and CXXFLAGS

AC_DEFUN([SF_MISSING_DEP],[
  if test "x$enable_dep_check" != "xno"; then
    AC_MSG_ERROR([$1])
  fi
])


AC_DEFUN([SFAC_LIB_CORE_FLAGS],
[
    AC_REQUIRE([SFAC_INIT_FLAGS])
    AC_REQUIRE([CHECK_SSL])

    AC_ARG_ENABLE(dep-check,
      AS_HELP_STRING([--disable-dep-check],
      [Do not check for dependencies other then those required for dist target]))

    OSSLIBSCOREINC=$foundpath
    CFLAGS="-I${prefix}/include $CFLAGS"
    CXXFLAGS="-I${prefix}/include $CXXFLAGS"

    OSS_CORE_VERSION_CURRENT="2"
    OSS_CORE_VERSION_REVISION="0"
    OSS_CORE_VERSION_AGE="0"
    OSS_CORE_VERSION_FULL="$OSS_CORE_VERSION_CURRENT.$OSS_CORE_VERSION_REVISION.$OSS_CORE_VERSION_AGE"
    OSS_CORE_VERSION_INFO="$OSS_CORE_VERSION_CURRENT:$OSS_CORE_VERSION_REVISION:$OSS_CORE_VERSION_AGE"
    AC_SUBST(OSS_CORE_VERSION_CURRENT)
    AC_SUBST(OSS_CORE_VERSION_REVISION)
    AC_SUBST(OSS_CORE_VERSION_AGE)
    AC_SUBST(OSS_CORE_VERSION_FULL)
    AC_SUBST(OSS_CORE_VERSION_INFO)

    #
    # Check if iproute2 is installed
    #
    AC_CHECK_FILE(/sbin/ip, [OSS_IP_ROUTE_2="/sbin/ip"], [SF_MISSING_DEP("Install iproute2 package")])
    AC_SUBST(OSS_IP_ROUTE_2)
    CXXFLAGS="-DOSS_IP_ROUTE_2=\\\"$OSS_IP_ROUTE_2\\\" $CXXFLAGS"

    #
    # Check if iptables is installed
    #
    AC_CHECK_FILE(/sbin/iptables, [OSS_IP_TABLES="/sbin/iptables"], [SF_MISSING_DEP("Install iptables package")])
    AC_SUBST(OSS_IP_TABLES)
    CXXFLAGS="-DOSS_IP_TABLES=\\\"$OSS_IP_TABLES\\\" $CXXFLAGS"

    AC_CHECK_LIB(boost_thread-mt, main,
        [BOOST_LIBS="-lboost_date_time-mt -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt -lboost_program_options-mt -lboost_iostreams-mt -lboost_regex-mt"],
        [AC_CHECK_LIB(boost_thread, main,
        [BOOST_LIBS="-lboost_date_time -lboost_filesystem -lboost_system -lboost_thread -lboost_program_options -lboost_iostreams -lboost_regex"],
        [SF_MISSING_DEP("no boost thread found")])])
    AC_CHECK_LIB(boost_random-mt, main, [BOOST_LIBS="$BOOST_LIBS -lboost_random-mt"],
        [AC_CHECK_LIB(boost_random, main,
        [BOOST_LIBS="$BOOST_LIBS -lboost_random"])])
    AC_SUBST(BOOST_LIBS)

    CXXFLAGS="-DBOOST_THREAD_DONT_USE_CHRONO $CXXFLAGS"


    AC_CHECK_LIB(PocoFoundation, main,
        [POCO_LIBS="-lPocoFoundation -lPocoUtil -lPocoNet -lPocoXML"],
        [SF_MISSING_DEP("Poco C++ Library not found")])
    AC_CHECK_LIB(PocoUtil, main,[], [SF_MISSING_DEP("PocoUtil C++ Library not found")])
    AC_CHECK_LIB(PocoNet, main,[], [SF_MISSING_DEP("PocoNet C++ Library not found")])
    AC_CHECK_LIB(PocoXML, main,[], [SF_MISSING_DEP("PocoXML C++ Library not found")])
    AC_SUBST(POCO_LIBS)

    AC_CHECK_LIB(hiredis, main, [], [SF_MISSING_DEP("Redis client library not found")])
    AC_CHECK_LIB(config++, main, [], [SF_MISSING_DEP("libconfig C++ library not found")])
    AC_CHECK_LIB(v8, main, [], [SF_MISSING_DEP("Google V8 Javascript engine not found")])
    AC_CHECK_LIB(gtest, main, [], [SF_MISSING_DEP("Google Test Framework not found")])
    AC_CHECK_LIB(ltdl, main, [], [SF_MISSING_DEP("libltdl not found")])
    AC_CHECK_LIB(mcrypt, main, [], [SF_MISSING_DEP("Mcrypt Encryption Library not found")])
    AC_CHECK_LIB(ssl, main, [], [SF_MISSING_DEP("SSL Development Library not found")])
    AC_CHECK_LIB(crypto, main, [], [SF_MISSING_DEP("libcrypto not found")])
    AC_CHECK_LIB(pthread, main, [], [SF_MISSING_DEP("libpthread not found")])
    AC_CHECK_LIB(dl, main, [], [SF_MISSING_DEP("libdl not found")])
    AC_CHECK_LIB(rt, main, [], [SF_MISSING_DEP("librt not found")])
    AC_CHECK_LIB(crypt, main, [], [SF_MISSING_DEP("libcrypt not found")])
    AC_CHECK_LIB(resolv, main, [], [SF_MISSING_DEP("libresolv not found")])
    AC_CHECK_LIB(pcap, main, [], [SF_MISSING_DEP("libpcap not found")])
    AC_CHECK_LIB(gtest, main, [], [SF_MISSING_DEP("Google Test Framework not found")])
    AC_CHECK_LIB(xmlrpc, main, [], [SF_MISSING_DEP("XML RPC C Foundation classes not found")])
    AC_CHECK_LIB(xmlrpc_client++, main, [], [SF_MISSING_DEP("XML RPC C++ client classes not found")])
    AC_CHECK_LIB(xmlrpc_server_abyss++, main, [], [SF_MISSING_DEP("XML RPC C++ server classes not found")])


    OSS_CORE_DEP_LIBS=""
    OSS_CORE_DEP_LIBS+=" -lgtest "
    OSS_CORE_DEP_LIBS+=" -lhiredis "
    OSS_CORE_DEP_LIBS+=" -lconfig++  "
    OSS_CORE_DEP_LIBS+=" -lv8  "
    OSS_CORE_DEP_LIBS+=" -lltdl  "
    OSS_CORE_DEP_LIBS+=" -lmcrypt  "
    OSS_CORE_DEP_LIBS+=" -lssl  "
    OSS_CORE_DEP_LIBS+=" -lcrypto "
    OSS_CORE_DEP_LIBS+=" -lpthread  "
    OSS_CORE_DEP_LIBS+=" -ldl  "
    OSS_CORE_DEP_LIBS+=" -lrt  "
    OSS_CORE_DEP_LIBS+=" -lcrypt  "
    OSS_CORE_DEP_LIBS+=" -lresolv  "
    OSS_CORE_DEP_LIBS+=" -lpcap  "
    OSS_CORE_DEP_LIBS+=" -lxmlrpc "
    OSS_CORE_DEP_LIBS+=" -lxmlrpc_client++  "
    OSS_CORE_DEP_LIBS+=" -lxmlrpc_server_abyss++ "

    #
    # Check for TURN dependencies
    #

    AC_CHECK_LIB([event], [main], [], libevent_found=no)
    AC_CHECK_LIB([event_openssl], [main], [], libevent_ssl_found=no)
    AC_CHECK_LIB([event_pthreads], [main], [], libevent_thread_found=no)

    AM_CONDITIONAL([ENABLE_TURN], false)

    AC_ARG_ENABLE(turn,
      AC_HELP_STRING([--enable-turn], [Build TURN Server]),
      enable_turn=yes)

    if test "x$enable_turn" == "xyes"; then
        if test "x$libevent_thread" = "xno" -a "x$libevent_thread" = "xno"; then
            echo "libevent 2 not found.  Disabling TURN compilation"
        else
            if test "x$libevent_thread_found" = "xno" -a "x$libevent_thread_found" = "xno"; then
                echo "libevent 2 not found.  Disabling TURN compilation"
            else
                if test "x$libevent_ssl_found" = "xno" -a "x$libevent_ssl_found" = "xno"; then
                   echo "libevent 2 not found.  Disabling TURN compilation"
                else
                   echo "TURN compilation enabled"
                   AM_CONDITIONAL([ENABLE_TURN], true)
                   AC_DEFINE([ENABLE_TURN], [1], [Flags TURN compilation due to the presence of libevent 2])
                fi
            fi
        fi
    fi

    AC_SUBST(OSS_CORE_DEP_LIBS, "$BOOST_LIBS $POCO_LIBS $OSS_CORE_DEP_LIBS")

]) # SFAC_LIB_CORE_FLAGS

AC_DEFUN([SFAC_LIB_CORE],
[
    foundpath=""

    SFAC_ARG_WITH_INCLUDE([OSS/Core.h],
            [osscoreinc],
            [ --with-osscoreinc=<dir> portability include path ],
            [oss_coreLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_WARN([    assuming it will be in '${prefix}/include'])
        foundpath=${prefix}/include
    fi
    OSSLIBSCOREINC=$foundpath
    AC_SUBST(OSSLIBSCOREINC)

    CFLAGS="-I$OSSLIBSCOREINC $CFLAGS"
    CXXFLAGS="-I$OSSLIBSCOREINC $CXXFLAGS"

    foundpath=""
    SFAC_ARG_WITH_LIB([liboss_core.la],
            [osscorelib],
            [ --with-osscorelib=<dir> portability library path ],
            [oss_coreLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_WARN([    assuming it will be in '${prefix}/lib'])
        foundpath=${prefix}/lib
    fi
    AC_SUBST(LIB_OSS_CORE_LA, "$foundpath/liboss_core.la")


    foundpath=""
    SFAC_ARG_WITH_LIB([liboss_carp.la],
            [osscarplib],
            [ --with-osscarplib=<dir> carp library path ],
            [oss_carpLib])

    if test x_$foundpath != x_; then
        AC_MSG_RESULT($foundpath)
    else
        AC_MSG_WARN([    assuming it will be in '${prefix}/lib'])
        foundpath=${prefix}/lib
    fi
    AC_SUBST(LIB_OSS_CARP_LA, "$foundpath/liboss_carp.la")

    AC_ARG_ENABLE(turn,
      AC_HELP_STRING([--enable-turn], [Build TURN Server]),
      enable_turn=yes)

    if test "x$enable_turn" == "xyes"; then
        foundpath=""
        SFAC_ARG_WITH_LIB([liboss_turn.la],
                [ossturnlib],
                [ --with-ossturnlib=<dir> turn library path ],
                [oss_turnLib])

        if test x_$foundpath != x_; then
            AC_MSG_RESULT($foundpath)
        else
            AC_MSG_WARN([    assuming it will be in '${prefix}/lib'])
            foundpath=${prefix}/lib
        fi
        AC_SUBST(LIB_OSS_TURN_LA, "$foundpath/liboss_turn.la")
    fi

    AC_REQUIRE([SFAC_LIB_CORE_FLAGS])

]) # SFAC_LIB_CORE


##  Generic find of an include
#   Fed from AC_DEFUN([SFAC_INCLUDE_{module name here}],
#
# $1 - sample include file
# $2 - variable name (for overridding with --with-$2
# $3 - help text
# $4 - directory name (assumed parallel with this script)
AC_DEFUN([SFAC_ARG_WITH_INCLUDE],
[
    SFAC_SRCDIR_EXPAND()

    AC_MSG_CHECKING(for [$4] includes)
    AC_ARG_WITH( [$2],
        [ [$3] ],
        [ include_path=$withval ],
        [ include_path="$includedir ${prefix}/include /usr/include /usr/local/include" ]
    )
    foundpath=""
    for dir in $include_path ; do
        if test -f "$dir/[$1]";
        then
            foundpath=$dir;
            break;
        fi;
    done
    if test x_$foundpath = x_; then
       AC_MSG_WARN('$1' not found)
       AC_MSG_WARN([    searched $include_path])
    fi
        

]) # SFAC_ARG_WITH_INCLUDE


##  Generic find of a library
#   Fed from AC_DEFUN([SFAC_LIB_{module name here}],
#
# $1 - sample lib file
# $2 - variable name (for overridding with --with-$2
# $3 - help text
# $4 - directory name (assumed parallel with this script)
AC_DEFUN([SFAC_ARG_WITH_LIB],
[
    SFAC_SRCDIR_EXPAND()

    AC_MSG_CHECKING(for [$4] libraries)
    AC_ARG_WITH( [$2],
        [ [$3] ],
        [ lib_path=$withval ],
        [ lib_path="$libdir ${prefix}/lib /usr/lib /usr/local/lib"]
    )
    foundpath=""
    for dir in $lib_path ; do
        if test -f "$dir/[$1]";
        then
            foundpath=$dir;
            break;
        fi;
    done
    if test x_$foundpath = x_; then
       AC_MSG_WARN('$1' not found)
       AC_MSG_WARN([    searched $lib_path])
    fi
]) # SFAC_ARG_WITH_LIB


AC_DEFUN([SFAC_SRCDIR_EXPAND], 
[
    abs_srcdir=`cd $srcdir && pwd`
    abs_build_dir=`pwd`
    AC_SUBST(TOP_ABS_SRCDIR, $abs_srcdir)
    AC_SUBST(TOP_ABS_BUILDDIR, $abs_srcdir)
    AC_SUBST(TOP_SRCDIR, $srcdir)
])


# This allows configuring where the script that starts oss gets installed
AC_DEFUN([CHECK_SERVICEDIR],
[
  AC_ARG_WITH([servicedir],
    AC_HELP_STRING([--with-servicedir=directory],
      [The directory containing scripts that start services]),
    [SERVICEDIR=${withval}],[SERVICEDIR='$(sysconfdir)/init.d'])
  AC_SUBST([SERVICEDIR])
])
