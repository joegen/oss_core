include config/utility.am
include config/project.am

@PACKAGE@_EXTRA_DIST = \
	README

SUBDIRS = \
	. \
	src \
        include

if ENABLE_REM
    SUBDIRS += src/ua/core
endif