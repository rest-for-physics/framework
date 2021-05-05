FROM rootproject/root
LABEL maintainer=luis.antonio.obis@gmail.com

ARG APPS_DIR=/opt

ENV GEANT4_VERSION=v10.4.3
ARG GEANT4_DIR=$APPS_DIR/geant4

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y git apt-utils software-properties-common dpkg-dev cmake binutils openssl libssl-dev \
    libexpat-dev libxerces-c-dev libmpc-dev && \
    apt-get autoremove -y && \
    apt-get clean -y && \
    rm -rf /var/cache/apt/archives/* && \
    rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/Geant4/geant4.git --branch=${GEANT4_VERSION} --depth 1 $GEANT4_DIR/source_$GEANT4_VERSION && \
    mkdir -p $GEANT4_DIR/build &&  cd $GEANT4_DIR/build && \
    cmake ../source_$GEANT4_VERSION/ -DCMAKE_INSTALL_PREFIX=$GEANT4_DIR/install \
    -DGEANT4_BUILD_MULTITHREADED=OFF -DGEANT4_USE_QT=OFF -DGEANT4_USE_OPENGL_X11=OFF -DGEANT4_USE_RAYTRACER_X11=OFF \
    -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_GDML=ON && \
    make -j$(nproc) install && \
    rm -rf $GEANT4_DIR/build && \
    echo "source $GEANT4_DIR/install/bin/geant4.sh" >> ~/.bashrc

ENV GARFIELDPP_VERSION=3.0
ARG GARFIELDPP_DIR=$APPS_DIR/garfieldpp

RUN git clone https://gitlab.cern.ch/garfield/garfieldpp.git --branch=${GARFIELDPP_VERSION} --depth 1 $GARFIELDPP_DIR/source_$GARFIELDPP_VERSION && \
    mkdir -p $GARFIELDPP_DIR/build &&  cd $GARFIELDPP_DIR/build && \
    cmake ../source_$GARFIELDPP_VERSION/ -DCMAKE_INSTALL_PREFIX=$GARFIELDPP_DIR/install \
    -DWITH_DOCS=OFF -DWITH_EXAMPLES=OFF && \
    make -j$(nproc) install && \
    rm -rf $GARFIELDPP_DIR/build && \
    echo "export GARFIELD_HOME=$GARFIELDPP_DIR/install" >> ~/.bashrc

WORKDIR /

CMD ["/bin/bash"]
