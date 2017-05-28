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

deb:
	rm -rf `pwd`/DEBBUILD
	mkdir -p `pwd`/DEBBUILD/libosscore-@VERSION@/DEBIAN
	DESTDIR=`pwd`/DEBBUILD/libosscore-@VERSION@ make install 
	cp debian.control `pwd`/DEBBUILD/libosscore-@VERSION@/DEBIAN/control
	cd DEBBUILD && dpkg-deb --build libosscore-@VERSION@