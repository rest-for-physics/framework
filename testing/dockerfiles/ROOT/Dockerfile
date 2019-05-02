#
# author: Luis Obis (lobis@unizar.es)
#

FROM ubuntu:18.04

RUN apt-get update
# root dependencies
RUN apt-get install -y git dpkg-dev cmake g++ gcc binutils libx11-dev libxpm-dev libxft-dev libxext-dev
RUN apt-get install -y gfortran libssl-dev libpcre3-dev \
    xlibmesa-glu-dev libglew1.5-dev libftgl-dev \
    libmysqlclient-dev libfftw3-dev libcfitsio-dev \
    graphviz-dev libavahi-compat-libdnssd-dev \
    libldap2-dev python-dev libxml2-dev libkrb5-dev \
    libgsl0-dev libqt4-dev
RUN apt-get install -y curl

# download and build ROOT with provided options
ARG ROOT_VERSION="6.16.00"
RUN curl -o /tmp/root.tar.gz https://root.cern/download/root_v${ROOT_VERSION}.source.tar.gz
RUN tar xzfv /tmp/root.tar.gz -C /opt && rm -rf /tmp/root.tar.gz
WORKDIR /opt/root-${ROOT_VERSION}/build

ARG ROOT_INSTALL_PREFIX=/opt/root
ARG MAKE_N_JOBS=""
# $((`nproc` + 1)) = number of cores + 1 for the fastest make
# MAKE_N_JOBS can be passed as argument in --build-arg
RUN if [ -z "$MAKE_N_JOBS" ]; then MAKE_N_JOBS=$((`nproc` + 1)); fi
RUN cmake -DCMAKE_INSTALL_PREFIX=$ROOT_INSTALL_PREFIX -DCXX17=ON -DGDML=ON ..
RUN make install -j$MAKE_N_JOBS

ENV ROOTSYS "${ROOT_INSTALL_PREFIX}"
ENV PATH "${ROOTSYS}/bin:${PATH}"
ENV LD_LIBRARY_PATH "${ROOTSYS}/lib:${LD_LIBRARY_PATH}"

CMD ["/bin/bash"]
