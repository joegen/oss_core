# common directories and variables used in oss
AC_PREFIX_DEFAULT([/usr/local/oss])

# This is a "common fix" to a "known issue" with using ${prefix} variable
test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'

AC_SUBST(OSSLIBS_INCDIR, [${includedir}])
AC_SUBST(OSSLIBS_LIBDIR, [${libdir}])
AC_SUBST(OSSLIBS_LIBEXECDIR, [${libexecdir}/oss])
AC_SUBST(OSSLIBS_BINDIR,  [${bindir}])
AC_SUBST(OSSLIBS_CONFDIR, [${sysconfdir}/karoo.conf.d])
AC_SUBST(OSSLIBS_DATADIR, [${datadir}/oss])
AC_SUBST(OSSLIBS_INTEROP, [${datadir}/oss/interop])
AC_SUBST(OSSLIBS_DOCDIR,  [${datadir}/doc/oss])
AC_SUBST(OSSLIBS_JAVADIR, [${datadir}/java/oss])
AC_SUBST(OSSLIBS_VARDIR,  [${localstatedir}/run/karoo])
AC_SUBST(OSSLIBS_TMPDIR,  [${localstatedir}/run/karoo/tmp])
AC_SUBST(OSSLIBS_DBDIR,   [${localstatedir}/run/karoo/data])
AC_SUBST(OSSLIBS_LOGDIR,  [${localstatedir}/log/karoo])
AC_SUBST(OSSLIBS_RUNDIR,  [${localstatedir}/run/karoo])
AC_SUBST(OSSLIBS_VARLIB,  [${localstatedir}/lib/karoo])
# oss RPMs should be hardcoded to use karoobridge user for their oss user, not the buildbot user
AC_SUBST(OSSLIBS_RPM_CONFIGURE_OPTIONS,  [OSSLIBSSYSUSER=karoobridge])

# Get the user to run oss_ under.
AC_ARG_VAR(OSSLIBSSYSUSER, [The oss_ service daemon user name, default is ${USER}])
test -n "$OSSLIBSSYSUSER" || OSSLIBSSYSUSER=$USER

# Get the group to run oss_ under.
AC_ARG_VAR(OSSLIBSPBXGROUP, [The oss_ service daemon group name, default is value of OSSLIBSSYSUSER])
test -n "$OSSLIBSPBXGROUP" || OSSLIBSPBXGROUP=$OSSLIBSSYSUSER

PACKAGE_REVISION=`cd ${srcdir} && ./config/revision-gen ${PACKAGE_VERSION}`

AC_SUBST(PACKAGE_REVISION)
AC_DEFINE_UNQUOTED([PACKAGE_REVISION], "${PACKAGE_REVISION}", [Revion number including git SHA])
