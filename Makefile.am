include config/utility.am
include config/project.am

@PACKAGE@_EXTRA_DIST = \
	README

SUBDIRS = .
SUBDIRS += src 
SUBDIRS += include

system-install:
	mkdir -p .system-install
	cd .system-install && ../../configure \
	    --enable-all-features \
	    --program-prefix= \
	    --prefix=/usr \
	    --exec-prefix=/usr \
	    --bindir=/usr/bin \
	    --sbindir=/usr/sbin \
	    --sysconfdir=/etc \
	    --datadir=/usr/share \
	    --includedir=/usr/include \
	    --libdir=/usr/lib \
	    --libexecdir=/usr/libexec \
	    --localstatedir=/var \
	    --sharedstatedir=/var/lib \
	    --mandir=/usr/share/man \
	    --infodir=/usr/share/info
	cd .system-install && make -j4 install

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

resip-install:
	cd $(top_builddir)/libs/resiprocate && make install