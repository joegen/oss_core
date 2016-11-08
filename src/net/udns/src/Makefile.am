SUBDIRS = . 
AM_CPPFLAGS = \
	-Wall \
	-DHAVE_BOOSTLIBS \
	-DHAVE_INET_PTON_NTOP \
	-I$(top_srcdir)/include
	
AM_CFLAGS = \
	-DHAVE_INET_PTON_NTOP \
	-I$(top_srcdir)/include

lib_LTLIBRARIES = libudnspp.la

version_Current=0
version_Age=0
version_Revision=4

libudnspp_la_LDFLAGS = \
    -version-info ${version_Current}:${version_Revision}:${version_Age} \
    @BOOST_LIBS@

libudnspp_la_SOURCES = \
	udns_bl.c \
	udns_dn.c \
	udns_init.c  \
	udns_misc.c   \
	udns_resolver.c  \
	udns_rr_mx.c     \
	udns_rr_ptr.c  \
	udns_rr_txt.c \
	udns_codes.c  \
	udns_dntosp.c  \
	udns_jran.c  \
	udns_parse.c  \
	udns_rr_a.c      \
	udns_rr_naptr.c  \
	udns_rr_srv.c  \
	udns_XtoX.c \
	udnspp/dnsarecord.cpp  \
	udnspp/dnscontext.cpp  \
	udnspp/dnsmxrecord.cpp  \
	udnspp/dnsnaptrrecord.cpp  \
	udnspp/dnsptrrecord.cpp  \
	udnspp/dnsresolver.cpp  \
	udnspp/dnssrvrecord.cpp  \
	udnspp/dnstxtrecord.cpp
	
bin_PROGRAMS = dnsgetpp

dnsgetpp_CXXFLAGS = \
	-fno-gcse -O0 -fno-implement-inlines -fno-default-inline

dnsgetpp_LDADD = \
    libudnspp.la \
    @BOOST_LIBS@

dnsgetpp_SOURCES = \
    udnspp/dnsgetpp.cpp
