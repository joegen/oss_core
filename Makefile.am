include config/utility.am
include config/project.am

@PACKAGE@_EXTRA_DIST = \
	README

SUBDIRS = .
if ENABLE_FEATURE_LIBRE
    SUBDIRS += src/ua/core
endif
SUBDIRS += src include
