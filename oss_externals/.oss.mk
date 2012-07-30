oss_externals_VER = 1.0.2
oss_externals_PACKAGE_REVISION = $(shell cd $(SRC)/$(PROJ); ../config/revision-gen $(oss_externals_VER))

oss_externals_SRPM = oss_externals-$(oss_externals_VER)-$(oss_externals_PACKAGE_REVISION).src.rpm
oss_externals_SPEC = $(SRC)/$(PROJ)/oss_externals.spec
oss_externals_TARBALL = $(BUILDDIR)/$(PROJ)/oss_externals-$(oss_externals_VER).tar.bz2
oss_externals_SOURCES = $(oss_externals_TARBALL)

oss_externals_SRPM_DEFS = --define "buildno $(oss_externals_PACKAGE_REVISION)"
oss_externals_RPM_DEFS = --define="buildno $(oss_externals_PACKAGE_REVISION)"

# we could, but we don't these targets. FS doesn't support running ./configure from anywhere but source root.
oss_externals.autoreconf oss_externals.configure:;

oss_externals.dist :
	test -d $(dir $(oss_externals_TARBALL)) || mkdir -p $(dir $(oss_externals_TARBALL))
	cd $(SRC)/$(PROJ); \
		wget -c http://bridge.ossapp.com/downloads/karoo/externals/oss_externals-$(oss_externals_VER).tar.bz2; \
		cp oss_externals-$(oss_externals_VER).tar.bz2 $(oss_externals_TARBALL)

