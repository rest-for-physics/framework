#!/bin/bash

set -e
WP=$PWD

G4VER=geant4_10_02_b01
t=$WP/external/$G4VER
mkdir -p $t


mkdir build -p
cd build
f=geant4_10_02_b01.zip
if [ -f $f ]; then
    echo "use build/$f"
else
    wget http://geant4.cern.ch/support/source/geant4_10_02_b01.zip
fi

unzip -q $f
mkdir $G4VER-build -p
cd $G4VER-build
    cmake ../$G4VER -DGEANT4_USE_GDML=ON  -DGEANT4_USE_QT=ON -DCMAKE_INSTALL_PREFIX=$t -DGEANT4_INSTALL_DATA=ON -DGEANT4_INSTALL_DATA_TIMEOUT=7200
    make -j4
    make install
cd $WP
