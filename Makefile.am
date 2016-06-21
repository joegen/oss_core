include config/utility.am
include config/project.am

@PACKAGE@_EXTRA_DIST = \
	README

SUBDIRS = \
	. \
	src \
        include

if ENABLE_FEATURE_LIBRE
    SUBDIRS += src/ua/core
endif