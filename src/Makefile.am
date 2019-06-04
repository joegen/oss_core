
OSS_CORE_DEP_LIBS = @OSS_CORE_DEP_LIBS@
OSS_CORE_LA = liboss_core.la

#
# Compile External libs
#
RESIP_DIR = @OSS_CORE_SRCDIR@/libs/resiprocate
RESIP_BUILD_DIR = $(top_builddir)/libs/resiprocate
RESIPLA = $(RESIP_BUILD_DIR)/resip/stack/libresip.la
$(RESIPLA):
	cd $(RESIP_BUILD_DIR) && make


SUBDIRS = .

EXTRA_DIST =

lib_LTLIBRARIES = liboss_core.la

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
    -I@OSS_CORE_SRCDIR@/src/net/esl/include \
    -I$(RESIP_DIR) \
    @OSS_CORE_ADDITIONAL_INCLUDES@

version_Current=@OSS_CORE_VERSION_CURRENT@
version_Revision=@OSS_CORE_VERSION_REVISION@
version_Age=@OSS_CORE_VERSION_AGE@

LDADD = $(OSS_CORE_LA) $(OSS_CORE_DEP_LIBS)

LDFLAGS = -rdynamic

liboss_core_la_LDFLAGS = -rdynamic -fpic -avoid-version

liboss_core_la_LIBADD = $(OSS_CORE_DEP_LIBS)

liboss_core_la_CFLAGS = -DHAVE_INET_PTON_NTOP
liboss_core_la_CPPFLAGS = $(AM_CPPFLAGS) -fPIC

BUILT_SOURCES = boost_pch.hpp.gch
CLEANFILES = boost_pch.hpp.gch

liboss_core_la_SOURCES = boost_pch.hpp OSS.cpp
bin_PROGRAMS =
#
# Non-SIP sources
#
include zmq/src.am
include crypto/src.am
include persistent/src.am
include js/src.am
include net/src.am
include exec/src.am
include stun/src.am
include utl/src.am
include json/src.am
include udns/src.am

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
include sbc/src.am


DISTCLEANFILES = Makefile.in

#
# Build rule for Boost precompiled header
#

%.hpp.gch: %.hpp
	$(CXX) ${CXXFLAGS} -D_REENTRANT -D_FILE_OFFSET_BITS=64 -O0 -g -O2 -fmessage-length=0 -Wall -Wformat -Wwrite-strings -Wpointer-arith -fPIC -DPIC -c $<

	
#
# Unit Test
#
include unit_test/Makefile.am

#
# install hook
#
install-exec-hook: js_plugin-install-hook
