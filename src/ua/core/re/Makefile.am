SUBDIRS = .

EXTRA_DIST = @OSS_CORE_SRCDIR@/src/ua/re/mk

include @OSS_CORE_SRCDIR@/src/ua/re/mk/re.mk

AM_CFLAGS = \
	-I@OSS_CORE_SRCDIR@/src/ua/re/include \
	-DHAVE_ROUTE_LIST \
	-DUSE_OPENSSL_DTLS=1 \
	-DUSE_OPENSSL_SRTP=1 \
	-DUSE_VIDEO

nobase_lib_LTLIBRARIES = \
	re/libre.la

SRCS = ""

include aes/mod.mk
include base64/mod.mk

baresip_libre_la_SOURCES = ${SRCS}
	





