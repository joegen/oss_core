AC_PREREQ(2.57)

#
# Init library version
#
AC_INIT(oss_core, 4.0.0, info@ossapp.com)
OSS_CORE_VERSION_CURRENT="4"
OSS_CORE_VERSION_AGE="0"
OSS_CORE_VERSION_REVISION="0"

AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([subdir-objects foreign] )
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_SRCDIR([src/OSS.cpp])
AM_CONFIG_HEADER([src/config.h])
AC_PROG_LIBTOOL
AC_LTDL_SHLIBEXT
AC_PATH_PROG([BASH], [bash])
AC_PROG_CXX
AC_PROG_CC
AC_CANONICAL_HOST


#
# Configure package revision number
#
PACKAGE_REVISION=`date +%s`.`cd ${srcdir} && git rev-parse --short HEAD`
AC_SUBST(PACKAGE_REVISION)
AC_DEFINE_UNQUOTED([PACKAGE_REVISION], "${PACKAGE_REVISION}", [Revion number including git SHA])

m4_include([oss_core.m4])

AC_CONFIG_FILES([
    include/OSS/build.h
    Makefile
    oss_core.spec
    debian.control
    src/Makefile
    include/Makefile
])

AC_OUTPUT
