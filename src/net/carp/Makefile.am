if ENABLE_FEATURE_CARP
liboss_carp_la_LDFLAGS = -version-info @OSS_CORE_VERSION_INFO@
liboss_carp_la_LIBADD = $(LTLIBINTL)
liboss_carp_la_CFLAGS = -DLOCALEDIR=\"$(localedir)\"
liboss_carp_la_SOURCES = \
	net/carp/gettext.h \
	net/carp/ucarp.c \
	net/carp/ucarp.h \
	net/carp/carp.c \
	net/carp/carp_p.h \
	net/carp/ucarp_p.h \
	net/carp/ip_carp.h \
	net/carp/crypto-sha1.c \
	net/carp/crypto.h \
	net/carp/crypto-sha1.h \
	net/carp/globals.h \
	net/carp/fillmac.c \
	net/carp/fillmac.h \
	net/carp/bsd-getopt_long.h \
	net/carp/bsd-getopt_long.c \
	net/carp/garp.c \
	net/carp/garp.h \
	net/carp/spawn.c \
	net/carp/spawn.h \
	net/carp/fakesnprintf.c \
	net/carp/fakesnprintf.h \
	net/carp/mysnprintf.c \
	net/carp/mysnprintf.h \
	net/carp/log.c \
	net/carp/log.h \
	net/carp/syslognames.h
endif