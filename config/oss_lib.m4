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



AC_DEFUN([SFAC_DOWNLOAD_DEPENDENCIES],
[  
  # URLs to files pulled down files
  AC_SUBST(RUBY_AUX_RPMS_URL, http://people.redhat.com/dlutter/yum)
  AC_SUBST(MOD_CPLUSPLUS_URL, http://umn.dl.sourceforge.net/sourceforge/modcplusplus)
  AC_SUBST(JPKG_FREE_URL, http://mirrors.dotsrc.org/jpackage/1.7/generic/free)
  AC_SUBST(JPKG_NONFREE_URL, http://mirrors.dotsrc.org/jpackage/1.7/generic/non-free)
  AC_SUBST(XERCES_C_URL, http://www.apache.org/dist/xerces/c/sources)
  AC_SUBST(RUBY_RPM_URL, http://dev.centos.org/centos/4/testing)
  AC_SUBST(FC4_RUBY_RPM_URL, http://download.fedora.redhat.com/pub/fedora/linux/core/updates/4)
  AC_SUBST(W3C_URL, http://ftp.redhat.com/pub/redhat/linux/enterprise/4/en/os/i386)
  AC_SUBST(W3C_SRC_URL, http://www.w3.org/Library/Distribution)
  AC_SUBST(GRAPHVIZ_URL, ftp://194.199.20.114/linux/SuSE-Linux/i386/9.3/suse/src)
  AC_SUBST(CENTOS_URL, http://mirrors.easynews.com/linux/centos)
  AC_SUBST(FC5_URL, http://redhat.download.fedoraproject.org/pub/fedora/linux/core)
  AC_SUBST(FC5_EXTRAS_URL, http://download.fedora.redhat.com/pub/fedora/linux/extras)
  AC_SUBST(FC5_UPDATES_URL, http://download.fedora.redhat.com/pub/fedora/linux/core/updates)
  AC_SUBST(RHEL5_URL, http://mirrors.kernel.org/centos)
  AC_SUBST(RHEL5_UPDATES_URL, http://mirrors.kernel.org/centos/5/updates)
  AC_SUBST(FC6_URL, http://mirrors.kernel.org/fedora/core)
  AC_SUBST(FC6_UPDATES_URL, http://mirrors.kernel.org/fedora/core/updates)
  AC_SUBST(FC6_EXTRAS_URL, http://mirrors.kernel.org/fedora/extras)
  AC_SUBST(RRDTOOL_URL, http://oss.oetiker.ch/rrdtool/pub)
  AC_SUBST(JAIN_SIP_URL, http://download.java.net/communications/jain-sip/nightly/jain-sip-src)
  AC_SUBST(DNSJAVA_URL, http://www.dnsjava.org/download/)

  SFAC_SRCDIR_EXPAND
  download_file="$abs_srcdir/config/download-file"
  AC_SUBST(DOWNLOAD_FILE, $download_file)
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
