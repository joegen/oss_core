include config/utility.am
include config/project.am

@PACKAGE@_EXTRA_DIST = \
	README

SUBDIRS = .
SUBDIRS += src 
SUBDIRS += include

rpm: dist
	rm -rf `pwd`/RPMBUILD && rm -rf `pwd`/RPMS; \
	mkdir -p `pwd`/RPMBUILD/{BUILD,SOURCES,RPMS,SRPMS,SPECS}; \
	rpmbuild -ta --define "%_topdir `pwd`/RPMBUILD" oss_core*.gz; \
	mv `pwd`/RPMBUILD/RPMS `pwd`/RPMS && rm -rf `pwd`/RPMBUILD
	
install-rpm: rpm
	yum localinstall -y RPMS/x86_64/*.rpm
	
