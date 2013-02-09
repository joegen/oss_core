##
## Libs from SipFoundry
##

## Common C and C++ flags for oss related source
AC_DEFUN([SFAC_INIT_FLAGS],
[
    m4_include([config/oss_lib2.m4])
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
    	#SF_CXX_WARNINGS="-Wall -Wformat -Wwrite-strings -Wpointer-arith"
    	SF_CXX_WARNINGS="-fpermissive"
        CXXFLAGS="$CXXFLAGS $SF_CXX_C_FLAGS $SF_CXX_WARNINGS"
    	CFLAGS="$CFLAGS $SF_CXX_C_FLAGS -Wnested-externs -Wmissing-declarations"

      # the sfac_strict_compile flag is set by SFAC_STRICT_COMPILE_NO_WARNINGS_ALLOWED
      AC_MSG_CHECKING(how to treat compilation warnings)
      #if test x$sfac_strict_compile = xstrictmode 
      #then 
      #   AC_MSG_RESULT([strict mode - treat as errors])
      #   CXXFLAGS="$CXXFLAGS -Wno-strict-aliasing -fno-strict-aliasing -Werror"
      #   CFLAGS="$CXXFLAGS -Wno-strict-aliasing -fno-strict-aliasing -Werror"
      #else 
      #   AC_MSG_RESULT([normal mode - allow warnings])
      #fi

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
AC_DEFUN([SFAC_LIB_CORE],
[
    AC_REQUIRE([SFAC_INIT_FLAGS])
    AC_REQUIRE([CHECK_SSL])

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

    OSS_CORE_VERSION_CURRENT="2"
    OSS_CORE_VERSION_REVISION="0"
    OSS_CORE_VERSION_AGE="0"
    OSS_CORE_VERSION_FULL="$OSS_CORE_VERSION_CURRENT.$OSS_CORE_VERSION_REVISION.$OSS_CORE_VERSION_AGE"
    AC_SUBST(OSS_CORE_VERSION_CURRENT)
    AC_SUBST(OSS_CORE_VERSION_REVISION)
    AC_SUBST(OSS_CORE_VERSION_AGE)
    AC_SUBST(OSS_CORE_VERSION_FULL)

    AC_CHECK_LIB(boost_date_time, main, [],
        [AC_CHECK_LIB(boost_date_time-mt, main, [],
        [AC_MSG_ERROR("Required boost library dependency missing.")])])

    AC_CHECK_LIB(boost_filesystem, main, [],
        [AC_CHECK_LIB(boost_filesystem-mt, main, [],
        [AC_MSG_ERROR("Required boost library dependency missing.")])])

    AC_CHECK_LIB(boost_system, main, [],
        [AC_CHECK_LIB(boost_system-mt, main, [],
        [AC_MSG_ERROR("Required boost library dependency missing.")])])

    AC_CHECK_LIB(boost_program_options, main, [],
        [AC_CHECK_LIB(boost_program_options-mt, main, [],
        [AC_MSG_ERROR("Required boost library dependency missing.")])])

    AC_CHECK_LIB(boost_iostreams, main, [],
        [AC_CHECK_LIB(boost_iostreams-mt, main, [],
        [AC_MSG_ERROR("Required boost library dependency missing.")])])

    AC_CHECK_LIB(boost_random, main, [],
        [AC_CHECK_LIB(boost_random-mt, main, [],
        [AC_MSG_ERROR("Required boost library dependency missing.")])])

    AC_CHECK_LIB(boost_regex, main, [],
        [AC_CHECK_LIB(boost_regex-mt, main, [],
        [AC_MSG_ERROR("Required boost library dependency missing.")])])

    AC_CHECK_LIB(boost_thread, main, [BOOST_LIBS="-lboost_date_time -lboost_filesystem -lboost_system -lboost_thread -lboost_program_options -lboost_iostreams -lboost_random -lboost_regex"],
        [AC_CHECK_LIB(boost_thread-mt, main,
        [BOOST_LIBS="-lboost_date_time-mt -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt -lboost_program_options-mt -lboost_iostreams-mt -lboost_random-mt -lboost_regex-mt"],
        [AC_MSG_ERROR("no boost thread found")])])
    AC_SUBST(BOOST_LIBS)

    
    AC_CHECK_LIB(PocoFoundation, main,
        [POCO_LIBS="-lPocoFoundation -lPocoUtil -lPocoNet -lPocoXML"],
        [AC_MSG_ERROR("Poco C++ Library not found")])
    AC_CHECK_LIB(PocoUtil, main,[], [AC_MSG_ERROR("PocoUtil C++ Library not found")])
    AC_CHECK_LIB(PocoNet, main,[], [AC_MSG_ERROR("PocoNet C++ Library not found")])
    AC_CHECK_LIB(PocoXML, main,[], [AC_MSG_ERROR("PocoXML C++ Library not found")])
    AC_SUBST(POCO_LIBS)

    AC_CHECK_LIB(hiredis, main, [], [AC_MSG_ERROR("Redis client library not found")])
    AC_CHECK_LIB(config++, main, [], [AC_MSG_ERROR("libconfig C++ library not found")])
    AC_CHECK_LIB(v8, main, [], [AC_MSG_ERROR("Google V8 Javascript engine not found")])
    AC_CHECK_LIB(ltdl, main, [], [AC_MSG_ERROR("libltdl not found")])
    AC_CHECK_LIB(mcrypt, main, [], [AC_MSG_ERROR("Mcrypt Encryption Library not found")])
    AC_CHECK_LIB(ssl, main, [], [AC_MSG_ERROR("SSL Development Library not found")])
    AC_CHECK_LIB(crypto, main, [], [AC_MSG_ERROR("libcrypto not found")])
    AC_CHECK_LIB(pthread, main, [], [AC_MSG_ERROR("libpthread not found")])
    AC_CHECK_LIB(dl, main, [], [AC_MSG_ERROR("libdl not found")])
    AC_CHECK_LIB(rt, main, [], [AC_MSG_ERROR("librt not found")])
    AC_CHECK_LIB(crypt, main, [], [AC_MSG_ERROR("libcrypt not found")])
    AC_CHECK_LIB(resolv, main, [], [AC_MSG_ERROR("libresolv not found")])
    AC_CHECK_LIB(pcap, main, [], [AC_MSG_ERROR("libpcap not found")])
    AC_CHECK_LIB(gtest, main, [], [AC_MSG_ERROR("Google Test Framework not found")])
    AC_CHECK_LIB(xmlrpc, main, [], [AC_MSG_ERROR("XML RPC C Foundation classes not found")])
    AC_CHECK_LIB(xmlrpc_client++, main, [], [AC_MSG_ERROR("XML RPC C++ client classes not found")])
    AC_CHECK_LIB(xmlrpc_server_abyss++, main, [], [AC_MSG_ERROR("XML RPC C++ server classes not found")])

    OSS_CORE_DEP_LIBS=" -lhiredis -lconfig++ -lv8 -lltdl -lmcrypt -lssl -lcrypto -lpthread -ldl -lrt -lcrypt -lresolv -lpcap -lxmlrpc -lxmlrpc_client++ -lxmlrpc_server_abyss++"
    AC_SUBST(OSS_CORE_DEP_LIBS, "$BOOST_LIBS $POCO_LIBS $OSS_CORE_DEP_LIBS")

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
    AC_SUBST(OSSLIBSCORE_LIBS, "$foundpath/liboss_core.la")
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
