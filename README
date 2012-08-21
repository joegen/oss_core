# OSS Core Library build environment is influenced by [sipXecs](http://www.sipfoundry.org) build setup.

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
    cd oss_core
    autoreconf -if
    mkdir -p .build
    cd .build
    ../configure --prefix=`pwd`/root
  
## Building RPMS (CentOS or Fedora)
-----------------------------------
  
### Create RPM Build environment
    echo "%_topdir      $HOME/rpmbuild" >> ~/.rpmmacros
    mkdir -p ~/rpmbuild/{BUILD,SOURCES,RPMS,SRPMS,SPECS}
    cd oss_core
    autoreconf -if
    mkdir -p .build
    cd .build
    ../configure
    make install
    rpmbuild -ta oss_core-*tar.gz
