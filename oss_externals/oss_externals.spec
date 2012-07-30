%define __arch_install_post /usr/lib/rpm/check-buildroot
%define _prefix   /usr/local/lib/oss-externals
%define prefix    %{_prefix}

Name:         	oss_externals
Summary:      	OSS Support Libraries for Karoo Bridge
License:      	MPL
Group:        	Productivity/Telephony/Servers
Version:      	1.0.2
Release:      	3
URL:          	http://www.ossapp.com
Packager:     	OSS Software Solutions
Vendor:       	http://www.ossapp.com


Source0:    %{name}-%{version}.tar.bz2


Prefix:     %{prefix}
BuildRoot:    %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:	texinfo
BuildRequires:  python-devel
BuildRequires:  bzip2-devel
BuildRequires:  zlib-devel
BuildRequires:	libpcap-devel
BuildRequires:  openssl-devel >= 0.9.7
BuildRequires:  db4-devel
BuildRequires: popt
BuildRequires: scons
BuildRequires: expat-devel
BuildRequires: flex
BuildRequires: bison

%if %{?rhel}%{!?rhel:0} >= 6
BuildRequires: libuuid-devel
Requires:      libuuid
BuildRequires: popt
Requires:      popt
%elseif %{?rhel}%{!?rhel:0} >= 5
BuildRequires: e2fsprogs-devel
Requires:      e2fsprogs
%elseif 0%{?fedora} >= 4
BuildRequires: uuid-devel
Requires:      uuid
%else
BuildRequires: uuid-devel
Requires:      uuid
%endif

Requires: openssl >= 0.9.7
Requires: chkconfig
Requires:	libpcap

BuildRequires: ncurses-devel
Requires:      ncurses

%description
OSS Support Libraries for Karoo Bridge

%package devel
Summary:        Development package for Karoo Bridge SBC platform
Group:          System/Libraries
Requires:       %{name} = %{version}-%{release}
Requires:  automake
Requires:  autoconf
Requires:  gcc-c++
Requires:  git
Requires:  mock
Requires:  createrepo
Requires:  enscript
Requires:  thttpd
Requires:  rpm-build
Requires:  squid
Requires:  wget
Requires:  which
Requires:  flex
Requires:  bison
Requires:  libtool
Requires:  libmcrypt
Requires:  scons
Requires:  libtiff
Requires:  texinfo
Requires:  gnutls-devel
Requires:  ncurses-devel
Requires:  unixODBC-devel
Requires:  openldap-devel
Requires:  curl-devel
Requires:  pcre-devel
Requires:  libmcrypt-devel
Requires:  openssl-devel
Requires:  libtool-ltdl-devel
Requires:  gdbm-devel
Requires:  db4-devel
Requires:  python-devel
Requires:  libogg-devel
Requires:  libvorbis-devel
Requires:  libjpeg-devel
Requires:  alsa-lib-devel
Requires:  zlib-devel
Requires:  e2fsprogs-devel
Requires:  libtheora-devel
Requires:  libxml2-devel
Requires:  bzip2-devel
Requires:  libpcap-devel
Requires:  libX11-devel

%description devel
OSS Support Library Headers for Karoo Bridge

%prep
%setup -b0 -q


%build
#
# Compile libdialog
#
cd dialog
./configure \
		--prefix=%{_prefix} \
		--enable-header-subdir \
		--enable-nls \
		--enable-widec \
		--disable-rpath-hack
make
cd ..

#
# Compile boost
#
cd boost
./bootstrap.sh --prefix=%{buildroot}%{prefix}
cd ..

#
# Compile google v8
#
cd v8
%ifarch i386
scons mode=release library=shared snapshot=off
%endif
%ifarch i686
scons mode=release library=shared snapshot=off
%endif
%ifarch x86_64
scons mode=release library=shared snapshot=off arch=x64
%endif
cd ..


#
# Compile poco
#
cd poco
./configure --no-samples --no-tests --prefix=%{buildroot}%{prefix}
make
cd ..


#
# Compile libcli
#
make -C libcli

#
# Compile libconfig
#
cd libconfig
./configure --prefix=%{buildroot}%{prefix}
make
cd ..

#
# Compile GTEST
#
#cd gtest
#./configure --prefix=%{buildroot}%{prefix}
#make
#cd ..


#
# Compile ucarp
#
# Note:  Do not use --prefix=%{buildroot}%{prefix} because ucarp embeds it in the binary resulting to errors in rpmbuild
#
cd ucarp
./configure --prefix=/usr/local/lib/oss-externals
make
cd ..

#
# Compile redis
#
#
make -C redis-2.4.2
make -C redis-2.4.2/hiredis



%install
mkdir -p %{buildroot}%{prefix}/lib/
mkdir -p %{buildroot}%{prefix}/sbin/
mkdir -p %{buildroot}%{prefix}/include/


make -C dialog install-full DESTDIR=%{buildroot}
make -C poco install
make -C libcli install PREFIX=%{prefix} DESTDIR=%{buildroot}
make -C libconfig install
#
# Cleanup libconfig
#
rm -rf %{buildroot}%{prefix}/lib/pkgconfig
cp %{buildroot}%{prefix}/lib/liboss_config.so.8.1.2 %{buildroot}%{prefix}/lib/tmp.liboss_config.so
cp %{buildroot}%{prefix}/lib/liboss_config++.so.8.1.2 %{buildroot}%{prefix}/lib/tmp.liboss_config++.so
rm -f %{buildroot}%{prefix}/lib/liboss_config*
mv %{buildroot}%{prefix}/lib/tmp.liboss_config.so %{buildroot}%{prefix}/lib/liboss_config.so
mv %{buildroot}%{prefix}/lib/tmp.liboss_config++.so %{buildroot}%{prefix}/lib/liboss_config++.so

#
# Install boost
#
cd boost
./b2 install
cd ..

#make -C gtest install
#
# Cleanup libgtest
#
#cp %{buildroot}%{prefix}/lib/libgtest.a %{buildroot}%{prefix}/lib/tmp.libgtest.a
#rm -f %{buildroot}%{prefix}/lib/libgtest*
#rm -rf /%{buildroot}%{prefix}/bin
#mv %{buildroot}%{prefix}/lib/tmp.libgtest.a %{buildroot}%{prefix}/lib/libgtest.a

#
# Install google v8
#
cp v8/libv8.so %{buildroot}%{prefix}/lib/liboss_v8.so
cp v8/include/*.h %{buildroot}%{prefix}/include

#
# Install ucarp
#
cp ucarp/src/ucarp %{buildroot}%{prefix}/sbin/ucarp

#
# Install redis
#
#
DESTDIR=%{buildroot} make -C redis-2.4.2 install
DESTDIR=%{buildroot} make -C redis-2.4.2/hiredis install



%pre
%ifos linux
if ! /usr/bin/id karoo &>/dev/null; then
       /usr/sbin/useradd -r -g daemon -s /bin/false -c "Karoo Bridge Session Border Controller Platform" -d /opt/karoo karoo || \
                %logmsg "Unexpected error adding user \"karoo\". Aborting installation."
fi
%endif

%post
chmod +x %{prefix}/sbin/*
chmod +x %{prefix}/bin/*
ln -snf %{prefix}/sbin/ucarp  %{prefix}/sbin/x-ucarp
ln -snf %{prefix}/sbin/repro  %{prefix}/sbin/x-repro
ln -snf %{prefix}/bin/redis-server %{prefix}/bin/x-redis-server
EXTERNALS_PATH=`grep oss-externals /etc/ld.so.conf`
if [ -z "${EXTERNALS_PATH:-}" ]; then
	echo /usr/local/lib/oss-externals/lib >> /etc/ld.so.conf
	ldconfig
fi

%post devel
usermod -a -G mock karoo
chkconfig squid on
service squid start


%files
%defattr(0644,karoo,daemon,0755)
%{prefix}/lib
%{prefix}/sbin
%{prefix}/bin
%{prefix}/share



%files devel
%defattr(0644,karoo,daemon,0755)
%{prefix}/include






