#
# Docker container for building OSS_CORE
#

FROM centos:centos6
MAINTAINER Joegen Baclor <joegen@ossapp.com>

RUN yum -y update; yum -y install epel-release; yum clean all; yum -y --disablerepo=epel update  ca-certificates
RUN sed -i "s/#baseurl/baseurl/" /etc/yum.repos.d/epel.repo; sed -i "s/mirrorlist/#mirrorlist/" /etc/yum.repos.d/epel.repo
RUN yum -y install automake libtool gcc-c++ git tar rpm-build createrepo openssl-devel libmcrypt-devel libtool-ltdl-devel pcre-devel findutils db4-devel iptables iproute boost-devel v8-devel libsrtp-devel libpcap-devel libdnet-devel xmlrpc-c-devel libevent-devel poco-devel libconfig-devel hiredis-devel gtest-devel leveldb-devel; yum clean all


