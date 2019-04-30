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

# download and build ROOT
ARG ROOT_VERSION="6.16.00"
RUN curl -o /tmp/root.tar.gz https://root.cern/download/root_v${ROOT_VERSION}.source.tar.gz
RUN tar xzfv /tmp/root.tar.gz -C /opt && rm -rf /tmp/root.tar.gz
WORKDIR /opt/root-${ROOT_VERSION}/build

#CMD ["/bin/bash"]

RUN cmake ..
RUN make install
RUN /bin/bash /opt/${ROOT_VERSION}/install/bin/thisroot.sh

CMD ["/bin/bash"]