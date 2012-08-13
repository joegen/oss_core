# common directories and variables used in oss
AC_PREFIX_DEFAULT([/usr/local/oss])

# This is a "common fix" to a "known issue" with using ${prefix} variable
test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'

AC_SUBST(OSSLIBS_INCDIR, [${includedir}])
AC_SUBST(OSSLIBS_LIBDIR, [${libdir}])
AC_SUBST(OSSLIBS_LIBEXECDIR, [${libexecdir}/oss_core])
AC_SUBST(OSSLIBS_BINDIR,  [${bindir}])
AC_SUBST(OSSLIBS_CONFDIR, [${sysconfdir}/oss_core.d])
AC_SUBST(OSSLIBS_DATADIR, [${datadir}/oss_core])
AC_SUBST(OSSLIBS_DOCDIR,  [${datadir}/doc/oss_core])
AC_SUBST(OSSLIBS_JAVADIR, [${datadir}/java/oss_core])
AC_SUBST(OSSLIBS_VARDIR,  [${localstatedir}/run/oss_core])
AC_SUBST(OSSLIBS_TMPDIR,  [${localstatedir}/run/oss_core/tmp])
AC_SUBST(OSSLIBS_DBDIR,   [${localstatedir}/run/oss_core/data])
AC_SUBST(OSSLIBS_LOGDIR,  [${localstatedir}/log/oss_core])
AC_SUBST(OSSLIBS_RUNDIR,  [${localstatedir}/run/oss_core])
AC_SUBST(OSSLIBS_VARLIB,  [${localstatedir}/lib/oss_core])
# oss RPMs should be hardcoded to use karoo user for their oss_core user, not the buildbot user
AC_SUBST(OSSLIBS_RPM_CONFIGURE_OPTIONS,  [OSSLIBSSYSUSER=oss_core])

# Get the user to run oss_ under.
AC_ARG_VAR(OSSLIBSSYSUSER, [The oss_ service daemon user name, default is ${USER}])
test -n "$OSSLIBSSYSUSER" || OSSLIBSSYSUSER=$USER

# Get the group to run oss_core under.
AC_ARG_VAR(OSSLIBSPBXGROUP, [The oss_core service daemon group name, default is value of OSSLIBSSYSUSER])
test -n "$OSSLIBSPBXGROUP" || OSSLIBSPBXGROUP=$OSSLIBSSYSUSER

PACKAGE_REVISION=`cd ${srcdir} && ./config/revision-gen ${PACKAGE_VERSION}`

AC_SUBST(PACKAGE_REVISION)
AC_DEFINE_UNQUOTED([PACKAGE_REVISION], "${PACKAGE_REVISION}", [Revion number including git SHA])
