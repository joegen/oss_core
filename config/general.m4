##
## AC macros for general packages like OpenSSL, Xerces, etc
##

# ============ C L O V E R  =======================
AC_DEFUN([CHECK_CLOVER],
[
   AC_ARG_VAR(CLOVER_JAR, [Clover home directory])

   if test x_"${CLOVER_JAR}" != x_
   then
       AC_CHECK_FILE([$CLOVER_JAR],
       [
           CLOVER_JAR=$CLOVER_RPM_JAR
       ],
       [
           AC_MSG_ERROR([Invalid CLOVER_JAR environment variable: Cannot find $CLOVER_JAR.])
       ])
   else
       CLOVER_RPM_JAR=/usr/share/java/ant/clover.jar
       AC_CHECK_FILE([$CLOVER_RPM_JAR],
       [
           CLOVER_JAR=$CLOVER_RPM_JAR
       ],)
   fi
])

# ================ NET SNMP ================
AC_DEFUN([CHECK_NET_SNMP],
[
    AC_MSG_CHECKING([for net-snmp-config.h ])
    include_path="$includedir $prefix/include /usr/include /usr/local/include"
    include_check="net-snmp/net-snmp-config.h"

    foundpath=""
    for dir in $include_path ; do
        if test -f "$dir/$include_check";
        then
            foundpath=$dir;
            break;
        fi;
    done
    if test x_$foundpath = x_; then
       AC_MSG_RESULT([no])
       AC_MSG_WARN([searched $include_path    ])
       AC_MSG_ERROR('$include_check' not found)
    else
       AC_MSG_RESULT($foundpath/$include_check)
    fi

    AC_MSG_CHECKING([for net-snmp-includes.h ])
    include_path="$includedir $prefix/include /usr/include /usr/local/include"
    include_check="net-snmp/net-snmp-includes.h"

    foundpath=""
    for dir in $include_path ; do
        if test -f "$dir/$include_check";
        then
            foundpath=$dir;
            break;
        fi;
    done
    if test x_$foundpath = x_; then
       AC_MSG_RESULT([no])
       AC_MSG_WARN([searched $include_path    ])
       AC_MSG_ERROR('$include_check' not found)
    else
       AC_MSG_RESULT($foundpath/$include_check)
    fi
])


# ============ J D K  =======================
AC_DEFUN([CHECK_JDK],
[
    AC_ARG_VAR(JAVA_HOME, [Java Development Kit])

    TRY_JAVA_HOME=`ls -dr /usr/java/* 2> /dev/null | head -n 1`
    for dir in $JAVA_HOME $JDK_HOME /usr/lib/jvm/java /usr/lib64/jvm/java /usr/local/jdk /usr/local/java $TRY_JAVA_HOME; do
        AC_CHECK_FILE([$dir/lib/dt.jar],[jar=$dir/lib/dt.jar])
        if test x$jar != x; then
            found_jdk="yes";
            JAVA_HOME=$dir
            break;
        fi
    done

    if test x_$found_jdk != x_yes; then
        AC_MSG_ERROR([Cannot find dt.jar in expected location. You may try setting the JAVA_HOME environment variable if you haven't already done so])
    fi

    AC_SUBST(JAVA, [$JAVA_HOME/jre/bin/java])

    AC_ARG_VAR(JAVAC_OPTIMIZED, [Java compiler option for faster performance. Default is on])
    test -z $JAVAC_OPTIMIZED && JAVAC_OPTIMIZED=on

    AC_ARG_VAR(JAVAC_DEBUG, [Java compiler option to reduce code size. Default is off])
    test -z $JAVAC_DEBUG && JAVAC_DEBUG=off
])


# ============ J N I =======================
AC_DEFUN([CHECK_JNI],
[
   AC_REQUIRE([CHECK_JDK])

   JAVA_HOME_INCL=$JAVA_HOME/include
   AC_CHECK_FILE([$JAVA_HOME_INCL/jni.h],
       [
           XFLAGS="-I$JAVA_HOME_INCL -I$JAVA_HOME_INCL/linux";
           CFLAGS="$XFLAGS $CFLAGS";
           CXXFLAGS="$XFLAGS $CXXFLAGS";

           ## i386 is a big assumption, TODO: make smarter
           JAVA_LIB_DIR="$JAVA_HOME/jre/lib/i386";

           ## Effectively LD_LIBRARY_PATH for JVM for unittests or anything else
           AC_SUBST(JAVA_LIB_PATH, [$JAVA_LIB_DIR:$JAVA_LIB_DIR/client])

           LDFLAGS="$LDFLAGS -L$JAVA_LIB_DIR -ljava -lverify"

           ## Use client flags as only call for this is phone. config server
           ## should use jre/lib/i386/server, but not a big deal
           LDFLAGS="$LDFLAGS -L$JAVA_LIB_DIR/client -ljvm"
       ],
       AC_MSG_ERROR([Cannot find or validate header file $JAVA_HOME_INCL/jni.h]))
])


# ============ A N T  ==================
AC_DEFUN([CHECK_ANT],
[
   AC_REQUIRE([AC_EXEEXT])
   AC_REQUIRE([CHECK_JDK])
   AC_ARG_VAR(ANT, [Ant program])

   test -z $ANT_HOME || ANT_HOME_BIN=$ANT_HOME/bin
   for dir in $ANT_HOME_BIN $PATH /usr/local/ant/bin; do
       # only works because unix does not use ant.sh
       AC_PATH_PROG(ANT, ant$EXEEXT ant.bat, ,$dir)
       if test x$ANT != x; then
           found_ant="yes";
           break;
       fi
   done

   if test x_$found_ant != x_yes; then
       AC_MSG_ERROR([Cannot find ant program. Try setting ANT_HOME environment variable or use 'configure ANT=<path to ant executable>])
   fi

  AC_SUBST(ANT_FLAGS, '-e -Dtop.build.dir=$(shell cd $(top_builddir) && pwd) -f $(srcdir)/build.xml')
  AC_SUBST(ANT_CMD, "JAVA_HOME=${JAVA_HOME} ${ANT}")
])


# ============ O P E N S S L ==================
#
# OpenSSL is required
#
AC_DEFUN([CHECK_SSL],
[   AC_ARG_WITH(openssl,
                [  --with-openssl=PATH to openssl source directory],
                [openssl_path=$withval],
                [openssl_path="/usr/local /usr/local/ssl /usr/lib/x86_64-linux-gnu /usr/ssl /usr/pkg /usr / /sw/lib"]
                )
    AC_PATH_PROG([OPENSSL],[openssl])
    AC_MSG_CHECKING([for openssl includes])
    found_ssl_inc="no";
    tried_path=""
    for dir in $openssl_path ; do
        if test -f "$dir/openssl/ssl.h"; then
            found_ssl_inc="yes";
            sslincdir="$dir"
            break;
        elif test -f "$dir/include/openssl/ssl.h"; then
            found_ssl_inc="yes";
            sslincdir="$dir/include"
            break;
        else
            tried_path="${tried_path} $dir $dir/include"
        fi
    done
    if test x_$found_ssl_inc != x_yes ; then
        AC_MSG_ERROR(['openssl/ssl.h' not found; tried ${tried_path}])
    else
        AC_MSG_RESULT($sslincdir)
        HAVE_SSL=yes
        AC_SUBST(HAVE_SSL)
        SSL_CFLAGS="-DHAVE_SSL"

        # don't need to add -I/usr/include
        if test "${sslincdir}" != "/usr/include"; then
           SSL_CFLAGS="$SSL_CFLAGS -I$sslincdir"
        fi
    fi

    AC_MSG_CHECKING([for openssl libraries])
    found_ssl_lib="no";
    for libsubdir in lib lib64 lib/x86_64-linux-gnu i386-linux-gnu lib32 lib/hpux32; do
      for dir in $openssl_path ; do
        if test -f "$dir/$libsubdir/libssl.so" -o -f "$dir/$libsubdir/libssl.dylib" -o -f "$dir/$libsubdir/libssl.a"; then
            found_ssl_lib="yes";
            ssllibdir="$dir/lib"
            break;
        # This test is an ugly hack to make sure that the current builds work.
        # But our test should be improved to allow libssl.so to have any version
        # and let the test succeed, since "-lssl" works with any version number.
        elif test -f "$dir/$libsubdir/libssl.so.4"; then
            found_ssl_lib="yes";
            ssllibdir="$dir/lib"
            break;
        elif test -f "$dir/$libsubdir/openssl/libssl.so"; then
            found_ssl_lib="yes";
            ssllibdir="$dir/$libsubdir/openssl"
            break;
        elif test -f "$dir/$libsubdir/ssl/libssl.so"; then
            found_ssl_lib="yes";
            ssllibdir="$dir/$libsubdir/ssl"
            break;
        fi
      done
    done

    if test x_$found_ssl_lib != x_yes ; then
        AC_MSG_ERROR(['libssl.so' not found; tried $openssl_path, each with lib, lib/openssl, and lib/ssl])
    else
        AC_MSG_RESULT($ssllibdir)
       if test x_"`uname -s`" = x_SunOS ; then
          AC_SUBST(SSL_LDFLAGS,"-L$ssllibdir -R$ssllibdir")
	else
	  AC_SUBST(SSL_LDFLAGS,"-L$ssllibdir")
	fi
        AC_SUBST(SSL_LIBS,"-lssl -lcrypto")
    fi

## openSUSE installs kerberos as part of krb5-devel in /usr/include
    AC_MSG_CHECKING(for extra kerberos includes)
    krb_found="no"
    if test -f "/usr/include/krb5.h"; then
      krb_found="yes"
      AC_MSG_RESULT(/usr/include/krb5.h)
    fi
## openssl-devel rpm sometimes installs kerberos in another dir
    if test x_$krb_found = x_no; then
      for krbdir in $openssl_path ; do
        if test -f "$krbdir/kerberos/include/krb5.h"; then
          krb_found="yes"
          AC_MSG_RESULT($krbdir/kerberos/include)
          SSL_CFLAGS="$SSL_CFLAGS -I$krbdir/kerberos/include"
          break;
        fi
      done
    fi
    if test x_$krb_found = x_no; then
        AC_MSG_RESULT(['kerberos krb5.h' not found - looked in /usr/include $openssl_path])
    fi

    AC_SUBST(SSL_CFLAGS,"$SSL_CFLAGS")
    AC_SUBST(SSL_CXXFLAGS,"$SSL_CFLAGS")
])


# ============ L I B R T  =========================
AC_DEFUN([CHECK_LIBRT],
[
   AC_MSG_CHECKING([for librt])

   rt_found="no"
   for dir in /lib /usr/lib /usr/local/lib; do
      if test -f "$dir/librt.so.1"; then
        rt_found="yes"
        break;
      fi
   done
   if test x_$rt_found = x_yes; then
        AC_SUBST(RT_LIBS,"-lrt")
	AC_MSG_RESULT([-lrt])
   else
        AC_SUBST(RT_LIBS,"")
        AC_MSG_RESULT([not needed])
   fi
])


# ============ L I B O B J C  =====================
AC_DEFUN([CHECK_LIBOBJC],
[
   AC_MSG_CHECKING([for libobjc])

   objc_found="no"
   for dir in /lib /usr/lib /usr/local/lib; do
      if test -f "$dir/libobjc.dylib"; then
        objc_found="yes"
        break;
      fi
   done
   if test x_$objc_found = x_yes; then
        AC_SUBST(OBJC_LIBS,"-lobjc /usr/lib/libstdc++.6.dylib")
        AC_MSG_RESULT([-lobjc])
   else
        AC_SUBST(OBJC_LIBS,"")
        AC_MSG_RESULT([not needed])
   fi
])


# ============ C O R E A U D I O =======================
AC_DEFUN([CHECK_COREAUDIO],
[
   AC_MSG_CHECKING([for CoreAudio])

   if test "`uname`" = "Darwin"; then
        AC_SUBST(CA_LIBS,"-framework CoreAudio -framework AudioToolbox")
	AC_MSG_RESULT([yes])
   else
        AC_SUBST(CA_LIBS,"")
	AC_MSG_RESULT([not needed])
   fi
])


# ============ X E R C E S ==================
AC_DEFUN([CHECK_XERCES],
[
   AC_MSG_CHECKING([for xerces])
    AC_ARG_WITH(xerces,
                [  --with-xerces=PATH to xerces source directory],
                [xerces_path=$withval],
                [xerces_path="/usr/local/xercesc /usr/lib/xercesc /usr/xercesc /usr/pkg /usr/local /usr"]
                )
    # try older version 2.8 version first
    for dir in $xerces_path ; do
        xercesdir="$dir"
        if test -f "$dir/include/xercesc/dom/DOMBuilder.hpp"; then
            found_xerces="yes";
            XERCES_CFLAGS="-I$xercesdir/include -DXERCES_2_8";
            break;
        fi
    done
    # set or upgrade them if 3.0+ version if found
    for dir in $xerces_path ; do
        xercesdir="$dir"
        if test -f "$dir/include/xercesc/dom/DOMLSParser.hpp"; then
            found_xerces="yes";
            XERCES_CFLAGS="-I$xercesdir/include";
            break;
        fi
    done

    if test x_$found_xerces != x_yes; then
        AC_MSG_ERROR([Cannot find xerces - looked for include/xercesc/dom/DOMBuilder.hpp or /include/xercesc/dom/DOMBuilder.hpp in $xerces_path])
    else
        AC_MSG_RESULT($xercesdirm)
        AC_SUBST(XERCES_CFLAGS,"$XERCES_CFLAGS")
        AC_SUBST(XERCES_CXXFLAGS,"$XERCES_CFLAGS")
        AC_SUBST(XERCES_LIBS,["-lxerces-c"])
        AC_SUBST(XERCES_LDFLAGS,["-L$xercesdir/lib"])
    fi
])

# ============ A P A C H E 2 ==================
AC_DEFUN([CHECK_APACHE2],
[
   ## Apache httpd executable
   AC_MSG_CHECKING([for Apache2 httpd])
   AC_ARG_WITH([apache-httpd],
               [--with-apache-httpd=PATH the apache2 httpd executable],
               [ apache2_bin_search_path="$withval"
                 ],
               [ apache2_bin_search_path="/usr/local/apache2/bin /usr/apache2/bin /etc/httpd/bin /usr/local/sbin /usr/local/bin /usr/sbin /usr/bin /usr/sbin/apache2"
                 ]
               )

   found_apache2_httpd="no";
   for apache2_httpd_dir in $apache2_bin_search_path; do
     if test -x "$apache2_httpd_dir/httpd"; then
       found_apache2_httpd="yes";
       apache2_httpd="$apache2_httpd_dir/httpd";
       break;
     elif test -x "$apache2_httpd_dir/httpd2"; then
       found_apache2_httpd="yes";
       apache2_httpd="$apache2_httpd_dir/httpd2";
       break;
     elif test -f "$apache2_httpd_dir" -a -x "$apache2_httpd_dir"; then
       found_apache2_httpd="yes";
       apache2_httpd="$apache2_httpd_dir";
       break;
     fi
   done
   if test x_$found_apache2_httpd = x_yes; then
       AC_MSG_RESULT($apache2_httpd)
       AC_SUBST(APACHE2_HTTPD, $apache2_httpd)
   else
       AC_MSG_ERROR('httpd' not found; tried: $apache2_bin_search_path)
   fi

   ## APACHE2_VERSION is the Apache version number.
   ## This makes it easier for the uninitiated to see what versions of Apache
   ## might be compatible with this mod_cplusplus.  But compatibility is really
   ## controlled by the MMN value.
   apache2_version=`$apache2_httpd -version | sed -n -e 's,Server version: Apache/,,p'`
   AC_SUBST(APACHE2_VERSION, $apache2_version)
   AC_MSG_RESULT(apache2_version=$apache2_version)
   AC_MSG_CHECKING(which apache host access module to use)
   case $apache2_version in
   2.2.*)
      apache2_host_access="authz_host_module"
      apache2_mod_access="mod_authz_host.so"
      ;;
   2.0.*)
      apache2_host_access="access_module"
      apache2_mod_access="mod_access.so"
      ;;
   *)
      apache2_host_access="UNKNOWN"
      apache2_mod_access="UNKNOWN"
      AC_MSG_ERROR(Unknown apache version $apache2_version)
      ;;
   esac
   AC_MSG_RESULT($apache2_host_access = $apache2_mod_access)
   AC_SUBST(APACHE2_HOST_ACCESS, $apache2_host_access)
   AC_SUBST(APACHE2_MOD_ACCESS, $apache2_mod_access)

   ## Apache Modules Directory
   AC_MSG_CHECKING([for apache2 modules directory])
   AC_ARG_WITH([apache-modules],
               [--with-apache-modules=PATH where apache modules are installed],
               [ apache2_mod_search_path="$withval"
                 apache2_mod_override="$withval"
                ],
               [ apache2_mod_search_path="/usr/local/apache2/modules /usr/apache2/modules /etc/httpd/modules /usr/lib/httpd/modules
/usr/lib/apache2-prefork /usr/lib/apache2-worker /usr/lib/apache2/modules /usr/lib64/apache2 /usr/lib64/apache2-prefork /usr/lib64/apache2-worker"
                 apache2_mod_override=""
                ]
              )
   found_apache2_mod="no";
   tried_path=""
   ## Older versions of Apache seem to always have mod_access.so in their
   ## modules directory.  Newer ones can have it linked into the httpd
   ## executable, but they seem to have an httpd.exp file in the modules
   ## directory.  apache 2.2 has mod_cgi.so, So we check for any of them.
   for apache2_moddir in $apache2_mod_search_path; do
     if test -f "$apache2_moddir/$apache2_mod_access"; then
       found_apache2_mod="yes";
       break;
     elif test -f "$apache2_moddir/httpd.exp"; then
       found_apache2_mod="yes";
       break;
     elif test -f "$apache2_moddir/mod_cgi.so"; then
       found_apache2_mod="yes";
       break;
     else
       tried_path="${tried_path} $apache2_moddir"
     fi
   done
   if test x_$found_apache2_mod = x_yes; then
       AC_MSG_RESULT($apache2_moddir)
       AC_SUBST(APACHE2_MOD, $apache2_moddir)
   elif test x_$apache2_mod_override != x_; then
       AC_SUBST(APACHE2_MOD, $apache2_mod_override)
       AC_MSG_WARN('$apache2_mod_access', 'mod_cgi.so', and 'httpd.exp' not found; using explicit value: $tried_path)
   else
       AC_MSG_ERROR('$apache2_mod_access' and 'httpd.exp' not found; tried: $tried_path)
   fi
])dnl

# ================ ZLIB ================
AC_DEFUN([CHECK_ZLIB],
[
    have_zlib='no'
    LIB_ZLIB=''
    if test "$with_zlib" != 'no'
    then
      LIB_ZLIB=''
      AC_MSG_CHECKING(for ZLIB support )
      AC_MSG_RESULT()
      failed=0;
      passed=0;
      AC_CHECK_HEADER(zconf.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
      AC_CHECK_HEADER(zlib.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
      AC_CHECK_LIB(z,compress,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
      AC_CHECK_LIB(z,uncompress,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
      AC_CHECK_LIB(z,deflate,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
      AC_CHECK_LIB(z,inflate,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
      AC_CHECK_LIB(z,gzseek,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
      AC_CHECK_LIB(z,gztell,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
      AC_MSG_CHECKING(if ZLIB package is complete)
      if test $passed -gt 0
      then
        if test $failed -gt 0
        then
          AC_MSG_RESULT(no -- some components failed test)
          have_zlib='no (failed tests)'
        else
          LIB_ZLIB='-lz'
          LIBS="$LIB_ZLIB $LIBS"
          AC_DEFINE(HasZLIB,1,Define if you have zlib compression library)
          AC_MSG_RESULT(yes)
          have_zlib='yes'
        fi
      else
        AC_MSG_RESULT(no)
        AC_MSG_ERROR(ZLIB required)
      fi
    fi
    AM_CONDITIONAL(HasZLIB, test "$have_zlib" = 'yes')
    AC_SUBST(LIB_ZLIB)
])




dnl @synopsis AX_COMPARE_VERSION(VERSION_A, OP, VERSION_B, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
dnl
dnl This macro compares two version strings.  It is used heavily in the
dnl macro _AX_PATH_BDB for library checking. Due to the various number
dnl of minor-version numbers that can exist, and the fact that string
dnl comparisons are not compatible with numeric comparisons, this is
dnl not necessarily trivial to do in a autoconf script.  This macro makes
dnl doing these comparisons easy.
dnl
dnl The six basic comparisons are available, as well as checking
dnl equality limited to a certain number of minor-version levels.
dnl
dnl The operator OP determines what type of comparison to do, and
dnl can be one of:
dnl  eq  - equal (test A == B)
dnl  ne  - not equal (test A != B)
dnl  le  - less than or equal (test A <= B)
dnl  ge  - greater than or equal (test A >= B)
dnl  lt  - less than (test A < B)
dnl  gt  - greater than (test A > B)
dnl
dnl Additionally, the eq and ne operator can have a number after it
dnl to limit the test to that number of minor versions.
dnl  eq0 - equal up to the length of the shorter version
dnl  ne0 - not equal up to the length of the shorter version
dnl  eqN - equal up to N sub-version levels
dnl  neN - not equal up to N sub-version levels
dnl
dnl When the condition is true, shell commands ACTION-IF-TRUE are run,
dnl otherwise shell commands ACTION-IF-FALSE are run.  The environment
dnl variable 'ax_compare_version' is always set to either 'true' or 'false'
dnl as well.
dnl
dnl Examples:
dnl   AX_COMPARE_VERSION([3.15.7],[lt],[3.15.8])
dnl   AX_COMPARE_VERSION([3.15],[lt],[3.15.8])
dnl would both be true.
dnl
dnl   AX_COMPARE_VERSION([3.15.7],[eq],[3.15.8])
dnl   AX_COMPARE_VERSION([3.15],[gt],[3.15.8])
dnl would both be false.
dnl
dnl   AX_COMPARE_VERSION([3.15.7],[eq2],[3.15.8])
dnl would be true because it is only comparing two minor versions.
dnl   AX_COMPARE_VERSION([3.15.7],[eq0],[3.15])
dnl would be true because it is only comparing the lesser number of
dnl minor versions of the two values.
dnl
dnl Note: The characters that separate the version numbers do not
dnl matter.  An empty string is the same as version 0.  OP is evaluated
dnl by autoconf, not configure, so must be a string, not a variable.
dnl
dnl The author would like to acknowledge Guido Draheim whose advice about
dnl the m4_case and m4_ifvaln functions make this macro only include
dnl the portions necessary to perform the specific comparison specified
dnl by the OP argument in the final configure script.
dnl
dnl @version $Id: ax_compare_version.m4,v 1.1 2004/03/01 19:11.53 guidod Exp $
dnl @author Tim Toolan <toolan@ele.uri.edu>
dnl
dnl #########################################################################
AC_DEFUN([AX_COMPARE_VERSION], [
  # Used to indicate true or false condition
  ax_compare_version=false

  # Convert the two version strings to be compared into a format that
  # allows a simple string comparison.  The end result is that a version
  # string of the form 1.12.5-r617 will be converted to the form
  # 0001001200050617.  In other words, each number is zero padded to four
  # digits, and non digits are removed.
  AS_VAR_PUSHDEF([A],[ax_compare_version_A])
  A=`echo "$1" | sed -e 's/\([[0-9]]*\)/Z\1Z/g' \
                     -e 's/Z\([[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/[[^0-9]]//g'`

  AS_VAR_PUSHDEF([B],[ax_compare_version_B])
  B=`echo "$3" | sed -e 's/\([[0-9]]*\)/Z\1Z/g' \
                     -e 's/Z\([[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/[[^0-9]]//g'`

  dnl # In the case of le, ge, lt, and gt, the strings are sorted as necessary
  dnl # then the first line is used to determine if the condition is true.
  dnl # The sed right after the echo is to remove any indented white space.
  m4_case(m4_tolower($2),
  [lt],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort -r | sed "s/x${A}/false/;s/x${B}/true/;1q"`
  ],
  [gt],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort | sed "s/x${A}/false/;s/x${B}/true/;1q"`
  ],
  [le],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort | sed "s/x${A}/true/;s/x${B}/false/;1q"`
  ],
  [ge],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort -r | sed "s/x${A}/true/;s/x${B}/false/;1q"`
  ],[
    dnl Split the operator from the subversion count if present.
    m4_bmatch(m4_substr($2,2),
    [0],[
      # A count of zero means use the length of the shorter version.
      # Determine the number of characters in A and B.
      ax_compare_version_len_A=`echo "$A" | $AWK '{print(length)}'`
      ax_compare_version_len_B=`echo "$B" | $AWK '{print(length)}'`

      # Set A to no more than B's length and B to no more than A's length.
      A=`echo "$A" | sed "s/\(.\{$ax_compare_version_len_B\}\).*/\1/"`
      B=`echo "$B" | sed "s/\(.\{$ax_compare_version_len_A\}\).*/\1/"`
    ],
    [[0-9]+],[
      # A count greater than zero means use only that many subversions
      A=`echo "$A" | sed "s/\(\([[0-9]]\{4\}\)\{m4_substr($2,2)\}\).*/\1/"`
      B=`echo "$B" | sed "s/\(\([[0-9]]\{4\}\)\{m4_substr($2,2)\}\).*/\1/"`
    ],
    [.+],[
      AC_WARNING(
        [illegal OP numeric parameter: $2])
    ],[])

    # Pad zeros at end of numbers to make same length.
    ax_compare_version_tmp_A="$A`echo $B | sed 's/./0/g'`"
    B="$B`echo $A | sed 's/./0/g'`"
    A="$ax_compare_version_tmp_A"

    # Check for equality or inequality as necessary.
    m4_case(m4_tolower(m4_substr($2,0,2)),
    [eq],[
      test "x$A" = "x$B" && ax_compare_version=true
    ],
    [ne],[
      test "x$A" != "x$B" && ax_compare_version=true
    ],[
      AC_WARNING([illegal OP parameter: $2])
    ])
  ])

  AS_VAR_POPDEF([A])dnl
  AS_VAR_POPDEF([B])dnl

  dnl # Execute ACTION-IF-TRUE / ACTION-IF-FALSE.
  if test "$ax_compare_version" = "true" ; then
    m4_ifvaln([$4],[$4],[:])dnl
    m4_ifvaln([$5],[else $5])dnl
  fi
]) dnl AX_COMPARE_VERSION


# ============ Test the nature of the va_list type. ==================
AC_DEFUN([CHECK_VA_LIST],
[
  # Check to see how the compiler defines va_list, as pointer or array.
  AC_MSG_CHECKING(whether va_lists can be copied by value)
  AC_CACHE_VAL(ac_cv_valistisarray,[
      AC_TRY_RUN([
          #include <stdarg.h>
          void f(int i, ...) {
              va_list args1, args2;
              va_start(args1, i);
              args2 = args1;
              if (va_arg(args2, int) != 42 || va_arg(args1, int) != 42)
                  exit(1);
              va_end(args1); va_end(args2);
          }
          int main() { f(0, 42); return(0); }],
          ac_cv_valistisarray=false,
          ac_cv_valistisarray=true,
          ac_cv_valistisarray=false
      )
  ])

  if test "$ac_cv_valistisarray" = true ; then
      AC_DEFINE(HAVE_VA_LIST_AS_ARRAY, [1], [Define if va_list is defined as an array])
      AC_MSG_RESULT(yes)
  else
      AC_MSG_RESULT(no)
  fi
])


# ==================== wxWidgets  =========================
AC_DEFUN([CHECK_WXWIDGETS],
[
    AC_MSG_CHECKING([for wxWidgets])
    AC_PATH_PROG([WXWIDGETS_CONFIG],wx-config)

    if test "x_$WXWIDGETS_CONFIG" != "x_"
    then
        wxCflags=`$WXWIDGETS_CONFIG --cflags`
        AC_SUBST(WXWIDGETS_CFLAGS,$wxCflags)
        wxCXXflags=`$WXWIDGETS_CONFIG --cxxflags`
        AC_SUBST(WXWIDGETS_CXXFLAGS,$wxCXXflags)
        wxlibs=`$WXWIDGETS_CONFIG --libs`
        AC_SUBST(WXWIDGETS_LIBS,$wxlibs)
        wxver=`$WXWIDGETS_CONFIG --version`

        AC_MSG_CHECKING([wxWidgets revision])
        AC_MSG_RESULT([found version $wxver])

        enable_wxwidgets=yes
    else
        enable_wxwidgets=no
        AC_MSG_WARN([no wx-config found - wxWidgets disabled])
    fi
])dnl


# ==================== named ====================
# Find the installed executable of named/bind.
AC_DEFUN([CHECK_NAMED],
[
    AC_ARG_WITH(named,
                [--with-named=PATH the named/bind executable],
                [named_program=$withval],
                [named_program=""],
	             )

    if test x_$named_program != x_; then
      AC_MSG_RESULT([Using named from --with-named $named_program])
      AC_SUBST(NAMED_PROGRAM, $named_program)
    else
      AC_PATH_PROG([NAMED_PROGRAM], [named],
                   [named],
                   [$PATH:/sbin:/usr/sbin:/usr/local/sbin]
                   )
    fi

    if ! test -x $NAMED_PROGRAM; then
        AC_MSG_WARN([Cannot execute $NAMED_PROGRAM.  Tests that require it will not be executed.])
        NAMED_PROGRAM=""
    fi
])


# ==================== SELinux ====================
# Is selinux supported on this platform?
# sets SELINUX_GETENFORCE to either 'NOT_SUPPORTED' or the path to the 'getenforce' program
AC_DEFUN([CHECK_SELINUX],
[
    AC_MSG_CHECKING([for selinux support])
    AC_PATH_PROG([SELINUX_GETENFORCE], [getenforce], [NOT_SUPPORTED],
                   [$PATH:/sbin:/usr/sbin:/usr/local/sbin]
                   )
    if test "${SELINUX_GETENFORCE}" = "NOT_SUPPORTED"; then
       AC_MSG_RESULT([selinux not supported - check disabled])
    else
       AC_MSG_RESULT([selinux supported - check enabled])
    fi
])


# ==================== Ruby ====================
AC_DEFUN([CHECK_RUBY],
[
  AC_PATH_PROG([RUBY], ruby)

  if test "x$RUBY" = "x" ; then
    AC_MSG_ERROR([ruby is required])
  fi

  minRubyVersion=[$1]
  AC_MSG_CHECKING([for ruby minimum version $minRubyVersion])

  ## warning about line below: use $ 2 instead of $2 otherwise m4 trys to
  ## interpret, luckily awk doesn't care
  rubyVersion=`$RUBY --version | awk '{print $ 2}'`

  AX_COMPARE_VERSION([$rubyVersion],[ge],[$minRubyVersion],
       [AC_MSG_RESULT($rubyVersion is ok)],
       [AC_MSG_ERROR([ruby version must be >= $minRubyVersion - found $rubyVersion])])
])

# ==================== Ruby Gem ====================
# Like Perl's CPAN
AC_DEFUN([CHECK_GEM],
[
  AC_PATH_PROG([GEM], gem)

  minGemVersion=[$1]
  if test "x$GEM" = "x" ; then
    AC_MSG_RESULT([* to install ruby gems follow your distro instructions, ])
    AC_MSG_RESULT([* locate the rpm on pbone or run the following commands: ])
cat 1>&2 <<GEMS_HOWTO
 wget http://rubyforge.org/frs/download.php/5207/rubygems-${minGemVersion}.tgz
 tar -xzf rubygems-${minGemVersion}.tgz
 cd rubygems-${minGemVersion}
 sudo ruby setup.rb config
 sudo ruby setup.rb install
GEMS_HOWTO
    AC_MSG_ERROR([ruby gem command is required])
  fi

  AC_MSG_CHECKING([for gem minimum version $minGemVersion])

  gemVersion=`$GEM --version`

  AX_COMPARE_VERSION([$gemVersion],[ge],[$minGemVersion],
       [AC_MSG_RESULT($gemVersion is ok)],
       [AC_MSG_ERROR([gem version must be >= $minGemVersion - found $gemVersion])])
])

# ==================== Rake ====================
# build files
AC_DEFUN([CHECK_RAKE],
[
  AC_PATH_PROG([RAKE], rake)

  if test "x$RAKE" = "x" ; then
    AC_MSG_ERROR([rake is required.  type 'gem install rake --no-rdoc'])
  fi

  minRakeVersion=[$1]
  AC_MSG_CHECKING([for rake minimum version $minRakeVersion])

  rakeVersion=`$RAKE --version | awk '{print $ 3}'`

  AX_COMPARE_VERSION([$rakeVersion],[ge],[$minRakeVersion],
       [AC_MSG_RESULT($rakeVersion is ok)],
       [AC_MSG_ERROR([rake version must be >= $minRakeVersion - found $rakeVersion])])
])

##
##  pass module path (e.g. wsdl/soap/wsl2ruby)
##
AC_DEFUN([CHECK_RUBY_MODULE],
[
  rubyModule=[$1]
  AC_MSG_CHECKING([for ruby module $rubyModule])

  if $RUBY -r $rubyModule -e '' 2> /dev/null
  then
    AC_MSG_RESULT([ok])
  else
    AC_MSG_ERROR([Required ruby $rubyModule is missing])
  fi
])

##
##  pass module path (e.g. wsdl/soap/wsl2ruby)
##
AC_DEFUN([CHECK_RUBY_GEM],
[
  rubyGem=[$1]
  AC_MSG_CHECKING([for ruby gem $rubyGem])

  if $GEM list --local | egrep "^$rubyGem"
  then
    AC_MSG_RESULT([ok])
  else
    AC_MSG_RESULT([missing])
    AC_MSG_ERROR([type 'gem install $rubyGem --no-rdoc' to install])
  fi
])

# ==================== profile with gprof ====================
AC_DEFUN([ENABLE_PROFILE],
[
  AC_ARG_ENABLE(profile,
                [  --enable-profile        Enable profiling via gprof (no)],
                [enable_profile="$enableval"], [enable_profile=no])

  if test x"$enable_profile" = xyes
  then
    # Add gprof flags
    CFLAGS="$CFLAGS -pg -ggdb"
    CXXFLAGS="$CXXFLAGS -pg -ggdb"
    LDFLAGS="$LDFLAGS -pg"
  fi
])

# ==================== unixODBC  =========================
AC_DEFUN([CHECK_ODBC],
[
    AC_MSG_CHECKING([for unixODBC])

    # Process the --with-odbc argument which gives the odbc base directory.
    AC_ARG_WITH(odbc,
                [  --with-odbc=PATH path to odbc install directory],
                [odbc_homeval=$withval],
                [odbc_homeval=""]
                )

    # Process the --with-odbc_includedir argument which gives the odbc include
    # directory.
    AC_ARG_WITH(odbc_includedir,
                [  --with-odbc_includedir=PATH path to odbc include directory (containing sql.h)],
                [includeval=$withval],
                [if test -n "$odbc_homeval";
                 then includeval="$odbc_homeval/include";
                 else includeval="/usr/include /usr/include/odbc /usr/include/unixODBC /usr/local/include /usr/local/odbc/include";
                 fi
                ]
                )
    # Check for sql.h
    found_odbc_include="no";
    for dir in $includeval ; do
        if test -f "$dir/sql.h"; then
            found_odbc_include="yes";
            includeval=$dir
            break;
        fi
    done

    # Process the --with-odbc_libdir argument which gives the odbc library
    # directory.
    AC_ARG_WITH(odbc_libdir,
                [  --with-odbc_libdir=PATH path to odbc lib directory (containing libodbc.{so,a})],
                [libval=$withval],
                [if test -n "$odbc_homeval";
                 then libval="$odbc_homeval/lib";
                 else libval="/usr/lib64 /usr/lib /usr/lib/odbc /usr/lib/unixODBC /usr/local/lib /usr/local/odbc/lib";
                 fi
                ]
                )
    # Check for libodbc.{so,a}
    found_odbc_lib="no";
    for dir in $libval; do
        if test -f "$dir/libodbc.so" -o -f "$dir/libodbc.a"; then
            found_odbc_lib="yes";
            libval=$dir
            break;
        fi
    done

    # Test that we've been able to find both directories, and set the various
    # makefile variables.
    if test x_$found_odbc_include != x_yes; then
        AC_MSG_ERROR(Cannot find sql.h - looked in $includeval)
    else
        if test x_$found_odbc_lib != x_yes; then
            AC_MSG_ERROR(Cannot find libodbc.so or libodbc.a libraries - looked in $libval)
        else
            ## Test for version
            odbc_ver=`odbcinst --version`
            AX_COMPARE_VERSION([$odbc_ver],[ge],[2.2],
                               [AC_MSG_RESULT($odbc_ver is ok)],
                               [AC_MSG_ERROR([unixODBC version must be >= 2.2 - found $odbc_ver])])

            AC_MSG_RESULT([    odbc includes found in $includeval])
            AC_MSG_RESULT([    odbc libraries found in $libval])

            ODBC_CFLAGS="-I$includeval"
            ODBC_CXXFLAGS="-I$includeval"
            AC_SUBST(ODBC_CFLAGS)
            AC_SUBST(ODBC_CXXFLAGS)

            AC_SUBST(ODBC_LIBS, "-lodbc" )
            AC_SUBST(ODBC_LDFLAGS, "-L$libval")
            AC_SUBST(ODBC_LIBDIR, $libval)
            AC_MSG_NOTICE([ODBC_LIBDIR = $ODBC_LIBDIR])
        fi
    fi
])dnl

# ============ F R E E S W I T C H ==================
AC_DEFUN([CHECK_FREESWITCH],
[
    AC_MSG_CHECKING([for FreeSWITCH])
    AC_ARG_WITH(freeswitch,
                [--with-freeswitch=PATH to FreeSWITCH install directory],
		[freeswitch_path=$withval],
		[freeswitch_path="/opt/freeswitch"]
                )
    for dir in $freeswitch_path /usr/local/freeswitch; do
        freeswitch_dir="$dir"
        if test -x "$dir/bin/freeswitch"; then
            found_freeswitch="yes";
            break;
        fi
    done

    if test x_$found_freeswitch = x_yes; then
        AC_MSG_RESULT([    FreeSWITCH installed in $freeswitch_dir])
    else
        AC_MSG_WARN([    'bin/freeswitch' not in any of: $freeswitch_path])
        freeswitch_dir="/opt/freeswitch"
        AC_MSG_WARN([    assuming it will be in $freeswitch_dir])
    fi
    AC_SUBST(FREESWITCH_PREFIX, $freeswitch_dir)
])dnl

# ================== COMPILER VENDOR ====================================

AC_DEFUN([AX_COMPILER_VENDOR],
[
  AC_CACHE_CHECK([for _AC_LANG compiler vendor], ax_cv_[]_AC_LANG_ABBREV[]_compiler_vendor,
                 [ax_cv_[]_AC_LANG_ABBREV[]_compiler_vendor=unknown
  # note: don't check for gcc first since some other compilers define __GNUC__
  for ventest in intel:__ICC,__ECC,__INTEL_COMPILER ibm:__xlc__,__xlC__,__IBMC__,__IBMCPP__ gnu:__GNUC__ sun:__SUNPRO_C,__SUNPRO_CC hp:__HP_cc,__HP_aCC dec:__DECC,__DECCXX,__DECC_VER,__DECCXX_VER borland:__BORLANDC__,__TURBOC__ comeau:__COMO__ cray:_CRAYC kai:__KCC lcc:__LCC__ metrowerks:__MWERKS__ sgi:__sgi,sgi microsoft:_MSC_VER watcom:__WATCOMC__ portland:__PGI; do
    vencpp="defined("`echo $ventest | cut -d: -f2 | sed 's/,/) || defined(/g'`")"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM(,[
#if !($vencpp)
      thisisanerror;
#endif
    ])],
    [ax_cv_]_AC_LANG_ABBREV[_compiler_vendor=`echo $ventest | cut -d: -f1`; break])
  done
 ])
])

AC_DEFUN([CHECK_MSG_NOSIGNAL],
[
   AC_MSG_CHECKING(whether MSG_NOSIGNAL exists)
   AC_TRY_COMPILE([#include <sys/socket.h>],
   [
    int f;
    f = MSG_NOSIGNAL;		// Make sure variable 'f' is "used".
   ],
    # Yes, we have it...
    AC_MSG_RESULT(yes)
   ,
    # We'll have to use signals
   [ AC_MSG_RESULT(no)
    CPPFLAGS="$CPPFLAGS -DMSG_NOSIGNAL=0"
   ])
])

# ============ REQUIRED RPM PKG ==============
# Only useful for projects that are strictly rpm based, which should be
# non-functional packaging based projects like making iso images for example
AC_DEFUN([REQUIRE_RPM],
[
  required_rpm_pkg=[$1]
  AC_MSG_CHECKING($required_rpm_pkg)
  if ! rpm -q $required_rpm_pkg >/dev/null
  then
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([Required rpm pkg missing $required_rpm_pkg])
  fi
  AC_MSG_RESULT(yes)
])


# ============ REQUIRED RPM PKG ==============
AC_DEFUN([CHECK_CRON],
[
  AC_MSG_CHECKING([cron.d])
  if test x$CRON_D = x ; then
    CRON_D=/etc/cron.d
  fi
  if ! test -d $CRON_D; then
    AC_MSG_ERROR([Required directory $CRON_D missing])
  fi
  AC_SUBST(CRON_D)
  AC_MSG_RESULT(yes)
])

AC_DEFUN([CHECK_XSLTPROC],
[
  AC_PATH_PROG([XSLTPROC], [xsltproc])
])

AC_DEFUN([CHECK_OPENJADE],
[
  AC_PATH_PROG([OPENJADE], [openjade])
  AC_PATH_PROG([PDFJADETEX], [pdfjadetex])
])

AC_DEFUN([CHECK_HTML2TXT],
[
   AC_REQUIRE([ENABLE_DOC])
   AC_REQUIRE([CHECK_DOCBOOK2HTML])
   # This relies on docbook->html, and then a converter from html->txt
   HTML2TXT=
   AC_PATH_PROG([ELINKS],[elinks])
   if test x$ELINKS != x
   then
       HTML2TXT="$ELINKS -dump -no-home -no-references -no-numbering"
   else
      AC_PATH_PROG([W3M],[w3m])
      if test x$W3M != x
      then
          HTML2TXT="$W3M -dump"
      else
          AC_PATH_PROG([LYNX],[lynx])
          if test x$LYNX != x
          then
              HTML2TXT="$LYNX -dump"
          fi
      fi
   fi
   if test "$HTML2TXT" != ""
   then
      enable_xml2txt=yes
   else
      enable_xml2txt=no
   fi
   AC_SUBST(enable_xml2txt)
   AC_SUBST(HTML2TXT)
])

AC_DEFUN([CHECK_DOCBOOK2HTML],
[
  AC_REQUIRE([ENABLE_DOC])
  AC_REQUIRE([CHECK_XSLTPROC])
  if test x$XSLTPROC != x
  then
     xml2xhtml_files="/usr/share/sgml/docbook/xsl-stylesheets/xhtml/docbook.xsl"
     AC_MSG_CHECKING("docbook inputs for generating xhtml")
     enable_xml2xhtml=yes
     for f in ${xml2xhtml_files}
     do
        if ! test -r "$f"
        then
            AC_MSG_WARN("failed to find '$f'")
            enable_xml2xhtml=no
        fi
     done
     test x$enable_xml2xhtml = xyes && AC_MSG_RESULT([ok])
  else
     enable_xml2xhtml=no
  fi
  test x$enable_xml2xhtml = xno && AC_MSG_WARN([DocBook XML to XHTML disabled])
  AC_SUBST(enable_xml2xhtml)
])

AC_DEFUN([CHECK_DOCBOOK2PDF],
[
  AC_REQUIRE([ENABLE_DOC])
  AC_REQUIRE([CHECK_OPENJADE])

  if test x$OPENJADE != x -a x$PDFJADETEX != x
  then
     xml2pdf_files="/usr/share/sgml/docbook/dsssl-stylesheets/print/docbook.dsl /usr/share/sgml/docbook/dsssl-stylesheets/dtds/decls/xml.dcl /usr/share/sgml/openjade/catalog"
     AC_MSG_CHECKING("docbook inputs for generating pdf")
     enable_xml2pdf=yes
     for f in ${xml2pdf_files}
     do
        if ! test -r "$f"
        then
            AC_MSG_WARN("failed to find '$f'")
            enable_xml2pdf=no
        fi
     done
     test x$enable_xml2pdf = xyes && AC_MSG_RESULT([ok])
  else
     enable_xml2pdf=no
  fi

  test $enable_xml2pdf = no && AC_MSG_WARN([DocBook XML to PDF disabled])
  AC_SUBST(enable_xml2pdf)
])

AC_DEFUN([CHECK_DOCBOOKXML],
[
  AC_REQUIRE([ENABLE_DOC])
  AC_REQUIRE([CHECK_DOCBOOK2HTML])
  AC_REQUIRE([CHECK_HTML2TXT])
  if test x$enable_xml2xhtml != xyes
  then
      enable_xml2txt=no
  fi
  AC_REQUIRE([CHECK_DOCBOOK2PDF])
])

AC_DEFUN([ENABLE_DOC],
[
   AC_ARG_ENABLE(doc,
                 [  --disable-doc controls all documentation building - saves time for developer builds],
                 [enable_doc="$enableval"], [enable_doc=yes]
                 )
   AC_SUBST(enable_doc)
])

# ============ REQUIRED STUNNEL PKG ==============
AC_DEFUN([CHECK_STUNNEL],
[
  AC_PATH_PROG([STUNNEL], [stunnel],,[/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin:/usr/local/sbin])
  if test x$STUNNEL = x; then
    AC_MSG_ERROR([not found])
  fi
])

# ============ XARGS_REPLACE  =========================
AC_DEFUN([CHECK_XARGS_REPLACE],
[
   AC_MSG_CHECKING([for xargs replace option])

   echo | xargs -I % echo
   if test $? -eq 0  ; then
      AC_SUBST(XARGS_REPLACE,"-I ")
      AC_MSG_RESULT([-I])
   else
      echo | xargs --replace=% echo
      if test $? -eq 0  ; then
         AC_SUBST(XARGS_REPLACE,"--replace=")
         AC_MSG_RESULT([--replace])
      else
         AC_MSG_ERROR([tried -I and --replace; neither worked])
      fi
   fi
])

# ============ DATE_PARSE_ARGS  =========================
AC_DEFUN([CHECK_DATE_PARSE_ARGS],
[
   AC_MSG_CHECKING([for date conversion command arguments])
   # Find a method to convert a date string to an epoch number

   # this method seems to work on most Linuxes
   date --date="`date +%b\ %e\ %T\ %Y\ %Z`" +%s > /dev/null 2>&1
   if test $? -eq 0
   then
      AC_SUBST(DATE_PARSE_ARGS,"--date=")
      AC_MSG_RESULT([--date=])
   else
      # this method works on FreeBSD
      date -j -f "%b %e %T %Y %Z" "`date +%b\ %e\ %T\ %Y\ %Z`" +%s > /dev/null 2>&1
      if test $? -eq 0
      then
         AC_SUBST(DATE_PARSE_ARGS,"-j -f \"%b %e %T %Y %Z\" ")
         AC_MSG_RESULT([-j -f \"%b %e %T %Y %Z\"])
      else
         AC_MSG_ERROR([no working method found for converting date strings])
      fi
   fi
])


# ==================== NSIS ====================
AC_DEFUN([CHECK_NSIS],
[
  AC_PATH_PROG([MAKENSIS], makensis)

  if test "x$MAKENSIS" = "x" ; then
    AC_MSG_ERROR([NSIS is required -- makensis not found in the path.
Build it from the lib/nsis directory,
install the mingw32-nsis rpm,
or get the RPM's from the SIPfoundry repositories.])
  fi
])


# ============ rpmbuild debugging information RPM =========================
#AC_DEFUN([CHECK_RPM_DEBUG],
#[
#   # Function to extract the value of an rpmbuild macro, as configured
#   # in the current situation.
#   [ # Protect [...] from m4
#   function rpmbuild_macro () {
#	   rpmbuild --showrc |
#	   sed -e "/^-[0-9][0-9]*[:=] $][1"$'\t'"/,/^-[0-9][0-9]*[:=]/!d" |
#	   head -n-1
#   }
#   ]
#
#   # Determine if %debug_package is needed to cause the debugging information
#   # RPM to be built by checking to see if %install contains a test on
#   # 'enable_debug_packages' -- Older rpmbuilds do not, and the .spec file
#   # must manually specify %debug_package.  Newer rpmbuilds do, and
#   # the .spec file must not specify %debug_package.
#   # Thus, you can use "@RPMBUILD_DEBUG_PACKAGE_SPEC@" in a .spec.in file to
#   # ensure that a debugging information RPM is built.
#   AC_MSG_CHECKING([for rpmbuild debugging RPM directive])
#   if rpmbuild_macro install | grep enable_debug_packages >/dev/null 2>&1 ; then
#	AC_SUBST(RPMBUILD_DEBUG_PACKAGE_SPEC,"")
#   else
#	AC_SUBST(RPMBUILD_DEBUG_PACKAGE_SPEC,"%debug_package")
#   fi
#   AC_MSG_RESULT([it is '$RPMBUILD_DEBUG_PACKAGE_SPEC'])
#
#   # Determine name of the debugging information RPM.
#   # Search in the definition of %debug_package to find this.
#   AC_MSG_CHECKING([for rpmbuild debugging RPM name])
#   # $[2] is to protect the $ from interpretation by m4.
#   RPMBUILD_DEBUG_PACKAGE_NAME=` rpmbuild_macro debug_package | grep '%package' | awk '{print $[2]}' `
#   AC_SUBST(RPMBUILD_DEBUG_PACKAGE_NAME)
#   AC_MSG_RESULT([$RPMBUILD_DEBUG_PACKAGE_NAME])
#])

# ============ Y U M  ==================
AC_DEFUN([CHECK_YUM],
[
   AC_ARG_VAR(YUM_EXISTS, [Yum program])
   AC_CHECK_PROG(YUM_EXISTS, yum, "true", "false", $PATH:/usr/bin)
])

