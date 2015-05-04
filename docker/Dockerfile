#
# Docker container for building OSS_CORE
#

FROM centos:centos6
MAINTAINER Joegen Baclor <joegen@ossapp.com>

RUN yum -y update
RUN yum -y install epel-release; yum clean all
RUN yum -y --disablerepo=epel update  ca-certificates
RUN sed -i "s/#baseurl/baseurl/" /etc/yum.repos.d/epel.repo
RUN sed -i "s/mirrorlist/#mirrorlist/" /etc/yum.repos.d/epel.repo
RUN cat /etc/yum.repos.d/epel.repo
RUN yum -y install automake libtool gcc-c++ git tar openssl-devel libmcrypt-devel libtool-ltdl-devel pcre-devel findutils db4-devel iptables iproute boost-devel v8-devel libsrtp-devel libpcap-devel libdnet-devel xmlrpc-c-devel libevent-devel poco-devel libconfig-devel hiredis-devel gtest-devel leveldb-devel; yum clean all


