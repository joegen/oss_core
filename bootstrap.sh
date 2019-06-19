#!/bin/sh

BASEDIR=`pwd`;
LIBDIR=${BASEDIR}/libs;
SUBDIRS="resiprocate leak_tracer";

https://github.com/fredericgermain/LeakTracer.git

bootstrap_leak_tracer() {
  cd ${LIBDIR}
  if [ ! -f ./LeakTracer/libleaktracer/include/leaktracer.h ]; then
    git clone https://github.com/fredericgermain/LeakTracer.git
  fi
}

bootstrap_resiprocate() {
  cd ${LIBDIR}
  if [ ! -f ./resiprocate/resip/dum/Dialog.hxx ]; then
    git clone https://github.com/joegen/resiprocate
    cd resiprocate
    autoreconf -if
  fi
}

bootstrap_libs() {
  for i in ${SUBDIRS}; do
    bootstrap_$i
  done
  ${BGJOB} && wait
}

bootstrap_libs