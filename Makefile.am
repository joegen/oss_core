include config/utility.am
include config/project.am

@PACKAGE@_EXTRA_DIST = \
	README

SUBDIRS = .
SUBDIRS += src 
SUBDIRS += include

rpm: dist
	rm -rf `pwd`/RPMBUILD; \
	mkdir -p `pwd`/RPMBUILD/{BUILD,SOURCES,RPMS,SRPMS,SPECS}; \
	rpmbuild -ta --define "%_topdir `pwd`/RPMBUILD" oss_core*.gz
