SUBDIRS = .

EXTRA_DIST = apps/scripts 

corelibs =

corelibldadd = \
    @OSS_CORE_DEP_LIBS@ \
    liboss_core.la

corelibs += liboss_core.la

lib_LTLIBRARIES = $(corelibs)

localedir = $(datadir)/locale

AM_CFLAGS = \
    -DHAVE_POLL \
    -DHAVE_INET_PTON_NTOP \
    -I@OSS_CORE_SRCDIR@/src/utl/linux/pidfile \
    -I$(top_builddir)/include

AM_CPPFLAGS = \
    -DHAVE_POLL \
    -DBOOST_THREAD_PROVIDES_FUTURE \
    -I$(top_builddir)/include \
    -I$(top_builddir)/src \
    -I@OSS_CORE_SRCDIR@/include \
    -I@OSS_CORE_SRCDIR@/src/utl/linux/pidfile \
    -I@OSS_CORE_SRCDIR@/src/net/udns/include \
    @OSS_CORE_ADDITIONAL_INCLUDES@



version_Current=@OSS_CORE_VERSION_CURRENT@
version_Revision=@OSS_CORE_VERSION_REVISION@
version_Age=@OSS_CORE_VERSION_AGE@

LDADD = $(corelibldadd)

LDFLAGS = -rdynamic

liboss_core_la_LDFLAGS = -version-info @OSS_CORE_VERSION_INFO@
liboss_core_la_LIBADD = @OSS_CORE_DEP_LIBS@
liboss_core_la_CFLAGS = -DHAVE_INET_PTON_NTOP

BUILT_SOURCES = boost_pch.hpp.gch
CLEANFILES = boost_pch.hpp.gch

liboss_core_la_SOURCES = boost_pch.hpp OSS.cpp

#
# Non-SIP sources
#
include zmq/src.am
include crypto/src.am
include persistent/src.am
include raft/src.am
include lmdb/src.am
include js/src.am
include net/src.am
include exec/src.am
include stun/src.am
include utl/src.am
include bson/src.am
include json/src.am

#
# SIP Implementation
#
include abnf/src.am
include sdp/src.am
include sipparser/src.am
include siptransport/src.am
include sipfsm/src.am
include sipep/src.am
include ep/src.am
include rtp/src.am
include b2bua/src.am


DISTCLEANFILES = Makefile.in

#
# Build rule for Boost precompiled header
#

%.hpp.gch: %.hpp
	$(CXX) -D_REENTRANT -D_FILE_OFFSET_BITS=64 -O0 -g -O2 -fmessage-length=0 -Wall -Wformat -Wwrite-strings -Wpointer-arith -fPIC -DPIC -c $<

	
#
# Unit Test
#
include unit_test/Makefile.am

#
# Applications
#
bin_PROGRAMS =
include apps/Makefile.am

#
# install hook
#
install-exec-hook: js_plugin-install-hook
