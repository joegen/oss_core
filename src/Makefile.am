SUBDIRS = .

EXTRA_DIST = js/scripts

corelibs =

if ENABLE_FEATURE_CARP
corelibs += liboss_carp.la
endif

corelibldadd = \
    @OSS_CORE_DEP_LIBS@ \
    liboss_core.la

if ENABLE_FEATURE_CARP
    corelibldadd += liboss_carp.la
endif

if ENABLE_FEATURE_LIBRE
    corelibldadd += ua/core/re/libre.a \
		    ua/core/rem/librem.a
endif

if ENABLE_FEATURE_TURN
    corelibs += liboss_turn.la
    corelibldadd += liboss_turn.la
endif
corelibs += liboss_core.la

lib_LTLIBRARIES = $(corelibs)

localedir = $(datadir)/locale

AM_CFLAGS = \
    -DHAVE_INET_PTON_NTOP \
    -I@OSS_CORE_SRCDIR@/src/utl/linux/pidfile \
    -I$(top_builddir)/include

AM_CPPFLAGS = \
    -I/usr/include \
    -I$(top_builddir)/include \
    -I$(top_builddir)/src \
    -I@OSS_CORE_SRCDIR@/include \
    -I@OSS_CORE_SRCDIR@/src/utl/linux/pidfile \
    -I@OSS_CORE_SRCDIR@/src/udns/include

if ENABLE_FEATURE_LIBRE
AM_CPPFLAGS +=	\
    -I@OSS_CORE_SRCDIR@/src/ua/core/re/include \
    -I@OSS_CORE_SRCDIR@/src/ua/core/rem/include
endif

if ENABLE_FEATURE_TURN
AM_CPPFLAGS += \
    -I@OSS_CORE_SRCDIR@/src/net/turn \
    -I@OSS_CORE_SRCDIR@/src/net/turn/client \
    -I@OSS_CORE_SRCDIR@/src/net/turn/client++ \
    -I@OSS_CORE_SRCDIR@/src/net/turn/server \
    -I@OSS_CORE_SRCDIR@/src/net/turn/apps/common \
    -I@OSS_CORE_SRCDIR@/src/net/turn/apps/relay

AM_CFLAGS += \
    -I@OSS_CORE_SRCDIR@/src/net/turn \
    -I@OSS_CORE_SRCDIR@/src/net/turn/client \
    -I@OSS_CORE_SRCDIR@/src/net/turn/client++ \
    -I@OSS_CORE_SRCDIR@/src/net/turn/server \
    -I@OSS_CORE_SRCDIR@/src/net/turn/apps/common \
    -I@OSS_CORE_SRCDIR@/src/net/turn/apps/relay
endif

version_Current=@OSS_CORE_VERSION_CURRENT@
version_Revision=@OSS_CORE_VERSION_REVISION@
version_Age=@OSS_CORE_VERSION_AGE@

LDADD = $(corelibldadd)

LDFLAGS = -rdynamic

#
# Libraries for CARP and TURN
#
include net/carp/Makefile.am
include net/turn/Makefile.am

liboss_core_la_LDFLAGS = -version-info @OSS_CORE_VERSION_INFO@
liboss_core_la_LIBADD = @OSS_CORE_DEP_LIBS@
liboss_core_la_CFLAGS = -DHAVE_INET_PTON_NTOP

BUILT_SOURCES = boost_pch.hpp.gch
CLEANFILES = boost_pch.hpp.gch

liboss_core_la_SOURCES = boost_pch.hpp OSS.cpp
include udns.am
include ws.am
include abnf/src.am
include exec/src.am
include crypto/src.am
include sdp/src.am
include sipparser/src.am
include siptransport/src.am
include sipfsm/src.am
include sipep/src.am
include sipq/src.am
include net/src.am
include utl/src.am
include ep/src.am
include bson/src.am
include raft/src.am
include lmdb/src.am
include persistent/src.am
include stun/src.am
include ua/src.am
include rtp/src.am
include zmq/src.am
include b2bua/src.am
include js/src.am

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
include lohika_js/Makefile.am


