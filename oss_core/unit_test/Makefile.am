## Process this file with automake to produce Makefile.in
include $(top_srcdir)/config/subdir.am

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS = oss_core_unit_test

oss_core_unit_test_CXXFLAGS = \
	${common_cxx_flags} \
	-fno-gcse -O0 -fno-implement-inlines -fno-default-inline

oss_core_unit_test_LDFLAGS = \
	-rdynamic \
	-lgtest

oss_core_unit_test_LDADD = \
	@OSSLIBSCORE_LIBS@ \
	${common_libs}

oss_core_unit_test_SOURCES = \
	src/TestRequestLine.cpp \
	src/TestBasicParser.cpp \
	src/TestSDP.cpp \
	src/TestCSeq.cpp \
	src/TestCache.cpp \
	src/TestFoundationAPI.cpp \
	src/TestVia.cpp \
	src/TestContact.cpp \
	src/TestRouteHeader.cpp \
	src/TestFrom.cpp \
	src/TestPersistent.cpp \
	src/TestDNS.cpp \
	src/TestSIPURI.cpp \
	src/TestSuite.cpp


