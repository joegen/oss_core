# OSS Core Library build environment is based on the [sipXecs](http://www.sipfoundry.org) build system.

## Prerequisites (CentOS or Fedora)
-----------------------------------
  
### Install yum repo file from [ossapp](http://www.ossapp.com) repository 
    cd /etc/yum.repos.d
    CentOS:  wget http://bridge.ossapp.com/karoo/repo/1.5.0/stage/karoo-1.5.0-centos.repo
    Fedora:  wget http://bridge.ossapp.com/karoo/repo/1.5.0/stage/karoo-1.5.0-fedora.repo
    yum update
  
## Building From Source (CentOS or Fedora)
------------------------------------------
  
### Download the source code from https://github.com/joegen/oss_core and build it
    git clone https://github.com/joegen/oss_core.git
    cd oss_core/
    autoreconf -if
    mkdir -p .build
    cd .build
    ../configure --prefix=`pwd`/root
    CentOS: sudo yum install epel-release
    sudo yum install `make oss.deps-build-list`
  
## Building RPMS (CentOS or Fedora)
-----------------------------------
  
### Create RPM Build environment
    echo "%_topdir      $HOME/rpmbuild" >> ~/.rpmmacros
    mkdir -p ~/rpmbuild/{BUILD,SOURCES,RPMS,SRPMS,SPECS}
    cd oss_core/
    autoreconf -if
    mkdir -p .rpmbuild
    cd .rpmbuild
    ../configure --enable-rpm \
      --with-yum-proxy=http://localhost:3128 \
      CENTOS_BASE_URL="http://ftp.jaist.ac.jp/pub/Linux/CentOS"  \
      FEDORA_BASE_URL="http://ftp.jaist.ac.jp/pub/Linux/Fedora" \
      MIRROR_SITE="http://ftp.linux.ncsu.edu/pub" \
      FEDORA_ARCHIVE_BASE_URL=\
      "http://dl.fedoraproject.org/pub/archive/fedora/linux"
  
### Create chroots for [Mock](https://fedoraproject.org/wiki/Projects/Mock)
    sudo yum install mock squid
    sudo usermod -a -G mock $USER
    make distro.centos-6-x86_64.repo-chroot-init
    make distro.fedora-16-x86_64.repo-chroot-init
  
### Building the RPMS
    make distro.centos-6-x86_64.oss_core.rpm
    make distro.fedora-16-x86_64.oss_core.rpm

	


	

